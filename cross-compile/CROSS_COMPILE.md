# Cross-Compiling for the Plexus P/20

A complete guide to the cross-compilation toolkit that builds MC68010 COFF
executables on a modern host, targeting AT&T UNIX System V Release 2 on the
Plexus P/20.

## Table of Contents

1. [System Overview](#system-overview)
2. [Toolchain Architecture](#toolchain-architecture)
3. [Building the Tools](#building-the-tools)
4. [The COFF Binary Format](#the-coff-binary-format)
5. [elf2coff — ELF to COFF Object Converter](#elf2coff)
6. [cofflink — COFF Linker](#cofflink)
7. [SVR2 Source Portability](#svr2-source-portability)
8. [Bugs Found and Fixed](#bugs-found-and-fixed)
9. [Disk Image Injection](#disk-image-injection)
10. [Quick Reference](#quick-reference)

---

## System Overview

The Plexus P/20 is a dual-CPU 68010 system running AT&T UNIX System V
Release 2 (SVR2), circa 1984.

| Component | Detail |
|-----------|--------|
| CPU | Dual Motorola MC68010 |
| RAM | 2 MB physical, 8 MB virtual (external MMU) |
| MMU | Custom mapper, 4 KB pages, 2048 user + 2048 system entries |
| Disk | SCSI hard disk, S5 filesystem (512-byte or 1 KB blocks) |
| OS | AT&T UNIX System V Release 2.8 |
| Native CC | AT&T Portable C Compiler (K&R C, no prototypes) |
| Binary format | COFF (MC68 magic 0x0150) |

The native compiler is extremely slow under emulation. A build that takes
seconds on the host can take 30+ minutes on the emulated system. This
toolkit eliminates the need for native compilation entirely.

## Toolchain Architecture

```
  Source code (.c, K&R C)
        │
        ▼
  m68k-elf-gcc -m68010 -std=gnu89    ← Host cross-compiler
        │
        ▼
  ELF .o files (m68k, 32-bit BE)
        │
        ▼
  elf2coff                            ← Object format converter (C)
        │
        ▼
  COFF .o files (MC68MAGIC)
        │
        ▼
  cofflink                            ← Linker (C)
   ├── Plexus crt0.o (native COFF)
   ├── Plexus libc.a (native COFF)
   └── libgcc helpers (m68000 multilib, converted through elf2coff)
        │
        ▼
  COFF executable (NMAGIC 0410)
        │
        ▼
  Inject into S5 filesystem on disk image
        │
        ▼
  Run on Plexus P/20 emulator
```

Both `elf2coff` and `cofflink` are written in C (single-file, no
dependencies beyond libc). Python reference implementations (`elf2coff.py`,
`cofflink.py`) are also available and produce byte-identical output.

### Prerequisites

```bash
brew install m68k-elf-gcc    # Homebrew on macOS
```

The m68000 multilib libgcc (at
`/opt/homebrew/Cellar/m68k-elf-gcc/15.2.0/lib/gcc/m68k-elf/15.2.0/m68000/libgcc.a`)
must be used for helper functions. The default libgcc contains 68020-only
instructions. See [Bug #1](#bug-1-68020-instructions-in-libgcc-helpers).

The Plexus system libraries (`crt0.o`, `libc.a`, `libtc.a`, etc.) and
system headers are included in the `sysroot/` directory.

## Building the Tools

The converter and linker are self-contained C99 programs with no external
dependencies:

```bash
gcc -O -Wall -o elf2coff elf2coff.c
gcc -O -Wall -o cofflink cofflink.c
```

This works with Apple Clang (`gcc` on macOS), Homebrew GCC (`gcc-15`), or
any C99-compliant compiler. The tools handle big-endian m68k binary data
via explicit byte-swap helpers, so they run correctly on any host
architecture (ARM64, x86-64, etc.).

| File | Lines | Description |
|------|-------|-------------|
| `elf2coff.c` | ~650 | ELF-to-COFF object converter |
| `cofflink.c` | ~580 | COFF linker with library resolution |
| `elf2coff.py` | ~470 | Python reference (byte-identical output) |
| `cofflink.py` | ~490 | Python reference (byte-identical output) |

The C versions are drop-in replacements for the Python versions with
identical command-line interfaces.

### Compilation Flags

```bash
m68k-elf-gcc -c -O -m68010 -std=gnu89 -fno-builtin -Dm68 \
    -Isysroot/usr/include \
    -o output.elf.o input.c
```

| Flag | Purpose |
|------|---------|
| `-m68010` | Target MC68010 instruction set (no 68020 extensions) |
| `-std=gnu89` | K&R-compatible C with GNU extensions |
| `-fno-builtin` | Don't use GCC builtins (SVR2 libc is the authority) |
| `-Dm68` | Define `m68` macro (expected by SVR2 headers) |
| `-I.../usr/include` | Use Plexus system headers for types and syscalls |

---

## The COFF Binary Format

COFF (Common Object File Format) is the native binary format on SVR2. The
Plexus uses the MC68000 variant.

### File Layout

```
┌──────────────────────────────┐  offset 0
│ File Header (20 bytes)       │
├──────────────────────────────┤  offset 20
│ Optional (a.out) Header      │  (28 bytes for executables)
├──────────────────────────────┤  offset 48
│ Section Headers              │  (40 bytes × N sections)
├──────────────────────────────┤  offset 48 + 40×N (= 0xA8 for 3 sections)
│ .text raw data               │
├──────────────────────────────┤
│ .data raw data               │
├──────────────────────────────┤
│ Relocation entries           │  (in .o files only)
├──────────────────────────────┤
│ Symbol table                 │  (18 bytes per entry)
├──────────────────────────────┤
│ String table                 │  (4-byte length prefix + strings)
└──────────────────────────────┘
```

### File Header (20 bytes)

```c
struct filehdr {
    unsigned short  f_magic;    /* 0x0150 = MC68MAGIC */
    unsigned short  f_nscns;    /* number of sections (typically 3) */
    long            f_timdat;   /* timestamp */
    long            f_symptr;   /* file offset to symbol table */
    long            f_nsyms;    /* number of symbol table entries */
    unsigned short  f_opthdr;   /* optional header size (28 for exec, 0 for .o) */
    unsigned short  f_flags;    /* F_RELFLG|F_EXEC|F_AR32W = 0x0203 */
};
```

### Optional (a.out) Header (28 bytes)

Present only in executables. Controls how the kernel loads the binary.

```c
struct aouthdr {
    short   magic;      /* 0407 = OMAGIC, 0410 = NMAGIC */
    short   vstamp;     /* version stamp (0) */
    long    tsize;      /* .text size in bytes */
    long    dsize;      /* .data size in bytes */
    long    bsize;      /* .bss size in bytes */
    long    entry;      /* entry point address */
    long    text_start; /* .text virtual address (always 0) */
    long    data_start; /* .data virtual address */
};
```

**NMAGIC (0410)**: Text is loaded read-only on a page boundary; data starts
at the next page boundary after text. This is the standard format for SVR2
executables. In practice, the Plexus kernel does not enforce text
write-protection (native libc writes to .text-range addresses), but it does
use the NMAGIC layout to determine page mapping.

**OMAGIC (0407)**: Text and data loaded as a single read-write segment. Data
immediately follows text with no page gap. Simpler but prevents text sharing
between processes.

### Section Header (40 bytes)

```c
struct scnhdr {
    char            s_name[8];  /* ".text", ".data", ".bss" */
    long            s_paddr;    /* physical address */
    long            s_vaddr;    /* virtual address */
    long            s_size;     /* section size in bytes */
    long            s_scnptr;   /* file offset to raw data */
    long            s_relptr;   /* file offset to relocations */
    long            s_lnnoptr;  /* file offset to line numbers */
    unsigned short  s_nreloc;   /* number of relocation entries */
    unsigned short  s_nlnno;    /* number of line number entries */
    long            s_flags;    /* STYP_TEXT=0x20, STYP_DATA=0x40, STYP_BSS=0x80 */
};
```

**Key nuance — `s_paddr`**: In object files (.o), `s_paddr` encodes the
section's starting address within the object's virtual address space.
For native Plexus cc output:
- `.text` paddr = 0
- `.data` paddr = (offset after .text, e.g., 0x44)
- `.bss`  paddr = (offset after .data)

The native linker and our `cofflink.py` must account for paddr when applying
relocations. See [Relocation Vaddr Adjustment](#relocation-vaddr-adjustment).

### Symbol Table Entry (18 bytes)

```c
struct syment {
    union {
        char    n_name[8];  /* symbol name (if ≤ 8 chars) */
        struct {
            long  n_zeroes; /* 0 if name is in string table */
            long  n_offset; /* offset into string table */
        } n_n;
    } n_name;
    long            n_value;    /* symbol value */
    short           n_scnum;    /* section number (1-based, 0=undef) */
    unsigned short  n_type;     /* type info */
    char            n_sclass;   /* storage class */
    char            n_numaux;   /* number of auxiliary entries */
};
```

Storage classes:
- `C_EXT (2)` — external (global) symbol
- `C_STAT (3)` — static (section symbols, local)
- `C_FILE (103)` — source file name

**Common symbols**: Uninitialized globals that appear as `C_EXT` with
`n_scnum = 0` and `n_value > 0` (the value is the required size in bytes).
The linker must allocate space in BSS for these. The termcap library uses
common symbols for `BC`, `PC`, `UP`, and `ospeed`.

### Relocation Entry (10 bytes)

```c
struct reloc {
    long            r_vaddr;    /* address of reference */
    long            r_symndx;   /* symbol table index */
    unsigned short  r_type;     /* relocation type */
};
```

Relocation types:
- `R_RELLONG (0x11)` — 32-bit absolute
- `R_RELWORD (0x10)` — 16-bit absolute
- `R_PCRLONG (0x14)` — 32-bit PC-relative
- `R_PCRWORD (0x13)` — 16-bit PC-relative

**Critical difference between native and cross-compiled .o files**:

In native COFF (Plexus cc), `r_vaddr` is an *absolute* address within the
object's virtual space (i.e., it includes `s_paddr`). In cross-compiled COFF
(from `elf2coff.py`), `r_vaddr` is *section-relative* (offset from section
start). The linker must handle both. See
[Bug #5](#bug-5-native-vs-cross-compiled-relocation-distinction).

---

## elf2coff

Converts ELF object files produced by `m68k-elf-gcc` into COFF object files
compatible with the Plexus linker and libraries.

Implemented in C (`elf2coff.c`) with a Python reference (`elf2coff.py`).
Both produce byte-identical output.

### What It Does

1. **Parses the ELF file** — reads section headers, symbol table, and
   relocations (both RELA and REL formats).

2. **Merges .rodata into .text** — GCC puts string literals and const data
   in `.rodata` sections. SVR2 COFF has no `.rodata`; the native compiler
   puts all read-only data in `.text`. The converter appends each `.rodata*`
   section to `.text`, 4-byte aligned, and adjusts all relocations
   accordingly via `rodata_offsets`.

3. **Converts RELA to REL** — ELF uses RELA relocations (addend stored in
   the relocation entry). COFF uses REL relocations (addend baked into the
   section data). The converter takes each RELA addend and writes it into
   the section data at the relocation offset.

4. **Adds paddr to section-symbol addends** — When a relocation references a
   section symbol (e.g., `.data`), the addend must account for the section's
   `paddr` so that the COFF linker's paddr subtraction produces the correct
   result:
   ```c
   if (sym->shndx == data_idx && data_addr > 0)
       addend += data_addr;   /* add .data paddr */
   else if (sym->shndx == bss_idx && bss_addr > 0)
       addend += bss_addr;    /* add .bss paddr */
   ```

5. **Maps symbols** — ELF symbols are converted to COFF symbols with
   underscore prefix (C convention on SVR2). Section symbols (`.text`,
   `.data`, `.bss`) are created with auxiliary entries. ELF section indices
   are mapped to COFF section numbers (1=text, 2=data, 3=bss).

6. **Ensures even text alignment** — The MC68010 requires instructions at
   even addresses. The converter pads `.text` to even length.

### Usage

```bash
./elf2coff input.elf.o output.o
```

### Section Address Calculation

Within each `.o` file, elf2coff sets:
```
text_addr = 0
data_addr = len(text_data)     # immediately after text
bss_addr  = data_addr + len(data_data)
```

These become the `s_paddr` values in the COFF section headers and are
used as base addresses for symbol values in sections 2 and 3.

---

## cofflink

Links COFF object files and Plexus system libraries into an executable.

Implemented in C (`cofflink.c`) with a Python reference (`cofflink.py`).
Both produce byte-identical output.

### Usage

```bash
./cofflink -o output \
    /path/to/crt0.o \
    obj1.o obj2.o ... \
    -L/path/to/libs -lc -ltermlib
```

### Key Design Decisions

#### Native vs. Cross-Compiled Object Distinction

Every object is tagged with a `native_coff` flag:
- `crt0.o` → `native_coff = True` (from Plexus system)
- Library objects (from `.a` archives) → `native_coff = True`
- All other input `.o` files → `native_coff = False` (from elf2coff)

This flag controls how relocations are processed.

#### Relocation Vaddr Adjustment

The linker computes a section-relative offset for each relocation:

```c
if (obj->native_coff)
    rvaddr_adj = rvaddr - sec->paddr;   /* native: vaddr includes paddr */
else
    rvaddr_adj = rvaddr;                /* cross: vaddr is already relative */
```

This is critical because:
- Native objects (Plexus cc): relocation `r_vaddr` = `s_paddr` + offset
- Cross-compiled objects (elf2coff): relocation `r_vaddr` = offset directly

#### Section Symbol Resolution (C_STAT)

For section symbols, the linker always subtracts paddr:
```c
int32_t paddr = ssec ? ssec->paddr : 0;
int32_t sym_val = base + obj_section_base - paddr;
```

This works for both native and cross-compiled objects because elf2coff
adds paddr to pre-relocation addends, which cancels the subtraction.

#### Named Symbol Resolution (C_EXT)

```c
int32_t section_offset = value - sec->paddr;
sym_values[name] = base + section_offset;
```

Always subtracts paddr to get the section-relative offset, then adds
the final base address.

#### Common Symbol Handling

Common symbols (`n_scnum = 0, n_value > 0`) represent uninitialized
globals whose size is in `n_value`. The linker:

1. Tracks them in a `common_syms` dict during symbol collection
2. Considers them as "provided" during library resolution (so library
   members with common symbols can satisfy undefined references)
3. Allocates space in BSS at the end of layout, aligned to 4 bytes
4. The BSS total in the COFF header includes all common allocations

This was necessary for termcap variables (`BC`, `PC`, `UP`, `ospeed`)
from `libtermlib.a`.

#### Auto BSS Symbols

Some libc symbols are referenced but never defined in any library
(they're expected to be allocated by the linker). These are handled
with a hardcoded dictionary:

```c
struct { const char *name; int size; } common_bss[] = {
    {"_errno", 4}, {"__sibuf", 1024}, {"__smbuf", 1024}, {"__sobuf", 1024},
    {"__sigfunc", 128}, ...
};
```

#### Text Page Alignment

The Plexus kernel's COFF loader requires `.text` to fill to an exact
4 KB page boundary. If text doesn't fill the last page, the kernel fails
to map the data segment pages correctly. The linker pads text:

```c
uint32_t padded_text = (text_offset + page_size - 1) & ~(page_size - 1);
total_text = padded_text;
data_start = text_start + total_text;
```

#### Linker-Provided Symbols

```c
sym_set("_etext", text_start + total_text);
sym_set("_edata", data_start + total_data);
sym_set("_end",   bss_start_addr + total_bss);
```

`_end` is critically used by `sbrk()` in libc to set the initial heap
break. If it's wrong, `malloc()` will crash on the first allocation.

### Output Format

The linker produces a COFF executable with:
- NMAGIC (0410) in the optional header
- 3 sections: `.text`, `.data`, `.bss`
- No symbol table or string table (stripped)
- Flags: `F_RELFLG | F_EXEC | F_AR32W` (0x0203)

---

## SVR2 Source Portability

Code must be ported to compile under both `m68k-elf-gcc -std=gnu89` and
the constraints of SVR2. Key issues encountered:

### Language Differences

| Issue | SVR2 cc behavior | Fix |
|-------|-----------------|-----|
| `#elif` | Not supported | Nested `#ifdef`/`#else`/`#endif` |
| Function prototypes | Not supported in K&R mode | Use `()` declarations |
| `void *` | Not supported | `typedef char *pointer;` or `typedef char *vptr;` |
| `const` | May not be supported | `#define const` in non-ANSI mode |

### Missing APIs

| Modern API | SVR2 equivalent | Notes |
|------------|----------------|-------|
| `waitpid()` | `wait()` | SVR2 has no waitpid; use `wait(&status)` |
| `SIGCHLD` | `SIGCLD` | `#define SIGCHLD SIGCLD` |
| `setpgid()` | `setpgrp()` | SVR2 setpgrp takes no arguments |
| `lstat()` | `stat()` | No symlinks on SVR2; `#define lstat stat` |
| `sigaction()` | `signal()` | SVR2 resets handlers after delivery (`RESETHANDNEEDED`) |
| `tcsetpgrp()` | N/A | No POSIX terminal control |
| `SIGTSTP/SIGSTOP` | N/A | No job control signals |
| `select()` | N/A | Not available |

### Header Differences

- `<sys/errno.h>` → use `<errno.h>`
- `<sys/dir.h>` → may conflict with custom `dirent.h`
- `<setjmp.h>` → exists but use Plexus system headers
- `<stdlib.h>` → doesn't exist; declare `malloc()` etc. manually
- `<string.h>` → exists as `HAS_STRING`

### ash Configuration (shell.h)

```c
#define JOBS 0        /* no job control on SVR2 */
#define SYMLINKS 0    /* no symlinks */
#define DIRENT 0      /* use custom dirent, not sys/dirent.h */
#define ATTY 0        /* no terminal attribute detection */
#define SYSV          /* enable System V code paths */
#define STATIC static /* make STATIC actually static */
```

### zsh Configuration (config.h)

```c
#define PLEXUS_SVR2
#define TERMIO          /* not TERMIOS */
#define RESETHANDNEEDED /* signal handlers reset after delivery */
#define HOSTTYPE "m68k"
#define SIGCHLD SIGCLD  /* SVR2 name for child signal */
#define lstat stat      /* no symlinks */
typedef char *vptr;     /* no void* */
```

### MC68010 Alignment

The MC68010 requires word and long accesses at even addresses. Any array
used as a lookup table that might be accessed with word operations must
have even size. The `syntax.c` arrays in ash were padded from 257 to 258
bytes to prevent odd-address faults.

### Symbol Table Overflow

SVR2 cc has a limited symbol table. Large source files (like zsh's
`builtin.c` with hundreds of forward declarations from `.pro` files) can
overflow it. Fix: strip `.pro` includes and add only the necessary forward
declarations.

### 14-Character Filename Limit

SVR2 S5 filesystem limits filenames to 14 characters. Source files and
object files must respect this limit.

---

## Bugs Found and Fixed

Five critical bugs were discovered during the development of this toolkit.

### Bug #1: 68020 Instructions in libgcc Helpers

**Symptom**: "Illegal instruction" crash in `hashvar()` → `__umodsi3()`.

**Root cause**: The default `m68k-elf-gcc` libgcc helper functions
(`_divsi3.o`, `_modsi3.o`, `_umodsi3.o`) contain `BSR.L` (opcode `0x61FF`),
a 32-bit branch-to-subroutine instruction introduced with the MC68020. The
MC68010 does not support this instruction. On the 68010, `0x61FF` is
interpreted as `BSR.B` with displacement -1, jumping to an odd address and
causing a trap.

**Detection**: Scanning object file `.text` sections for the byte sequence
`0x61 0xFF`:
```c
for (int i = 0; i < text_len - 1; i++)
    if (text[i] == 0x61 && text[i+1] == 0xFF)
        printf("BSR.L at offset 0x%04X\n", i);
```

**Fix**: Extract helpers from the m68000 multilib:
```bash
m68k-elf-ar x /opt/homebrew/Cellar/m68k-elf-gcc/15.2.0/lib/gcc/m68k-elf/15.2.0/m68000/libgcc.a \
    _divsi3.o _modsi3.o _mulsi3.o _udivsi3.o _umodsi3.o
```

The m68000 versions use `JSR` (opcode `0x4EB9`) with absolute addressing
instead of `BSR.L`, which is valid on all 68k processors.

| Function | Default (68020) | m68000 multilib |
|----------|----------------|-----------------|
| `_divsi3` | 1 BSR.L | 0 BSR.L |
| `_modsi3` | 2 BSR.L | 0 BSR.L |
| `_umodsi3` | 2 BSR.L | 0 BSR.L |
| `_mulsi3` | clean | clean |
| `_udivsi3` | clean | clean |

### Bug #2: Relocation Vaddr Not Adjusted for Section Paddr

**Symptom**: `sbrk()` break variable initialized to 0 instead of `_end`;
`malloc()` crashes with Memory fault on first allocation.

**Root cause**: In the linker, relocation `r_vaddr` values from native
COFF objects include the section's `s_paddr`. The linker was using `r_vaddr`
directly as an offset into the merged section buffer, causing relocations to
be applied at the wrong location. For `sbrk.o` from libc:

```
sbrk.o .data section: paddr = 0x44, size = 4
Relocation: r_vaddr = 0x44, sym = _end

Before fix: abs_off = obj_data_base + 0x44  → WRONG (past end of section)
After fix:  abs_off = obj_data_base + 0     → CORRECT (0x44 - 0x44 = 0)
```

The `_end` relocation was silently skipped (out-of-bounds check), leaving
the break variable at zero. When `sbrk(n)` ran, it computed
`new_break = 0 + n` and called `brk(n)`, which the kernel rejected because
it was below the current break.

**Fix**: Subtract paddr from r_vaddr for native COFF objects:
```c
if (obj->native_coff)
    rvaddr_adj = rvaddr - sec->paddr;
else
    rvaddr_adj = rvaddr;
```

### Bug #3: Text Must Be Page-Aligned

**Symptom**: Memory fault accessing `.data` section; mapper shows "proc uid 4
page uid 0" — the data page wasn't mapped to the user process.

**Root cause**: The Plexus SVR2 kernel's COFF loader does not correctly map
data segment pages when `.text` does not fill to a 4 KB page boundary. If
text is, say, 65012 bytes (ending partway through page 15), data starts at
page 16 (0x10000). The kernel maps text pages 0-15 but fails to map data
page 16.

Survey of all native binaries on the Plexus disk confirmed that no binary
has data starting at page 16 (0x10000). The unique data start pages are:
`[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,17,19,20,21,22,24,25,26,27,33,79]`
— page 16 is conspicuously absent.

**Fix**: Pad `.text` to an exact page boundary:
```c
#define PAGE_SIZE 0x1000
total_text = (text_offset + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
data_start = text_start + total_text;
```

This ensures there is no gap between the last text page and the first data
page.

### Bug #4: Common Symbols from Libraries Not Handled

**Symptom**: Unresolved symbols `_BC`, `_PC`, `_UP`, `_ospeed` when linking
zsh with termcap.

**Root cause**: COFF "common" symbols have `n_scnum = 0` and `n_value > 0`
(the value represents the required allocation size). The linker was only
considering symbols with `n_scnum != 0` as "provided" by a library member:

```c
/* Old (broken): */
if (sym->sclass == C_EXT && sym->scnum != 0)
    provides = 1;

/* Fixed: */
if (sym->sclass == C_EXT && (sym->scnum != 0 || sym->value > 0))
    provides = 1;
```

Common symbols must be allocated in BSS by the linker, with the largest
requested size taking precedence when multiple objects define the same
common symbol.

### Bug #5: Native vs. Cross-Compiled Relocation Distinction

**Symptom**: Cross-compiled data section relocations applied at negative
offsets (silently skipped), producing corrupt binaries.

**Root cause**: Bug #2's fix (subtracting paddr from r_vaddr) was applied
unconditionally to all objects. But cross-compiled objects from elf2coff
already have section-relative r_vaddr values. Subtracting paddr from these
produced negative offsets:

```
Cross-compiled .data: paddr = 0x3A0, reloc vaddr = 0x10 (section-relative)
rvaddr_adj = 0x10 - 0x3A0 = -0x390  → NEGATIVE → relocation skipped!
```

**Fix**: Use the `native_coff` flag to distinguish:
```c
if (obj->native_coff)
    rvaddr_adj = rvaddr - sec->paddr;
else
    rvaddr_adj = rvaddr;
```

---

## Disk Image Injection

The Plexus disk image uses the S5 (System V) filesystem. To install
cross-compiled binaries without running the emulator, we parse the
filesystem directly.

### S5 Filesystem Layout

```
Block 0:  Boot block
Block 1:  Superblock (contains free block/inode lists)
Block 2+: Inode list (64 bytes per inode, 16 per 1 KB block)
Block N+: Data blocks
```

### Key Superblock Fields

```
Offset 0x000: s_isize  (2 bytes) — inode list size in blocks
Offset 0x002: s_fsize  (4 bytes) — total filesystem size in blocks
Offset 0x006: s_nfree  (2 bytes) — entries in free block list
Offset 0x008: s_free[] (50×4 bytes) — free block numbers
Offset 0x1F0: s_magic  (4 bytes) — 0xFD187E20
Offset 0x1F4: s_type   (4 bytes) — 1=512B blocks, 2=1KB blocks
```

### Inode Structure (64 bytes)

```
Offset 0:  di_mode   (2 bytes) — file type and permissions
Offset 2:  di_nlink  (2 bytes)
Offset 4:  di_uid    (2 bytes)
Offset 6:  di_gid    (2 bytes)
Offset 8:  di_size   (4 bytes) — file size
Offset 12: di_addr   (39 bytes) — 13 × 3-byte block addresses
           [0-9]: direct blocks
           [10]:  single indirect
           [11]:  double indirect
           [12]:  triple indirect
```

### Block Address Encoding

Block addresses are 3 bytes (24 bits), big-endian:
```python
addr = (disk[offset] << 16) | (disk[offset+1] << 8) | disk[offset+2]
```

### Directory Entries (16 bytes each)

```
Offset 0: d_ino  (2 bytes) — inode number (0 = empty)
Offset 2: d_name (14 bytes) — filename, NUL-padded
```

### Partition Discovery

The disk has multiple partitions. Each partition's superblock can be found
by searching for the S5 magic `0xFD187E20` at offset `0x1F0` within
candidate blocks:

```python
for off in range(0, len(disk), 1024):
    sb = off + 1024  # superblock is at block 1
    if disk[sb+0x1F0:sb+0x1F4] == b'\xFD\x18\x7E\x20':
        print(f"S5 filesystem at disk offset 0x{off:X}")
```

On the Plexus disk:
| Partition | Offset | Name | Block size |
|-----------|--------|------|------------|
| dk0 (root) | 0x000000 | `root` | 512B |
| dk2 (/user) | 0x203A000 | `user` | 1KB |
| dk3 (stock) | 0x30D4000 | `stocku` | 1KB |

### File Injection Process

To replace a file's contents without the emulator:

1. Find the file's inode by traversing directories
2. Read the inode's block list (direct + indirect)
3. Write new data to those blocks, padding to the original file size
4. Do NOT change the inode's di_size (keeps the filesystem consistent)

**Important**: Files are NOT necessarily contiguous on disk. The zsh binary
had 77 gaps across 312 blocks. Always follow the block pointers.

To create a new file:
1. Allocate blocks from the free list in the superblock
2. Allocate an inode from the free inode list
3. Write the inode with mode, size, and block addresses
4. Add a directory entry in the parent directory

---

## Quick Reference

### Build the tools

```bash
gcc -O -Wall -o elf2coff elf2coff.c
gcc -O -Wall -o cofflink cofflink.c
```

### Build ash

```bash
# Compile each source file
for f in main var init eval ...; do
    m68k-elf-gcc -c -O -DSHELL -m68010 -std=gnu89 -fno-builtin -Dm68 \
        -Isysroot/usr/include -Iash-plexus \
        -o ash-plexus/${f}.elf.o ash-plexus/${f}.c
    ./elf2coff ash-plexus/${f}.elf.o ash-plexus/${f}.o
done

# Extract m68000 libgcc helpers
m68k-elf-ar x .../m68000/libgcc.a _divsi3.o _modsi3.o _mulsi3.o _udivsi3.o _umodsi3.o
for f in _divsi3 _modsi3 _mulsi3 _udivsi3 _umodsi3; do
    ./elf2coff ${f}.o ash-plexus/${f}.o
done

# Link
./cofflink -o ash.coff \
    sysroot/lib/crt0.o \
    ash-plexus/*.o \
    -Lsysroot/lib -lc
```

### Run on emulator

```bash
cd plexus_20_emu
./emu -u15 ../ROMs/U15-MERGED.BIN -u17 ../ROMs/U17-MERGED.BIN \
    -hd ../disk/plexus-crosstest.img
# At the # prompt:
mount /dev/dk2 /user
/user/xash -c 'echo hello world'
```
