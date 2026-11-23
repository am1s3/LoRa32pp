# LoRa32++ V1.2 — Hardware Reference

Source of truth for assembly. If README and this file disagree, this file wins.
All voltages DC unless stated. All prices UAH, approximate.

---

## 1. Power tree

### 1.1 Net list

| Net | From | To | Notes |
|---|---|---|---|
| CELL_INT+ | holder spring (+) | SS14 #1 anode | keep lead < 40 mm |
| CELL_INT− | holder spring (−) | GND_STAR | |
| EXT+ | JST pin 1 | SS14 #2 anode | **1S only, 3.0–4.2 V** |
| EXT− | JST pin 2 | GND_STAR | |
| OR_OUT | SS14 #1 cathode + SS14 #2 cathode | FUSE in | joined cathodes = OR node |
| FUSED | FUSE out | SW1 (SS12D06) pole | |
| BAT_SW | SW1 throw | MT3608 IN+, TP4056 OUT+* , divider top | *see 1.3 |
| RAIL_3V3 | MT3608 OUT+ → L1 → C_out | ESP32 3V3, Ra-02 VCC, OLED VCC, GNSS VCC | single rail |
| GND_STAR | all grounds | single star point at MT3608 GND pad | avoid daisy-chained ground loops |

### 1.2 ORing behaviour

- Higher source voltage forward-biases its diode and feeds the load; the lower source's diode is reverse-biased. No equalisation current between packs.
- Transfer on plug/unplug is seamless (< 1 ms, held by bulk capacitance). Node does not reboot.
- Forward drop ≈ 0.25–0.35 V at 100–300 mA (SS14). Budget it: at cell 3.2 V the converter sees ≈ 2.9 V. MT3608 starts from 2 V, fine.
- Reverse leakage of SS14 at 25 °C ≈ 20–100 µA max per diode; at 60 °C it grows several-fold. This is why SS34-class (0.5 mA class leakage) is rejected for the battery path.

### 1.3 Charger isolation

TP4056 B+/B− connect **directly across the internal holder**, i.e. *before* SS14 #1. Consequences:

