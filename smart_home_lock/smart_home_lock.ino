#include <SoftwareSerial.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <EEPROM.h>
#include <Wire.h>
#include "RTClib.h"
#include <Servo.h>


RTC_DS3231 rtc;
Servo lock;
// #include "LowPower.h"
// #include <TimeLib.h>

// see https://learn.sparkfun.com/tutorials/reducing-arduino-power-consumption/all

#define STATE_PIN 5  // set state pin to pin 4
#define EN_PIN 4  // set enable pin to pin 5
#define BUTTON_PIN 2  // set button pin to pin 2
#define CLOCK 3 // interrupt pin from DS3231
#define SERVO 9 // pwm pin for servo
#define MOS 12 // servo enable pin
SoftwareSerial BTserial(6, 7); // RX | TX

// function declarations
int menu();
void pair();
void scheduler();
void save(int day, int hour, int minute, char state);
void del(int index);
char* load(bool print);
String nextEvent(char* events);
void setAlarm(String event);
void bluetoothMode();
void scheduleMode();
void dev();
void servo(int state);
void buttonInterrupt();
void rtcInterrupt();

// setup function
void setup() {
  // Low Power Settings
  ADCSRA = 0; // Disable ADC
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set sleep mode to power-down
  sleep_enable(); // Enable sleep mode
 
  // Serial Settings
  Serial.begin(9600);
  BTserial.begin(9600);

  //rtc config
  Wire.begin();
  rtc.begin();
  pinMode(CLOCK, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CLOCK), rtcInterrupt, FALLING);
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);
  rtc.disableAlarm(2);
  // rtc.setAlarm1(“00:00:01:00”, RTC_OFFSET, RTC_ALM1_MODE4 ); 


  lock.attach(9);
  pinMode(STATE_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT); // -- CHANGE TO BE INTERRUPT
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);
  pinMode(MOS, OUTPUT);
  digitalWrite(MOS, LOW);
  if(digitalRead(STATE_PIN) == LOW){
    pair();
  }
}

// main loop
void loop() {
  int mode = menu();
  if (mode == '1'){
    //run scheduler
    scheduler();
  }
  if (mode == '2'){
    //run bluetooth mode
    bluetoothMode();
  }
  else if (mode == '3'){
    //run schedule mode
    scheduleMode();
  }
  else if (mode =='4'){
    // run debug mode
    dev();
  }
}


// function definitions
int menu(){
  if(digitalRead(STATE_PIN) == LOW){
    pair();
  }
  BTserial.write("\n---Main Menu---\n1: Scheduler\n2: Bluetooth Mode\n3: Schedule Mode\n4: Dev");
  while(true){
    if (BTserial.available()) {
      char command = BTserial.read();
      if (command == '0') {
        return command;
      } 
      else if (command == '1') {
        return command;
      }
      else if (command == '2') {
        return command;
      }
      else if (command == '3') {
        return command;
      }
      else if (command == '4') {
        return command;
      }
    }
    if (digitalRead(STATE_PIN) == LOW){
      pair();
    }
  }
    
}

void pair(){
  Serial.print("Pairing...");
  while(digitalRead(STATE_PIN) == LOW){
    Serial.print(" ... ");
    delay(3000);
  } 
  Serial.println("Paired!");
}

void scheduler(){  
  BTserial.write("\n--Scheduler--\n");
  BTserial.write("1:Add  2:Delete  3:See all Schedules   or Exit\n");
  // format is d:hh:mm - ex: 1:12:00 is Sunday at 12:00
  while(true){
    if (BTserial.available()) {
      String input = BTserial.readString();
      delay(30);
      if (input == "0"){
        Serial.println("Exiting...");
        break;
      }
      else if (input == "1"){
        //Add schedule
        BTserial.write("\nEnter Schedule: (d:hh:mm:U/L) or Exit\n");
        while(true){
          if (BTserial.available()){
            String input = BTserial.readString();
            delay(30);
            if (input == "0"){
              Serial.println("Exiting...");
              return;
            }
            else{
              // Parse input to get hour, minute, and day of week
              int day = input.substring(0, 1).toInt();
              int hour = input.substring(2, 4).toInt();
              int minute = input.substring(5, 7).toInt();
              String stateTemp = input.substring(8, 9);
              char state = stateTemp.charAt(0);
              //save schedule to EEPROM
              save(day, hour, minute, state);
              // Serial.println(day);
              // Serial.println(hour);
              // Serial.println(minute);
              // Serial.println(state);
              BTserial.write("\nSchedule set!");
            }
          }
        }

      }
      else if(input == "2"){
        //Delete schedule
      }
      else if(input == "3"){
        //See all schedules
        load(true);
      }
      
    }
    if(digitalRead(STATE_PIN) == LOW){
      pair();
    }
  }
}

void save(int day, int hour, int minute, char state){
  //check eeprom to see where the next available slot is
  int address = 1;
  int saves = EEPROM.read(0);
  while(EEPROM.read(address) != 0){
    address += 4;
  }
  //save day
  EEPROM.write(address, day);
  //save hour
  EEPROM.write(address + 1, hour);
  //save minute
  EEPROM.write(address + 2, minute);
  //save state
  EEPROM.write(address + 3, state);
  //save number of schedules
  EEPROM.write(0, saves + 1);
}

