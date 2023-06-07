
/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL4dq9h60Th"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "303ELrmD74-D2QYcMHovV24plkhn0oW0"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <DHTesp.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define testingPin D7
int testPinValue;

// Two photoresistors in series, light intensity sensor normalized on relevant values
#define analogPin A0
#define switchingPinLight D6
int lightValue;

// Soil moisture sensor power
#define switchingPinMoisture D5
int moistureValue;

// Temperature & Humidity sensor
DHTesp myDHT;
#define DHT11_PIN D2

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "";
char pass[] = "";

BlynkTimer timer;


// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent() {
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V0, millis() / 1000);
}

// TE for Timer Event
// DE for Delayed Event

void TE_sendLightAndThenMoistureData() {

  digitalWrite(switchingPinLight, HIGH);
  timer.setTimeout(1000L, DE_sendLight);
  Serial.println("Looping");
}

void DE_sendLight() {

  lightValue = analogRead(analogPin);
  lightValue = lightValue / 10;
  Blynk.virtualWrite(V1, lightValue);
  digitalWrite(switchingPinLight, LOW);

  // Then send moisture
  digitalWrite(switchingPinMoisture, HIGH);
  timer.setTimeout(1000L, DE_sendMoisture);
}

void DE_sendMoisture() {

  moistureValue = analogRead(analogPin);
  moistureValue = 1024 - moistureValue;
  // Turn it into a percentage where 150 = 0% and 850 = 100%
  // Serial.println(moistureValue);
  moistureValue = (moistureValue - 150) / 7;
  Blynk.virtualWrite(V2, moistureValue);
  digitalWrite(switchingPinMoisture, LOW);
}

void TE_testPin() {
  testPinValue = digitalRead(testingPin);
  if (testPinValue == HIGH) {
    Serial.print("Button is being held down.");
    Blynk.logEvent("test_event", "Button is being held down.");
  }
}

void TE_sendDHT() {

  float temperature = myDHT.getTemperature();
  float humidity = myDHT.getHumidity();

  Blynk.virtualWrite(V3, temperature);
  Blynk.virtualWrite(V4, humidity);
}

void setup() {
  // Debug console
  Serial.begin(115200);
  // Debug pin
  pinMode(testingPin, INPUT);

  // We will multiplex inputs on the analogPin to read moisture and light intensity
  pinMode(analogPin, INPUT);
  pinMode(switchingPinLight, OUTPUT);
  pinMode(switchingPinMoisture, OUTPUT);


  // Set up the pin for the DHT11 using DHTesp library
  myDHT.setup(DHT11_PIN, DHTesp::DHT11);  // Connect DHT sensor to GPIO 17

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Setup timed functions
  timer.setInterval(1000L, myTimerEvent);
  timer.setInterval(5000L, TE_sendLightAndThenMoistureData);
  timer.setInterval(5000L, TE_sendDHT);
  timer.setInterval(1000L, TE_testPin);
}

void loop() {
  Blynk.run();
  timer.run();
}
