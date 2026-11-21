# LoRa32++

> DIY Meshtastic node built on the LILYGO T-LoRa32 V2.1 pinout, with GPS, hardware power gating and a serviceable sandwich construction.

**Status:** 🚧 Prototype / documentation stage. Hardware BOM and pinout are finalised, assembly in progress. 3D-printable enclosure coming later.

---

## What this is

LoRa32++ is not a new PCB design (yet). It is a **hand-soldered node on perfboard**, wired to be pin-compatible with the stock Meshtastic `tlora-v2-1-1_6` firmware target, plus the peripherals that board never had.

The idea: take the well-tested T-LoRa32 schematic base, add everything a T-Beam has, and keep every module socketed so nothing is unrepairable.

```
T-LoRa32 V2.1  +  GPS  +  18650 power path  +  bigger OLED  +  fixed DIO1  =  LoRa32++
```

### Why "++"

| Improvement | Stock T-LoRa32 V2.1 | LoRa32++ |
|---|---|---|
| GNSS | ❌ none | ✅ u-blox NEO-6M + patch antenna |
| Battery | JST connector for flat LiPo only | ✅ 18650 holder + TP4056 charge/protection |
| Power conversion | direct LiPo / USB | ✅ MT3608 trimmed to 3.3 V, single-stage |
| Display | 0.96" SSD1306 | ✅ 1.3" SH1106 (more readable area) |
| `LORA_DIO1` | not routed on early revisions | ✅ wired to GPIO33 from day one |
| Peripheral power gating | ❌ none | ✅ IRF7342 dual P-MOSFET, firmware-driven |
| Battery sensing | 100k/100k divider | ✅ same divider + local RC filter on ADC |
| Repairability | SMD, board-level replacement | ✅ every module on headers |

---

## Hardware

### Bill of materials

| # | Component | Part / marking | Qty | Notes |
|---|---|---|---|---|
| 1 | MCU board | ESP32-WROOM-32 DevKit V1 (38-pin) | 1 | Must have onboard USB-UART + LDO |
| 2 | LoRa transceiver | Ai-Thinker Ra-02 (SX1278, 433 MHz) **on breakout** | 1 | Breakout exposes DIO0–DIO5, 2.54 mm pitch |
| 3 | GNSS | u-blox NEO-6M, GY-GPS6MV2 + IPEX patch antenna | 1 | GPS-only constellation |
| 4 | Display | OLED 1.3" 128×64, SH1106, I²C | 1 | Address `0x3C` |
| 5 | Boost converter | MT3608 module **with trimmer** | 1 | Set output to exactly 3.30 V |
| 6 | Charger | TP4056 **with protection** (DW01 + FS8205A) | 1 | Verify protection ICs are present |
| 7 | Cell | 18650 Li-Ion, flat-top unprotected | 1 | e.g. EVE 2400 mAh |
| 8 | Cell holder | 1×18650 SMD holder | 1 | ~77 × 20 × 18 mm |
| 9 | Load switch | IRF7342TRPBF (dual P-channel, SO-8) | 1 | −55 V, −3.4 A, R<sub>DS(on)</sub> 0.17 Ω @ V<sub>GS</sub>=−4.5 V |
| 10 | Antenna | 433 MHz 3 dBi whip, RP-SMA + IPX pigtail 15 cm | 1 | 48 mm whip, vertical polarisation |
| 11 | Power switch | SS12D06 slide switch | 1 | Physically breaks battery rail |
| 12 | Fuse | 1 A + holder | 1 | Between cell + and everything else |
| 13 | Perfboard | 60 × 80 mm double-sided, 2.54 mm grid | 3 | Sandwich layers |
| 14 | Standoffs | M2 × 20 brass | 4 | Layer 1 → 2 gap (cell clearance) |
| 15 | Standoffs | M2 × 15 nylon | 4 | Layer 2 → 3 gap (DevKit clearance) |
| 16 | Screws | M2 × 5 ISO 7380-1 | ~12 | |
| 17 | Headers | PLS + PBS 2.54 mm | 4 rows | Inter-layer connectors + module sockets |
| 18 | Tactile switches | 6 × 6 × 5 mm | 3 | BOOT, RESET, USER |

#### Passives

