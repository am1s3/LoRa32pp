# LoRa32++

> DIY Meshtastic node built on the LILYGO T-LoRa32 V2.1 pinout, with GPS, dual-source hot-swap power and a serviceable sandwich construction.

**Current version: V1.2**
**Status:** 🚧 Prototype / documentation stage. Hardware design finalised, assembly in progress. 3D-printable enclosure coming later.

---

## What this is

LoRa32++ is not a new PCB design (yet). It is a **hand-soldered node on perfboard**, wired to be pin-compatible with the stock Meshtastic `tlora-v2-1-1_6` firmware target, plus the peripherals that board never had.

The idea: take the well-tested T-LoRa32 schematic base, add everything a T-Beam has, keep every module socketed so nothing is unrepairable, and make the power system modular.

```
T-LoRa32 V2.1 + GPS + dual 18650 power path + bigger OLED + fixed DIO1 = LoRa32++
```

### Why "++"

| Improvement | Stock T-LoRa32 V2.1 | LoRa32++ V1.2 |
|---|---|---|
| GNSS | ❌ none | ✅ ATGM336H (AT6558, multi-constellation) + patch antenna |
| Battery | JST connector for flat LiPo only | ✅ internal 18650 holder **plus external 1S pack input** |
| Hot-swap power | ❌ none | ✅ diode-ORing: swap packs without rebooting the node |
| Power conversion | direct LiPo / USB | ✅ MT3608 trimmed to 3.3 V, single-stage, LC-filtered |
| Display | 0.96" SSD1306 | ✅ 1.3" SH1106 (more readable area) |
| `LORA_DIO1` | not routed on early revisions | ✅ wired to GPIO33 from day one |
| Battery sensing | 100k/100k divider | ✅ same divider + local RC filter + software averaging |
| Repairability | SMD, board-level replacement | ✅ every module on headers |

---

## V1.2 changelog

- **Perfboards back to 60 × 80 mm.** The 50 × 70 experiment was too tight: SPI wires ran over components, the GPS patch sat against the Ra-02 shield, and standoff holes weakened the corners. 60 × 80 gives clean routing and a proper diagonal battery bay.
- **Internal SMD 18650 holder retained**, mounted diagonally corner-to-corner on the bottom board. The two free triangles host the TP4056 and the MT3608 + LC filter.
- **External battery input added** (JST-PH 2.0, **1S only**). Diode-ORing with the internal cell enables hot-swap: plug a charged pack, the node keeps running, unplug it and the internal cell takes over instantly.
- **Reverse-polarity and overcurrent protection** on both power paths (Schottky ORing + 1 A fuse).
- **Removed the dummy-load resistor idea** from earlier drafts: a permanent 10 mA bleed wastes ~25 % of a 3000 mAh cell per day and contradicts the whole point of this build. Ripple is handled by the LC filter and ADC oversampling instead.
- **BOM.csv removed.** The parts list now lives in this README so the repo stays self-contained.

---

## ⚠️ Power rules (read before plugging anything in)

1. **EXTERNAL INPUT IS 1S ONLY: 3.0 – 4.2 V.**
   The MT3608 is a **boost** converter. It cannot step down. If you feed it 2S (7.4 V) or a 5 V power bank, the output rail rises toward the input voltage and **every 3.3 V component on the node dies at once**. There is no firmware protection against this. Label your packs.
2. **Never connect two sources without the ORing diodes fitted.** Direct paralleling of cells at different state-of-charge drives equalisation currents of several amps through perfboard wiring. The diodes are not optional.
3. **Always attach the antenna before transmitting.** TX into an open connector reflects power into the SX1278 PA and degrades it permanently.
4. **Fit the 1 A fuse.** A 18650 delivers 15–20 A into a short; TP4056 protection trips around 3 A, which is above what thin perfboard wiring survives.

---

## Bill of materials (V1.2)

Prices are approximate street prices in UAH at the time of writing. Anything marked "owned" assumes you already have the basic maker stash.

