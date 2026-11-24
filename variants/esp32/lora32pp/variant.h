#pragma once

/*
 * LoRa32++ V1.3 — Custom Meshtastic Variant
 * 
 * Hardware: ESP32-WROOM-32 + Ra-02 (SX1278) + NEO-7M GPS + SH1106 OLED 1.3"
 * PCB: 2-layer custom design in EasyEDA Pro
 * Power: MT3608 boost (trimmed to 3.3V) + TP4056 Type-C charger
 * 
 * This variant uses PRIVATE_HW model. Flash via PlatformIO or esptool.py.
 * Not available in official Web Flasher.
 */

// ---- Architecture --------------------------------------------------------
#define ARCH_ESP32

// ---- Hardware Model (PRIVATE_HW for DIY boards) --------------------------
#define HW_VENDOR meshtastic_HardwareModel_PRIVATE_HW

// ---- Display: SH1106 1.3" 128x64 I2C -------------------------------------
#define HAS_SCREEN 1
#define USE_SH1106              // Force SH1106 driver (not SSD1306)
#define I2C_SDA 21              // GPIO21
#define I2C_SCL 22              // GPIO22
// Address 0x3C is default; if your panel uses 0x3D, add: #define OLED_ADDR 0x3D

// ---- LoRa: Ai-Thinker Ra-02 (SX1278, 433 MHz) ----------------------------
#define HAS_RADIO 1
#define USE_RF95                // SX127x driver in RadioLib/Meshtastic

#undef LORA_SCK
#define LORA_SCK 5              // GPIO5

#undef LORA_MISO
#define LORA_MISO 19            // GPIO19

#undef LORA_MOSI
#define LORA_MOSI 27            // GPIO27

#undef LORA_CS
#define LORA_CS 18              // GPIO18

#define LORA_RESET 23           // GPIO23
#define LORA_DIO0 26            // GPIO26 — primary IRQ
#define LORA_DIO1 33            // GPIO33 — MUST be wired for RX interrupt
#define LORA_DIO2 32            // GPIO32 — optional, unused on SX127x

// SX1278 PA_BOOST accepts 2..17 dBm or exactly 20; 18/19 coerced to 17.
// UA_433 region clamps to 10 dBm anyway.
#define RF95_MAX_POWER 17

// ---- GNSS: u-blox NEO-7M (GPS + GLONASS + QZSS + SBAS) -------------------
#define HAS_GPS 1
#define GPS_UBLOX               // Enables u-blox driver
#define GPS_BAUDRATE 9600       // Default NEO-7M baud rate

#define GPS_RX_PIN 13           // GPIO13 — ESP32 receives ← module TX
#define GPS_TX_PIN 14           // GPIO14 — ESP32 transmits → module RX

// No hardware power gating (IRF7342 removed in V1.3).
// GPS stays powered whenever the node is on.
// Uncomment below if you add external MOSFET later:
// #define PIN_GPS_EN 15
// #define GPS_EN_ACTIVE 0

// ---- Battery Sensing -----------------------------------------------------
#define BATTERY_PIN 35          // GPIO35 (ADC1_CH7, input-only)
#define ADC_CHANNEL ADC_CHANNEL_7
#define ADC_MULTIPLIER 2.0      // 100k/100k divider: 4.2V → 2.1V at ADC
#define BATTERY_SENSE_SAMPLES 30

// ---- Buttons -------------------------------------------------------------
#define HAS_BUTTON 1
#define BUTTON_PIN 39           // GPIO39 — USER button (input-only, needs ext pull-up)
#define BUTTON_NEED_PULLUP false // External 10k already on PCB

// BOOT (GPIO0) and RESET (EN) handled by DevKit onboard circuits.
// If you wire external buttons, uncomment:
// #define BUTTON_PIN_ALT 0

// ---- LED -----------------------------------------------------------------
#define LED_POWER 25            // GPIO25 — DevKit onboard LED

// ---- Power Management ----------------------------------------------------
// No PMIC (AXP192/AXP2101). MT3608 provides fixed 3.3V rail.
// TP4056 handles charging independently.
#undef HAS_PMU

// ---- Serial / Debug ------------------------------------------------------
#define SERIAL0_RX_GPIO 3       // Always GPIO3 on ESP32

// ---- Misc Flags ----------------------------------------------------------
#define HAS_WIFI 1
#define HAS_BLUETOOTH 1
#define HAS_WIRE 1              // I2C support
#define HAS_TELEMETRY 1
#define HAS_SENSOR 0            // No environmental sensors on this board
#define HAS_CPU_SHUTDOWN 1

// Disable features not present on this hardware
#undef HAS_AXP192
#undef HAS_AXP2100
#undef HAS_RAK11200
#undef HELTEC_V2_0
#undef HELTEC_V3

// ---- Region Defaults (can be overridden via app) -------------------------
// UA_433: 433.0–434.7 MHz, 10% duty cycle, 10 dBm ERP limit
// Firmware enforces these automatically when region is set.
