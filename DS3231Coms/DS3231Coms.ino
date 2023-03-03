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
String nextEvent(char* events[]);


// main loop
void loop() {
  char* events[] = {"5:17:05:U", "6:15:02:U", "4:19:00", "2:20:01:L", "3:23:00:U", "4:01:00:L", "4:18:00:U", "5:15:00:L", "0:10:00:U", NULL};
  int saves = 2
  char sched[6*saves]; // array of 10 pointers to strings
  for i in 
  String final = num1 + ":" + num2 + ":" + num3 + ":" + state;
  myStrings[0] = ;
  Serial.println(myStrings[0]);

  // // add a string to the array
  // myArray[0] = (char*) malloc(strlen("hello") + 1); // allocate memory for the string
  // strcpy(myArray[0], "hello"); // copy the string into the allocated memory

  // // add another string to the array
  // myArray[1] = (char*) malloc(strlen("world") + 1); // allocate memory for the string
  // strcpy(myArray[1], "world"); // copy the string into the allocated memory

  // nextEvent(events);
  delay(10000);
}


// function definitions
String nextEvent(char* events[]) {
  // get length of array based off NULL operator
  int numEvents = 0;
  while (events[numEvents] != NULL) {
    numEvents++;
  }
  Serial.print("Number of Events: ");
  Serial.println(numEvents);
  DateTime now = rtc.now();      // get current date/time from RTC module
  long currentDOW = now.dayOfTheWeek();
  long currentHour = now.hour();
  long currentMinute = now.minute();
  long eventTime = 0;
  long closestEventTime = 0;
  String closestEvent = "";
  String debugMsg = "";

  for (int i = 0; i < numEvents; i++, eventTime=0) {
    String event = String(events[i]);
    long eventDOW = event.substring(0,1).toInt();
    long eventHour = event.substring(2,4).toInt();
    long eventMinute = event.substring(5,7).toInt();
    Serial.print(currentHour);
    Serial.print(":");
    Serial.print(currentMinute);
    long timeToMidnight = (24L * 3600L) - (currentHour * 3600L + currentMinute * 60L); 
    
    Serial.print("Event: ");
    Serial.print("");
    Serial.println(String(events[i]));  
    
    if (eventDOW < currentDOW || (eventDOW == currentDOW && (eventHour < currentHour || (eventHour == currentHour && eventMinute < currentMinute)))) {
      // The event is in the past or next week
      eventTime = timeToMidnight + (eventDOW - currentDOW) * 3600L*24L + eventHour * 3600L + eventMinute * 60L;
      eventTime += 3600L*24L*7L; // Add one week to the time
      debugMsg = "Event will occur next week";
    }
    else if(eventDOW > currentDOW){
      // The event is in the future before the end of the week
      eventTime = timeToMidnight + (eventDOW - currentDOW) * 3600L*24L + eventHour * 3600L + eventMinute * 60L;
      debugMsg = "Event is in the future before the end of the week";
    }
    else{
      // The event is in the future today
      eventTime = (eventHour - currentHour) * 3600L + (eventMinute - currentMinute) * 60L;
      debugMsg = "Event is in the future today";
    }
    // Serial.print("Time to Midnight: ");
    // Serial.print(timeToMidnight);
    // Serial.print(" ");
    Serial.print("Time to Event: ");
    Serial.print(eventTime);
    Serial.print(" ");
    Serial.println(debugMsg);

    if (closestEventTime == 0 || eventTime < closestEventTime) {
      closestEventTime = eventTime;
      closestEvent = events[i];
      // Serial.print("\nNew Closest Event: ");
      // Serial.println(closestEvent);
    }
    delay(2000);
  }
  Serial.println("");
  Serial.print("Closest Event: ");
  Serial.println(closestEvent);
  return closestEvent;
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
