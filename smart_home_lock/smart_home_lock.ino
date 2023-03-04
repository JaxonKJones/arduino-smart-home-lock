#include <SoftwareSerial.h>
#include <avr/sleep.h>
#include <EEPROM.h>
#include <Wire.h>
#include "RTClib.h"
#include <Servo.h>


RTC_DS3231 rtc;
Servo lock;

#define STATE_PIN 5  // set state pin to pin 4
#define EN_PIN 4  // set enable pin to pin 5
#define BUTTON_PIN 2  // set button pin to pin 2
#define ALARM_PIN 3 // interrupt pin from DS3231
#define SERVO 9 // pwm pin for servo
#define MOS 12 // servo enable pin
SoftwareSerial BTserial(6, 7); // RX | TX

// function declarations
void auth();
int menu();
void pair();
void scheduler();
void save(char day, char h1, char h2, char m1, char m2, char state);
void del(int index);
char* load(bool print);
long nextEvent(char events[]);
void setAlarm(long);
void bluetoothMode();
void scheduleMode();
void enterSleep();
void dev();
void servo(int state);
void buttonISR();
void alarmISR();

// setup function
void setup() { 
  // Serial Settings
  Serial.begin(9600);
  BTserial.begin(9600);

  //rtc config
  Wire.begin();
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  pinMode(ALARM_PIN, INPUT_PULLUP);
  rtc.disableAlarm(1);
  rtc.disableAlarm(2);
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);
  rtc.writeSqwPinMode(DS3231_OFF);

  //servo config
  lock.attach(9);

  //pin config
  pinMode(STATE_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT); // -- CHANGE TO BE INTERRUPT
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);
  pinMode(MOS, OUTPUT);
  digitalWrite(MOS, LOW);
  if(digitalRead(STATE_PIN) == LOW){
    pair();
  }
  auth();
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
void auth(){
  BTserial.write("\n--Jones Home--\n");
  BTserial.write("Enter Password: ");
  while(true){
    if (BTserial.available()) {
      String input = BTserial.readString();
      delay(30);
      if (input == "1123"){
        BTserial.write("\nAuthenticated...\n");
        return;
      }
      else{
        BTserial.write("\nPassword Incorrect");
      }
    }
  }
}

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
        // Serial.println("Exiting...");
        break;
      }
      else if (input == "1"){
        //Add schedule
        BTserial.write("\nEnter Schedule: (d-hh:mm-U/L) or Exit\n");
        while(true){
          if (BTserial.available()){
            String input = BTserial.readString();
            delay(30);
            if (input == "0"){
              // Serial.println("Exiting...");
              return;
            }
            else{
              // Serial.println(input);
              // Serial.println(strlen(input.c_str()));
              if(strlen(input.c_str()) == 9){
                // Parse input to get hour, minute, and day of week
                char day = input.substring(0, 1).charAt(0);
                char h1 = input.substring(2, 3).charAt(0);
                char h2 = input.substring(3, 4).charAt(0);
                char m1 = input.substring(5, 6).charAt(0);
                char m2 = input.substring(6, 7).charAt(0);
                char state = input.substring(8, 9).charAt(0);
                Serial.println(String(day) + "-" + String(h1) + String(h2) + ":" + String(m1) + String(m2) + "-" + String(state));
                //save schedule to EEPROM
                save(day, h1, h2, m1, m2, state);
                BTserial.write("\nSchedule Saved!\n");
                break;
              }
            }
          }
        }

      }
      else if(input == "2"){
        //Send schedules to BTserial
        load(true);
        BTserial.write("\nEnter Schedule index to Delete or Exit");
        while(true){
          if (BTserial.available()){
            String input = BTserial.readString();
            delay(30);
            if (input == "0"){
              // Serial.println("Exiting...");
              return;
            }
            else{
              // Serial.println(input);
              // Serial.println(strlen(input.c_str()));
              if(strlen(input.c_str()) == 1){
                // Parse input to get hour, minute, and day of week
                int index = input.substring(0, 1).toInt();
                Serial.println(String(index));
                //save schedule to EEPROM
                del(index-1);
                BTserial.write("\nSchedule Deleted!\n");
                break;
              }
            }
          }
        }

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

void save(char day, char h1, char h2, char m1, char m2, char state){
  //check eeprom to see where the next available slot is
  int address = 2; // index 1 reserved for number of schedules, index 2 reserved for next state (U/L)
  int saves = EEPROM.read(0);
  int i = saves*6+address;
  //save day
  EEPROM.write(i, day);
  Serial.println("var saved: " + String(day) + " to address: " + String(i) + " in eeprom");
  //save h1
  EEPROM.write(i + 1, h1);
  Serial.println("var saved: " + String(h1) + " to address: " + String(i + 1) + " in eeprom");  
  //save h2
  EEPROM.write(i + 2, h2);
  Serial.println("var saved: " + String(h2) + " to address: " + String(i + 2) + " in eeprom");
  //save m1
  EEPROM.write(i + 3, m1);
  Serial.println("var saved: " + String(m1) + " to address: " + String(i + 3) + " in eeprom");
  //save m2
  EEPROM.write(i + 4, m2);
  Serial.println("var saved: " + String(m2) + " to address: " + String(i + 4) + " in eeprom");
  //save state
  EEPROM.write(i + 5, state);
  Serial.println("var saved: " + String(state) + " to address: " + String(i + 5) + " in eeprom");
  //increment number of schedules
  EEPROM.write(0, saves + 1);
  Serial.println("var saved: " + String(saves + 1) + " to address: " + String(0) + " in eeprom");

}

void del(int index){
  //go to index in eeprom
  int address = 2 + (index * 6);
  //delete schedule
  EEPROM.write(address, 0);
  EEPROM.write(address + 1, 0);
  EEPROM.write(address + 2, 0);
  EEPROM.write(address + 3, 0);
  EEPROM.write(address + 4, 0);
  EEPROM.write(address + 5, 0);
  //decrement number of schedules
  int saves = EEPROM.read(0);
  EEPROM.write(0, saves - 1);
  //move all schedules after index up
  for(int i = address; i < (saves * 6) + 1; i++){
    EEPROM.write(i, EEPROM.read(i + 6));
  }
}

char* load(bool print){
  int sched = EEPROM.read(0);
  int eventsLength = sched * 6 + 1;//add 1 for null terminator
  char *events = (char*)malloc(eventsLength * sizeof(char));
  if (print){
    // Serial.println("Schedules: " + String(sched));
    // Serial.println("Day:  HH:  MM:  State:");
    BTserial.write(("\nSchedules: " + String(sched) + "\n").c_str());
    BTserial.write(("#:   Day:    HH:   MM:   State:"));
    //print out all schedules from eeprom
    int address = 2;
    for(int i = 0; i < sched; i++){
      BTserial.write(("\n" + String(i+1) + ":     "+ String(char(EEPROM.read(address))) + "     " + String(char(EEPROM.read(address + 1))) + String(char(EEPROM.read(address + 2))) + "     " + String(char(EEPROM.read(address + 3))) + String(char(EEPROM.read(address + 4))) + "       " + String(char(EEPROM.read(address + 5))) + "").c_str());
      address += 6;
      }
    return NULL;
  }
  else{
    // load all schedules from eeprom
    for(int i = 0; i < eventsLength; i++){
      events[i] = char(EEPROM.read(i+2));
      // Serial.println("var loaded: " + String(events[i]) + " from address: " + String(i+2) + " in eeprom");
    }

    //add null as last element of events array
    events[eventsLength] = NULL;
    // Serial.println(events);
    return events;
  }
}

long nextEvent(char events[]) {
  // get length of array
  Serial.println(events);
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
    String event = String(eventDOW) + "-" + String(eventHour) + ":" + String(eventMinute) + "-" + String(state);

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
  Serial.println("Setting alarm for " + String(timeToEvent) + " seconds from now");
  Serial.flush();
  DateTime now = rtc.now();
  DateTime nextAlarm = now.unixtime() + timeToEvent;
  rtc.setAlarm1(nextAlarm, DS3231_A1_Minute);
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
        BTserial.write("\nDoor locked");
      }
      else if (command == '2') {
        //unlock door
        servo(0);
        BTserial.write("\nDoor Unlocked");
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
    char* events = load(false);
    setAlarm(nextEvent(events));
    free(events);
    enterSleep();
    char state = char(EEPROM.read(1));
    Serial.print("State: " + String(state));
    Serial.flush();
    if(state == 'U'){
      servo(0);
    }
    else if(state == 'L'){
      servo(1);
    }
    delay(60000);
  }
}

