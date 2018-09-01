#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>

    
int N_PIXELS = 16;  // Number of pixels you are using
int N_LEDPIN = 13; 

// pin de connexion du module bluetooth
int Rx=10;
int Tx=11;
int lf=10;
int ledPin=13;
String strRecu;
int intMove=0; // -1 reculer; 0: arrêt; +1: avancer

SoftwareSerial bt(Rx,Tx);
Adafruit_NeoPixel  strip = Adafruit_NeoPixel(N_PIXELS, N_LEDPIN, NEO_GRB + NEO_KHZ800);

     // Set all pixels to a color (synchronously)
    void ColorSet(uint32_t color)
    {
        for (int i = 0; i < N_PIXELS; i++)
        {
            strip.setPixelColor(i, color);
        }
        strip.show();
    }

    void ColorSetOdd(uint32_t color)
    {
        for (int i = 0; i < N_PIXELS; i++)
        {
            if(i%2==0) 
            {
              strip.setPixelColor(i, color);
             } 
             else
             {
              strip.setPixelColor(i, 0);
              };
        }
        strip.show();
    }
    
void setup() {
 pinMode(ledPin,OUTPUT);
 bt.begin(9600);
 strip.begin();
 strip.show();
 Serial.begin(9600);

}

void loop() {
  if (bt.available()) {
    strRecu = bt.readStringUntil(lf);
    Serial.print(strRecu + String("\n"));
    if(strRecu=="LED:ON")
    {
      ColorSet(0x00FF00);
    }
    else if(strRecu=="LED:OFF")
    {
       ColorSet(0);
    }
    else if(strRecu=="AVANCER"){
      ColorSetOdd(0x0000FF);
      intMove=1;
    }
    else if(strRecu="ARRETER"){
      ColorSetOdd(0xFF0000);
      intMove=-1;
    }
    
  }
 

}
