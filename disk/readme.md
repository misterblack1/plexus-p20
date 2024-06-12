# Plexus P/20 Storage Drive related files

Update June 11 2024: Bitsaver's posted a bunch of dumped Plexus tapes: https://bitsavers.org/bits/Plexus/ There appears to be one Robin Plexus P/20 tape in there, but it only contains a few hundred kilobytes of data, mostly stuff we alreaday have. The other tapes all appear to be for the other Plexus models. 

The Plexus P/15 and P/20 interfaces to all storage devices via on board SCSI.

- The motherboard SCSI controller ID is 03.
- The OMTI 5200 SCSI interposer is SCSI ID 00.
  - The interposer is connected to a ST-506/MFM hard drive (LUN 0) and can support up to two MFM drives.
  - A standard Shugart style floppy drive. (LUN 1)
  - The floppy drive is a 80 track, double sided, 300rpm, double density drive. (Often called Quad Density) The drive would have a formatted capacity of roughly 720k.
- The Archive tape drive (QIC) is SCSI ID 07.
- /dev/tape (6,0) -> /dev/rmt/0m (6,0) and 0mn (6,4) (major, minor)
- /dev/floppy (26,0) -> /dev/0m (26,0) and 0s0 (26,0) (major, minor)

The first sector on the Plexus hard drive contains a bootblock used by the boot ROM to do various things:
- Setup the OMTI 5200 SCSI interposer with the correct heads and cylinders for the attached drive
- Find the UNIX kernel image on the drive
- Load the drive's partition table

See the the 'bootblock.md' and associated visualization for more info.

This Plexus P/20 hard drive has three partitions on it. An image of hte entier drive is available here as plexus-sanitized.img. A version of the image designed to be used with an MFM emulator is available here: https://archive.org/details/plexus-sanitized.emu

The system is running a port of AT&T Unix System V Release 2. 

```
Partition table
Device  start  length (in blocks)
dsk/0s0 0      10000000
dsk/0s1 0      66000
dsk/0s2 66000  34000
dsk/0s3 100000 10000000
dsk/0s4 100000 45000

Swap location: 58000 length 8000 (overlaps the rootfs, which is normal)

Map:
/dev/dsk/0s0 is the entire drive (do not use)
/dev/dsk/0s1 is the rootfs
/dev/dsk/0s2 is the user partition
/dev/dsk/Os3 is the rest of the drive (do not use)
/dev/dsk/0s4 is the /stocku partition


  /dev/dsk/0s1
  File System: root Volume: 1

  ** Phase 1 - Check Blocks and Sizes
  ** Phase 2 - Check Pathnames
  ** Phase 3 - Check Connectivity
  ** Phase 4 - Check Reference Counts
  ** Phase 5 - Check Free List
  3185 files 27142 blocks 1403 free

  /dev/dsk/0s2
  File System: user Volume: 1

  ** Phase 1 - Check Blocks and Sizes
  ** Phase 2 - Check Pathnames
  ** Phase 3 - Check Connectivity
  ** Phase 4 - Check Reference Counts
  ** Phase 5 - Check Free List
  82 files 868 blocks 15865 free

  /dev/dsk/0s4
  File System: stocku Volume: 1

  ** Phase 1 - Check Blocks and Sizes
  ** Phase 2 - Check Pathnames
  ** Phase 3 - Check Connectivity
  ** Phase 4 - Check Reference Counts
  ** Phase 5 - Check Free List
  1970 files 16792 blocks 5355 free

```
The kernel loads from /unix (a file on the rootfs) and the file is 180,208 bytes, dated Nov 27  1985 on the running system. The ROM understands the filesystem and is able to find the file in the root partition for loading the system.

The filesystem block size is 1024 bytes, but the hard drive sector size is 512 bytes.

The original drive had another /user partition which was originally created by the original owner. This original /user partition started at 60,000 presumable at the end of the rootfs. At some point, the rootfs was extended to 66,000 with a swap of 8000 (starting at 58,000 and ending at 66,000.) A new /user partition was created at 66,000. 

It appears the swap space was never used by the running system, so the original /user partition that existed at 60,000 was preserved. We were able to recover most of the data from this partition, which is why we have technical details about this system. 

On the drive image here, this swap space was zero'd along with the new /user partition at 66,000 because they both contained a lot of PII. The resultant drive image is fully functional on the real system.

### Accounts

There are two user accounts on the Plexus P/20 drive image:
```
root <blank password>
adrian <blank password>
```
Both accounts have blank passwords, so just hit enter if prompted. 

### Extracted files
```
plexus_headers.tar.gz -> Extraction of all C library headers. These are a copy of the rootfs headers.
plexus_stocku.tar.gz -> Extraction of all files from the stocku (/stocku) partition from the dive image.
                        This appears to be a recovery partition with an untouched copy of UNIX
plexus_rootfs.tar.gz -> Extraction of all files from the rootfs (/) from the drive image here
                        This is the running root filesystem taken off the Plexus P/20 I have

I did not upload the /user files, as I deleted all of them to sanitize the drive image.
```

### Successful boot

```
PLEXUS PRIMARY BOOT REV 1.2
: /unix

UNIX/1.2: Sys5.2r8
real mem  = 2097152
avail mem = 1794048

INIT: SINGLE USER MODE

The current runlevel:
   .       run-level S  Jun  3 18:30    S    1    2

NOTE: If just booting, type init 2 to go into multiuser mode.

You are now logged in as root!

Your fortune:
Troubled day for virgins over 16 who are beautiful and wealthy and live
in eucalyptus trees.

#
```
