#!/usr/bin/env python3
"""
ELF-to-COFF object file converter for Motorola 68010 (Plexus P/20).
Converts m68k-elf-gcc output to AT&T UNIX System V COFF format.

Usage: python3 elf2coff.py input.o output.o
       python3 elf2coff.py --link -o output crt0.o obj1.o obj2.o ... -L/path -lc
"""

import struct
import sys
import os

# ---- COFF Constants ----
MC68MAGIC = 0x0150
F_RELFLG  = 0x0001  # no relocations
F_EXEC    = 0x0002  # executable
F_LNNO    = 0x0004  # no line numbers
F_LSYMS   = 0x0008  # no local symbols
F_AR32W   = 0x0200  # 32-bit big-endian

# COFF section flags
STYP_TEXT = 0x0020
STYP_DATA = 0x0040
STYP_BSS  = 0x0080

# COFF symbol storage classes
C_EFCN    = -1
C_NULL    = 0
C_AUTO    = 1
C_EXT     = 2    # external symbol
C_STAT    = 3    # static
C_REG     = 4
C_LABEL   = 6
C_FILE    = 103
C_HIDDEN  = 106

# COFF relocation types (m68k)
R_RELBYTE  = 0x000F
R_RELWORD  = 0x0010
R_RELLONG  = 0x0011
R_PCRBYTE  = 0x0012
R_PCRWORD  = 0x0013
R_PCRLONG  = 0x0014

# ELF constants
ELF_MAGIC = b'\x7fELF'
EM_68K = 4
SHT_SYMTAB = 2
SHT_STRTAB = 3
SHT_RELA = 4
SHT_REL = 9
STB_LOCAL = 0
STB_GLOBAL = 1
STT_SECTION = 3
STT_FILE = 4
SHN_UNDEF = 0
SHN_ABS = 0xFFF1
SHN_COMMON = 0xFFF2

# ELF m68k relocation types
R_68K_NONE = 0
R_68K_32 = 1
R_68K_16 = 2
R_68K_8 = 3
R_68K_PC32 = 4
R_68K_PC16 = 5
R_68K_PC8 = 6

def read_elf(path):
    """Parse an ELF object file, return sections/symbols/relocations."""
    with open(path, 'rb') as f:
        data = f.read()

    if data[:4] != ELF_MAGIC:
        raise ValueError(f"Not an ELF file: {path}")

    ei_class = data[4]  # 1=32bit
    ei_data = data[5]   # 2=big-endian
    if ei_class != 1 or ei_data != 2:
        raise ValueError("Expected 32-bit big-endian ELF")

    e_type = struct.unpack('>H', data[16:18])[0]
    e_machine = struct.unpack('>H', data[18:20])[0]
    if e_machine != EM_68K:
        raise ValueError(f"Expected m68k ELF, got machine={e_machine}")

    e_shoff = struct.unpack('>I', data[32:36])[0]
    e_shentsize = struct.unpack('>H', data[46:48])[0]
    e_shnum = struct.unpack('>H', data[48:50])[0]
    e_shstrndx = struct.unpack('>H', data[50:52])[0]

    # Parse section headers
    sections = []
    for i in range(e_shnum):
        off = e_shoff + i * e_shentsize
        sh = data[off:off+e_shentsize]
        sh_name = struct.unpack('>I', sh[0:4])[0]
        sh_type = struct.unpack('>I', sh[4:8])[0]
        sh_flags = struct.unpack('>I', sh[8:12])[0]
        sh_addr = struct.unpack('>I', sh[12:16])[0]
        sh_offset = struct.unpack('>I', sh[16:20])[0]
        sh_size = struct.unpack('>I', sh[20:24])[0]
        sh_link = struct.unpack('>I', sh[24:28])[0]
        sh_info = struct.unpack('>I', sh[28:32])[0]
        sh_addralign = struct.unpack('>I', sh[32:36])[0]
        sh_entsize = struct.unpack('>I', sh[36:40])[0]
        sections.append({
            'name_off': sh_name, 'type': sh_type, 'flags': sh_flags,
            'addr': sh_addr, 'offset': sh_offset, 'size': sh_size,
            'link': sh_link, 'info': sh_info, 'addralign': sh_addralign,
            'entsize': sh_entsize, 'data': data[sh_offset:sh_offset+sh_size] if sh_size > 0 else b''
        })

    # Get section name string table
    shstrtab = sections[e_shstrndx]['data']
    for s in sections:
        end = shstrtab.index(b'\x00', s['name_off'])
        s['name'] = shstrtab[s['name_off']:end].decode()

    # Parse symbol table
    symbols = []
    symtab_sec = None
    strtab_data = b''
    for s in sections:
        if s['type'] == SHT_SYMTAB:
            symtab_sec = s
            strtab_data = sections[s['link']]['data']
            break

    if symtab_sec:
        d = symtab_sec['data']
        for i in range(0, len(d), 16):
            st_name = struct.unpack('>I', d[i:i+4])[0]
            st_value = struct.unpack('>I', d[i+4:i+8])[0]
            st_size = struct.unpack('>I', d[i+8:i+12])[0]
            st_info = d[i+12]
            st_other = d[i+13]
            st_shndx = struct.unpack('>H', d[i+14:i+16])[0]

            if st_name:
                end = strtab_data.index(b'\x00', st_name)
                name = strtab_data[st_name:end].decode()
            else:
                name = ''

            symbols.append({
                'name': name, 'value': st_value, 'size': st_size,
                'bind': st_info >> 4, 'type': st_info & 0xf,
                'shndx': st_shndx
            })

    # Parse relocations
    relocs = {}  # keyed by target section index
    for s in sections:
        if s['type'] == SHT_RELA:
            target_sec = s['info']
            rels = []
            d = s['data']
            for i in range(0, len(d), 12):
                r_offset = struct.unpack('>I', d[i:i+4])[0]
                r_info = struct.unpack('>I', d[i+4:i+8])[0]
                r_addend = struct.unpack('>i', d[i+8:i+12])[0]
                r_sym = r_info >> 8
                r_type = r_info & 0xff
                rels.append({'offset': r_offset, 'sym': r_sym, 'type': r_type, 'addend': r_addend})
            relocs[target_sec] = rels
        elif s['type'] == SHT_REL:
            target_sec = s['info']
            rels = []
            d = s['data']
            for i in range(0, len(d), 8):
                r_offset = struct.unpack('>I', d[i:i+4])[0]
                r_info = struct.unpack('>I', d[i+4:i+8])[0]
                r_sym = r_info >> 8
                r_type = r_info & 0xff
                rels.append({'offset': r_offset, 'sym': r_sym, 'type': r_type, 'addend': 0})
            relocs[target_sec] = rels

    return sections, symbols, relocs, data


