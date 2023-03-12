#include <Wire.h>
#include "RTClib.h"
#include <avr/sleep.h>  // AVR library for controlling the sleep modes
#include <EEPROM.h>

RTC_DS3231 rtc;

#define ALARM_PIN 2

void setup() {
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  // uncomment the following line to set the time and date for the first time
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  pinMode(ALARM_PIN, INPUT_PULLUP);
  Serial.println(""); // buffer
  Serial.print("Current Time: ");
  Serial.print(rtc.now().dayOfTheWeek());
  Serial.print("-");
  Serial.print(rtc.now().hour());
  Serial.print(":");
  Serial.println(rtc.now().minute());

  rtc.disableAlarm(1);
  rtc.disableAlarm(2);
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);

  rtc.writeSqwPinMode(DS3231_OFF);

}

// function declarations
long nextEvent(char events[]);
void setAlarm(long timeToEvent);
void enterSleep();
void alarmISR();

// main loop
void loop() {
  // char events[] = {'5', '1', '4', '3', '6', 'L', '0', '1', '0', '0', '5', 'U', '6', '1', '5', '0', '2', 'U', '4', '2', '3', '5', '9', 'L', '5', '1', '4', '1', '8', 'L', NULL};
  char events[] = {'3', '1', '7', '5', '4', 'L'};
  long alarm = nextEvent(events);
  setAlarm(alarm);
  enterSleep();
  delay(100000);

  
}


// function definitions
long nextEvent(char events[]) {
  // get length of array
  int numEvents = 0;
  for (int i = 0; events[i] != NULL; i++) {
    numEvents++;
  }
  numEvents = numEvents/6;
  Serial.print("Number of Events: ");
  Serial.println(numEvents);
  DateTime now = rtc.now();// get current date/time from RTC module
  long currentDOW = now.dayOfTheWeek();
  long currentHour = now.hour();
  long currentMinute = now.minute();
  long eventTime = 0;
  long closestEventTime = 0;
  char nextState;
  int eventIndex = NULL;
  String nextEvent = "";


  for (int i = 0, j = 0; i < numEvents; i++, j+=6, eventTime=0) { // i handles how many events and j handles characters in events 
    // get day of week, hour, minute, and state of event
    long eventDOW = (events[j] - '0');
    long eventHour = (events[j+1] - '0') * 10 + (events[j+2] - '0');
    long eventMinute = (events[j+3] - '0') * 10 + (events[j+4] - '0');
    char state = events[j+5];
    String event = String(eventDOW) + "-" + String(eventHour) + ":" + String(eventMinute) + ":" + String(state);

    // calculate time to midnight
    long timeToMidnight = (24L * 3600L) - (currentHour * 3600L + currentMinute * 60L); 
    
    // Serial.print("Event: " + event);
    
    if (eventDOW < currentDOW || (eventDOW == currentDOW && (eventHour < currentHour || (eventHour == currentHour && eventMinute < currentMinute)))) {
      // The event is in the past or next week
      eventTime = timeToMidnight + (eventDOW - currentDOW - 1) * 3600L*24L + eventHour * 3600L + eventMinute * 60L;
      eventTime += 3600L*24L*7L; // Add one week to the time
    }
    else if(eventDOW > currentDOW){
      // The event is in the future before the end of the week
      eventTime = timeToMidnight + (eventDOW - currentDOW - 1) * 3600L*24L + eventHour * 3600L + eventMinute * 60L;
    }
    else{
      // The event is in the future today
      eventTime = (eventHour - currentHour) * 3600L + (eventMinute - currentMinute) * 60L;
    }
    // Serial.print("Time to Event: ");
    // Serial.println(eventTime);

    if (closestEventTime == 0 || eventTime < closestEventTime) {
      closestEventTime = eventTime;
      nextState = state;
      eventIndex = i;
      nextEvent = event;

    }
    // delay(2000);
  }
  Serial.println("");
  Serial.println("Next Event: " + nextEvent);
  Serial.println("Time to event: " + String(closestEventTime) + " (" +String(float(closestEventTime)/float(3600L)) +" hours)");
  Serial.println("Next State: " + String(nextState));
  Serial.flush();
  EEPROM.write(1, nextState); // save next state to EEPROM index 1

  return closestEventTime;
}



void setAlarm(long timeToEvent) {
  DateTime now = rtc.now();
  DateTime nextAlarm = now.unixtime() + timeToEvent;
  rtc.setAlarm1(nextAlarm, DS3231_A1_Minute);
}

void enterSleep(){
  sleep_enable();                       // Enabling sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // Setting the sleep mode, in this case full sleep
  
  noInterrupts();                       // Disable interrupts
  attachInterrupt(digitalPinToInterrupt(ALARM_PIN), alarmISR, LOW);
  Serial.println("Going to sleep!");
  Serial.flush(); 
  interrupts();                         // Allow interrupts again
  sleep_cpu();                          // Enter sleep mode

  /* The program will continue from here when it wakes */
  
  // Disable and clear alarm
  rtc.disableAlarm(1);
  rtc.clearAlarm(1);
}

void alarmISR() {
  sleep_disable(); // Disable sleep mode
  detachInterrupt(digitalPinToInterrupt(ALARM_PIN)); // Detach the interrupt to stop it firing
  // This function is called when the alarm goes off
  Serial.println("Alarm triggered!");
  // read state from eeprom index 1
  char state = EEPROM.read(1);
  Serial.print("State: ");
  Serial.println(state);
}


// void getInput(){
//   if (Serial.available() > 0) {
//     String command = Serial.readStringUntil('\n');
//     if (command == "read") {
//       DateTime now = rtc.now();
//       float temperature = rtc.getTemperature();
//       Serial.print("Date: ");
//       Serial.print(now.year(), DEC);
//       Serial.print('/');
//       Serial.print(now.month(), DEC);
//       Serial.print('/');
//       Serial.print(now.day(), DEC);
//       Serial.print(" ");
//       Serial.print(now.hour(), DEC);
//       Serial.print(':');
//       Serial.print(now.minute(), DEC);
//       Serial.print(':');
//       Serial.print(now.second(), DEC);
//       Serial.print(", Temperature: ");
//       Serial.print(temperature);
//       Serial.println(" °C");
//     }
//   }
//   delay(1000);
// }

// void DOW() {
//   DateTime now = rtc.now();      // get current date/time from RTC module
//   int dayOfWeek = now.dayOfTheWeek();  // get day of the week (0=Sunday, 1=Monday, etc.)
//   // Print day of the week to serial monitor
//   Serial.print("Today is ");
//   switch (dayOfWeek) {
//     case 0:
//       Serial.println("Sunday");
//       break;
//     case 1:
//       Serial.println("Monday");
//       break;
//     case 2:
//       Serial.println("Tuesday");
//       break;
//     case 3:
//       Serial.println("Wednesday");
//       break;
//     case 4:
//       Serial.println("Thursday");
//       break;
//     case 5:
//       Serial.println("Friday");
//       break;
//     case 6:
//       Serial.println("Saturday");
//       break;
//     default:
//       Serial.println("Invalid day of the week");
//       break;
//   }
//   delay(1000);  // wait for 1 second before repeating loop
// }

