# Documents I have found on the Plexus machines, mostly submitted by viewers. 

MC1488 / MC1489 used at RS232 drivers/buffers

MOSTEK MK68564 used as the SIO chip to drive the 8 serial ports (4 of them)

### Serial port map:

(Thanks to Patron Peter)

The book "Complete Guide to Rs-232 and Parallel Connections: A Step-By-Step Approach to Connecting Computers, Printers, Terminals, and Modems" actually has the serial connector pinouts for the Plexus P/15 and P/20.
```
Port 0 - console
port 1-3 no HW handshake
port 4-5 signals provided for modems
port 6-7 include timing lead support

Ports 1-3: 1-TX, 6-RX and 8-GND
Ports 4-5: 1-TX, 2-RTS, 4-DTR, 6-RX, 7-CTS, 8-GND, 9-DCD
Port 6-7: Normal DB25 RS232 with full flow control
```
