/*
  As a hearing-impaired individual, I’ve found that studying computer music offers a powerful way to understand audio at a discrete, 
  granular level. By breaking sound into its smallest components, one can gain insights that could disrupt industries like the 
  hearing aid market. Imagine devices designed with the precision of a composer, adapting dynamically to each individual’s 
  unique hearing profile. For those of us who rely on discrete technology, the potential is both personal and transformative.

  ~ Friendly reminder, protect your hearing - Chris Lanuza 2024 ~

 */

#define CONTROL_RATE 128  // Hz, powers of 2 are most reliable
#include <Meap.h>         // MEAP library, includes all dependent libraries, including all Mozzi modules
#include <ArduinoBLE.h>   // ESP32 Bluetooth Connectivity library

Meap meap;                                            // creates MEAP object to handle inputs and other MEAP library functions
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);  // defines MIDI in/out ports

// ---------- YOUR GLOBAL VARIABLES BELOW ----------
#include <tables/sin8192_int8.h>  // loads sine wavetable

// Include Samples (Melody and Drums)
#include "/Users/calodii/Desktop/MEAP-ARDUINOBLE/meapble/samples/melody.h"
#include "/Users/calodii/Desktop/MEAP-ARDUINOBLE/meapble/samples/drum_loop_hiphop_110bpm.h"

// Variables for sample playback
mSample<melody_NUM_CELLS, AUDIO_RATE, int8_t> my_sample(melody_DATA);
float default_freq;

// Creating a single oscillator using sine wavetable
mOscil<8192, AUDIO_RATE> my_sine(SIN8192_DATA);
int octave_multiplier = 1;  // Changes frequency octave

// -------- FILTER PARAMETER RANGES --------
const int MIN_CUTOFF = 20;    // Minimum filter cutoff frequency (Hz)
const int MAX_CUTOFF = 10000; // Maximum filter cutoff frequency (Hz)
const int MIN_RESONANCE = 0;  // Minimum filter resonance
const int MAX_RESONANCE = 200;// Maximum filter resonance
const int MIN_MIX = 0;        // Minimum dry/wet mix (all dry)
const int MAX_MIX = 255;      // Maximum dry/wet mix (all wet)

// -------- DIP SWITCH CONTROL FLAGS --------
bool sidechainEnabled = false;     // DIP 0: Sidechain compression
bool envelopeMode = false;         // DIP 1: Filter envelope follower
bool filterType = false;           // DIP 2: false = lowpass, true = highpass
bool filterSlope = false;          // DIP 3: false = normal, true = steeper
bool velocityMode = false;         // DIP 4: Velocity sensitive filtering
bool reverseEnvelope = false;      // DIP 5: Reverse envelope direction
bool dualFilterMode = false;       // DIP 6: Dual filter mode
bool extremeResonance = false;     // DIP 7: Extreme resonance mode

// -------- ENVELOPE AND MODULATION --------
EventDelay sidechainTimer;         // Timer for sidechain effect
ADSR<CONTROL_RATE, AUDIO_RATE> filterEnvelope;  // Envelope for filter modulation
float envelopeAmount = 0.5f;       // Amount of envelope modulation
float sidechainDepth = 0.7f;       // Depth of sidechain effect
int sidechainRate = 250;           // Rate of sidechain in milliseconds
float currentEnvValue = 0.0f;      // Current envelope value
float velocityValue = 0.0f;        // Current velocity value

// -------- FILTER OBJECTS --------
// Lowpass filters
MultiResonantFilter leftLowpass;   
MultiResonantFilter rightLowpass;  
// Highpass filters (implemented using lowpass and subtraction)
MultiResonantFilter leftHighpass;  
MultiResonantFilter rightHighpass;

// -------- PHYSICAL POTENTIOMETER VARIABLES --------
// Variables to store physical potentiometer values
int physPot1Value = 0;  // Physical pot 1 - Filter cutoff
int physPot2Value = 0;  // Physical pot 2 - Filter resonance
int physPot3Value = 0;  // Physical pot 3 - Dry/wet mix

// Constants for potentiometer pins
const int POT1_PIN = A0;  // Analog pin for potentiometer 1
const int POT2_PIN = A1;  // Analog pin for potentiometer 2
const int POT3_PIN = A2;  // Analog pin for potentiometer 3