def elf_reltype_to_coff(rtype):
    """Convert ELF m68k relocation type to COFF."""
    mapping = {
        R_68K_32: R_RELLONG,
        R_68K_16: R_RELWORD,
        R_68K_8: R_RELBYTE,
        R_68K_PC32: R_PCRLONG,
        R_68K_PC16: R_PCRWORD,
        R_68K_PC8: R_PCRBYTE,
    }
    if rtype in mapping:
        return mapping[rtype]
    raise ValueError(f"Unsupported ELF relocation type: {rtype}")


def convert_elf_to_coff(elf_path, coff_path, source_name=None):
    """Convert a single ELF .o to COFF .o"""
    sections, symbols, relocs, raw = read_elf(elf_path)

    if source_name is None:
        source_name = os.path.basename(elf_path)

    # Identify ELF sections we care about
    # All .rodata* sections merge into .text (same as before)
    text_idx = data_idx = bss_idx = None
    rodata_indices = []
    text_data = b''
    data_data = b''
    bss_size = 0

    for i, s in enumerate(sections):
        if s['name'] == '.text':
            text_idx = i
            text_data = s['data']
        elif s['name'].startswith('.rodata'):
            rodata_indices.append(i)
        elif s['name'] == '.data':
            data_idx = i
            data_data = s['data']
        elif s['name'] == '.bss':
            bss_idx = i
            bss_size = s['size']

    # Merge all .rodata* sections into .text
    rodata_offsets = {}  # section_index -> offset within merged text
    for ri in rodata_indices:
        rodata_data = sections[ri]['data']
        if not rodata_data:
            rodata_offsets[ri] = len(text_data)
            continue
        # Align to 4 bytes
        while len(text_data) % 4:
            text_data += b'\x00'
        rodata_offsets[ri] = len(text_data)
        text_data = text_data + rodata_data
        # Merge relocations
        if ri in relocs:
            if text_idx not in relocs:
                relocs[text_idx] = []
            merged_relocs = list(relocs[ri])
            del relocs[ri]
            for r in merged_relocs:
                r['offset'] += rodata_offsets[ri]
                relocs[text_idx].append(r)

    # Ensure .text size is even (68010 requires even-aligned code)
    if len(text_data) % 2:
        text_data += b'\x00'

    # Build COFF symbol table
    # COFF symbols need: .file, .text, .data, .bss section symbols, then all globals
    coff_symbols = []
    coff_strtab = b'\x00\x00\x00\x00'  # string table starts with 4-byte length

    # Map from ELF symbol index to COFF symbol index
    elf_to_coff_sym = {}

    # Section base addresses (for .o files)
    text_addr = 0
    data_addr = len(text_data)
    bss_addr = data_addr + len(data_data)

    def add_coff_sym(name, value, scnum, stype, sclass, numaux=0, aux_data=b''):
        idx = len(coff_symbols)
        if len(name) <= 8:
            name_bytes = name.encode().ljust(8, b'\x00')
        else:
            # Long name: store in string table
            nonlocal coff_strtab
            str_offset = len(coff_strtab)
            coff_strtab += name.encode() + b'\x00'
            name_bytes = struct.pack('>II', 0, str_offset)

        entry = name_bytes + struct.pack('>iHHbB', value, scnum, stype, sclass, numaux)
        coff_symbols.append(entry)
        if numaux:
            coff_symbols.append(aux_data.ljust(18, b'\x00'))
        return idx

    # Add .file symbol
    add_coff_sym('.file', 0, 0xFFFE, 0, C_FILE, 1, source_name[:18].encode().ljust(18, b'\x00'))

    # Add section symbols
    text_sym = add_coff_sym('.text', 0, 1, 0, C_STAT, 1,
        struct.pack('>iHH', len(text_data), 0, 0).ljust(18, b'\x00'))
    data_sym = add_coff_sym('.data', data_addr, 2, 0, C_STAT, 1,
        struct.pack('>iHH', len(data_data), 0, 0).ljust(18, b'\x00'))
    bss_sym = add_coff_sym('.bss', bss_addr, 3, 0, C_STAT, 1,
        struct.pack('>iHH', bss_size, 0, 0).ljust(18, b'\x00'))

    # Map ELF section indices to COFF section numbers
    elf_sec_to_coff = {}
    if text_idx is not None: elf_sec_to_coff[text_idx] = 1
    for ri in rodata_indices: elf_sec_to_coff[ri] = 1  # all .rodata -> .text
    if data_idx is not None: elf_sec_to_coff[data_idx] = 2
    if bss_idx is not None: elf_sec_to_coff[bss_idx] = 3

    # Map ELF section symbols to COFF section symbol indices
    elf_sec_to_coff_sym = {}
    if text_idx is not None: elf_sec_to_coff_sym[text_idx] = text_sym
    for ri in rodata_indices: elf_sec_to_coff_sym[ri] = text_sym
    if data_idx is not None: elf_sec_to_coff_sym[data_idx] = data_sym
    if bss_idx is not None: elf_sec_to_coff_sym[bss_idx] = bss_sym

    # Add regular symbols
    for i, sym in enumerate(symbols):
        if sym['type'] == STT_FILE or sym['type'] == STT_SECTION:
            # Map section symbols to our COFF section symbols
            if sym['shndx'] in elf_sec_to_coff_sym:
                elf_to_coff_sym[i] = elf_sec_to_coff_sym[sym['shndx']]
            continue
        if not sym['name']:
            continue

        # COFF uses _ prefix for C symbols
        coff_name = '_' + sym['name'] if not sym['name'].startswith('.') else sym['name']

        if sym['shndx'] == SHN_UNDEF:
            # External undefined
            coff_idx = add_coff_sym(coff_name, 0, 0, 0, C_EXT)
        elif sym['shndx'] == SHN_ABS:
            coff_idx = add_coff_sym(coff_name, sym['value'], 0xFFFF, 0, C_EXT)
        elif sym['shndx'] == SHN_COMMON:
            # Common symbol - treat as BSS
            coff_idx = add_coff_sym(coff_name, sym['size'], 0, 0, C_EXT)
        elif sym['shndx'] in elf_sec_to_coff:
            coff_scn = elf_sec_to_coff[sym['shndx']]
            value = sym['value']
            # Adjust value for merged/offset sections
            if sym['shndx'] in rodata_offsets:
                value += rodata_offsets[sym['shndx']]  # .rodata merged into .text
            elif coff_scn == 2:
                value += data_addr
            elif coff_scn == 3:
                value += bss_addr
            sclass = C_EXT if sym['bind'] == STB_GLOBAL else C_STAT
            coff_idx = add_coff_sym(coff_name, value, coff_scn, 0, sclass)
        else:
            continue

        elf_to_coff_sym[i] = coff_idx

    # Build COFF relocations
    text_relocs = []
    data_relocs = []

    # Apply addends to section data (COFF uses REL, not RELA)
    text_data = bytearray(text_data)
    data_data = bytearray(data_data)

    for sec_idx, rels in relocs.items():
        sec_name = sections[sec_idx]['name']
        for r in rels:
            coff_rtype = elf_reltype_to_coff(r['type'])
            sym = symbols[r['sym']]

            if r['sym'] in elf_to_coff_sym:
                coff_sym_idx = elf_to_coff_sym[r['sym']]
            elif sym['type'] == STT_SECTION and sym['shndx'] in elf_sec_to_coff_sym:
                coff_sym_idx = elf_sec_to_coff_sym[sym['shndx']]
            else:
                print(f"Warning: unmapped symbol {sym['name']} (idx={r['sym']}), skipping reloc", file=sys.stderr)
                continue

            # For RELA, apply addend into the section data.
            # The COFF linker subtracts paddr from section symbol values.
            # To match, we add paddr to the pre-relocation values so they
            # cancel out correctly. For rodata (merged into text), add the
            # merge offset. For .data/.bss references, add their paddr.
            addend = r['addend']
            if sym['type'] == STT_SECTION:
                if sym['shndx'] in rodata_offsets:
                    addend += rodata_offsets[sym['shndx']]
                elif sym['shndx'] == data_idx and data_addr > 0:
                    addend += data_addr  # add .data paddr
                elif sym['shndx'] == bss_idx and bss_addr > 0:
                    addend += bss_addr   # add .bss paddr

            if addend != 0:
                off = r['offset']
                if sec_name == '.text':
                    actual_off = off
                    if coff_rtype in (R_RELLONG, R_PCRLONG):
                        old = struct.unpack('>i', text_data[actual_off:actual_off+4])[0]
                        struct.pack_into('>i', text_data, actual_off, old + addend)
                    elif coff_rtype in (R_RELWORD, R_PCRWORD):
                        old = struct.unpack('>h', text_data[actual_off:actual_off+2])[0]
                        struct.pack_into('>h', text_data, actual_off, old + addend)
                elif sec_name == '.data':
                    if coff_rtype in (R_RELLONG, R_PCRLONG):
                        old = struct.unpack('>i', data_data[off:off+4])[0]
                        struct.pack_into('>i', data_data, off, old + addend)

            coff_reloc = struct.pack('>iiH', r['offset'], coff_sym_idx, coff_rtype)

            if sec_name == '.text':
                text_relocs.append(coff_reloc)
            elif sec_name == '.data':
                data_relocs.append(coff_reloc)

    # Build the COFF file
    nscns = 3
    opthdr_size = 0  # no optional header for .o files

    # Calculate offsets
    headers_size = 20 + opthdr_size + nscns * 40
    text_offset = headers_size
    data_offset = text_offset + len(text_data)
    text_reloc_offset = data_offset + len(data_data)
    data_reloc_offset = text_reloc_offset + len(text_relocs) * 10
    sym_offset = data_reloc_offset + len(data_relocs) * 10

    nsyms = len(coff_symbols)

    # File header
    # Note: Plexus cc uses F_AR32WR (0x0100) not F_AR32W (0x0200) despite being big-endian
    F_AR32WR = 0x0100
    file_hdr = struct.pack('>HHiiiHH',
        MC68MAGIC, nscns, 0, sym_offset, nsyms, opthdr_size,
        F_LNNO | F_AR32WR)

    # Section headers
    def mksechdr(name, paddr, vaddr, size, scnptr, relptr, nreloc, flags):
        # 8 name + 4 paddr + 4 vaddr + 4 size + 4 scnptr + 4 relptr + 4 lnnoptr + 2 nreloc + 2 nlnno + 4 flags = 40
        return name.encode().ljust(8, b'\x00') + struct.pack('>iiiiiiHHi',
            paddr, vaddr, size, scnptr, relptr, 0, nreloc, 0, flags)

    text_hdr = mksechdr('.text', 0, 0, len(text_data),
        text_offset if text_data else 0,
        text_reloc_offset if text_relocs else 0,
        len(text_relocs), STYP_TEXT)

    data_hdr = mksechdr('.data', data_addr, data_addr, len(data_data),
        data_offset if data_data else 0,
        data_reloc_offset if data_relocs else 0,
        len(data_relocs), STYP_DATA)

    bss_hdr = mksechdr('.bss', bss_addr, bss_addr, bss_size,
        0, 0, 0, STYP_BSS)

    # Write output
    with open(coff_path, 'wb') as f:
        f.write(file_hdr)
        f.write(text_hdr)
        f.write(data_hdr)
        f.write(bss_hdr)
        f.write(bytes(text_data))
        f.write(bytes(data_data))
        for r in text_relocs:
            f.write(r)
        for r in data_relocs:
            f.write(r)
        for s in coff_symbols:
            f.write(s)
        # String table: first 4 bytes are the length
        strtab_len = struct.pack('>I', len(coff_strtab))
        f.write(strtab_len + coff_strtab[4:])

    return len(coff_symbols), len(text_relocs) + len(data_relocs)


if __name__ == '__main__':
    if len(sys.argv) < 3:
        print(f"Usage: {sys.argv[0]} input.o output.o", file=sys.stderr)
        sys.exit(1)

    nsyms, nrelocs = convert_elf_to_coff(sys.argv[1], sys.argv[2])
    outsize = os.path.getsize(sys.argv[2])
    print(f"Converted {sys.argv[1]} -> {sys.argv[2]} ({outsize} bytes, {nsyms} symbols, {nrelocs} relocations)")
