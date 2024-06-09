# Plexus P/20 and P/15 hard drive first sector
Discord user dgiller analyzed the first sector of the Plexus P/20 hard drive and found this was the data contained there. This first sector is used to setup the OMTI 5200 SCSI interposer and contains the head/cylinders used by that interposer to access the ST-506/MFM hard drive. The sector also contains the partition table.

```
omti_step_pulse: 0
omti_step_period: 0
omti_step_mode: 0
omti_nheads: 10
omti_ncyls: 753
omti_wpcomp: 1
omti_type_wsi: 0
omti_sct_per_trk: 17
omti_reserved: 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
fsbsize: 1024
id: 28772
initsize: 256
bootname: /unix
nodename:
nswap: 8000
swplo: 58000
rootdev: 1
pipedev: 1
dumpdev: 1
swapdev: 1
parttable: 0, 10000000, 0, 66000, 66000, 34000, 100000, 10000000, 100000, 45000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
ctl_type: 0
ctl_spec: 0
initlen: 10
cdb_op: 194
cdb_lunlad2: 0
cdb_lad1: 0
cdb_lad0: 0
cdb_byte4: 0
cdb_byte5: 0
scsiaddr: 0
scsimap: 0, 0, 0, 0, 252, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
alttrks: 224
miversion: 0
mirid: 0
mitimestamp: 0
miprimdriv: 0
misecondary: 0
miflags: 0
padding: 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
```
