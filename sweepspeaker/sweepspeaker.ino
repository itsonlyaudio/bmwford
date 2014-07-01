int RPM = 1300;
int freq;

int increment = 1;

void setup() {
  pinMode(10, OUTPUT);
}

void loop (){
  RPM = RPM + increment;
  freq = int(RPM/30);

  // 58 just doesn't work right
  if (freq == 59) freq = 58;
  
  // 124 just doesn't work right
  if (freq == 124) freq = 123;
  tone(10, freq);
  
  if (RPM > 7000) increment = -increment;
  if (RPM < 700) increment = -increment;
  
  delay(50);
}

