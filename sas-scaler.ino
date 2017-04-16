/*
Arduino Uno R3 using 2x DFRobot CAN BUS shield V2.0
Intercept and scale steering angle sensor signals for Subaru vehicles which have been swapped to a different ratio steering rack
Designed and tested on an AUDM MY12 Forester S-Edition swapped to an AUDM MY15 STI rack p/n 34110VA101
Written 27/11/2016 by Hired-Goon - hiredgoon@iinet.net.au or hired-goon at ozfoz.com
 
Includes code from DFRobot CAN BUS shield V2.0 examples
Includes code from various free online resources such as stackexchange.com
 
Covered under the MIT Licence, see full licence at the end of this file
*/
 
//Requires DFRobot CAN BUS Shield library found on DFRobots website: https://github.com/DFRobot/CAN_BUS/raw/master/mcp_can.zip 

#include <SPI.h>
#include <df_can.h>
 
//_________________________________________ User defined constants ___________________________________________________________________
 
const float scaleLevel = 1.3;           // What percentage should the signal scale? Must be a decimal number (not a percentage) e.g.  30% = 1.3
 
                                        // These CS pins are pin 10 on the DFRobot CAN BUS shield V2.0 - at least one needs to be bent away and jumpered to another pin, specify the pins being used here
const int SPI_CS_PIN_Car = 10;          // Set Chip Select/Slave Select (aka CS or SS) pin for Car board - connected to main wiring loom
const int SPI_CS_PIN_AngleSensor = 8;   // Set Chip Select/Slave Select (aka CS or SS) pin for AngleSensor board - connected to steering angle sensor
 
bool speakerEnabled = true;             // If true a warning beeper (if optionally installed) will announce startup complete or error. Set to false to if speaker not installed.
const int buzzerPin = 3;                // Set the pin the buzzer is connected to
const int canAttempts = 5;          // How many attempts (1 attempt per second) to start CANBUS before beeping - will beep the first 3 times this number is reached (e.g. canAttempts == 5, beep at 5, 10 and 15 fails)
 
//______________________________________________________________________________________________________________________________________
 
MCPCAN CAN_Car(SPI_CS_PIN_Car);
MCPCAN CAN_AngleSensor(SPI_CS_PIN_AngleSensor);
 
void setup()
{
    Serial.begin(250000);
    pinMode(buzzerPin, OUTPUT);
    canInit(CAN_Car, "Car");
    canInit(CAN_AngleSensor, "AngleSensor");
    if (speakerEnabled) beep(2); //if we've reached here, we've init'd correctly, beep twice to indicate the system is working
}
 
void loop()
{
    INT32U canId = 0x00;
    unsigned char len = 0;
    unsigned char buf[8];
    int angleData = 0;
    if (CAN_MSGAVAIL == CAN_AngleSensor.checkReceive()) {
        CAN_AngleSensor.readMsgBuf(&len, buf);
        canId = CAN_AngleSensor.getCanId();
        if ((canId == 0x02) && (buf[1] != 128)) {
           
            //Byte 0 and byte 1 combine to form steering angle, signed 16 bit int little endian
            //0 is middle, positive for angle left, negative for angle right.
            //With AUDM MY15 STI rack installed (p/n 34110VA101) I'm getting ~7000 full lock left and ~-7000 full lock right
            angleData = buf[1] << 8 | buf[0];
 
            //Output to serial port the 3 bytes of interest
            //Serial.print("in: "); Serial.print(buf[0]); Serial.print(" "); Serial.print(buf[1]); Serial.print(" "); Serial.println(buf[2]);
 
            //Scale up the angle data - angle data starts at -2 rather than 0 and works in fixed values of +/-16, thus the +2 then -2 later
            angleData = round((angleData + 2) / 16 * scaleLevel) * 16 - 2;
 
            //Set scaled up angle back to CAN message
            buf[0] = (angleData & 0xFF);
            buf[1] = (angleData >> 8);
 
            //Prepare checksum byte (byte 2)
            buf[2] = ((buf[0] & 0xF) ^ (buf[0] >> 4) ^ (buf[1] & 0xF) ^ (buf[1] >> 4) ^ (buf[2] & 0xF) ^ (buf[3] & 0xF) ^ (buf[3] >> 4) ^ (buf[4] & 0xF) ^ (buf[4] >> 4) ^ (buf[5] & 0xF) ^ (buf[5] >> 4) ^ (buf[6] & 0xF) ^ (buf[6] >> 4)) << 4 | (buf[2] & 0xF);
            //Serial.print("out: "); Serial.print(buf[0]); Serial.print(" "); Serial.print(buf[1]); Serial.print(" "); Serial.println(buf[2]);
        }
        //Send the message to the car
        CAN_Car.sendMsgBuf(canId, 0, len, buf);
    }
}
 
void canInit(MCPCAN &canToInit, String canName)
//Loop endlessly to initialize the CANBUS interface, break if successful
{
    int count = 0;
    while (true)
    {
        canToInit.init();
        if (CAN_OK == canToInit.begin(CAN_500KBPS)) {
            //Serial.print(canName);
            //Serial.println(" shield init ok!");
            break;
        }
        else {
            //Serial.print(canName);
            //Serial.println(" shield failed init");
            delay(1000);
        }
        count = (count + 1) % 7200; //reset the counter every 2 hour or so, as a reminder
        if ((count % canAttempts) == 0 && count <= (3 * canAttempts) && speakerEnabled) {
            beep(1);
        }      
    }
}
 
void beep(int mode) //mode 1 == error, mode 2 == success, mode 3 == debug
{
    switch (mode)
    {
    case(1): //beep 3 slow beeps times to announce a problem
    {
        buzz(1000,3);
    }
    case(2): //beep twice to signal its now working
    {
        buzz(60, 2);
    }
    case(3): //beep once for debug testing.
    {
        buzz(500, 1);
    }
    }
}
 
void buzz(long duration, int buzzes)
{
    int i;
    for (i = 1; i <= buzzes; i++)
    {
        if (i > 1) delay(2 * duration);
        tone(buzzerPin, 200, duration);
    }
}
 
/*
Copyright (c) 2016 "Hired-Goon"
 
Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:
 
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
 
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
*/
