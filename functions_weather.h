// Function to fetch and parse weather data

void setLocation_meteo(String latitude, String longitude){
  apiKey_meteo.replace("%LATITUDE%",latitude);
  apiKey_meteo.replace("%LONGITUDE%",longitude);
}
void getWeatherData_OWM(String loc_city, String loc_CountryCode, String apiKey) {
  String url = "http://api.openweathermap.org/data/2.5/weather?q=" + loc_city + "," + loc_CountryCode + "&appid=" + apiKey + "&units=metric&lang=id";
  //String url = "http://api.openweathermap.org/data/2.5/weather?id=" + cityID + "&appid=" + apiKey + "&units=metric&lang=id";
  Serial.println(url);
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(url);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String payload = http.getString();
      
      DynamicJsonDocument doc(2048); // Increased size for more data
      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
      }

      // Extract all weather data
      cityName = doc["name"].as<String>();
      weather = doc["weather"][0]["main"].as<String>();
      weatherDescription = doc["weather"][0]["description"].as<String>();
      weatherDescription.toUpperCase(); // Capitalize description
      
      temperature = String(doc["main"]["temp"].as<float>(), 1);
      feelsLike = String(doc["main"]["feels_like"].as<float>(), 1) + " C";
      humidity = String(doc["main"]["humidity"].as<int>()) + "%";
      windSpeed = String(doc["wind"]["speed"].as<float>(), 1) + " m/s";

      Serial.println("Weather Updated: " + cityName + ", " + temperature + "C");

    } else {
      Serial.print("Error on HTTP request: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
}

void getWeatherData_meteo(String loc_lat, String loc_long) {
  
  String url = "http://api.open-meteo.com/v1/forecast?latitude=" + loc_lat + "&longitude=" + loc_long  + "&current=temperature_2m,relative_humidity_2m,apparent_temperature,is_day,wind_speed_10m,wind_direction_10m,wind_gusts_10m,precipitation,rain,showers,snowfall,weather_code,cloud_cover,pressure_msl,surface_pressure&timezone=auto";


  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(url);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String payload = http.getString();
      
      DynamicJsonDocument doc(2048);
      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
      }

      // Parse weather data from "current"
      temperature = String(doc["current"]["temperature_2m"].as<float>());
      feelsLike = String(doc["current"]["apparent_temperature"].as<float>());
      humidity  = String(doc["current"]["relative_humidity_2m"].as<int>());
      windSpeed = String(doc["current"]["wind_speed_10m"].as<float>());
      //int windDir = doc["current"]["wind_direction_10m"].as<int>();
      cloudCoverage = String(doc["current"]["cloud_cover"].as<int>());
      airPressure = String(doc["current"]["pressure_msl"].as<float>());
      weatherCode = doc["current"]["weather_code"].as<int>();

      
      // Assign to string variables
      temperature = String(temperature) + " C";
      feelsLike = String(feelsLike) + " C";
      humidity = String(humidity) + "%";
      windSpeed = String(windSpeed) + " km/h";
      //omWindDirection = String(windDir) + "°";
      cloudCoverage = String(cloudCoverage) + "%";
      airPressure = String(airPressure) + " hPa";

      // Map code to description
      weather = mapWeatherCode_id(weatherCode);
      weatherDescription = weather;
      weatherDescription.toUpperCase();

      // Debug print
      Serial.println("Weather Updated:");
      Serial.println("City: " + cityName);
      Serial.println("Temperature: " + temperature);
      Serial.println("Feels Like: " + feelsLike);
      Serial.println("Humidity: " + humidity);
      Serial.println("Wind: " + windSpeed);
      Serial.println("Cloud Cover: " + cloudCoverage);
      Serial.println("Pressure: " + airPressure);
      Serial.println("Weather: " + weather);
      
    } else {
      Serial.print("Error on HTTP request: ");
      Serial.println(httpResponseCode);
    }
    http.end();
    
  }
  
}

// Map Open-Meteo WMO weather codes to descriptions
String mapWeatherCode_en(int code) {
  switch (code) {
    case 0: return "Clear sky";
    case 1: return "Mainly clear";
    case 2: return "Partly cloudy";
    case 3: return "Overcast";
    case 45: return "Fog";
    case 48: return "Depositing rime fog";
    case 51: return "Light drizzle";
    case 53: return "Moderate drizzle";
    case 55: return "Dense drizzle";
    case 61: return "Slight rain";
    case 63: return "Moderate rain";
    case 65: return "Heavy rain";
    case 71: return "Slight snow fall";
    case 73: return "Moderate snow fall";
    case 75: return "Heavy snow fall";
    case 80: return "Rain showers";
    case 81: return "Heavy rain showers";
    case 82: return "Violent rain showers";
    case 95: return "Thunderstorm";
    case 99: return "Hailstorm";
    default: return "Unknown";
  }
}

