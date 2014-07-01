/*

read steering wheel

 */

// declare global variables here

void setup(){
  //start serial debug connection
  Serial.begin(9600);
  //configure digital inputs and enable the internal pull-up resistor
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(A4, INPUT_PULLUP);
  pinMode(13, OUTPUT); 

}

void loop(){
  //read the pushbutton value into a variable
  int Pull = digitalRead(A0);
  int Horn = digitalRead(A1);
  int Right = digitalRead(A2);
  int Left = digitalRead(A3);
  int Push = digitalRead(A4);
  delay(50); // prevent saturating the serial port causing high load and unreadability
  
  //print out the value of the left stalk
  if (!Pull || !Horn || !Right || !Left || !Push) {
    if (!Pull)           Serial.println("PULL");
    if (!Horn)           Serial.println("HORN");
    if (!Right && !Left) Serial.println("ALARM");
    if (!Right &&  Left) Serial.println("RIGHT");
    if (!Left  && Right) Serial.println("LEFT");
    if (!Push)           Serial.println("PUSH");
  }
  else {
  }
  
}



