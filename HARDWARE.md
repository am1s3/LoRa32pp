# Hardware — LoRa32++ V1.3

## ⚡ Power Architecture
The device is powered by a Li-Ion battery (18650).
1. The battery is connected to a **TP4056** charging module (with protection).
2. From the TP4056 output, the voltage (3.7–4.2V) is fed to an **MT3608** boost converter.
3. The MT3608 is set to output **5.0 V** and supplies power to the ESP32 DevKit `VIN` pin.
4. The built-in regulator on the ESP32 DevKit provides stable 3.3V for the ESP32 itself, the LoRa module, GPS, and display.

> ⚠️ **Important:** The MT3608 output voltage MUST be strictly 5.0V. Applying 5V directly to the 3.3V pin will burn the board.

## 📌 Pinout

### ESP32 (30-pin DevKit)
| Component | Component Pin | ESP32 Pin | Note |
| :--- | :--- | :--- | :--- |
| **LoRa (Ra-02)** | NSS (CS) | GPIO 18 | |
| | SCK | GPIO 5 | |
| | MOSI | GPIO 27 | |
| | MISO | GPIO 19 | |
| | RESET | GPIO 23 | |
| | DIO0 (IRQ) | GPIO 26 | |
| | DIO1 | GPIO 33 | |
| **GPS (ATGM336H)** | TX | GPIO 13 | *Fixed in V1.3* |
| | RX | GPIO 16 | |
| **OLED (SH1106)** | SDA | GPIO 21 | |
| | SCL | GPIO 22 | |
| **Buttons** | User Button 1 | GPIO 15 | Former Boot button. Pulled up to 3.3V (10k). |
| | User Button 2 | GPIO 4 | Former button on 39. Pulled up to 3.3V (10k). |

## 🛠 Component Specification (BOM)
*   **ESP32 DevKit**: 30-pin, standard form factor.
*   **LoRa**: Ai-Thinker Ra-02 (SX1278, 433MHz).
*   **GPS**: ATGM336H (UART).
*   **Display**: 1.3-inch OLED, SH1106 chip, I2C.
*   **Power**: MT3608, TP4056 (with DW01 protection).
*   **Antenna**: IPEX (U.FL) to SMA female pigtail + external 433 MHz antenna.
*   **Capacitors**: 470 µF (electrolytic), 22 µF, 100 nF (ceramic).

## 🖨 PCB Manufacturing
Gerber files for ordering the board are located in the `hardware/gerber/` folder.
*   Board size: 122 mm x 46 mm.
*   Layers: 2 layers (Top/Bottom).
*   Thickness: 1.6 mm.
