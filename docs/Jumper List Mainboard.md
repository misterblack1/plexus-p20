Discored user mxshift (05/30/2024 9:36 AM)
```

Because I was curious, the 4 pins near the SCSI connector:
J20 -> pin 25 of SCSI connector
J21 -> pin 26 of SCSI connector
J22 -> VCC
J23 -> GND
Notably, those pins are officially unused on SCSI

Then the 12 pins above U15K:
J61 - J64 -> preload input (pins 3-6) of U12J (LS169)
J53, J55, J57, J59 -> GND
J54, J56, J58, J60 -> PU5 (pullup to VCC)
looks like those jumpers above U15K are setting a clock divider for one of the PALs

J27, J24, J26 just to the bottom-right of the ROMs:
J27 -> U16F(42) which is address line 14
J24 -> output enable of ROMs (U17K, U17L, U15K, U15L)
J26 -> U16F(43) which is address line 15
Those are for selecting between 27128 and 2764 ROM sizes

J44-J46 (left and up from U27H) are selecting between two signals somewhere in a complex set of 7400-series logic.  J44 is an input to U24H (74S64). 
J45 and J46 are outputs.

Near the bottom SIO (U26P):
J29 -> input to MC1488 RS-232 driver
J30 -> DTR output from U26P port A
J31 -> GND
```