String mapWeatherCode_id(int code) {
  switch (code) {
    case 0: return "Cerah";
    case 1: return "Cerah berawan";
    case 2: return "Berawan sebagian";
    case 3: return "Berawan";
    case 45: return "Kabut";
    case 48: return "Kabut rime";
    case 51: return "Gerimis ringan";
    case 53: return "Gerimis sedang";
    case 55: return "Gerimis lebat";
    case 61: return "Hujan ringan";
    case 63: return "Hujan sedang";
    case 65: return "Hujan lebat";
    case 71: return "Hujan salju ringan";
    case 73: return "Hujan salju sedang";
    case 75: return "Hujan salju lebat";
    case 80: return "Hujan deras";
    case 81: return "Hujan deras lebat";
    case 82: return "Hujan deras disertai badai";
    case 95: return "Badai petir";
    case 99: return "Badai es";
    default: return "Tidak diketahui";
  }
}

void drawWeatherIcon_OWM(String weatherName){
  if (weatherName == "Clouds"){LCD.drawXBitmap(130, 65, icon_clouds, icon_clouds_width, icon_clouds_height, TFT_BLACK, TFT_DARKGREY);}
  else if (weatherName == "Clear") {LCD.drawXBitmap(130, 65, icon_sunny, icon_sunny_width, icon_sunny_height, TFT_BLACK, TFT_WHITE);}
  else if (weatherName == "Atmosphere") {}
  else if (weatherName == "Snow") {}
  else if (weatherName == "Rain") {LCD.drawXBitmap(130, 65, icon_rain, icon_rain_width, icon_rain_height, TFT_BLACK, TFT_WHITE);}
  else if (weatherName == "Drizzle") {}
  else if (weatherName == "Thunderstorm") {LCD.drawXBitmap(130, 65, icon_thunderstorm, icon_thunderstorm_width, icon_thunderstorm_height, TFT_BLACK, TFT_WHITE);}
  else {LCD.drawString(weatherName, 180, 120);}
}

void drawWeatherIcon_meteo(int code){
  Serial.println("Weather Code: " + code);
  // SUNNY
  if (code == 2){LCD.drawXBitmap(130, 65, icon_clouds, icon_clouds_width, icon_clouds_height, TFT_BLACK, TFT_DARKGREY);}
  else if (code == 3) {LCD.drawString(mapWeatherCode_id(code), 180, 120);}
  // CLOUDY
  else if (code == 45) {LCD.drawString(mapWeatherCode_id(code), 180, 120);}
  // DRIZZLE
  else if (code == 51) {LCD.drawString(mapWeatherCode_id(code), 180, 120);}
  else if (code == 53) {LCD.drawString(mapWeatherCode_id(code), 180, 120);}
  else if (code == 55) {LCD.drawString(mapWeatherCode_id(code), 180, 120);}
  // RAIN
  else if (code == 61) {LCD.drawString(mapWeatherCode_id(code), 180, 120);}
  else if (code == 63) {LCD.drawString(mapWeatherCode_id(code), 180, 120);}
  else if (code == 65) {LCD.drawString(mapWeatherCode_id(code), 180, 120);}
  // RAIN SNOW
  else if (code == 71) {LCD.drawString(mapWeatherCode_id(code), 180, 120);}
  else if (code == 73) {LCD.drawString(mapWeatherCode_id(code), 180, 120);}
  else if (code == 75) {LCD.drawString(mapWeatherCode_id(code), 180, 120);}
  // HEAVY RAIN
  else if (code == 80) {LCD.drawString(mapWeatherCode_id(code), 180, 120);}
  else if (code == 81) {LCD.drawString(mapWeatherCode_id(code), 180, 120);}
  else if (code == 82) {LCD.drawString(mapWeatherCode_id(code), 180, 120);}
  // THUNDERSTORM
  else if (code == 95) {LCD.drawString(mapWeatherCode_id(code), 180, 120);}
  else if (code == 99) {LCD.drawString(mapWeatherCode_id(code), 180, 120);}
  else {LCD.drawString(mapWeatherCode_id(code), 180, 120);}
}
