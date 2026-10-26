# Learning Resources: Wearable Health Monitor (ESP32-WROOM-32)

Links for coding the MAX30102 heart rate/SpO2 sensor, the DS18B20 temperature sensor, and the Bluetooth Low Energy (BLE) connection to a phone.

> **Board note:** our worksheet design uses an **ESP32-WROOM-32** (I2C on GPIO21/22, DS18B20 on GPIO4, buttons on GPIO13/14). Most tutorials below use this same board, so their pin numbers match ours. Follow **BLE** tutorials, since many App Inventor examples use Classic Bluetooth (`BluetoothSerial`), which wouldn't suit our phone-relay design.

---

## Our board

| Item | Details |
|---|---|
| Board | ESP32 38-Pin WiFi + Bluetooth NodeMCU Development Board (ESP-WROOM-32 module) |
| Where | Robocraze: https://robocraze.com/products/esp32-development-board |
| Price | about ₹425 incl. GST at the time of checking (check the live price when ordering) |
| Delivery | Free above ₹999, so order it together with the sensors |
| Returns | 7 days, for non-working or damaged items only. Test it on arrival. |
| Pinout | The product page has a 38-pin pinout diagram. Check it before wiring. |

**Upload tip:** when uploading code, hold the **IO0 (BOOT)** button for 1 to 2 seconds, as the product page advises.

**Practical tip:** 38-pin boards are wide and often cover most of a breadboard, so keep a second breadboard or extra jumper wires handy. While using the microcontroller, I realized that Female to female wires are not necessary, so i used my female to male wires in order to connect it to my breadboard. However, your ESP32 may turn in weird angles, so i recommend attaching it to a hard surface with screws

---

## 1. MAX30102 (heart rate + SpO2)

| Resource | What it's good for | Link |
|---|---|---|
| Last Minute Engineers: Interfacing MAX30102 with Arduino | Best starting point: how the sensor works plus working examples | https://lastminuteengineers.com/max30102-pulse-oximeter-heart-rate-sensor-arduino-tutorial/ |
| SparkFun MAX3010x Sensor Library (GitHub) | The library we install. The `/examples` folder has beat detection and SpO2 sketches. Works with the MAX30102. | https://github.com/sparkfun/SparkFun_MAX3010x_Sensor_Library |
| SparkFun MAX3010x library on Arduino docs | Install via the Arduino IDE Library Manager | https://docs.arduino.cc/libraries/sparkfun-max3010x-pulse-and-proximity-sensor-library/ |
| Zbotic: MAX30102 SpO2 and Heart Rate with Arduino | Indian supplier tutorial, includes accuracy notes | https://zbotic.in/max30102-pulse-oximeter-spo2-and-heart-rate-arduino/ |
| Microcontrollers Lab: MAX30102 with Arduino | Alternative walkthrough | https://microcontrollerslab.com/max30102-pulse-oximeter-heart-rate-sensor-arduino/ |
| CircuitDigest: How MAX30102 works | Explains the working principle plus interfacing | https://circuitdigest.com/microcontroller-projects/how-max30102-pulse-oximeter-and-heart-rate-sensor-works-and-how-to-interface-with-arduino |

**Tip:** run the SparkFun example sketches unchanged first, to confirm the sensor works, before writing any of our own code.

---

## 2. DS18B20 (temperature)

| Resource | What it's good for | Link |
|---|---|---|
| Random Nerd Tutorials: ESP32 DS18B20 with Arduino IDE | Wiring, the OneWire and DallasTemperature libraries, and complete reading code | https://randomnerdtutorials.com/esp32-ds18b20-temperature-arduino-ide/ |
| Random Nerd Tutorials: ESP32 sensor guides index | Many other ESP32 sensor tutorials in one place | https://randomnerdtutorials.com/esp32-guides-sensors-modules/ |

**Libraries needed:** `OneWire` and `DallasTemperature` (both in the Arduino Library Manager). Remember the 4.7k pull-up resistor between the data line and 3.3V.

---

## 3. Bluetooth Low Energy (BLE) on ESP32

| Resource | What it's good for | Link |
|---|---|---|
| Random Nerd Tutorials: Getting Started with ESP32 BLE | Start here: BLE concepts (server, service, characteristic) plus a basic server example | https://randomnerdtutorials.com/esp32-bluetooth-low-energy-ble-arduino-ide/ |
| Random Nerd Tutorials: ESP32 BLE Server and Client | A server that sends sensor readings, and a client that reads them | https://randomnerdtutorials.com/esp32-ble-server-client/ |
| Random Nerd Tutorials: BLE Environmental Sensing Service | Exposes sensor data using a standard BLE structure. Closest to what we need. | https://randomnerdtutorials.com/esp32-ble-server-environmental-sensing-service |
| Random Nerd Tutorials: ESP32 Bluetooth Ultimate Guide | Overview of all the ESP32 Bluetooth options | https://randomnerdtutorials.com/esp32-bluetooth-guide/ |
| Random Nerd Tutorials: ESP32 Web Bluetooth | Alternative: read BLE data in a web page instead of a phone app | https://randomnerdtutorials.com/esp32-web-bluetooth/ |

---

## 4. Phone app (caregiver's phone)

| Resource | What it's good for | Link |
|---|---|---|
| MIT App Inventor: BluetoothLE extension intro | Official guide to the BLE extension for building the phone app with drag-and-drop blocks | http://iot.appinventor.mit.edu/#/bluetoothle/bluetoothleintro |
| MIT App Inventor + IoT: Basic Connection (PDF) | Step-by-step: scan for and connect to a BLE device | https://iot.appinventor.mit.edu/assets/howtos/MIT_App_Inventor_Basic_Connection.pdf |
| MIT App Inventor community thread: BLE with ESP32 | Real-world warning: the ESP32 may not reconnect after a disconnect unless the code restarts BLE advertising on disconnect | https://community.appinventor.mit.edu/t/bluetoothle-extension-very-confusing-esp32-c3-communication/160633/11 |

**Note:** the BLE extension tutorial above is marked "may be obsolete" in places, so test it early. The community thread above also explains a common bug (the board won't reconnect until reset). Fix it by handling `onDisconnect` and restarting advertising in the ESP32 code.

---

## 5. Not yet verified (search for these yourself)

These are tools worth finding, but I haven't confirmed exact page URLs:

- **nRF Connect for Mobile** (Nordic Semiconductor, free phone app): lets you connect to the ESP32 and see its BLE data before the real app exists. Search the Play Store or App Store for "nRF Connect".
- **Arduino-ESP32 official documentation** (Espressif): search "Arduino-ESP32 BLE docs" for the reference and examples.
- **Board pinout**: once the board is bought, search "[your exact board name] pinout" to confirm GPIO21/22/4/13/14 are available on it.

---

## Suggested learning order

1. DS18B20 alone (easiest quick win)
2. MAX30102 alone, using the SparkFun examples
3. A BLE "hello world", viewed in nRF Connect
4. Combine all three
5. Build the phone app

Test each piece on its own before combining, so any bug is easy to trace.
