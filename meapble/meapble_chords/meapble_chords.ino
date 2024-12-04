/*
  Chords
 */

#define CONTROL_RATE 128  // Hz, powers of 2 are most reliable
#include <Meap.h>         // MEAP library, includes all dependent libraries, including all Mozzi modules
#include <ArduinoBLE.h>   // ESP32 Bluetooth Connectivity library

Meap meap;                                            // creates MEAP object to handle inputs and other MEAP library functions
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);  // defines MIDI in/out ports

// ---------- YOUR GLOBAL VARIABLES BELOW ----------
#include <tables/sin8192_int8.h>  // loads sine wavetable

// Creating oscillators for chords
mOscil<8192, AUDIO_RATE> sine1(SIN8192_DATA);  // Root note
mOscil<8192, AUDIO_RATE> sine2(SIN8192_DATA);  // Third
mOscil<8192, AUDIO_RATE> sine3(SIN8192_DATA);  // Fifth

// Add envelope for amplitude control
ADSR <CONTROL_RATE, AUDIO_RATE> envelope;

int octave_multiplier = 1;  // Changes frequency octave
bool pad_active = false;    // Track if any pad is currently active
int current_pad = -1;      // Track which pad is currently active

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

// Variables to store current frequencies
float current_freq1 = 0;
float current_freq2 = 0;
float current_freq3 = 0;

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
    
    // Initialize envelope
    envelope.setADLevels(255, 255);
    envelope.setTimes(50, 200, 10000, 200); // 50ms attack, 200ms decay, 10s sustain, 200ms release
    
    // Initialize all oscillators to 0 frequency
    sine1.setFreq(0);
    sine2.setFreq(0);
    sine3.setFreq(0);
    
    Serial.println("Setup complete! Waiting for BLE connections...");
}

void loop() {
    audioHook();
    
    BLEDevice central = BLE.central();
    
    if (central) {
        Serial.print("Connected to central: ");
        Serial.println(central.address());
        
        while (central.connected()) {
            audioHook();
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
        }
        Serial.println("Disconnected from central");
    }
}

void updateControl() {
    meap.readInputs();  // Read all inputs including potentiometers
    envelope.update();

    // Always apply potentiometer controls, but blend with pad frequencies when pad is active
    float my_freq1 = map(blePot1Value, 0, 4095, 20, 2000); 
    float my_freq2 = map(blePot2Value, 0, 4095, 20, 2000); 
    float my_freq3 = map(blePot3Value, 0, 4095, 20, 2000); 

    if (!pad_active) {
        // When no pad is active, use direct potentiometer control
        current_freq1 = my_freq1 * octave_multiplier;
        current_freq2 = my_freq2 * octave_multiplier;
        current_freq3 = my_freq3 * octave_multiplier;
    } else {
        // When pad is active, use pot values as frequency modifiers
        // Modify the pad frequencies by the pot values (scaled down for subtle effect)
        current_freq1 = current_freq1 + (my_freq1 - 1000) * 0.1;
        current_freq2 = current_freq2 + (my_freq2 - 1000) * 0.1;
        current_freq3 = current_freq3 + (my_freq3 - 1000) * 0.1;
    }
    
    // Apply the frequencies to the oscillators
    sine1.setFreq(current_freq1);
    sine2.setFreq(current_freq2);
    sine3.setFreq(current_freq3);
}

AudioOutput_t updateAudio() {
    int64_t out_sample = (sine1.next() + sine2.next() + sine3.next()) / 3;  // Mix and normalize
    out_sample = (out_sample * envelope.next()) >> 8;  // Apply envelope
    return StereoOutput::fromNBit(8, (out_sample * meap.volume_val) >> 12, (out_sample * meap.volume_val) >> 12);
}

