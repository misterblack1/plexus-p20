# Plexus P/15 and P/20 hardware registers:

### Read:
```
E00000 15 = spare 7 = MWT
14 = spare     6 = MA9
13 = PEH       5-0 = MA21 - MA16
12 = PEL
11 = EN.BLK
10 = EN.MBUS
9 = EN.DMA
8 = EN.JOB
All 16 bits are latched on PELAT

E00002
15 = DIS.MAP    7 = BREQ3*
14 = RES.DMA-   6 = BREQ2*
13 = HALT.DMA-  5 = BREQ1*
12 = RES.JOB-   4 = BREQ11 = HALT.JOB-
                3 = BUSY  
10 = UPS        2 = SCSIBSY
9 = TEMP        1 = BERR.JOB8 = PFW
                0 = BERR.DMAAll 16 bits are latched on PELATCH

E00004
15-8 Not used   7 = A.JOB18
                6 = A.JOB17
                5 = A.JOB16
                4 = A.JOB15
                3 = A.JOB14
                2 = A.JOB13
                1 = A.JOB12
                0 = READ.JOB
                All 8 bits are latched on MBERR

E00014
15 = AS26       7 = AERR.JOB
14 = S.OOPS     6 = DERR.JOB
13 =            5 = MBTO
12 = UBE.DMA    4 = UBE.JOB
11 = ABE.DMA    3 = ABE.JOB
10 = EN.BLK     2 = EN.JOB
9 = EN.DMA      1 = EN.BLK
8 = EN.JOB      0 = EN.MBUS
Latched by DBE- Latched by JBE

E00016
15 = Boot.DMA-            7 = TBUSY
14 = BOOT.JOB-            6 = DIAG.UART
13 = SCSIDL- (Diag latch) 5 = HOLDMBUS
12 = DIAG.PEH (force err) 4 = RESMB- (Multi bus INIT*)
11 = DIAG.PEL (force err) 3 = CINTD.EN
10 = DIAG.PESC (force err)2 = CINTJ.EN
9 = DIAG.MB (Multi bus)   1 = TINT.EN (Temperature)
8 = DIS.MAP               0 = UINT.EN (UPS)

E00018
15-8 Not used    7 = EN.JOB
                 6 = JKPD
                 5-4 = spare
                 3 = INT.JOB
                 2 = INT.DMA
                 1 = KILL.JOB- (Reset job -)
                 0 = KILL.DMA (Reset DMA)

E0001A
15 = spare
14 = spare
13 = RI.B
12 = RI.A
11 = TCE.B-     3 = DSR.D
10 = TCE.A-     2 = DSR.C
9 = RCE.B-      1 = DSR.B
8 = RCE.A-      0 = DSR.A

Note: N=0 and N=1 indicate the state in which that bit
MUST be to allow normal system operation.
```

### Write
```
Address Reset selection:

E00000 NOP
E00020 Reset Multi bus interface error flag
E00040 Reset SCSI parity error flag
E00060 Reset Job processor software interrupt
E00080 Set Job processor software interrupt
E000A0 Reset DMA processor software interrupt
E000C0 Set DMA processor software interrupt
E000E0 Reset Job processor clock interrupt
E00100 Reset DMA processor clock interrupt
E00120 Reset Job processor bus error flag
E00140 Reset DMA processor bus error flag
E00160 Reset Memory Parity error flag

Master hardware reset sets this flag:

E00180 Reset Switch Interrupt Latch
E001A0 Reset SCSI Bus Error Flag
E001C0 Unassigned
E001E0 Unassigned

NOTE: Writing to these addresses causes the action. Data values are ignored.


E00010
15-8 Not used     7-4 Not used
                  3-0 Software settable LEDs

E00016
15 = Boot.DMA- (N=1)               7 = spare
14 = BOOT.JOB- (N=1)               6 = DIAG.UART (N=0)
13 = SCSIDL- (Diag latch ) (N=1)   5 = HOLDMBUS
12 = DIAG.PEH (force error) (N=0)  4 = RESMB- (Multi bus INIT*)
11 = DIAG.PEL (force error) (N=0)  3 = CINTD.EN
10 = DIAG.PESC (force error) (N=0) 2 = CINTJ.EN
9 = DIAG.MB (Multi bus) (N=0)      1 = TINT.EN (Temperature)
8 = DIS.MAP (N=0)                  0 = UINT.EN (UPS)
All 16 bits are reset low during System RESET

E00018
8-15 Not used     7 = spare
                  6 = JKPD Job control protection disable
                  5 - 2 = spare
                  1 = KILL.JOB- (Reset Job) (N=1)
                  0 = KILL.DMA (Reset DMA) (N=0)
All 8 bits are reset low during System RESET

E0001A
15 = spare        7-0 Not used
14 = spare
13 = spare
12 = spare
11 = TCE.B
10 = TCE.A
9 = RCE.B
8 = RCE.A

For the registers with N=0 or N=1: N=0 and N=1 indicate the state in which that bit MUST be to allow normal system operation.
```
