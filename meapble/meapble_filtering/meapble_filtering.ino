/*
  MEAP with BLE Potentiometer Control and Audio Filtering
  This code combines BLE control with audio processing capabilities:
  - Allows remote control via web app
  - Processes stereo audio input through filters
  - Uses both physical and web-based potentiometers
  - Includes filter presets via touch pads
  - Provides filter mode control via DIP switches
  - Maintains all MEAP's touch and DIP switch functionality
 */

#define CONTROL_RATE 128  // Hz, powers of 2 are most reliable for audio processing
#include <Meap.h>         // MEAP library includes Mozzi and other dependencies
#include <ArduinoBLE.h>   // Library for ESP32 Bluetooth Low Energy functionality

// Create main objects for MEAP and MIDI
Meap meap;                                            // Handles all MEAP board functionality
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);  // Sets up MIDI communication

// ---------- GLOBAL VARIABLES AND OBJECTS ----------

// Create two filter objects for stereo audio processing
MultiResonantFilter l_filter;  // Left channel filter
MultiResonantFilter r_filter;  // Right channel filter

// Define the BLE service UUID - this is unique to our MEAP device
BLEService meapService("a597fb2d-5947-5bb6-9b20-9fa9ec68916f");

// Create BLE characteristics for each potentiometer
BLEStringCharacteristic pot1Characteristic("a597fb2e-5947-5bb6-9b20-9fa9ec68916f", BLERead | BLEWrite, 20);  // Cutoff frequency
BLEStringCharacteristic pot2Characteristic("a597fb2f-5947-5bb6-9b20-9fa9ec68916f", BLERead | BLEWrite, 20);  // Resonance
BLEStringCharacteristic pot3Characteristic("a597fb30-5947-5bb6-9b20-9fa9ec68916f", BLERead | BLEWrite, 20);  // Extra control

// Variables to store values received over BLE
int blePot1Value = 0;  // Stores web app pot1 value (0-4095)
int blePot2Value = 0;  // Stores web app pot2 value (0-4095)
int blePot3Value = 0;  // Stores web app pot3 value (0-4095)

// Structure for filter presets
struct FilterPreset {
    int cutoff;
    int resonance;
};

// Define filter presets for touch pads
const FilterPreset FILTER_PRESETS[] = {
    {255, 128},  // Pad 0: High pass, medium resonance
    {128, 200},  // Pad 1: Band pass, high resonance
    {64, 128},   // Pad 2: Low pass, medium resonance
    {200, 64},   // Pad 3: High-mid pass, low resonance
    {100, 180},  // Pad 4: Low-mid pass, high resonance
    {160, 150},  // Pad 5: Mid pass, medium-high resonance
    {40, 220},   // Pad 6: Very low pass, very high resonance
    {255, 255}   // Pad 7: Highest cutoff and resonance
};

// Filter mode enumeration for DIP switches
enum CustomFilterMode {
    FILTER_MODE_LOW,
    FILTER_MODE_BAND,
    FILTER_MODE_HIGH
};

// Global variables for filter control
CustomFilterMode currentMode = FILTER_MODE_LOW;  // Default to lowpass
bool filterEnabled = true;         // Enable/disable filter processing
bool smoothingEnabled = false;     // Enable/disable value smoothing
float filterMix = 1.0;            // Wet/dry mix (0.0 to 1.0)

void setup() {
    delay(1000);  // Short delay for stability on startup
    Serial.begin(115200);  // Initialize serial communication for debugging
    Serial.println("Starting MEAP BLE setup...");
    
    // Initialize Bluetooth Low Energy
    if (!BLE.begin()) {
        Serial.println("Failed to start BLE!");
        while (1) {
            delay(1000);
            Serial.println("BLE initialization failed - check hardware");
        }
    }

    // Configure BLE device settings
    BLE.setDeviceName("MEAP");
    BLE.setLocalName("MEAP");
    
    // Set up BLE service and characteristics
    BLE.setAdvertisedService(meapService);
    meapService.addCharacteristic(pot1Characteristic);
    meapService.addCharacteristic(pot2Characteristic);
    meapService.addCharacteristic(pot3Characteristic);
    BLE.addService(meapService);
    BLE.advertise();
    
    // Initialize MEAP hardware
    Serial1.begin(31250, SERIAL_8N1, 43, 44);
    startMozzi(CONTROL_RATE);
    meap.begin();
    
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
            }
            if (pot2Characteristic.written()) {
                String potString = pot2Characteristic.value();
                blePot2Value = constrain(potString.toInt(), 0, 4095);
            }
            if (pot3Characteristic.written()) {
                String potString = pot3Characteristic.value();
                blePot3Value = constrain(potString.toInt(), 0, 4095);
            }
            
            audioHook();
        }
        Serial.println("Disconnected from central");
    }
    
    audioHook();
}

