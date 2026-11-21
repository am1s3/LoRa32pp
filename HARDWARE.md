# Hardware reference — LoRa32++

Detailed wiring for the sandwich build. For the high-level overview see [`README.md`](README.md); for parts see [`BOM.csv`](BOM.csv).

---

## Power tree

```
                        ┌─────────────────────────────────────────┐
                        │            18650 Li-Ion 3.0–4.2 V       │
                        └──────┬──────────────────────┬───────────┘
                               │ BAT+                 │ BAT−
                          [F1 1A fuse]                 │
                               │                       │
                          [SW1 SS12D06]                │
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
        [U1 ESP32 3V3 pin]    [U2 Ra-02 VCC]  [U4 OLED VCC]  [U7 IRF7342 S1]
              │                          │                       │
        [internal LDO fed            [C3 100nF]             [D1]─┴─→ [U3 NEO-6M VCC]
         from USB 5V when
         plugged in]
```

### Net list

| Net name | Voltage | Connects |
|---|---|---|
| `BAT+` | 3.0–4.2 V | Cell +, fuse in, TP4056 B+, MT3608 VIN+, C_470U bulk |
| `BAT−` / `GND` | 0 V | Cell −, all module GND, divider bottom, cap negatives |
| `3V3` | 3.30 V regulated | MT3608 VOUT, ESP32 3V3 pin, Ra-02 VCC, OLED VCC, IRF7342 S1/S2, gate pull-up tops |
| `GPS_VCC` | 3.30 V switched | IRF7342 D1 (pins 7+8), NEO-6M VCC |

> The ESP32 DevKit is powered through its **3V3 pin**, not VIN. The onboard AMS1117 is bypassed. When USB is plugged, the laptop's 5 V feeds the DevKit's own LDO and USB-UART chip directly — that path is independent of the MT3608 rail.

---

## Module-by-module wiring

### U1 — ESP32 DevKit V1

| DevKit pin | Goes to | Net / signal |
|---|---|---|
| 3V3 | 3.3 V rail | `3V3` |
| GND | common ground | `GND` |
| VIN | **leave unconnected** | (USB supplies it when plugged) |
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
| GPIO13 | NEO-6M TX | `GPS_RX_PIN` |
| GPIO14 | NEO-6M RX | `GPS_TX_PIN` |
| GPIO15 | IRF7342 G1 (pin 2) | `PIN_GPS_EN` |
| GPIO35 | divider midpoint | `BATTERY_PIN` |
| GPIO39 | USER button | `BUTTON_PIN` (+ ext 10 kΩ to 3V3) |
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

### U3 — NEO-6M GY-GPS6MV2

| Module pin | Connection | Note |
|---|---|---|
| TX | ESP32 GPIO13 | module transmits → ESP32 receives |
| RX | ESP32 GPIO14 | ESP32 transmits → module receives |
| VCC | IRF7342 D1 (pins 7+8) | switched 3.3 V |
| GND | GND | common |
| PPS | — | not used in this build |
| IPEX | patch antenna | mount under top lid, skyward |

### U4 — OLED 1.3" SH1106

| Pin | Connection |
|---|---|
| SDA | ESP32 GPIO21 |
| SCL | ESP32 GPIO22 |
| VCC | 3.3 V rail |
| GND | GND |

Address `0x3C`. Controller auto-detected as SH1106; force with `#define USE_SH1106` if the image renders shifted.

### U5 — MT3608 boost

| Pin | Connection |
|---|---|
| VIN+ | BAT+ (after fuse + switch) |
| VIN− | GND |
| VOUT+ | 3.3 V rail |
| VOUT− | GND |

**Trim procedure (do before connecting the load):**
1. Feed 5–7 V to VIN from a bench supply or power bank.
2. Measure VOUT with a multimeter.
3. Turn the blue trimmer until VOUT reads **3.30 ± 0.05 V**.
4. Apply ~100 mA load, confirm VOUT stays above 3.25 V.
5. Only then solder into the circuit.

Decoupling: 10 µF + 100 nF on input, 10 µF + 470 µF LowESR + 100 nF on output.

### U6 — TP4056 with protection

| Pin | Connection |
|---|---|
| B+ | BAT+ (after fuse) |
| B− | BAT− |
| IN+ | USB 5 V (charging input) |
| IN− | USB GND |
| OUT+ | BAT+ (to MT3608 / load) |
| OUT− | BAT− |

Verify DW01 + FS8205A are populated next to the main TP4056 IC. A bare TP4056 has no over-discharge cutoff — the cell can be driven below 2.5 V and die.

Charge current is set by the PROG resistor (typically 1.2 kΩ → 1 A). At 1 A the module gets warm; ensure airflow or reduce to 500 mA (2.4 kΩ) for enclosed builds.

### U7 — IRF7342 dual P-FET

Top view, SO-8:

```
        ┌───∪───┐
  S1 ──1│       │8── D1
  G1 ──2│IRF7342│7── D1
  S2 ──3│       │6── D2
  G2 ──4│       │5── D2
        └───────┘
```

| Pin | Name | Connection |
|---|---|---|
| 1 | S1 | 3.3 V rail |
| 2 | G1 | ESP32 GPIO15 **+ 100 kΩ to pin 1** |
| 3 | S2 | 3.3 V rail |
| 4 | G2 | tie to pin 3 (3.3 V) — never float |
| 5, 6 | D2 | unused (leave open) |
| 7, 8 | D1 | NEO-6M VCC (join pins 7+8 on the pad for current sharing) |