// -------- BLUETOOTH VARIABLES --------
// BLE service UUID
BLEService meapService("a597fb2d-5947-5bb6-9b20-9fa9ec68916f");

// BLE characteristics for each potentiometer
BLEStringCharacteristic pot1Characteristic("a597fb2e-5947-5bb6-9b20-9fa9ec68916f", BLERead | BLEWrite, 20);
BLEStringCharacteristic pot2Characteristic("a597fb2f-5947-5bb6-9b20-9fa9ec68916f", BLERead | BLEWrite, 20);
BLEStringCharacteristic pot3Characteristic("a597fb30-5947-5bb6-9b20-9fa9ec68916f", BLERead | BLEWrite, 20);

// Variables to store BLE potentiometer values
int blePot1Value = 0;  // BLE pot 1 - Filter cutoff
int blePot2Value = 0;  // BLE pot 2 - Filter resonance
int blePot3Value = 0;  // BLE pot 3 - Dry/wet mix

// Smoothing objects to prevent clicks and pops when parameters change
Smooth<uint8_t> cutoffSmooth(0.95f);    // Smoothing for cutoff frequency changes
Smooth<uint8_t> resonanceSmooth(0.95f);  // Smoothing for resonance changes
Smooth<uint8_t> mixSmooth(0.95f);        // Smoothing for dry/wet mix changes

void setup() {
    // Initialize serial communication
    delay(1000);
    Serial.begin(115200);
    Serial.println("Starting MEAP with BLE and Physical Controls...");
    
    // Initialize physical potentiometer pins
    pinMode(POT1_PIN, INPUT);
    pinMode(POT2_PIN, INPUT);
    pinMode(POT3_PIN, INPUT);
    
    // Initialize BLE
    if (!BLE.begin()) {
        Serial.println("Failed to start BLE!");
        while (1) {
            delay(1000);
            Serial.println("BLE initialization failed - check hardware");
        }
    }
    
    // Set up BLE device
    BLE.setDeviceName("MEAP");
    BLE.setLocalName("MEAP");
    
    // Setup BLE service and characteristics
    BLE.setAdvertisedService(meapService);
    meapService.addCharacteristic(pot1Characteristic);
    meapService.addCharacteristic(pot2Characteristic);
    meapService.addCharacteristic(pot3Characteristic);
    BLE.addService(meapService);
    BLE.advertise();
    
    // Initialize MEAP and audio
    Serial1.begin(31250, SERIAL_8N1, 43, 44);
    startMozzi(CONTROL_RATE);
    meap.begin();
    
    // Set initial frequency for oscillator
    my_sine.setFreq(220);
    
    // Set initial sample playback rate
    default_freq = (float)melody_SAMPLERATE / (float)melody_NUM_CELLS;
    my_sample.setFreq(default_freq);
    
    // Initialize envelope parameters
    filterEnvelope.setADLevels(255, 200);
    filterEnvelope.setTimes(50, 200, 200, 200); // Attack, Decay, Sustain, Release
    
    // Initialize sidechain timer
    sidechainTimer.set(sidechainRate);
    
    Serial.println("Setup complete! System ready.");
}

void loop() {
    // Handle BLE connections
    BLEDevice central = BLE.central();
    
    if (central) {
        Serial.print("Connected to central: ");
        Serial.println(central.address());
        
        while (central.connected()) {
            // Handle BLE potentiometer updates
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
            
            // Continue audio processing
            audioHook();
        }
        Serial.println("Disconnected from central");
    }
    
    // Continue audio processing even when not connected to BLE
    audioHook();
}

