#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =================================================
// RFID PINS
// =================================================

#define SS_PIN 5
#define RST_PIN 27

MFRC522 rfid(SS_PIN, RST_PIN);


// =================================================
// OLED
// =================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);


// =================================================
// BUZZER
// =================================================

#define BUZZER_PIN 25


// =================================================
// RFID UIDs
// =================================================

// Nandjni
byte uidNandjni[] = {
  0x4E, 0x74, 0x8F, 0x04
};

// Srijeet
byte uidSrijeet[] = {
  0xBA, 0xE6, 0x69, 0x05
};

// Deepra
byte uidDeepra[] = {
  0x49, 0x5C, 0xDA, 0xB7
};

// Ahan
byte uidAhan[] = {
  0x29, 0x02, 0x56, 0xB7
};


// =================================================
// INSIDE / OUTSIDE STATUS
// =================================================

// false = outside
// true  = inside

bool nandjniInside = false;
bool srijeetInside = false;
bool deepraInside  = false;
bool ahanInside    = false;


// =================================================
// COMPARE RFID UID
// =================================================

bool checkUID(byte *scannedUID, byte *storedUID, byte length) {

  for (byte i = 0; i < length; i++) {

    if (scannedUID[i] != storedUID[i]) {
      return false;
    }

  }

  return true;
}


// =================================================
// BUZZER FUNCTIONS
// =================================================

// One beep = ENTRY

void entryBeep() {

  tone(BUZZER_PIN, 2000);
  delay(250);
  noTone(BUZZER_PIN);

}


// Two beeps = EXIT

void exitBeep() {

  tone(BUZZER_PIN, 2000);
  delay(200);
  noTone(BUZZER_PIN);

  delay(150);

  tone(BUZZER_PIN, 2000);
  delay(200);
  noTone(BUZZER_PIN);

}


// Three beeps = UNKNOWN CARD

void deniedBeep() {

  for (int i = 0; i < 3; i++) {

    tone(BUZZER_PIN, 1500);
    delay(150);

    noTone(BUZZER_PIN);
    delay(150);

  }

}


// =================================================
// OLED READY SCREEN
// =================================================

void showReadyScreen() {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(15, 5);
  display.println("RFID");

  display.setTextSize(1);
  display.setCursor(20, 35);
  display.println("Attendance");

  display.setCursor(30, 50);
  display.println("Scan Card");

  display.display();

}


// =================================================
// OLED ENTRY SCREEN
// =================================================

void showEntry(String name) {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(25, 3);
  display.println("ENTRY RECORDED");

  display.setTextSize(2);
  display.setCursor(10, 22);
  display.println(name);

  display.setTextSize(1);
  display.setCursor(35, 50);
  display.println("WELCOME");

  display.display();

}


// =================================================
// OLED EXIT SCREEN
// =================================================

void showExit(String name) {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(25, 2);
  display.println("EXIT RECORDED");

  display.setTextSize(2);
  display.setCursor(10, 22);
  display.println(name);

  display.setTextSize(1);
  display.setCursor(25, 50);
  display.println("ATTENDANCE OK");

  display.display();

}


// =================================================
// OLED DENIED SCREEN
// =================================================

void showDenied() {

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(30, 5);
  display.println("ACCESS DENIED");

  display.setTextSize(2);
  display.setCursor(20, 25);
  display.println("UNKNOWN");

  display.setTextSize(1);
  display.setCursor(40, 50);
  display.println("CARD");

  display.display();

}


// =================================================
// SETUP
// =================================================

void setup() {

  // Serial Monitor
  Serial.begin(115200);


  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  noTone(BUZZER_PIN);


  // OLED
  Wire.begin(21, 22);

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        0x3C
      )) {

    Serial.println("OLED NOT FOUND!");

    while (1);
  }


  // RFID
  SPI.begin();

  rfid.PCD_Init();


  // Initial OLED screen
  showReadyScreen();


  // Serial information
  Serial.println();
  Serial.println("==============================");
  Serial.println(" RFID ATTENDANCE SYSTEM");
  Serial.println("==============================");
  Serial.println();

  Serial.println("System Ready.");
  Serial.println("Scan RFID card...");
  Serial.println();

}


// =================================================
// MAIN LOOP
// =================================================