| # | Component | Part / spec | Qty | ~Price |
|---|---|---|---|---|
| 1 | MCU board | ESP32-WROOM-32 DevKit V1 (38-pin) | 1 | owned / 150 ₴ |
| 2 | LoRa transceiver | Ai-Thinker Ra-02 (SX1278, 433 MHz), **full-pin breakout with DIO1** | 1 | 298 ₴ |
| 3 | GNSS | ATGM336H (AT6558) + IPEX patch antenna | 1 | 338 ₴ |
| 4 | Display | OLED 1.3" 128×64, SH1106, I²C | 1 | owned / 100 ₴ |
| 5 | Boost converter | MT3608 module **with trimmer** | 1 | owned / 40 ₴ |
| 6 | Charger | TP4056 **with protection** (DW01 + FS8205A) | 1 | owned / 30 ₴ |
| 7 | Internal cell | 18650 Li-Ion, flat-top unprotected | 1 | 150–250 ₴ |
| 8 | Cell holder | 18650 SMD holder, wired to perfboard | 1 | 19 ₴ |
| 9 | External input | JST-PH 2.0 female (board) + male pigtails | 1+2 | ~13 ₴ |
| 10 | ORing diodes | Schottky **SS14** (1 A, 40 V, SMA/DO-214AC) | 2 | ~6 ₴ |
| 11 | Fuse | 1 A fast + holder (or PPTO resettable 1.1 A) | 1 | ~5 ₴ |
| 12 | Power switch | SS12D06 slide switch | 1 | 11 ₴ |
| 13 | Antenna | 433 MHz whip, RP-SMA + IPEX pigtail 15 cm | 1 | 59 ₴ |
| 14 | LC filter | 10 µH inductor (≥0.5 A) + 22 µF X7R + 100 nF X7R | 1 set | ~5 ₴ |
| 15 | Bulk caps | 470 µF 16 V low-ESR electrolytic | 2 | ~4 ₴ |
| 16 | Divider / pull-ups | 100 kΩ ×2 (ADC), 10 kΩ ×1 (button) | 3 | ~2 ₴ |
| 17 | ADC filter cap | 100 nF X7R | 1 | ~1 ₴ |
| 18 | Buttons | tactile DIP 6×6 mm (USER; RESET/BOOT onboard) | 1 | ~2 ₴ |
| 19 | Perfboard | 60 × 80 mm double-sided, 2.54 mm grid | 3 | 87 ₴ |
| 20 | Stack hardware | M2 standoffs 20 mm + 15 mm, screws | 8+8 | ~40 ₴ |
| 21 | Interconnect | header pairs PLS/PBS, Dupont set, hookup wire | 1 set | ~35 ₴ |

**Total new spend (assuming owned stash): ≈ 950–1000 ₴.**
For comparison: a factory T-Beam V1.1 costs ≈ 1600 ₴ and has neither hot-swap power nor a modern GNSS.

> Do **not** substitute the ORing Schottkys with ordinary silicon diodes (1N400x): 0.7–1.0 V forward drop starves the boost converter at low cell voltage. Do **not** use high-leakage big Schottkys (SS34-class) on the battery path if you care about deep-sleep current; SS14-class leakage is an order of magnitude lower.

---

## Documentation

- [`HARDWARE.md`](HARDWARE.md) — wiring tables, power tree, mechanical stack-up, bring-up checklist, test points. **Source of truth for assembly.**
- [`variants/esp32/lora32pp/variant.h`](variants/esp32/lora32pp/variant.h) — custom firmware variant (Option B).

The sections below summarise the essentials; the linked files win if there is ever a conflict.

---

## Hardware summary

### Pinout (base = stock `tlora-v2-1-1_6`)

#### LoRa — Ra-02 / SX1278

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

> `LORA_DIO1` is not cosmetic. The Meshtastic RX task is woken by this interrupt. Leaving it unconnected forces polling fallback and costs packets. Buy the **full-pin** Ra-02 breakout; 8-pin variants without DIO1 are a trap.

#### Display — OLED SH1106 (I²C)

| Module pin | ESP32 GPIO |
|---|---|
| SDA | **21** |
| SCL | **22** |

Address `0x3C`. No reset pin, no `VEXT_ENABLE`.

#### GNSS — ATGM336H (UART)

| Module pin | ESP32 GPIO | Direction |
|---|---|---|
| **TX** | **13** (`GPS_RX_PIN`) | module → ESP32 |
| **RX** | **14** (`GPS_TX_PIN`) | ESP32 → module |
| VCC | 3.3 V rail | see note below |
| PPS | spare pad | optional, not used by default |

> Naming follows Meshtastic convention: `GPS_RX_PIN` is the pin **the ESP32 receives on**, so it goes to the module's TX. Getting this backwards is the single most common GPS wiring mistake.

