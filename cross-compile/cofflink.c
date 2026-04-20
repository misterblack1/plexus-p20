/*
 * COFF linker for Motorola 68010 (Plexus P/20).
 * Links COFF .o files with Plexus libc.a to produce an executable.
 *
 * Usage: cofflink -o output crt0.o obj1.o obj2.o -L/path -lc -ltermlib
 *
 * Compile: cc -O -Wall -o cofflink cofflink.c
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#define GROW(arr, cnt, cap) do {                \
    if ((cnt) >= (cap)) {                       \
        (cap) = (cap) ? (cap) * 2 : 16;        \
        (arr) = realloc((arr), (cap) * sizeof(*(arr))); \
        if (!(arr)) { fprintf(stderr, "out of memory\n"); exit(1); } \
    }                                           \
} while (0)

/* ---- COFF Constants ---- */

#define MC68MAGIC   0x0150
#define F_RELFLG    0x0001
#define F_EXEC      0x0002
#define F_AR32W     0x0200

#define STYP_TEXT   0x0020
#define STYP_DATA   0x0040
#define STYP_BSS    0x0080

#define C_EXT       2
#define C_STAT      3

#define R_RELLONG   0x0011
#define R_PCRLONG   0x0014
#define R_RELWORD   0x0010
#define R_PCRWORD   0x0013

#define PAGE_SIZE   0x1000

/* ---- Data structures ---- */

typedef struct {
    int32_t rvaddr;
    int32_t rsymndx;
    uint16_t rtype;
} Reloc;

typedef struct {
    char name[9];
    int32_t paddr;
    int32_t vaddr;
    int32_t size;
    int32_t scnptr;
    int32_t relptr;
    int32_t lnnoptr;
    uint16_t nreloc;
    uint16_t nlnno;
    int32_t flags;
    uint8_t *data;      /* mutable copy of section data */
    Reloc *relocs;
    int reloc_count;
} Section;

typedef struct {
    char *name;
    int32_t value;
    uint16_t scnum;
    uint16_t stype;
    int8_t sclass;
    uint8_t numaux;
    int index;
} Symbol;

typedef struct {
    char *obj_name;
    uint16_t magic;
    uint16_t nscns;
    int32_t symptr;
    int32_t nsyms;
    uint16_t opthdr;
    uint16_t flags;
    Section *sections;
    int section_count;
    Symbol *symbols;
    int symbol_count;
    int native_coff;
} CoffObj;

/* ---- Defined symbol entry ---- */
typedef struct {
    char *name;
    int obj_idx;
    uint16_t scnum;
    int32_t value;
} DefinedSym;

/* ---- Undefined symbol list ---- */
typedef struct {
    char **names;
    int count;
    int cap;
} UndefList;

/* ---- Common symbol entry ---- */
typedef struct {
    char *name;
    int32_t size;
} CommonSym;

/* ---- Symbol value entry ---- */
typedef struct {
    char *name;
    int32_t value;
} SymValue;

/* ---- Section part for layout ---- */
typedef struct {
    int obj_idx;
    Section *sec;
    int32_t base_offset;
} SectionPart;

/* ---- Per-object base offsets ---- */
typedef struct {
    int obj_idx;
    int32_t base;
} ObjBase;

/* ---- Library (archive) member ---- */
typedef struct {
    char *name;
    CoffObj *obj;
    int used;   /* set to 1 once pulled in */
} LibMember;

typedef struct {
    LibMember *members;
    int count;
    int cap;
} Library;

/* ---- Auto BSS entry ---- */
typedef struct {
    const char *name;
    int32_t size;
} AutoBssEntry;

/* ---- Helpers for string sets/maps ---- */

static int undef_contains(UndefList *u, const char *name) {
    for (int i = 0; i < u->count; i++)
        if (strcmp(u->names[i], name) == 0) return 1;
    return 0;
}

static void undef_add(UndefList *u, const char *name) {
    if (undef_contains(u, name)) return;
    GROW(u->names, u->count, u->cap);
    u->names[u->count++] = strdup(name);
}

static void undef_remove(UndefList *u, const char *name) {
    for (int i = 0; i < u->count; i++) {
        if (strcmp(u->names[i], name) == 0) {
            free(u->names[i]);
            u->names[i] = u->names[--u->count];
            return;
        }
    }
}

static DefinedSym *defined_find(DefinedSym *defs, int count, const char *name) {
    for (int i = 0; i < count; i++)
        if (strcmp(defs[i].name, name) == 0) return &defs[i];
    return NULL;
}

