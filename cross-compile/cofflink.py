#!/usr/bin/env python3
"""
COFF linker for Motorola 68010 (Plexus P/20).
Links COFF .o files with Plexus libc.a to produce an executable.

Usage: python3 cofflink.py -o output crt0.o obj1.o obj2.o -L/lib -lc -lcurses
"""

import struct
import sys
import os

MC68MAGIC = 0x0150
F_RELFLG  = 0x0001
F_EXEC    = 0x0002
F_LNNO    = 0x0004
F_LSYMS   = 0x0008
F_AR32WR  = 0x0100
F_AR32W   = 0x0200

STYP_TEXT = 0x0020
STYP_DATA = 0x0040
STYP_BSS  = 0x0080

C_EXT  = 2
C_STAT = 3
C_FILE = 103

R_RELLONG = 0x0011
R_PCRLONG = 0x0014
R_RELWORD = 0x0010
R_PCRWORD = 0x0013

class CoffObj:
    """Parsed COFF object file."""
    def __init__(self, name, data):
        self.name = name
        self.data = data
        self.magic, self.nscns, self.timdat, self.symptr, self.nsyms, \
            self.opthdr, self.flags = struct.unpack('>HHiiiHH', data[:20])
        if self.magic != MC68MAGIC:
            raise ValueError(f"{name}: not MC68MAGIC (got 0x{self.magic:04x})")

        self.sections = []
        off = 20 + self.opthdr
        for i in range(self.nscns):
            s = data[off:off+40]
            sec = {
                'name': s[:8].rstrip(b'\x00').decode(),
                'paddr': struct.unpack('>i', s[8:12])[0],
                'vaddr': struct.unpack('>i', s[12:16])[0],
                'size': struct.unpack('>i', s[16:20])[0],
                'scnptr': struct.unpack('>i', s[20:24])[0],
                'relptr': struct.unpack('>i', s[24:28])[0],
                'lnnoptr': struct.unpack('>i', s[28:32])[0],
                'nreloc': struct.unpack('>H', s[32:34])[0],
                'nlnno': struct.unpack('>H', s[34:36])[0],
                'flags': struct.unpack('>i', s[36:40])[0],
            }
            if sec['scnptr'] and sec['size']:
                sec['data'] = bytearray(data[sec['scnptr']:sec['scnptr']+sec['size']])
            else:
                sec['data'] = bytearray(sec['size'])

            # Parse relocations
            sec['relocs'] = []
            if sec['nreloc'] and sec['relptr']:
                for j in range(sec['nreloc']):
                    roff = sec['relptr'] + j * 10
                    rvaddr, rsymndx, rtype = struct.unpack('>iiH', data[roff:roff+10])
                    sec['relocs'].append((rvaddr, rsymndx, rtype))

            self.sections.append(sec)
            off += 40

        # Parse symbols
        self.symbols = []
        strtab_start = self.symptr + self.nsyms * 18
        i = 0
        while i < self.nsyms:
            sym = data[self.symptr+i*18:self.symptr+i*18+18]
            if len(sym) < 18:
                break
            nb = sym[:8]
            value, scnum, stype, sclass, numaux = struct.unpack('>iHHbB', sym[8:18])
            if nb[:4] == b'\x00\x00\x00\x00':
                so = struct.unpack('>i', nb[4:8])[0]
                if strtab_start + so < len(data):
                    end = data.index(b'\x00', strtab_start + so)
                    name = data[strtab_start+so:end].decode()
                else:
                    name = f'<bad:{so}>'
            else:
                name = nb.rstrip(b'\x00').decode()
            self.symbols.append({
                'name': name, 'value': value, 'scnum': scnum,
                'stype': stype, 'sclass': sclass, 'numaux': numaux,
                'index': i
            })
            i += 1 + numaux

    def get_section(self, name):
        for s in self.sections:
            if s['name'] == name:
                return s
        return None


