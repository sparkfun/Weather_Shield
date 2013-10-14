/* 
 HTU21D Humidity Sensor Example Code
 By: Nathan Seidle
 SparkFun Electronics
 Date: September 15th, 2013
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 
 Get humidity and temperature from the HTU21D sensor.
 
 Serial.print it out at 9600 baud to serial monitor.
 */

#include <SoftwareSerial.h>

SoftwareSerial imp(8, 9); // RX, TX into Imp pin 7

int winddir = 14; // [0-360 instantaneous wind direction]
float windspeedmph = 12.2; // [mph instantaneous wind speed]
float windgustmph = 13.1; // [mph current wind gust, using software specific time period]
int windgustdir = 233; // [0-360 using software specific time period]
float windspdmph_avg2m = 11.5; // [mph 2 minute average wind speed mph]
int winddir_avg2m = 230; // [0-360 2 minute average wind direction]
float windgustmph_10m = 15.6; // [mph past 10 minutes wind gust mph ]
int windgustdir_10m = 200; // [0-360 past 10 minutes wind gust direction]
float humidity = 33.5; // [%]
float tempf = 32.1; // [temperature F]
float rainin = 0.75; // [rain in - per hour?]
float dailyrainin = 0.07; // [daily rain in accumulated]
float baromin = 30.03;// [barom in]
float dewptf = 40.12; // [dewpoint F]

//These are not wunderground values, they are just for us
float batt_lvl = 11.8;
float light_lvl = 455;

void setup()
{
  Serial.begin(9600);
  imp.begin(9600);

  Serial.println("Imp hello world!");

}

void loop()
{
  //Wait for the imp to ping us with the ! character
  if(imp.available())
  {
    byte incoming = imp.read();
    if(incoming == '!')
    {
      Serial.print("We've been pinged!");

      printWeather(); //Send all the current readings out the imp and to its agent for posting to wunderground

    }
  }

  delay(100);
}

//Prints the various variables directly to the port
//I don't like the way this function is written but Arduino doesn't support floats under sprintf
void printWeather()
{
  imp.print("$,winddir=");
  imp.print(winddir);
  imp.print(",windspeedmph=");
  imp.print(windspeedmph, 1);
  imp.print(",windgustmph=");
  imp.print(windgustmph, 1);
  imp.print(",windgustdir=");
  imp.print(windgustdir);
  imp.print(",windspdmph_avg2m=");
  imp.print(windspdmph_avg2m, 1);
  imp.print(",winddir_avg2m=");
  imp.print(winddir_avg2m);
  imp.print(",windgustmph_10m=");
  imp.print(windgustmph_10m, 1);
  imp.print(",windgustdir_10m=");
  imp.print(windgustdir_10m);
  imp.print(",humidity=");
  imp.print(humidity, 1);
  imp.print(",tempf=");
  imp.print(tempf, 1);
  imp.print(",rainin=");
  imp.print(rainin, 2);
  imp.print(",dailyrainin=");
  imp.print(dailyrainin, 2);
  imp.print(",baromin=");
  imp.print(baromin, 2);
  imp.print(",dewptf=");
  imp.print(dewptf, 1);
  imp.print(",batt_lvl=");
  imp.print(batt_lvl, 1);
  imp.print(",light_lvl=");
  imp.print(light_lvl);
  imp.print(",");
  imp.println("#,");

  /*Serial.print("$,winddir=");
  Serial.print(winddir);
  Serial.print(",windspeedmph=");
  Serial.print(windspeedmph, 1);
  Serial.print(",windgustmph=");
  Serial.print(windgustmph, 1);
  Serial.print(",windgustdir=");
  Serial.print(windgustdir);
  Serial.print(",windspdmph_avg2m=");
  Serial.print(windspdmph_avg2m, 1);
  Serial.print(",winddir_avg2m=");
  Serial.print(winddir_avg2m);
  Serial.print(",windgustmph_10m=");
  Serial.print(windgustmph_10m, 1);
  Serial.print(",windgustdir_10m=");
  Serial.print(windgustdir_10m);
  Serial.print(",humidity=");
  Serial.print(humidity, 1);
  Serial.print(",tempf=");
  Serial.print(tempf, 1);
  Serial.print(",rainin=");
  Serial.print(rainin, 2);
  Serial.print(",dailyrainin=");
  Serial.print(dailyrainin, 2);
  Serial.print(",baromin=");
  Serial.print(baromin, 2);
  Serial.print(",dewptf=");
  Serial.print(dewptf, 1);
  Serial.print(",");
  Serial.print(batt_lvl, 1);
  Serial.print(",");
  Serial.print(light_lvl);
  Serial.print(",");
  Serial.println("#");*/

}


