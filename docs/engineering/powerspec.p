


          Plexus Computers, Inc.                       Robin Power Supply Specification


                                  ROBIN POWER SUPPLY SPECIFICATION


                                            MAR 6, 1985



          Output Ratings:         +5V            +12V             -12V
                            ----------------------------------------------------
                            | max  min  avg | max  min  avg  | max  min   avg  |
                            ----------------------------------------------------
                  Main      | 35A  10A  35A | 10A   1A   8A  |  3A  0.5A   3A  |
                            ----------------------------------------------------
                  Auxiliary | 20A  .5A  20A |  6A   0A   4A  |  3A   0A    3A  |
                            ----------------------------------------------------

                  Peak current is used on +12V to start motors on disk drives.  The
          supply must be capable of supplying max current for a 60 second period
          after turn on at max rated temperature.  Drawing peak current beyond 60
          seconds must not damage the supply but may cause the supply to shut down.

                  The 12 Volt outputs must not exceed 13.5 Volts, and the 5 Volt
	  output must not exceed 5.5 Volts.

                  The two supplies are to be connected to a combination of loads
          which share a common ground connection.  The main supply is to power the
          main logic board, up to two peripheral controller boards, and two drives
          (tape/winchester or floppy/winchester).  The auxiliary supply is to power
          one peripheral controller, one drive, and/or up to three logic boards
          through a backplane.  The main logic board ground connects to the backplane
          ground and to the peripheral controller grounds.  The peripheral controller
          grounds also connect to the drive grounds.  See the following diagram.






















          Feb 25, 1985                     Page 1         11-00026, 11-00027   Rev: G
          


          Plexus Computers, Inc.                       Robin Power Supply Specification



           Output Regulation:  +5.1V Main and Aux Supply
                                        2% max     70% +/- 30% max rated load change
                                        2% max     overshoot for turn on or turn off

                               +12V Aux Supply
                                        5% max     60% +/- 40% max rated load change
                                        2% max     overshoot for turn on or turn off

                               +/- 12V Main Supply, -12V Aux Supply
                                        5% max     60% +/- 40% max rated load change
                                                   70% +/- 30% max rated 5.1V load change
                                        2% max     overshoot for turn on or turn off

           Adjustments:        All factory preset and frozen

           Remote Sense:       Not required.

           Holdup Time:        16 mSec after loss of nominal ac power at full load.
                               A missing half-cycle of nominal ac power at 47 Hz should
                               not cause any change in any outputs including Power Fail
                               Output.

           Efficiency:         Greater than 70% at full load with nominal ac input

           Operational Temp    Deliver rated output at 50 C with forced air cooling
                               (35 cfm)

           Regulatory:         Supply must be designed and manufactured to meet:

                               Safety:   UL 478
                                         CSA C22.2 #154
                                         VDE 0806 and 0805
                                         IEC 380 and IEC 435 Class I

                                         Supplies will be initially submitted to VDE
                                         0806 and IEC 380, but we anticipate submitting
                                         them to 0805 and 435 in the future.

                                         Must be encased in metal to prevent access to
                                         high voltages.

                               Emissions:  VDE 0871 Level B
                                           FCC Part 15 subpart J Class A

           Mechanical Shock:   10 Gs operating, 20 Gs non operating










          Feb 25, 1985                     Page 2         11-00026, 11-00027   Rev: G
          


          Plexus Computers, Inc.                       Robin Power Supply Specification


           Output protection:    Over voltage on +5V set to 6.2V +/- 0.4V
           		         Over voltage on +/- 12V set to 14.0V +/- 0.5V

                                 Over power each supply (125 - 150% of rated load)
			         Turn on into a short circuit should not damage the
			         supply.

                                 Over temperature protection between 95 and 110 C

                                 Both supplies shall shut down simultaneously (within
                                 10 mSec) on fault conditions, latching off and
			         requiring remote start to become inactive before
			         turning on again.

           Altitude:             Operating: 10,000 ft. Non operating: 40,000 ft.

           Storage Temp:         -55 C to +85 C

           MTBF:                 30,000 Hour Minimum at max outputs and 35 C

           Relative Humidity:    5% to 95% (non condensing)

           Output Temp Coef:     .02% per degree C max on 5 Volt.
                                 .05% per degree C max on +/- 12 Volt.

           Transient Response:   Recovery to within 1% in less than 1.5 mSec.  Maximum
                                 voltage is 5%.

           AC Input:             85 - 130/ 190 - 264 vac input
                                 47 - 63 Hz
                                 Max Irms at low voltage = 14A.
                                 Input Surge Current (cold start) less than 50 Amp
                                 total for both supplies.

           Main Supply Features: The main supply must include the following:

                                 o  RFI line filtering for FCC and VDE compliance
                                 o  AC line cord receptacle
                                 o  Fuse
                                 o  120/240 line voltage selection switch
                                 o  Reset input
                                 o  Remote AC turn-on line
                                 o  Power fail sense circuit
                                 o  Expansion output for connection to auxiliary supply
                                    with noise immune control signals.
                                 o  Fan power output
                                 o  Fan

           Power Fail Output:    Active at least 2 mSec before outputs go out of spec
                                 (including outputs of secondary supply) due to brown
                                 out or total loss of ac power or remote turn off.
                                 Output valid when Reset output is inactive.

                                 Active = 0 - .5 Volts at -2 mA
                                 Inactive = 2.6 - 5 Volts (.5 mA at 2.6 V)

          Feb 25, 1985                     Page 3         11-00026, 11-00027   Rev: G
          


          Plexus Computers, Inc.                       Robin Power Supply Specification


           Reset Output:       Output is always valid.  Output changes to its inactive
                               state when the Power Fail Output is inactive and all
                               outputs have been in regulation for 150 mSec minimum.
                               Outputs change to active when they become unregulated
                               (at least 2 mSec after Power Fail Output becomes active)
                               or when the reset input is active low.  Minimum active
                               pulse is 150 mSec.
                               We want to be heavily involved in the design of this
                               circuit.

                               Active = 0 - .8 Volts at -2 mA
                               Inactive = 2.6 - 5 Volts (3mA at 2.6 V)

           Remote turn on
           Line:               The primary and secondary supply and fan must be turned
                               on when this line is connected to common.  The remote
                               turn on line must be isolated from primary circuitry.
                               When the supplies are shut off via this line, the
                               supplies should draw no more than 5 watts of AC power.

           RESET Input:        Asserts Reset Output to active state when connected to
                               Common.  This signal will be driven by a switch and must
                               be pulled up by a 300 ohm resistor to at least 5 volts and
                               not more than 12 volts.

           Fan Output:         115 vac 38 Watt at line frequency.  This output must
                               be switched by the remote turn on line.

           COMMON to EARTH:    Secondary COM (0 V dc) will be connected to EARTH
                               (green wire in the line cord) through a 100 ohm 5 watt
                               resistor in the primary supply.  COM and EARTH are not
                               to be connected in the auxiliary supply.

           Safety Shield:      All Main Supplies will be shipped with a dummy plug/
                               protective cover on P7 to protect personnel from the
                               300 Vdc lines.

           Safety Label:       A warning notice shall be applied to the connectors of
                               both supplies containing the 300 VDC lines.  This notice
                               shall indicate the presence of 300 VDC during normal
                               operation and for a period of three minutes after
                               disconnection of the mains.

           Burn In:            All supplies will be run for a minimum of 24 hours
			       in a 50C burn in chamber.  The supplies will be turned
			       off, using remote start, for 15 minutes every 6 hours.
			       The supplies will be loaded with the "average" load
			       as defined in the Output Ratings chart at the beginning
			       of this document.







          Feb 25, 1985                     Page 4         11-00026, 11-00027   Rev: G
          


          Plexus Computers, Inc.                       Robin Power Supply Specification


           Connectors:         Main Supply
           (AMP MTA-156 series)

                                P1  Main board power (1-640444-8)
                                    + 5 Output pins 14-18
                                    +12 Output pin 13
                                    -12 Output pin 12
                                    Key pin        11
                                    COM Output pins 5-10
                                    RES Output pin 4
                                    PFW Output pin 3
                                    RES Input pin 2
                                    Remote Turn-on pin 1

                                P2-5  Peripheral controller/device power ( 640444-5)
                                    +12 Output pin 1
                                    COM Output pins 2-3
                                    Key pin 4
                                    + 5 Output pin 5

                                P6  Fan power (640444-5)
                                    EARTH    pin 1
                                    Key pin 2
                                    ac power pin 3
                                    spare        4
                                    ac power pin 5

                                P7  Main/Auxiliary supply connections (1-640444-0)
                                    300 VDC NEG pin 1
                                    Key pins   2-3
                                    300 VDC POS pins 4
                                    Key pins 5-6
                                    Common pin 7
                                    Shutdown pin 8 (Both supplies can pull this line
                                                    low, causing both supplies to turn
                                                    off - overrides remote start)
                                    Control circuitry power pin 9 (rectified 60Hz
                                                                  transformer output)
                                    Remote start pin 10 (Same as P1 pin 1)

                                P8  Lug connection to green wire and frame ground.















          Feb 25, 1985                     Page 5         11-00026, 11-00027   Rev: G
          


          Plexus Computers, Inc.                       Robin Power Supply Specification





           Connectors:         Auxiliary Supply
           (AMP MTA-156 series)

                                P1  Expansion board power (1-640444-2)
                                    + 5 Output pins 1-4
                                    +12 Output pin 5
                                    -12 Output pin 6
                                    Key pin 7
                                    COM Output pins 8-12

                                P2-3 Peripheral controller/device power (640444-5)
                                     +12 Output pin 1
                                     COM Output pins 2-3
                                     Key pin 4
                                     + 5 Output pin 5

                                P4  Main/Auxiliary supply connections (connects to
                                    main supply P7) (1-640444-0)
                                    300 VDC NEG pin 1
                                    Key pin   2-3
                                    300 VDC POS pin 4
                                    Key pin 5-6
                                    Common pin 7
                                    Shutdown pin 8
                                    Control circuitry power pin 9
                                    Remote start pin 10

                                P5  Lug connection to Chassis

























          Feb 25, 1985                     Page 6         11-00026, 11-00027   Rev: G
          

