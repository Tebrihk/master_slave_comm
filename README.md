# master_slave_comm
# Master-Slave RF24 Bulb Control System

## Overview

This project implements a Master-Slave system where two devices communicate using RF24 (nRF24L01) modules to control the status of a green bulb on both devices. The Master device sends a signal to the Slave device, and based on the communication, the green bulb blinks on both devices. Once the Slave device acknowledges the message by pressing a button, both the Master and Slave bulbs stop blinking.

## Components Required

- 2x Arduino (Master and Slave devices)
- 2x nRF24L01 modules
- 2x Green LEDs (for both Master and Slave)
- 1x Red LED (optional)
- 1x Blue LED (optional)
- 2x Push buttons (for Slave to acknowledge)
- Jumper wires
- Breadboard

## Wiring

### Master Device:
- nRF24L01 module:
  - VCC -> 3.3V
  - GND -> GND
  - CE -> Pin 7
  - CSN -> Pin 8
  - SCK -> Pin 13
  - MOSI -> Pin 11
  - MISO -> Pin 12
- Green LED -> Pin 4
- Red LED (optional) -> Pin 3
- Blue LED (optional) -> Pin 5
- Push button (optional) -> Pin 6 (used for manual input)

### Slave Device:
- nRF24L01 module:
  - VCC -> 3.3V
  - GND -> GND
  - CE -> Pin 53
  - CSN -> Pin 48
  - SCK -> Pin 13
  - MOSI -> Pin 11
  - MISO -> Pin 12
- Green LED -> Pin 4
- Red LED (optional) -> Pin 7
- Push button -> Pin 6

## Software Libraries

This project requires the following libraries:
- **SPI**: Standard SPI library for Arduino
- **RF24**: Library to interface with the nRF24L01 modules
- **RF24Network**: Library for creating networked communication between Arduino devices

### Install Libraries

1. Open the Arduino IDE.
2. Go to **Sketch > Include Library > Manage Libraries**.
3. In the Library Manager, search for `RF24` and `RF24Network`.
4. Install both libraries.

## Setup and Operation

### Master Device

1. The Master device will constantly listen for a button press on the Slave device.
2. When the Slave button is pressed, the Master sends a signal to the Slave to initiate blinking of the green LED.
3. The Master then starts blinking its green LED for a duration (1 second) after receiving the signal from the Slave.

### Slave Device

1. The Slave device listens for a signal from the Master device.
2. Upon receiving a signal from the Master, the Slave device starts blinking its green LED for the same duration.
3. After acknowledging the signal, the green LED on both Master and Slave turns off.

## Code Explanation

### Master Code

The Master code listens for button presses and sends a signal to the Slave when the button is pressed. The green bulb will blink for a defined period when the signal is sent. The code uses `RF24Network` to handle communication between the Master and Slave.

```cpp
while (network.available()) {
  RF24NetworkHeader header;
  payload_t response;
  network.read(header, &response, sizeof(response));
  greenBlink = true;
  if (greenBlink) {
    greenBlink = false;
    digitalWrite(bulbGreen, LOW);
  }
}
