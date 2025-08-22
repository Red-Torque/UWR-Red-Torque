
# Design Approach v1 — RT‑X7 System Architecture & Control Flow

**Labels:** `design`, `architecture`, `control`, `v1`  
**Milestone:** Alpha Bring‑up  
**Assignees:** @red-torque/core-team

---

## Why
We need a first, reviewable **Design 1** for RT‑X7 that every teammate can understand and rebuild: who does what (Raspberry Pi 5 vs ESP32), how sensors map to behaviors, and what interfaces we will lock for the first integrated demo.

## Goals
- Clear **division of responsibilities** between Raspberry Pi 5 and ESP32.
- Document **sensor layout** (6× ultrasonic: 3 front, 3 rear) and behaviors (wall avoidance, front stop, reverse guard).
- Define **data contracts** between Pi↔ESP32 (message IDs, units, rates, failsafes).
- Capture **control modes**: Vision‑driven targeting (cubes), Obstacle guard, Manual override.
- Produce a **definition of done** for v1 with integration tests.

## Constraints & Assumptions
- Controller: **Raspberry Pi 5** (primary brain, vision & global logic).  
- **ESP32**: real‑time I/O for ultrasonics + motor drivers (drive + steer).  
- Camera: Pi Camera via CSI.  
- Comms: **UART** (fallback: I²C) between Pi and ESP32.  
- Power domains isolated; common ground.  
- WRO FE rules: safety first, speed capped for indoor track; no external compute during runs.

## High‑Level Block Diagram (Design 1)
> Attach this image to the issue (drag & drop), or commit it in `/docs/diagrams/` and paste the link below.

![RT‑X7 Design 1 Diagram](rtx7-design-1-architecture.jpg)

*Legend:*  
- **Pi 5**: vision (cube detection), high‑level planner, arbitration, logging.  
- **ESP32**: ultrasonic polling, PWM to **Drive Motor**, servo/PWM to **Steer Motor**.  
- **Ultrasonics (Front L/C/R, Rear L/C/R)**: wall avoidance, stop, reverse guard.

## Interfaces (Draft v1)
**UART 115200 8N1**

| Msg ID | Dir | Name | Payload | Rate |
|---|---|---|---|---|
| 0x10 | Pi→ESP | `SET_DRIVE` | int16 speed_mmps (- = reverse) | on change |
| 0x11 | Pi→ESP | `SET_STEER` | int16 steer_deg (- = left) | on change |
| 0x12 | Pi→ESP | `MODE` | u8 {0=IDLE,1=AUTO,2=MANUAL} | on change |
| 0x20 | ESP→Pi | `ULTRA_FRONT` | u16 L, u16 C, u16 R (mm) | 20 Hz |
| 0x21 | ESP→Pi | `ULTRA_REAR` | u16 L, u16 C, u16 R (mm) | 20 Hz |
| 0x22 | ESP→Pi | `HEARTBEAT` | u8 status | 2 Hz |
| 0x30 | Pi→ESP | `E_STOP` | none | immediate |

**Failsafe:** If no Pi heartbeat or command for >300 ms, ESP32 forces `speed=0`, centers steering, and signals `status=FAILSAFE`.

## Control Modes (v1)
1. **AUTO – Vision Primary, Guard by Ultrasonics**
   - Pi runs cube detection; publishes steering & speed.
   - ESP32 enforces **guard rails**:
     - Front center < *stop_threshold* → override speed=0.
     - Front left/right < *wall_threshold* → clamp steering away from wall.
     - Rear sensors used while reversing; block if obstacle.
2. **MANUAL** (for testing): Pi forwards joystick/keyboard commands; ESP32 still guards stops.
3. **IDLE**: motors disabled; sensors alive.

## Sensor Layout & Thresholds (initial)
- Front: L/R angled ~25–30° outward; Center straight.  
- Rear: L/R angled similarly; Center straight.  
- `stop_threshold` = 250 mm, `wall_threshold` = 180 mm (tune on track).

## Test Plan (Definition of Done for v1)
- Bench: UART loopback verified; message framing robust to noise.
- Sensor: Each ultrasonic stable within ±10 mm at 0.2–1.5 m; publish at 20 Hz.
- Actuation: Drive PWM linearization table made; steer neutral calibrated (±2°).
- Safety: E‑Stop tested; loss of comms → stop within 300 ms.
- Track dry‑run: Robot starts only after Pi is ready; goes straight when no cubes visible; turns only when cubes detected; stops for front obstacle.
- Logs: CSV of distances & commands saved on Pi with timestamp.

## Open Questions
- Keep UART or move to I²C/SPI for lower latency?
- Do we need a low‑pass or median filter on ultrasonic streams on the ESP32?
- PID split: steering PID on Pi or ESP32? (Current: Pi computes angle; ESP applies raw PWM.)

## Tasks
- [ ] Finalize UART spec & error handling.
- [ ] ESP32 FW: sensor polling + guard logic + motor drivers.
- [ ] Pi SW: vision module, arbitration, logger, CLI.
- [ ] Wiring diagram & pin map (rev A).
- [ ] Threshold tuning session; record results.
- [ ] Update README with Design 1 and diagram.
- [ ] Review & sign‑off by team.

---

**How to use this issue**
1) Paste this body when creating the GitHub issue titled: **“Design 1 approach – RT‑X7”**.  
2) Upload the attached image `rtx7-design-1-architecture.jpg` into the issue so it renders above.  
3) Convert checked tasks into PRs and link them here.
