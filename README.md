# Plexus P/20 and P/15 Multi User UNIX server information repository 

I have a Plexus P/20 machine on the bench so I'm going to try to discover everything I can about how it works. There will be videos about the machine on my channel and I will upload binaries, docs and images to this repo.

Plexus was a California based company that was making UNIX server from around 1980 to 1988. The number of machines sold is eastimated to be about 2,500 globally across all model lines. The P/15 and P/20 seen on my channel is their low-end (8 or 16 user) machine designed to compete with more inexpensive UNIX systems. 

The P/15 and P/20 are dual Motorola 68010 systems which ran a port of AT&T System V Release 2 UNIX. The base system was the P/10 which supported 8 concurrent users and had 512k of RAM. The P/20 system included a three slot Multibus expansion chassis and second power supply to support the multibus cards. The system I have is a P/20 with the multibus expansion chassis and 2.0mb of DRAM. The P/20 is a completely new system compared to the P/25 and other Plexus models. It runs ports of the OS and BOOTROM, but is not likely to be backwards compatible in any way.

We have a discussion happening on Discord around this machine: [https://discord.gg/hyrhQqWh](https://discord.gg/kRrnvF4HSd)

Work is ongoing to create an emulator:
[https://github.com/Spritetm/plexus\_20\_emu]([https://github.com/Spritetm/plexus_20_emu])

Watch the videos on this machine:

- Part 1: [What is this rare multi-user UNIX workstation? (Plexus P/20)](https://youtu.be/iltZYXg5hZw)
- Part 2: [First power up of the Plexus P/20 dual processor UNIX system](https://youtu.be/lCPZAYvk940)
- Part 3: [Booting UNIX on the Plexus P/20 didn't go to plan](https://youtu.be/_IrxvDE6Fyo)
- Part 4: [We made some unbelievable discoveries about this Plexus P/20!](https://youtu.be/Ve1SuuRkx_o)
- Part 5: [The Plexus P/20 is now fully operational!](https://youtu.be/10b50ECWXLk)

Thank you to everyone who has contributed to finding this information!

### Updates
- June 19, 2024: Added files to /docs/engineering like PAL files and other information
- June 11, 2024: bootable disk images added. See /disks
- June 11, 2024: Bitsavers posted tape archives for various plexus machines. See /disks
  

### Reference Hardware

The original hardware of the Plexus P/20 featured in the videos above
(serial 0-03135) is:

*  [Plexus P/20 main board](docs/pdf/Plexus_P15_P20_Brochure_1985.pdf) with dual (asymmetric) Motorola 68010 CPUs (`job` and `dma` CPUs), and 8 x RS232C serial interfaces (6 x DE9 female, 2 x DB25 female)

*  2MiB DRAM board (behind the main board)

*  [Multibus](https://en.wikipedia.org/wiki/Multibus) backplane (with only a termination resistor card installed)

*  [Omti 5200 SCSI-1 to MFM/Floppy interposer card](http://bitsavers.org/pdf/sms/omti_5x00/3001206_OMTI_5000_Series_Reference_Aug85.pdf), configured for the MFM hard drive on LUN 0, and the floppy drive on LUN 1

*  [87MB/68MB Fujitsu M2243AS 3600rpm full height 5.25" MFM hard drive (with ST-412/ST-506 interface)](https://archive.org/details/bitsavers_fujitsubro1984_2565589) (connected as Unit 0, LUN 0 via the Omti 5200; part number Fujitsu B03B-4805-B003A; about 76MB usable on Plexus P/20)

*  ??? 720KiB (quad density) full height 5.25" Floppy Disk Drive (connected as Unit 0, LUN 1 via the Omti 5200)

*  [Archive Scorpion 5945C QIC-24 tape drive (with QIC-36 interface)](https://bitsavers.org/pdf/archive/scorpion/20271-001_scorpPrDesc_Mar84.pdf) (connected via SCSI to QIC-36 interposer card), with 45MB/60MB capacity (on QIC 300 XL and QIC 600 tapes), half height 5.25"

*  ??? SCSI-1 to QIC-36 tape controller interposer card, as SCSI Unit 7
