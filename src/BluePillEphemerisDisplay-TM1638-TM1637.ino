/* Ephemeris module to display Lat, Lon, Time, Date from json object
   sent by raspberry pi.
   Button control selects planet of choice and display rise and set time
   This V7 adds a toggle switch to show time in UTC or Local time.

   Implemented on Blue Pill

  */

#include <HardwareSerial.h>
#include <ArduinoJson.h>
#include <TM1637.h>
#include <TM1638.h>
#include <TM16xxDisplay.h>

// Initialize Blue Pill UART2 for receiving Pi data
HardwareSerial Serial2(PA3, PA2);

// Allocate the JSON document
JsonDocument doc;

#define RISE_DIO PB13
#define RISE_CLK PB12
#define SET_DIO PA11
#define SET_CLK PA12

TM1637 rise_module(RISE_DIO, RISE_CLK);  // DIO, CLK
TM1637 set_module(SET_DIO, SET_CLK);     // DIO, CLK
TM16xxDisplay display_rise(&rise_module, 4);
TM16xxDisplay display_set(&set_module, 4);

const byte STBPin1 = PB3;  // STB - TM1638 Module 1
const byte STBPin2 = PB4;  // STB - TM1638 Module 2
const byte STBPin3 = PB5;  // STB - TM1638 Module 3
const byte STBPin4 = PB6;  // STB - TM1638 Module 4

const byte TM1638Clk = PB7;   // 15 CLK on TM1638
const byte TM1638Data = PB8;  // 14 Data on TM1638

// data, clk
// on the TM1638, we share the data and clk line
TM1638 module1(TM1638Data, TM1638Clk, STBPin1);  // 1st TM1638 Time
TM1638 module2(TM1638Data, TM1638Clk, STBPin2);  // 2nd TM1638 Date
TM1638 module3(TM1638Data, TM1638Clk, STBPin3);  // 3rd TM1638 Latitude
TM1638 module4(TM1638Data, TM1638Clk, STBPin4);  // 4TH TM1638 Longitude

TM16xxDisplay display1(&module1, 8);  // TM16xx object, 8 digits
TM16xxDisplay display2(&module2, 8);  // TM16xx object, 8 digits
TM16xxDisplay display3(&module3, 8);  // TM16xx object, 8 digits
TM16xxDisplay display4(&module4, 8);  // TM16xx object, 8 digits

int SunRise;
int SunSet;
int MercuryRise;
int MercurySet;
int VenusRise;
int VenusSet;
int MoonRise;
int MoonSet;
int MarsRise;
int MarsSet;
int JupiterRise;
int JupiterSet;
int SaturnRise;
int SaturnSet;

int LED_DISPLAY_BRIGHTNESS = 1;  // 0 dim to 15 bright

#define LED_SUN PA5      // sun white 4.7k
#define LED_MERCURY PA6  // mercury yellow 2.7k
#define LED_VENUS PA7    // venus pink 5.6k
#define LED_MOON PB0     // moon warm white 15k
#define LED_MARS PB1     // mars red 10k
#define LED_JUPITER PA0  // jupiter purple 15
#define LED_SATURN PA1   // saturn blue 33k
#define BUTTON_PIN PB14
#define BUTTON_LED PB15  // yellow 100

int current_position = 0;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 200;
bool SunActive = false;
bool MercuryActive = false;
bool VenusActive = false;
bool MoonActive = false;
bool MarsActive = false;
bool JupiterActive = false;
bool SaturnActive = false;

// switch from Local time to UTC time display

#define MODE_SWITCH_PIN PA0
bool useUTC = false;

