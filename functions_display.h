void LCD_refresh(bool clearLCD){
  LCD.setTextColor(TFT_WHITE, TFT_BLACK);
  // --- Draw to Display ---

  if (clearLCD) {LCD.fillScreen(TFT_BLACK);}
  
  
  // Draw City Name
  LCD.setTextDatum(TC_DATUM); // Top-Center
  LCD.setFreeFont(&FreeSans9pt7b);

  if (weatherProvider == "OpenWeatherMap") {LCD.drawString(cityName, LCD.width() / 2 + wiggle_x, 5 + wiggle_y);}
  else if (weatherProvider == "Open-Meteo") {LCD.drawString(cityName_OM, LCD.width() / 2 + wiggle_x, 5 + wiggle_y);}

  // Draw Weather Description
  LCD.drawString(weatherDescription, LCD.width() / 2 + wiggle_x, 35 + wiggle_y);

    // Draw a dividing line
    LCD.drawFastHLine(20, 60 + wiggle_y, 200, TFT_WHITE);

   // Draw Time in the middle
   LCD.setTextColor(TFT_YELLOW, TFT_BLACK);
   LCD.setFreeFont(&FreeSansBold24pt7b);
   formattedTime = timeClient.getFormattedTime();
   
   LCD.drawString(formattedTime.substring(0, 5), LCD.width() / 4 - 5 + wiggle_x, 80 + wiggle_x);
  
   LCD.setTextColor(TFT_WHITE, TFT_BLACK);


   
   // Draw Date at left side
   time_t epochTime = timeClient.getEpochTime();
   struct tm* ptm = gmtime(&epochTime);
   char dateBuffer[32];
   strftime(dateBuffer, 32, "%A, %d %B", ptm);
   LCD.setFreeFont(&FreeSans12pt7b);


   // Draw weather icon at right side
   drawWeatherIcon_meteo(weatherCode);
   

   
   //LCD.drawString(String(ptm->tm_mday), 180, 80);
   //LCD.drawString(String(ptm->tm_mon), 180, 110);
   //LCD.drawString(String(ptm->tm_year+1900), 180, 140);
   tempstring = String(ptm->tm_mday) + "/" + String(ptm->tm_mon) + "/" + String(ptm->tm_year+1900);
   LCD.drawString(tempstring, LCD.width() / 4 + wiggle_x, 140 + wiggle_y);
  
  // Draw a center dividing line
  LCD.drawFastVLine(120, 60 + wiggle_y, 110, TFT_WHITE);
  
  // Draw a dividing line
  LCD.drawFastHLine(20 + wiggle_x, 170, 200, TFT_WHITE);

  // --- Draw bottom row of data ---
  LCD.setFreeFont(&FreeSans9pt7b);
  
  // Temperature
  LCD.setTextDatum(BC_DATUM); // Bottom-Center for labels
  LCD.drawString("Temp.", 45 + wiggle_x, 200 + wiggle_y);
  LCD.setTextDatum(TC_DATUM); // Top-Center for values
  
  if (temperature.toInt() > 35){LCD.setTextColor(TFT_YELLOW, TFT_BLACK);}
  else {LCD.setTextColor(TFT_GREEN, TFT_BLACK);}
  LCD.drawString(temperature, 45 + wiggle_x, 210 + wiggle_y);
  LCD.setTextColor(TFT_WHITE, TFT_BLACK);

  // Humidity
  LCD.setTextDatum(BC_DATUM);
  LCD.drawString("Lembab", 120 + wiggle_x, 200 + wiggle_y);
  LCD.setTextDatum(TC_DATUM);
  
  if (humidity.toInt() >= 70){LCD.setTextColor(TFT_RED, TFT_BLACK);}
  else if (humidity.toInt() >= 60){LCD.setTextColor(TFT_YELLOW, TFT_BLACK);}
  else {LCD.setTextColor(TFT_GREEN, TFT_BLACK);}
  LCD.drawString(humidity, 120 + wiggle_x, 210 + wiggle_y);
  LCD.setTextColor(TFT_WHITE, TFT_BLACK);

  // Wind Speed
  LCD.setTextDatum(BC_DATUM);
  LCD.drawString("Angin", 195 + wiggle_x, 200 + wiggle_y);
  LCD.setTextDatum(TC_DATUM);
  LCD.drawString(windSpeed, 195 + wiggle_x, 210 + wiggle_y);


}

// Function to set the backlight brightness (0-100%)
void setBrightness(int percentage) {
  // Clamp the percentage value between 0 and 100
  if (percentage < 0) percentage = 0;
  if (percentage > 100) percentage = 100;

  // Map the 0-100% range to the 1023-0 PWM range (inverted)
  int dutyCycle = map(percentage, 0, 100, 1023, 0);
  
  // Set the PWM duty cycle on the backlight pin
  analogWrite(TFT_BL, dutyCycle);
}

void blinkClock(int repeatCount){
   LCD_refresh(false);

   for (int i=1;i <= repeatCount;i++){
      LCD.fillRect(0, 80, 115, 60, TFT_BLACK);
      delay(500);
      LCD.setTextColor(TFT_YELLOW, TFT_BLACK);
      LCD.setFreeFont(&FreeSansBold24pt7b);
      formattedTime = timeClient.getFormattedTime();
      LCD.drawString(formattedTime.substring(0, 5), LCD.width() / 4 - 5, 80);
      delay(500);
   }
}

void blinkClockLimiter(){
      LCD.fillRect(50, 80, 10, 50, TFT_BLACK);
      delay(500);
      LCD.setTextColor(TFT_YELLOW, TFT_BLACK);
      LCD.setFreeFont(&FreeSansBold24pt7b);
      formattedTime = timeClient.getFormattedTime();
      LCD.drawString(formattedTime.substring(0, 5), LCD.width() / 4 - 5, 80);
      //delay(500);
}

void addAnimationCycle(){
  animationCycle += 1;
  if (animationCycle >20){animationCycle = 1;}
}

void wiggleCoordinate(){
  wiggle_x = random(0,5); 
  wiggle_y = random(0,5);
}
