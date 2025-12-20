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
const long gmtOffset_sec = 7 * 3600;
const int daylightOffset_sec = 0;

time_t targetTime = 0;
time_t testTargetTime = 0;
bool finalDone = false;
bool testMode = false;

unsigned long lastBeep = 0;
unsigned long lastLedPulse = 0;
const unsigned long ledPulseMs = 80;

time_t getNow() {
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
  struct tm t = {};
  t.tm_year = 2026 - 1900;
  t.tm_mon  = 0;
  t.tm_mday = 1;
  t.tm_hour = 0;
  t.tm_min  = 0;
  t.tm_sec  = 0;

  targetTime = mktime(&t);
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

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  waitForValidTime();

  setTarget2026();
  lcd.clear();
}

void loop() {
  time_t now = getNow();
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);

  time_t activeTarget = targetTime;

  if (!finalDone && isTestTriggered()) {
    if (!testMode) {
      testMode = true;
      testTargetTime = now + 5;
      lcd.clear();
    }
    activeTarget = testTargetTime;
  } else {
    testMode = false;
  }

  long remaining = activeTarget - now;

  if (remaining > 0 && !finalDone) {
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
    if (remaining <= 10) interval = 500;
    if (remaining <= 5)  interval = 250;
    if (remaining <= 3)  interval = 150;
    if (remaining <= 1)  interval = 80;

    if (nowMs - lastBeep >= interval) {
      lastBeep = nowMs;
      lastLedPulse = nowMs;
      tone(buzzerPin, 1200, 60);
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

    tone(buzzerPin, 2000, 3000);
    digitalWrite(ledPin, HIGH);
  }
}