- USB charging charges only the internal cell. Correct: TP4056 has no balancing and no knowledge of external pack capacity.
- External pack can never back-feed the charger (SS14 #2 blocks toward OR node; charger is on the other side of SS14 #1).
- With USB plugged and external pack connected, the node runs from whichever source is higher after the diodes; charging proceeds independently. No conflict.
- **Do not** move TP4056 after the ORing node. That would put charger output onto both packs through diodes and defeat isolation.

### 1.4 Boost stage and filtering

```
MT3608 OUT+ ──[L1 10µH ≥0.5A]──┬── RAIL_3V3
                               │
                          [C1 22µF X7R]
                          [C2 100nF X7R]
                          [C3 470µF low-ESR]
                               │
                              GND
```

- Trim MT3608 to **3.30 ± 0.05 V** *before* attaching loads. Verify under ~100 mA load.
- L1 + C1 form the ripple filter (PFM pulse-skipping at light load is the noise source). C2 handles HF edges; C3 holds TX bursts and hot-swap transients.
- Place C1/C2 within 10 mm of Ra-02 VCC pin; C3 near ESP32 3V3 pin.
- **No dummy load.** Earlier drafts proposed 330 Ω bleed to force CCM. Rejected: 10 mA permanent = ~240 mAh/day ≈ 25 % of a 3000 mAh cell daily. Ripple is instead handled here (hardware) and by ADC oversampling (firmware).
- MT3608 is **boost-only**. Input above ~3.3 V passes toward the rail minus internal drops. Hence the 1S-only rule on both sources. There is no overvoltage crowbar on V1.2; discipline is the protection. (V2.0 candidate: buck-boost PMIC.)

### 1.5 DevKit feeding

- Feed the rail into the DevKit **3V3 pin**, never VIN, when running on battery. Onboard AMS1117 dropout ≈ 1.1 V; at 3.3 V in it outputs ~2.2 V → brownout.
- USB flashing: battery switch OFF, USB supplies VIN → onboard LDO → 3V3 pin normally. The external rail is disconnected by SW1, so no back-feed into MT3608 output (which is non-synchronous and tolerant anyway, but keep the switch off as habit).
- The onboard USB-UART remains powered whenever the rail is live: expect 1–3 mA overhead. Accepted trade-off for tool-less flashing. Bare-module variant is V2.0.

### 1.6 Battery sense

```
BAT_SW ──[R1 100k]──┬── GPIO35
                    │
                 [R2 100k] [C4 100nF]
                    │        │
                   GND ──────GND
```

- Divider current @ 4.2 V ≈ 21 µA. ADC sees 2.10 V max — inside linear region.
- Multiplier 2.0 matches stock variant; no calibration needed. If readings drift, trim `config.power.adc_multiplier_override` from the app.
- C4 at the pin + `BATTERY_SENSE_SAMPLES 64` in firmware = ripple immunity without bleed resistors.
- Sense point is **after** ORing and fuse, **before** switch: with switch OFF the divider still reads pack voltage (useful for storage checks); divider drain 21 µA is acceptable. If you want zero storage drain, move divider top to BAT_SW (after switch) instead.

---

## 2. Module-by-module wiring

### 2.1 Ra-02 (SX1278), full-pin breakout

| Ra-02 | ESP32 | Wire colour suggestion |
|---|---|---|
| SCK | 5 | yellow |
| MISO | 19 | green |
| MOSI | 27 | blue |
| NSS | 18 | white |
| RST | 23 | orange |
| DIO0 | 26 | purple |
| DIO1 | 33 | grey |
| DIO2 | 32 | spare (optional) |
| VCC | RAIL_3V3 | red |
| GND | GND_STAR | black |

- Keep SPI leads < 60 mm, twisted with a ground return where possible.
- DIO1 mandatory. 8-pin breakouts lacking DIO1 are incompatible with interrupt-driven RX.
- Antenna: IPEX pigtail → RP-SMA whip. Anchor the pigtail connector at board edge with a hot-glue cone so mating torque never reaches the U.FL pad. **Antenna on before TX, always.** Do not cut whip elements blindly; internal loading coils exist. Verify SWR if you modify.

### 2.2 OLED SH1106 1.3"

| OLED | ESP32 |
|---|---|
| SDA | 21 |
| SCL | 22 |
| VCC | RAIL_3V3 |
| GND | GND_STAR |

Address 0x3C. No reset line. Do not power-gate the display: Meshtastic probes I²C at boot and skips screen creation if absent.

### 2.3 ATGM336H GNSS

| GNSS | ESP32 / rail |
|---|---|
| TX | 13 (ESP32 RX) |
| RX | 14 (ESP32 TX) |
| VCC | RAIL_3V3 (subject to 2.4 test) |
| GND | GND_STAR |
| PPS | test pad only |

#### 2.4 LDO dropout verification (mandatory before fixing rail voltage)

The breakout is sold as "5 V" because of an onboard LDO; the AT6558 die is a 3.3 V part. Procedure:

1. Bench-feed VCC = 5.00 V. Confirm NMEA at 9600 baud, current 20–30 mA.
2. Black probe on GND pad. Probe the MLCC terminals around the shield can; the stable ≈ 3.3 V node is the die rail (LDO output). Mark it.
3. Drop input to 3.30 V. Re-measure marked node:
   - **≥ 3.0 V** → keep rail 3.30 V. Add 10 µF tantalum at module VCC/GND for cold-start peaks.
   - **2.8–3.0 V** → trim rail to 3.40–3.45 V (safe: ESP32 ≤ 3.6 V, SX1278 ≤ 3.6 V, SH1106 module tolerant). Re-verify.
   - **< 2.8 V** → high-dropout LDO fitted. Either jumper VCC pad → LDO output (bypass; acceptable because the whole node shares one regulated rail — an overvoltage event kills everything anyway, so the LDO adds no system-level protection), or substitute a 3.3 V-native GNSS.
4. Cold-start soak: unpowered 30 min, then power at chosen rail; fix within 30–60 s, no current sawtooth (reset loop).
5. If a fix-indicator LED is populated on the breakout and lights on fix, remove it for battery builds (2–3 mA).

### 2.5 USER button

GPIO39 → tactile switch → GND. External 10 kΩ from GPIO39 to RAIL_3V3. No internal pull exists on 34–39. Debounce in firmware.

### 2.6 External input connector

- JST-PH 2.0, 2-pin, board-mount female on bottom layer edge.
- Pin 1 = +, Pin 2 = −. **Mark polarity on the board silkscreen/paint and on every pack.** JST-PH has no universal polarity convention; your own convention is law.
- Packs: 1S only (single 18650 in wired holder, 1S LiPo, parallel 1S blocks of any capacity). Parallel 1S blocks are safe to combine *inside the pack* (same chemistry, matched cells, fused); never parallel at different SoC through the node without the ORing diodes.
- XT30 deliberately not fitted: current rating irrelevant at 0.3 A peaks, and its drone-pack ecosystem invites 2S+ insertion which destroys the boost-only rail.

---

## 3. Mechanical stack-up (60 × 80 mm)

### 3.1 Bottom layer layout

```
┌────────────────────────────────────────────┐
│ [MT3608 + L1/C1/C2/C3]          [JST EXT+] │
│        ╲                                   │
│         ╲                                  │
│          ╲   18650 holder, diagonal        │
│           ╲  (corner → corner)             │
│            ╲                               │
│             ╲                              │
│              ╲                             │
│               ╲                            │
│                ╲                           │
│                 ╲                          │
│                  ╲                         │
│                   ╲                        │
│                    ╲                       │
│                     ╲                      │
│                      ╲                     │
│                       ╲                    │
│                        ╲                   │
│                         ╲                  │
│                          ╲                 │
│                           ╲                │
│                            ╲               │
│                             ╲              │
│                              ╲             │
│                               ╲            │
│                                ╲           │
│                                 ╲          │
│                                  ╲         │
│                                   ╲        │
│                                    ╲       │
│                                     ╲      │
│                                      ╲     │
│                                       ╲    │
│                                        ╲   │
│                                         ╲  │
│                                          ╲ │
│ [TP4056 + SS14#1 + SS14#2 + FUSE + SW1]    │
└────────────────────────────────────────────┘
```

- Diagonal = 100 mm; holder + cell ≈ 78 mm → contacts land near opposite corners, adjacent to charger and converter respectively. Short power leads.
- Cell retention: hook-and-loop strap across cell + board, or two hot-glue dabs at holder ends. Never glue contacts.
- Underside insulation for open-frame use: kapton/cardboard sheet or four rubber feet. Conductive debris under the board is the #1 killer of open prototypes.

### 3.2 Middle layer

ESP32 DevKit on female headers (male pins on bottom layer or direct soldered standoffs — choose one convention and keep it). USER button at edge. Keep DevKit USB port accessible from the side for flashing without disassembly.

### 3.3 Top layer

Ra-02 left, GNSS right, OLED centre-front, pigtail anchor at rear edge. GPS patch faces up, ≥ 15 mm from Ra-02 shield can. Nothing metallic above the patch.

### 3.4 Standoffs

M2 × 20 between L1–L2 (cell clearance 18 mm + margin), M2 × 15 between L2–L3. Nylon preferred near antenna; brass acceptable elsewhere. Do not overtighten into perfboard — crack risk; use nuts + washers.

---

## 4. Test points

| TP | Net | Expected |
|---|---|---|
| TP1 | OR_OUT | max(Vint, Vext) − 0.3 V |
| TP2 | BAT_SW | TP1 when SW1 ON, 0 V when OFF |
| TP3 | RAIL_3V3 | 3.30 ± 0.05 V, ripple < 30 mV pp under load |
| TP4 | GPIO35 | Vpack / 2, ≤ 2.1 V |
| TP5 | GNSS die rail | per 2.4 table |
| TP6 | GND_STAR | 0 V reference for all measurements |

Measure ripple with AC coupling, 20 MHz BW limit, short ground spring — not the 15 cm alligator lead.

---

## 5. Bring-up sequence

1. Visual: polarity of SS14 #1/#2 (band = cathode toward fuse), TP4056 protection ICs present, fuse seated.
2. MT3608 standalone trim to 3.30 V, load test 100 mA.
3. Power rail only (no modules): verify TP3, ripple, quiescent current (expect 1–3 mA, dominated by converter + UART if DevKit fitted later).
4. Fit ESP32, flash via USB with SW1 OFF. Verify boot log.
5. Fit Ra-02 **with antenna**. Verify `RF95 init result 0`.
6. Fit OLED. Verify SH1106 at 0x3C.
7. GNSS dropout test (2.4), then fit. Verify NMEA/fix.
8. Battery sense sanity: compare TP4 × 2 against multimeter at pack; adjust override if > 0.1 V off.
9. Hot-swap: internal running → plug charged external → no reboot, TP1 rises; unplug → no reboot, TP1 falls to internal. Repeat 5×.
10. Sleep current: screen off, BLE off, power saving on, measure at TP2 shunt or series ammeter. Expect single-digit mA on DevKit build. Log it; this number is your real autonomy baseline.
11. Range sanity: two nodes, LONG_FAST, known distance, log RSSI/SNR. Compare against stock T-LoRa if available.

---

## 6. Failure modes & mitigations

| Failure | Cause | Mitigation in V1.2 |
|---|---|---|
| Rail overvoltage, all ICs dead | 2S+ pack or 5 V bank into boost-only input | 1S-only rule, labelled packs, JST (not XT30). No electronic OVP on V1.2 — discipline required. V2.0: buck-boost + OVP. |
| Equalisation fire between packs | ORing diodes omitted | SS14 #1/#2 mandatory, verified at bring-up step 1 |
| Reverse pack destroys converter | JST polarity confusion | paint-marked convention + SS14 blocks reverse current from reaching rail (diode reverse-biased; pack sees open circuit) |
| Short under open frame | conductive debris | underside insulation / feet |
| PA degradation | TX without antenna | procedural rule + pigtail strain relief so antenna stays attached |
| GPS resets at cold start | LDO dropout marginal | 2.4 test + tantalum at module + rail trim option |
| Deep sleep never reached | DevKit UART overhead | accepted limitation; measure and document; bare module in V2.0 |
| Battery % lies | voltage-only estimation | ±10–15 % accepted; override trim; coulomb counter is V2.0 |

---

## 7. V1.2 delta summary (for maintainers)

- Boards 60 × 80 mm (reverted from 50 × 70 experiment).
- Internal SMD holder retained, diagonal mount.
- External JST-PH 1S input added.
- SS14 ×2 ORing, 1 A fuse, SS12D06 retained.
- Dummy-load bleed removed (was a autonomy bug).
- ORing diode class changed SS34 → SS14 (leakage).
- GNSS LDO verification made mandatory; rail trim window 3.30–3.45 V documented.
- BOM.csv deleted; parts table moved to README.
- Firmware variant unchanged electrically; no pin changes; `PRIVATE_HW` build as before.