> **GNSS supply note:** the ATGM336H breakout carries an onboard LDO and is marketed as "5 V". The AT6558 die itself runs at ~3.3 V. Feeding the breakout 3.3 V puts the LDO in dropout; the die then sees ≈ 3.0–3.2 V, which is inside its operating range but with little margin. **Verify before assembly:** power the module at 5.0 V, find the node that reads ≈ 3.3 V (LDO output), then drop the input to 3.30 V and re-measure that node. ≥ 3.0 V → run as designed. 2.8–3.0 V → trim the rail to 3.40–3.45 V (still safe for ESP32/SX1278/SH1106). < 2.8 V → bypass the breakout LDO with a solder jumper from VCC pad to LDO output (acceptable on a single regulated rail) or pick a 3.3 V-native GNSS module. Full procedure in HARDWARE.md.

#### Battery sensing

```
BAT+ (after ORing, before switch) ──[100k]──┬── GPIO35 (ADC1_CH7)
                                            │
                                         [100k]  [100nF]
                                            │       │
                                           GND ────GND
```

| Parameter | Value |
|---|---|
| `BATTERY_PIN` | 35 |
| `ADC_MULTIPLIER` | 2.0 |
| `BATTERY_SENSE_SAMPLES` | 64 |

GPIO35 is input-only, no internal pulls — ideal for ADC. **ADC1 only** (GPIO32–39); ADC2 is unusable while Wi-Fi is active. The 100 nF at the ADC pin plus 64-sample averaging absorbs MT3608 ripple; no dummy load required.

#### Buttons

| Button | GPIO | Wiring |
|---|---|---|
| BOOT | 0 | onboard |
| RESET | EN | onboard |
| **USER** | **39** | switch to GND + **external 10 kΩ pull-up to 3.3 V** |

GPIO34–39 have no internal pull-ups; the external resistor is mandatory.

#### Reserved / do not touch

| GPIO | Reason |
|---|---|
| 6–11 | SPI flash — using these bricks the board |
| 12 | Strapping (MTDI); HIGH at boot → 1.8 V flash → no start |
| 2 | Strapping; must be LOW/floating during flashing |
| 34, 36 | Free, input-only, available for future sensors |

---

## Power architecture (V1.2)

```
[internal 18650] ──[SS14 #1]──┐
                              ├──[FUSE 1A]──[SS12D06]──┬──[TP4056 B+/B-]*
[external JST 1S] ──[SS14 #2]──┘                        │
                                                       ├──[100k/100k + 100nF]── GPIO35
                                                       │
                                                  [MT3608 IN]
                                                       │ trim 3.30 V
                                                       ▼
                                              [10µH]──┬── 3.3 V RAIL
                                                      │
                                                 [22µF]+[100nF]+[470µF]
                                                      │
                          ┌───────────┬───────────────┼──────────────┐
                          │           │               │              │
                    [ESP32 3V3]  [Ra-02 VCC]     [OLED VCC]    [ATGM336H VCC]
```

\* TP4056 sits **before the ORing diodes**, directly across the internal holder, so USB charging only ever charges the internal cell. The external pack is isolated from the charger by its own diode. Charge external packs with their own charger.

### Key decisions

- **Single-stage conversion.** MT3608 trimmed to **3.30 V**, not 5 V. No second LDO in the main path. Two-stage (5 V → AMS1117 → 3.3 V) would cost ≈ 40 % of the energy; direct 3.3 V keeps ≈ 90 %.
- **Full cell utilisation.** MT3608 operates down to ~2 V input, so the cell drains to the TP4056 cutoff instead of stalling at 3.8 V behind an LDO.
- **Hot-swap via ORing.** The higher-voltage source wins; the other diode is reverse-biased. Unplugging the external pack transfers load to the internal cell with zero interruption. This is a poor-man's UPS.
- **DevKit powered through its 3V3 pin**, not VIN. The onboard AMS1117 needs ≥ 4.5 V at VIN to regulate; at 3.3 V rail it would output ~2.2 V and brown out the ESP32. Feeding the 3V3 pin bypasses it entirely. USB flashing still works normally with the battery rail switched off.
- **Physical disconnect.** SS12D06 breaks the rail completely for storage.

### Realistic autonomy (honest numbers)

Assumptions: 3000 mAh cell, CLIENT_MUTE role, LONG_FAST preset, screen 10 s, GPS fix hourly, BLE off when idle, power saving on.