static CommonSym *common_find(CommonSym *coms, int count, const char *name) {
    for (int i = 0; i < count; i++)
        if (strcmp(coms[i].name, name) == 0) return &coms[i];
    return NULL;
}

static SymValue *symval_find(SymValue *sv, int count, const char *name) {
    for (int i = 0; i < count; i++)
        if (strcmp(sv[i].name, name) == 0) return &sv[i];
    return NULL;
}

static ObjBase *objbase_find(ObjBase *bases, int count, int obj_idx) {
    for (int i = 0; i < count; i++)
        if (bases[i].obj_idx == obj_idx) return &bases[i];
    return NULL;
}

static int32_t objbase_get(ObjBase *bases, int count, int obj_idx) {
    ObjBase *b = objbase_find(bases, count, obj_idx);
    return b ? b->base : 0;
}

static void common_remove(CommonSym **coms, int *count, const char *name) {
    for (int i = 0; i < *count; i++) {
        if (strcmp((*coms)[i].name, name) == 0) {
            free((*coms)[i].name);
            (*coms)[i] = (*coms)[--(*count)];
            return;
        }
    }
}

/* ---- Read entire file into buffer ---- */

static uint8_t *read_file(const char *path, size_t *out_size) {
    FILE *f = fopen(path, "rb");
    if (!f) { fprintf(stderr, "Cannot open %s\n", path); exit(1); }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t *buf = malloc(sz);
    if (!buf) { fprintf(stderr, "out of memory\n"); exit(1); }
    if ((long)fread(buf, 1, sz, f) != sz) {
        fprintf(stderr, "Read error: %s\n", path);
        exit(1);
    }
    fclose(f);
    *out_size = (size_t)sz;
    return buf;
}

/* ---- Extract basename from path ---- */

static const char *my_basename(const char *path) {
    const char *p = strrchr(path, '/');
    return p ? p + 1 : path;
}

/* ---- Parse a COFF object from raw data ---- */

static CoffObj *parse_coff(const char *name, const uint8_t *data, size_t datalen) {
    if (datalen < 20) return NULL;
    uint16_t magic = rd16(data);
    if (magic != MC68MAGIC) {
        fprintf(stderr, "%s: not MC68MAGIC (got 0x%04x)\n", name, magic);
        return NULL;
    }

    CoffObj *obj = calloc(1, sizeof(CoffObj));
    obj->obj_name = strdup(name);
    obj->magic = magic;
    obj->nscns = rd16(data + 2);
    obj->symptr = rd32s(data + 8);
    obj->nsyms = rd32s(data + 12);
    obj->opthdr = rd16(data + 16);
    obj->flags = rd16(data + 18);

    /* Parse sections */
    obj->section_count = obj->nscns;
    obj->sections = calloc(obj->nscns, sizeof(Section));
    int off = 20 + obj->opthdr;

    for (int i = 0; i < obj->nscns; i++) {
        Section *sec = &obj->sections[i];
        const uint8_t *s = data + off;

        memset(sec->name, 0, 9);
        memcpy(sec->name, s, 8);
        /* strip trailing nulls for comparison */
        for (int k = 7; k >= 0 && sec->name[k] == '\0'; k--) {}

        sec->paddr   = rd32s(s + 8);
        sec->vaddr   = rd32s(s + 12);
        sec->size    = rd32s(s + 16);
        sec->scnptr  = rd32s(s + 20);
        sec->relptr  = rd32s(s + 24);
        sec->lnnoptr = rd32s(s + 28);
        sec->nreloc  = rd16(s + 32);
        sec->nlnno   = rd16(s + 34);
        sec->flags   = rd32s(s + 36);

        /* Copy section data */
        if (sec->scnptr && sec->size > 0) {
            sec->data = calloc(sec->size, 1);
            int32_t copylen = sec->size;
            if (sec->scnptr + copylen > (int32_t)datalen)
                copylen = (int32_t)datalen - sec->scnptr;
            if (copylen > 0)
                memcpy(sec->data, data + sec->scnptr, copylen);
        } else {
            sec->data = calloc(sec->size > 0 ? sec->size : 1, 1);
        }

        /* Parse relocations */
        sec->reloc_count = 0;
        sec->relocs = NULL;
        if (sec->nreloc && sec->relptr) {
            sec->relocs = calloc(sec->nreloc, sizeof(Reloc));
            sec->reloc_count = sec->nreloc;
            for (int j = 0; j < sec->nreloc; j++) {
                int roff = sec->relptr + j * 10;
                sec->relocs[j].rvaddr  = rd32s(data + roff);
                sec->relocs[j].rsymndx = rd32s(data + roff + 4);
                sec->relocs[j].rtype   = rd16(data + roff + 8);
            }
        }

        off += 40;
    }

    /* Parse symbols */
    int32_t strtab_start = obj->symptr + obj->nsyms * 18;
    obj->symbols = NULL;
    obj->symbol_count = 0;
    int sym_cap = 0;

    int si = 0;
    while (si < obj->nsyms) {
        int sym_off = obj->symptr + si * 18;
        if (sym_off + 18 > (int32_t)datalen) break;
        const uint8_t *sb = data + sym_off;

        GROW(obj->symbols, obj->symbol_count, sym_cap);
        Symbol *sym = &obj->symbols[obj->symbol_count];

        /* Parse name */
        if (sb[0] == 0 && sb[1] == 0 && sb[2] == 0 && sb[3] == 0) {
            int32_t so = rd32s(sb + 4);
            if (strtab_start + so < (int32_t)datalen) {
                /* Find null terminator */
                int end = strtab_start + so;
                while (end < (int32_t)datalen && data[end] != 0) end++;
                int len = end - (strtab_start + so);
                sym->name = malloc(len + 1);
                memcpy(sym->name, data + strtab_start + so, len);
                sym->name[len] = '\0';
            } else {
                sym->name = strdup("<bad>");
            }
        } else {
            /* Inline name, up to 8 chars */
            int len = 0;
            while (len < 8 && sb[len] != 0) len++;
            sym->name = malloc(len + 1);
            memcpy(sym->name, sb, len);
            sym->name[len] = '\0';
        }

        sym->value  = rd32s(sb + 8);
        sym->scnum  = rd16(sb + 12);
        sym->stype  = rd16(sb + 14);
        sym->sclass = (int8_t)sb[16];
        sym->numaux = sb[17];
        sym->index  = si;

        obj->symbol_count++;
        si += 1 + sym->numaux;
    }

    return obj;
}

