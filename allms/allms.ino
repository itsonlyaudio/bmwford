/*

 All
 Models car engine behaviour
 
*/

// constants won't change. Used here to 
// set pin numbers:
const int KL30        =  2;
const int RPMGauge    =  5;
const int RPMmin      =  700;
const int RPMmax      = 7500;
const int RPM_update_interval = 3; // milliseconds
const int FuelGauge   =  6;
const int BlinkR      =  7;
const int BlinkL      =  8;
const int TempGauge   =  9;
const int Speaker     = 10;
const int EcoGauge    = 11;
const int HighBeam    = 12;
const int SpeedGauge  = 13;
const unsigned long second      = 1000;  // milliseconds

const int GasMax      = 750.0;

float Fuel = 162.0;        // Fuel content of the tank
boolean Fuelempty = 0;

int Speed = 0;
int Speedmax = 235;
boolean Speedstate;
unsigned long Speedinterval;

int Temp = 155;
unsigned long TempInterval = 5*second; // microseconds
unsigned long previous_time_temp = 0;

int RPM = 700;
boolean RPMstate;
unsigned long previous_rpm_update;
unsigned long RPMinterval;
unsigned long switchrpm;
unsigned long hysteresis;

int gear = 1;
int gear_ratio[5] = {100, 70, 50, 35, 25};

boolean clutch;   // 1 for engaged, 0 for disengaged

const int Fuelmin = 2;
const int Fuelmax = 162;

unsigned long tone_interval = 0.025 * second;
unsigned long gear_interval = 1 * second;
int tonefreq = 0;
int oldtonefreq = 0;

unsigned long previousMillis = 0;
unsigned long previousMillisup = 0;
unsigned long previousMillisdown = 0;
unsigned long previous_time_speed_up = 0;
unsigned long previous_time_speed_down = 0;
unsigned long previous_time_tone = 0;
unsigned long previous_time_gear = 0;

// left stalk variables
boolean Pull;
boolean Horn;
boolean Right;
boolean Left;
boolean Push;
boolean Alarm;

boolean Blink;
unsigned long BlinkTime = 0.5 * second; //microseconds
unsigned long previous_time_blink = 0;

// the setup routine runs once when you press reset:
void setup()  { 
  pinMode(KL30,       OUTPUT);
  pinMode(RPMGauge,   OUTPUT);
  pinMode(FuelGauge,  OUTPUT);
  pinMode(BlinkR,     OUTPUT);      
  pinMode(BlinkL,     OUTPUT);
  pinMode(TempGauge,  OUTPUT);
  pinMode(FuelGauge,  OUTPUT);
  pinMode(EcoGauge,   OUTPUT);
  pinMode(HighBeam,   OUTPUT);
  pinMode(SpeedGauge, OUTPUT);
  pinMode(Speaker,    OUTPUT);

  //configure digital inputs and enable the internal pull-up resistor
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(A4, INPUT_PULLUP);

  // turn off blinkers
  digitalWrite(BlinkR, LOW);
  digitalWrite(BlinkL, LOW);

  // activate Klemme 30
  delay(500);
  digitalWrite(KL30, HIGH);

  // turn off High Beam
  digitalWrite(HighBeam, LOW);

  // populate slow gauges
  analogWrite(FuelGauge, int(Fuel));    
  analogWrite(TempGauge, Temp);

  Serial.begin(9600);

} 