void setup() {
  pinMode(MODE_SWITCH_PIN, INPUT_PULLUP);  // Active LOW

   // Start the default serial console
  Serial.begin(115200);

  // Start UART2 at the same baud rate as Pi
  Serial2.begin(115200);

  pinMode(LED_SUN, OUTPUT);
  pinMode(LED_MERCURY, OUTPUT);
  pinMode(LED_VENUS, OUTPUT);
  pinMode(LED_MOON, OUTPUT);
  pinMode(LED_MARS, OUTPUT);
  pinMode(LED_JUPITER, OUTPUT);
  pinMode(LED_SATURN, OUTPUT);
  pinMode(BUTTON_LED, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  display1.setIntensity(LED_DISPLAY_BRIGHTNESS);
  display2.setIntensity(LED_DISPLAY_BRIGHTNESS);
  display3.setIntensity(LED_DISPLAY_BRIGHTNESS);
  display4.setIntensity(LED_DISPLAY_BRIGHTNESS);

  display1.println(F("TIME1234"));  // Time
  display2.println(F("DATE1234"));  // Date
  display3.println(F("LATITUDE"));  // Latitude
  display4.println(F("LONGITUD"));  // Longitude

  display_rise.println("RISE");
  display_set.println("SET ");

  Serial.println("STM32 Blue Pill ready to receive data...");
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW && (millis() - lastDebounceTime) > debounceDelay) {
    lastDebounceTime = millis();
    allLEDoff();
    Serial.print("button state ");
    Serial.print(buttonState);
    Serial.print(" current_position ");
    Serial.println(current_position);

    switch (current_position) {
      case 0:
        allLEDoff();
        digitalWrite(LED_SUN, HIGH);
        digitalWrite(BUTTON_LED, HIGH);
        delay(100);
        digitalWrite(BUTTON_LED, LOW);
        allActiveFalse();
        SunActive = true;
        current_position++;
        break;
      case 1:
        allLEDoff();
        digitalWrite(LED_MERCURY, HIGH);
        digitalWrite(BUTTON_LED, HIGH);
        delay(100);
        digitalWrite(BUTTON_LED, LOW);
        allActiveFalse();
        MercuryActive = true;
        current_position++;
        break;
      case 2:
        allLEDoff();
        digitalWrite(LED_VENUS, HIGH);
        digitalWrite(BUTTON_LED, HIGH);
        delay(100);
        digitalWrite(BUTTON_LED, LOW);
        allActiveFalse();
        VenusActive = true;
        current_position++;
        break;
      case 3:
        allLEDoff();
        digitalWrite(LED_MOON, HIGH);
        digitalWrite(BUTTON_LED, HIGH);
        delay(100);
        digitalWrite(BUTTON_LED, LOW);
        allActiveFalse();
        MoonActive = true;
        current_position++;
        break;
      case 4:
        allLEDoff();
        digitalWrite(LED_MARS, HIGH);
        digitalWrite(BUTTON_LED, HIGH);
        delay(100);
        digitalWrite(BUTTON_LED, LOW);
        allActiveFalse();
        MarsActive = true;
        current_position++;
        break;
      case 5:
        allLEDoff();
        digitalWrite(LED_JUPITER, HIGH);
        digitalWrite(BUTTON_LED, HIGH);
        delay(100);
        digitalWrite(BUTTON_LED, LOW);
        allActiveFalse();
        JupiterActive = true;
        current_position++;
        break;
      case 6:
        allLEDoff();
        digitalWrite(LED_SATURN, HIGH);
        digitalWrite(BUTTON_LED, HIGH);
        delay(100);
        digitalWrite(BUTTON_LED, LOW);
        allActiveFalse();
        SaturnActive = true;
        current_position = 0;  // start over
        break;
      default:
        Serial.println("Default position");
        allLEDoff();
        allActiveFalse();
        current_position = 0;
        break;
    };
  }

  // Check if data is available on UART2
  if (Serial2.available()) {
    // Read data from UART2
    String json_str = Serial2.readStringUntil('\n');
    Serial.print("Received string: ");  // Debug print
    Serial.println(json_str);           // Debug print
    const char* json = json_str.c_str();

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, json);

    // Test if parsing succeeds
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
///////////////////////////
// 🔌 Read SPST switch
bool useUTC = digitalRead(MODE_SWITCH_PIN) == HIGH;

// Fetch time and date from JSON
const char* date    = doc["date"];           // "2025-01-16"
const char* UTCtime = doc["time"];           // "18:22:27.000Z"

char cleanTime[9];     // HH:MM:SS
strncpy(cleanTime, UTCtime, 8);
cleanTime[8] = '\0';

bool utcIsNextDay = false;
char utcDate[11];      // YYYY-MM-DD
char localDate[11];    // YYYY-MM-DD
char localTime[9];     // HH-MM-SS

// Copy UTC date into buffer
strncpy(utcDate, date, 10);
utcDate[10] = '\0';

// Convert time and detect rollover
convertUTCToLocal(cleanTime, localTime, &utcIsNextDay);

// Adjust local date if rollover occurred
if (utcIsNextDay) {
  decrementDate(utcDate, localDate);
} else {
  strncpy(localDate, utcDate, 11);
}

// Debug print
Serial.print(F("Local Time: "));
Serial.println(localTime);
Serial.print(F("UTC Time: "));
Serial.println(cleanTime);
Serial.print(F("Local Date: "));
Serial.println(localDate);
Serial.print(F("UTC Date: "));
Serial.println(utcDate);


    // 🎛️ Read TM1638 buttons
    uint8_t buttons = tm1638.readButtons();
    for (int i = 0; i < 7; i++) {
      if (buttons & (1 << i)) {
        activePlanetIndex = i;
        break;
      }
    }

    // 🪐 Get planet data
    const PlanetDisplay& planet = planets[activePlanetIndex];
    const char* riseStr = useUTC ? doc[planet.jsonKey + String("Rise")] : doc[planet.jsonKey + String("Rise")];
    const char* setStr  = useUTC ? doc[planet.jsonKey + String("Set")]  : doc[planet.jsonKey + String("Set")];

    // 🖥️ Display planet rise/set
    displayPlanet(planet.name, riseStr, setStr, useUTC, utcIsNextDay);
/////////////////////////
    // Fetch the values from the JSON structure

    //const char* LocalTime = doc["LocalTime"];  // "2025/1/16 13:22:30"
    const char* date = doc["date"];            // "2025-01-16"
    const char* UTCtime = doc["time"];         // "18:22:27.000Z"

    char LocalTime[9];  // HH-MM-SS + null
    convertUTCToLocal(UTCtime, LocalTime);  // invoke the conversion routine for local time
    Serial.println(LocalTime);  // Output: 09-30-45

    const char* latitudeString = doc["latitude"];
    float latitude = convertDMSToDecimal(latitudeString);
    Serial.print("Latitude in deg:mm:ss ");
    Serial.println(latitudeString);
    Serial.print("Latitude in decimal degrees: ");
    Serial.println(latitude, 6);  // print the result with 6 decimal places

    //char* displayLatitude;
    //sscanf(displayLatitude, "%f2.4", &latitude);
    Serial.print(F("displayLatitude: "));
    Serial.println(latitudeString);
    display3.println(latitudeString);

    const char* longitudeString = doc["longitude"];  // "-73:30:15.5"
    Serial.print("Longitude in deg:mm:ss: ");
    Serial.println(longitudeString);
    float longitude = convertDMSToDecimal(longitudeString);
    Serial.print("Longitude in decimal degrees: ");
    Serial.println(longitude, 6);
    float altitude = doc["altitude"];  // 63.64399999999999
    Serial.print("Localtime ");
    Serial.println(LocalTime);
    Serial.print("date ");
    Serial.println(date);
    Serial.print("UTCtime ");
    Serial.println(UTCtime);


    display4.println(F(longitudeString));

    char LocalDisplayDate[9];
    int year, month, day;
    // Parse the input string
    sscanf(date, "%d-%d-%d", &year, &month, &day);
    // Format into the new string
    sprintf(LocalDisplayDate, "%02d-%02d-%02d", month, day, year % 100);

    display1.println(LocalDisplayDate);
    display2.println(LocalTime);
    //display2.println(LocalDisplayTime);

    Serial.print("altitude ");
    Serial.println(altitude);


    const char* SunRiseString = doc["SunRise"];  // "2025/1/17 07:15:33"
    char SunRise[6];
    convertToTimeValue(SunRiseString, SunRise);
    Serial.print("SunRise (HHMM): ");
    Serial.println(SunRise);  // prints the result
    const char* SunSetString = doc["SunSet"];
    char SunSet[6];
    convertToTimeValue(SunSetString, SunSet);

    Serial.print("SunSet (HHMM): ");
    Serial.println(SunSet);  // prints the result

    float SunAltitude = doc["SunAltitude"];  // 25.517578201431174
    float SunAzimuth = doc["SunAzimuth"];    // 200.37731432217743

    Serial.print("Sun Alt ");
    Serial.println(SunAltitude);
    Serial.print("Sun Az ");
    Serial.println(SunAzimuth);
    /*
    const char* MercuryRise = doc["MercuryRise"];     // "2025/1/17 06:30:23"
    const char* MercurySet = doc["MercurySet"];       // "2025/1/16 15:35:13"
    double MercuryAltitude = doc["MercuryAltitude"];  // 17.441034894694717
    double MercuryAzimuth = doc["MercuryAzimuth"];    // 213.54654741936565
    const char* VenusRise = doc["VenusRise"];         // "2025/1/17 09:26:19"
    const char* VenusSet = doc["VenusSet"];           // "2025/1/16 20:50:01"
    double VenusAltitude = doc["VenusAltitude"];      // 36.67773335237495
    double VenusAzimuth = doc["VenusAzimuth"];        // 146.30639496533828
    const char* MoonRise = doc["MoonRise"];           // "2025/1/16 19:52:34"
    const char* MoonSet = doc["MoonSet"];             // "2025/1/17 09:28:57"
    double MoonAltitude = doc["MoonAltitude"];        // -34.443639946566314
    double MoonAzimuth = doc["MoonAzimuth"];          // 342.1392565836261
    const char* MarsRise = doc["MarsRise"];           // "2025/1/16 16:22:22"
    const char* MarsSet = doc["MarsSet"];             // "2025/1/17 07:39:59"
    double MarsAltitude = doc["MarsAltitude"];        // -21.244509403833646
    double MarsAzimuth = doc["MarsAzimuth"];          // 19.14235134712756
    const char* JupiterRise = doc["JupiterRise"];     // "2025/1/16 13:25:25"
    const char* JupiterSet = doc["JupiterSet"];       // "2025/1/17 04:11:58"
    double JupiterAltitude = doc["JupiterAltitude"];  // -0.3904390394043313
    double JupiterAzimuth = doc["JupiterAzimuth"];    // 59.60623443381171
    const char* SaturnRise = doc["SaturnRise"];       // "2025/1/17 09:39:01"
    const char* SaturnSet = doc["SaturnSet"];         // "2025/1/16 20:55:19"
    double SaturnAltitude = doc["SaturnAltitude"];    // 34.64292779107436
    double SaturnAzimuth = doc["SaturnAzimuth"];      // 143.99641132766163
  */
    // Mercury
    const char* MercuryRiseString = doc["MercuryRise"];  // "2025/1/17 06:30:23"
    char MercuryRise[6];
    convertToTimeValue(MercuryRiseString, MercuryRise);
    Serial.print("MercuryRise (HHMM): ");
    Serial.println(MercuryRise);  // prints the result
    const char* MercurySetString = doc["MercurySet"];
    char MercurySet[6];
    convertToTimeValue(MercurySetString, MercurySet);

    double MercuryAltitude = doc["MercuryAltitude"];  // 17.441034894694717
    double MercuryAzimuth = doc["MercuryAzimuth"];    // 213.54654741936565
    Serial.print("Mercury Alt ");
    Serial.println(MercuryAltitude);
    Serial.print("Mercury Az ");
    Serial.println(MercuryAzimuth);

    // Venus
    const char* VenusRiseString = doc["VenusRise"];  // "2025/1/17 09:26:19"
    char VenusRise[6];
    convertToTimeValue(VenusRiseString, VenusRise);
    Serial.print("VenusRise (HHMM): ");
    Serial.println(VenusRise);  // prints the result
    const char* VenusSetString = doc["VenusSet"];
    char VenusSet[6];
    convertToTimeValue(VenusSetString, VenusSet);

    double VenusAltitude = doc["VenusAltitude"];  // 36.67773335237495
    double VenusAzimuth = doc["VenusAzimuth"];    // 146.30639496533828
    Serial.print("Venus Alt ");
    Serial.println(VenusAltitude);
    Serial.print("Venus Az ");
    Serial.println(VenusAzimuth);

    // Moon
    const char* MoonRiseString = doc["MoonRise"];  // "2025/1/16 19:52:34"
    char MoonRise[6];
    convertToTimeValue(MoonRiseString, MoonRise);
    Serial.print("MoonRise (HHMM): ");
    Serial.println(MoonRise);  // prints the result
    const char* MoonSetString = doc["MoonSet"];
    char MoonSet[6];
    convertToTimeValue(MoonSetString, MoonSet);

    double MoonAltitude = doc["MoonAltitude"];  // -34.443639946566314
    double MoonAzimuth = doc["MoonAzimuth"];    // 342.1392565836261
    Serial.print("Moon Alt ");
    Serial.println(MoonAltitude);
    Serial.print("Moon Az ");
    Serial.println(MoonAzimuth);

    // Mars
    const char* MarsRiseString = doc["MarsRise"];  // "2025/1/16 16:22:22"
    char MarsRise[6];
    convertToTimeValue(MarsRiseString, MarsRise);
    Serial.print("MarsRise (HHMM): ");
    Serial.println(MarsRise);  // prints the result
    const char* MarsSetString = doc["MarsSet"];
    char MarsSet[6];
    convertToTimeValue(MarsSetString, MarsSet);

    double MarsAltitude = doc["MarsAltitude"];  // -21.244509403833646
    double MarsAzimuth = doc["MarsAzimuth"];    // 19.14235134712756
    Serial.print("Mars Alt ");
    Serial.println(MarsAltitude);
    Serial.print("Mars Az ");
    Serial.println(MarsAzimuth);

    // Jupiter
    const char* JupiterRiseString = doc["JupiterRise"];  // "2025/1/16 13:25:25"
    char JupiterRise[6];
    convertToTimeValue(JupiterRiseString, JupiterRise);
    Serial.print("JupiterRise (HHMM): ");
    Serial.println(JupiterRise);  // prints the result
    const char* JupiterSetString = doc["JupiterSet"];
    char JupiterSet[6];
    convertToTimeValue(JupiterSetString, JupiterSet);

    double JupiterAltitude = doc["JupiterAltitude"];  // -0.3904390394043313
    double JupiterAzimuth = doc["JupiterAzimuth"];    // 59.60623443381171
    Serial.print("Jupiter Alt ");
    Serial.println(JupiterAltitude);
    Serial.print("Jupiter Az ");
    Serial.println(JupiterAzimuth);

    // Saturn
    const char* SaturnRiseString = doc["SaturnRise"];  // "2025/1/17 09:39:01"
    char SaturnRise[6];
    convertToTimeValue(SaturnRiseString, SaturnRise);
    Serial.print("SaturnRise (HHMM): ");
    Serial.println(SaturnRise);  // prints the result
    const char* SaturnSetString = doc["SaturnSet"];
    char SaturnSet[6];
    convertToTimeValue(SaturnSetString, SaturnSet);

    double SaturnAltitude = doc["SaturnAltitude"];  // 34.64292779107436
    double SaturnAzimuth = doc["SaturnAzimuth"];    // 143.99641132766163
    Serial.print("Saturn Alt ");
    Serial.println(SaturnAltitude);
    Serial.print("Saturn Az ");
    Serial.println(SaturnAzimuth);

    // For the active planet set by the button, display rise and set times
    // Display HH:MM if rise/set time
    if (SunActive) {
      display_rise.println(SunRise);
      display_set.println(SunSet);
    }
    // Display HH:MM if rise/set time
    if (MercuryActive) {
      display_rise.println(MercuryRise);
      display_set.println(MercurySet);
    }
    // Display HH:MM if rise/set time
    if (VenusActive) {
      display_rise.println(VenusRise);
      display_set.println(VenusSet);
    }
    // Display HH:MM if rise/set time
    if (MoonActive) {
      display_rise.println(MoonRise);
      display_set.println(MoonSet);
    }
    // Display HH:MM if rise/set time
    if (MarsActive) {
      display_rise.println(MarsRise);
      display_set.println(MarsSet);
    }
    // Display HH:MM if rise/set time
    if (JupiterActive) {
      display_rise.println(JupiterRise);
      display_set.println(JupiterSet);
    }
    // Display HH:MM if rise/set time
    if (SaturnActive) {
      display_rise.println(SaturnRise);
      display_set.println(SaturnSet);
    }
  }
}
void displayPlanet(const char* label, const char* riseStr, const char* setStr, bool useUTC, bool utcIsNextDay) {
  // Extract HH:MM from timestamp string: "2025/9/19 06:39:32"
  String riseTime = String(riseStr).substring(11, 16);  // "06:39"
  String setTime  = String(setStr).substring(11, 16);   // "18:56"

  // Optional: annotate mode
  // String modeLabel = useUTC ? (utcIsNextDay ? "UTC+1" : "UTC") : "LOCAL";

  // Display rise/set times on separate modules
  display_rise.println(riseTime);  // Left module
  display_set.println(setTime);    // Right module
}