void updateControl() {
    // Read all MEAP inputs
    meap.readInputs();
    
    // Read physical potentiometer values
    physPot1Value = analogRead(POT1_PIN);
    physPot2Value = analogRead(POT2_PIN);
    physPot3Value = analogRead(POT3_PIN);
    
    // Combine physical and BLE values
    int combinedPot1 = max(physPot1Value, blePot1Value);
    int combinedPot2 = max(physPot2Value, blePot2Value);
    int combinedPot3 = max(physPot3Value, blePot3Value);
    
    // Update envelope and modulation
    filterEnvelope.update();
    currentEnvValue = filterEnvelope.next();
    if (reverseEnvelope) currentEnvValue = 255 - currentEnvValue;
    
    // Handle sidechain timing
    if (sidechainEnabled && sidechainTimer.ready()) {
        filterEnvelope.noteOn();
        sidechainTimer.start(sidechainRate);
    }
    
    // Calculate base cutoff frequency
    int cutoff = map(combinedPot1, 0, 4095, MIN_CUTOFF, MAX_CUTOFF);
    
    // Apply envelope modulation if enabled
    if (envelopeMode) {
        float envMod = (currentEnvValue / 255.0f) * envelopeAmount;
        cutoff = cutoff + (envMod * (MAX_CUTOFF - cutoff));
    }
    
    // Apply velocity modulation if enabled
    if (velocityMode) {
        cutoff = cutoff + (velocityValue * (MAX_CUTOFF - cutoff));
    }
    
    // Smooth the cutoff changes
    cutoff = cutoffSmooth.next(cutoff);
    
    // Calculate resonance with extreme mode option
    int resonance = map(combinedPot2, 0, 4095, MIN_RESONANCE, MAX_RESONANCE);
    if (extremeResonance) resonance = resonance * 1.5;
    resonance = resonanceSmooth.next(resonance);
    
    // Update filter parameters based on modes
    if (dualFilterMode) {
        // Dual filter mode: slightly different cutoffs for stereo width
        int rightCutoff = cutoff * 1.5;
        
        // Update all filters (both lowpass and highpass)
        leftLowpass.setCutoffFreqAndResonance(cutoff, resonance);
        rightLowpass.setCutoffFreqAndResonance(rightCutoff, resonance);
        leftHighpass.setCutoffFreqAndResonance(cutoff, resonance);
        rightHighpass.setCutoffFreqAndResonance(rightCutoff, resonance);
    } else {
        // Single filter mode: same cutoff for both channels
        leftLowpass.setCutoffFreqAndResonance(cutoff, resonance);
        rightLowpass.setCutoffFreqAndResonance(cutoff, resonance);
        leftHighpass.setCutoffFreqAndResonance(cutoff, resonance);
        rightHighpass.setCutoffFreqAndResonance(cutoff, resonance);
    }
}

AudioOutput_t updateAudio() {
    // Get the raw audio samples
    int64_t out_sample = my_sine.next();
    int64_t melody_sample = my_sample.next();
    
    // Combine samples
    int64_t left_sample = (out_sample + melody_sample) >> 1;
    int64_t right_sample = left_sample;
    
    // Process through appropriate filters based on filter type
    int64_t filtered_left, filtered_right;
    
    if (filterType) {  // Highpass mode
        // Use built-in highpass output
        leftHighpass.next(left_sample);
        rightHighpass.next(right_sample);
        filtered_left = leftHighpass.high();
        filtered_right = rightHighpass.high();
    } else {  // Lowpass mode
        leftLowpass.next(left_sample);
        rightLowpass.next(right_sample);
        filtered_left = leftLowpass.low();
        filtered_right = rightLowpass.low();
    }
    
    // Apply sidechain compression if enabled
    if (sidechainEnabled) {
        float sidechain_amt = 1.0f - ((currentEnvValue / 255.0f) * sidechainDepth);
        filtered_left = (filtered_left * sidechain_amt);
        filtered_right = (filtered_right * sidechain_amt);
    }
    
    // Apply steeper slope if enabled
    if (filterSlope) {
        if (filterType) {
            leftHighpass.next(filtered_left);
            rightHighpass.next(filtered_right);
            filtered_left = leftHighpass.high();
            filtered_right = rightHighpass.high();
        } else {
            leftLowpass.next(filtered_left);
            rightLowpass.next(filtered_right);
            filtered_left = leftLowpass.low();
            filtered_right = rightLowpass.low();
        }
    }
    
    // Mix dry/wet
    int mix = mixSmooth.next(map(max(physPot3Value, blePot3Value), 0, 4095, MIN_MIX, MAX_MIX));
    int64_t final_left = ((255 - mix) * left_sample + mix * filtered_left) >> 8;
    int64_t final_right = ((255 - mix) * right_sample + mix * filtered_right) >> 8;
    
    return StereoOutput::fromNBit(
        10, // Change 8 to 10 for more headroom
        (final_left * meap.volume_val) >> 12,
        (final_right * meap.volume_val) >> 12
    );
}

