# LoRa32++ V1.3

> Custom Meshtastic node on a self-designed 2-layer PCB. ESP32 + Ra-02 (SX1278) + NEO-7M GPS + SH1106 OLED. Independent firmware via `PRIVATE_HW` variant.

**Status:** ✅ PCB designed in EasyEDA Pro. Schematic and layout complete. Ready for fabrication order.

![PCB 3D Preview](docs/images/pcb-3d-preview.png)

---

## What this is

LoRa32++ V1.3 is a **custom PCB implementation** of a Meshtastic node, evolved from the original perfboard sandwich prototype. It keeps the pin-compatible base of LILYGO T-LoRa32 V2.1 but adds GPS, bigger display, Type-C charging, and runs its own firmware variant instead of relying on stock binaries.

```
T-LoRa32 V2.1 pinout  +  NEO-7M GPS  +  SH1106 1.3"  +  Custom PCB  +  PRIVATE_HW variant  =  LoRa32++ V1.3
```

### Why "++" and why V1.3

| Feature | Stock T-LoRa32 V2.1 | LoRa32++ V1.3 |
|---|---|---|
| Form factor | Module on breakout | **Custom 2-layer PCB** |
| GNSS | ❌ none | ✅ **u-blox NEO-7M** (GPS + GLONASS + QZSS + SBAS) |
| Display | 0.96" SSD1306 | ✅ **1.3" SH1106** (more readable area) |
| Charging | Micro-USB or JST | ✅ **Type-C TP4056 module** |
| Power conversion | Direct LiPo / USB | ✅ **MT3608 trimmed to 3.3V**, single-stage |
| `LORA_DIO1` | Not routed on early revisions | ✅ **Wired to GPIO33** from day one |
| Firmware | Stock `tlora-v2-1-1_6` binary | ✅ **Custom `PRIVATE_HW` variant**, self-maintained |
| Battery sensing | 100k/100k divider | ✅ Same divider + local RC filter on ADC |
| Repairability | SMD, board-level replacement | ✅ **Modular connectors**, replaceable sub-assemblies |

### What changed from V1.0–V1.2 (perfboard era)

- ❌ Removed IRF7342 GPS power gating — simplified BOM, GPS stays powered always
- ❌ Removed three-layer sandwich construction — single PCB now
- ✅ Added Type-C charging (was micro-USB or bare JST)
- ✅ Moved from perfboard + wires to proper 2-layer PCB with ground pours
- ✅ Standalone `variant.h` — no dependency on upstream T-LoRa32 binaries

---

## Documentation

- [`BOM.csv`](BOM.csv) — full parts list with sources and prices
- [`HARDWARE.md`](HARDWARE.md) — wiring tables, power tree, PCB layer description, test points
- [`variants/esp32/lora32pp/variant.h`](variants/esp32/lora32pp/variant.h) — custom firmware variant
- [`variants/esp32/lora32pp/platformio.ini`](variants/esp32/lora32pp/platformio.ini) — PlatformIO build environment

The sections below summarise the essentials; the linked files are the source of truth.

---

## Hardware summary

Full bill of materials in [`BOM.csv`](BOM.csv). Headline components:

| # | Component | Part | Qty |
|---|---|---|---|
| 1 | MCU | ESP32-WROOM-32 (30-pin or 38-pin DevKit footprint) | 1 |
| 2 | LoRa | Ai-Thinker Ra-02 (SX1278, 433 MHz) on breakout | 1 |
| 3 | GNSS | **u-blox NEO-7M**, GY-GPSU3-NEO + IPEX patch antenna | 1 |
| 4 | Display | OLED 1.3" 128×64, **SH1106**, I²C | 1 |
| 5 | Boost | MT3608 module **with trimmer** | 1 |
| 6 | Charger | **TP4056 Type-C module** with protection | 1 |
| 7 | Cell | 18650 Li-Ion, flat-top unprotected | 1 |
| 8 | Antenna | 433 MHz 3 dBi whip, RP-SMA + IPX pigtail 15 cm | 1 |
| 9 | PCB | **Custom 2-layer**, ~80×40 mm, designed in EasyEDA Pro | 1 |