void updateTouch(int number, bool pressed) {
    if (pressed) {  // Any pad pressed
        pad_active = true;
        current_pad = number;
        envelope.noteOn();
    } else {  // Any pad released
        if (number == current_pad) {  // Only release if it's the current active pad
            pad_active = false;
            current_pad = -1;
            envelope.noteOff();
            // When released, set all oscillators and stored frequencies to 0
            current_freq1 = 0;
            current_freq2 = 0;
            current_freq3 = 0;
            sine1.setFreq(0);
            sine2.setFreq(0);
            sine3.setFreq(0);
        }
    }

    switch (number) {
        case 0:
            if (pressed) {  // Pad 0 pressed
                Serial.println("t0 pressed ");
                // A minor chord (A3, C4, E4)
                current_freq1 = 220.00 * octave_multiplier;   // A3
                current_freq2 = 261.63 * octave_multiplier;   // C4
                current_freq3 = 329.63 * octave_multiplier;   // E4
                sine1.setFreq(current_freq1);
                sine2.setFreq(current_freq2);
                sine3.setFreq(current_freq3);
            }
            break;
        case 1:
            if (pressed) {  // Pad 1 pressed
                Serial.println("t1 pressed");
                // B minor chord (B3, D4, F#4)
                current_freq1 = 246.94 * octave_multiplier;   // B3
                current_freq2 = 293.66 * octave_multiplier;   // D4
                current_freq3 = 369.99 * octave_multiplier;   // F#4
                sine1.setFreq(current_freq1);
                sine2.setFreq(current_freq2);
                sine3.setFreq(current_freq3);
            }
            break;
        case 2:
            if (pressed) {  // Pad 2 pressed
                Serial.println("t2 pressed");
                // C# minor chord (C#4, E4, G#4)
                current_freq1 = 277.18 * octave_multiplier;   // C#4
                current_freq2 = 329.63 * octave_multiplier;   // E4
                current_freq3 = 415.30 * octave_multiplier;   // G#4
                sine1.setFreq(current_freq1);
                sine2.setFreq(current_freq2);
                sine3.setFreq(current_freq3);
            }
            break;
        case 3:
            if (pressed) {  // Pad 3 pressed
                Serial.println("t3 pressed");
                // D major chord (D4, F#4, A4)
                current_freq1 = 293.66 * octave_multiplier;   // D4
                current_freq2 = 369.99 * octave_multiplier;   // F#4
                current_freq3 = 440.00 * octave_multiplier;   // A4
                sine1.setFreq(current_freq1);
                sine2.setFreq(current_freq2);
                sine3.setFreq(current_freq3);
            }
            break;
        case 4:
            if (pressed) {  // Pad 4 pressed
                Serial.println("t4 pressed");
                // E minor chord (E4, G4, B4)
                current_freq1 = 329.63 * octave_multiplier;   // E4
                current_freq2 = 392.00 * octave_multiplier;   // G4
                current_freq3 = 493.88 * octave_multiplier;   // B4
                sine1.setFreq(current_freq1);
                sine2.setFreq(current_freq2);
                sine3.setFreq(current_freq3);
            }
            break;
        case 5:
            if (pressed) {  // Pad 5 pressed
                Serial.println("t5 pressed");
                // F# minor chord (F#4, A4, C#5)
                current_freq1 = 369.99 * octave_multiplier;   // F#4
                current_freq2 = 440.00 * octave_multiplier;   // A4
                current_freq3 = 554.37 * octave_multiplier;   // C#5
                sine1.setFreq(current_freq1);
                sine2.setFreq(current_freq2);
                sine3.setFreq(current_freq3);
            }
            break;
        case 6:
            if (pressed) {  // Pad 6 pressed
                Serial.println("t6 pressed");
                // G# minor chord (G#4, B4, D#5)
                current_freq1 = 415.30 * octave_multiplier;   // G#4
                current_freq2 = 493.88 * octave_multiplier;   // B4
                current_freq3 = 622.25 * octave_multiplier;   // D#5
                sine1.setFreq(current_freq1);
                sine2.setFreq(current_freq2);
                sine3.setFreq(current_freq3);
            }
            break;
        case 7:
            if (pressed) {  // Pad 7 pressed
                Serial.println("t7 pressed");
                // A major chord (A4, C#5, E5)
                current_freq1 = 440.00 * octave_multiplier;   // A4
                current_freq2 = 554.37 * octave_multiplier;   // C#5
                current_freq3 = 659.25 * octave_multiplier;   // E5
                sine1.setFreq(current_freq1);
                sine2.setFreq(current_freq2);
                sine3.setFreq(current_freq3);
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