/* 
This is the C++ code to calibrate/check if your MAX30102 works!
*/

#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

MAX30105 particleSensor;

const byte averageRates = 4; //If you want more averaging in the device, increase this
byte heartRates[averageRates]; // Array of Heart rates recorded
byte ratesMeasured = 0; // It is a counter - which counts the number of BPM rates measued
long timeLastBeat = 0; // It records the time when the last heartbeat occurred 

float beatsPerMinute;
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
  long irValue = particleSensor.getIR(); //We are defining a local variable

  //Beat Sensing
  if(checkForBeat(irValue) == true)
  {
    long change = millis() - timeLastBeat;
    timeLastBeat = millis(); // change isnt 0, it is this beat - previous beat

    beatsPerMinute = 60 / (change / 1000.0); // Converts it into an actual NUMBER

    //Checking if the sensor isnt picking up noise
    if(beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      heartRates[ratesMeasured++] = (byte)beatsPerMinute; //Store the reading in the array with ratesMeasured as a counter
      ratesMeasured %= averageRates; // Like a clock from 1 - 12, it helps ratesMeasured loop back to 0

      //Time to calculate the average
      beatAvg = 0;
      for(byte i = 0; i < averageRates; i++)
      {
        beatAvg += heartRates[i];
      }
      beatAvg /= averageRates;
    }

    Serial.print("IR=");
    Serial.print(irValue);
    Serial.print(", BPM=");
    Serial.print(beatsPerMinute);
    Serial.print(", Avg BPM=");
    Serial.println(beatAvg);
  }

  if(irValue < 50000)
  {
    Serial.println("No Finger?");
  }
}