/* ---- Parse archive ---- */

static void parse_archive(const char *path, Library *lib) {
    size_t datalen;
    uint8_t *data = read_file(path, &datalen);

    if (datalen < 8 || memcmp(data, "!<arch>\n", 8) != 0) {
        fprintf(stderr, "%s: not an ar archive\n", path);
        free(data);
        return;
    }

    size_t off = 8;
    while (off + 60 <= datalen) {
        const uint8_t *hdr = data + off;

        /* Parse member name */
        char mname[17];
        memcpy(mname, hdr, 16);
        mname[16] = '\0';
        /* Strip trailing spaces and slash */
        int end = 15;
        while (end >= 0 && (mname[end] == ' ' || mname[end] == '/')) {
            mname[end] = '\0';
            end--;
        }

        /* Parse size */
        char szbuf[11];
        memcpy(szbuf, hdr + 48, 10);
        szbuf[10] = '\0';
        long size = strtol(szbuf, NULL, 10);

        size_t moff = off + 60;
        if (moff + 2 <= datalen && (size_t)size >= 2) {
            uint16_t magic = rd16(data + moff);
            if (magic == MC68MAGIC) {
                CoffObj *obj = parse_coff(mname, data + moff,
                    ((size_t)size <= datalen - moff) ? (size_t)size : datalen - moff);
                if (obj) {
                    GROW(lib->members, lib->count, lib->cap);
                    lib->members[lib->count].name = strdup(mname);
                    lib->members[lib->count].obj = obj;
                    lib->members[lib->count].used = 0;
                    lib->count++;
                }
            }
        }

        off = moff + (size_t)size;
        if (off % 2) off++;
    }

    free(data);
}

/* ---- Main linker ---- */

