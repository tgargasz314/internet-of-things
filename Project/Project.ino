#include <ArduinoJson.h>

#include <DS3231.h>
DS3231 rtc;

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#include <HTTPClient.h>

#include <WiFi.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* ssid = "";
const char* password = "";

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

String serverName = "http://api.openweathermap.org/data/2.5/weather?zip=44243&appid=";

void setup()
{
  Serial.begin(9600);

  Wire.begin();

  rtc.setHour(3);
  
  lcd.begin();

  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connecting...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(100);
  }
  Serial.println(WiFi.localIP());
  
  //lcd.print("Cool");
}

void loop()
{
  if((millis() - lastTime) > timerDelay) {
    //Serial.print(rtc.getSecond());
    if(WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverName.c_str());
      int httpResponseCode = http.GET();
      if(httpResponseCode > 0) {
        //Serial.print("HTTP Response code: ");
        //Serial.println(httpResponseCode);
        String payload = http.getString();
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, payload.c_str());
        float temp = doc["main"]["temp"];
        temp = ((temp-273.15) * (9.0 / 5.0)) + 32.0;
        const char * city = doc["name"];
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(city);
        lcd.setCursor(0,1);
        lcd.print(temp);
        int x = 7;
        bool ignore = true;
        lcd.setCursor(x,0);
        lcd.print(rtc.getHour(ignore,ignore));
        lcd.setCursor(x+2,0);
        lcd.print(":");
        lcd.setCursor(x+3,0);
        lcd.print(rtc.getMinute());
        lcd.setCursor(x+5,0);
        lcd.print(":");
        lcd.setCursor(x+6,0);
        lcd.print(rtc.getSecond());
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}