void convertUTCToLocal(const char* utcTime, char* output, bool* didRollover) {
  int hour = (utcTime[0] - '0') * 10 + (utcTime[1] - '0');
  int minute = (utcTime[3] - '0') * 10 + (utcTime[4] - '0');
  int second = (utcTime[6] - '0') * 10 + (utcTime[7] - '0');

  hour -= 5;
  if (hour < 0) {
    hour += 24;
    *didRollover = true;  // UTC was early morning, local is previous day
  } else {
    *didRollover = false;
  }

  sprintf(output, "%02d-%02d-%02d", hour, minute, second);
}

void decrementDate(const char* inputDate, char* outputDate) {
  int year  = (inputDate[0] - '0') * 1000 + (inputDate[1] - '0') * 100 + (inputDate[2] - '0') * 10 + (inputDate[3] - '0');
  int month = (inputDate[5] - '0') * 10 + (inputDate[6] - '0');
  int day   = (inputDate[8] - '0') * 10 + (inputDate[9] - '0');

  int daysInMonth[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

  // Leap year check
  bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
  if (isLeap) daysInMonth[1] = 29;

  day--;
  if (day == 0) {
    month--;
    if (month == 0) {
      month = 12;
      year--;
    }
    day = daysInMonth[month - 1];
  }

  // Format back to YYYY-MM-DD
  sprintf(outputDate, "%04d-%02d-%02d", year, month, day);
}



float convertDMSToDecimal(const char* dms) {
  int deg, min;
  int sec, tenths;
  sscanf(dms, "%d:%d:%d.%d", &deg, &min, &sec, &tenths);
  Serial.print("sec.tenths");
  Serial.print(sec);
  Serial.print(".");
  Serial.println(tenths);
  float decimalDegrees = 0.;
  float d = 0.0, m = 0.0, s = 0.0;
  if (deg > 0) {
    d = float(deg);
    m = float(min / 60.0);
    if (tenths < 10)
      s = (sec + tenths / 10) / 3600.0;
    else
      s = (sec + tenths / 100) / 3600.0;
    decimalDegrees = d + m + s;
  } else {
    d = float(deg);
    m = float(min / 60.0);
    s = sec / 3600.0;
    decimalDegrees = d - m - s;
  }
  return decimalDegrees;
}

void convertToTimeValue(const char* datetime, char* output) {
  int len = strlen(datetime);

  // Defensive check: ensure string is long enough
  if (len < 8) {
    strcpy(output, "??:??");  // fallback
    return;
  }

  // Extract HH:MM from the end: "HH:MM:SS"
  output[0] = datetime[len - 8];  // H tens
  output[1] = datetime[len - 7];  // H units
  output[2] = ':';                // colon
  output[3] = datetime[len - 5];  // M tens
  output[4] = datetime[len - 4];  // M units
  output[5] = '\0';               // null terminator
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++ Useful Functions++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void allLEDoff() {
  digitalWrite(LED_SUN, LOW);
  digitalWrite(LED_MERCURY, LOW);
  digitalWrite(LED_VENUS, LOW);
  digitalWrite(LED_MOON, LOW);
  digitalWrite(LED_MARS, LOW);
  digitalWrite(LED_JUPITER, LOW);
  digitalWrite(LED_SATURN, LOW);
}
void testLEDon() {
  digitalWrite(LED_SUN, HIGH);
  delay(2000);
  digitalWrite(LED_MERCURY, HIGH);
  delay(2000);
  digitalWrite(LED_VENUS, HIGH);
  delay(2000);
  digitalWrite(LED_MOON, HIGH);
  delay(2000);
  digitalWrite(LED_MARS, HIGH);
  delay(2000);
  digitalWrite(LED_JUPITER, HIGH);
  delay(2000);
  digitalWrite(LED_SATURN, HIGH);
}
void allActiveFalse() {
  SunActive = false;
  MercuryActive = false;
  VenusActive = false;
  MoonActive = false;
  MarsActive = false;
  JupiterActive = false;
  SaturnActive = false;
}
