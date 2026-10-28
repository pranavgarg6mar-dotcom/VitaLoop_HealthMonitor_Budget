/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com  
*********/

// v3.0: MAX30102 (heart rate + SpO2) and DS18B20 (temperature) merged into one sketch

#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include <OneWire.h>
#include <DallasTemperature.h>

MAX30105 particleSensor;

// GPIO where the DS18B20 is connected to
const int oneWireBus = 4;
const int ledPinNumber = 2; //This can be changed but avoid GPIO 4(DS18B20) and GPIO 21/22 (I2C protocols for the MAX30102) , v3.1: Alert LED

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

uint32_t irBuffer[100];   //infrared LED sensor data
uint32_t redBuffer[100];  //red LED sensor data

int32_t bufferLength;   //data length
int32_t spo2;           //SPO2 value
int8_t validSPO2;       //indicator to show if the SPO2 calculation is valid
int32_t heartRate;      //heart rate value
int8_t validHeartRate;  //indicator to show if the heart rate calculation is valid

const long fingerThreshold = 50000;   // v2.2: tune this using your own finger / no-finger values

// v3.0: heart rate and SpO2 limits - readings outside these are labelled "probably noise", not skipped
const int minHeartRate = 40;      // below this is probably noise
const int maxHeartRate = 180;     // above this is probably noise (the 250 we saw)
const int minSpO2 = 70;           // below this is probably noise
const int maxSpO2 = 100;          // SpO2 cannot be above 100 percent
const int settleSeconds = 10;     // the first seconds after the finger is placed are labelled "settling"
const int maxHRJump = 20;         // biggest believable change in heart rate between two seconds
const byte maxUnsteady = 3;       // after this many jumps in a row, accept the new value (stops lock-out)

int secondsSinceStart = 0;        // counts the seconds since the finger was placed
int lastGoodHR = 0;               // the last heart rate that passed every check
bool haveLastHR = false;          // false until the first good reading
byte unsteadyCount = 0;           // how many jumpy readings in a row

// V1.1 settings, kept at the top so they are easy to change
const float minTemp = 20.0;
const float maxTemp = 45.0;
const float maxJump = 3.0;

float lastGoodTemp = 0;
bool haveLastTemp = false;   // false until the first good reading
byte tempUnsteadyCount = 0;  // v3.0: how many jumpy temperature readings in a row

// v3.0: the temperature notebook - keeps the last good readings so we can tell when it flattens out
const byte flatSeconds = 30;        // how many recent readings we look at (one per second)
const float flatTolerance = 0.15;   // if the highest and lowest differ by less than this (in C), it has flattened
float tempNotebook[flatSeconds];    // the last good temperature readings
byte tempSpot = 0;                  // which box to write into next
byte tempCount = 0;                 // how many good readings are in the notebook so far

unsigned long lastTempTime = 0;     // v3.0: when we last read the temperature

// v3.0: reads and prints the temperature once per second, without freezing the MAX30102
void updateTemperature()
{
  if (millis() - lastTempTime < 1000)
  {
    return;   // not time yet
  }
  lastTempTime = millis();

  float temperatureC = sensors.getTempCByIndex(0);          // the result of the measurement we asked for last second
  float temperatureF = temperatureC * 9.0 / 5.0 + 32.0;     // convert to Fahrenheit ourselves
  sensors.requestTemperatures();                            // start the next measurement in the background

  if (temperatureC == -127) 
  {
    Serial.println("DS18B20 not found. Check wiring and the 4.7k resistor.");
    tempCount = 0;   // v3.0: a broken probe means the old notebook can't be trusted
  }

  else if (temperatureC == 85) 
  {
    Serial.println("Power-up value, not a real reading.");
  }

  else if (temperatureC < minTemp || temperatureC > maxTemp) 
  {
    Serial.println("Out of plausible range.");
  }

  else if (haveLastTemp && fabs(temperatureC - lastGoodTemp) > maxJump && tempUnsteadyCount < maxUnsteady) 
  {
    tempUnsteadyCount++;   // v3.0: count it, so we never get stuck rejecting everything
    Serial.println("Jumped too much, probably noise.");
  }
  
  else {
    // good reading
    lastGoodTemp = temperatureC;
    haveLastTemp = true;
    tempUnsteadyCount = 0;

    // v3.0: write it in the notebook, wrapping back to box 0 after the last box
    tempNotebook[tempSpot] = temperatureC;
    tempSpot++;
    tempSpot %= flatSeconds;
    if (tempCount < flatSeconds)
    {
      tempCount++;
    }

    // v3.0: has it flattened out? only check once the notebook is full
    bool flat = false;
    if (tempCount == flatSeconds)
    {
      float lowest = tempNotebook[0];
      float highest = tempNotebook[0];
      for (byte i = 1; i < flatSeconds; i++)
      {
        if (tempNotebook[i] < lowest)
        {
          lowest = tempNotebook[i];
        }
        if (tempNotebook[i] > highest)
        {
          highest = tempNotebook[i];
        }
      }
      flat = (highest - lowest <= flatTolerance);
    }

    Serial.print("Temp=");
    Serial.print(temperatureC);
    Serial.print("ºC  ");
    Serial.print(temperatureF);
    Serial.print("ºF");
    if (flat)
    {
      Serial.print("  <-- estimated final temperature");
    }
    Serial.println();
  }
}