| Item | Average contribution |
|---|---|
| ESP32 deep sleep + RTC + radio wake cycles | ~0.5–1.5 mA |
| GNSS active fixes (≈ 30 s/h × 25 mA) | ~0.2 mA |
| MT3608 quiescent + feedback divider | ~1–2 mA |
| TP4056 + DW01 quiescent | ~0.01 mA |
| SS14 reverse leakage (room temp) | ~0.02–0.1 mA |
| DevKit USB-UART chip idle (price of convenient flashing) | ~1–3 mA |
| **Total average** | **≈ 3–7 mA** |

**3000 mAh / 5 mA ≈ 25 days per charge.** With a 9000 mAh external 1S pack: ≈ 2–3 months. Self-discharge of the cell itself (~2–3 %/month) becomes relevant only beyond that horizon. Earlier claims of "years of runtime" were nonsense and have been removed; the DevKit's always-on USB-UART and the boost converter's quiescent current set the real floor. If you need months on one internal cell, move to a bare WROOM module (removes the UART overhead) — that is a V2.0 topic.

---

## Mechanical: three-layer sandwich (60 × 80 mm)

| Layer | Contents | Height budget |
|---|---|---|
| **1 — bottom** | 18650 holder **diagonal corner-to-corner**, TP4056 in one free triangle, MT3608 + LC filter in the other, fuse, slide switch, JST external input on the edge | 18 mm (cell) |
| **2 — middle** | ESP32 DevKit on headers, USER button | 12 mm |
| **3 — top** | Ra-02 breakout, ATGM336H, OLED, antenna pigtail anchored at the edge | ~8 mm |

Diagonal math: board diagonal = √(60² + 80²) = **100 mm**; holder + cell ≈ 78 mm fits with > 20 mm spare, contacts landing near opposite corners where the charger and converter live. Secure the cell with a hook-and-loop strap or two dabs of hot glue at the holder ends (never on contacts).

Gaps: **M2 × 20** between layers 1–2, **M2 × 15** between layers 2–3. Inter-layer wiring uses paired PLS/PBS headers so layers separate like shields. Overall envelope ≈ **80 × 60 × 45 mm**. Mount the GPS patch on the top layer facing skyward; keep it clear of the Ra-02 shield can.

Open-frame operation is fine for bench/indoor use: insulate the underside (cardboard/kapton/feet), strain-relieve the antenna pigtail with a hot-glue cone at the board edge, and keep the node away from conductive debris. Enclosure is a later step, after the electronics has proven itself for a week.

---

## Firmware

### Option A — stock target (recommended first boot)

Flash `tlora-v2-1-1_6` from the Meshtastic Web Flasher. Radio, display, battery sensing and buttons work immediately because the pinout matches. Then enable GPS:

```bash
meshtastic --set position.gps_mode ENABLED
meshtastic --set position.rx_gpio 13
meshtastic --set position.tx_gpio 14
meshtastic --set position.gps_update_interval 3600
```

### Option B — custom variant

Copy the files from `variants/esp32/lora32pp/` into a Meshtastic firmware checkout and build:

```bash
git clone https://github.com/meshtastic/firmware.git
cd firmware
git submodule update --init
mkdir -p variants/esp32/lora32pp   # drop variant.h + platformio.ini here
pio run --environment lora32pp
pio run --environment lora32pp --target upload
pio device monitor
```

Per Meshtastic policy, DIY hardware uses `PRIVATE_HW`; it will not appear in the Web Flasher and you maintain it yourself.

Expected boot log:

```
RF95 init result 0
Frequency set to 433.xxx
SH1106 found at address 0x3C
Use analog input 35 for battery level
```

`RF95 init result -2` (`CHIP_NOT_FOUND`) means SPI wiring, 3.3 V supply, or swapped MISO/MOSI.

---

## Configuration

Region **UA_433**: 433.0–434.7 MHz, 10 % duty cycle, 10 dBm limit. The regulatory clamp is applied automatically.

| Setting | Value | Why |
|---|---|---|
| Region | `UA_433` | local band |
| Modem preset | `LONG_FAST` | good range/airtime trade-off |
| Role | `CLIENT` / `CLIENT_MUTE` | no rebroadcast load |
| TX power | 10 dBm | region clamps anyway |
| Override duty cycle | `false` | stay legal |
| `position.gps_update_interval` | 3600 s | one fix per hour |
| `display.screen_on_secs` | 10 | panel is a real consumer |
| `power.is_power_saving` | `true` | |
| Bluetooth | off when not configuring | |
| `power.adc_multiplier_override` | 0 | only set if voltage reads wrong |