void enterSleep(){
  //turn off hc-05
  
  sleep_enable();                       // Enabling sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // Setting the sleep mode, in this case full sleep
  
  noInterrupts();                       // Disable interrupts
  attachInterrupt(digitalPinToInterrupt(ALARM_PIN), alarmISR, LOW);
  // Serial.println("Going to sleep!");
  // Serial.flush(); 
  interrupts();                         // Allow interrupts again
  sleep_cpu();                          // Enter sleep mode

  /* The program will continue from here when it wakes */
  
  // Disable and clear alarm
  rtc.disableAlarm(1);
  rtc.clearAlarm(1);
  
}

void dev(){
  BTserial.write("\n--DEBUG--\n");
  BTserial.write("1: States\n2: RTC Info\n3: Set RTC\n4: Reset EEPROM\nOr Exit");
  while(true){
    if (BTserial.available()) {
      String input = BTserial.readString();
      delay(30);
      if (input == "0"){
        // Serial.println("Exiting...");
        break;
      }
      else if (input == "1"){
        BTserial.write("\nStates here...");
      }
      else if(input == "2"){
        DateTime now = rtc.now();
        float temperature = rtc.getTemperature();
        // Serial.println("Date: " + String(now.year()) + "/" + String(now.month()) + "/" + String(now.day()) + "  " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + ", Temperature: " + String(temperature) + " °C");
        BTserial.write(("\nDate: " + String(now.year()) + "/" + String(now.month()) + "/" + String(now.day()) + "  " + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + ", Temperature: " + String(temperature) + " °C").c_str());
      }
      else if(input == "3"){
        set_rtc();
      }
      else if(input == "4"){
        for (int i = 0 ; i < EEPROM.length() ; i++) {
          EEPROM.write(i, 0);
        }
        BTserial.write("\nEEPROM Reset\n");
      }
      else if(input == "5"){
        for (int i = 0 ; i < EEPROM.length() ; i++) {
          Serial.print(String(char(EEPROM.read(i))) + " ");
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
  }
  else if(state == 1){
    digitalWrite(MOS, HIGH);
    delay(50);
    lock.write(90);
    delay(500);
    digitalWrite(MOS, LOW);
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


// Interrupt Service Routines
void buttonISR(){
  // exit to main menu
  Serial.println("Button Pressed");
}

void alarmISR() {
  sleep_disable(); // Disable sleep mode
  detachInterrupt(digitalPinToInterrupt(ALARM_PIN)); // Detach the interrupt to stop it firing
  // Serial.println("Alarm triggered!");
  // read state from eeprom index 1
}