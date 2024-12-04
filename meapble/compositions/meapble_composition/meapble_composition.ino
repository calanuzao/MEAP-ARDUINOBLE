/*
  Sample playback with BLE Potentiometer Control
 */

#define CONTROL_RATE 128  // Hz, powers of 2 are most reliable
#include <Meap.h>         // MEAP library, includes all dependent libraries, including all Mozzi modules
#include <ArduinoBLE.h>   // ESP32 Bluetooth Connectivity library

Meap meap;                                            // creates MEAP object to handle inputs and other MEAP library functions
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);  // defines MIDI in/out ports

// ---------- YOUR GLOBAL VARIABLES BELOW ----------
// Include Samples
#include "../../samples/melody.h"
#include "../../samples/drum_loop_hiphop_110bpm.h"
#include "../../samples/kick.h"
#include "../../samples/snare1.h"
#include "../../samples/snare2.h"
#include "../../samples/snare3.h"
#include "../../samples/shaker.h"

// Sample players
mSample<melody_NUM_CELLS, AUDIO_RATE, int8_t> melody_sample(melody_DATA);
mSample<antianrose__eliemusic_garota_drum_loop_hiphop_110bpm_NUM_CELLS, AUDIO_RATE, int8_t> drum_sample(antianrose__eliemusic_garota_drum_loop_hiphop_110bpm_DATA);
mSample<antianrose__eliemusic_garota_drum_loop_hiphop_110bpm_NUM_CELLS, AUDIO_RATE, int8_t> perc_sample(antianrose__eliemusic_garota_drum_loop_hiphop_110bpm_DATA);
mSample<kick_NUM_CELLS, AUDIO_RATE, int8_t> kick_sample(kick_DATA);
mSample<snare1_NUM_CELLS, AUDIO_RATE, int8_t> snare1_sample(snare1_DATA);
mSample<snare2_NUM_CELLS, AUDIO_RATE, int8_t> snare2_sample(snare2_DATA);
mSample<snare3_NUM_CELLS, AUDIO_RATE, int8_t> snare3_sample(snare3_DATA);
mSample<shaker_NUM_CELLS, AUDIO_RATE, int8_t> shaker_sample(shaker_DATA);

// Default frequencies for samples
float melody_default_freq;
float drum_default_freq;
float perc_default_freq;
float kick_default_freq;
float snare1_default_freq;
float snare2_default_freq;
float snare3_default_freq;
float shaker_default_freq;

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
    
    // Initialize MEAP and audio
    Serial.println("Initializing MEAP...");
    Serial1.begin(31250, SERIAL_8N1, 43, 44);
    startMozzi(CONTROL_RATE);
    meap.begin();
    
    // Set initial sample playback rates
    melody_default_freq = (float)melody_SAMPLERATE / (float)melody_NUM_CELLS;
    drum_default_freq = (float)antianrose__eliemusic_garota_drum_loop_hiphop_110bpm_SAMPLERATE / (float)antianrose__eliemusic_garota_drum_loop_hiphop_110bpm_NUM_CELLS;
    perc_default_freq = (float)antianrose__eliemusic_garota_drum_loop_hiphop_110bpm_SAMPLERATE / (float)antianrose__eliemusic_garota_drum_loop_hiphop_110bpm_NUM_CELLS;
    kick_default_freq = (float)kick_SAMPLERATE / (float)kick_NUM_CELLS;
    snare1_default_freq = (float)snare1_SAMPLERATE / (float)snare1_NUM_CELLS;
    snare2_default_freq = (float)snare2_SAMPLERATE / (float)snare2_NUM_CELLS;
    snare3_default_freq = (float)snare3_SAMPLERATE / (float)snare3_NUM_CELLS;
    shaker_default_freq = (float)shaker_SAMPLERATE / (float)shaker_NUM_CELLS;
    
    melody_sample.setFreq(melody_default_freq);
    drum_sample.setFreq(drum_default_freq);
    perc_sample.setFreq(perc_default_freq);
    kick_sample.setFreq(kick_default_freq);
    snare1_sample.setFreq(snare1_default_freq);
    snare2_sample.setFreq(snare2_default_freq);
    snare3_sample.setFreq(snare3_default_freq);
    shaker_sample.setFreq(shaker_default_freq);
    
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
    
    audioHook();  // Keep audio processing running even when not connected
}

void updateControl() {
    meap.readInputs();  // Read all inputs including potentiometers
    
    // Map pot values to playback speeds (0.5x to 2x)
    float speed1 = map(blePot1Value, 0, 4095, 5, 20) / 10.0f;  // Convert to 0.5 - 2.0 range
    float speed2 = map(blePot2Value, 0, 4095, 5, 20) / 10.0f;
    float speed3 = map(blePot3Value, 0, 4095, 5, 20) / 10.0f;
    
    // Update sample frequencies based on pot values
    melody_sample.setFreq(melody_default_freq * speed1);
    drum_sample.setFreq(drum_default_freq * speed2);
    perc_sample.setFreq(perc_default_freq * speed3);
}

AudioOutput_t updateAudio() {
    // Mix all sample outputs
    int64_t mix = (melody_sample.next() + 
                   drum_sample.next() + 
                   perc_sample.next() + 
                   kick_sample.next() + 
                   snare1_sample.next() + 
                   snare2_sample.next() + 
                   snare3_sample.next() + 
                   shaker_sample.next()) >> 3;  // Divide by 8 to prevent clipping
    
    return StereoOutput::fromNBit(8, (mix * meap.volume_val) >> 12, (mix * meap.volume_val) >> 12);
}

void updateTouch(int number, bool pressed) {
    if (pressed) {
        switch (number) {
            case 0:  // Melody
                Serial.println("Pad 0 - Melody");
                melody_sample.start();
                break;
                
            case 1:  // Drum loop
                Serial.println("Pad 1 - Drum Loop");
                drum_sample.start();
                break;
                
            case 2:  // Percussion loop
                Serial.println("Pad 2 - Percussion Loop");
                perc_sample.start();
                break;
                
            case 3:  // Kick
                Serial.println("Pad 3 - Kick");
                kick_sample.start();
                break;
                
            case 4:  // Snare 1
                Serial.println("Pad 4 - Snare 1");
                snare1_sample.start();
                break;
                
            case 5:  // Snare 2
                Serial.println("Pad 5 - Snare 2");
                snare2_sample.start();
                break;
                
            case 6:  // Snare 3
                Serial.println("Pad 6 - Snare 3");
                snare3_sample.start();
                break;
                
            case 7:  // Shaker
                Serial.println("Pad 7 - Shaker");
                shaker_sample.start();
                break;
        }
    }
}

void updateDip(int number, bool up) {
    switch (number) {
        case 0:  // DIP 0 - Start/Stop melody
            if (up) {
                melody_sample.start();
                Serial.println("Melody started");
            } else {
                melody_sample.stop();
                Serial.println("Melody stopped");
            }
            break;
            
        case 1:  // DIP 1 - Start/Stop drum loop
            if (up) {
                drum_sample.start();
                Serial.println("Drum loop started");
            } else {
                drum_sample.stop();
                Serial.println("Drum loop stopped");
            }
            break;
            
        case 2:  // DIP 2 - Start/Stop percussion loop
            if (up) {
                perc_sample.start();
                Serial.println("Percussion loop started");
            } else {
                perc_sample.stop();
                Serial.println("Percussion loop stopped");
            }
            break;
    }
}