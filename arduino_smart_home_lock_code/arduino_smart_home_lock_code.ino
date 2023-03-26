#include <SoftwareSerial.h>
#include <avr/sleep.h>
#include <EEPROM.h>
#include <Wire.h>
#include "RTClib.h"
#include <Servo.h>


RTC_DS3231 rtc;
Servo lock;

#define STATE_PIN 5  // set state pin to pin 5
#define EN_PIN 4  // set enable pin to pin 4
#define BUTTON_PIN 3  // set button pin to pin 3
volatile bool fromButton = false;
volatile int buttonState = 0;
volatile long buttonCalls = 0;
#define ALARM_PIN 2 // interrupt pin from DS3231
#define BLUETOOTH_MOS 8 // mosfet for hc-05
#define SERVO 9 // pwm pin for servo
#define SERVO_MOS 11 // servo enable pin
SoftwareSerial BTserial(7, 6); // RX | TX

// User Variables
#define password "11235" //set your password here
#define verbose false //turn on when debugging from serial monitor, otherwise keep this false
#define TEST false //keep this false unless testing specific code in the test() function

// function declarations
void bprint(String arg);
void Fbprint(__FlashStringHelper* arg);
void auth();
int menu();
void pair(bool checkTimeOut);
void scheduler();
void save(char day, char h1, char h2, char m1, char m2, char state);
void del(int index);
char* load(bool print);
long nextEvent(char events[]);
void setAlarm(long);
void bluetoothMode();
void bluetoothLimited();
void scheduleMode();
void enterSleep();
void dev();
void servo(int state);
void vacationMode();
void buttonISR();
void alarmISR();


// setup function
void setup() { 
  // Serial Settings
  if(verbose) {
    Serial.begin(9600);
  }
  BTserial.begin(9600);
  Fbprint(F("Powering on..."));

  //rtc config
  Wire.begin();
  rtc.begin();
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //Keep this commented out otherwise the RTC will reset anytime the arduino is powercycled
  pinMode(ALARM_PIN, INPUT_PULLUP);
  rtc.disableAlarm(1);
  rtc.disableAlarm(2);
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);
  rtc.writeSqwPinMode(DS3231_OFF);
  rtc.disable32K();

  //servo config
  lock.attach(9);

  //pin config
  pinMode(STATE_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BLUETOOTH_MOS, OUTPUT);
  digitalWrite(BLUETOOTH_MOS, HIGH);
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);
  pinMode(SERVO_MOS, OUTPUT);
  digitalWrite(SERVO_MOS, LOW);

  if(TEST == true){
    test();
  }

  //pair if not already connected
  if(digitalRead(STATE_PIN) == LOW){
    pair(true); // pair mode with timeout
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
    //run schedule mode
    scheduleMode();
  }
  else if (mode == '3'){
    //run bluetooth mode
    bluetoothMode();
  }
  else if (mode =='4'){
    //config bluetooth limited mode
    bluetoothLimited();
  }
  else if (mode == '5'){
    // run vacation mode
    vacationMode();
  }
  else if (mode == '6'){
    // run dev mode
    dev();
  }
}

void test(){
  Fbprint(F("Testing..."));
  // BTserial.print(F(\Testing...\n"));
  while(true){
    //test code here.
     char events[] = {'5', '1', '1', '1', '0', 'L','5', '1', '1', '1', '2', 'L','5', '1', '1', '1', '3', 'L','5', '1', '1', '1', '4', 'L', NULL};
    // char events[] = {'5', '1', '1', '0', '8', 'L'};
    long alarm = nextEvent(events);
    setAlarm(alarm);
    enterSleep();

    // delay(10000);
  }
}

// function definitions
void bprint(String arg) {
  if (verbose){
    Serial.println(arg);
  }
}

void Fbprint(__FlashStringHelper* arg) {
  if (verbose) {
    Serial.println(arg);
  }
}