def parse_archive(path):
    """Parse a COFF .a archive, return list of (name, CoffObj)."""
    with open(path, 'rb') as f:
        data = f.read()
    if data[:8] != b'!<arch>\n':
        raise ValueError(f"{path}: not an ar archive")

    members = []
    off = 8
    while off < len(data):
        hdr = data[off:off+60]
        name = hdr[:16].rstrip().rstrip(b'/').decode()
        size = int(hdr[48:58].strip())
        mdata = data[off+60:off+60+size]
        if len(mdata) >= 2:
            magic = struct.unpack('>H', mdata[:2])[0]
            if magic == MC68MAGIC:
                try:
                    obj = CoffObj(name, mdata)
                    members.append((name, obj))
                except:
                    pass
        off += 60 + size
        if off % 2:
            off += 1
    return members


def link(object_files, libraries, output_path, text_start=0x0):
    """Link COFF objects and libraries into an executable."""

    # Phase 1: Collect all input objects
    # Detect native (Plexus cc) vs cross-compiled (elf2coff) objects.
    # Native objects: compiled by Plexus cc, pre-relocation values include paddr.
    # Cross-compiled: from elf2coff, values are section-relative (no paddr).
    # Heuristic: if the .data section's first relocation references a symbol
    # whose value < paddr, it's cross-compiled (value is section-relative).
    # Simpler: crt0.o is from the Plexus system (native). Everything else
    # that's an input .o is cross-compiled. Library objects are native.
    objects = []
    for path in object_files:
        with open(path, 'rb') as f:
            data = f.read()
        obj = CoffObj(os.path.basename(path), data)
        # Mark as native if it's crt0.o (from Plexus)
        obj.native_coff = 'crt0' in path
        objects.append(obj)

    # Parse library archives
    lib_archives = []
    for path in libraries:
        lib_archives.append(parse_archive(path))

    # Phase 2: Collect all defined and undefined symbols
    defined = {}   # name -> (obj_index, section, value)
    undefined = set()
    common_syms = {}  # name -> size (for COMMON symbols: scnum==0, value>0)

    for idx, obj in enumerate(objects):
        for sym in obj.symbols:
            if sym['sclass'] == C_EXT:
                if sym['scnum'] == 0:
                    if sym['value'] > 0:
                        # Common symbol — allocate in BSS
                        if sym['name'] not in defined:
                            old = common_syms.get(sym['name'], 0)
                            common_syms[sym['name']] = max(old, sym['value'])
                    elif sym['name'] not in defined and sym['name'] not in common_syms:
                        undefined.add(sym['name'])
                else:
                    defined[sym['name']] = (idx, sym['scnum'], sym['value'])
                    undefined.discard(sym['name'])
                    common_syms.pop(sym['name'], None)

    # Phase 3: Resolve undefined symbols from libraries
    changed = True
    while changed and undefined:
        changed = False
        for lib in lib_archives:
            for mname, obj in lib:
                # Check if this member defines any undefined symbol
                provides = set()
                for sym in obj.symbols:
                    if sym['sclass'] == C_EXT and (sym['scnum'] != 0 or sym['value'] > 0):
                        provides.add(sym['name'])

                if provides & undefined:
                    # Library members are native Plexus COFF
                    obj.native_coff = True
                    idx = len(objects)
                    objects.append(obj)
                    for sym in obj.symbols:
                        if sym['sclass'] == C_EXT:
                            if sym['scnum'] != 0:
                                defined[sym['name']] = (idx, sym['scnum'], sym['value'])
                                undefined.discard(sym['name'])
                                common_syms.pop(sym['name'], None)
                            elif sym['value'] > 0:
                                # Common symbol from library
                                if sym['name'] not in defined:
                                    old = common_syms.get(sym['name'], 0)
                                    common_syms[sym['name']] = max(old, sym['value'])
                                    undefined.discard(sym['name'])
                            elif sym['name'] not in defined and sym['name'] not in common_syms:
                                undefined.add(sym['name'])
                    changed = True

    # Auto-define common undefined symbols
    # _end, _etext, _edata are linker-provided (resolved after layout)
    # _errno, __sibuf, __smbuf, __sobuf, __sigfunc are BSS globals
    auto_bss = {}
    linker_syms = {'_end', '_etext', '_edata'}
    common_bss = {
        '_errno': 4, '__sibuf': 1024, '__smbuf': 1024, '__sobuf': 1024,
        '__sigfunc': 128, '_COLS': 4, '_LINES': 4, '_SP': 4,
        '_curscr': 4, '_stdscr': 4, '_lwin': 4, '_ttytype': 20,
        '__first_term': 4, '_ttytype': 20,
    }
    for sym_name in list(undefined):
        if sym_name in linker_syms:
            undefined.discard(sym_name)
        elif sym_name in common_bss:
            auto_bss[sym_name] = common_bss[sym_name]
            undefined.discard(sym_name)

    if undefined:
        print(f"Warning: {len(undefined)} undefined symbols:", file=sys.stderr)
        for s in sorted(undefined):
            print(f"  {s}", file=sys.stderr)

    # Phase 4: Layout sections
    # Merge all .text sections, then .data, then .bss
    text_parts = []  # (obj_idx, section_data, base_offset)
    data_parts = []
    bss_parts = []

    # Track per-object section offsets
    obj_text_base = {}
    obj_data_base = {}
    obj_bss_base = {}

    text_offset = 0
    data_offset = 0
    bss_offset = 0

    for idx, obj in enumerate(objects):
        for sec in obj.sections:
            if sec['flags'] & STYP_TEXT:
                # Align to 2 bytes
                while text_offset % 2:
                    text_offset += 1
                obj_text_base[idx] = text_offset
                text_parts.append((idx, sec, text_offset))
                text_offset += sec['size']
            elif sec['flags'] & STYP_DATA:
                while data_offset % 2:
                    data_offset += 1
                obj_data_base[idx] = data_offset
                data_parts.append((idx, sec, data_offset))
                data_offset += sec['size']
            elif sec['flags'] & STYP_BSS:
                while bss_offset % 2:
                    bss_offset += 1
                obj_bss_base[idx] = bss_offset
                bss_parts.append((idx, sec, bss_offset))
                bss_offset += sec['size']

    # Pad text to exact page boundary so there's no gap between text and data pages.
    # The Plexus SVR2 kernel doesn't map data pages correctly if there's a gap.
    page_size = 0x1000
    padded_text = (text_offset + page_size - 1) & ~(page_size - 1)
    total_text = padded_text
    data_start = text_start + total_text
    total_data = data_offset
    bss_start_addr = data_start + total_data
    total_bss = bss_offset

    # Build merged section data
    merged_text = bytearray(total_text)
    merged_data = bytearray(total_data)

    for idx, sec, base in text_parts:
        merged_text[base:base+sec['size']] = sec['data']
    for idx, sec, base in data_parts:
        merged_data[base:base+sec['size']] = sec['data']

    # Phase 5: Resolve symbol values
    sym_values = {}  # name -> absolute address

    for name, (obj_idx, scnum, value) in defined.items():
        obj = objects[obj_idx]
        sec = obj.sections[scnum - 1]
        # ALWAYS subtract section paddr to get section-relative offset.
        # Both native and cross-compiled objects include paddr in values.
        section_offset = value - sec['paddr']
        if sec['flags'] & STYP_TEXT:
            base = text_start + obj_text_base.get(obj_idx, 0)
        elif sec['flags'] & STYP_DATA:
            base = data_start + obj_data_base.get(obj_idx, 0)
        elif sec['flags'] & STYP_BSS:
            base = bss_start_addr + obj_bss_base.get(obj_idx, 0)
        else:
            base = 0
        sym_values[name] = base + section_offset

    # Allocate common symbols in BSS
    for sym_name, sym_size in common_syms.items():
        if sym_name not in sym_values:
            while total_bss % 4:
                total_bss += 1
            sym_values[sym_name] = bss_start_addr + total_bss
            total_bss += sym_size

    # Allocate auto BSS symbols
    for sym_name, sym_size in auto_bss.items():
        while total_bss % 4:
            total_bss += 1
        sym_values[sym_name] = bss_start_addr + total_bss
        total_bss += sym_size

    # Add linker-provided symbols
    sym_values['_etext'] = text_start + total_text
    sym_values['_edata'] = data_start + total_data
    sym_values['_end'] = bss_start_addr + total_bss

    # Phase 6: Apply relocations
    for idx, obj in enumerate(objects):
        for sec in obj.sections:
            if sec['flags'] & STYP_TEXT:
                sec_base = text_start + obj_text_base.get(idx, 0)
                target = merged_text
                target_base = obj_text_base.get(idx, 0)
            elif sec['flags'] & STYP_DATA:
                sec_base = data_start + obj_data_base.get(idx, 0)
                target = merged_data
                target_base = obj_data_base.get(idx, 0)
            else:
                continue

            for rvaddr, rsymndx, rtype in sec['relocs']:
                # Find the symbol
                sym = None
                for s in obj.symbols:
                    if s['index'] == rsymndx:
                        sym = s
                        break
                if sym is None:
                    continue

                # For native COFF objects (Plexus cc), relocation vaddr includes
                # the section paddr. Subtract it to get section-relative offset.
                # For cross-compiled objects (elf2coff), vaddr is already
                # section-relative, so don't subtract.
                if obj.native_coff:
                    rvaddr_adj = rvaddr - sec['paddr']
                else:
                    rvaddr_adj = rvaddr

                # Get symbol value
                if sym['sclass'] == C_EXT:
                    if sym['name'] in sym_values:
                        sym_val = sym_values[sym['name']]
                    elif sym['name'] in undefined:
                        sym_val = 0  # unresolved
                    else:
                        sym_val = 0
                elif sym['sclass'] == C_STAT:
                    # Section symbol resolution.
                    # Native COFF (.o from Plexus cc): pre-relocation values
                    #   include the section paddr, so subtract it.
                    # Cross-compiled COFF (.o from elf2coff): pre-relocation
                    #   values DON'T include paddr (RELA addends were applied
                    #   relative to section start), so DON'T subtract.
                    # Detect: native objects have .text paddr=0 and .data paddr>0
                    #   where paddr = section VMA within the object.
                    #   Cross-compiled have the same, but the pre-reloc values
                    #   are different. We use a heuristic: if .text paddr is 0
                    #   (always true) and .data paddr == .text size, it's likely
                    #   cross-compiled. Native cc sets paddr = cumulative section start.
                    # Simplest correct approach: DON'T subtract paddr at all.
                    # Instead, rely on the pre-relocation values being correct
                    # as-is (they include whatever offsets the compiler embedded).
                    ssec = obj.sections[sym['scnum'] - 1] if sym['scnum'] > 0 else None
                    # ALWAYS subtract paddr — both native and cross-compiled
                    # objects include paddr in section symbol values and in
                    # pre-relocation data values.
                    paddr = ssec['paddr'] if ssec else 0
                    if ssec and ssec['flags'] & STYP_TEXT:
                        sym_val = text_start + obj_text_base.get(idx, 0) - paddr
                    elif ssec and ssec['flags'] & STYP_DATA:
                        sym_val = data_start + obj_data_base.get(idx, 0) - paddr
                    elif ssec and ssec['flags'] & STYP_BSS:
                        sym_val = bss_start_addr + obj_bss_base.get(idx, 0) - paddr
                    else:
                        sym_val = sym['value']
                else:
                    sym_val = sym['value']

                # Apply relocation using section-relative offset
                abs_off = target_base + rvaddr_adj
                if abs_off < 0 or abs_off + 4 > len(target):
                    continue  # skip out-of-bounds relocations
                if rtype == R_RELLONG:
                    old = struct.unpack('>i', target[abs_off:abs_off+4])[0]
                    struct.pack_into('>i', target, abs_off, old + sym_val)
                elif rtype == R_PCRLONG:
                    old = struct.unpack('>i', target[abs_off:abs_off+4])[0]
                    pc = sec_base + rvaddr_adj
                    struct.pack_into('>i', target, abs_off, old + sym_val - pc)
                elif rtype == R_RELWORD:
                    old = struct.unpack('>h', target[abs_off:abs_off+2])[0]
                    struct.pack_into('>h', target, abs_off, old + sym_val)
                elif rtype == R_PCRWORD:
                    old = struct.unpack('>h', target[abs_off:abs_off+2])[0]
                    pc = sec_base + rvaddr_adj
                    struct.pack_into('>h', target, abs_off, old + sym_val - pc)

    # Phase 7: Write output COFF executable
    nscns = 3
    opthdr_size = 28
    headers_size = 20 + opthdr_size + nscns * 40

    entry = sym_values.get('start', sym_values.get('_start', text_start))

    # File header - match native cc flags exactly
    # Native uses F_RELFLG | F_EXEC | F_AR32W = 0x0203
    file_hdr = struct.pack('>HHiiiHH',
        MC68MAGIC, nscns, 0, 0, 0, opthdr_size,
        F_RELFLG | F_EXEC | F_AR32W)

    # Optional (a.out) header
    opt_hdr = struct.pack('>HHiiiiii',
        0o410,  # NMAGIC - pure text
        0,      # vstamp - match native
        total_text,
        total_data,
        total_bss,
        entry,
        text_start,
        data_start)

    # Section headers
    def mksechdr(name, paddr, vaddr, size, scnptr, flags):
        return name.encode().ljust(8, b'\x00') + struct.pack('>iiiiiiHHi',
            paddr, vaddr, size, scnptr, 0, 0, 0, 0, flags)

    text_hdr = mksechdr('.text', text_start, text_start, total_text, headers_size, STYP_TEXT)
    data_hdr = mksechdr('.data', data_start, data_start, total_data, headers_size + total_text, STYP_DATA)
    bss_hdr = mksechdr('.bss', bss_start_addr, bss_start_addr, total_bss, 0, STYP_BSS)

    with open(output_path, 'wb') as f:
        f.write(file_hdr)
        f.write(opt_hdr)
        f.write(text_hdr)
        f.write(data_hdr)
        f.write(bss_hdr)
        f.write(bytes(merged_text))
        f.write(bytes(merged_data))

    outsize = os.path.getsize(output_path)
    print(f"Linked {output_path}: {outsize} bytes")
    print(f"  .text: {total_text} bytes at 0x{text_start:x}")
    print(f"  .data: {total_data} bytes at 0x{data_start:x}")
    print(f"  .bss:  {total_bss} bytes at 0x{bss_start_addr:x}")
    print(f"  entry: 0x{entry:x}")
    print(f"  objects: {len(objects)} ({len(object_files)} input + {len(objects)-len(object_files)} from libs)")
    if undefined:
        print(f"  UNRESOLVED: {len(undefined)} symbols")
    return len(undefined)


if __name__ == '__main__':
    args = sys.argv[1:]
    output = 'a.out'
    obj_files = []
    lib_paths = ['/tmp/plexus_root/root/lib']
    lib_names = []

    i = 0
    while i < len(args):
        if args[i] == '-o' and i+1 < len(args):
            output = args[i+1]
            i += 2
        elif args[i].startswith('-L'):
            lib_paths.append(args[i][2:])
            i += 1
        elif args[i].startswith('-l'):
            lib_names.append(args[i][2:])
            i += 1
        elif args[i].endswith('.o'):
            obj_files.append(args[i])
            i += 1
        else:
            print(f"Unknown arg: {args[i]}", file=sys.stderr)
            i += 1

    # Resolve library paths
    libraries = []
    for lname in lib_names:
        found = False
        for lpath in lib_paths:
            candidate = os.path.join(lpath, f'lib{lname}.a')
            if os.path.exists(candidate):
                libraries.append(candidate)
                found = True
                break
        if not found:
            print(f"Warning: library -l{lname} not found", file=sys.stderr)

    n_undef = link(obj_files, libraries, output)
    sys.exit(1 if n_undef else 0)
