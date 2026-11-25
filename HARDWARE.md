# Hardware Architecture & Pinout — LoRa32++ V1.3

## ⚡ Power Architecture
The device is powered by a standard 18650 Li-Ion battery.
1. The battery is connected to the **TP4056** charging module (with DW01 protection).
2. The output from the TP4056 (3.7V - 4.2V) is fed into a **MT3608** boost converter.
3. The MT3608 is tuned to output exactly **5.0V** and supplies power to the `VIN` pin of the ESP32 DevKit.
4. The onboard LDO regulator of the ESP32 DevKit provides a stable 3.3V for the ESP32, LoRa module, GPS, and OLED.

> ⚠️ **WARNING:** Do not connect 5V directly to the 3.3V pin of the ESP32. Ensure MT3608 is set to 5.0V using a multimeter *before* connecting it to the DevKit.

## 📌 Pinout

### ESP32 (30-pin DevKit)
| Component | Pin Name | ESP32 GPIO | Note |
| :--- | :--- | :--- | :--- |
| **LoRa (Ra-02)** | NSS (CS) | GPIO 18 | SPI Chip Select |
| | SCK | GPIO 5 | SPI Clock |
| | MOSI | GPIO 27 | SPI MOSI |
| | MISO | GPIO 19 | SPI MISO |
| | RESET | GPIO 23 | LoRa Reset |
| | DIO0 (IRQ) | GPIO 26 | Interrupt Request |
| | DIO1 | GPIO 33 | Secondary Interrupt |
| **GPS (ATGM336H)**| TX | GPIO 13 | ESP32 RX (Serial2) |
| | RX | GPIO 16 | ESP32 TX (Serial2) |
| **OLED (SH1106)**| SDA | GPIO 21 | I2C Data |
| | SCL | GPIO 22 | I2C Clock |
| **Buttons** | User Button 1 | GPIO 15 | Pull-up 10k to 3.3V (Former Boot button) |
| | User Button 2 | GPIO 4 | Pull-up 10k to 3.3V |

## 🛠 Bill of Materials (BOM)
*   **1x** ESP32 DevKit (30-pin)
*   **1x** Ai-Thinker Ra-02 (SX1278, 433MHz)
*   **1x** ATGM336H GPS Module
*   **1x** OLED 1.3" Display (SH1106, I2C)
*   **1x** TP4056 Charging Module (with protection)
*   **1x** MT3608 Boost Converter
*   **1x** IPEX (U.FL) to SMA pigtail
*   **1x** 433MHz SMA Antenna
*   **2x** Tactile Push Buttons
*   **Capacitors:** 470µF (Electrolytic), 22µF, 100nF (Ceramic)
*   **Resistors:** 2x 10kΩ (for buttons)

## 🖨 PCB Specifications
*   **Dimensions:** ~122mm x 46mm
*   **Layers:** 2 (Top/Bottom)
*   **Thickness:** 1.6mm
*   *Gerber files are located in `hardware/gerber/`.*