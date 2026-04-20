/*
 * ELF-to-COFF object file converter for Motorola 68010 (Plexus P/20).
 * Converts m68k-elf-gcc output to AT&T UNIX System V COFF format.
 *
 * Usage: elf2coff input.o output.o
 *
 * Compile: cc -O -Wall -o elf2coff elf2coff.c
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

/* ---- Big-endian read/write helpers ---- */

static inline uint16_t rd16(const uint8_t *p) {
    return (uint16_t)((p[0] << 8) | p[1]);
}

static inline uint32_t rd32(const uint8_t *p) {
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8)  | (uint32_t)p[3];
}

static inline int32_t rd32s(const uint8_t *p) {
    return (int32_t)rd32(p);
}

static inline int16_t rd16s(const uint8_t *p) {
    return (int16_t)rd16(p);
}

static inline void wr16(uint8_t *p, uint16_t v) {
    p[0] = (v >> 8) & 0xff;
    p[1] = v & 0xff;
}

static inline void wr32(uint8_t *p, uint32_t v) {
    p[0] = (v >> 24) & 0xff;
    p[1] = (v >> 16) & 0xff;
    p[2] = (v >> 8) & 0xff;
    p[3] = v & 0xff;
}

/* ---- Dynamic array GROW macro ---- */

#define GROW(arr, cnt, cap) do {               \
    if ((cnt) >= (cap)) {                      \
        (cap) = (cap) ? (cap) * 2 : 16;       \
        (arr) = realloc((arr), (cap) * sizeof(*(arr))); \
        if (!(arr)) { fprintf(stderr, "out of memory\n"); exit(1); } \
    }                                          \
} while (0)

/* ---- COFF Constants ---- */

#define MC68MAGIC   0x0150
#define F_LNNO      0x0004
#define F_AR32WR    0x0100

#define STYP_TEXT   0x0020
#define STYP_DATA   0x0040
#define STYP_BSS    0x0080

#define C_EXT       2
#define C_STAT      3
#define C_FILE      103

/* COFF relocation types (m68k) */
#define R_RELBYTE   0x000F
#define R_RELWORD   0x0010
#define R_RELLONG   0x0011
#define R_PCRBYTE   0x0012
#define R_PCRWORD   0x0013
#define R_PCRLONG   0x0014

/* ---- ELF Constants ---- */

#define ELF_MAGIC0  0x7f
#define ELF_MAGIC1  'E'
#define ELF_MAGIC2  'L'
#define ELF_MAGIC3  'F'
#define EM_68K      4
#define SHT_SYMTAB  2
#define SHT_STRTAB  3
#define SHT_RELA    4
#define SHT_REL     9
#define STB_GLOBAL  1
#define STT_SECTION 3
#define STT_FILE    4
#define SHN_UNDEF   0
#define SHN_ABS     0xFFF1
#define SHN_COMMON  0xFFF2

/* ELF m68k relocation types */
#define R_68K_32    1
#define R_68K_16    2
#define R_68K_8     3
#define R_68K_PC32  4
#define R_68K_PC16  5
#define R_68K_PC8   6

/* ---- Data structures ---- */

typedef struct {
    uint32_t name_off;
    uint32_t type;
    uint32_t flags;
    uint32_t addr;
    uint32_t offset;
    uint32_t size;
    uint32_t link;
    uint32_t info;
    uint32_t addralign;
    uint32_t entsize;
    const uint8_t *data;
    char name[256];
} elf_section_t;

typedef struct {
    char name[256];
    uint32_t value;
    uint32_t size;
    uint8_t bind;
    uint8_t type;
    uint16_t shndx;
} elf_symbol_t;

typedef struct {
    uint32_t offset;
    uint32_t sym;
    uint8_t type;
    int32_t addend;
} elf_reloc_t;

typedef struct {
    elf_reloc_t *rels;
    int count;
    int cap;
} reloc_list_t;

typedef struct {
    uint8_t data[18];
} coff_sym_entry_t;

typedef struct {
    uint8_t data[10];
} coff_reloc_entry_t;

/* ---- Helpers ---- */

static uint16_t elf_reltype_to_coff(uint8_t rtype) {
    switch (rtype) {
    case R_68K_32:   return R_RELLONG;
    case R_68K_16:   return R_RELWORD;
    case R_68K_8:    return R_RELBYTE;
    case R_68K_PC32: return R_PCRLONG;
    case R_68K_PC16: return R_PCRWORD;
    case R_68K_PC8:  return R_PCRBYTE;
    default:
        fprintf(stderr, "Unsupported ELF relocation type: %d\n", rtype);
        exit(1);
    }
}

