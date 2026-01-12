void handleRoot() {
  server_isAlreadyHandled = false;
  String client_uri = webServer.uri();
  Serial.println("Requested URL : " + client_uri);


  // --- START: Argument Logging ---
  int numArgs = webServer.args();
  if (numArgs > 0) {
    Serial.println(F("Arguments received:"));
    for (int i = 0; i < numArgs; i++) {
      Serial.print(F("  - "));
      Serial.print(webServer.argName(i));
      Serial.print(F(": "));
      Serial.println(webServer.arg(i));
    }
  }
  // --- END: Argument Logging ---

  // --- START: Argument Processing ---

  if (webServer.hasArg("Weather_Provider")) {
    if (webServer.arg("Weather_Provider") == "OpenWeatherMap") {Serial.println("Using OpenWeatherMap");weatherProvider = "OpenWeatherMap";}
    else if (webServer.arg("Weather_Provider") == "Open-Meteo") {Serial.println("Using Open-Meteo");weatherProvider = "Open-Meteo";}
    pending_operation = "FETCHDATA";
  }

  if (webServer.hasArg("City")) {
    city = webServer.arg("City");
    cityName = city;
    Serial.println("Got city : " + city);
    pending_operation = "FETCHDATA";
  }

  if (webServer.hasArg("City_OM")) {
    city_OM = webServer.arg("City_OM");
    cityName_OM = city_OM;
    Serial.println("Got City_OM : " + city_OM);
    pending_operation = "FETCHDATA";
  }

  if (webServer.hasArg("CountryCode")) {
    countryCode = webServer.arg("CountryCode");
    Serial.println("Got CountryCode : " + countryCode);
    pending_operation = "FETCHDATA";
  }

  if (webServer.hasArg("API_Key")) {
    apiKey = webServer.arg("API_Key");
    Serial.println("Got API_Key : " + apiKey);
    pending_operation = "FETCHDATA";
  }

  if (webServer.hasArg("Latitude")) {
    location_latitude = webServer.arg("Latitude");
    Serial.println("Got latitude : " + location_latitude);
    pending_operation = "FETCHDATA";
  }

  if (webServer.hasArg("Longitude")) {
    location_longitude = webServer.arg("Longitude");
    Serial.println("Got longitude : " + location_longitude);
    pending_operation = "FETCHDATA";
  }

  if (webServer.hasArg("unit")) {
    // = webServer.arg("");
  }

  if (webServer.hasArg("showTemp")) {
    // = webServer.arg("");
  }

  if (webServer.hasArg("showHumidity")) {
    // = webServer.arg("");
  }

  if (webServer.hasArg("slideDuration")) {
    // = webServer.arg("");
  }

  // --- END: Argument Processing ---

  if (client_uri.endsWith(F(".html")) || client_uri.endsWith(F(".css")) || client_uri.endsWith(F(".js")) || client_uri.endsWith(F(".ttf")) || client_uri.endsWith(F(".png")) || client_uri.endsWith(F(".ico")) || client_uri.endsWith(F(".svg")) || client_uri.endsWith(F(".jpg")) || client_uri.endsWith(F(".woff2")) || client_uri.endsWith(F(".json"))){
  if (!LittleFS.exists(client_uri)) {streamFile(extractFileName(client_uri));server_isAlreadyHandled = true;}
  else {streamFile(client_uri);server_isAlreadyHandled = true;}
  }
  
  if (server_isAlreadyHandled == false) {
    // This now only runs for the root page "/" or other unhandled routes.
    sendProgmemChunk(index_html, sizeof(index_html));
  }
}


// Function to extract file name from path
String extractFileName(String filePath) {
  int lastSlashIndex = filePath.lastIndexOf("/");
  if (lastSlashIndex != -1) {
    return filePath.substring(lastSlashIndex + 1);
  } else {
    // If no slashes found, return the original path
    return filePath;
  }
}

void streamFile(String path) {
    if (!LittleFS.exists(path)) {   
        Serial.println(F("File not found"));
        return;
    }

    File openFile = LittleFS.open(path, "r");
    if (!openFile) {
        Serial.println(F("Failed to open file"));
        return;
    }

    // *** START DEBUGGING BLOCK ***
    String contentType = mime::getContentType(path);
    Serial.println(String(F("Streaming file: ")) + path);
    Serial.println(String(F("Size: ")) + String(openFile.size()));
    Serial.println(String(F("Content-Type: ")) + contentType); // <-- THIS IS THE MOST IMPORTANT LINE
    // *** END DEBUGGING BLOCK ***

    webServer.sendHeader(F("Cache-Control"), F("max-age=600, public"));
    if (webServer.streamFile(openFile, mime::getContentType(path)) != openFile.size()) {
        Serial.println(F("File streaming incomplete"));
    }

    openFile.close();
}

