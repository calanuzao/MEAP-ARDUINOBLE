/*
  Constant sine wave output with BLE Potentiometer Control
 */

#define CONTROL_RATE 128  // Hz, powers of 2 are most reliable
#include <Meap.h>         // MEAP library, includes all dependent libraries, including all Mozzi modules
#include <ArduinoBLE.h>   // ESP32 Bluetooth Connectivity library

Meap meap;                                            // creates MEAP object to handle inputs and other MEAP library functions
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);  // defines MIDI in/out ports

// ---------- YOUR GLOBAL VARIABLES BELOW ----------
#include <tables/sin8192_int8.h>  // loads sine wavetable

// Creating a single oscillator that shows how many samples are in the wavetable as well as the audio rate
mOscil<8192, AUDIO_RATE> my_sine(SIN8192_DATA);
int octave_multiplier = 1;  // Changes frequency octave

// BLE service UUID
BLEService meapService("a597fb2d-5947-5bb6-9b20-9fa9ec68916f");

// BLE characteristics for each potentiometer
BLEStringCharacteristic pot1Characteristic("a597fb2e-5947-5bb6-9b20-9fa9ec68916f", BLERead | BLEWrite, 20);
BLEStringCharacteristic pot2Characteristic("a597fb2f-5947-5bb6-9b20-9fa9ec68916f", BLERead | BLEWrite, 20);
BLEStringCharacteristic pot3Characteristic("a597fb30-5947-5bb6-9b20-9fa9ec68916f", BLERead | BLEWrite, 20);

// Variables to store potentiometer values
int blePot1Value = 0;
int blePot2Value = 0;
int blePot3Value = 0;

void setup() {
    delay(1000);
    Serial.begin(115200);
    Serial.println("TEST MESSAGE - If you see this, Serial is working");
    Serial.println("Starting MEAP BLE setup...");
    
    // Initialize BLE
    Serial.println("Initializing BLE...");
    if (!BLE.begin()) {
        Serial.println("Failed to start BLE!");
        while (1) {
            delay(1000);
            Serial.println("BLE initialization failed - check hardware");
        }
    }

    Serial.println("BLE initialized successfully!");
    
    // Set up BLE device
    BLE.setDeviceName("MEAP");
    BLE.setLocalName("MEAP");
    
    // Setup service and characteristics
    Serial.println("Setting up BLE service and characteristics...");
    BLE.setAdvertisedService(meapService);
    meapService.addCharacteristic(pot1Characteristic);
    meapService.addCharacteristic(pot2Characteristic);
    meapService.addCharacteristic(pot3Characteristic);
    BLE.addService(meapService);
    
    // Start advertising
    Serial.println("Starting BLE advertising...");
    BLE.advertise();
    
    // Initialize MEAP
    Serial.println("Initializing MEAP...");
    Serial1.begin(31250, SERIAL_8N1, 43, 44);
    startMozzi(CONTROL_RATE);
    meap.begin();
    my_sine.setFreq(220);
    
    Serial.println("Setup complete! Waiting for BLE connections...");
}

void loop() {
    BLEDevice central = BLE.central();
    
    if (central) {
        Serial.print("Connected to central: ");
        Serial.println(central.address());
        
        while (central.connected()) {
            if (pot1Characteristic.written()) {
                String potString = pot1Characteristic.value();
                blePot1Value = constrain(potString.toInt(), 0, 4095);
                Serial.print("Pot1 value received: ");
                Serial.println(blePot1Value);
            }
            if (pot2Characteristic.written()) {
                String potString = pot2Characteristic.value();
                blePot2Value = constrain(potString.toInt(), 0, 4095);
                Serial.print("Pot2 value received: ");
                Serial.println(blePot2Value);
            }
            if (pot3Characteristic.written()) {
                String potString = pot3Characteristic.value();
                blePot3Value = constrain(potString.toInt(), 0, 4095);
                Serial.print("Pot3 value received: ");
                Serial.println(blePot3Value);
            }
          
        audioHook();

        }

        Serial.println("Disconnected from central");
    }

}

void updateControl() {
    meap.readInputs();  // Read all inputs including potentiometers

    // Use either BLE values or physical pot values
    // int pot1Value = (blePot1Value > 0) ? blePot1Value : meap.pot_vals[0];
    // int pot2Value = (blePot2Value > 0) ? blePot2Value : meap.pot_vals[1];
    // int pot3Value = (blePot3Value > 0) ? blePot3Value : meap.pot_vals[2];

    // Map pot1 to frequency range (adjust the ranges for each pot as needed)
    float my_freq = map(blePot1Value, 0, 4095, 20, 2000); 
  
    // // Map pot2 to something else
    // float my_something_else = map(blePot2Value, 0, 4095, 0, 255);

    // // Map pot3 to something else
    // float my_something_else = map(blePot3Value, 0, 4095, 0, 100);
    
    // Control blepotentiometers
    my_sine.setFreq(my_freq * octave_multiplier); // pot1

}

