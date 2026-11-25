#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <TinyGPSPlus.h>
#include "pins.h"

// Initialize OLED (SH1106, 128x64, Full Buffer, I2C)
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Initialize GPS
TinyGPSPlus gps;
HardwareSerial GPS_Serial(2); // Use UART2

// Variables for LoRa
const long frequency = 433E6; // 433 MHz

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("LoRa32++ V1.3 Booting...");

  // 1. Initialize Buttons
  pinMode(BTN_USER_1, INPUT_PULLUP);
  pinMode(BTN_USER_2, INPUT_PULLUP);

  // 2. Initialize I2C and OLED
  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 15, "LoRa32++ V1.3");
  u8g2.drawStr(0, 30, "Booting...");
  u8g2.sendBuffer();

  // 3. Initialize GPS
  GPS_Serial.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  Serial.println("GPS Serial Initialized.");

  // 4. Initialize LoRa
  // Note: For ESP32, LoRa.setPins() is highly recommended
  LoRa.setPins(LORA_NSS_PIN, LORA_RESET_PIN, LORA_DIO0_PIN);
  
  if (!LoRa.begin(frequency)) {
    Serial.println("Starting LoRa failed!");
    u8g2.clearBuffer();
    u8g2.drawStr(0, 15, "LoRa Init Failed!");
    u8g2.sendBuffer();
    while (1);
  }
  Serial.println("LoRa Initialized successfully.");
  
  u8g2.clearBuffer();
  u8g2.drawStr(0, 15, "System Ready!");
  u8g2.sendBuffer();
  delay(1000);
}

void loop() {
  // ==========================================
  // 1. Process GPS Data
  // ==========================================
  while (GPS_Serial.available() > 0) {
    if (gps.encode(GPS_Serial.read())) {
      if (gps.location.isUpdated()) {
        Serial.print("Lat: ");
        Serial.print(gps.location.lat(), 6);
        Serial.print(" | Lng: ");
        Serial.println(gps.location.lng(), 6);
      }
    }
  }

  // ==========================================
  // 2. Handle Buttons
  // ==========================================
  if (digitalRead(BTN_USER_1) == LOW) {
    Serial.println("Button 1 Pressed");
    delay(200); // Basic debounce
  }
  if (digitalRead(BTN_USER_2) == LOW) {
    Serial.println("Button 2 Pressed");
    delay(200); // Basic debounce
  }

  // ==========================================
  // 3. Update OLED Display (Every 1 second)
  // ==========================================
  static unsigned long lastDisplayUpdate = 0;
  if (millis() - lastDisplayUpdate > 1000) {
    lastDisplayUpdate = millis();
    
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);
    
    // Header
    u8g2.drawStr(0, 10, "LoRa32++ V1.3");
    u8g2.drawLine(0, 12, 128, 12);

    // GPS Info
    if (gps.location.isValid()) {
      char latStr[20];
      char lngStr[20];
      sprintf(latStr, "Lat: %.4f", gps.location.lat());
      sprintf(lngStr, "Lng: %.4f", gps.location.lng());
      u8g2.drawStr(0, 25, latStr);
      u8g2.drawStr(0, 37, lngStr);
    } else {
      u8g2.drawStr(0, 25, "GPS: Searching...");
    }

    // Satellites
    char satStr[20];
    sprintf(satStr, "Sats: %d", gps.satellites.value());
    u8g2.drawStr(0, 50, satStr);

    u8g2.sendBuffer();
  }

  // Add LoRa receiving/transmitting logic here...
}