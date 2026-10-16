// Include libraries
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

// Replace with your network credentials
const char* ssid = "CBCI-34B4";
const char* password = "class0452anchor";
const char* slack_webhook = "https://hooks.slack.com/services/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";

// Constants
unsigned long previousMillis = 0;
unsigned long interval = 30000;

const unsigned long LEAK_CONFIRM_TIME = 30000;   // 30 s of continuous HIGH
const unsigned long ALERT_REPEAT_TIME = 600000;  // 10 min between alerts
unsigned long highSince = 0;
unsigned long lastAlert = 0;
bool sensorHigh = false;
bool alertSent = false;

// Set pin numbrs
const int waterSensor = 4;

// Variable for storing the watersSensor status
int waterState = 0;

// Wifi connection function
void initWiFi() 
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void setup() 
{
  Serial.begin(115200);

  pinMode(waterSensor, INPUT);

  initWiFi();
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());
}

void loop() 
{ 
   unsigned long currentMillis = millis();

  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval)) 
  {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }

  //Leak sensor
  unsigned long now = millis();

  if (digitalRead(waterSensor) == HIGH)
  {
    if (!sensorHigh)              // just went HIGH: start the 30 s timer
    {
      sensorHigh = true;
      highSince = now;
    }

    if (now - highSince >= LEAK_CONFIRM_TIME && (!alertSent || now - lastAlert >= ALERT_REPEAT_TIME))
    {
      Serial.println("Water leak detected");
      WiFiClientSecure client;
      client.setInsecure();
      HTTPClient https;
      https.begin(client, slack_webhook);
      https.addHeader("Content-Type", "application/json");
      https.POST("{\"text\":\"<!channel> Water leak detected\"}");
      https.end();
      alertSent = true;
      lastAlert = now;
    }
  }
  else                            // any LOW reading resets everything
  {
    sensorHigh = false;
    alertSent = false;
  }
}
