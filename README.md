
# MEAP with Arduino BLE Implementation

This project demonstrates an interactive audio playback system controlled via Bluetooth® Low Energy (BLE) and potentiometers. It leverages the MEAP library for MIDI and audio handling, as well as the ArduinoBLE library for BLE connectivity.

The initial BLE implementation for this project was prototyped using the guide available at [Mason Mann's MEAP Documentation](https://masonmann.online/electronics/meap/).

---

## Features

- BLE peripheral and central mode support.

- Playback control of audio samples using potentiometers.

- MIDI connectivity.

- Real-time frequency control of audio samples.

- Support for multiple audio samples, including drums, melodies, and effects.

---

## Prerequisites

### Hardware

- Arduino-compatible board with BLE support (e.g., ESP32).

- Potentiometers connected to analog inputs.

- External audio hardware (e.g., speakers or amplifiers) for audio output.

### Software

- Arduino IDE with necessary libraries installed:

  - [MEAP Library](https://github.com/Mozzi/MEAP)

  - ArduinoBLE library

- Node.js and npm for hosting the development server (for the `meap-web` interface).

## Important Note: BLE UUID Configuration

The BLE UUID is used to identify the service for communication between the Arduino and the JavaScript-based `meap-web` interface.

The UUID in the Arduino code (`BLEService meapService(...)`) must match the UUID used in the `meap-web` JavaScript setup files. This value **must be lowercase** and correctly formatted.

### Generating a UUID

Here are some ways to generate a valid UUID:

- **Linux/macOS:** Use the `uuidgen` command in the terminal:

   ```bash
   uuidgen | tr '[:upper:]' '[:lower:]'

Windows: Use PowerShell:

powershell

   ```bash
   [guid]::NewGuid().ToString().ToLower()

Online UUID Generators: Use an online tool such as UUIDGenerator.net. Ensure you convert the result to lowercase if it's not already.

Updating the UUID

Replace the UUID in the following line in your Arduino code:

   ```cpp

   BLEService meapService("a597fb2d-5947-5bb6-9b20-9fa9ec68916f");

Ensure the same UUID is updated in the corresponding JavaScript setup file within meap-web to maintain compatibility.

---

## Setup and Usage

### 1. Hardware Setup

- Connect the MEAP-compatible board to your computer using a USB cable.

- Connect potentiometers and ensure they are mapped correctly in your code.

- Verify that the BLE hardware is functioning.

### 2. Install Dependencies

- Ensure the required libraries are installed in your Arduino IDE:

- MEAP

- ArduinoBLE

- Use the Arduino Library Manager or add the libraries manually if they are not installed.

### 3. Upload the Code

- Open the project in the Arduino IDE or any preferred IDE with Arduino CLI support.

- Verify the code for any hardware-specific adjustments.

- Upload the code to your MEAP-compatible board.

### 4. Web Interface Setup

1\. Navigate to the `meap-web` directory:

   ```bash

   cd meap-web


2\. Install the required Node.js dependencies:

   ```bash

   npm install


3\. Start the hosting environment:

   ```bash

   npm start


4\. Open the hosting website in Google Chrome (other browsers are not supported for this version):

   ```bash

   google-chrome http://localhost:3000


5\. Follow these steps in the browser:

- Open the log to monitor connection messages.

- Connect to the MEAP device via BLE.

- Wait for the log message:

   ```bash

   MEAP Connected!


### Have Fun with Bluetooth Music!!!