// --- User Configuration ---
// WiFi Credentials
String ssid     = "Anjames1";
String password = "samziko12345";
String pending_operation = "";

// --- Captive Portal Configuration ---
// Set the access point (AP) name and password.
// Leave the password empty for an open network.
String ap_ssid = "WeatherStation2";
String ap_password = "123456789";

const byte DNS_PORT = 53;
DNSServer dnsServer;
ESP8266WebServer webServer(80);
IPAddress apIP(192, 168, 1, 1);
bool server_isAlreadyHandled = false;
// --- OTA Configuration ---
const char* ota_hostname = "12345678";
const char* ota_password = "12345678"; // Set a password for secure updates

String weatherProvider = "Open-Meteo";

// OpenWeatherMap Configuration
String apiKey = "71d38d66e3114f2990a6a42d9ecab22a"; // <-- PASTE YOUR API KEY HERE
String city = "Jakarta";
String cityID = "1642911";
String countryCode = "ID";

// open-meteo Configuration
String apiKey_meteo = "https://api.open-meteo.com/v1/forecast?latitude=%LATITUDE%&longitude=%LONGITUDE%&current_weather=true";
String location_latitude = "-6.130304";
String location_longitude = "106.899268";
String city_OM = "";

// Timezone Configuration
const long utcOffsetInSeconds = 25200;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
time_t epochTime = timeClient.getEpochTime();
struct tm *ptm = gmtime ((time_t*)&epochTime);

// --- Pin Configuration ---
#define BACKLIGHT_PIN 5

// --- Global Objects & Variables ---
TFT_eSPI LCD = TFT_eSPI();

// Firmware update constants
const String fwver_magicString = "^~1.0.0~^";
const String fwname_magicString = "Nimbus";

// Variables to store weather data
String cityName = "Jakarta";
String cityName_OM = "--";
String weather = "--";
String weatherDescription = "--";
String temperature = "--";
String feelsLike = "--";
String humidity = "--";
String windSpeed = "--";
String cloudCoverage = "--";
String airPressure = "--";
String PM25_airQualityIndex = "--";
int weatherCode = -1;




String tempstring = "";

String formattedTime = "";

int oldHour = 0;
int currentHour = 0;
int animationCycle = 1;
int wiggle_x = 0;
int wiggle_y = 0;

unsigned long lastWeatherUpdate = 0;
unsigned long ticker_checks = 0;
unsigned long ticker_LCD = 0;
unsigned long ticker_time = 0;
unsigned long ticker_wiggle = 0;
// Update weather every 15 minutes
const unsigned long weatherUpdateInterval = 300000; 


void drawWeatherIcon_OWM(String weatherName);
void drawWeatherIcon_meteo(int code);
String mapWeatherCode_en(int code);
String mapWeatherCode_id(int code);

void sendProgmemChunk(const char* progmemData, size_t length);
String extractFileName(String filePath);
void streamFile(String path);
