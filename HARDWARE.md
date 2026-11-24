# Hardware reference — LoRa32++ V1.3

Detailed wiring for the custom PCB build. For overview see [`README.md`](README.md); for parts see [`BOM.csv`](BOM.csv).

---

## Power tree

```
                        ┌─────────────────────────────────────────┐
                        │            18650 Li-Ion 3.0–4.2 V       │
                        └──────┬──────────────────────┬───────────┘
                               │ BAT+                 │ BAT−
                          [F1 1A fuse]                 │
                               │                       │
                ┌──────────────┴──────────┐            │
                │                         │            │
          [U6 TP4056 B+]            ┌─────┴─────┐      │
          [U6 TP4056 B−]────────────┤  BAT rail ├──────┘
                │                   └─────┬─────┘
           CHRG/STAT LEDs                 │
                                          │
                                    [U5 MT3608 VIN]
                                    trim → 3.30 V
                                          │
                                    ┌─────┴──────┐
                                    │  3.3 V RAIL│
                                    └─┬──┬──┬──┬─┘
                                      │  │  │  │
              ┌───────────────────────┘  │  │  └────────────────┐
              │                          │  │                   │
        [U1 ESP32 3V3 pin]    [U2 Ra-02 VCC]  [U4 OLED VCC]  [U3 NEO-7M VCC]
              │                          │                       │
        [internal LDO fed            [C 100nF]             [always on,
         from USB 5V when                                     no gating]
         plugged in]
```

### Net list

| Net name | Voltage | Connects |
|---|---|---|
| `BAT+` | 3.0–4.2 V | Cell +, fuse in, TP4056 B+, MT3608 VIN+, bulk caps |
| `BAT−` / `GND` | 0 V | Cell −, all module GND, divider bottom, cap negatives |
| `3V3` | 3.30 V regulated | MT3608 VOUT, ESP32 3V3, Ra-02 VCC, OLED VCC, NEO-7M VCC |
| `USB_5V` | 5 V | TP4056 IN+, USB-C connector VBUS |

> ESP32 DevKit powered through **3V3 pin**, not VIN. Onboard AMS1117 bypassed. When USB plugged, laptop's 5 V feeds DevKit's own LDO and USB-UART directly — independent of MT3608 rail.

---

## Module-by-module wiring

### U1 — ESP32-WROOM-32 (30-pin or 38-pin DevKit)

| DevKit pin | Goes to | Net / signal |
|---|---|---|
| 3V3 | 3.3 V rail | `3V3` |
| GND | common ground | `GND` |
| VIN | **leave unconnected** | (USB supplies when plugged) |
| GPIO5 | Ra-02 SCK | `LORA_SCK` |
| GPIO19 | Ra-02 MISO | `LORA_MISO` |
| GPIO27 | Ra-02 MOSI | `LORA_MOSI` |
| GPIO18 | Ra-02 NSS | `LORA_CS` |
| GPIO23 | Ra-02 RST | `LORA_RESET` |
| GPIO26 | Ra-02 DIO0 | `LORA_DIO0` |
| GPIO33 | Ra-02 DIO1 | `LORA_DIO1` |
| GPIO32 | Ra-02 DIO2 | `LORA_DIO2` (optional) |
| GPIO21 | OLED SDA | `I2C_SDA` |
| GPIO22 | OLED SCL | `I2C_SCL` |
| GPIO13 | NEO-7M TX | `GPS_RX_PIN` |
| GPIO14 | NEO-7M RX | `GPS_TX_PIN` |
| GPIO35 | divider midpoint | `BATTERY_PIN` |
| GPIO39 | USER button | `BUTTON_PIN` (+ ext 10 kΩ on PCB) |
| GPIO0 | BOOT button | onboard or external to GND |
| EN | RESET button | onboard or external to GND |
| GPIO25 | onboard LED | `LED_POWER` |

### U2 — Ra-02 SX1278 breakout

| Breakout pin | ESP32 GPIO | Required |
|---|---|---|
| SCK | 5 | ✅ |
| MISO | 19 | ✅ |
| MOSI | 27 | ✅ |
| NSS | 18 | ✅ |
| RST | 23 | ✅ |
| DIO0 | 26 | ✅ |
| **DIO1** | **33** | ✅ **must be wired** |
| DIO2 | 32 | optional |
| 3V3 | 3.3 V rail | ✅ never 5 V |
| GND | GND | ✅ |
| ANT (IPX) | AOC898 pigtail | seat vertically until click |

### U3 — NEO-7M GY-GPSU3-NEO

| Module pin | Connection | Note |
|---|---|---|
| TX | ESP32 GPIO13 | module transmits → ESP32 receives |
| RX | ESP32 GPIO14 | ESP32 transmits → module receives |
| VCC | 3.3 V rail | **always on in V1.3** |
| GND | GND | common |
| PPS | — | not used |
| IPEX | patch antenna | mount under top lid, skyward |

