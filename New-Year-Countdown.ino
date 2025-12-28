#include <LiquidCrystal.h>
#include <WiFi.h>
#include <time.h>

const int rs = 13;
const int en = 33;
const int d4 = 14;
const int d5 = 27;
const int d6 = 26;
const int d7 = 25;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define buzzerPin 32
#define ledPin 2
#define TEST_PIN 34

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

const char* ntpServer = "pool.ntp.org";
const char* ntpServer2 = "time.google.com";
const long gmtOffset_sec = 7 * 3600;

time_t targetTime = 0;
time_t testTargetTime = 0;
bool finalDone = false;
bool testMode = false;

unsigned long lastBeep = 0;
unsigned long lastLedPulse = 0;
const unsigned long ledPulseMs = 50;

time_t getNowUtc() {
  time_t now;
  time(&now);
  return now;
}

void waitForValidTime() {
  time_t now = 0;
  while (now < 1700000000) {
    time(&now);
    delay(200);
  }
}

void setTarget2026() {
  targetTime = 1767200400;
  finalDone = false;
}

bool isTestTriggered() {
  return digitalRead(TEST_PIN) == HIGH;
}

void setup() {
  Serial.begin(115200);

  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(TEST_PIN, INPUT);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Connecting WiFi");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);
  delay(100);
  WiFi.begin(ssid, password);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 30000) {
    delay(500);
  }

  lcd.clear();
  if (WiFi.status() == WL_CONNECTED) {
    lcd.print("WiFi Connected");
    delay(500);
  } else {
    lcd.print("WiFi Failed");
    delay(2000);
  }

  configTime(0, 0, ntpServer, ntpServer2);
  
  lcd.clear();
  lcd.print("Syncing Time...");
  waitForValidTime();

  setTarget2026();
  lcd.clear();
}

void loop() {
  time_t nowUtc = getNowUtc();
  time_t activeTarget = targetTime;

  if (!finalDone && isTestTriggered()) {
    if (!testMode) {
      testMode = true;
      testTargetTime = nowUtc + 10;
      lcd.clear();
    }
    activeTarget = testTargetTime;
  } else if (!isTestTriggered() && testMode) {
    testMode = false;
    lcd.clear();
  } else if (!testMode) {
    activeTarget = targetTime;
  }

  long remaining = activeTarget - nowUtc;

  if (remaining > 0 && !finalDone) {
    time_t localDisplayTime = nowUtc + gmtOffset_sec;
    struct tm timeinfo;
    gmtime_r(&localDisplayTime, &timeinfo);

    int tenths = (millis() % 1000) / 100;

    char line1[17];
    snprintf(line1, sizeof(line1), "%02d:%02d:%02d.%1d%s",
             timeinfo.tm_hour,
             timeinfo.tm_min,
             timeinfo.tm_sec,
             tenths,
             testMode ? " T" : "  ");
    lcd.setCursor(0, 0);
    lcd.print(line1);

    char line2[17];
    snprintf(line2, sizeof(line2), "T-%05ld sec ", remaining);
    lcd.setCursor(0, 1);
    lcd.print(line2);

    unsigned long nowMs = millis();
    int interval = 1000;
    
    if (remaining <= 60) interval = 1000;
    if (remaining <= 30) interval = 700;
    if (remaining <= 15) interval = 400;
    if (remaining <= 10) interval = 250;
    if (remaining <= 5)  interval = 120;
    if (remaining <= 2)  interval = 70;

    if (nowMs - lastBeep >= interval) {
      lastBeep = nowMs;
      lastLedPulse = nowMs;
      tone(buzzerPin, 2000, 50);
      digitalWrite(ledPin, HIGH);
    }

    if (millis() - lastLedPulse >= ledPulseMs) {
      digitalWrite(ledPin, LOW);
    }
  }

  if (remaining <= 0 && !finalDone) {
    finalDone = true;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("HAPPY NEW YEAR!");
    lcd.setCursor(0, 1);
    lcd.print("00:00:00.0     ");

    digitalWrite(ledPin, HIGH);
    tone(buzzerPin, 2000); 
    delay(4000);
    noTone(buzzerPin);
    digitalWrite(ledPin, LOW);
  }
}
