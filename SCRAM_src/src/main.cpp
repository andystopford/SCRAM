#include <Arduino.h>

byte PROBE_PIN(A0);
byte RELAY_PIN(7);
byte BZR_PIN(3);
byte WRN_PIN(6);
// TODO convert the following to ints:
float LEVEL_DANGER(3.0);
float LEVEL_WARN(2.0);
float LEVEL_OK(1.0);

unsigned long previousMillis = 0;
byte ledState = LOW;

void warn(unsigned int interval, bool buzz)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis > interval)
      {
        if (ledState == LOW)
          ledState = HIGH;
        else
          ledState = LOW;
        digitalWrite(WRN_PIN, ledState);
        if (buzz)
          {
            digitalWrite(BZR_PIN, ledState);
          }
        previousMillis = currentMillis;
      }
  } // warn()

void setup()
  {
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(BZR_PIN, OUTPUT);
    pinMode(WRN_PIN, OUTPUT);
    while (analogRead(PROBE_PIN) < LEVEL_OK)
      {
        warn(250, false);
      }
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(BZR_PIN, LOW);
    digitalWrite(WRN_PIN, LOW);
  } // setup()

void loop()
  {
    float level = analogRead(PROBE_PIN);
    if (level >= LEVEL_WARN && level < LEVEL_DANGER)
      {
        warn(500, true);
      }
    if (level >= LEVEL_DANGER)
      {
        digitalWrite(RELAY_PIN, HIGH);
      }
  } // loop()