/**
   * Runs whenever a touch pad is pressed or released
   *
   * int number: the number (0-7) of the pad that was pressed
   * bool pressed: true indicates pad was pressed, false indicates it was released
   */
void updateTouch(int number, bool pressed) {
    if (pressed) {
        velocityValue = meap.touch_vals[number] / 255.0f;  // Normalize velocity
        filterEnvelope.noteOn();
    } else {
        filterEnvelope.noteOff();
    }
    
    switch (number) {
        case 0:
            if (pressed) {  // Pad 0 pressed
                Serial.println("t0 pressed ");
                my_sample.setFreq(default_freq);
                my_sample.start();
            } else {  // Pad 0 released
                Serial.println("t0 released");
            }
            break;
        case 1:
            if (pressed) {  // Pad 1 pressed
                Serial.println("t1 pressed");
                my_sample.setFreq(default_freq * pow(2, 2.0 / 12.0));
                my_sample.start();
            } else {  // Pad 1 released
                Serial.println("t1 released");
            }
            break;
        case 2:
            if (pressed) {  // Pad 2 pressed
                Serial.println("t2 pressed");
                my_sample.setFreq(default_freq * pow(2, 4.0 / 12.0));
                my_sample.start();
            } else {  // Pad 2 released
                Serial.println("t2 released");
            }
            break;
        case 3:
            if (pressed) {  // Pad 3 pressed
                Serial.println("t3 pressed");
                my_sample.setFreq(default_freq * pow(2, 5.0 / 12.0));
                my_sample.start();
            } else {  // Pad 3 released
                Serial.println("t3 released");
            }
            break;
        case 4:
            if (pressed) {  // Pad 4 pressed
                Serial.println("t4 pressed");
                my_sample.setFreq(default_freq * pow(2, 7.0 / 12.0));
                my_sample.start();
            } else {  // Pad 4 released
                Serial.println("t4 released");
            }
            break;
        case 5:
            if (pressed) {  // Pad 5 pressed
                Serial.println("t5 pressed");
                my_sample.setFreq(default_freq * pow(2, 9.0 / 12.0));
                my_sample.start();
            } else {  // Pad 5 released
                Serial.println("t5 released");
            }
            break;
        case 6:
            if (pressed) {  // Pad 6 pressed
                Serial.println("t6 pressed");
                my_sample.setFreq(default_freq * pow(2, 11.0 / 12.0));
                my_sample.start();
            } else {  // Pad 6 released
                Serial.println("t6 released");
            }
            break;
        case 7:
            if (pressed) {  // Pad 7 pressed
                Serial.println("t7 pressed");
                my_sample.setFreq(default_freq * pow(2, 12.0 / 12.0));
                my_sample.start();
            } else {  // Pad 7 released
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
    switch (number) {
        case 0:  // Sidechain compression
            sidechainEnabled = up;
            if (up) {
                filterEnvelope.noteOn();
                Serial.println("Sidechain enabled");
            } else {
                filterEnvelope.noteOff();
                Serial.println("Sidechain disabled");
            }
            break;
            
        case 1:  // Filter envelope follower
            envelopeMode = up;
            Serial.println(up ? "Envelope mode ON" : "Envelope mode OFF");
            break;
            
        case 2:  // Filter type
            filterType = up;
            Serial.println(up ? "Highpass filter" : "Lowpass filter");
            break;
            
        case 3:  // Filter slope
            filterSlope = up;
            Serial.println(up ? "24dB slope" : "12dB slope");
            break;
            
        case 4:  // Velocity sensitivity
            velocityMode = up;
            Serial.println(up ? "Velocity ON" : "Velocity OFF");
            break;
            
        case 5:  // Envelope direction
            reverseEnvelope = up;
            Serial.println(up ? "Reverse envelope" : "Normal envelope");
            break;
            
        case 6:  // Dual filter mode
            dualFilterMode = up;
            Serial.println(up ? "Dual filter mode" : "Single filter mode");
            break;
            
        case 7:  // Extreme resonance
            extremeResonance = up;
            Serial.println(up ? "Extreme resonance" : "Normal resonance");
            break;
    }
}