void del(int index){
  //go to index in eeprom
  int address = 1 + (index * 4);
  //delete schedule
  EEPROM.write(address, 0);
  EEPROM.write(address + 1, 0);
  EEPROM.write(address + 2, 0);
  EEPROM.write(address + 3, 0);
  //decrement number of schedules
  int saves = EEPROM.read(0);
  EEPROM.write(0, saves - 1);
  //move all schedules after index up
  while(EEPROM.read(address + 4) != 0){
    EEPROM.write(address, EEPROM.read(address + 4));
    EEPROM.write(address + 1, EEPROM.read(address + 5));
    EEPROM.write(address + 2, EEPROM.read(address + 6));
    EEPROM.write(address + 3, EEPROM.read(address + 7));
    address += 4;
  }
}

char* load(bool print){
  int sched = EEPROM.read(0);
  char* events[sched];
  if (print){
    Serial.print("Schedules: ");
    Serial.println(sched);
    Serial.println("Day:  Hour:  Minute:  State:");
    //print out all schedules from eeprom
    int address = 1;
    while(EEPROM.read(address) != 0){
      int day = EEPROM.read(address);
      int hour = EEPROM.read(address + 1);
      int minute = EEPROM.read(address + 2);
      char state = EEPROM.read(address + 3);
      Serial.print(day);
      Serial.print("      ");
      Serial.print(hour);
      Serial.print("      ");
      Serial.print(minute);
      Serial.print("      ");
      Serial.println(state);
      
      address += 4;
    }
    return NULL;
  }
  else{
    int address = 1;
    int i = 0;
    while(EEPROM.read(address) != 0){
      int day = EEPROM.read(address);
      int hour = EEPROM.read(address + 1);
      int minute = EEPROM.read(address + 2);
      char state = EEPROM.read(address + 3);
      Serial.print(day);
      Serial.print("      ");
      Serial.print(hour);
      Serial.print("      ");
      Serial.print(minute);
      Serial.print("      ");
      Serial.println(state);
      
      address += 4;
      //add event to events array
      String event = String(day) + ":" + String(hour) + ":" + String(minute) + ":" + String(state);
      events[i] = event;
      i++;
    }
    return events;
  }
  
}

char* nextEvent(char* events[]) {
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
    // Serial.print(currentHour);
    // Serial.print(":");
    // Serial.print(currentMinute);
    long timeToMidnight = (24L * 3600L) - (currentHour * 3600L + currentMinute * 60L); 
    
    // Serial.print("Event: ");
    // Serial.print("");
    // Serial.println(String(events[i]));  
    
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
    // Serial.print("Time to Event: ");
    // Serial.print(eventTime);
    // Serial.print(" ");
    // Serial.println(debugMsg);

    if (closestEventTime == 0 || eventTime < closestEventTime) {
      closestEventTime = eventTime;
      closestEvent = events[i];
      // Serial.print("\nNew Closest Event: ");
      // Serial.println(closestEvent);
    }
    delay(2000);
  }
  // Serial.println("");
  // Serial.print("Closest Event: ");
  // Serial.println(closestEvent);
  return closestEvent;
}

void setAlarm(String event){

}

void bluetoothMode(){
  if(digitalRead(STATE_PIN) == LOW){
    pair();
  }
  BTserial.write("\n--Bluetooth Mode Active--");
  BTserial.write("\n1: Lock Door\n2: Unlock Door\n or Exit to Main Menu\n");
  while(true){
    if (BTserial.available()) {
      char command = BTserial.read();
      if (command == '1') {
        //lock door
        servo(1);
      }
      else if (command == '2') {
        //unlock door
        servo(0);
      }
      else if (command == '0') {
        //exit to main menu
        break;        
      }
      else{
        // BTserial.write("\nCommand not recognized...");
      }
    }
  }
}

void scheduleMode(){
  BTserial.write("\n--Schedule Mode Active--\n");
  BTserial.write("Bluetooth will disactivate but can be awoken using the on system switch...\n");
  while(true){
    setAlarm(nextEvent(load(false)))
    // sleep_cpu();
    delay(1000);
  }
}

void dev(){
  BTserial.write("\n--DEBUG--\n");
  BTserial.write("1: States\n2: RTC Info\n3: Set RTC\n4: Reset EEPROM\nOr Exit");
  while(true){
    if (BTserial.available()) {
      String input = BTserial.readString();
      delay(30);
      if (input == "0"){
        Serial.println("Exiting...");
        break;
      }
      else if (input == "1"){
        BTserial.write("\nStates here...");
      }
      else if(input == "2"){
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
      else if(input == "3"){
        set_rtc();
      }
      else if(input == "4"){
        for (int i = 0 ; i < EEPROM.length() ; i++) {
          EEPROM.write(i, 0);
        }
      }
    }
  }
}

void servo(int state){
  if(state == 0){
    digitalWrite(MOS, HIGH);
    delay(50);
    lock.write(0);
    delay(500);
    digitalWrite(MOS, LOW);
    BTserial.write("\nDoor Unlocked");
  }
  else if(state == 1){
    digitalWrite(MOS, HIGH);
    delay(50);
    lock.write(90);
    delay(500);
    digitalWrite(MOS, LOW);
    BTserial.write("\nDoor Locked");
  }
}

void set_rtc(){
  BTserial.write("--Set Time & Day (d:hh:mm)--");
  while(true){
    if (BTserial.available()) {
      String time = BTserial.readString();
      Serial.println(time);
      int day = time.substring(0, 1).toInt();
      int hour = time.substring(2, 4).toInt();
      int minute = time.substring(5, 7).toInt();
      break;
    }
  }
}

void buttonInterrupt(){
  // exit to main menu
  Serial.println("Button Pressed");
}

void rtcInterrupt(){
  if (rtc.alarmFired(1)) { 
      Serial.println("Alarm!");
      delay(1000); // wait for a second
      rtc.clearAlarm(1); // clear alarm flag 
    }
}