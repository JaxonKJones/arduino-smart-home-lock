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
#define CLOCK_SQW 3 // interrupt pin from DS3231
#define SERVO 9 // pwm pin for servo
#define MOS 12 // servo enable pin
SoftwareSerial BTserial(6, 7); // RX | TX


int state = digitalRead(STATE_PIN);

void setup() {
  // Disable ADC
  ADCSRA = 0;
  // Set sleep mode to power-down
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  // Enable sleep mode
  sleep_enable();
 
  Serial.begin(9600);
  BTserial.begin(9600);
  Wire.begin();
  rtc.begin();
  lock.attach(9);
  pinMode(STATE_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT); // -- CHANGE TO BE INTERRUPT
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);
  pinMode(MOS, OUTPUT);
  digitalWrite(MOS, LOW);
  if(state == LOW){
    pair();
  }
}

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





int menu(){
  if(state == LOW){
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
  }
}

void pair(){
  Serial.print("Pairing...");
  int state = digitalRead(STATE_PIN);
  while(state == LOW){
    Serial.print(" ... ");
    delay(3000);
    state = digitalRead(STATE_PIN);
  } 
  Serial.println("Paired!");
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

void scheduler(){  
  BTserial.write("\n--Scheduler--\n");
  BTserial.write("Enter Schedule: (d:hh:mm:U/L) or exit\n");
  // format is d:hh:mm - ex: 1:12:00 is Sunday at 12:00
  while(true){
    if (BTserial.available()) {
      String input = BTserial.readString();
      delay(30);
      if (input == "0"){
        Serial.println("Exiting...");
        break;
      }
      else{
        // Parse input to get hour, minute, and day of week
        int day = input.substring(0, 1).toInt();
        int hour = input.substring(2, 4).toInt();
        int minute = input.substring(5, 7).toInt();
        String stateTemp = input.substring(8, 9);
        char state = stateTemp.charAt(0);
        //save schedule to EEPROM
        // save(day, hour, minute, state);
        Serial.println(day);
        Serial.println(hour);
        Serial.println(minute);
        Serial.println(state);
        BTserial.write("\nSchedule set!");
      }
    }
  }
}

void save(int day, int hour, int minute, char state){
  //check eeprom to see where the next available slot is
  int address = 3;
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
}

void load(){
  //load schedule from EEPROM
  int address = 3;
  while(EEPROM.read(address) != 0){
    int day = EEPROM.read(address);
    int hour = EEPROM.read(address + 1);
    int minute = EEPROM.read(address + 2);
    char state = EEPROM.read(address + 3);
    // Serial.println(day);
    // Serial.println(hour);
    // Serial.println(minute);
    // Serial.println(state);
    address += 4;
  }
}

void bluetoothMode(){
  if(state == LOW){
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
    // check to see if button has been pressed. If so, exit to main menu
    // if (digitalRead(BUTTON_PIN)==HIGH){
    //   BTserial.write("button pressed...");
    //   break;
    // }


    // load schedule from EEPROM
    // load();

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

void button_interrupt(){
  // exit to main menu
  Serial.println("Button Pressed");
}