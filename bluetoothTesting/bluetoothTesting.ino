#include <SoftwareSerial.h>

// Define Bluetooth module's RX and TX pins
SoftwareSerial BTSerial(10, 11); // RX, TX

void setup() {
  // Initialize serial communication with computer
  Serial.begin(9600);

  // Initialize Bluetooth module serial communication
  BTSerial.begin(9600);
  Serial.println("starting...");
  BTSerial.write("starting");
}

void loop() {
  if (BTSerial.available()) {
    Serial.println("Available");
    // Read data from Bluetooth module
    String input = BTSerial.readString();
    Serial.println(input);

    // Parse input into hours, minutes, and seconds using ':' as separator
    int colon1 = input.indexOf(':');
    int colon2 = input.lastIndexOf(':');
    String hours = input.substring(0, colon1);
    String minutes = input.substring(colon1+1, colon2);
    String seconds = input.substring(colon2+1);

    // Convert strings to integers
    int h = hours.toInt();
    int m = minutes.toInt();
    int s = seconds.toInt();

    // Print parsed data to serial monitor
    Serial.print("Hours: ");
    Serial.println(h);
    Serial.print("Minutes: ");
    Serial.println(m);
    Serial.print("Seconds: ");
    Serial.println(s);
  }
}
