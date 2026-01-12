#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoOTA.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "icons.h"
#include "time.h"
#include "config.h"
#include "webpages.h"
#include "JSON.h"
#include "functions_wifi.h"
#include "functions_server.h"
#include "functions_display.h"
#include "functions_weather.h"



void setup() {
  Serial.begin(115200);
  delay(100);
  pinMode(BACKLIGHT_PIN, OUTPUT);
  digitalWrite(BACKLIGHT_PIN, LOW);

  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return;
  }

  loadSettingJSON();

  LCD.init();
  setBrightness(0);
  LCD.setFreeFont(&FreeSans12pt7b);
  LCD.setRotation(0);
  LCD.fillScreen(TFT_BLACK);
  LCD.setTextColor(TFT_WHITE, TFT_BLACK);
  LCD.setTextDatum(MC_DATUM);
  delay(500);
  LCD.drawString("Connecting to WiFi...", LCD.width() / 2, LCD.height() / 2);

  WiFi.mode(WIFI_AP_STA);

  WiFi.softAPConfig(IPAddress(192, 168, 1, 1) , IPAddress(192, 168, 1, 1) , IPAddress(255, 255, 255, 0));
  WiFi.softAP(ap_ssid, ap_password);
  // Get the IP address of the access point.
  IPAddress apIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(apIP);
  
  for (int i=0;i <=100;i++){
    setBrightness(i);
    delay(33);
  }

  LCD.drawString("Connecting to WiFi...", LCD.width() / 2, LCD.height() / 2);
  WiFi.begin(ssid, password);WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  tempstring = WiFi.localIP().toString();
  LCD.fillScreen(TFT_BLACK);
  LCD.drawString("Connected", LCD.width() / 2, LCD.height() / 2);
  LCD.drawString(tempstring, LCD.width() / 2, LCD.height() / 2 + 50);
  
  for (int i=100;i>=50;i--){
    setBrightness(i);
    delay(33);
  }
  
  LCD.fillScreen(TFT_BLACK);
  LCD.drawString("Fetching data...", LCD.width() / 2, LCD.height() / 2);
  if (weatherProvider == "OpenWeatherMap") {LCD.drawString("OpenWeatherMap", LCD.width() / 2, LCD.height() / 2 + 50);}
  else if (weatherProvider == "Open-Meteo") {LCD.drawString("Open-Meteo", LCD.width() / 2, LCD.height() / 2 + 50);}


  // Start the DNS server.
  dnsServer.start(DNS_PORT, "*", apIP);
  Serial.println("DNS server started.");

  // Set up the web server routes.
  webServer.on("/", handleRoot);

  // This is for handling captive portal detection on different OS.
  webServer.on("/generate_204", handleRoot); // Android
  webServer.on("/gen_204", handleRoot); // Android
  webServer.on("/hotspot-detect.html", handleRoot); // Apple / iOS
  webServer.on("/library/test/success.html", handleRoot); // Apple
  webServer.on("/ncsi.txt", handleRoot); // Microsoft
  webServer.on("/fwlink", handleRoot); // Microsoft

  // Set up the "not found" handler.
  //webServer.onNotFound(handleNotFound);
  
    webServer.onNotFound([]() {
    Serial.println(F("Server not found"));
    Serial.println(webServer.uri());
    handleRoot();
    //if (!captivePortal()) {
    //  handleRoot();
    //} else {handleRoot();}
  });
  
  webServer.begin();


  
  delay(1000);

  while (true){
    timeClient.update();
    delay(1000);
    epochTime = timeClient.getEpochTime();
    if (epochTime > 90000000) {break;}
  }

  getWeatherData_meteo(location_latitude, location_longitude); // Get initial weather data on startup
  timeClient.update();
  lastWeatherUpdate = millis();

  currentHour = timeClient.getHours();
  oldHour = currentHour;

  LCD_refresh(true);

  Serial.println("boot complete");
}

void loop() {
  delay(1);
  dnsServer.processNextRequest();
  webServer.handleClient();
  delay(1);

  
  if (millis() - lastWeatherUpdate > weatherUpdateInterval) {
    if (weatherProvider == "OpenWeatherMap"){getWeatherData_OWM(city, countryCode, apiKey);}
    else if (weatherProvider == "Open_Meteo"){getWeatherData_meteo(location_latitude, location_longitude);}
    lastWeatherUpdate = millis();
  }
  
  if (millis() - ticker_time > 1000) {
    timeClient.update();

    //Serial.println("Pending Operation : " + pending_operation);
    if (pending_operation == "FETCHDATA"){
      pending_operation = "";
      if (weatherProvider == "OpenWeatherMap"){getWeatherData_OWM(city, countryCode, apiKey);}
      else if (weatherProvider == "Open-Meteo"){getWeatherData_meteo(location_latitude, location_longitude);}
      LCD_refresh(true);
      saveSettingsJSON();
      }

    
    ticker_time = millis();
  }

  if (millis() - ticker_LCD > 60000) {
    LCD_refresh(true);
    ticker_LCD = millis();
  }
 
  if (millis() - ticker_checks > 500) {
    //Serial.println(".");
    oldHour = currentHour;
    currentHour = timeClient.getHours();

    //blinkClockLimiter();
    //blinkClock(5);
    //wiggleCoordinate();
    // LCD_refresh(false);
    //getWeatherData_meteo();

    
    
    if (oldHour != currentHour) {blinkClock(5);}
    if (currentHour > 18){setBrightness(0);}
    if (currentHour > 8){setBrightness(50);}

    ticker_checks = millis();
  }


  if (millis() - ticker_wiggle > 3600000) {
    wiggleCoordinate();
    ticker_wiggle = millis();
  }

 
}
