#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  // uncomment the following line to set the time and date for the first time
  rtc.adjust(DateTime(2022, 2, 25, 2, 49, 0));
  // rtc.enableInterrupts(RTC_DS3231::Alarm1);
  // rtc.clearAlarm(RTC_DS3231::Alarm1);
  // rtc.writeSqwPinMode(RTC_DS3231::SquareWave1Hz);
}

void loop() {
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