void setup() {
  Serial.begin(115200);  // initialize serial communication at 115200 bits per second:
  pinMode(ledPinNumber, OUTPUT); //v3.1

  digitalWrite(ledPinNumber, LOW);//start off

  // Start the DS18B20 sensor
  sensors.begin();
  sensors.setWaitForConversion(false);   // v3.0: don't freeze the MAX30102 while the DS18B20 measures
  sensors.requestTemperatures();         // v3.0: start the first measurement

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST))  //Use default I2C port, 400kHz speed
  {
    Serial.println(F("MAX30105 was not found. Please check wiring/power."));
    while (1);
  }

  byte ledBrightness = 60;  //Options: 0=Off to 255=50mA
  byte sampleAverage = 4;   //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2;         //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 100;    //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411;     //Options: 69, 118, 215, 411
  int adcRange = 4096;      //Options: 2048, 4096, 8192, 16384

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);  //Configure sensor with these settings
}

void loop() {
  bufferLength = 100;  //buffer length of 100 stores 4 seconds of samples running at 25sps

  // v2.1: wait for a finger before starting
  while (particleSensor.getIR() < fingerThreshold) {
    Serial.println("Place finger on sensor");
    updateTemperature();   // v3.0: the temperature keeps running even without a finger
    delay(200);
  }

  //read the first 100 samples, and determine the signal range
  for (byte i = 0; i < bufferLength; i++) {
    while (particleSensor.available() == false)  //do we have new data?
      particleSensor.check();                    //Check the sensor for new data

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample();  //We're finished with this sample so move to next sample
    updateTemperature();   // v3.0: it only does something once per second
  }

  //calculate heart rate and SpO2 after first 100 samples (first 4 seconds of samples)
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

  // v3.0: fresh start every time a finger is placed
  secondsSinceStart = 0;
  haveLastHR = false;
  unsteadyCount = 0;

  //Continuously taking samples from MAX30102.  Heart rate and SpO2 are calculated every 1 second
  while (1) {
    bool fingerOn = true;   // v2.2: reset every pass

    //dumping the first 25 sets of samples in the memory and shift the last 75 sets of samples to the top
    for (byte i = 25; i < 100; i++) {
      redBuffer[i - 25] = redBuffer[i];
      irBuffer[i - 25] = irBuffer[i];
    }

    //take 25 sets of samples before calculating the heart rate.
    for (byte i = 75; i < 100; i++) {
      while (particleSensor.available() == false)  //do we have new data?
        particleSensor.check();                    //Check the sensor for new data

      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
      particleSensor.nextSample();  //We're finished with this sample so move to next sample

      if (irBuffer[i] < fingerThreshold) 
      {
        fingerOn = false;
      }
    }

    // v2.2: leave the cycle if the finger was removed
    if (fingerOn == false) 
    {   
      Serial.println("Finger removed");
      break; // Exit the while(1) loop     
    }

    //After gathering 25 new samples recalculate HR and SP02
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

    // v2.2: print once per second, only when both results are valid
    // v3.0: ...and suspicious values are now printed with a label instead of being skipped
    secondsSinceStart++;   // one pass of this loop is about one second

    if (validHeartRate == 1 && validSPO2 == 1) 
    { 
      // v3.0: three separate yes/no questions about the reading
      bool hrPlausible = (heartRate >= minHeartRate && heartRate <= maxHeartRate);
      bool spo2Plausible = (spo2 >= minSpO2 && spo2 <= maxSpO2);
      bool hrSteady = (!haveLastHR || abs(heartRate - lastGoodHR) <= maxHRJump);

      const char* label = "";   // v3.0: what gets printed after the numbers

      if (secondsSinceStart < settleSeconds)
      {
        label = "  (settling)";   // the signal needs a few seconds to settle
      }
      else if (!hrPlausible || !spo2Plausible)
      {
        label = "  (probably noise)";
      }
      else if (!hrSteady && unsteadyCount < maxUnsteady)
      {
        unsteadyCount++;   // count it, so we never get stuck labelling everything as noise
        label = "  (probably noise)";
      }
      else
      {
        // it passed every check, so remember it for the jump check next second
        lastGoodHR = heartRate;
        haveLastHR = true;
        unsteadyCount = 0;
      }

      // v3.1: alert LED - only fires on readings that passed every check
      bool hrAlert = (heartRate > 140 || heartRate < 65);
      bool spo2Alert = (spo2 < 93);
      if (hrAlert || spo2Alert) 
      {
        digitalWrite(alertLedPin, HIGH);
      } 
      else 
      {
        digitalWrite(alertLedPin, LOW);
      }

      Serial.print("HR=");
      Serial.print(heartRate);
      Serial.print("  SpO2=");
      Serial.print(spo2);
      Serial.println(label);
    }
    else
    {
      // v3.0: the library itself said it could not find a reliable pulse
      Serial.println("No valid HR/SpO2 reading yet");
    }

    updateTemperature();   // v3.0: it only does something once per second
  }
}
