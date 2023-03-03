#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  // uncomment the following line to set the time and date for the first time
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  Serial.println(""); // buffer
  Serial.print("Current Time: ");
  Serial.print(rtc.now().dayOfTheWeek());
  Serial.print(":");
  Serial.print(rtc.now().hour());
  Serial.print(":");
  Serial.println(rtc.now().minute());

}

// function declarations
char nextEvent(char events[]);

// main loop
void loop() {
  // char* events[] = {"5:17:05:U", "6:15:02:U", "4:19:00", "2:20:01:L", "3:23:00:U", "4:01:00:L", "4:18:00:U", "5:15:00:L", "0:10:00:U", NULL};

  char events[] = {'0', '1', '0', '0', '5', 'U', '6', '1', '5', '0', '2', 'U', '4', '2', '3', '5', '9', 'L', NULL};
  // Serial.println(events);
  nextEvent(events);
  delay(100000);
}


// function definitions
char nextEvent(char events[]) {
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

  char nextEvent[7];

  for (int i = 0, j = 0; i < numEvents; i++, j+=6, eventTime=0) { // i handles how many events and j handles characters in events 
    // get day of week, hour, minute, and state of event
    long eventDOW = (events[j] - '0');
    long eventHour = (events[j+1] - '0') * 10 + (events[j+2] - '0');
    long eventMinute = (events[j+3] - '0') * 10 + (events[j+4] - '0');
    char state = events[j+5];

    // calculate time to midnight
    long timeToMidnight = (24L * 3600L) - (currentHour * 3600L + currentMinute * 60L); 
    
    Serial.print("Event: ");
    Serial.println(String(eventDOW) + ":" + String(eventHour) + ":" + String(eventMinute) + ":" + String(state));  
    
    if (eventDOW < currentDOW || (eventDOW == currentDOW && (eventHour < currentHour || (eventHour == currentHour && eventMinute < currentMinute)))) {
      // The event is in the past or next week
      eventTime = timeToMidnight + (eventDOW - currentDOW) * 3600L*24L + eventHour * 3600L + eventMinute * 60L;
      eventTime += 3600L*24L*7L; // Add one week to the time
    }
    else if(eventDOW > currentDOW){
      // The event is in the future before the end of the week
      eventTime = timeToMidnight + (eventDOW - currentDOW) * 3600L*24L + eventHour * 3600L + eventMinute * 60L;
    }
    else{
      // The event is in the future today
      eventTime = (eventHour - currentHour) * 3600L + (eventMinute - currentMinute) * 60L;
    }
    Serial.print("Time to Event: ");
    Serial.println(eventTime);

    if (closestEventTime == 0 || eventTime < closestEventTime) {
      closestEventTime = eventTime;
      for(int k = 0; k < 6; k++){
        nextEvent[k] = events[j+k];
      }
      nextEvent[7] = NULL;
    }
    delay(2000);
  }
  Serial.println("");
  Serial.print("Next Event: ");
  Serial.println(nextEvent);
  return nextEvent;
}




void getInput(){
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    if (command == "read") {
      DateTime now = rtc.now();
      float temperature = rtc.getTemperature();

      Serial.print("Date: ");
      Serial.print(now.year(), DEC);
      Serial.print('/');
      Serial.print(now.month(), DEC);
      Serial.print('/');
      Serial.print(now.day(), DEC);
      Serial.print(" ");
      Serial.print(now.hour(), DEC);
      Serial.print(':');
      Serial.print(now.minute(), DEC);
      Serial.print(':');
      Serial.print(now.second(), DEC);
      Serial.print(", Temperature: ");
      Serial.print(temperature);
      Serial.println(" °C");
    }
  }
  delay(1000);
}

void DOW() {
  DateTime now = rtc.now();      // get current date/time from RTC module
  int dayOfWeek = now.dayOfTheWeek();  // get day of the week (0=Sunday, 1=Monday, etc.)
  
  // Print day of the week to serial monitor
  Serial.print("Today is ");
  switch (dayOfWeek) {
    case 0:
      Serial.println("Sunday");
      break;
    case 1:
      Serial.println("Monday");
      break;
    case 2:
      Serial.println("Tuesday");
      break;
    case 3:
      Serial.println("Wednesday");
      break;
    case 4:
      Serial.println("Thursday");
      break;
    case 5:
      Serial.println("Friday");
      break;
    case 6:
      Serial.println("Saturday");
      break;
    default:
      Serial.println("Invalid day of the week");
      break;
  }

  delay(1000);  // wait for 1 second before repeating loop
}
