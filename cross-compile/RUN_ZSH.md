# Running Shells on the Plexus P/20 Emulator

## Quick Start

```bash
cd plexus_20_emu
./emu -u15 ../ROMs/U15-MERGED.BIN -u17 ../ROMs/U17-MERGED.BIN -hd ../disk/plexus-shells.img
```

Wait for the `#` prompt, then:

```bash
mount /dev/dk2 /user
/user/ash/ash
```

Or for zsh:

```bash
mount /dev/dk2 /user
/user/zsh/zsh
```

## Installed Shells (natively compiled)

| Shell | Path | Size | Notes |
|-------|------|------|-------|
| ash (Almquist 1989) | `/user/ash/ash` | 80KB | Lightweight, fast |
| zsh 2.2 | `/user/zsh/zsh` | 318KB | Line editing, history |
| sh (Bourne) | `/bin/sh` | built-in | System default |

## Exiting

- Exit a shell: `exit` or Ctrl-D
- Exit the emulator: Ctrl-C three times
