#define NUM_LEDS 200
#define DATA_PIN 5

#define SAMPLES 128             //Must be a power of 2
#define SAMPLING_FREQUENCY 1000 //Hz, must be less than 10000 due to ADC

CRGB leds[NUM_LEDS];