static void get_string(const uint8_t *strtab, uint32_t strtab_size,
                        uint32_t offset, char *out, size_t outsize) {
    if (offset >= strtab_size) {
        out[0] = '\0';
        return;
    }
    size_t i;
    for (i = 0; i < outsize - 1 && offset + i < strtab_size; i++) {
        if (strtab[offset + i] == 0) break;
        out[i] = (char)strtab[offset + i];
    }
    out[i] = '\0';
}

/* ---- Main conversion ---- */

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s input.o output.o\n", argv[0]);
        return 1;
    }

    const char *elf_path = argv[1];
    const char *coff_path = argv[2];

    /* Read entire ELF file into memory */
    FILE *fp = fopen(elf_path, "rb");
    if (!fp) {
        fprintf(stderr, "Cannot open %s\n", elf_path);
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    uint8_t *raw = malloc(file_size);
    if (!raw) {
        fprintf(stderr, "out of memory\n");
        return 1;
    }
    if ((long)fread(raw, 1, file_size, fp) != file_size) {
        fprintf(stderr, "Read error\n");
        return 1;
    }
    fclose(fp);

    /* Validate ELF header */
    if (file_size < 52 ||
        raw[0] != ELF_MAGIC0 || raw[1] != ELF_MAGIC1 ||
        raw[2] != ELF_MAGIC2 || raw[3] != ELF_MAGIC3) {
        fprintf(stderr, "Not an ELF file: %s\n", elf_path);
        return 1;
    }
    if (raw[4] != 1 || raw[5] != 2) {
        fprintf(stderr, "Expected 32-bit big-endian ELF\n");
        return 1;
    }
    if (rd16(raw + 18) != EM_68K) {
        fprintf(stderr, "Expected m68k ELF, got machine=%d\n", rd16(raw + 18));
        return 1;
    }

    uint32_t e_shoff = rd32(raw + 32);
    uint16_t e_shentsize = rd16(raw + 46);
    uint16_t e_shnum = rd16(raw + 48);
    uint16_t e_shstrndx = rd16(raw + 50);

    /* Parse section headers */
    elf_section_t *sections = calloc(e_shnum, sizeof(elf_section_t));
    if (!sections) { fprintf(stderr, "out of memory\n"); return 1; }

    for (int i = 0; i < e_shnum; i++) {
        const uint8_t *sh = raw + e_shoff + i * e_shentsize;
        sections[i].name_off = rd32(sh + 0);
        sections[i].type = rd32(sh + 4);
        sections[i].flags = rd32(sh + 8);
        sections[i].addr = rd32(sh + 12);
        sections[i].offset = rd32(sh + 16);
        sections[i].size = rd32(sh + 20);
        sections[i].link = rd32(sh + 24);
        sections[i].info = rd32(sh + 28);
        sections[i].addralign = rd32(sh + 32);
        sections[i].entsize = rd32(sh + 36);
        sections[i].data = (sections[i].size > 0) ? raw + sections[i].offset : NULL;
    }

    /* Resolve section names */
    const uint8_t *shstrtab = sections[e_shstrndx].data;
    uint32_t shstrtab_size = sections[e_shstrndx].size;
    for (int i = 0; i < e_shnum; i++) {
        get_string(shstrtab, shstrtab_size, sections[i].name_off,
                   sections[i].name, sizeof(sections[i].name));
    }

    /* Parse symbol table */
    elf_symbol_t *symbols = NULL;
    int nsymbols = 0;
    int symtab_sec_idx = -1;
    const uint8_t *strtab_data = NULL;
    uint32_t strtab_size = 0;

    for (int i = 0; i < e_shnum; i++) {
        if (sections[i].type == SHT_SYMTAB) {
            symtab_sec_idx = i;
            strtab_data = sections[sections[i].link].data;
            strtab_size = sections[sections[i].link].size;
            break;
        }
    }

    if (symtab_sec_idx >= 0) {
        const uint8_t *d = sections[symtab_sec_idx].data;
        uint32_t dsize = sections[symtab_sec_idx].size;
        nsymbols = dsize / 16;
        symbols = calloc(nsymbols, sizeof(elf_symbol_t));
        if (!symbols) { fprintf(stderr, "out of memory\n"); return 1; }

        for (int i = 0; i < nsymbols; i++) {
            const uint8_t *p = d + i * 16;
            uint32_t st_name = rd32(p);
            symbols[i].value = rd32(p + 4);
            symbols[i].size = rd32(p + 8);
            symbols[i].bind = p[12] >> 4;
            symbols[i].type = p[12] & 0xf;
            symbols[i].shndx = rd16(p + 14);
            if (st_name) {
                get_string(strtab_data, strtab_size, st_name,
                           symbols[i].name, sizeof(symbols[i].name));
            } else {
                symbols[i].name[0] = '\0';
            }
        }
    }

    /* Parse relocations - allocate per-section reloc lists */
    reloc_list_t *relocs = calloc(e_shnum, sizeof(reloc_list_t));
    if (!relocs) { fprintf(stderr, "out of memory\n"); return 1; }

    for (int i = 0; i < e_shnum; i++) {
        if (sections[i].type == SHT_RELA) {
            uint32_t target_sec = sections[i].info;
            const uint8_t *d = sections[i].data;
            uint32_t dsize = sections[i].size;
            for (uint32_t j = 0; j < dsize; j += 12) {
                GROW(relocs[target_sec].rels, relocs[target_sec].count, relocs[target_sec].cap);
                elf_reloc_t *r = &relocs[target_sec].rels[relocs[target_sec].count++];
                r->offset = rd32(d + j);
                uint32_t r_info = rd32(d + j + 4);
                r->sym = r_info >> 8;
                r->type = r_info & 0xff;
                r->addend = rd32s(d + j + 8);
            }
        } else if (sections[i].type == SHT_REL) {
            uint32_t target_sec = sections[i].info;
            const uint8_t *d = sections[i].data;
            uint32_t dsize = sections[i].size;
            for (uint32_t j = 0; j < dsize; j += 8) {
                GROW(relocs[target_sec].rels, relocs[target_sec].count, relocs[target_sec].cap);
                elf_reloc_t *r = &relocs[target_sec].rels[relocs[target_sec].count++];
                r->offset = rd32(d + j);
                uint32_t r_info = rd32(d + j + 4);
                r->sym = r_info >> 8;
                r->type = r_info & 0xff;
                r->addend = 0;
            }
        }
    }

    /* Identify ELF sections */
    int text_idx = -1, data_idx = -1, bss_idx = -1;
    int *rodata_indices = NULL;
    int nrodata = 0, rodata_cap = 0;

    uint8_t *text_data = NULL;
    uint32_t text_len = 0;
    uint32_t text_cap = 0;

    uint8_t *data_data = NULL;
    uint32_t data_len = 0;
    uint32_t data_cap = 0;

    uint32_t bss_size = 0;

    for (int i = 0; i < e_shnum; i++) {
        if (strcmp(sections[i].name, ".text") == 0) {
            text_idx = i;
            text_len = sections[i].size;
            text_cap = text_len + 256;
            text_data = malloc(text_cap);
            if (!text_data) { fprintf(stderr, "out of memory\n"); return 1; }
            if (text_len > 0 && sections[i].data)
                memcpy(text_data, sections[i].data, text_len);
        } else if (strncmp(sections[i].name, ".rodata", 7) == 0) {
            GROW(rodata_indices, nrodata, rodata_cap);
            rodata_indices[nrodata++] = i;
        } else if (strcmp(sections[i].name, ".data") == 0) {
            data_idx = i;
            data_len = sections[i].size;
            data_cap = data_len + 256;
            data_data = malloc(data_cap);
            if (!data_data) { fprintf(stderr, "out of memory\n"); return 1; }
            if (data_len > 0 && sections[i].data)
                memcpy(data_data, sections[i].data, data_len);
        } else if (strcmp(sections[i].name, ".bss") == 0) {
            bss_idx = i;
            bss_size = sections[i].size;
        }
    }

    /* Ensure text_data and data_data are allocated even if sections empty */
    if (!text_data) {
        text_cap = 256;
        text_data = calloc(1, text_cap);
        if (!text_data) { fprintf(stderr, "out of memory\n"); return 1; }
    }
    if (!data_data) {
        data_cap = 256;
        data_data = calloc(1, data_cap);
        if (!data_data) { fprintf(stderr, "out of memory\n"); return 1; }
    }

    /* Merge .rodata* sections into .text */
    /* rodata_offsets[i] maps section index to offset within merged text.
       We use a flat array indexed by section index. */
    int32_t *rodata_offsets = calloc(e_shnum, sizeof(int32_t));
    int *has_rodata_offset = calloc(e_shnum, sizeof(int));
    if (!rodata_offsets || !has_rodata_offset) {
        fprintf(stderr, "out of memory\n"); return 1;
    }

    for (int ri = 0; ri < nrodata; ri++) {
        int sidx = rodata_indices[ri];
        const uint8_t *rdata = sections[sidx].data;
        uint32_t rsize = sections[sidx].size;

        if (!rdata || rsize == 0) {
            rodata_offsets[sidx] = (int32_t)text_len;
            has_rodata_offset[sidx] = 1;
            continue;
        }

        /* Align to 4 bytes */
        while (text_len % 4) {
            if (text_len >= text_cap) {
                text_cap = text_cap * 2;
                text_data = realloc(text_data, text_cap);
                if (!text_data) { fprintf(stderr, "out of memory\n"); return 1; }
            }
            text_data[text_len++] = 0;
        }

        rodata_offsets[sidx] = (int32_t)text_len;
        has_rodata_offset[sidx] = 1;

        /* Append rodata */
        while (text_len + rsize > text_cap) {
            text_cap = text_cap * 2;
            text_data = realloc(text_data, text_cap);
            if (!text_data) { fprintf(stderr, "out of memory\n"); return 1; }
        }
        memcpy(text_data + text_len, rdata, rsize);
        text_len += rsize;

        /* Merge relocations */
        if (relocs[sidx].count > 0) {
            if (text_idx < 0) {
                /* No .text section exists yet - should not happen but handle it */
                fprintf(stderr, "Error: .rodata relocs but no .text section\n");
                return 1;
            }
            for (int j = 0; j < relocs[sidx].count; j++) {
                GROW(relocs[text_idx].rels, relocs[text_idx].count, relocs[text_idx].cap);
                elf_reloc_t *r = &relocs[text_idx].rels[relocs[text_idx].count++];
                *r = relocs[sidx].rels[j];
                r->offset += rodata_offsets[sidx];
            }
            relocs[sidx].count = 0;
        }
    }

    /* Even text padding */
    if (text_len % 2) {
        if (text_len >= text_cap) {
            text_cap = text_cap * 2;
            text_data = realloc(text_data, text_cap);
            if (!text_data) { fprintf(stderr, "out of memory\n"); return 1; }
        }
        text_data[text_len++] = 0;
    }

    /* Section base addresses */
    uint32_t data_addr = text_len;
    uint32_t bss_addr = data_addr + data_len;

    /* ---- Build COFF symbol table ---- */
    coff_sym_entry_t *coff_symbols = NULL;
    int ncoff_sym = 0, coff_sym_cap = 0;

    uint8_t *coff_strtab = NULL;
    uint32_t strtab_len_out = 4; /* starts with 4-byte length */
    uint32_t strtab_cap_out = 256;
    coff_strtab = malloc(strtab_cap_out);
    if (!coff_strtab) { fprintf(stderr, "out of memory\n"); return 1; }
    memset(coff_strtab, 0, 4); /* placeholder for length */

    /* Map from ELF sym index to COFF sym index. -1 = unmapped */
    int *elf_to_coff_sym = malloc(nsymbols * sizeof(int));
    if (!elf_to_coff_sym && nsymbols > 0) {
        fprintf(stderr, "out of memory\n"); return 1;
    }
    for (int i = 0; i < nsymbols; i++) elf_to_coff_sym[i] = -1;

    /* --- .file symbol + aux --- */
    {
        GROW(coff_symbols, ncoff_sym, coff_sym_cap);
        coff_sym_entry_t *e = &coff_symbols[ncoff_sym];
        memset(e->data, 0, 18);
        /* name = ".file" (<=8 chars) */
        memcpy(e->data, ".file\0\0\0", 8);
        /* value = 0 */
        wr32(e->data + 8, 0);
        /* scnum = 0xFFFE = N_DEBUG */
        wr16(e->data + 12, 0xFFFE);
        /* type = 0 */
        wr16(e->data + 14, 0);
        /* sclass = C_FILE (103) */
        e->data[16] = (uint8_t)C_FILE;
        /* numaux = 1 */
        e->data[17] = 1;
        ncoff_sym++;

        /* aux entry: source filename, up to 18 bytes */
        GROW(coff_symbols, ncoff_sym, coff_sym_cap);
        coff_sym_entry_t *aux = &coff_symbols[ncoff_sym];
        memset(aux->data, 0, 18);
        /* basename of elf_path */
        char *path_copy = strdup(elf_path);
        char *bname = basename(path_copy);
        size_t blen = strlen(bname);
        if (blen > 18) blen = 18;
        memcpy(aux->data, bname, blen);
        free(path_copy);
        ncoff_sym++;
    }

    /* --- Section symbols: .text, .data, .bss --- */
    /* Each gets a main entry + 1 aux entry */

    int text_sym_idx, data_sym_idx, bss_sym_idx;

    /* .text */
    {
        text_sym_idx = ncoff_sym;
        GROW(coff_symbols, ncoff_sym, coff_sym_cap);
        coff_sym_entry_t *e = &coff_symbols[ncoff_sym];
        memset(e->data, 0, 18);
        memcpy(e->data, ".text\0\0\0", 8);
        wr32(e->data + 8, 0);       /* value = 0 */
        wr16(e->data + 12, 1);      /* scnum = 1 */
        wr16(e->data + 14, 0);      /* type = 0 */
        e->data[16] = (uint8_t)C_STAT;
        e->data[17] = 1;            /* numaux = 1 */
        ncoff_sym++;

        GROW(coff_symbols, ncoff_sym, coff_sym_cap);
        coff_sym_entry_t *aux = &coff_symbols[ncoff_sym];
        memset(aux->data, 0, 18);
        wr32(aux->data, text_len);   /* section length */
        wr16(aux->data + 4, 0);     /* nreloc (filled in aux) */
        wr16(aux->data + 6, 0);     /* nlnno */
        ncoff_sym++;
    }

    /* .data */
    {
        data_sym_idx = ncoff_sym;
        GROW(coff_symbols, ncoff_sym, coff_sym_cap);
        coff_sym_entry_t *e = &coff_symbols[ncoff_sym];
        memset(e->data, 0, 18);
        memcpy(e->data, ".data\0\0\0", 8);
        wr32(e->data + 8, data_addr); /* value = data_addr */
        wr16(e->data + 12, 2);       /* scnum = 2 */
        wr16(e->data + 14, 0);
        e->data[16] = (uint8_t)C_STAT;
        e->data[17] = 1;
        ncoff_sym++;

        GROW(coff_symbols, ncoff_sym, coff_sym_cap);
        coff_sym_entry_t *aux = &coff_symbols[ncoff_sym];
        memset(aux->data, 0, 18);
        wr32(aux->data, data_len);
        wr16(aux->data + 4, 0);
        wr16(aux->data + 6, 0);
        ncoff_sym++;
    }

    /* .bss */
    {
        bss_sym_idx = ncoff_sym;
        GROW(coff_symbols, ncoff_sym, coff_sym_cap);
        coff_sym_entry_t *e = &coff_symbols[ncoff_sym];
        memset(e->data, 0, 18);
        memcpy(e->data, ".bss\0\0\0\0", 8);
        wr32(e->data + 8, bss_addr);
        wr16(e->data + 12, 3);
        wr16(e->data + 14, 0);
        e->data[16] = (uint8_t)C_STAT;
        e->data[17] = 1;
        ncoff_sym++;

        GROW(coff_symbols, ncoff_sym, coff_sym_cap);
        coff_sym_entry_t *aux = &coff_symbols[ncoff_sym];
        memset(aux->data, 0, 18);
        wr32(aux->data, bss_size);
        wr16(aux->data + 4, 0);
        wr16(aux->data + 6, 0);
        ncoff_sym++;
    }

    /* Map ELF section indices to COFF section numbers */
    int *elf_sec_to_coff = calloc(e_shnum, sizeof(int)); /* 0 = unmapped */
    int *elf_sec_to_coff_sym = malloc(e_shnum * sizeof(int));
    if (!elf_sec_to_coff || !elf_sec_to_coff_sym) {
        fprintf(stderr, "out of memory\n"); return 1;
    }
    for (int i = 0; i < e_shnum; i++) elf_sec_to_coff_sym[i] = -1;

    if (text_idx >= 0) {
        elf_sec_to_coff[text_idx] = 1;
        elf_sec_to_coff_sym[text_idx] = text_sym_idx;
    }
    for (int ri = 0; ri < nrodata; ri++) {
        int sidx = rodata_indices[ri];
        elf_sec_to_coff[sidx] = 1;
        elf_sec_to_coff_sym[sidx] = text_sym_idx;
    }
    if (data_idx >= 0) {
        elf_sec_to_coff[data_idx] = 2;
        elf_sec_to_coff_sym[data_idx] = data_sym_idx;
    }
    if (bss_idx >= 0) {
        elf_sec_to_coff[bss_idx] = 3;
        elf_sec_to_coff_sym[bss_idx] = bss_sym_idx;
    }

    /* ---- Add regular symbols ---- */
    for (int i = 0; i < nsymbols; i++) {
        elf_symbol_t *sym = &symbols[i];

        if (sym->type == STT_FILE || sym->type == STT_SECTION) {
            /* Map section symbols to COFF section symbols */
            if (sym->type == STT_SECTION && sym->shndx < e_shnum &&
                elf_sec_to_coff_sym[sym->shndx] >= 0) {
                elf_to_coff_sym[i] = elf_sec_to_coff_sym[sym->shndx];
            }
            continue;
        }
        if (sym->name[0] == '\0')
            continue;

        /* COFF uses _ prefix for C symbols */
        char coff_name[260];
        if (sym->name[0] != '.') {
            coff_name[0] = '_';
            strncpy(coff_name + 1, sym->name, sizeof(coff_name) - 2);
            coff_name[sizeof(coff_name) - 1] = '\0';
        } else {
            strncpy(coff_name, sym->name, sizeof(coff_name));
            coff_name[sizeof(coff_name) - 1] = '\0';
        }

        int coff_idx;
        GROW(coff_symbols, ncoff_sym, coff_sym_cap);
        coff_sym_entry_t *e = &coff_symbols[ncoff_sym];
        memset(e->data, 0, 18);

        /* Set name */
        size_t nlen = strlen(coff_name);
        if (nlen <= 8) {
            memcpy(e->data, coff_name, nlen);
        } else {
            /* Long name -> string table */
            uint32_t str_offset = strtab_len_out;
            uint32_t needed = strtab_len_out + nlen + 1;
            while (needed > strtab_cap_out) {
                strtab_cap_out *= 2;
                coff_strtab = realloc(coff_strtab, strtab_cap_out);
                if (!coff_strtab) { fprintf(stderr, "out of memory\n"); return 1; }
            }
            memcpy(coff_strtab + strtab_len_out, coff_name, nlen);
            coff_strtab[strtab_len_out + nlen] = '\0';
            strtab_len_out += nlen + 1;
            wr32(e->data, 0);
            wr32(e->data + 4, str_offset);
        }

        if (sym->shndx == SHN_UNDEF) {
            /* External undefined */
            wr32(e->data + 8, 0);
            wr16(e->data + 12, 0);
            wr16(e->data + 14, 0);
            e->data[16] = (uint8_t)C_EXT;
            e->data[17] = 0;
        } else if (sym->shndx == SHN_ABS) {
            wr32(e->data + 8, sym->value);
            wr16(e->data + 12, 0xFFFF);
            wr16(e->data + 14, 0);
            e->data[16] = (uint8_t)C_EXT;
            e->data[17] = 0;
        } else if (sym->shndx == SHN_COMMON) {
            /* Common symbol */
            wr32(e->data + 8, sym->size);
            wr16(e->data + 12, 0);
            wr16(e->data + 14, 0);
            e->data[16] = (uint8_t)C_EXT;
            e->data[17] = 0;
        } else if (sym->shndx < e_shnum && elf_sec_to_coff[sym->shndx] != 0) {
            int coff_scn = elf_sec_to_coff[sym->shndx];
            uint32_t value = sym->value;
            if (has_rodata_offset[sym->shndx]) {
                value += rodata_offsets[sym->shndx];
            } else if (coff_scn == 2) {
                value += data_addr;
            } else if (coff_scn == 3) {
                value += bss_addr;
            }
            uint8_t sclass = (sym->bind == STB_GLOBAL) ? C_EXT : C_STAT;
            wr32(e->data + 8, value);
            wr16(e->data + 12, (uint16_t)coff_scn);
            wr16(e->data + 14, 0);
            e->data[16] = sclass;
            e->data[17] = 0;
        } else {
            /* Unmapped section - skip */
            continue;
        }

        coff_idx = ncoff_sym;
        ncoff_sym++;
        elf_to_coff_sym[i] = coff_idx;
    }

    /* ---- Build COFF relocations ---- */
    coff_reloc_entry_t *text_relocs_out = NULL;
    int ntext_relocs = 0, text_relocs_cap = 0;

    coff_reloc_entry_t *data_relocs_out = NULL;
    int ndata_relocs = 0, data_relocs_cap = 0;

    for (int sec_idx = 0; sec_idx < e_shnum; sec_idx++) {
        if (relocs[sec_idx].count == 0)
            continue;

        const char *sec_name = sections[sec_idx].name;

        for (int j = 0; j < relocs[sec_idx].count; j++) {
            elf_reloc_t *r = &relocs[sec_idx].rels[j];
            uint16_t coff_rtype = elf_reltype_to_coff(r->type);
            elf_symbol_t *sym = &symbols[r->sym];

            int coff_sym_idx;
            if (elf_to_coff_sym[r->sym] >= 0) {
                coff_sym_idx = elf_to_coff_sym[r->sym];
            } else if (sym->type == STT_SECTION && sym->shndx < e_shnum &&
                       elf_sec_to_coff_sym[sym->shndx] >= 0) {
                coff_sym_idx = elf_sec_to_coff_sym[sym->shndx];
            } else {
                fprintf(stderr, "Warning: unmapped symbol %s (idx=%u), skipping reloc\n",
                        sym->name, r->sym);
                continue;
            }

            /* Apply addend (RELA -> REL conversion) */
            int32_t addend = r->addend;
            if (sym->type == STT_SECTION) {
                if (has_rodata_offset[sym->shndx]) {
                    addend += rodata_offsets[sym->shndx];
                } else if ((int)sym->shndx == data_idx && data_addr > 0) {
                    addend += (int32_t)data_addr;
                } else if ((int)sym->shndx == bss_idx && bss_addr > 0) {
                    addend += (int32_t)bss_addr;
                }
            }

            if (addend != 0) {
                uint32_t off = r->offset;
                if (strcmp(sec_name, ".text") == 0) {
                    if (coff_rtype == R_RELLONG || coff_rtype == R_PCRLONG) {
                        int32_t old_val = rd32s(text_data + off);
                        wr32(text_data + off, (uint32_t)(old_val + addend));
                    } else if (coff_rtype == R_RELWORD || coff_rtype == R_PCRWORD) {
                        int16_t old_val = rd16s(text_data + off);
                        wr16(text_data + off, (uint16_t)(int16_t)(old_val + addend));
                    }
                } else if (strcmp(sec_name, ".data") == 0) {
                    if (coff_rtype == R_RELLONG || coff_rtype == R_PCRLONG) {
                        int32_t old_val = rd32s(data_data + off);
                        wr32(data_data + off, (uint32_t)(old_val + addend));
                    }
                }
            }

            /* Build COFF reloc entry (10 bytes: 4 addr + 4 symndx + 2 type) */
            coff_reloc_entry_t cr;
            memset(cr.data, 0, 10);
            wr32(cr.data, r->offset);
            wr32(cr.data + 4, (uint32_t)coff_sym_idx);
            wr16(cr.data + 8, coff_rtype);

            if (strcmp(sec_name, ".text") == 0) {
                GROW(text_relocs_out, ntext_relocs, text_relocs_cap);
                text_relocs_out[ntext_relocs++] = cr;
            } else if (strcmp(sec_name, ".data") == 0) {
                GROW(data_relocs_out, ndata_relocs, data_relocs_cap);
                data_relocs_out[ndata_relocs++] = cr;
            }
        }
    }

    /* ---- Build COFF output file ---- */
    int nscns = 3;
    uint32_t headers_size = 20 + nscns * 40;
    uint32_t text_offset = headers_size;
    uint32_t data_offset = text_offset + text_len;
    uint32_t text_reloc_offset = data_offset + data_len;
    uint32_t data_reloc_offset = text_reloc_offset + (uint32_t)ntext_relocs * 10;
    uint32_t sym_offset = data_reloc_offset + (uint32_t)ndata_relocs * 10;

    /* Write output */
    FILE *out = fopen(coff_path, "wb");
    if (!out) {
        fprintf(stderr, "Cannot create %s\n", coff_path);
        return 1;
    }

    /* File header (20 bytes) */
    {
        uint8_t hdr[20];
        wr16(hdr + 0, MC68MAGIC);
        wr16(hdr + 2, (uint16_t)nscns);
        wr32(hdr + 4, 0);               /* timestamp */
        wr32(hdr + 8, sym_offset);
        wr32(hdr + 12, (uint32_t)ncoff_sym);
        wr16(hdr + 16, 0);              /* opthdr size */
        wr16(hdr + 18, F_LNNO | F_AR32WR);
        fwrite(hdr, 1, 20, out);
    }

    /* Section headers (40 bytes each) */
    /* .text */
    {
        uint8_t sh[40];
        memset(sh, 0, 40);
        memcpy(sh, ".text\0\0\0", 8);
        wr32(sh + 8, 0);            /* paddr */
        wr32(sh + 12, 0);           /* vaddr */
        wr32(sh + 16, text_len);    /* size */
        wr32(sh + 20, text_len ? text_offset : 0);
        wr32(sh + 24, ntext_relocs ? text_reloc_offset : 0);
        wr32(sh + 28, 0);           /* lnnoptr */
        wr16(sh + 32, (uint16_t)ntext_relocs);
        wr16(sh + 34, 0);           /* nlnno */
        wr32(sh + 36, STYP_TEXT);
        fwrite(sh, 1, 40, out);
    }

    /* .data */
    {
        uint8_t sh[40];
        memset(sh, 0, 40);
        memcpy(sh, ".data\0\0\0", 8);
        wr32(sh + 8, data_addr);
        wr32(sh + 12, data_addr);
        wr32(sh + 16, data_len);
        wr32(sh + 20, data_len ? data_offset : 0);
        wr32(sh + 24, ndata_relocs ? data_reloc_offset : 0);
        wr32(sh + 28, 0);
        wr16(sh + 32, (uint16_t)ndata_relocs);
        wr16(sh + 34, 0);
        wr32(sh + 36, STYP_DATA);
        fwrite(sh, 1, 40, out);
    }

    /* .bss */
    {
        uint8_t sh[40];
        memset(sh, 0, 40);
        memcpy(sh, ".bss\0\0\0\0", 8);
        wr32(sh + 8, bss_addr);
        wr32(sh + 12, bss_addr);
        wr32(sh + 16, bss_size);
        wr32(sh + 20, 0);
        wr32(sh + 24, 0);
        wr32(sh + 28, 0);
        wr16(sh + 32, 0);
        wr16(sh + 34, 0);
        wr32(sh + 36, STYP_BSS);
        fwrite(sh, 1, 40, out);
    }

    /* Section data */
    if (text_len > 0)
        fwrite(text_data, 1, text_len, out);
    if (data_len > 0)
        fwrite(data_data, 1, data_len, out);

    /* Relocations */
    for (int i = 0; i < ntext_relocs; i++)
        fwrite(text_relocs_out[i].data, 1, 10, out);
    for (int i = 0; i < ndata_relocs; i++)
        fwrite(data_relocs_out[i].data, 1, 10, out);

    /* Symbol table */
    for (int i = 0; i < ncoff_sym; i++)
        fwrite(coff_symbols[i].data, 1, 18, out);

    /* String table: 4-byte length prefix + rest of string data */
    {
        uint8_t stlen[4];
        wr32(stlen, strtab_len_out);
        fwrite(stlen, 1, 4, out);
        if (strtab_len_out > 4)
            fwrite(coff_strtab + 4, 1, strtab_len_out - 4, out);
    }

    fclose(out);

    /* Print stats */
    printf("Converted %s -> %s (%u bytes, %d symbols, %d relocations)\n",
           elf_path, coff_path, sym_offset + ncoff_sym * 18 + strtab_len_out,
           ncoff_sym, ntext_relocs + ndata_relocs);

    /* Cleanup */
    free(raw);
    free(sections);
    free(symbols);
    for (int i = 0; i < e_shnum; i++)
        free(relocs[i].rels);
    free(relocs);
    free(rodata_indices);
    free(text_data);
    free(data_data);
    free(rodata_offsets);
    free(has_rodata_offset);
    free(coff_symbols);
    free(coff_strtab);
    free(elf_to_coff_sym);
    free(elf_sec_to_coff);
    free(elf_sec_to_coff_sym);
    free(text_relocs_out);
    free(data_relocs_out);

    return 0;
}
