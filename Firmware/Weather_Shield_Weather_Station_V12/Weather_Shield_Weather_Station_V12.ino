/*
  Weather Shield Example
  By: Nathan Seidle
  SparkFun Electronics
  Date: November 16th, 2013
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  Much of this is based on Mike Grusin's USB Weather Board code: https://www.sparkfun.com/products/10586

  This is a more advanced example of how to utilize every aspect of the weather shield. See the basic
  example if you're just getting started.

  This code reads all the various sensors (wind speed, direction, rain gauge, humidity, pressure, light, batt_lvl)
  and reports it over the serial comm port. This can be easily routed to a datalogger (such as OpenLog) or
  a wireless transmitter (such as Electric Imp).

  Measurements are reported once a second but windspeed and rain gauge are tied to interrupts that are
  calculated at each report.

  This example code assumes the GPS module is not used.

  Updated by Joel Bartlett
  03/02/2017
  Removed HTU21D code and replaced with Si7021

  Updated be Wes Furuya
  06/19/2023
  Implemented "Weather Meter" Arduino library
*/

#include <Wire.h>                                        //I2C needed for sensors
#include "SparkFunMPL3115A2.h"                           //Pressure sensor - Search "SparkFun MPL3115" and install from Library Manager
#include "SparkFun_Si7021_Breakout_Library.h"            //Humidity sensor - Search "SparkFun Si7021" and install from Library Manager
#include "SparkFun_Weather_Meter_Kit_Arduino_Library.h"  //Weather meter kit - Search "SparkFun Weather Meter" and install from Library Manager


//Hardware pin definitions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// digital I/O pins
const byte WSPEED = 3;
const byte RAIN = 2;
const byte STAT1 = 7;
const byte STAT2 = 8;

// analog I/O pins
const byte REFERENCE_3V3 = A3;
const byte LIGHT = A1;
const byte BATT = A2;
const byte WDIR = A0;
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//Global Variables
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
long lastSecond;  //The millis counter to see when a second rolls by

float humidity = 0;  // [%]
float tempf = 0;     // [temperature F]
//float baromin = 30.03;// [barom in] - It's hard to calculate baromin locally, do this in the agent
float pressure = 0;

float wind_dir = 0;    // [degrees (Cardinal)]
float wind_speed = 0;  // [kph]
float rain = 0;        // [mm]

float batt_lvl = 11.8;  //[analog value from 0 to 1023]
float light_lvl = 455;  //[analog value from 0 to 1023]
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


MPL3115A2 myPressure;                                      //Create an instance of the pressure sensor
Weather myHumidity;                                        //Create an instance of the humidity sensor
SFEWeatherMeterKit myweatherMeterKit(WDIR, WSPEED, RAIN);  // Create an instance of the weather meter kit


void setup() {
  Serial.begin(115200);
  Serial.println("Weather Shield Example");

  pinMode(STAT1, OUTPUT);  //Status LED Blue
  pinMode(STAT2, OUTPUT);  //Status LED Green

  // pinMode(WSPEED, INPUT_PULLUP); // input from wind meters windspeed sensor
  // pinMode(RAIN, INPUT_PULLUP); // input from wind meters rain gauge sensor

  pinMode(REFERENCE_3V3, INPUT);
  pinMode(LIGHT, INPUT);

  //Configure the pressure sensor
  myPressure.begin();               // Get sensor online
  myPressure.setModeBarometer();    // Measure pressure in Pascals from 20 to 110 kPa
  myPressure.setOversampleRate(7);  // Set Oversample to the recommended 128
  myPressure.enableEventFlags();    // Enable all three pressure and temp event flags

  //Configure the humidity sensor
  myHumidity.begin();

  // The weather meter kit library assumes a 12-bit ADC
  // Configuring a 10-bit ADC resolution for the ATmega328 (RedBoard/Uno)
  myweatherMeterKit.setADCResolutionBits(10);

  // Begin weather meter kit
  myweatherMeterKit.begin();

  lastSecond = millis();

  // // attach external interrupt pins to IRQ functions
  // attachInterrupt(0, rainIRQ, FALLING);
  // attachInterrupt(1, wspeedIRQ, FALLING);

  // // turn on interrupts
  // interrupts();

  Serial.println("Weather Shield online!");
}

void loop() {
  //Keep track of which minute it is
  if (millis() - lastSecond >= 1000) {
    digitalWrite(STAT1, HIGH);  //Blink stat LED

    lastSecond += 1000;

    //Report all readings every second
    printWeather();
  }

  digitalWrite(STAT1, LOW);  //Turn off stat LED

  delay(100);
}

//Calculates each of the variables that wunderground is expecting
void calcWeather() {

  //Calc temp/humidity from Si7021 sensor
  humidity = myHumidity.getRH();
  tempf = myHumidity.readTempF();

  //Weather Meter Kit
  //Calc Wind
  wind_dir = myweatherMeterKit.getWindDirection();
  wind_speed = myweatherMeterKit.getWindSpeed();
  //Calc Rain
  rain = myweatherMeterKit.getTotalRainfall();

  //Calc pressure from MPL3115A2
  pressure = myPressure.readPressure();

  //Calc light level
  light_lvl = get_light_level();

  //Calc battery level
  batt_lvl = get_battery_level();
}

//Returns the voltage of the light sensor based on the 3.3V rail
//This allows us to ignore what VCC might be (an Arduino plugged into USB has VCC of 4.5 to 5.2V)
float get_light_level() {
  float operatingVoltage = analogRead(REFERENCE_3V3);

  float lightSensor = analogRead(LIGHT);

  operatingVoltage = 3.3 / operatingVoltage;  //The reference voltage is 3.3V

  lightSensor = operatingVoltage * lightSensor;

  return (lightSensor);
}

//Returns the voltage of the raw pin based on the 3.3V rail
//This allows us to ignore what VCC might be (an Arduino plugged into USB has VCC of 4.5 to 5.2V)
//Battery level is connected to the RAW pin on Arduino and is fed through two 5% resistors:
//3.9K on the high side (R1), and 1K on the low side (R2)
float get_battery_level() {
  float operatingVoltage = analogRead(REFERENCE_3V3);

  float rawVoltage = analogRead(BATT);

  operatingVoltage = 3.30 / operatingVoltage;  //The reference voltage is 3.3V

  rawVoltage = operatingVoltage * rawVoltage;  //Convert the 0 to 1023 int to actual voltage on BATT pin

  rawVoltage *= 4.90;  //(3.9k+1k)/1k - multiple BATT voltage by the voltage divider to get actual system voltage

  return (rawVoltage);
}


//Prints the various variables directly to the port
//I don't like the way this function is written but Arduino doesn't support floats under sprintf
void printWeather() {
  calcWeather();  //Go calc all the various sensors

  Serial.println();
  Serial.print("humidity=");
  Serial.print(humidity, 1);
  Serial.print(" %RH, tempf=");
  Serial.print(tempf, 1);
  Serial.print(" F, pressure=");
  Serial.print(pressure, 2);
  Serial.print(" Pa, wind direction= ");
  Serial.print(wind_dir, 1);
  Serial.print(" deg, wind speed= ");
  Serial.print(wind_speed, 1);
  Serial.print(" kph, total rain= ");
  Serial.print(rain, 1);
  Serial.print(" mm, batt_lvl=");
  Serial.print(batt_lvl, 2);
  Serial.print(" V, light_lvl=");
  Serial.print(light_lvl, 2);
  Serial.print(",");
  Serial.println("#");
}
