/*
    
 _____  ___ _____     ______ ___________ _____  
 /  ___|/ _ \_   _|    |  _  \  _  | ___ \  ___| 
 \ `--./ /_\ \| |______| | | | | | | |_/ / |__   
 `--. \  _  || |______| | | | | | |  __/|  __|  
 /\__/ / | | || |      | |/ /\ \_/ / |   | |___  
 \____/\_| |_/\_/      |___/  \___/\_|   \____/ 
 
 [Satellite Data On Personal Eqipment]
 targeting for your APT antenna!
 
 Sam Clarke 2013
 
 This code uses NORAD issued TLE information coupled with an Adafuit Ulitmate GPS 
 module to track and target a given sattelite using the Plan13 satellite tracking 
 algorithms from James Miller [http://www.amsat.org/amsat/articles/g3ruh/111.html]. 
 The program also calculates Doppler shift of the target satellite's downlink & 
 uplink frequencies.
 
 Thanks to Andrew Stancliffe for northern hemisphere testing.
 
 ** PLACE THE ACTIVE TLE IN THE BOX BELOW TO MAP VARIABLES FOR NEXT TIME!! **
 */
//////////////////////// EXAMPLE NORAD TLE INFORMATION \\\\\\\\\\\\\\\\\\\\\\\\\\\
//                                                                              \\
//    "ASTRA 2F",                                                               \\
//                                                                              \\
//    1 38778U 12051A   14010.00000000  .00000143  00000-0  00000+0 0  2381     \\
//    2 38778 000.0670 267.2510 0004011 341.2500 249.1500 01.00276604  4755     \\
//                                                                              \\
///////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

// recent data https://celestrak.com/NORAD/elements/amateur.txt

#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <Plan13.h>
#include "TimeLib.h"

//ISS (ZARYA)
//1 25544U 98067A   19181.39493521 -.00156611  00000-0 -26708-2 0  9993
//k lllllm bbyyyp   AABBBBBBBBBBBB IIIIIIIIII yyyyyyyy pppppppp b rrrrg
//2 25544  51.6396 295.7455 0007987  98.4040   9.2643 15.51194651177305
//b rrrrr CCCCCCCC DDDDDDDD EEEEEEE FFFFFFFF GGGGGGGG HHHHHHHHHHHJJJJJb

String  BIRD=               "ISS (ZARYA)";
#define YEAR                2019            // 20AA
#define EPOCH               181.39493521    // BBBBBBBBBBBB        
#define INCLINATION         51.6396         // CCCCCCCC
#define RAAN                295.7455        // DDDDDDDD     
#define ECCENTRICITY        0.0007987       // 0.EEEEEE
#define ARGUMENT_PEDIGREE   98.404          // FFFFFFFF
#define MEAN_ANOMALY        9.2643          // GGGGGGGG
#define MEAN_MOTION         15.51194651     // HHHHHHHHHHH
#define TIME_MOTION_D       -0.00156611     // IIIIIIIIII
#define EPOCH_ORBIT         17730           // JJJJJ

//String  BIRD=               "ASTRA 2F";
//#define YEAR                2014
//#define EPOCH               10.00000000        
//#define INCLINATION         000.0670            
//#define RAAN                267.2510          
//#define ECCENTRICITY        00000            
//#define ARGUMENT_PEDIGREE   341.2500           
//#define MEAN_ANOMALY         249.1500           
//#define MEAN_MOTION         01.0027660      
//#define TIME_MOTION_D       0.00000143
//#define EPOCH_ORBIT         4755

#define GPSECHO  true

#define ONEPPM              1.0e-6
#define ONEPPTM             1.0e-7
 
unsigned long APT_FREQUENCY  =137100000;
unsigned long HRPT_FREQUENCY =1700000000;
uint32_t timer = millis();
boolean firstboot = true;
 
Plan13 p13;

static const int RXPin = 8, TXPin = 9;
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;

SoftwareSerial ss(RXPin, TXPin);
 
 
void setup () 
{
  Serial.begin(115200);
  ss.begin(GPSBaud);
  //p13.setFrequency(APT_FREQUENCY, HRPT_FREQUENCY);
}
 
void loop() 
{ 
  if(firstboot == true){
    delay(1000);
    Serial.println("     *********   SAT-D.O.P.E   **********     ");
    Serial.println("     Satellite Data On Personal Equipment     ");
    Serial.println();
    Serial.println();
    firstboot = false;  
  }

  while (ss.available() > 0)
  if (gps.encode(ss.read()))

  if(!gps.location.isValid()){
        Serial.println("# Aquiring Satellites..."); 
        delay(3000);
  }
  if (gps.location.isValid()) 
  {
    if (timer > millis())  timer = millis();
    if (millis() - timer > 1500) { 
      timer = millis();                  
      if (gps.location.isValid()) {
        setTime(gps.time.hour(), gps.time.minute(), gps.time.second(),gps.date.day(), gps.date.month(), gps.date.year());
        p13.setTime(year(), month(), day(), hour(), minute(), second());
        p13.setElements(YEAR, EPOCH, INCLINATION, RAAN, ECCENTRICITY*ONEPPTM, ARGUMENT_PEDIGREE, 
                        MEAN_ANOMALY, MEAN_MOTION, TIME_MOTION_D, EPOCH_ORBIT, 180);
                        
        float latitude = gps.location.lat();

        float longitude = gps.location.lng();

        float altitude = gps.altitude.meters();
            
        p13.setLocation(longitude, latitude, altitude); 
        p13.calculate();
        Serial.println();
        Serial.print("Target: ");
        Serial.println(BIRD);
        Serial.println();
        Serial.print("Observer Location: ");
        Serial.print(latitude, 6);
        Serial.print(", "); 
        Serial.println(longitude, 6);
        Serial.print("Observer Altitude: "); 
        Serial.print(altitude);
        Serial.println(" m");
         
        p13.printdata();
        Serial.println();
      }
    }
  }
}
