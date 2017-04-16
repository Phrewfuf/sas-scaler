# sas-scaler
Scales the steering angle sensor signal to account for quickrack installation on Subarus with VDC

This was made by user "hired-goon" at ozfoz.com in this thread: http://ozfoz.com/forum/viewtopic.php?f=29&t=56658&start=0&st=0&sk=t&sd=a

The issue was that a Forester with VDC presented unwanted behaviour after the installation of a quicker steering rack than OEM.
The underlying problem is, that the quicker steering rack needs less steering wheel input to achieve a turn-in-radius than the original rack.
This confuses the heck out of the VDC system, because it receives a low steering wheel angle and measures a higher change in direction than it expects to, causing the system to assume an oversteer condition and react accordingly.

This was solved by "hired-goon" with an arduino and two DFRobot CAN-BUS Shield V2.0 units to intercept the signal from the steering wheel angle sensor and scale it up according to the higher ratio of the new steering rack.

Mandatory hardware:

- 1x Arduino Uno R3
- 2x DFRobot CAN-BUS Shield V2.0
- 1x Shield stacking headers for Arduino (R3 Compatible)
- various lengths of coloured wire and heat shrink. Main wire used is "13x 0.12mm hookup wire"

Optional hardware:

- 1x Set of Steering Angle Sensor Male and Female plugs (cleanup, crimping is better than soldering in automotive)
- 1x LM2596 Voltage Regulator DC-DC Buck Converter Adjustable Step Down Module
- 1x ABS SEAL BOX 115X65X55 WITH FLANGE G308MF
- 1x 5VDC 30mm Thin 2 Wire Fan
- 1x 6 Pin Miniature Nylon MOLEX-Type Connector
- 1x Self-closing Braided Wire Wrap - 6mm x 2m
- 1x Piezo Buzzer 
- 2x Posi-Tap EX-110M
- Foam tape for the back of the enclosure to prevent rattling
- Large cable tie to secure the enclosure to the car
- 39 ohm resistor to limit the fan's speed
- 2x 1N5822 Schottky Diodes - used to tap 2 power sources and prevent backflow

The two shields need some modifications to fit the whole thing in the enclosure. Details see in linked forum thread.
