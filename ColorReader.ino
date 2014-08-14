#include <Adafruit_NeoPixel.h>

int scale = 128;
int R_MAX = 0;
int G_MAX = 0;
int B_MAX = 0;

int R_MIN = 9999;
int G_MIN = 9999;
int B_MIN = 9999;

int S0 = 8;
int S1 = 9;
int S2 = 10;
int S3 = 11;
int OE = 12;
int OUT = 47;

int RING = 51;
int RING_POWER = 53;

int BUTTON = 0;
int LED = 3;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, RING, NEO_GRB + NEO_KHZ800);

volatile int state = LOW;
volatile int ledState = LOW;

int currentPixel = 0;

void setup()
{
  attachInterrupt(BUTTON, buttonPressed, CHANGE);

  pinMode(RING, OUTPUT);
  pinMode(RING_POWER, OUTPUT);
  digitalWrite(RING_POWER, HIGH);

  //communication freq (sensitivity) selection
  pinMode(S0,OUTPUT);
  pinMode(S1,OUTPUT);

  //color mode selection
  pinMode(S2,OUTPUT);
  pinMode(S3,OUTPUT);
  
  pinMode(OE, OUTPUT);

  // Color sensor always on
  digitalWrite(OE, LOW);

  //color response pin (only actual input from taos)
  pinMode(OUT, INPUT);

  
  pinMode(LED, OUTPUT);
  digitalWrite(LED, ledState);
  
  
  Serial.begin(112500);
  Serial.println("Ready");
  colorWipe(strip.Color(128, 128, 128), 30);
  colorWipe(strip.Color(0, 0, 0), 30);
}

void loop()
{
  if(state == HIGH)
  {
    readColor();
  }
}

void buttonPressed()
{
  state = HIGH;
}

void readColor()
{
  state = HIGH;
  float red = 0;
  float green = 0;
  float blue = 0;

  digitalWrite(LED, HIGH);
  
  // Frequency Scaling: 100%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, HIGH);

  // Red
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  red = getFrequency(OUT);
  
  if(red < R_MIN)
    R_MIN = red;
  if(red > R_MAX)
    R_MAX = red;
  red = normalize(red, R_MIN, R_MAX);

  // Green
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  green = getFrequency(OUT);

  if(green < G_MIN)
    G_MIN = green;
  if(green > G_MAX)
    G_MAX = green;

  green = normalize(green, G_MIN, G_MAX);

  // Blue
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  blue = getFrequency(OUT);
  
  if(blue < B_MIN)
    B_MIN = blue;
  if(blue > B_MAX)
    B_MAX = blue;

  blue = normalize(blue, B_MIN, B_MAX);

  // Power down
  digitalWrite(S0, LOW);
  digitalWrite(S1, LOW);
  digitalWrite(LED, LOW);

  Serial.println("\nGot reading:");
//  Serial.print("  White: ");
//  Serial.println(white);

  Serial.print("  Red:   ");
  Serial.println(red);

  Serial.print("  Green: ");
  Serial.println(green);

  Serial.print("  Blue:  ");
  Serial.println(blue);

  colorWipe(strip.Color(red * scale, green * scale, blue * scale), 10);
  //colorStep(strip.Color(255*red, 255*green, 255*blue));

  state = LOW;
}
 
float normalize(long val, long minimum, long maximum)
{
  if (val <= minimum)
    return 0.0;
  
  if (val >= maximum)
    return 1.0;

  float newVal = val - minimum;
  float newMax = maximum - minimum;
  return (newVal/newMax);
}

long getFrequency(int pin) {
  int samples = 1024;
  long freq = 0;
  for(unsigned int j=0; j<samples; j++)
  {
    freq+= 500000/pulseIn(pin, HIGH, 250000);
  }
  return freq / samples;
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void colorStep(uint32_t c) {
  strip.setPixelColor(currentPixel, c);
  strip.setPixelColor((currentPixel+12)%16, strip.Color(0,0,0));
  strip.show();
  currentPixel = (currentPixel+1) % 16;
}