void updateControl() {
    meap.readInputs();
    
    int cutoff = BLE.connected() ? 
                 map(blePot1Value, 0, 4095, 0, 255) :
                 map(meap.pot_vals[0], 0, 4095, 0, 255);
                 
    int resonance = BLE.connected() ? 
                    map(blePot2Value, 0, 4095, 0, 255) :
                    map(meap.pot_vals[1], 0, 4095, 0, 255);
    
    if (smoothingEnabled) {
        static int lastCutoff = cutoff;
        static int lastResonance = resonance;
        cutoff = (cutoff + lastCutoff) / 2;
        resonance = (resonance + lastResonance) / 2;
        lastCutoff = cutoff;
        lastResonance = resonance;
    }
    
    l_filter.setCutoffFreqAndResonance(cutoff, resonance);
    r_filter.setCutoffFreqAndResonance(cutoff, resonance);
}

AudioOutput_t updateAudio() {
    int64_t l_sample = meap_input_frame[0];
    int64_t r_sample = meap_input_frame[1];
    
    if (filterEnabled) {
        int64_t l_dry = l_sample;
        int64_t r_dry = r_sample;
        
        l_filter.next(l_sample);
        r_filter.next(r_sample);
        
        switch(currentMode) {
            case FILTER_MODE_LOW:
                l_sample = l_filter.low();
                r_sample = r_filter.low();
                break;
            case FILTER_MODE_BAND:
                l_sample = l_filter.band();
                r_sample = r_filter.band();
                break;
            case FILTER_MODE_HIGH:
                l_sample = l_filter.high();
                r_sample = r_filter.high();
                break;
        }
        
        if (filterMix < 1.0) {
            l_sample = (l_sample * filterMix + l_dry * (1.0 - filterMix));
            r_sample = (r_sample * filterMix + r_dry * (1.0 - filterMix));
        }
    }
    
    return StereoOutput::fromNBit(17, 
        (l_sample * meap.volume_val) >> 12,
        (r_sample * meap.volume_val) >> 12);
}

void updateTouch(int number, bool pressed) {
    if (pressed) {
        int preset_cutoff = FILTER_PRESETS[number].cutoff;
        int preset_resonance = FILTER_PRESETS[number].resonance;
        
        l_filter.setCutoffFreqAndResonance(preset_cutoff, preset_resonance);
        r_filter.setCutoffFreqAndResonance(preset_cutoff, preset_resonance);
        
        Serial.print("Applied filter preset ");
        Serial.print(number);
        Serial.print(" - Cutoff: ");
        Serial.print(preset_cutoff);
        Serial.print(", Resonance: ");
        Serial.println(preset_resonance);
    }
}

void updateDip(int number, bool up) {
    switch (number) {
        case 0:
            if (up) {
                switch(currentMode) {
                    case FILTER_MODE_LOW:
                        currentMode = FILTER_MODE_BAND;
                        Serial.println("Mode: Band Pass");
                        break;
                    case FILTER_MODE_BAND:
                        currentMode = FILTER_MODE_HIGH;
                        Serial.println("Mode: High Pass");
                        break;
                    case FILTER_MODE_HIGH:
                        currentMode = FILTER_MODE_LOW;
                        Serial.println("Mode: Low Pass");
                        break;
                }
            }
            break;
            
        case 1:
            filterEnabled = up;
            Serial.print("Filter: ");
            Serial.println(filterEnabled ? "Enabled" : "Bypassed");
            break;
            
        case 2:
            smoothingEnabled = up;
            Serial.print("Smoothing: ");
            Serial.println(smoothingEnabled ? "Enabled" : "Disabled");
            break;
            
        case 3:
            filterMix = up ? 1.0 : 0.5;
            Serial.print("Mix: ");
            Serial.println(up ? "100%" : "50%");
            break;
            
        default:
            if (up) {
                Serial.print("DIP ");
                Serial.print(number);
                Serial.println(" up (reserved)");
            }
            break;
    }
}