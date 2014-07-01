/*
  Multiple tone player
 
 Plays multiple tones on multiple pins in sequence
 
 circuit:
 * 3 8-ohm speaker on digital pins 6, 7, and 11
 
 created 8 March 2010
 by Tom Igoe 
 based on a snippet from Greg Borenstein

This example code is in the public domain.
 
 http://arduino.cc/en/Tutorial/Tone4
 
 */
int speed = 1;
int rpm = 1000;
int delta = 1;
int gaspedal = 25; // %

void setup() {
}

void loop() {
  int deltarpm;
  int gearchange;
  
  // modeling real engine behaviour:
  // more gas means engine rpm wants to increase
  // more rpm means engine rpm does not want to increase
  // gearbox switches gear depending on gas pedal
  // (effect of gear ratio on engine not modeled yet, on purpose,
  //  to create an infinite number of gears)

  deltarpm = 250*gaspedal/100 - rpm/35;  // weighting of gaspedal against rpm
  gearchange = 1000 + 6000*gaspedal/100; // max is 7000 rpm

  speed = speed + delta;
  rpm = rpm + deltarpm;
  //tone(5, speed, 1000);
  tone(5, rpm/30, 1000);
  delay (100);
  /*if (speed > 233 || speed < 1) 
  {
    delta = -delta;
  }*/
  /*if (rpm > 7000 || rpm < 800) 
  {
    deltarpm = -deltarpm;
  }*/
  if (rpm > gearchange) 
  {
    rpm=rpm-4000;
    if (rpm < 800) {rpm=800;}
  }

}
