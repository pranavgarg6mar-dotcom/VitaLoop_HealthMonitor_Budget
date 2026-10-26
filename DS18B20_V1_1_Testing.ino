/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com  
*********/

#include <OneWire.h>
#include <DallasTemperature.h>

// GPIO where the DS18B20 is connected to
const int oneWireBus = 4;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

// V1.1 settings, kept at the top so they are easy to change
const float minTemp = 20.0;
const float maxTemp = 45.0;
const float maxJump = 3.0;

float lastGoodTemp = 0;
bool haveLastTemp = false;   // false until the first good reading

void setup() {
  // Start the Serial Monitor
  Serial.begin(115200);
  // Start the DS18B20 sensor
  sensors.begin();
}

void loop() {
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  float temperatureF = sensors.getTempFByIndex(0);

  if (temperatureC == -127) 
  {
    Serial.println("DS18B20 not found. Check wiring and the 4.7k resistor.");
  }

  else if (temperatureC == 85) 
  {
    Serial.println("Power-up value, not a real reading.");
  }

  else if (temperatureC < minTemp || temperatureC >  maxTemp) 
  {
    Serial.println("Out of plausible range.");
  }

  else if (haveLastTemp && abs(TemperatureC - lastGoodTemp) > maxTemp ) 
  {
    Serial.println("Jumped too much, probably noise.");
  }
  
  else {
    // good reading
    Serial.print(temperatureC);
    Serial.println("ºC");
    Serial.print(temperatureF);
    Serial.println("ºF");
    lastGoodTemp = temperatureC ;
    haveLastTemp = true ;
  }

  delay(1000);
}