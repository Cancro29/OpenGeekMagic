# OpenGeekMagic
A Weather Dashboard project based on GeekMagic SmallTV running on ESP8266

## Features:
- Open-Meteo and OpenWeatherMap weather provider
- Customizable icons
- Dashboard Manager
- Uses Tailwind CSS. Any changes related to Dashboard HTML must be recompiled using Tailwind
- Custom image viewer

## How to use:
- Go to toolkit folder, extract TFT_eSPI.rar into your Arduino library folder. Skip if you have figured the LCD pins.
- Open the .ino file
- Tools > ESP8266 LittleFS Data Upload
- Go to config.h, set your WiFi Credentials
- Flash it
- Note: For some reason it will repeatedly crash before finally starting.

## Note:
### This code is incomplete, some features may be missing or buggy.