Constellations: GPS + GLONASS + QZSS + SBAS. Update rate up to 10 Hz (default 1 Hz in Meshtastic). Warm-start ~1 s if backup element populated [[12]].

### U4 — OLED 1.3" SH1106

| Pin | Connection |
|---|---|
| SDA | ESP32 GPIO21 |
| SCL | ESP32 GPIO22 |
| VCC | 3.3 V rail |
| GND | GND |

Address `0x3C`. Driver forced via `#define USE_SH1106` in variant.h [[19]].

### U5 — MT3608 boost

| Pin | Connection |
|---|---|
| VIN+ | BAT+ (after fuse) |
| VIN− | GND |
| VOUT+ | 3.3 V rail |
| VOUT− | GND |

**Trim procedure (before connecting load):**
1. Feed 5–7 V to VIN.
2. Measure VOUT, adjust trimmer to **3.30 ± 0.05 V**.
3. Apply ~100 mA load, confirm >3.25 V.
4. Then solder into circuit.

Decoupling: 10 µF + 100 nF on input, 22 µF + 470 µF LowESR + 100 nF on output (per schematic C1–C4).

### U6 — TP4056 Type-C with protection

| Pin | Connection |
|---|---|
| B+ | BAT+ (after fuse) |
| B− | BAT− |
| IN+ | USB-C VBUS (5 V) |
| IN− | USB GND |
| OUT+ | BAT+ (to load) |
| OUT− | BAT− |

Verify DW01 + FS8205A populated. Charge current set by PROG resistor (typically 1.2 kΩ → 1 A).

### Battery divider

```
BAT+ ──[ R4 100k ]──┬── ESP32 GPIO35
                    │
                 [ R5 100k ]   [ C_filter 100nF ]
                    │                  │
                   GND ────────────── GND
```

- Ratio 1:1 → `ADC_MULTIPLIER = 2.0`
- 4.20 V cell → 2.100 V at ADC
- Divider current: 21 µA @ 4.2 V

### Buttons

| Button | GPIO | Wiring | Pull |
|---|---|---|---|
| BOOT | 0 | to GND | internal or external |
| RESET | EN | to GND | internal RC |
| USER | 39 | to GND | **external 10 kΩ to 3.3 V** (R8/R9/R10 on schematic) |

GPIO39 is input-only with no internal pull. External resistor mandatory.

---

## PCB layout notes

### Layer stackup (2-layer)

| Layer | Content |
|---|---|
| **Top** | ESP32, Ra-02, OLED, buttons, silkscreen "LoRa32++ V1.3" |
| **Bottom** | MT3608, 18650 holder, bulk caps, divider resistors |

### Design rules

- Track width: **10 mil signal**, **25–30 mil power** (BAT+, 3V3, GND)
- Via: 24 mil outer / 12 mil inner
- Clearance: 8 mil minimum
- Ground pour: both layers, connected to GND net

### Mounting holes

4× M2 or M3 holes at corners for enclosure standoffs.

---

## Test points to expose

| Pad | Signal | Use |
|---|---|---|
| TP1 | 3V3 rail | verify MT3608 output under load |
| TP2 | BAT+ | cell voltage vs divider reading |
| TP3 | GND | reference |
| TP4 | GPIO35 | raw ADC input (~half of TP2) |
| TP5 | TX debug | ESP32 UART0 TX for serial log |

---

## Optional: restoring GPS power gating

If you want to retrofit hardware gating after assembly:

1. Cut the 3.3 V trace to NEO-7M VCC on PCB.
2. Solder SO-8 pad for IRF7342 (or any P-channel MOSFET in SOT-23).
3. Wire: Source → 3.3 V, Gate → free GPIO (e.g. GPIO15), Drain → NEO-7M VCC.
4. Add 100 kΩ gate-to-source pull-up.
5. In variant.h, uncomment:
   ```c
   #define PIN_GPS_EN 15
   #define GPS_EN_ACTIVE 0
   ```
6. Rebuild and flash.

This restores ~3–4 days of autonomous runtime at the cost of one SMD component and a bodge wire.

---

## What not to do

- ❌ Feed 5 V into 3.3 V rail (kills Ra-02, GPS, OLED)
- ❌ Leave DIO1 unwired (RX interrupt won't fire)
- ❌ Swap GPS TX/RX (module won't communicate)
- ❌ Seat U.FL at angle (lifts pad after ~30 cycles)
- ❌ Transmit without antenna
- ❌ Skip MT3608 trimming (factory default may be 5 V)
- ❌ Use ADC2 pins (GPIO0/2/4/12–15/25–27) for battery — unusable with Wi-Fi
