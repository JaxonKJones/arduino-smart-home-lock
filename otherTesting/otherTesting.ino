#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

void setup() {
   Serial.begin(9600);
   Serial.println("Starting...");
  // Set sleep mode to power-down
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  // Enable sleep mode
  sleep_enable();
  
  // Configure watchdog timer to wake up the microcontroller after 8 seconds
  // wdt_enable(WDTO_8S);
}

void loop() {
  Serial.println("Starting...");
  // Put the microcontroller into sleep mode
  sleep_cpu();
  
  // // The microcontroller will be woken up by the watchdog timer after 8 seconds
  // Serial.println("Its been 8 seconds!");
  
  // // Disable watchdog timer to prevent it from triggering again
  // wdt_disable();
  // exit(0);
}