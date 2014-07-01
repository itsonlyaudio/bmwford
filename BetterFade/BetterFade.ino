/*
 Fade
 
 This example shows how to fade an LED on pin 9
 using the analogWrite() function.
 
 This example code is in the public domain.
 */

int led = 5;           // the pin that the LED is attached to
int brightness = 128;    // how bright the LED is
int fadeAmount = 1;    // how many points to fade the LED by
long previousMillis=0;
long interval=100;
int bmin = 0;
int bmax = 255;

// the setup routine runs once when you press reset:
void setup()  { 
  // declare pin 9 to be an output:
  pinMode(led, OUTPUT);
} 

// the loop routine runs over and over again forever:
void loop()  { 
  unsigned long currentMillis=millis();
  // set the brightness of pin 9:
  analogWrite(led, brightness);    

  // reverse the direction of the fading at the ends of the fade: 
  if (brightness < bmin) {
    brightness = bmin;
    fadeAmount = -fadeAmount; 
  }
  if (brightness > bmax) {
    brightness = bmax;
    fadeAmount = -fadeAmount; 
  }
    
  if (currentMillis - previousMillis > interval) {
    // change the brightness for next time through the loop:
    brightness = brightness + fadeAmount;
    previousMillis = currentMillis;
  }

  // some delay
  delay(interval/4);
}