AudioOutput_t updateAudio() {
    int64_t out_sample = my_sine.next();
    return StereoOutput::fromNBit(8, (out_sample * meap.volume_val) >> 12, (out_sample * meap.volume_val) >> 12);
}

/**
   * Runs whenever a touch pad is pressed or released
   *
   * int number: the number (0-7) of the pad that was pressed
   * bool pressed: true indicates pad was pressed, false indicates it was released
   */
void updateTouch(int number, bool pressed) {
  if (pressed) {  // Any pad pressed

  } else {  // Any pad released
  }
  switch (number) {
    case 0:
      if (pressed) {  // Pad 0 pressed
        Serial.println("t0 pressed ");
        my_sine.setFreq(220);  // A3
      } else {                 // Pad 0 released  
        Serial.println("t0 released");
      }
      break;
    case 1:
      if (pressed) {  // Pad 1 pressed
        Serial.println("t1 pressed");
        my_sine.setFreq(246.94);  // B3
      } else {                    // Pad 1 released
        Serial.println("t1 released");
      }
      break;
    case 2:
      if (pressed) {  // Pad 2 pressed
        Serial.println("t2 pressed");
        my_sine.setFreq(277.18);  // C#4
      } else {                    // Pad 2 released
        Serial.println("t2 released");
      }
      break;
    case 3:
      if (pressed) {  // Pad 3 pressed
        Serial.println("t3 pressed");
        my_sine.setFreq(293.66);  // D4
      } else {                    // Pad 3 released
        Serial.println("t3 released");
      }
      break;
    case 4:
      if (pressed) {  // Pad 4 pressed
        Serial.println("t4 pressed");
        my_sine.setFreq(329.63);  // E4
      } else {                    // Pad 4 released
        Serial.println("t4 released");
      }
      break;
    case 5:
      if (pressed) {  // Pad 5 pressed
        Serial.println("t5 pressed");
        my_sine.setFreq(369.99);  // F#4
      } else {                    // Pad 5 released
        Serial.println("t5 released");
      }
      break;
    case 6:
      if (pressed) {  // Pad 6 pressed
        Serial.println("t6 pressed");
        my_sine.setFreq(415.3);  // G#4
      } else {                   // Pad 6 released
        Serial.println("t6 released");
      }
      break;
    case 7:
      if (pressed) {  // Pad 7 pressed
        Serial.println("t7 pressed");
        my_sine.setFreq(440);  // A4
      } else {                 // Pad 7 released
        Serial.println("t7 released");
      }
      break;
  }
}

/**
   * Runs whenever a DIP switch is toggled
   *
   * int number: the number (0-7) of the switch that was toggled
   * bool up: true indicated switch was toggled up, false indicates switch was toggled
   */
void updateDip(int number, bool up) {
  if (up) {  // Any DIP toggled up

  } else {  //Any DIP toggled down
  }
  switch (number) {
    // case 0 switch changes frequency octave
    case 0:
      if (up) {  // DIP 0 up
        Serial.println("d0 up");
        octave_multiplier = 2;
      } else {  // DIP 0 down
        Serial.println("d0 down");
        octave_multiplier = 1;
      }
      break;
    case 1:
      if (up) {  // DIP 1 up
        Serial.println("d1 up");
      } else {  // DIP 1 down
        Serial.println("d1 down");
      }
      break;
    case 2:
      if (up) {  // DIP 2 up
        Serial.println("d2 up");
      } else {  // DIP 2 down
        Serial.println("d2 down");
      }
      break;
    case 3:
      if (up) {  // DIP 3 up
        Serial.println("d3 up");
      } else {  // DIP 3 down
        Serial.println("d3 down");
      }
      break;
    case 4:
      if (up) {  // DIP 4 up
        Serial.println("d4 up");
      } else {  // DIP 4 down
        Serial.println("d4 down");
      }
      break;
    case 5:
      if (up) {  // DIP 5 up
        Serial.println("d5 up");
      } else {  // DIP 5 down
        Serial.println("d5 down");
      }
      break;
    case 6:
      if (up) {  // DIP 6 up
        Serial.println("d6 up");
      } else {  // DIP 6 down
        Serial.println("d6 down");
      }
      break;
    case 7:
      if (up) {  // DIP 7 up
        Serial.println("d7 up");
      } else {  // DIP 7 down
        Serial.println("d7 down");
      }
      break;
  }
}