| Value | Package | Qty | Purpose |
|---|---|---|---|
| 100 kΩ ±1% | axial 0.25 W | 2 | Battery divider (R1 = R2) |
| 100 kΩ ±1% | axial 0.25 W | 2 | MOSFET gate pull-ups (gate → source) |
| 10 kΩ | axial 0.25 W | 1 | External pull-up for USER button (GPIO39 has no internal pull) |
| 100 nF X7R 50 V | ceramic | 8 | Decoupling: AMS/MT input+output, each module VCC, ADC filter |
| 10 µF 50 V LowESR | electrolytic | 2 | MT3608 input/output bulk |
| 470 µF 16 V LowESR | electrolytic | 2 | Rail bulk on 3.3 V and battery side |

> ⚠️ **Do not substitute the load switch with an N-channel part** (2N7002, BSS138). An N-FET in the high side needs V<sub>G</sub> > V<sub>S</sub> + V<sub>th</sub> ≈ 5.8 V, which a 3.3 V GPIO cannot produce. It will sit half-open and drop 1.5 V at 45 mA. Use P-channel only.

---

## Pinout

Base is the stock `tlora-v2-1-1_6` variant. SPI pins match both that variant and the ESP32 architecture defaults (`LORA_SCK 5`, `LORA_MISO 19`, `LORA_MOSI 27`, `LORA_CS 18`).

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
| DIO2 | 32 | `LORA_DIO2` | optional, unused on SX127x |
| 3V3 | 3.3 V rail | — | **never 5 V** |
| GND | GND | — | |

> `LORA_DIO1` is not cosmetic. The Meshtastic RX readout task is woken by this interrupt. Leaving it unconnected forces polling fallback and costs packets.

### Display — OLED SH1106 (I²C)

| Module pin | ESP32 GPIO | Define |
|---|---|---|
| SDA | **21** | `I2C_SDA` |
| SCL | **22** | `I2C_SCL` |
| VCC | 3.3 V rail | — |
| GND | GND | — |

No `RESET_OLED` — this panel has no reset pin broken out. No `VEXT_ENABLE` — display is fed directly from the 3.3 V rail.

### GNSS — NEO-6M (UART1)

| Module pin | ESP32 GPIO | Define | Direction |
|---|---|---|---|
| **TX** | **13** | `GPS_RX_PIN` | module → ESP32 |
| **RX** | **14** | `GPS_TX_PIN` | ESP32 → module |
| VCC | IRF7342 drain | — | switched rail |
| GND | GND | — | common |

> Naming follows Meshtastic convention: `GPS_RX_PIN` is the pin **the ESP32 receives on**, so it goes to the module's TX. Getting this backwards is the single most common GPS wiring mistake.

### Load switch — IRF7342 (SO-8, dual P-FET)

Pinout per Infineon datasheet, top view:

```
        ┌───∪───┐
  S1 ──1│       │8── D1
  G1 ──2│IRF7342│7── D1
  S2 ──3│       │6── D2
  G2 ──4│       │5── D2
        └───────┘
```

| Function | Pins | Connection |
|---|---|---|
| **Channel 1 — GPS rail** | S1 = 1, G1 = 2, D1 = 7+8 | S1 → 3.3 V, G1 → GPIO15, D1 → NEO-6M VCC |
| Gate pull-up | — | 100 kΩ between G1 (2) and S1 (1) |
| **Channel 2 — spare** | S2 = 3, G2 = 4, D2 = 5+6 | Tie G2 to S2 and to 3.3 V so it never floats |

Gate logic: **LOW = ON**, HIGH = OFF. Hence `GPS_EN_ACTIVE 0`.

The gate pull-up is mandatory. Without it the gate floats during reset/flashing and the receiver randomly power-cycles.

> Do **not** gate the OLED with channel 2. Meshtastic scans I²C at boot; if the panel is absent on the bus, `screen_found.port == NO_I2C` and the Screen object is never created. You lose the display entirely.

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
| `BATTERY_SENSE_SAMPLES` | 30 |
| Divider current @ 4.2 V | 21 µA |
| ADC voltage @ 4.2 V | **2.100 V** — inside the linear region |

Why 100k/100k and not something else:

| R1 / R2 | V<sub>ADC</sub> @ 4.2 V | Multiplier | Verdict |
|---|---|---|---|
| **100k / 100k** | **2.100 V** | **2.0** | ✅ matches stock variant, no calibration |
| 100k / 220k | 2.888 V | 1.4545 | ❌ outside ADC linear range (~2.45–2.6 V ceiling) |
| 220k / 100k | 1.312 V | 3.2 | ⚠️ linear but wastes ADC span |

GPIO35 is input-only with no internal pull resistors — ideal for ADC. **Use ADC1 (GPIO32–39) only.** ADC2 channels are unusable while Wi-Fi is active.

