## Plexus P/20 Boot Menu

After the power on self tests (successfully) complete (`SELFTEST
COMPLETE`), the Plexus P/20 ROMs will go to a (command line) boot menu:

```
PLEXUS PRIMARY BOOT REV 1.2
:
```

which accepts commands to configure and boot the system into Unix.

(If the selftest does not successfuly complete, then `*` can be used
to exit from the selftest failed state to the boot menu.  This may
be required when, eg, the clock configuration RAM is invalid and is
referencing devices which do not exist.)

### Boot Menu commands

The boot menu accepts a single line command, which can be:

*  empty (ie, do the default command)

*  a short code alias defined in the ROM (see below)

*  a Unix style path name for a program to run from the hard drive
   (only the programs in `/stand` -- ie stand alone programs -- or
   a Unix kernel will run successfully)

*  a SCSI disk device (`sc(N,S)`), followed by a Unix style path name

*  a SCSI tape device (`mt(D,F)` or `pt(D,F)`), including a file offset number

*  a SCSI floppy device (`fp(...)`)

These commands can be preceded by `!` to enable the verbose debugging mode
in the ROM (which will print debug progress messages as it enters each
hardware interaction function).  `!` by itself will enable verbose debugging
then run the default command.

### Default boot command

The default boot command (reached by just pressing enter at the boot menu)
is `sc(0,0)/unix`, and will boot the Unix kernel from `/unix` on the first
hard drive.

### Boot menu short codes

There are a series of short codes which are intended to be used during
early bring up of the system from a (QIC-24) magnetic tape.  The built
in references are to `pt(,N)`, but on the Plexus P/20 they always load
from the magnetic tape (ie the same as `mt(,N)`).

In order these short codes are:

*    `?`       -- runs `pt(,)`, the first file on the tape (alias for `help`)
*    `help`    -- runs `pt(,)`, the first file on the tape
*    `unix`    -- runs `pt(,2)`, the third file on the tape
*    `sys5`    -- runs `pt(,2)`, the third file on the tape (alias for `unix`)
*    `format`  -- runs `pt(,3)`, the fourth file on the tape
*    `mkfs`    -- runs `pt(,4)`, the fifth file on the tape
*    `restor`  -- runs `pt(,5)`, the sixth file on the tape
*    `fsck`    -- runs `pt(,6)`, the seventh file on the tape
*    `dd`      -- runs `pt(,7)`, the eighth file on the tape
*    `fbackup` -- runs `pt(,8)`, the nineth file on the tape
*    `od`      -- runs `pt(,9)`, the tenth file on the tape
*    `ccal`    -- runs `pt(,10)`, the eleventh file on the tape
*    `fsdb`    -- runs `pt(,12)`, the thirteenth file on the tape
*    `du`      -- runs `pt(,13)`, the fourteenth file on the tape
*    `ls`      -- runs `pt(,14)`, the fifteenth file on the tape
*    `cat`     -- runs `pt(,15)`, the sixteenth file on the tape
`
This list is hard coded in the Plexus P/20 ROMs; tape positions `pt(,1)`
and `pt(,11)` are not used on the Plexus P/20 install tapes, but we
believe they held utilties needed by earlier Plexus systems on their
install tapes.

The `ccal` utility is particularly important as it is used to configure
the "clock calendar" chip, which has configuration RAM that stores the
hardware configuration of the system (eg, which SCSI devices are present, etc).

Note that running `unix` by itself will attempt to boot unix *from the
magnetic tape*.  To boot unix from the hard drive either just press enter,
or enter `/unix` with the initial slash, as that will reference the default
hard drive.

### Unix style path names

The boot command line will accept a unix style path name, starting with
a leading `/`.  This always references the hard drive, and assumes the
operating system has been installed onto the hard drive.

A custom Unix kernel can be booted with, eg, `/unixnew`, or the default
Unix kernel can be booted with /unix` (or just enter).

In addition to this, all the "early install" utilties present on the
install tape are also installed to the hard drive in the `/stand`
file system, which are intended for "standalone" usage from the boot
ROMs.  These can be run with:

`/stand/COMMAND`

where the `COMMAND` is a command like `ccal`, `ls`, `dd`, `od`, etc, eg
`/stand/ccal` will run the "clock calendar" RAM configuration tool to
configure the hardware present.

These stand alone tools operate in a *similar* manner to the Unix
utilties of the same name, but either interactively take additional
input (eg `/stand/ccal` is an interactive question and answer UI), or
present a second prompt (`$$`) to receive any required "command line"
parameters (eg `/stand/ls`).  There is no ability to pass additional
parameters from the boot ROM command line to the satnd alone utility,
and attempting to do so will simply cause the, eg, `/stand/ls /bin` commmand
(including a space!) to not be found.


### SCSI disk device -- `sc(N,S)`

The device prefix `sc(N,...)` refers to a SCSI disk device, but because
the Omti 5200 presents two hard drive LUNs (0 for first hard drive,
1 for second hard drive) a confusing system is used to encode *both*
the SCSI ID *and* the LUN into a single value.  (On the Omti 5200 LUN 2
is the floppy drive.)

Specifically for:

*   SCSI (Physical) Unit `U`; and
*   SCSI LUN (Logical Unit) `L` (0 or 1)

the value for `N` in the `sc(N,...) prefix is given by the formula:

```
N = (U * 2) + L
```

(and only LUNs 0 or 1 are supported).

For SCSI Unit 0 (ie, the Omti 5200 in the default hardware) this works
out to `sc(0,...)` referring to the (first) built in hard drive, and
`sc(1,...)` referring to the second (absent) hard drive.

But to access *additional* drives, it is important to remember the encoding
used.  For instance a SCSI device configured for SCSI ID 1, LUN 0, needs
to be access as `sc(2,0)` to get U=1 and L=0 in the ROM SCSI access routines.

The second parameter in `sc(N,S)` (ie, the `S` parameter) is the hard
drive "slice", ie the partition of the drive that should be accessed.
(On the Plexus P/20 these slices are configured with `/stand/format`, and
the partition table forms part of the block 0 information stored on the
hard drive.)

The slices are numbered from 0, and the default value for `S` if it is
omitted is 0.  Ie `sc(N,)` is the same as sc(N,0).


### SCSI tape device -- `mt(D,F)` and `pt(D,F)`

The SCSI tape drive can be accessed with either the `mt()` prefix,
or (presumably for historical reasons) the `pt()` prefix.  Both appear
to act the same.

The parameters to `mt(D,F)` are:

*   the SCSI device to access, `D` (if omitted it will access the default tape device); and

*   the SCSI file mark to access, `F` (starting at 0 for the first file)

Because magnetic tapes do not encode filenames, the Plexus P/20 install
tape used file marks on the tapes to separate files.  This means that a
specific install utility could be accessed as, eg the `ccal` configuration
command could be run from the same as `mt(,10)` since it is the eleventh
file on the tape (and the `F` values are offset 0).


### SCSI floppy device -- `fp(...)`

The 5.25" floppy disk drive is connected to the Omti 5200, as SCSI Unit 0,
LUN 2.  It can be accessed as `fp(...)`.

Unfortunately in the Plexus P/20 that Adrian's Digital Basement has, the
floppy drive is not currently working (does not spin), so the floppy
access details have not been confirmed (including, eg, whether the BIOS
expects the floppy to be like a hard drive with a file system, or like
a tape with file or block marks).
