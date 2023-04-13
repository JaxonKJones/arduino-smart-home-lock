#include <EEPROM.h>

void setup() {
  Serial.begin(9600);
}

// main loop
void loop() {
  // define schedule to load into eeprom
  char events[] = {'1', '0', '7', '5', '0', 'L', '2', '0', '7', '5', '0', 'L', '3', '0', '7', '5', '0', 'L', '5', '0', '7', '5', '0', 'L', '5', '0', '7', '5', '0', 'L', '0', '0', '7', '2', '0', 'U', '1', '0', '7', '2', '0', 'U', '2', '0', '7', '2', '0', 'U', '3', '0', '7', '2', '0', 'U', '4', '0', '7', '2', '0', 'U', '5', '0', '7', '2', '0', 'U', '1', '1', '5', '0', '0', 'U', '2', '1', '0', '2', '0', 'U', '3', '1', '6', '2', '0', 'U', '4', '1', '0', '2', '0', 'U', '5', '1', '0', '2', '0', 'U', '0', '2', '3', '3', '0', 'L', '1', '2', '3', '3', '0', 'L', '2', '2', '3', '3', '0', 'L', '3', '2', '3', '3', '0', 'L', '4', '2', '3', '3', '0', 'L', '5', '2', '3', '3', '0', 'L', '6', '2', '3', '3', '0', 'L'};

  
  // load schedule into eeprom
  EEPROM.write(0, sizeof(events)/6);
  Serial.println("Writing: " + String(sizeof(events)/6) + " to address: 0");
  EEPROM.write(1, 'L');
  for (int i = 2; i < sizeof(events)+2; i++) {
    EEPROM.write(i, events[i]);
    Serial.println("Writing: " + String(events[i]) + " to address: " + String(i));
  }

  // write NULL to the rest of eeprom
  for (int i = sizeof(events)+2; i < 512; i++) {
    EEPROM.write(i, NULL);
  }

  Serial.println("done");
  delay(100000);
}