void loop() {

  // Check for new RFID card
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }


  // Read RFID card
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }


  // =================================================
  // PRINT UID
  // =================================================

  Serial.print("Card UID: ");

  for (byte i = 0; i < rfid.uid.size; i++) {

    if (rfid.uid.uidByte[i] < 0x10) {
      Serial.print("0");
    }

    Serial.print(
      rfid.uid.uidByte[i],
      HEX
    );

    Serial.print(" ");
  }

  Serial.println();


  // =================================================
  // NANDJNI
  // =================================================

  if (
    rfid.uid.size == 4 &&
    checkUID(
      rfid.uid.uidByte,
      uidNandjni,
      4
    )
  ) {

    Serial.println("Person: Nandjni");


    // ---------------------------------------------
    // Nandjni is OUTSIDE
    // Therefore this is ENTRY
    // ---------------------------------------------

    if (nandjniInside == false) {

      nandjniInside = true;

      Serial.println("STATUS: ENTRY");
      Serial.println("Nandjni is now INSIDE");
      Serial.println("Attendance NOT recorded yet.");

      showEntry("Nandjni");

      entryBeep();

    }


    // ---------------------------------------------
    // Nandjni is INSIDE
    // Therefore this is EXIT
    // ---------------------------------------------

    else {

      nandjniInside = false;

      Serial.println("STATUS: EXIT");
      Serial.println("Nandjni has LEFT");
      Serial.println("ATTENDANCE RECORDED!");

      showExit("Nandjni");

      exitBeep();

    }

  }


  // =================================================
  // SRIJEET
  // =================================================

  else if (
    rfid.uid.size == 4 &&
    checkUID(
      rfid.uid.uidByte,
      uidSrijeet,
      4
    )
  ) {

    Serial.println("Person: Srijeet");


    if (srijeetInside == false) {

      srijeetInside = true;

      Serial.println("STATUS: ENTRY");
      Serial.println("Srijeet is now INSIDE");
      Serial.println("Attendance NOT recorded yet.");

      showEntry("Srijeet");

      entryBeep();

    }

    else {

      srijeetInside = false;

      Serial.println("STATUS: EXIT");
      Serial.println("Srijeet has LEFT");
      Serial.println("ATTENDANCE RECORDED!");

      showExit("Srijeet");

      exitBeep();

    }

  }


  // =================================================
  // DEEPRA
  // =================================================

  else if (
    rfid.uid.size == 4 &&
    checkUID(
      rfid.uid.uidByte,
      uidDeepra,
      4
    )
  ) {

    Serial.println("Person: Deepra");


    if (deepraInside == false) {

      deepraInside = true;

      Serial.println("STATUS: ENTRY");
      Serial.println("Deepra is now INSIDE");
      Serial.println("Attendance NOT recorded yet.");

      showEntry("Deepra");

      entryBeep();

    }

    else {

      deepraInside = false;

      Serial.println("STATUS: EXIT");
      Serial.println("Deepra has LEFT");
      Serial.println("ATTENDANCE RECORDED!");

      showExit("Deepra");

      exitBeep();

    }

  }


  // =================================================
  // AHAN
  // =================================================

  else if (
    rfid.uid.size == 4 &&
    checkUID(
      rfid.uid.uidByte,
      uidAhan,
      4
    )
  ) {

    Serial.println("Person: Ahan");


    if (ahanInside == false) {

      ahanInside = true;

      Serial.println("STATUS: ENTRY");
      Serial.println("Ahan is now INSIDE");
      Serial.println("Attendance NOT recorded yet.");

      showEntry("Ahan");

      entryBeep();

    }

    else {

      ahanInside = false;

      Serial.println("STATUS: EXIT");
      Serial.println("Ahan has LEFT");
      Serial.println("ATTENDANCE RECORDED!");

      showExit("Ahan");

      exitBeep();

    }

  }


  // =================================================
  // UNKNOWN RFID CARD
  // =================================================

  else {

    Serial.println("UNKNOWN CARD!");
    Serial.println("ACCESS DENIED");

    showDenied();

    deniedBeep();

  }


  // Stop RFID communication
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();


  // Keep result on OLED
  delay(2500);


  // Return to ready screen
  showReadyScreen();


  // Small delay to prevent immediate double reading
  delay(500);

}