If readings drift, trim `config.power.adc_multiplier_override` from the phone app — no reflash needed.

### Buttons

| Button | ESP32 GPIO | Wiring |
|---|---|---|
| BOOT | 0 | onboard, or external switch to GND |
| RESET | EN | onboard, or external switch to GND |
| **USER** | **39** | switch to GND + **external 10 kΩ pull-up to 3.3 V** |

> GPIO34–39 have **no internal pull-ups**. `BUTTON_NEED_PULLUP` alone will not work on GPIO39 — the external resistor is required. If you prefer an internal-pull pin, use GPIO0 (`BUTTON_PIN 0` + `BUTTON_NEED_PULLUP`) as T-LoRa V1 does, accepting that BOOT doubles as USER.

### Reserved / do not touch

| GPIO | Reason |
|---|---|
| 6–11 | SPI flash — using these bricks the board |
| 12 | Strapping (MTDI). HIGH at boot → VDD_SDIO = 1.8 V → flash won't start |
| 2 | Strapping. Must be LOW/floating during flashing |
| 25 | `LED_POWER` in stock variant (DevKit LED) |
| 34, 36 | Free, input-only, available for future ADC/sensors |

---

## Power architecture

```
[18650] ──[fuse 1A]──[SS12D06]──┬──> [TP4056 B+/B-]
                                │
                        [100k/100k + 100nF] ──> GPIO35
                                
[TP4056 OUT+] ──> [MT3608 IN] ──(trim to 3.30 V)──> 3.3 V RAIL
                                                       │
                                    ┌──────────────────┼──────────────────┐
                                    │                  │                  │
                              [ESP32 3V3]         [Ra-02 VCC]      [IRF7342 S1]
                                    │                                     │
                              [OLED VCC]                            [D1] ─┴─> [NEO-6M VCC]
```

### Key decisions

**Single-stage conversion.** MT3608 is trimmed to **3.30 V**, not 5 V. The external AMS1117 is removed entirely. This avoids the two-stage penalty:

| Path | Efficiency |
|---|---|
| MT3608 → 5 V → AMS1117 → 3.3 V | 0.90 × 0.66 = **59 %** |
| MT3608 → 3.3 V direct | **~90 %** |

**Full cell utilisation.** MT3608 operates down to ~2 V input, so the cell is drained to the TP4056 cutoff at 3.0 V instead of stalling at 3.8 V. That recovers roughly 68 % more usable capacity compared to feeding a 1.1 V-dropout LDO directly.

**Flashing stays trivial.** Plug USB → the laptop supplies 5 V on `VIN`, the DevKit's own LDO and USB-UART chip run normally, flash over the onboard port. Unplug → the node runs from the trimmed 3.3 V rail. Nothing needs desoldering.

**Physical disconnect.** SS12D06 breaks the battery rail completely for storage, eliminating converter quiescent draw.

### Decoupling (not optional)

| Location | Components |
|---|---|
| MT3608 input | 10 µF + 100 nF |
| MT3608 output / 3.3 V rail | 470 µF LowESR + 100 nF |
| Ra-02 VCC, at the pin | 100 nF (+ 10 µF if space) |
| NEO-6M VCC | 100 nF |
| OLED VCC | 100 nF |
| Across divider lower leg | 100 nF |

Skipping per-module decoupling produces brownout resets on every LoRa TX burst — a bug that looks like firmware but is pure layout.

---

## Mechanical: three-layer sandwich

| Layer | Contents | Height budget |
|---|---|---|
| **1 — bottom** | 18650 holder, TP4056, fuse, slide switch | 18 mm (cell) |
| **2 — middle** | ESP32 DevKit on headers, MT3608, IRF7342 | 12 mm (DevKit) |
| **3 — top** | Ra-02 breakout, NEO-6M, OLED, USER button | ~8 mm |

Gaps: **M2 × 20** between layers 1–2 (cell clearance), **M2 × 15** between layers 2–3 (DevKit + wiring clearance).

Overall envelope: **≈ 80 × 60 × 45 mm**. Only the three buttons and the display protrude; the underside of layer 1 is kept clear.

Inter-layer wiring uses paired PLS/PBS headers so layers separate like Arduino shields instead of a 15-wire harness. Signals crossing layers: SPI ×7, UART ×2, I²C ×2, gate control ×1, button ×1, power/GND ×2.

> Mount the GPS patch antenna on the **inside of the top lid**, facing skyward. Burying it under another board costs fix time and sensitivity.

---

## Firmware

### Option A — stock target (recommended first boot)

