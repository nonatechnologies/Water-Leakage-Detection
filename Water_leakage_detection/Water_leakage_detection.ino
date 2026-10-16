// Include libraries
#include <Arduino.h>
#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "CBCI-34B4";
const char* password = "class0452anchor";
const char* slack_webhook = "https://hooks.slack.com/services/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"

// Constants
unsigned long previousMillis = 0;
unsigned long interval = 30000;
unsigned long loopHighMillis = 0;

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

  pinMode(waterSensor, INPUT)

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
  waterState = digitalRead(waterSensor);
  Serial.println(waterState);

  unsigned long loopStartMillis = millis(); 
  while (waterState == HIGH && loopHighMillis - loopStartMillis < interval)
    {
      loopHighMillis = millis();
    }
  if (waterState == HIGH && loopHighMillis - loopStartMillis = interval)
  {
    println("Water leak detected");
    //sendSlack("<!channel> Water leak detected");
  }
}
