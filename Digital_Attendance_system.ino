#include <WiFi.h>
#include <WebServer.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>

// ================= WIFI =================
const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// ================= PINS =================
#define SS_PIN       5
#define RST_PIN      27
#define BUZZER_PIN   25

#define OLED_SDA     21
#define OLED_SCL     22

// ================= OLED =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

// ================= RFID =================
MFRC522 rfid(SS_PIN, RST_PIN);

// ================= WEB SERVER =================
WebServer server(80);

// ================= MINIMUM STAY =================
const unsigned long MINIMUM_STAY_SECONDS = 60;

// ================= USER STRUCTURE =================
struct User {
  String uid;
  String name;

  bool inside;

  time_t entryTime;
  time_t exitTime;
};

User users[] = {
  {"4E748F04", "Nandini", false, 0, 0},
  {"BAE66905", "Srijeet", false, 0, 0},
  {"495CDAB7", "Deepra", false, 0, 0},
  {"290256B7", "Ahan", false, 0, 0}
};

const int USER_COUNT = sizeof(users) / sizeof(users[0]);

// ================= BUZZER =================
void entryBeep() {
  tone(BUZZER_PIN, 2000);
  delay(250);
  noTone(BUZZER_PIN);
}

void exitBeep() {
  tone(BUZZER_PIN, 2000);
  delay(200);
  noTone(BUZZER_PIN);

  delay(150);

  tone(BUZZER_PIN, 2000);
  delay(200);
  noTone(BUZZER_PIN);
}

void deniedBeep() {
  for (int i = 0; i < 3; i++) {
    tone(BUZZER_PIN, 1500);
    delay(150);
    noTone(BUZZER_PIN);
    delay(150);
  }
}

// ================= OLED =================
void showMessage(String line1, String line2 = "") {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(line1);

  display.setTextSize(2);
  display.setCursor(0, 20);
  display.println(line2);

  display.display();
}

void showClock() {

  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {

    display.clearDisplay();

    display.setTextColor(SSD1306_WHITE);

    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Attendance System");

    display.setTextSize(2);
    display.setCursor(10, 25);
    display.println("--:--:--");

    display.display();

    return;
  }

  char timeString[10];

  strftime(
    timeString,
    sizeof(timeString),
    "%H:%M:%S",
    &timeinfo
  );

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("DIGITAL ATTENDANCE");

  display.setTextSize(2);
  display.setCursor(10, 22);
  display.println(timeString);

  display.setTextSize(1);
  display.setCursor(20, 50);
  display.println("Scan RFID Card");

  display.display();
}

// ================= FIND USER =================
int findUser(String uid) {

  for (int i = 0; i < USER_COUNT; i++) {

    if (users[i].uid == uid) {
      return i;
    }
  }

  return -1;
}

// ================= GET RFID UID =================
String getUID() {

  String uid = "";

  for (byte i = 0; i < rfid.uid.size; i++) {

    if (rfid.uid.uidByte[i] < 0x10) {
      uid += "0";
    }

    uid += String(
      rfid.uid.uidByte[i],
      HEX
    );
  }

  uid.toUpperCase();

  return uid;
}

// ================= TIME FORMAT =================
String formatTime(time_t timestamp) {

  if (timestamp == 0) {
    return "--";
  }

  struct tm timeinfo;

  localtime_r(&timestamp, &timeinfo);

  char buffer[20];

  strftime(
    buffer,
    sizeof(buffer),
    "%d-%m-%Y %H:%M:%S",
    &timeinfo
  );

  return String(buffer);
}

