#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"

MAX30105 particleSensor;// Makes an oblect called particleSensor

#define MAX_BRIGHTNESS 255

uint32_t irBuffer[100];   //Infrared LED sensor data
uint32_t redBuffer[100];  //Red LED sensor data

int32_t bufferLength;   //Data length
int32_t spo2;           //spo2 value
int8_t validSPO2;       //Indicates that the SPO2 calculation is valid
int32_t heartRate;      //Heart rate value
int8_t validHeartRate;  //Indicates that the heart rate calculation is valid

byte pulseLED = 11; //pin no. be changed
byte readLED = 13; //Blinks with each data read and pin no. to be changed 

void setup() 
{
  Serial.begin(115200);

  pinMode(pulseLED, OUTPUT);
  pinMode(readLED, OUTPUT);

  // Let us initialize the sensor
  if(particleDensor.begin(Wire, I2C_SPEED_FAST))
  {
    Serial.println(F("Sensor:MAX30102 was not found"));
    Serial.println(F("Troubleshooting: Please check wiring/power"));
    while(1); //Freezes the program. Note - our target is to save memory as well
  }

  Serial.println(F("Attach the sensor to your finger with the help of a rubber band.Press any key to start"));
  while(Serial.available() == 0); //Delibrate pause such that the user can place their finger
  Serial.read(); //Throws the character away  

  byte ledBrightness = 100; //Strength of LEDs can be adjusted from 0(off) to MAX_BRIGHTNESS
  byte sampleAverage = 4; // Options are 1,2,4,8,16,32
  byte ledMode = 2; //1 = red, 2 = Red + IR,3 = Red + IR + Green
  byte sampleRate = 100; //50,100,200,400,800,1000,1600,3200
  int pulseWidth = 411; // 69,118,215,411
  int adcRange = 4096; //2048, 4096, 8192, 16384

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);  //Configure sensor with these settings
}

void loop() 
{
  bufferLength = 100;  //buffer length of 100 stores 4 seconds of samples running at 25sps

  //Reading the signal and determine the signal range 
  for (byte i = 0; i < bufferLength; i++) 
  {
    while (particleSensor.available() == false)  //do we have new data?
    {
      particleSensor.check(); //Check the sensor for new data
    }
    
    redBuffer[i] = particleSensor.getRed(); //Fill in the details
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample();  //We're finished with this sample so move to next sample

    Serial.print(F("red="));
    Serial.print(redBuffer[i], DEC);
    Serial.print(F(", ir="));
    Serial.println(irBuffer[i], DEC); // Time to print the array!
  }

  //calculate heart rate and SpO2 after first 100 samples (first 4 seconds of samples)
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate); //4 answers and theit respective locations(denoted by &)

  //Continuously taking samples from MAX30102.  Heart rate and SpO2 are calculated every 1 second
  while(1)
  {
   //dumping the first 25 sets of samples in the memory and shift the last 75 sets of samples to the top in order to recycle space
   for (byte i = 25; i < 100; i++) 
   {
      redBuffer[i - 25] = redBuffer[i];
      irBuffer[i - 25] = irBuffer[i];
   }

   //Taking new samples
   for (byte i = 75; i < 100; i++)
   {
    while (particleSensor.available() == false)  //do we have new data?
    {
      particleSensor.check(); //Check the sensor for new data
    }

    digitalWrite(readLED, !digitalRead(readLED));  //Reads whether it’s on, then writes the opposite. It blinks with every reading.

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample(); 

    //send samples and calculation result to terminal program through UART      Serial.print(F("red="));
    Serial.print(F("red="));
    Serial.print(redBuffer[i], DEC);
    Serial.print(F(", ir="));
    Serial.print(irBuffer[i], DEC);
      
    Serial.print(F(", HR="));
    Serial.println(heartRate, DEC);

    Serial.print(F(", HRvalid="));
    Serial.println(validHeartRate, DEC);

    Serial.print(F(", SPO2="));
    Serial.print(spo2, DEC);

    Serial.print(F(", SPO2Valid="));
    Serial.println(validSPO2, DEC);
   }

  //After gathering 25 new samples recalculate HR and SP02
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate); 
  
  }

}
