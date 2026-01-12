bool saveSettingsJSON() {
  Serial.println(F("Saving settings"));

  StaticJsonDocument<1024> doc;
  JsonObject json = doc.to<JsonObject>();

  json[F("Internet_SSID")] = ssid;
  json[F("Internet_Password")] = password;
  json[F("AP_SSID")] = ap_ssid;
  json[F("AP_Password")] = ap_password;
  json[F("Weather_Provider")] = weatherProvider;
  json[F("API_Key")] = apiKey;
  json[F("City")] = city;
  json[F("City_OM")] = city_OM;
  json[F("CityID")] = cityID;
  json[F("CountryCode")] = countryCode;
  json[F("Latitude")] = location_latitude;
  json[F("Longitude")] = location_longitude;

  File file = LittleFS.open(F("config.json"), "w");
  if (!file) {
    Serial.println(F("Failed to open file for writing"));
    return false;
  }

  if (serializeJson(json, file) == 0) {
    Serial.println(F("Failed to write to file"));
    file.close();
    return false;
  }

  Serial.println(F("Current settings saved"));
  file.close();
  Serial.println(F("File closed"));
  return true;
}


bool loadSettingJSON() {
  Serial.println(F("Loading config"));

  File file = LittleFS.open(F("config.json"), "r");
  if (!file) {
    Serial.println(F("No config file, using default settings"));
    saveSettingsJSON();
    Serial.println(F("Opening configuration file"));
    file = LittleFS.open(F("config.json"), "r");
    ESP.restart();
  }

  Serial.println(F("Reading configuration file"));
  size_t size = file.size();
  std::unique_ptr<char[]> buf(new char[size]);
  file.readBytes(buf.get(), size);

  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, buf.get());

  if (error) {
    Serial.print(F("Failed to parse JSON: "));
    Serial.println(error.f_str());
    Serial.print(F("Resetting to default settings"));
    file.close();
    saveSettingsJSON();
    return false;
  }

  JsonObject json = doc.as<JsonObject>();

  ssid = json["Internet_SSID"].as<const char*>();
  password = json["Internet_Password"].as<const char*>();
  ap_ssid = json["AP_SSID"].as<const char*>();
  ap_password = json["AP_Password"].as<const char*>();
  weatherProvider = json["Weather_Provider"].as<const char*>();
  apiKey = json["API_Key"].as<const char*>();
  city = json["City"].as<const char*>();
  city_OM = json["City_OM"].as<const char*>();
  cityID = json["CityID"].as<const char*>();
  countryCode = json["CountryCode"].as<const char*>();
  location_latitude = json["Latitude"].as<const char*>();
  location_longitude = json["Longitude"].as<const char*>();

  cityName = city;
  cityName_OM = city_OM;

  Serial.println(ssid);
  Serial.println(password);
  Serial.println(ap_ssid);
  Serial.println(ap_password);
  Serial.println(weatherProvider);
  Serial.println(apiKey);
  Serial.println(city);
  Serial.println(cityID);
  Serial.println(countryCode);
  Serial.println(location_latitude);
  Serial.println(location_longitude);


  Serial.println(F("Config load complete"));
  file.close();
  return true;
}
