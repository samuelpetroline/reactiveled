// Import das bibliotecas necessárias para o funcionamento
#include "FastLED.h"
#include "arduinoFFT.h"

#define NUM_LEDS 35 // Número de LEDs na fita que está conectada ao Arduino
#define DATA_PIN 5  // PIN na qual a conexão DATA da fita está conectada ao Arduino

// Informações base para a captura dos dados e execução do FFT.
#define SAMPLES 128             //Must be a power of 2
#define SAMPLING_FREQUENCY 1000 //Hz, must be less than 10000 due to ADC

arduinoFFT FFT = arduinoFFT();
CRGB leds[NUM_LEDS];


float scale = 256.0;
int huesPerPixel = 255 / NUM_LEDS;
CRGBPalette16 gPalette;
uint8_t gHue = 0;

const int bandCount = 16; // Número de bandas de frequencia a serem analizadas
float fftLevels[bandCount];
float level;
int shown;

double vReal[SAMPLES];
double vImag[SAMPLES];
unsigned long sampling_period_us;
unsigned long microseconds;
 
void setup() { 
  Serial.begin(9600);
  pinMode(A0, INPUT);

  gPalette = RainbowColors_p;
  sampling_period_us = round(1000000*(1.0/SAMPLING_FREQUENCY));
  
  FastLED.addLeds<UCS1903, DATA_PIN, BRG>(leds, NUM_LEDS);
}
 
void loop() {
    spectrumBar();
    
    EVERY_N_MILLISECONDS(20) { gHue++; }
}

void spectrumBar() {
    
	// Captura os dados de frequência do som capturado pelo microfone
    for(int i=0; i<SAMPLES; i++)
    {
        microseconds = micros();    //Overflows after around 70 minutes!
     
        vReal[i] = analogRead(A0);
        vImag[i] = 0;
     
        while(micros() < (microseconds + sampling_period_us)){
        }
    }
 
    /*FFT*/
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
    //double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);

	// Reseta o LED
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();

	// Isso deve ter alguma explicação...
    double maxFreq = vReal[2];
    
    for(int i=2; i<(SAMPLES/2); i++)
    {
      if (vReal[i] > maxFreq)
      {
        maxFreq = vReal[i];
      }     
    }
    
	// Mapeia o valor da frequência entre 1 e o numero de LEDs
    int val = map(maxFreq, 0, 1000, 1, NUM_LEDS); //maxFreq * scale;
    // Serial.println(val, 1);
	
    if (val >= NUM_LEDS) 
		val = NUM_LEDS - 1;

    if (val >= shown) {
        shown = val;
    }
    else {
        if (shown > 0) shown = shown - 1;
        val = shown;
    }

    for (int i = 0; i < shown; i++) {
        leds[i] = ColorFromPalette(gPalette, i * huesPerPixel + gHue, 255, NOBLEND);
    }

    FastLED.show();
 
}

