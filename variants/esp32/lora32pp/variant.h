#ifndef LORA32PP_V1_3_PINS_H
#define LORA32PP_V1_3_PINS_H

// ==========================================
// LoRa Ra-02 (SX1278)
// ==========================================
#define LORA_NSS_PIN    18
#define LORA_SCK_PIN    5
#define LORA_MOSI_PIN   27
#define LORA_MISO_PIN   19
#define LORA_RESET_PIN  23
#define LORA_DIO0_PIN   26
#define LORA_DIO1_PIN   33

// ==========================================
// GPS ATGM336H (Hardware Serial 2)
// ==========================================
#define GPS_TX_PIN      13  // ESP32 RX (получает данные от GPS TX)
#define GPS_RX_PIN      16  // ESP32 TX (отправляет данные на GPS RX)
// Примечание: не забудьте в коде указать Serial2.begin(9600, SERIAL_8N1, GPS_TX_PIN, GPS_RX_PIN);

// ==========================================
// OLED SH1106 (I2C)
// ==========================================
#define OLED_SDA_PIN    21
#define OLED_SCL_PIN    22

// ==========================================
// Кнопки управления
// ==========================================
#define BTN_USER_1      15  // Бывшая Boot
#define BTN_USER_2      4   // Бывшая на GPIO 39

// ==========================================
// Питание (для справки)
// ==========================================
// VIN (5V) - подается с MT3608
// 3V3 - питание LoRa, GPS, OLED

#endif // LORA32PP_V1_3_PINS_H