// ================= RFID PROCESS =================
void processRFID() {

  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  String uid = getUID();

  Serial.println();
  Serial.println("RFID Detected");
  Serial.print("UID: ");
  Serial.println(uid);

  int userIndex = findUser(uid);

  // ================= UNKNOWN CARD =================
  if (userIndex == -1) {

    Serial.println("ACCESS DENIED");

    showMessage("ACCESS DENIED", "Unknown");

    deniedBeep();

    delay(1500);

    showClock();

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();

    return;
  }

  User &user = users[userIndex];

  // ================= ENTRY =================
  if (!user.inside) {

    user.inside = true;

    user.entryTime = time(nullptr);
    user.exitTime = 0;

    Serial.println("ACCESS GRANTED");
    Serial.print("Name: ");
    Serial.println(user.name);

    Serial.println("ENTRY RECORDED");
    Serial.print("Entry Time: ");
    Serial.println(formatTime(user.entryTime));

    showMessage(
      "WELCOME",
      user.name
    );

    entryBeep();

    delay(1500);
  }

  // ================= EXIT =================
  else {

    time_t currentTime = time(nullptr);

    unsigned long stayTime =
      currentTime - user.entryTime;

    // Less than 1 minute
    if (stayTime < MINIMUM_STAY_SECONDS) {

      unsigned long remaining =
        MINIMUM_STAY_SECONDS - stayTime;

      Serial.println("EXIT BLOCKED");

      Serial.print("Please stay ");
      Serial.print(remaining);
      Serial.println(" more seconds.");

      showMessage(
        "EXIT BLOCKED",
        String(remaining) + " sec"
      );

      deniedBeep();

      delay(1500);
    }

    // More than 1 minute
    else {

      user.inside = false;

      user.exitTime = currentTime;

      Serial.println("EXIT ALLOWED");

      Serial.print("Name: ");
      Serial.println(user.name);

      Serial.print("Exit Time: ");
      Serial.println(formatTime(user.exitTime));

      Serial.println("ATTENDANCE RECORDED");

      showMessage(
        "GOODBYE",
        user.name
      );

      exitBeep();

      delay(1500);
    }
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();

  showClock();
}

// ================= API =================
void handleAPI() {

  String json = "{";

  json += "\"currentTime\":\"";
  json += formatTime(time(nullptr));
  json += "\",";

  json += "\"users\":[";

  for (int i = 0; i < USER_COUNT; i++) {

    json += "{";

    json += "\"name\":\"";
    json += users[i].name;
    json += "\",";

    json += "\"uid\":\"";
    json += users[i].uid;
    json += "\",";

    json += "\"inside\":";
    json += users[i].inside ? "true" : "false";
    json += ",";

    json += "\"entryTime\":\"";
    json += formatTime(users[i].entryTime);
    json += "\",";

    json += "\"exitTime\":\"";
    json += formatTime(users[i].exitTime);
    json += "\"";

    json += "}";

    if (i < USER_COUNT - 1) {
      json += ",";
    }
  }

  json += "]";

  json += "}";

  server.send(
    200,
    "application/json",
    json
  );
}

// ================= WEB SERVER =================
void handleRoot() {

  server.send(
    200,
    "text/plain",
    "ESP32 Attendance API is running."
  );
}

// ================= SETUP =================
void setup() {

  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);

  // OLED
  Wire.begin(
    OLED_SDA,
    OLED_SCL
  );

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        0x3C
      )) {

    Serial.println("OLED failed!");
  }

  showMessage(
    "Starting...",
    "Please wait"
  );

  // RFID
  SPI.begin();

  rfid.PCD_Init();

  Serial.println("RFID initialized.");

  // ================= WIFI =================
  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );

  Serial.print("Connecting WiFi");

  unsigned long wifiStart = millis();

  while (
    WiFi.status() != WL_CONNECTED &&
    millis() - wifiStart < 15000
  ) {

    delay(500);

    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {

    Serial.println("WiFi Connected!");

    Serial.print("ESP32 IP Address: ");
    Serial.println(WiFi.localIP());

    configTime(
      19800,
      0,
      "pool.ntp.org",
      "time.nist.gov"
    );

    server.on(
      "/",
      handleRoot
    );

    server.on(
      "/api/data",
      handleAPI
    );

    server.begin();

    Serial.println(
      "Dashboard API started."
    );
  }

  else {

    Serial.println(
      "WiFi not connected."
    );

    Serial.println(
      "Running RFID offline."
    );
  }

  showClock();
}

// ================= LOOP =================
void loop() {

  // Web server
  if (WiFi.status() == WL_CONNECTED) {
    server.handleClient();
  }

  // RFID
  processRFID();

  // Clock refresh
  static unsigned long lastClockUpdate = 0;

  if (millis() - lastClockUpdate >= 1000) {

    lastClockUpdate = millis();

    showClock();
  }

  delay(50);
}
