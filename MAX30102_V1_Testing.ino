/* 
This is the C++ code to calibrate/check if your MAX30102 works!
*/

#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

MAX30105 particleSensor;

const byte Average_Rates = 4; //If you want more averaging in the device, increase this
byte HeartRates[Average_Rates]; // Array of Heart rates recorded
byte RatesMeasured = 0; // It is a counter - which counts the number of BPM rates measued
long TimeLastBeat = 0; // It records the time when the last heartbeat occurred 

float BeatsPerMinute;
int beatAvg;

void setup() 
{
  Serial.begin(115200);
  Serial.println("Initializing in progress...");
  Serial.println("Hello World !");

  //Time to initialize the sensors
  if(particleSensor.begin(Wire, I2C_SPEED_FAST)==false) // Checks if the sensor is working
  {
    Serial.println("Sensor - MAX30102 was not found.");
    Serial.println("Troubleshooting: Please check Wiring/Power");
    while(1);
  }
  Serial.println("Sensor - MAX30102 is working");
  Serial.println("Place your indexfinger on the sensor with steady pressure");

  particleSensor.setup(); //Default Settings of the library
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn the Red LED low such that we indicate the sensor is running
  particleSensor.setPulseAmplitudeGreen(0); // Turn Green LED off
  //We have completed the setup
}

void loop() 
{
  long Value_IR = particleSensor.getIR(); //We are defining a local variable

  //Beat Sensing
  if(checkForBeat(Value_IR) == true)
  {
    long change = millis() - TimeLastBeat;
    TimeLastBeat = millis(); // change isnt 0, it is this beat - previous beat

    BeatsPerMinute = 60 / (change / 1000.0); // Converts it into an actual NUMBER

    //Checking if the sensor isnt picking up noise
    if(BeatsPerMinute < 255 && BeatsPerMinute > 20)
    {
      HeartRates[RatesMeasured++] = (byte)BeatsPerMinute; //Store the reading in the array with RatesMeasured as a counter
      RatesMeasured %= Average_Rates; // Like a clock from 1 - 12, it helps RatesMeasured loop back to 0

      //Time to calculate the average
      beatAvg = 0;
      for(byte i = 0; i < Average_Rates; i++)
      {
        beatAvg += HeartRates[i];
      }
      beatAvg /= Average_Rates;
    }

    Serial.print("IR=");
    Serial.print(Value_IR);
    Serial.print(", BPM=");
    Serial.print(BeatsPerMinute);
    Serial.print(", Avg BPM=");
    Serial.println(beatAvg);
  }

  if(Value_IR < 50000)
  {
    Serial.println("No Finger?");
  }
}