---

## Bring-up checklist

1. **Trim MT3608 before connecting anything.** Feed 5–7 V in, adjust to **3.30 ± 0.05 V**, load it (~100 mA) and confirm it holds. Never connect a 5 V-configured module to this rail.
2. **Verify TP4056 has protection** (DW01 + FS8205A next to the main IC).
3. **Verify ORing diode orientation** on both paths: anode to source, cathodes joined toward the fuse. Reversed, the body conducts backwards and the "protection" is a short.
4. **Continuity-check the antenna pigtail**: centre end-to-end ≈ 0 Ω, centre-to-shell open.
5. **GNSS LDO dropout test** (see HARDWARE.md) before committing the rail voltage.
6. **Seat U.FL straight down until it clicks.** Rated ~30 mating cycles.
7. **Antenna attached before any TX.**
8. First flash over USB with the battery rail switched off.
9. Hot-swap test: run on internal cell, plug a charged external pack, confirm no reboot; unplug, confirm no reboot.

---

## Known limitations

- **Size.** ≈ 216 cm³ vs 66 cm³ for a T-Beam v1.2. Three perfboards plus a cylindrical cell cannot match a multilayer factory PCB.
- **No PMIC.** No per-rail telemetry, no coulomb counting, no programmable charge current, no NTC thermal guard. Battery % is voltage-derived (±10–15 %).
- **USB-UART overhead.** The DevKit's CP2102/CH340 draws current whenever powered. Factory boards gate it; here it is the price of convenient flashing and the main floor on sleep current.
- **Boost-only external input.** 1S packs only. A buck-boost front end would allow 2S–4S but costs efficiency, money and complexity; deliberately out of scope for V1.2.
- **No TCXO** on Ra-02 — reference frequency drifts with temperature, which matters at narrow bandwidths.
- **Not in the Web Flasher** if you use the custom variant.

---

## Roadmap

- [x] Component selection and pinout compatible with stock `tlora-v2-1-1_6`
- [x] Single-stage 3.3 V power path, full cell utilisation
- [x] V1.2: 60 × 80 mm boards, diagonal internal holder
- [x] V1.2: external 1S input with Schottky ORing, fuse, hot-swap
- [ ] Perfboard assembly and bring-up
- [ ] Measured current profiles per role/interval
- [ ] Enclosure (3D-printed or repurposed IP-rated box) + bulkhead antenna mount
- [ ] Assembly photos and wiring diagram
- [ ] Second node (headless relay: ESP32 + Ra-02, no GPS/display)
- [ ] **v2.0:** single custom PCB in KiCad, SMD, buck-boost PMIC (TPS63020-class), USB-C, SX1262 + TCXO, MAX-M10S, bare WROOM module to kill UART overhead

---

## Safety and legality

- A 18650 cell delivers 15–20 A into a short. **Fit the fuse.**
- Never transmit without an antenna attached.
- Observe your regional band plan. `UA_433` limits ERP to 10 mW with 10 % duty cycle; firmware enforces this unless you explicitly override it. Overriding is on you.
- Li-Ion cells hate heat. Do not charge above 1 A on a bare TP4056 without heatsinking; never charge a swollen or damaged cell.
- External packs: **1S only**, correct polarity, fused. Label every pack you build.

---

## References

- [Meshtastic firmware](https://github.com/meshtastic/firmware) — variant sources, `architecture.h` defaults
- [Meshtastic build docs](https://meshtastic.org/docs/development/firmware/build/) — custom hardware / `PRIVATE_HW` policy
- [LILYGO T3 / LoRa32 pin mapping](https://wiki.lilygo.cc/products/t3-series/t3-lora32/)
- [AT6558 / ATGM336H datasheets](https://www.icofchina.com/) — GNSS electrical characteristics
- [SMA vs RP-SMA connector guide](https://en.wikipedia.org/wiki/SMA_connector)

---

## License

Hardware documentation: **CC BY-SA 4.0**.
Any firmware variant files derived from Meshtastic inherit **GPL-3.0**.

---

## Contributing

Built something on this pinout? Found a mistake in the tables? Open an issue or PR — especially with measured current draw, real-world range figures, or enclosure designs. Corrections backed by a multimeter beat corrections backed by opinion.
