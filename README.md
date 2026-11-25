# LoRa32++ (V1.3)

[![Version](https://img.shields.io/badge/version-1.3-blue.svg)]()
[![Platform](https://img.shields.io/badge/platform-ESP32-green.svg)]()

**LoRa32++** is a compact ESP32-based device with LoRa, GPS, and OLED display support. The project is intended for creating portable trackers, communication nodes (e.g., Meshtastic), or standalone sensors.

In version **V1.3**, the project was moved from breadboards to a custom printed circuit board (PCB), making the device more compact, reliable, and ready for everyday use.

## ✨ Key Features
*   **Microcontroller:** ESP32 (a ready-made 30-pin DevKit module is used for ease of assembly and replacement).
*   **Communication:** Ai-Thinker Ra-02 LoRa module (SX1278, 433 MHz) with an external antenna via an IPEX-SMA pigtail.
*   **Navigation:** Modern ATGM336H GPS module (GPS + BeiDou).
*   **Display:** 1.3" SH1106 OLED (I2C).
*   **Power:** 18650 Li-Ion battery with a TP4056 charging module and an MT3608 boost converter (up to 5V).
*   **Interface:** 2 user buttons for control.

## 📸 Appearance
*(Insert photos from the docs folder here)*
![PCB Top](docs/pcb_v1.3_top.png)
![PCB Bottom](docs/pcb_v1.3_bottom.png)

## 📂 Project Structure
*   `firmware/` — firmware source code (Arduino C++).
*   `hardware/` — EasyEDA project files (schematic, PCB, Gerber files).
*   `docs/` — documentation and photos.
*   `HARDWARE.md` — detailed hardware and pinout description.

## 🚀 Quick Start
1. Install PlatformIO (recommended) or Arduino IDE with ESP32 support.
2. Clone the repository.
3. Open the project in your development environment and upload the firmware to the ESP32.
4. Connect the antenna to the SMA connector **before applying power**.