void auth(){
  BTserial.print(F("\n-Home Lock-"));
  BTserial.print(F("\nEnter Password: "));
  long startTime = millis();
  // if user takes longer than 60s to enter password, it will go to scheduleMode
  while(true){
    if (BTserial.available()) {
      String input = BTserial.readString();
      delay(30);
      if (input == password){
        BTserial.print(F("\nAuthenticated..."));
        Fbprint(F("Authenticated..."));
        return;
      }
      else{
        BTserial.print(F("\nPassword Incorrect"));
      }
      // check to see if it has been 60s
      if (millis() - startTime > 60000){
        BTserial.print(F("\nTimed Out..."));
        scheduleMode();
      }
    }
  }
}

int menu(){
  if(digitalRead(STATE_PIN) == LOW){
    pair(false);
  }
  BTserial.print(F("\n\n-Main Menu-\n1: Scheduler\n2: Schedule Mode\n3: Bluetooth Mode\n4: Bluetooth Limited\n5: Vacation Mode\n6: Dev"));
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
      else if (command == '5') {
        return command;
      }
      else if (command == '6') {
        return command;
      }
      else if (command == '7') {
        return command;
      }
      else if (command == '8') {
        return command;
      }
      else if (command == '9') {
        return command;
      }
    }
    if (digitalRead(STATE_PIN) == LOW){
      pair(false);
    }
  }
    
}

void pair(bool checkTimeOut){
  Fbprint(F("Pairing..."));
  long startTime = millis();
  while(digitalRead(STATE_PIN) == LOW){
    delay(1000);
    if (checkTimeOut && millis() - startTime > 60000){
      Fbprint(F("Timed Out..."));
      scheduleMode();
      return;
    }
  } 
  Fbprint(F("Paired!"));
}