// the loop routine runs over and over again forever:
void loop()  { 
  unsigned long current_time=millis();
  
  // read inputs  
  int sensorValue = analogRead(A5); // gas pedal
  sensorValue = GasMax - sensorValue;
  float GasPedal = (float(sensorValue)/GasMax);
  if (Fuelempty) GasPedal = 0.0;
  
  sensorValue = analogRead(A6); // brake pedal
  float BrakePedal = (float(sensorValue)/GasMax);

  // these are low active inputs
  Pull  = !digitalRead(A0);
  Horn  = !digitalRead(A1);
  Left  = !digitalRead(A2);
  Right = !digitalRead(A3);
  Push  = !digitalRead(A4);
  
  // Engine model
  // determine if RPM should increase or decrease
  if (BrakePedal > 0.8 && gear == 1)
  {
    clutch = 0;
    RPM = RPM + 8*GasPedal - 2*RPM/7000 - 0.7;
  }
  else
  {
    clutch = 1;
    // we need to do regular RPM updates
    // because the timing changes due to the changed frequency
    // if we don't, then RPM will be updated faster as the RPM gets higher
    // leading to a runaway effect.
    if (current_time - previous_rpm_update > RPM_update_interval)
    {
      previous_rpm_update = current_time;
      RPM = RPM + (2.5*GasPedal - 6*BrakePedal - 0.0000*float(Speed)) * float(gear_ratio[gear-1])/50.0 - 0.25 - random(-200, 200)/Speed;
    }
  }
  if (RPM < RPMmin && !Fuelempty) RPM = RPMmin;
  if (RPM < 1000 && Fuelempty) RPM = 0;
  if (RPM > RPMmax) RPM = RPM - random(0, 250); // a more realistic revlimiter

  if (RPM < 1000 && Fuelempty) noTone(Speaker); else tone(Speaker, tonefreq);
  
  Speed = clutch * RPM / gear_ratio[gear-1];
  if (RPM < 1000 && gear==1) Speed = 0;
  if (Speed > Speedmax) Speed = Speedmax;
  
  if(current_time - previous_time_tone > tone_interval)
  {
    if (RPM>=RPMmin && RPM<=RPMmax) tonefreq = RPM / 30;
    if (Horn) tonefreq = 200;

    // 59 just doesn't work right
    if (tonefreq == 59) tonefreq = 58;
  
    // 124 just doesn't work right
    if (tonefreq == 124) tonefreq = 123;
    
    // smoothing
    if(oldtonefreq - tonefreq > 5) tonefreq = oldtonefreq - 5;
    if(tonefreq - oldtonefreq > 5) tonefreq = oldtonefreq + 5;
    oldtonefreq = tonefreq;
    
    if (RPM < 1000 && Fuelempty) noTone(Speaker); else tone(Speaker, tonefreq);
    
    previous_time_tone = current_time;
  }
  
  // we can not rely on toggling the output every half interval, since thus timing errors accumulate
  // so we evaluate the up and down slopes independently and let timing errors influence only the on/off timing
  // it is necessary to add the criterion that the previous up/down slope should be about a half cycle ago
  // therefore we check for RPMinterval / 2.5 rather than RPMinterval/2 for some, but relativel loose timing constraint
  RPMinterval = second * 1/((float(RPM)/30.0)+0.001);
  if(current_time - previousMillisup > RPMinterval && current_time - previousMillisdown > RPMinterval/2.5)
  {
    if(RPM >= RPMmin) RPMstate = HIGH;
    digitalWrite(RPMGauge, RPMstate);
    previousMillisup = current_time;
  }
  if(current_time - previousMillisdown > RPMinterval && current_time - previousMillisup > RPMinterval/2.5)
  {
    RPMstate = LOW;
    digitalWrite(RPMGauge, RPMstate);
    previousMillisdown = current_time;
  }
  
  // repeating this code since sometimes takes too long between updates
  if (RPM < 1000 && Fuelempty) noTone(Speaker); else tone(Speaker, tonefreq);


  //tone(SpeedGauge, Speed);
  Speedinterval = second * 1/((float(Speed)*285.0/220.0)+0.00001);
  //Serial.println(Speedinterval);
  if(current_time - previous_time_speed_up > Speedinterval && current_time - previous_time_speed_down > Speedinterval/2.5)
  {
    if (Speed > 0) Speedstate = HIGH;
    digitalWrite(SpeedGauge, Speedstate);
    previous_time_speed_up = current_time;
  }
  if(current_time - previous_time_speed_down > Speedinterval && current_time - previous_time_speed_up > Speedinterval/2.5)
  {
    Speedstate = LOW;
    digitalWrite(SpeedGauge, Speedstate);
    previous_time_speed_down = current_time;
  }
  
  // pwm of 100% is 0 L/100km
  // no need to normalise against km/h; this is done in the dashboard!
  analogWrite(EcoGauge, 255-(GasPedal*255));
    
  // gear box modeling
  // gear switching is dependent on gas pedal (more gas = higher upswitch)
  // and gear otherwise gets very boring to hear).
  
  switchrpm = 1000 + GasPedal * 1000;
  hysteresis = 3000;
  
  if (RPM > switchrpm + hysteresis + 250*(gear) && gear < 5 && clutch && current_time - previous_time_gear > gear_interval)
  {
    gear = gear + 1;
    //Serial.println(RPM);
    //Serial.println(gear);

    // at shift up, speed drops down a little bit
    RPM = (Speed-10) * gear_ratio[gear-1];
    previous_time_gear = current_time;
  }
  else if (RPM < max(1500, switchrpm + 250*(gear)) && gear >  1 && clutch && current_time - previous_time_gear > gear_interval)
  {
    gear = gear - 1;
    //Serial.println(RPM);
    //Serial.println(gear);

    RPM = Speed * gear_ratio[gear-1];
    previous_time_gear = current_time;
  }


  // update the slow sensors
  //
  if(current_time - previous_time_temp > TempInterval) // update temp sensor
  {
    // PWM for the Temp Gauge:
    // nothing happens from 255 down to 155 (stone cold)
    // 155 to 55 is cold region
    // 55 to 25 is mid
    // 14 is light on
    // 5 is minimum PWM for valid display
    previous_time_temp = current_time;
    
    // warm-up time: first 200 seconds we go from 155 to 55
    if (current_time < 200*second && Temp > 55)
    {
      Temp = Temp - 1;
    }
    
    // afterwards, we increase for high RPM and decrease for low RPM
    // but we don't go below 55
    if (current_time > 200*second && RPM > 4000)
    {
      Temp = Temp - RPM / 3000;
    }
    if (current_time > 200*second && RPM < 4000 && Temp < 55)
    {
      Temp = Temp + 1;
    }

    if (Temp < 5) Temp = 5; // the meter acts funny when PWM gets really small
    analogWrite(TempGauge, Temp);
    
    // the fuel meter also needs to be updated very slowly
    // so we'll do that in this timing cycle as well.
    if(GasPedal > 0.1)
    {
      Fuel = Fuel - GasPedal / 2.0;
    }
    else
    {
      Fuel = Fuel - 0.1 / 2.0;
    }
    //Serial.println(Fuel);
    
    if(Fuel < 1.0)
    {
      Fuelempty = 1;
      Fuel = 0.0;
    }

    analogWrite(FuelGauge, int(Fuel));
  }
  
  // create blinker timer, toggling every 'BlinkTime'
  if(current_time - previous_time_blink > BlinkTime)
  {
    if (Blink) Blink = false; else Blink = true;
    previous_time_blink = current_time;
  }
    
  // do stuff with the left stalk
  if (Push || Pull) digitalWrite(HighBeam, HIGH); else digitalWrite(HighBeam, LOW);
  if (Left) digitalWrite(BlinkL, Blink); else digitalWrite(BlinkL, LOW);   
  if (Right) digitalWrite(BlinkR, Blink); else digitalWrite(BlinkR, LOW);   
  
  delayMicroseconds(100);
}