Logic: G1 = LOW → channel 1 ON → GPS powered. G1 = HIGH (or floating, pulled up) → OFF. Firmware drives this via `PIN_GPS_EN 15` + `GPS_EN_ACTIVE 0`.

### Battery divider

```
BAT+ ──[ R_DIV 100k ]──┬── ESP32 GPIO35
                       │
                    [ R_DIV 100k ]   [ C_CER 100nF ]
                       │                  │
                      GND ────────────── GND
```

- Ratio 1:1 → `ADC_MULTIPLIER = 2.0`
- 4.20 V cell → 2.100 V at ADC (inside linear region)
- Divider current: 21 µA @ 4.2 V
- The 100 nF across the lower leg filters MT3608 ripple before sampling

### Buttons

| Button | GPIO | Wiring | Pull |
|---|---|---|---|
| BOOT | 0 | switch to GND | internal (`BUTTON_NEED_PULLUP`) |
| RESET | EN | switch to GND | internal RC on DevKit |
| USER | 39 | switch to GND | **external 10 kΩ to 3.3 V** |

GPIO39 is input-only with no internal pull resistors. The external 10 kΩ is mandatory; without it the pin floats and generates phantom presses.

---

## Inter-layer connections (sandwich)

Use paired PLS (male, on the lower board) / PBS (female, on the upper board) headers so layers mate like Arduino shields.

### Layer 1 (power) → Layer 2 (logic)

| Signal | Count | Notes |
|---|---|---|
| 3V3 | 2 | redundant for current capacity |
| GND | 2 | redundant, low impedance return |
| BAT_SENSE | 1 | from divider to GPIO35 (routed through) |
| **Total** | **5 pins** | one 1×5 or 1×6 header |

### Layer 2 (logic) → Layer 3 (radio/GPS/display)

| Signal | Count | Destination |
|---|---|---|
| LORA_SCK / MISO / MOSI / CS / RST / DIO0 / DIO1 | 7 | Ra-02 |
| GPS_RX / GPS_TX | 2 | NEO-6M |
| GPS_EN | 1 | IRF7342 G1 (routed from layer 2) |
| I2C_SDA / I2C_SCL | 2 | OLED |
| 3V3 | 2 | Ra-02, OLED, IRF7342 sources |
| GND | 2 | common return |
| USER_BTN | 1 | tactile on layer 3 → GPIO39 |
| **Total** | **17 pins** | two 1×10 headers or one 2×10 |

Recommended: **2×10 PBS/PLS pair** between layers 2–3, **1×6 pair** between layers 1–2. Keeps the harness to two connectors instead of 17 loose wires.

---

## Mechanical stack-up

```
   ┌─────────────────────────────────────────────┐  ← top lid (OLED window + button holes)
   │  Layer 3: Ra-02, NEO-6M, OLED, USER btn     │  +8 mm components
   │  perfboard 60×80                             │  1.6 mm
   ├─────────────────────────────────────────────┤
   │  M2×15 nylon standoffs (×4)                  │  15 mm gap
   ├─────────────────────────────────────────────┤
   │  Layer 2: ESP32 DevKit, MT3608, IRF7342      │  +12 mm (DevKit on headers)
   │  perfboard 60×80                             │  1.6 mm
   ├─────────────────────────────────────────────┤
   │  M2×20 brass standoffs (×4)                  │  20 mm gap (cell clearance)
   ├─────────────────────────────────────────────┤
   │  Layer 1: 18650 holder, TP4056, fuse, SW1    │  +18 mm (cell)
   │  perfboard 60×80                             │  1.6 mm
   └─────────────────────────────────────────────┘  ← bottom (flat, feet optional)

   Total internal height: 1.6×3 + 15 + 20 + 18 = 59.8 mm
   With 1.5 mm walls top+bottom:                  ≈ 63 mm
```

> Recheck against your actual component heights before cutting the enclosure. The 12 mm DevKit-on-headers figure assumes standard 8.5 mm female headers + 3.5 mm module PCB; measure yours.

Envelope target: **80 × 60 × ~45 mm** if the cell sits in the layer-1 gap rather than adding its full diameter on top. Confirm during first fit.

---

## Test points to expose

Bring these out to labelled pads or pin headers for debugging:

| Pad | Signal | Use |
|---|---|---|
| TP1 | 3V3 rail | verify MT3608 output under load |
| TP2 | BAT+ | cell voltage vs divider reading |
| TP3 | GPS_VCC | confirm IRF7342 gating works |
| TP4 | GND | reference |
| TP5 | GPIO35 | raw ADC input (should be ~half of TP2) |
| TP6 | TX debug | ESP32 UART0 TX for serial log |

---

## What not to do

- ❌ Feed 5 V into the 3.3 V rail (kills Ra-02, GPS, OLED instantly)
- ❌ Use an N-channel FET for the high-side GPS switch
- ❌ Leave IRF7342 gates floating (no pull-ups)
- ❌ Seat U.FL at an angle (lifts the pad after ~30 cycles)
- ❌ Transmit without the antenna attached
- ❌ Route GPS RX/TX backwards (TX↔TX, RX↔RX)
- ❌ Put the divider on an ADC2 pin (GPIO0/2/4/12–15/25–27) — unusable with Wi-Fi
- ❌ Skip per-module decoupling caps (brownout resets on every TX)
  
