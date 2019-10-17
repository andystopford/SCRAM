#include <Arduino.h>
#include "Wire.h"
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
// define pins
byte PROBE_PIN(A0);
byte RELAY_PIN(7);
byte BZR_PIN(3);
byte WRN_PIN(6);
// Level circuit voltages for triggering warnings. Determine values by
// measurement when commisioning device.
int LEVEL_DANGER(600);  // Trigger shutdown
int LEVEL_WARN(528);  // Trigger warnings
int LEVEL_OK(200);  // Checks if system is working. Probe shorted ~= 33

unsigned long g_previousMillis = 0;
byte g_wrnState = LOW;

void warn(unsigned int interval, bool buzz)
  {
    // Delay-less timer routine to flash LEDs and beep buzzer
    unsigned long currentMillis = millis();
    if (currentMillis - g_previousMillis > interval)
      {
        if (g_wrnState == LOW)
          g_wrnState = HIGH;
        else
          g_wrnState = LOW;
        digitalWrite(WRN_PIN, g_wrnState);
        if (buzz)
          {
            digitalWrite(BZR_PIN, g_wrnState);
          }
        else
          {
            digitalWrite(BZR_PIN, LOW);
          }
        g_previousMillis = currentMillis;
      }
  } // warn()

void setup()
  {
    Serial.begin(9600);
    lcd.begin();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("OK");
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(BZR_PIN, OUTPUT);
    pinMode(WRN_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);
    // Check for sensible value from probe:
    do
      {
        warn(250, false);
      }
    while (analogRead(PROBE_PIN) < LEVEL_OK);
    digitalWrite(BZR_PIN, LOW);
    digitalWrite(WRN_PIN, LOW);
  } // setup()

void loop()
  {
    int level = analogRead(PROBE_PIN);
    Serial.println(level);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.println(level);
    if (level >= LEVEL_DANGER)
    // Level at danger; shutdown and fast flash warning
      {
        digitalWrite(RELAY_PIN, LOW);
        digitalWrite(BZR_PIN, LOW);
        warn(250, false);
        lcd.println("Danger");
      }

    else if (level >= LEVEL_WARN && level < LEVEL_DANGER)
    // Level low, flash warning and sound buzzer, heater can't be turned
    // back on until water above WARN level
      {
        lcd.println("Low Level");
        warn(1000, true);
      }

    else if (level < LEVEL_WARN && level >= LEVEL_OK)
    // Level OK, turn heater on
      {
        digitalWrite(RELAY_PIN, HIGH);
        digitalWrite(WRN_PIN, LOW);
        digitalWrite(BZR_PIN, LOW);
        lcd.println("Level OK");
      }

    else
    // Something's wrong, shutdown and illuminate LED
      {
        digitalWrite(RELAY_PIN, LOW);
        digitalWrite(WRN_PIN, HIGH);
        digitalWrite(BZR_PIN, LOW);
        lcd.println("Error");
      }
  } // loop()
