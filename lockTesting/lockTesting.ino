#include <Servo.h>

Servo myservo;  // create servo object to control a servo

void setup() {
  Serial.begin(9600);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  myservo.write(0);       // sets the servo position to 0 degrees
  delay(1000);            // waits for 1 second
  myservo.write(90);      // sets the servo position to 90 degrees
  delay(1000);            // waits for 1 second
  myservo.write(180);     // sets the servo position to 180 degrees
  delay(1000);  
  // if (Serial.available() > 0) {
  //   String command = Serial.readStringUntil('\n');
  //   myservo.write(command.toInt());
  //   delay(500);
  // }

}