void scheduler(){  
  Fbprint(F("Scheduler"));
  BTserial.print(F("\n\n-Scheduler-"));
  BTserial.print(F("\n1:Add  2:Delete  3:See all Schedules   or Exit"));
  // format is d:hh:mm - ex: 1:12:00 is Sunday at 12:00
  while(true){
    if (BTserial.available()) {
      String input = BTserial.readString();
      delay(30);
      if (input == "0"){
        break;
      }
      else if (input == "1"){
        //Add schedule
        BTserial.print(F("\nEnter Schedule: (d-hh:mm-U/L) or Exit"));
        while(true){
          if (BTserial.available()){
            String input = BTserial.readString();
            delay(30);
            if (input == "0"){
              return;
            }
            else{
              bprint(input);
              if(strlen(input.c_str()) == 9){
                // Parse input to get hour, minute, and day of week
                char day = input.substring(0, 1).charAt(0);
                char h1 = input.substring(2, 3).charAt(0);
                char h2 = input.substring(3, 4).charAt(0);
                char m1 = input.substring(5, 6).charAt(0);
                char m2 = input.substring(6, 7).charAt(0);
                char state = input.substring(8, 9).charAt(0);
                //save schedule to EEPROM
                save(day, h1, h2, m1, m2, state);
                break;
              }
            }
          }
        }

      }
      else if(input == "2"){
        //Send schedules to BTserial
        load(true);
        BTserial.print(F("\nEnter Schedule index to Delete or Exit"));
        while(true){
          if (BTserial.available()){
            String input = BTserial.readString();
            delay(30);
            if (input == "0"){
              return;
            }
            else{
              bprint(input);
              if(strlen(input.c_str()) <= 2){
                // Parse input to get hour, minute, and day of week
                int index = input.toInt();
                //save schedule to EEPROM
                del(index-1);
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
      pair(false);
    }
  }
}

void save(char day, char h1, char h2, char m1, char m2, char state){
  //check eeprom to see where the next available slot is
  int address = 2; // index 0 reserved for number of schedules, index 1 reserved for next state (U/L)
  int saves = EEPROM.read(0);
  if (saves < 80){
    int i = saves*6+address;
    //save day
    EEPROM.write(i, day);
    bprint("var saved: " + String(day) + " to address: " + String(i) + " in eeprom");

    //save h1
    EEPROM.write(i + 1, h1);
    bprint("var saved: " + String(h1) + " to address: " + String(i + 1) + " in eeprom");

    //save h2
    EEPROM.write(i + 2, h2);
    bprint("var saved: " + String(h2) + " to address: " + String(i + 2) + " in eeprom");

    //save m1
    EEPROM.write(i + 3, m1);
    bprint("var saved: " + String(m1) + " to address: " + String(i + 3) + " in eeprom");

    //save m2
    EEPROM.write(i + 4, m2);
    bprint("var saved: " + String(m2) + " to address: " + String(i + 4) + " in eeprom");

    //save state
    EEPROM.write(i + 5, state);
    bprint("var saved: " + String(state) + " to address: " + String(i + 5) + " in eeprom");

    //increment number of schedules
    EEPROM.write(0, saves + 1);
    bprint("var saved: " + String(saves + 1) + " to address: " + String(0) + " in eeprom");

    BTserial.print(F("\nSaved!"));
  }
  else{
    BTserial.print(F("\nError: Max number of schedules reached!"));
  }
  
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
  BTserial.print(F("\nSchedule Deleted!\n"));
}

char* load(bool print){
  int sched = EEPROM.read(0);
  int eventsLength = sched * 6 + 1;//add 1 for null terminator
  char *events = (char*)malloc(eventsLength * sizeof(char));
  if (print){
    BTserial.print(("\nSchedules: " + String(sched) + "\n").c_str());
    BTserial.print(F("#:   Day:    HH:   MM:   State:"));
    //print out all schedules from eeprom
    int address = 2;
    for(int i = 0; i < sched; i++){
      BTserial.print(("\n" + String(i+1) + ":     "+ String(char(EEPROM.read(address))) + "     " + String(char(EEPROM.read(address + 1))) + String(char(EEPROM.read(address + 2))) + "     " + String(char(EEPROM.read(address + 3))) + String(char(EEPROM.read(address + 4))) + "       " + String(char(EEPROM.read(address + 5))) + "").c_str());
      address += 6;
      }
    return NULL;
  }
  else{
    // load all schedules from eeprom
    for(int i = 0; i < eventsLength; i++){
      events[i] = char(EEPROM.read(i+2));
      // bprint("var loaded: " + String(events[i]) + " from address: " + String(i+2) + " in eeprom");
    }

    //add null as last element of events array
    events[eventsLength] = NULL;
    bprint(String(events));
    return events;
  }
}

long nextEvent(char events[]) {
  // get length of array
  int numEvents = EEPROM.read(0);
  bprint("Number of Events: " + String(numEvents));
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

    if(eventTime == 0){
      // event just occured, set to next week
      eventTime = 3600L*24L*7L;
    }

    if(closestEventTime == 0 || eventTime < closestEventTime) {
      closestEventTime = eventTime;
      nextState = state;
      eventIndex = i;
      nextEvent = event;

    }
  }
  bprint("Next Event: " + nextEvent);
  bprint("Time to event: " + String(closestEventTime) + " (" +String(float(closestEventTime)/float(3600L)) +" hours)");
  bprint("Next State: " + String(nextState));
  Serial.flush();
  
  EEPROM.write(1, nextState); // save next state to EEPROM index 1

  return closestEventTime;
}

void setAlarm(long timeToEvent) {
  bprint("Setting alarm for " + String(timeToEvent) + " seconds from now \n");
  Serial.flush();
  DateTime now = rtc.now();
  DateTime nextAlarm = DateTime(now.unixtime() + timeToEvent);
  rtc.setAlarm1(nextAlarm, DS3231_A1_Day); // See https://github.com/garrysblog/DS3231-Alarm-With-Adafruit-RTClib-Library for more info
}

void bluetoothMode(){
  Fbprint(F("Bluetooth Mode"));
  if(digitalRead(STATE_PIN) == LOW){
    pair(false);
  }
  BTserial.print(F("\n\n-Bluetooth Mode-"));
  BTserial.print(F("\n1: Lock Door\n2: Unlock Door\n or Exit"));
  while(true){
    if (BTserial.available()) {
      char command = BTserial.read();
      if (command == '1') {
        //lock door
        servo(1);
        BTserial.print(F("\nLocked!"));
      }
      else if (command == '2') {
        //unlock door
        servo(0);
        BTserial.print(F("\nUnlocked!"));
      }
      else if (command == '0') {
        //exit to main menu
        break;        
      }
    }
  }
}

void bluetoothLimited(){
  // Function is not complete
  return;

  BTserial.print(F("\n\n-Bluetooth Limited-"));
  BTserial.print(F("\n1: Run Bluetooth Limited  2: Set Daily Schedule or Exit to Main Menu"));
  while(true){
    if (BTserial.available()) {
      char command = BTserial.read();
      if (command == '0') {
        //exit to main menu
        break;        
      }
      else if (command == '1') {
        //run bluetooth limited
        
      }
      else if (command == '2') {
        //set hours for bluetooth module to be on each day 
        BTserial.print(F("\n-Format HH:MM HH:MM-")); //start hour and minute, end hour and minute
        BTserial.print(F("\n-Example 08:00 20:00-")); //start hour and minute, end hour and minute
        while(true){
          if (BTserial.available()) {
            String command = BTserial.readString();
            if (command == '0') {
              //exit to main menu
              return;        
            }
            else{
              // parse command for start and end time
              char starth1 = command.substring(0,1).charAt(0);
              char starth2 = command.substring(1,2).charAt(0);
              char startm1 = command.substring(3,4).charAt(0);
              char startm2 = command.substring(4,5).charAt(0);
              char endh1 = command.substring(6,7).charAt(0);
              char endh2 = command.substring(7,8).charAt(0);
              char endm1 = command.substring(9,10).charAt(0);
              char endm2 = command.substring(10,11).charAt(0);

              // save start and end time to EEPROM - start at index 500
              EEPROM.write(500, starth1);
              EEPROM.write(501, starth2);
              EEPROM.write(502, startm1);
              EEPROM.write(503, startm2);
              EEPROM.write(504, endh1);
              EEPROM.write(505, endh2);
              EEPROM.write(506, endm1);
              EEPROM.write(507, endm2);
            }
          }
        }
        

      }
    }
  }
}

void scheduleMode(){
  Fbprint(F("Schedule Mode"));
  BTserial.print(F("\n\n-Schedule Mode Active-"));
  BTserial.print(F("\nBluetooth will deactivate but can be awoken using the on system switch..."));
  delay(500); //wait for bluetooth to send message
  digitalWrite(BLUETOOTH_MOS, LOW); //turn off bluetooth module
  int saves = EEPROM.read(0);
  while(saves != 0){
    char* events = load(false);    
    setAlarm(nextEvent(events));
    free(events);
    fromButton = false;
    enterSleep();
    bprint("fromButton: " + String(fromButton));
    //determine if alarm or button interrupt woke up
    if(fromButton == false){
    //interrupt came from alarm
      char state = char(EEPROM.read(1));
      bprint("State: " + String(state));
      Serial.flush();
      if(state == 'U'){
        servo(0);
        buttonState = 0;
      }
      else if(state == 'L'){
        servo(1);
        buttonState = 1;
      }
      delay(60000); //NOTE: temporary fix for alarm repeating - will disable lock for 1 minute
    }
    if(fromButton == true){
      if (buttonState == 0){
        buttonState = 1;
        servo(0);
      }
      else{
        buttonState = 0;
        servo(1);
      }
    }
  }

  //Otherwise, no events set - so sleep CPU
  sleep_enable();                       // Enabling sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // Setting the sleep mode, in this case full sleep
  noInterrupts();                       // Disable interrupts
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, LOW);
  interrupts();                         // Allow interrupts again
  sleep_cpu();
}

void enterSleep(){
  //turn off hc-05
  
  sleep_enable();                       // Enabling sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // Setting the sleep mode, in this case full sleep
  
  noInterrupts();                       // Disable interrupts
  attachInterrupt(digitalPinToInterrupt(ALARM_PIN), alarmISR, LOW);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, LOW);
  interrupts();                         // Allow interrupts again
  Fbprint(F("Sleeping..."));
  Serial.flush();
  sleep_cpu();                          // Enter sleep mode
  Fbprint(F("Awake..."));

  /* The program will continue from here when it wakes */
  
  // Disable and clear alarm
  rtc.disableAlarm(1);
  rtc.clearAlarm(1);
}

void dev(){
  Fbprint(F("Dev Mode"));
  BTserial.print(F("\n\n-DEBUG-"));
  BTserial.print(F("\n1: Set RTC\n2: RTC Info\n3:Reset EEPROM\nOr Exit"));
  while(true){
    if (BTserial.available()) {
      char input = BTserial.read();
      delay(30);
      if (input == '0'){
        break;
      }
      else if (input == '1'){
        set_rtc();
      }
      else if(input == '2'){
        DateTime now = rtc.now();
        float temp = rtc.getTemperature();
        BTserial.print(("\n" + String(now.day()) + "  " + String(now.hour()) + ":" + String(now.minute()) + "     Temp: " + String(temp).c_str()));
        bprint("RTC Info: " + String(now.day()) + "  " + String(now.hour()) + ":" + String(now.minute()) + "    Temp: " + String(temp).c_str());
      }
      else if(input == '3'){
        EEPROM.write(0, 0);
        for (int i = 1 ; i < EEPROM.length() ; i++) {
          EEPROM.write(i, NULL);
        }
        BTserial.print(F("\nEEPROM Reset\n"));
        Fbprint(F("EEPROM Reset"));
      }
      else if(input == '4'){
        for (int i = 0 ; i < EEPROM.length() ; i++) {
          bprint(String(char(EEPROM.read(i))) + " ");
        }
      }
    }
  }
}

void servo(int state){
  if(state == 0){
    //unlock door
    digitalWrite(SERVO_MOS, HIGH);
    delay(50);
    lock.write(140);
    Fbprint(F("Door Unlocked"));
    delay(500);
    digitalWrite(SERVO_MOS, LOW);
  }
  else if(state == 1){
    //lock door
    digitalWrite(SERVO_MOS, HIGH);
    delay(50);
    lock.write(30);
    Fbprint(F("Door Locked"));
    delay(500);
    digitalWrite(SERVO_MOS, LOW);
  }
}

void vacationMode(){
  //turn off the bluetooth module and do not look for alarms
  Fbprint(F("Vacation Mode"));
  BTserial.print(F("\n\n-Vacation Mode Active-"));
  BTserial.print(F("\nBluetooth will deactivate but can be awoken using the on system switch..."));
  delay(2000); //wait for bluetooth to send message
  digitalWrite(BLUETOOTH_MOS, LOW); //turn off bluetooth module
  while(true){
    //sleep the cpu
    sleep_enable();                       // Enabling sleep mode
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // Setting the sleep mode, in this case full sleep
    
    noInterrupts();                       // Disable interrupts     
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, LOW);
    interrupts();                         // Allow interrupts again
    sleep_cpu();                          // Enter sleep mode

    //allow button operation only
    if(fromButton == true){
      if (buttonState == 0){
        buttonState = 1;
        servo(0);
      }
      else{
        buttonState = 0;
        servo(1);
      }
      fromButton = false;
      delay(1000);
    }
  }
}

void set_rtc(){
  BTserial.print(F("-Set Date Time (yyyy:MM:dd:hh:mm:ss)-"));
  while(true){
    if (BTserial.available()) {
      String time = BTserial.readString();
      delay(30);
      int year = time.substring(0, 4).toInt();
      int month = time.substring(5, 7).toInt();
      int day = time.substring(8, 10).toInt();
      int hour = time.substring(11,13).toInt();
      int minute = time.substring(14,16).toInt();
      int second = time.substring(17,19).toInt();
      rtc.adjust(DateTime(year, month, day, hour, minute, second));
      break;
    }
  }
}


// Interrupt Service Routines
void buttonISR(){
  buttonCalls += 1;
  sleep_disable(); // Disable sleep mode
  detachInterrupt(digitalPinToInterrupt(BUTTON_PIN)); // Detach the interrupt to stop it firing
  fromButton = true;
  Fbprint(F("Button pressed!"));
}

void alarmISR() {
  sleep_disable(); // Disable sleep mode
  detachInterrupt(digitalPinToInterrupt(ALARM_PIN)); // Detach the interrupt to stop it firing
  fromButton = false;
  bprint("Alarm triggered!");
}