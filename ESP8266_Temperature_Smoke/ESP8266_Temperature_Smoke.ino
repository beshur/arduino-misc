/**
  ESP8266 (Wemos D1) Temperature and Smoke Sensor with Blynk

  This sketch gets input from external smoke sensor
  and OneWire-type temperature sensor.

  Smoke sensor is checked every second, and temperature checked every minute.

  If smoke detected a related Blynk event is raised, and temperature then
  checked every second.

  Initial purpose:
  Tracking situation near 3D-printer that is in remote location.

  To do:
  Trigger smart socket shutdown command.  
 */
#define BLYNK_TEMPLATE_ID "TMP"
#define BLYNK_DEVICE_NAME "Garage Temperature and Smoke Sensor ESP8266"
#define BLYNK_AUTH_TOKEN ""


#define BLYNK_PRINT Serial
#define APP_DEBUG
#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define SMOKE_PIN D2
#define TEMP_PIN D1

char ssid[] = "";
char pass[] = "";

long current = 0L;
long lastDhtCheck = 0L;
long lastSmokeCheck = 0L;

int CHECK_INTERVAL = 60*60*1000;

BlynkTimer myTimer;

OneWire oneWire(TEMP_PIN);
DallasTemperature sensors(&oneWire);
float Celcius = 0;

int ledVal = LOW;

void tempEvent()
{
  sensors.requestTemperatures(); 
  Celcius = sensors.getTempCByIndex(0);
  Blynk.virtualWrite(V1, Celcius);
  Serial.println("Temperature " + (String) Celcius);

  Blynk.logEvent("temperature", Celcius);
  if (Celcius > 60) {
    Blynk.logEvent("temperature_rising", "3D printer Temp " + (String)Celcius);
  }
}

void smokeEvent() {
  digitalWrite(LED_BUILTIN, ledVal);
  ledVal = !ledVal;

  int smokePresent = digitalRead(SMOKE_PIN);
  Serial.println("Smoke detector " + (String) smokePresent);
  if (smokePresent == LOW) {
    tempEvent();
    Blynk.logEvent("smoke", "SMOKE");
    Serial.println("SMOKE detected");
  }
  Blynk.virtualWrite(V3, smokePresent);
}


void setup()
{
  pinMode(SMOKE_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(SMOKE_PIN, LOW);
  // Debug console
  Serial.begin(115200);
  Serial.println("START");
  sensors.begin();

  myTimer.setInterval(1*1000L, smokeEvent);
  myTimer.setInterval(60*1000L, tempEvent);
  
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);  
}

void loop()
{
  Blynk.run();
  myTimer.run();
}
