# Plexus P/20 Hard Drive related files

The Plexus P/20 hard drive has three partitions on it:

- /dev/dk1 -> / (root file system)
- /dev/dsk/0s2 -> /user (home directory for all user files)
- /dev/dsk/0s3 -> /stocku (Appears to be a recovery partition)

The kernel loads from /unix and the file is 180,208 bytes, dated Nov 27  1985 on the running system.

The filesystem block size if 1024 bytes.

There is also another /user partition which was originally created by the original owner. The running /user partition was created for the computer's new life, but when it was created, the starting block was offset by 6,000 blocks, likely a type. (Original 60,000, new start location 66,000.) That preserved about 6 megs of the original partition and we have been able to recover these files as well. 