Flash `tlora-v2-1-1_6` (or `tlora-v2-1-1_8`) from the [Meshtastic Web Flasher](https://flasher.meshtastic.org). Radio, display, battery sensing and buttons work immediately because the pinout matches.

Then enable GPS from the phone app or CLI:

```bash
meshtastic --set position.gps_mode ENABLED
meshtastic --set position.rx_gpio 13
meshtastic --set position.tx_gpio 14
meshtastic --set position.gps_update_interval 3600
```

In this mode GPS power is **not** gated — the receiver stays powered.

### Option B — custom variant (enables hardware gating)

Copy an existing variant and add the GPS/gating defines. Per Meshtastic policy, DIY hardware must use `PRIVATE_HW`; it will not appear in the Web Flasher and you maintain it yourself.

`variants/esp32/lora32pp/platformio.ini`:

```ini
[env:lora32pp]
extends = esp32_base
board = esp32doit-devkit-v1
board_level = extra
monitor_filters = esp32_exception_decoder

build_flags =
  ${esp32_base.build_flags}
  -D PRIVATE_HW
  -D LORA32PP
  -I variants/esp32/lora32pp

lib_deps =
  ${esp32_base.lib_deps}

upload_speed = 921600
```

`variants/esp32/lora32pp/variant.h`:

```c
#pragma once

// ---- Display -------------------------------------------------------------
#define I2C_SDA 21
#define I2C_SCL 22
#define USE_SH1106          // comment out to let AutoOLEDWire probe

// ---- User button (input-only pin: external 10k pull-up required) ---------
#define BUTTON_PIN 39

// ---- Battery sense -------------------------------------------------------
#define BATTERY_PIN 35
#define ADC_CHANNEL ADC_CHANNEL_7
#define ADC_MULTIPLIER 2.0
#define BATTERY_SENSE_SAMPLES 30

// ---- GNSS ----------------------------------------------------------------
#define HAS_GPS 1
#define GPS_RX_PIN 13       // ESP32 receives  <- module TX
#define GPS_TX_PIN 14       // ESP32 transmits -> module RX
#define GPS_UBLOX
#define GPS_BAUDRATE 9600

#define PIN_GPS_EN 15       // IRF7342 channel 1 gate
#define GPS_EN_ACTIVE 0     // P-FET: LOW = powered

// ---- LoRa (explicit, so upstream default changes can't break us) ---------
#define USE_RF95
#undef LORA_SCK
#define LORA_SCK 5
#undef LORA_MISO
#define LORA_MISO 19
#undef LORA_MOSI
#define LORA_MOSI 27
#undef LORA_CS
#define LORA_CS 18
#define LORA_RESET 23
#define LORA_DIO0 26
#define LORA_DIO1 33        // MUST be physically wired
#define LORA_DIO2 32

// SX1278 PA_BOOST accepts 2..17 dBm or exactly 20; 18/19 are coerced to 17.
#define RF95_MAX_POWER 17

// ---- Misc ----------------------------------------------------------------
#define LED_POWER 2
```

Build:

```bash
git clone https://github.com/meshtastic/firmware.git
cd firmware
git submodule update --init
mkdir -p variants/esp32/lora32pp   # drop the two files here
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
U-blox 6 detected
Use analog input 35 for battery level
```

`RF95 init result -2` (`CHIP_NOT_FOUND`) means SPI wiring, 3.3 V supply, or swapped MISO/MOSI.

---

## Configuration

Region **UA_433** is defined in firmware as 433.0–434.7 MHz, **10 % duty cycle, 10 dBm limit**. The regulatory clamp is applied automatically; `isTxAllowedAirUtil()` additionally throttles TX at half the regional allowance.

Suggested settings for a low-traffic personal node:

| Setting | Value | Why |
|---|---|---|
| Region | `UA_433` | local band |
| Modem preset | `LONG_FAST` | SF11/BW250, ~330 ms per 20-byte packet |
| Role | `CLIENT` / `CLIENT_MUTE` | no rebroadcast load |
| TX power | 10 dBm | region clamps anyway |
| Override duty cycle | `false` | stay legal |
| `position.gps_update_interval` | 3600 s | one cold fix per hour |
| `position.gps_mode` | `ENABLED` | gating handled by firmware |
| `display.screen_on_secs` | 10 | panel is a real consumer |
| `power.is_power_saving` | `true` | |
| Bluetooth | off when not configuring | |
| `power.adc_multiplier_override` | 0 | only set if voltage reads wrong |

At `LONG_FAST` a 20-byte frame occupies ~330 ms of airtime; the effective 5 % throttle allows roughly 545 packets/hour — far more than a personal node needs.

---

## Bring-up checklist

1. **Trim MT3608 before connecting anything.** Feed 5–7 V in, measure `VOUT`, adjust the trimmer to **3.30 ± 0.05 V**. Then load it (~100 mA) and confirm it holds. Never connect a 5 V-configured module to this rail.
2. **Verify TP4056 has protection.** Look for DW01 + FS8205A next to the main IC. A bare TP4056 has no over-discharge cutoff.
3. **Continuity-check the antenna.** Centre conductor end-to-end should be near 0 Ω; centre-to-shell must be open. Some batches ship with the centre pin flooded with solder.
4. **Check IRF7342 orientation.** Source to 3.3 V, drain to load. Reversed, the body diode conducts permanently and GPS never turns off.
5. **Confirm gate pull-ups fitted** (100 kΩ gate→source on both channels; tie unused G2 to S2).
6. **Seat U.FL straight down until it clicks.** Rated for ~30 mating cycles; angled force lifts the pad.
7. **Always attach the antenna before transmitting.**
8. First flash over USB with the battery rail switched off.

---

## Known limitations

Honest list, so nobody expects a T-Beam clone:

- **Size.** ≈ 216 cm³ vs 66 cm³ for a T-Beam v1.2. Three perfboards plus a cylindrical cell cannot match a multilayer factory PCB. Footprint is about half an iPhone's, thickness about five times.
- **Efficiency ceiling.** ~90 % single-stage is good, but there is no PMIC. No per-rail telemetry, no coulomb counting, no programmable charge current, no NTC thermal guard.
- **USB-UART overhead.** The DevKit's CP2102/CH340 sits on the rail and draws current whenever the node is powered. A factory board gates it; here it is the price of convenient flashing.
- **GNSS.** NEO-6M is GPS-only, ~45 mA during acquisition, ~26 s cold start. NEO-M8N / MAX-M10S are faster, more sensitive and lighter on the battery.
- **No TCXO** on Ra-02 — reference frequency drifts with temperature, which matters at narrow bandwidths.
- **Not in the Web Flasher** if you use the custom variant. `PRIVATE_HW` builds are community-maintained by design.

---

## Roadmap

- [x] Component selection and BOM
- [x] Pinout compatible with stock `tlora-v2-1-1_6`
- [x] Power path design (single-stage 3.3 V, full cell utilisation)
- [x] Hardware GPS gating via IRF7342
- [ ] Perfboard assembly and bring-up
- [ ] Measured current profiles per role/interval
- [ ] 3D-printable enclosure (STEP + STL)
- [ ] Assembly photos and wiring diagram
- [ ] Second node (headless relay: ESP32 + Ra-02, no GPS/display)
- [ ] **v2.0:** single custom PCB in KiCad, SMD, TPS63020 or AXP-class PMIC, USB-C, SX1262 + MAX-M10S

---

## Safety and legality

- A 18650 cell delivers 15–20 A into a short. **Fit the fuse.** TP4056 protection trips around 3 A, which is above what thin perfboard wiring survives.
- Never transmit without an antenna attached.
- Observe your regional band plan. `UA_433` limits ERP to 10 mW with a 10 % duty cycle; firmware enforces this unless you explicitly override it. Overriding is on you.
- Li-Ion cells hate heat. Do not charge above 1 A on a bare TP4056 without heatsinking, and never charge a swollen or damaged cell.

---

## References

- [Meshtastic firmware](https://github.com/meshtastic/firmware) — variant sources, `architecture.h` defaults
- [Meshtastic build docs](https://meshtastic.org/docs/development/firmware/build/) — custom hardware / `PRIVATE_HW` policy
- [LILYGO T3 / LoRa32 pin mapping](https://wiki.lilygo.cc/products/t3-series/t3-lora32/)
- [IRF7342 datasheet (Infineon)](https://www.infineon.com/assets/row/public/documents/non-assigned/49/irf7342pbf-1.pdf)
- [SMA vs RP-SMA connector guide](https://en.wikipedia.org/wiki/SMA_connector)

---

## License

Hardware documentation: **CC BY-SA 4.0**.  
Any firmware variant files derived from Meshtastic inherit **GPL-3.0**.

---

## Contributing

Built something on this pinout? Found a mistake in the tables? Open an issue or PR — especially with measured current draw, real-world range figures, or enclosure designs. Corrections backed by a multimeter beat corrections backed by opinion.