> ⚠️ **No load switch in V1.3.** IRF7342 was removed to simplify assembly. GPS draws ~24–50 mA during fixes and ~22 mA tracking continuously. If you need hardware gating, see [`HARDWARE.md`](HARDWARE.md#optional-restoring-gps-power-gating) for the retrofit procedure.

---

## Pinout summary

Base is the stock `tlora-v2-1-1_6` SPI mapping. Full per-module wiring in [`HARDWARE.md`](HARDWARE.md#module-by-module-wiring).

### LoRa — Ra-02 / SX1278

| Module pin | ESP32 GPIO | Define | Required |
|---|---|---|---|
| SCK | **5** | `LORA_SCK` | ✅ |
| MISO | **19** | `LORA_MISO` | ✅ |
| MOSI | **27** | `LORA_MOSI` | ✅ |
| NSS / CS | **18** | `LORA_CS` | ✅ |
| RST | **23** | `LORA_RESET` | ✅ |
| DIO0 | **26** | `LORA_DIO0` | ✅ primary IRQ |
| **DIO1** | **33** | `LORA_DIO1` | ✅ **must be wired** |
| DIO2 | 32 | `LORA_DIO2` | optional |
| 3V3 | 3.3 V rail | — | **never 5 V** |

### Display — OLED SH1106 (I²C)

| Pin | ESP32 GPIO | Define |
|---|---|---|
| SDA | **21** | `I2C_SDA` |
| SCL | **22** | `I2C_SCL` |

Address `0x3C`. Controller forced to SH1106 via `#define USE_SH1106`.

### GNSS — NEO-7M (UART1)

| Module pin | ESP32 GPIO | Define | Direction |
|---|---|---|---|
| **TX** | **13** | `GPS_RX_PIN` | module → ESP32 |
| **RX** | **14** | `GPS_TX_PIN` | ESP32 → module |
| VCC | 3.3 V rail (always on) | — | no gating in V1.3 |

> NEO-7M supports GPS + GLONASS + QZSS + SBAS. Onboard backup element retains ephemeris for ~1 s warm starts after brief power loss [[12]].

### Battery sensing

```
BAT+ ──[ 100k ]──┬── GPIO35 (ADC1_CH7)
                 │
              [ 100k ]   [ 100nF ]
                 │          │
                GND ───────GND
```

| Parameter | Value |
|---|---|
| `BATTERY_PIN` | 35 |
| `ADC_CHANNEL` | `ADC_CHANNEL_7` |
| `ADC_MULTIPLIER` | **2.0** |
| Divider current @ 4.2 V | 21 µA |

### Buttons

| Button | ESP32 GPIO | Wiring |
|---|---|---|
| BOOT | 0 | onboard or external to GND |
| RESET | EN | onboard or external to GND |
| **USER** | **39** | to GND + **external 10 kΩ to 3.3 V** (already on PCB) |

---

## Power architecture

```
[18650] ──[fuse 1A]──┬──> [TP4056 B+/B-]
                     │
             [100k/100k + 100nF] ──> GPIO35

[TP4056 OUT+] ──> [MT3608 VIN] ──(trim to 3.30 V)──> 3.3 V RAIL
                                                       │
                                    ┌──────────────────┼──────────────────┐
                                    │                  │                  │
                              [ESP32 3V3]         [Ra-02 VCC]        [NEO-7M VCC]
                                    │                                     │
                              [OLED VCC]                            [always on]
```

**Single-stage conversion.** MT3608 trimmed to **3.30 V** direct. No AMS1117. Efficiency ~90% vs 59% for two-stage.

**Full cell utilisation.** MT3608 operates down to ~2 V input → cell drained to TP4056 cutoff at 3.0 V instead of stalling at 3.8 V.

**Type-C charging.** TP4056 module accepts USB-C 5 V. Charge current set by PROG resistor (typically 1 A). Protection ICs (DW01 + FS8205A) must be present on the module.

---

## Firmware

### Build from source

This board uses `PRIVATE_HW` model. It will **not** appear in the official Meshtastic Web Flasher. You must compile yourself.

```bash
git clone https://github.com/meshtastic/firmware.git
cd firmware
git submodule update --init

# Copy variant files
mkdir -p variants/esp32/lora32pp
cp path/to/your/variant.h variants/esp32/lora32pp/
cp path/to/your/platformio.ini variants/esp32/lora32pp/

# Build and flash
pio run --environment lora32pp
pio run --environment lora32pp --target upload
pio device monitor
```

Expected boot log:

```
RF95 init result 0
Frequency set to 433.xxx
Power output set to 10
SH1106 found at address 0x3C
U-blox 7 detected
Use analog input 35 for battery level
```

### Pre-built binaries

Once you've built successfully, upload the `.bin` to GitHub Releases. Users can then flash via:

```bash
esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 921600 \
  write_flash -z 0x1000 bootloader.bin \
  0x8000 partitions.bin \
  0xe000 boot_app0.bin \
  0x10000 firmware-lora32pp.bin
```

Or use the [Meshtastic Web Flasher](https://flasher.meshtastic.org) with "Custom firmware" option pointing to your release URL.

---

## Configuration

Region **UA_433**: 433.0–434.7 MHz, **10 % duty cycle, 10 dBm limit**. Firmware enforces this automatically.

Suggested settings:

| Setting | Value | Why |
|---|---|---|
| Region | `UA_433` | local band |
| Modem preset | `LONG_FAST` | SF11/BW250, ~330 ms per 20-byte packet |
| Role | `CLIENT` / `CLIENT_MUTE` | no rebroadcast load |
| TX power | 10 dBm | region clamps anyway |
| `position.gps_update_interval` | 3600 s | one fix per hour (GPS always powered) |
| `display.screen_on_secs` | 10 | panel is a real consumer |
| `power.is_power_saving` | `true` | light sleep between events |

---

## Bring-up checklist

1. **Trim MT3608 before connecting anything.** Feed 5–7 V in, measure `VOUT`, adjust to **3.30 ± 0.05 V**. Load with ~100 mA, confirm it holds.
2. **Verify TP4056 has protection.** Look for DW01 + FS8205A next to main IC.
3. **Continuity-check antenna.** Centre conductor end-to-end ≈ 0 Ω; centre-to-shell = open.
4. **Confirm DIO1 wired.** GPIO33 ← Ra-02 DIO1. Without this, RX interrupt doesn't fire.
5. **Check GPS TX/RX orientation.** Module TX → ESP32 GPIO13. Module RX → ESP32 GPIO14.
6. **Seat U.FL straight down until click.** Rated for ~30 mating cycles.
7. **Always attach antenna before transmitting.**
8. First flash over USB with battery disconnected.

---

## Known limitations

- **No GPS power gating.** NEO-7M draws ~22–50 mA continuously. Autonomous runtime is ~3–4 days on 2400 mAh cell vs ~7 days with gating. Retrofit possible — see HARDWARE.md.
- **Size.** Custom PCB is ~80×40 mm. Smaller than perfboard sandwich but larger than T-Beam v1.2 (100×33 mm).
- **USB-UART overhead.** DevKit's CP2102/CH340 draws ~10–20 mA when powered. Factory boards gate this; here it's the price of convenient flashing.
- **No TCXO** on Ra-02 — frequency drifts with temperature.
- **Not in Web Flasher** by default. `PRIVATE_HW` builds are community-maintained.

---

## Roadmap

- [x] Component selection and BOM
- [x] Pinout compatible with stock `tlora-v2-1-1_6` SPI mapping
- [x] Power path design (single-stage 3.3 V)
- [x] **Custom PCB designed in EasyEDA Pro**
- [x] **Standalone variant.h for PRIVATE_HW**
- [ ] PCB fabrication order (JLCPCB)
- [ ] Assembly and bring-up
- [ ] Measured current profiles per role/interval
- [ ] 3D-printable enclosure (STEP + STL)
- [ ] Pre-built firmware releases on GitHub
- [ ] Optional: restore GPS gating via external MOSFET retrofit

---

## Safety and legality

- 18650 delivers 15–20 A into short. **Fit the fuse.**
- Never transmit without antenna attached.
- Observe regional band plan. `UA_433` limits ERP to 10 mW, 10 % duty cycle.
- Li-Ion cells hate heat. Don't charge above 1 A without heatsinking.

---

## References

- [Meshtastic firmware](https://github.com/meshtastic/firmware) — variant sources, `architecture.h`
- [Meshtastic build docs](https://meshtastic.org/docs/development/firmware/build/) — `PRIVATE_HW` policy
- [u-blox NEO-7M datasheet](https://content.u-blox.com/sites/default/files/products/documents/NEO-7_DataSheet_%28UBX-13003830%29.pdf) [[12]]
- [SH1106 I2C address note](https://controllerstech.com/arduino-sh1106-oled-display-tutorial/) [[19]]
- [IRF7342 datasheet](https://www.infineon.com/assets/row/public/documents/non-assigned/49/irf7342pbf-1.pdf) (for optional retrofit)

---

## License

Hardware documentation: **CC BY-SA 4.0**.  
Firmware variant files derived from Meshtastic inherit **GPL-3.0**.

---

## Contributing

Built one? Found a mistake? Open an issue or PR — especially with measured current draw, real-world range figures, or enclosure designs. Corrections backed by a multimeter beat corrections backed by opinion.