bool isIp(String str) {
  for (size_t i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}

bool captivePortal() {
  String host = webServer.hostHeader();
  String IP_add = WiFi.softAPIP().toString().c_str();
  if (!isIp(host) && host != IP_add) {
    webServer.sendHeader(F("Location"), String(F("http://")) + WiFi.softAPIP().toString(), true);
    webServer.send(302, F("text/plain"), "");
    webServer.client().stop();
    return true;
  }
  return false;
}

void handleLogin() {
  String username = webServer.arg("username");
  String password = webServer.arg("password");

  String response = "<html><body><h1>Login Successful!</h1>";
  response += "<p>Username: " + username + "</p>";
  response += "<p>Password: " + password + "</p>";
  response += "<p>You can now access the internet.</p></body></html>";
  
  webServer.send(200, "text/html", response);
}

void handleNotFound() {
  String host = webServer.hostHeader();
  if (host.indexOf(":") > 0) {
    host = host.substring(0, host.indexOf(":"));
  }

  // Redirect to the captive portal page
  String url = "http://" + host;
  webServer.sendHeader("Location", url, true);
  webServer.send(302, "text/plain", "Redirecting...");
}

// --- OTA Setup ---
void setupOTA() {
  // Set hostname for OTA
  ArduinoOTA.setHostname(ota_hostname);

  // Set password for OTA
  ArduinoOTA.setPassword(ota_password);

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
    // NOTE: if updating SPIFFS, make sure all file handles are closed
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();
}

/**
 * @brief Called when the firmware upload is complete. Restarts the ESP.
 */
void handleUpdateFinished() {
  webServer.sendHeader("Connection", "close");
  // Send "OK" or "FAIL" response to the client
  webServer.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
  // Restart the ESP to apply the update
  ESP.restart();
}

/**
 * @brief Handles the incoming chunks of the firmware file during upload.
 */
void handleFirmwareUpload() {
  HTTPUpload& upload = webServer.upload();
  
  if (upload.status == UPLOAD_FILE_START) {
    Serial.setDebugOutput(true);
    WiFiUDP::stopAll();
    Serial.printf("Update: %s\n", upload.filename.c_str());
    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    if (!Update.begin(maxSketchSpace)) { // Start with max available size
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    Serial.println((char *)upload.buf);
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) { // True to set the size to the current progress
      Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
    } else {
      Update.printError(Serial);
    }
    Serial.setDebugOutput(false);
  }
  yield();
}

void sendChunk(const String& data) {
  if (data.length() == 0) {
    return; // Ignore empty strings
  }
  char buffer[10];
  
  //if (serial == true) {Serial.print(data.length());Serial.print(data);display.clear();display.drawString(0,0,String(data.length()));display.display();}
  
  snprintf(buffer, sizeof(buffer), "%x\r\n", data.length());
  //webServer.sendContent(buffer);
  //webServer.sendContent(data + "\r\n"); Will put line breaks on javascripts
  webServer.sendContent(data); // Fix for javascript unintentional line breaks
}

void sendProgmemChunk(const char* progmemData, size_t length) {
  const size_t chunkSize = 64; // Small chunk to avoid WDT reset
  char buffer[chunkSize + 1];

  size_t offset = 0;
  while (offset < length) {
    size_t toRead = min(chunkSize, length - offset);
    memcpy_P(buffer, progmemData + offset, toRead);
    buffer[toRead] = '\0';  // Ensure null-terminated string

    // Ensure we don't split CSS/HTML syntax mid-property
    size_t safeToRead = toRead;
    while (safeToRead > 0 && buffer[safeToRead - 1] != ';' && buffer[safeToRead - 1] != '}' && buffer[safeToRead - 1] != '\n' && buffer[safeToRead - 1] != ' ') {
      safeToRead--;  // Try to adjust to a safe breakpoint
    }

    // If we didn't find a safe point, just send the whole chunk to avoid infinite loops
    if (safeToRead == 0) safeToRead = toRead;

    buffer[safeToRead] = '\0';  // Ensure termination at the safe point
    sendChunk(String(buffer)); // Send valid chunk
    offset += safeToRead;

    yield(); // Prevent WDT reset by allowing system tasks to run
  }
}
