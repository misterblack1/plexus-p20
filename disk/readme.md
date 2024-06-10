# Plexus P/20 Hard Drive related files

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

This Plexus P/20 hard drive has three partitions on it:

- /dev/dk1 -> / (root file system)
- /dev/dsk/0s2 -> /user (home directory for all user files)
- /dev/dsk/0s3 -> /stocku (Appears to be a recovery partition)

The kernel loads from /unix and the file is 180,208 bytes, dated Nov 27  1985 on the running system.

The filesystem block size is 1024 bytes.

There is also another /user partition which was originally created by the original owner. The running /user partition was created for the computer's new life, but when it was created, the starting block was offset by 6,000 blocks, likely a type. (Original 60,000, new start location 66,000.) That preserved about 6 megs of the original partition and we have been able to recover these files as well. 

As of 9-June-2024, I have not uploaded the full drive image or the other partitions because they contains PII (personally identifyable information) about the previous users of this machine. Work is ongoing to sanitize the data on these partitions when at which point I'll share the complete bootable drive image. (Plus tar file extractions of the files on the partitions.) 