int main(int argc, char **argv) {
    const char *output = "a.out";
    const char **obj_files = NULL;
    int obj_file_count = 0, obj_file_cap = 0;
    const char **lib_paths = NULL;
    int lib_path_count = 0, lib_path_cap = 0;
    const char **lib_names = NULL;
    int lib_name_count = 0, lib_name_cap = 0;

    /* Default library search path */
    GROW(lib_paths, lib_path_count, lib_path_cap);
    lib_paths[lib_path_count++] = "/tmp/plexus_root/root/lib";

    /* Parse arguments */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output = argv[++i];
        } else if (strncmp(argv[i], "-L", 2) == 0) {
            GROW(lib_paths, lib_path_count, lib_path_cap);
            lib_paths[lib_path_count++] = argv[i] + 2;
        } else if (strncmp(argv[i], "-l", 2) == 0) {
            GROW(lib_names, lib_name_count, lib_name_cap);
            lib_names[lib_name_count++] = argv[i] + 2;
        } else {
            size_t len = strlen(argv[i]);
            if (len >= 2 && strcmp(argv[i] + len - 2, ".o") == 0) {
                GROW(obj_files, obj_file_count, obj_file_cap);
                obj_files[obj_file_count++] = argv[i];
            } else {
                fprintf(stderr, "Unknown arg: %s\n", argv[i]);
            }
        }
    }

    /* Resolve library paths */
    char **libraries = NULL;
    int lib_count = 0, lib_cap = 0;
    for (int i = 0; i < lib_name_count; i++) {
        int found = 0;
        for (int j = 0; j < lib_path_count; j++) {
            char cand[4096];
            snprintf(cand, sizeof(cand), "%s/lib%s.a", lib_paths[j], lib_names[i]);
            FILE *f = fopen(cand, "rb");
            if (f) {
                fclose(f);
                GROW(libraries, lib_count, lib_cap);
                libraries[lib_count++] = strdup(cand);
                found = 1;
                break;
            }
        }
        if (!found) {
            fprintf(stderr, "Warning: library -l%s not found\n", lib_names[i]);
        }
    }

    /* Phase 1: Load input objects */
    CoffObj **objects = NULL;
    int obj_count = 0, obj_cap = 0;
    int input_obj_count;

    for (int i = 0; i < obj_file_count; i++) {
        size_t datalen;
        uint8_t *data = read_file(obj_files[i], &datalen);
        CoffObj *obj = parse_coff(my_basename(obj_files[i]), data, datalen);
        if (!obj) {
            fprintf(stderr, "Failed to parse %s\n", obj_files[i]);
            exit(1);
        }
        /* native_coff if "crt0" in path */
        obj->native_coff = (strstr(obj_files[i], "crt0") != NULL) ? 1 : 0;
        GROW(objects, obj_count, obj_cap);
        objects[obj_count++] = obj;
        free(data);
    }
    input_obj_count = obj_count;

    /* Parse library archives */
    Library *lib_archives = calloc(lib_count > 0 ? lib_count : 1, sizeof(Library));
    for (int i = 0; i < lib_count; i++) {
        parse_archive(libraries[i], &lib_archives[i]);
    }

    /* Phase 2: Collect defined, undefined, common symbols */
    DefinedSym *defined = NULL;
    int def_count = 0, def_cap = 0;
    UndefList undefined = {NULL, 0, 0};
    CommonSym *common_syms = NULL;
    int com_count = 0, com_cap = 0;

    for (int idx = 0; idx < obj_count; idx++) {
        CoffObj *obj = objects[idx];
        for (int si = 0; si < obj->symbol_count; si++) {
            Symbol *sym = &obj->symbols[si];
            if (sym->sclass == C_EXT) {
                if (sym->scnum == 0) {
                    if (sym->value > 0) {
                        /* Common symbol */
                        if (!defined_find(defined, def_count, sym->name)) {
                            CommonSym *cs = common_find(common_syms, com_count, sym->name);
                            if (cs) {
                                if (sym->value > cs->size)
                                    cs->size = sym->value;
                            } else {
                                GROW(common_syms, com_count, com_cap);
                                common_syms[com_count].name = strdup(sym->name);
                                common_syms[com_count].size = sym->value;
                                com_count++;
                            }
                        }
                    } else {
                        if (!defined_find(defined, def_count, sym->name) &&
                            !common_find(common_syms, com_count, sym->name)) {
                            undef_add(&undefined, sym->name);
                        }
                    }
                } else {
                    /* Defined symbol */
                    DefinedSym *existing = defined_find(defined, def_count, sym->name);
                    if (existing) {
                        /* Update */
                        existing->obj_idx = idx;
                        existing->scnum = sym->scnum;
                        existing->value = sym->value;
                    } else {
                        GROW(defined, def_count, def_cap);
                        defined[def_count].name = strdup(sym->name);
                        defined[def_count].obj_idx = idx;
                        defined[def_count].scnum = sym->scnum;
                        defined[def_count].value = sym->value;
                        def_count++;
                    }
                    undef_remove(&undefined, sym->name);
                    common_remove(&common_syms, &com_count, sym->name);
                }
            }
        }
    }

    /* Phase 3: Resolve undefined symbols from libraries */
    int changed = 1;
    while (changed && undefined.count > 0) {
        changed = 0;
        for (int li = 0; li < lib_count; li++) {
            Library *lib = &lib_archives[li];
            for (int mi = 0; mi < lib->count; mi++) {
                if (lib->members[mi].used) continue;
                CoffObj *mobj = lib->members[mi].obj;

                /* Check if this member provides any undefined symbol */
                int provides = 0;
                for (int si = 0; si < mobj->symbol_count; si++) {
                    Symbol *sym = &mobj->symbols[si];
                    if (sym->sclass == C_EXT && (sym->scnum != 0 || sym->value > 0)) {
                        if (undef_contains(&undefined, sym->name)) {
                            provides = 1;
                            break;
                        }
                    }
                }

                if (!provides) continue;

                /* Pull in this library member */
                lib->members[mi].used = 1;
                mobj->native_coff = 1;
                int idx = obj_count;
                GROW(objects, obj_count, obj_cap);
                objects[obj_count++] = mobj;

                for (int si = 0; si < mobj->symbol_count; si++) {
                    Symbol *sym = &mobj->symbols[si];
                    if (sym->sclass == C_EXT) {
                        if (sym->scnum != 0) {
                            DefinedSym *existing = defined_find(defined, def_count, sym->name);
                            if (existing) {
                                existing->obj_idx = idx;
                                existing->scnum = sym->scnum;
                                existing->value = sym->value;
                            } else {
                                GROW(defined, def_count, def_cap);
                                defined[def_count].name = strdup(sym->name);
                                defined[def_count].obj_idx = idx;
                                defined[def_count].scnum = sym->scnum;
                                defined[def_count].value = sym->value;
                                def_count++;
                            }
                            undef_remove(&undefined, sym->name);
                            common_remove(&common_syms, &com_count, sym->name);
                        } else if (sym->value > 0) {
                            /* Common symbol from library */
                            if (!defined_find(defined, def_count, sym->name)) {
                                CommonSym *cs = common_find(common_syms, com_count, sym->name);
                                if (cs) {
                                    if (sym->value > cs->size)
                                        cs->size = sym->value;
                                } else {
                                    GROW(common_syms, com_count, com_cap);
                                    common_syms[com_count].name = strdup(sym->name);
                                    common_syms[com_count].size = sym->value;
                                    com_count++;
                                }
                                undef_remove(&undefined, sym->name);
                            }
                        } else {
                            if (!defined_find(defined, def_count, sym->name) &&
                                !common_find(common_syms, com_count, sym->name)) {
                                undef_add(&undefined, sym->name);
                            }
                        }
                    }
                }
                changed = 1;
            }
        }
    }

    /* Auto-define common undefined symbols */
    static const AutoBssEntry common_bss[] = {
        {"_errno", 4}, {"__sibuf", 1024}, {"__smbuf", 1024}, {"__sobuf", 1024},
        {"__sigfunc", 128}, {"_COLS", 4}, {"_LINES", 4}, {"_SP", 4},
        {"_curscr", 4}, {"_stdscr", 4}, {"_lwin", 4}, {"_ttytype", 20},
        {"__first_term", 4},
    };
    static const char *linker_syms[] = {"_end", "_etext", "_edata"};

    /* auto_bss: symbols that need BSS allocation */
    CommonSym *auto_bss = NULL;
    int auto_bss_count = 0, auto_bss_cap = 0;

    for (int i = undefined.count - 1; i >= 0; i--) {
        const char *sname = undefined.names[i];
        /* Check linker syms */
        int is_linker = 0;
        for (int j = 0; j < 3; j++) {
            if (strcmp(sname, linker_syms[j]) == 0) {
                is_linker = 1;
                break;
            }
        }
        if (is_linker) {
            undef_remove(&undefined, sname);
            continue;
        }
        /* Check common_bss */
        for (int j = 0; j < (int)(sizeof(common_bss)/sizeof(common_bss[0])); j++) {
            if (strcmp(sname, common_bss[j].name) == 0) {
                GROW(auto_bss, auto_bss_count, auto_bss_cap);
                auto_bss[auto_bss_count].name = strdup(common_bss[j].name);
                auto_bss[auto_bss_count].size = common_bss[j].size;
                auto_bss_count++;
                undef_remove(&undefined, sname);
                break;
            }
        }
    }

    if (undefined.count > 0) {
        fprintf(stderr, "Warning: %d undefined symbols:\n", undefined.count);
        for (int i = 0; i < undefined.count; i++)
            fprintf(stderr, "  %s\n", undefined.names[i]);
    }

    /* Phase 4: Layout sections */
    SectionPart *text_parts = NULL;
    int text_part_count = 0, text_part_cap = 0;
    SectionPart *data_parts = NULL;
    int data_part_count = 0, data_part_cap = 0;
    SectionPart *bss_parts = NULL;
    int bss_part_count = 0, bss_part_cap = 0;

    ObjBase *obj_text_base = NULL;
    int otb_count = 0, otb_cap = 0;
    ObjBase *obj_data_base = NULL;
    int odb_count = 0, odb_cap = 0;
    ObjBase *obj_bss_base = NULL;
    int obb_count = 0, obb_cap = 0;

    int32_t text_start = 0x0;
    int32_t text_offset = 0;
    int32_t data_offset = 0;
    int32_t bss_offset = 0;

    for (int idx = 0; idx < obj_count; idx++) {
        CoffObj *obj = objects[idx];
        for (int si = 0; si < obj->section_count; si++) {
            Section *sec = &obj->sections[si];
            if (sec->flags & STYP_TEXT) {
                while (text_offset % 2) text_offset++;
                GROW(obj_text_base, otb_count, otb_cap);
                obj_text_base[otb_count].obj_idx = idx;
                obj_text_base[otb_count].base = text_offset;
                otb_count++;
                GROW(text_parts, text_part_count, text_part_cap);
                text_parts[text_part_count].obj_idx = idx;
                text_parts[text_part_count].sec = sec;
                text_parts[text_part_count].base_offset = text_offset;
                text_part_count++;
                text_offset += sec->size;
            } else if (sec->flags & STYP_DATA) {
                while (data_offset % 2) data_offset++;
                GROW(obj_data_base, odb_count, odb_cap);
                obj_data_base[odb_count].obj_idx = idx;
                obj_data_base[odb_count].base = data_offset;
                odb_count++;
                GROW(data_parts, data_part_count, data_part_cap);
                data_parts[data_part_count].obj_idx = idx;
                data_parts[data_part_count].sec = sec;
                data_parts[data_part_count].base_offset = data_offset;
                data_part_count++;
                data_offset += sec->size;
            } else if (sec->flags & STYP_BSS) {
                while (bss_offset % 2) bss_offset++;
                GROW(obj_bss_base, obb_count, obb_cap);
                obj_bss_base[obb_count].obj_idx = idx;
                obj_bss_base[obb_count].base = bss_offset;
                obb_count++;
                GROW(bss_parts, bss_part_count, bss_part_cap);
                bss_parts[bss_part_count].obj_idx = idx;
                bss_parts[bss_part_count].sec = sec;
                bss_parts[bss_part_count].base_offset = bss_offset;
                bss_part_count++;
                bss_offset += sec->size;
            }
        }
    }

    /* Pad text to page boundary */
    int32_t padded_text = (text_offset + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    int32_t total_text = padded_text;
    int32_t data_start_addr = text_start + total_text;
    int32_t total_data = data_offset;
    int32_t bss_start_addr = data_start_addr + total_data;
    int32_t total_bss = bss_offset;

    /* Build merged section data */
    uint8_t *merged_text = calloc(total_text > 0 ? total_text : 1, 1);
    uint8_t *merged_data = calloc(total_data > 0 ? total_data : 1, 1);

    for (int i = 0; i < text_part_count; i++) {
        SectionPart *sp = &text_parts[i];
        memcpy(merged_text + sp->base_offset, sp->sec->data, sp->sec->size);
    }
    for (int i = 0; i < data_part_count; i++) {
        SectionPart *sp = &data_parts[i];
        memcpy(merged_data + sp->base_offset, sp->sec->data, sp->sec->size);
    }

    /* Phase 5: Resolve symbol values */
    SymValue *sym_values = NULL;
    int sv_count = 0, sv_cap = 0;

    for (int i = 0; i < def_count; i++) {
        DefinedSym *d = &defined[i];
        CoffObj *obj = objects[d->obj_idx];
        Section *sec = &obj->sections[d->scnum - 1];
        int32_t section_offset = d->value - sec->paddr;
        int32_t base = 0;
        if (sec->flags & STYP_TEXT) {
            base = text_start + objbase_get(obj_text_base, otb_count, d->obj_idx);
        } else if (sec->flags & STYP_DATA) {
            base = data_start_addr + objbase_get(obj_data_base, odb_count, d->obj_idx);
        } else if (sec->flags & STYP_BSS) {
            base = bss_start_addr + objbase_get(obj_bss_base, obb_count, d->obj_idx);
        }
        GROW(sym_values, sv_count, sv_cap);
        sym_values[sv_count].name = strdup(d->name);
        sym_values[sv_count].value = base + section_offset;
        sv_count++;
    }

    /* Allocate common symbols in BSS */
    for (int i = 0; i < com_count; i++) {
        if (symval_find(sym_values, sv_count, common_syms[i].name)) continue;
        while (total_bss % 4) total_bss++;
        GROW(sym_values, sv_count, sv_cap);
        sym_values[sv_count].name = strdup(common_syms[i].name);
        sym_values[sv_count].value = bss_start_addr + total_bss;
        sv_count++;
        total_bss += common_syms[i].size;
    }

    /* Allocate auto BSS symbols */
    for (int i = 0; i < auto_bss_count; i++) {
        while (total_bss % 4) total_bss++;
        GROW(sym_values, sv_count, sv_cap);
        sym_values[sv_count].name = strdup(auto_bss[i].name);
        sym_values[sv_count].value = bss_start_addr + total_bss;
        sv_count++;
        total_bss += auto_bss[i].size;
    }

    /* Add linker-provided symbols */
    GROW(sym_values, sv_count, sv_cap);
    sym_values[sv_count].name = strdup("_etext");
    sym_values[sv_count].value = text_start + total_text;
    sv_count++;

    GROW(sym_values, sv_count, sv_cap);
    sym_values[sv_count].name = strdup("_edata");
    sym_values[sv_count].value = data_start_addr + total_data;
    sv_count++;

    GROW(sym_values, sv_count, sv_cap);
    sym_values[sv_count].name = strdup("_end");
    sym_values[sv_count].value = bss_start_addr + total_bss;
    sv_count++;

    /* Phase 6: Apply relocations */
    for (int idx = 0; idx < obj_count; idx++) {
        CoffObj *obj = objects[idx];
        for (int si = 0; si < obj->section_count; si++) {
            Section *sec = &obj->sections[si];
            uint8_t *target;
            int32_t sec_base, target_base;
            int32_t target_len;

            if (sec->flags & STYP_TEXT) {
                sec_base = text_start + objbase_get(obj_text_base, otb_count, idx);
                target = merged_text;
                target_base = objbase_get(obj_text_base, otb_count, idx);
                target_len = total_text;
            } else if (sec->flags & STYP_DATA) {
                sec_base = data_start_addr + objbase_get(obj_data_base, odb_count, idx);
                target = merged_data;
                target_base = objbase_get(obj_data_base, odb_count, idx);
                target_len = total_data;
            } else {
                continue;
            }

            for (int ri = 0; ri < sec->reloc_count; ri++) {
                Reloc *r = &sec->relocs[ri];
                int32_t rvaddr = r->rvaddr;
                int32_t rsymndx = r->rsymndx;
                uint16_t rtype = r->rtype;

                /* Find symbol by raw index */
                Symbol *sym = NULL;
                for (int k = 0; k < obj->symbol_count; k++) {
                    if (obj->symbols[k].index == rsymndx) {
                        sym = &obj->symbols[k];
                        break;
                    }
                }
                if (!sym) continue;

                /* Adjust rvaddr for native vs cross-compiled */
                int32_t rvaddr_adj;
                if (obj->native_coff) {
                    rvaddr_adj = rvaddr - sec->paddr;
                } else {
                    rvaddr_adj = rvaddr;
                }

                /* Get symbol value */
                int32_t sym_val;
                if (sym->sclass == C_EXT) {
                    SymValue *sv = symval_find(sym_values, sv_count, sym->name);
                    if (sv) {
                        sym_val = sv->value;
                    } else {
                        sym_val = 0;
                    }
                } else if (sym->sclass == C_STAT) {
                    Section *ssec = (sym->scnum > 0) ? &obj->sections[sym->scnum - 1] : NULL;
                    int32_t paddr = ssec ? ssec->paddr : 0;
                    if (ssec && (ssec->flags & STYP_TEXT)) {
                        sym_val = text_start + objbase_get(obj_text_base, otb_count, idx) - paddr;
                    } else if (ssec && (ssec->flags & STYP_DATA)) {
                        sym_val = data_start_addr + objbase_get(obj_data_base, odb_count, idx) - paddr;
                    } else if (ssec && (ssec->flags & STYP_BSS)) {
                        sym_val = bss_start_addr + objbase_get(obj_bss_base, obb_count, idx) - paddr;
                    } else {
                        sym_val = sym->value;
                    }
                } else {
                    sym_val = sym->value;
                }

                /* Apply relocation */
                int32_t abs_off = target_base + rvaddr_adj;
                if (abs_off < 0 || abs_off + 4 > target_len) continue;

                if (rtype == R_RELLONG) {
                    int32_t old = rd32s(target + abs_off);
                    wr32(target + abs_off, (uint32_t)(old + sym_val));
                } else if (rtype == R_PCRLONG) {
                    int32_t old = rd32s(target + abs_off);
                    int32_t pc = sec_base + rvaddr_adj;
                    wr32(target + abs_off, (uint32_t)(old + sym_val - pc));
                } else if (rtype == R_RELWORD) {
                    int16_t old = rd16s(target + abs_off);
                    wr16(target + abs_off, (uint16_t)(int16_t)(old + sym_val));
                } else if (rtype == R_PCRWORD) {
                    int16_t old = rd16s(target + abs_off);
                    int32_t pc = sec_base + rvaddr_adj;
                    wr16(target + abs_off, (uint16_t)(int16_t)(old + sym_val - pc));
                }
            }
        }
    }

    /* Phase 7: Write output COFF executable */
    int nscns = 3;
    int opthdr_size = 28;
    int headers_size = 20 + opthdr_size + nscns * 40;

    /* Find entry point */
    SymValue *entry_sv = symval_find(sym_values, sv_count, "start");
    if (!entry_sv)
        entry_sv = symval_find(sym_values, sv_count, "_start");
    int32_t entry = entry_sv ? entry_sv->value : text_start;

    FILE *out = fopen(output, "wb");
    if (!out) {
        fprintf(stderr, "Cannot create %s\n", output);
        exit(1);
    }

    /* File header */
    uint8_t fhdr[20];
    wr16(fhdr + 0, MC68MAGIC);
    wr16(fhdr + 2, (uint16_t)nscns);
    wr32(fhdr + 4, 0);                          /* timdat */
    wr32(fhdr + 8, 0);                          /* symptr */
    wr32(fhdr + 12, 0);                         /* nsyms */
    wr16(fhdr + 16, (uint16_t)opthdr_size);
    wr16(fhdr + 18, F_RELFLG | F_EXEC | F_AR32W);
    fwrite(fhdr, 1, 20, out);

    /* Optional (a.out) header */
    uint8_t ohdr[28];
    wr16(ohdr + 0, 0410);      /* NMAGIC - octal 0410 = 0x108 */
    wr16(ohdr + 2, 0);         /* vstamp */
    wr32(ohdr + 4, (uint32_t)total_text);
    wr32(ohdr + 8, (uint32_t)total_data);
    wr32(ohdr + 12, (uint32_t)total_bss);
    wr32(ohdr + 16, (uint32_t)entry);
    wr32(ohdr + 20, (uint32_t)text_start);
    wr32(ohdr + 24, (uint32_t)data_start_addr);
    fwrite(ohdr, 1, 28, out);

    /* Section headers */
    for (int i = 0; i < 3; i++) {
        uint8_t shdr[40];
        memset(shdr, 0, 40);
        const char *sname;
        int32_t paddr, vaddr, sz, scnptr, flags;

        if (i == 0) {
            sname = ".text";
            paddr = text_start; vaddr = text_start;
            sz = total_text; scnptr = headers_size; flags = STYP_TEXT;
        } else if (i == 1) {
            sname = ".data";
            paddr = data_start_addr; vaddr = data_start_addr;
            sz = total_data; scnptr = headers_size + total_text; flags = STYP_DATA;
        } else {
            sname = ".bss";
            paddr = bss_start_addr; vaddr = bss_start_addr;
            sz = total_bss; scnptr = 0; flags = STYP_BSS;
        }

        size_t nlen = strlen(sname);
        memcpy(shdr, sname, nlen);
        wr32(shdr + 8, (uint32_t)paddr);
        wr32(shdr + 12, (uint32_t)vaddr);
        wr32(shdr + 16, (uint32_t)sz);
        wr32(shdr + 20, (uint32_t)scnptr);
        wr32(shdr + 24, 0);    /* relptr */
        wr32(shdr + 28, 0);    /* lnnoptr */
        wr16(shdr + 32, 0);    /* nreloc */
        wr16(shdr + 34, 0);    /* nlnno */
        wr32(shdr + 36, (uint32_t)flags);
        fwrite(shdr, 1, 40, out);
    }

    /* Write section data */
    fwrite(merged_text, 1, total_text, out);
    fwrite(merged_data, 1, total_data, out);

    fclose(out);

    /* Report */
    /* Get output file size */
    FILE *chk = fopen(output, "rb");
    fseek(chk, 0, SEEK_END);
    long outsize = ftell(chk);
    fclose(chk);

    printf("Linked %s: %ld bytes\n", output, outsize);
    printf("  .text: %d bytes at 0x%x\n", total_text, text_start);
    printf("  .data: %d bytes at 0x%x\n", total_data, data_start_addr);
    printf("  .bss:  %d bytes at 0x%x\n", total_bss, bss_start_addr);
    printf("  entry: 0x%x\n", entry);
    printf("  objects: %d (%d input + %d from libs)\n",
           obj_count, input_obj_count, obj_count - input_obj_count);
    if (undefined.count > 0)
        printf("  UNRESOLVED: %d symbols\n", undefined.count);

    return undefined.count ? 1 : 0;
}
