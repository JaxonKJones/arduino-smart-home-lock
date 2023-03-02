#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  // uncomment the following line to set the time and date for the first time
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {

  char* events[] = {"1:10:05:U", "2:20:01:L", "3:23:00:U"};
  Serial.print("Current DOW:H:M :");
  Serial.print(rtc.now().dayOfTheWeek());
  Serial.print(":");
  Serial.print(rtc.now().hour());
  Serial.print(":");
  Serial.print(rtc.now().minute());
  Serial.print(" Next Event:");
  Serial.println(nextEvent(events, 3));
  delay(5000);
}

String nextEvent(char* events[], int numEvents) {
  DateTime now = rtc.now();      // get current date/time from RTC module
  int currentDOW = now.dayOfTheWeek();
  int currentHour = now.hour();
  int currentMinute = now.minute();

  int closestEventTime = 0;
  String closestEvent = "";

  for (int i = 0; i < numEvents; i++) {
    int eventTime =0;
    String event = String(events[i]);
    int eventDOW = event.substring(0,1).toInt();
    int eventHour = event.substring(2,4).toInt();
    int eventMinute = event.substring(5,7).toInt();
  
    int timeToMidnight = (24 - currentHour) * 3600 + (60 - currentMinute) * 60; 

    
    if (eventDOW < currentDOW || (eventDOW == currentDOW && (eventHour < currentHour || (eventHour == currentHour && eventMinute <= currentMinute)))) {
      // The event is in the past or next week
      eventTime = timeToMidnight + (eventDOW - currentDOW) * 3600*24 + eventHour * 3600 + eventMinute * 60;
      // Add one week to the time
      eventTime += 3600*24*7;
    }
    else if(eventDOW > currentDOW){
      // The event is in the future before the end of the week
      eventTime = timeToMidnight + (eventDOW - currentDOW) * 3600*24 + eventHour * 3600 + eventMinute * 60;
    }
    else{
      // The event is in the future today
      eventTime = (eventHour - currentHour) * 3600 + (eventMinute - currentMinute) * 60;
    }
      

    if (closestEventTime == 0 || eventTime < closestEventTime) {
      closestEventTime = eventTime;
      closestEvent = events[i];
    }
  }
  return closestEvent;
  delay(1000);
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
