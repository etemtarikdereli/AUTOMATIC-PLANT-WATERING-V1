# 🌱 Automatic Plant Watering System V1

![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)
![Platform: Arduino](https://img.shields.io/badge/platform-Arduino%20Uno-00979D.svg)
![Status: Field Tested](https://img.shields.io/badge/status-field%20tested-brightgreen.svg)

**Automatic Plant Watering System V1** is a simple, cost-effective, and reliable automatic plant watering system designed for small indoor potted plants.

The system uses an **Arduino Uno**, a **soil moisture sensor**, a **relay module**, and a **small submersible water pump**. It continuously monitors soil moisture levels and automatically activates the pump when the soil becomes too dry. It easily integrates with custom 3D-printed enclosures and water reservoirs.

🖨️ **3D-printed enclosure & mounting parts:** published separately on MakerWorld →
[Automatic Plant Watering V1](https://makerworld.com/en/models/3344450-automatic-plant-watering-v1)

---

## Table of Contents

- [Key Features](#key-features)
- [Required Components](#required-components)
- [Wiring](#wiring)
- [How It Works](#how-it-works)
- [Installation & Calibration](#installation--calibration)
- [Safety & Operational Notes](#safety--operational-notes)
- [Getting Started](#getting-started)
- [Repository Structure](#repository-structure)
- [Future Improvements (V2 Roadmap)](#future-improvements-v2-roadmap)
- [License](#license)
- [Credits](#credits)

---

## Key Features

| Feature | Description |
|---|---|
| **Automatic Moisture Control** | Smart watering based on real-time soil moisture levels |
| **Isolated Power Architecture** | The pump circuit is powered independently from the Arduino to protect the microcontroller |
| **Relay-Controlled Pump** | Safely controls higher-current pumps |
| **Manual Override Button** | Trigger a watering cycle on demand, capped by a safety timer |
| **Hysteresis Control** | Separate ON/OFF thresholds prevent rapid pump cycling |
| **Non-Blocking Timing** | Fully responsive control loop — no long `delay()` freezes |
| **Safety Time Cap** | Hard limit on total consecutive watering time, guarding against sensor faults |
| **Readily Available Components** | Standard, budget-friendly hardware selection |
| **Field-Tested Design** | Successfully tested with a Japanese Hibiscus (*Hibiscus rosa-sinensis*) |

## Required Components

- Arduino Uno
- MH-Series Soil Moisture Sensor Module
- 1-Channel 5V Relay Module
- Mini Submersible Water Pump (3–6V)
- 4×AA Battery Holder & Batteries
- Suitable Water Tubing & Water Reservoir
- Jumper Wires
- Push Button (for manual watering)
- 3D-Printed Enclosure / Mounting Parts — [download on MakerWorld](https://makerworld.com/en/models/3344450-automatic-plant-watering-v1)

## Wiring

| Signal | Arduino Pin |
|---|---|
| Soil moisture sensor (analog out) | A0 |
| Manual button (other leg to GND) | D4 |
| Relay module signal (IN) | D8 |

> ⚠️ The pump is **not** powered from the Arduino. It runs on its own battery pack — the relay only completes/breaks that circuit, keeping pump current away from the Arduino's regulator.

## How It Works

1. **Measurement** — The soil moisture sensor continuously measures soil moisture and sends analog data to the Arduino.
2. **Activation** — When the moisture level drops below a set threshold, the Arduino triggers the relay.
3. **Watering Cycle** — The relay completes the battery-powered pump circuit, transferring water from the reservoir to the plant.
4. **Soaking Period** — After a brief watering burst, the pump stops. The system waits for water to fully distribute through the soil before taking new readings, preventing overwatering and root rot.
5. **Manual Override** — Pressing the button starts a capped watering cycle at any time, independent of the automatic logic.

## Installation & Calibration

- **Pump & Tubing:** Place the submersible pump at the bottom of the water reservoir. Securely attach the water tubing to the plant pot.
- **Sensor Placement:** Insert the sensor near the plant's root zone. Keep it away from the water outlet to prevent fresh water from causing false wet readings.
- **Calibration:** Sensor readings vary by soil type, pot size, and insertion depth. Measure the analog values in dry and fully saturated soil before final deployment, then set the code's `DRY_THRESHOLD` / `WET_THRESHOLD` accordingly (open the Serial Monitor at 9600 baud to read live values).
- **Relay Polarity Check:** Power the board with the button untouched. If the pump turns on immediately, the relay module is active-LOW — set `RELAY_ACTIVE_LOW = true` (default). If the pump stays off as expected, set it to `false`.

## Safety & Operational Notes

- **Electronics Protection:** Keep the Arduino, relay, and battery holder away from moisture.
- **Dry-Run Protection:** Never run the submersible pump dry; ensure it is fully submerged before operation.
- **Power Safety:** Do not power the pump directly from Arduino GPIO pins. Do not connect the battery pack directly to the Arduino 5V pin (use the VIN pin or barrel jack).
- **Initial Supervision:** Monitor the first few watering cycles to prevent leaks or overwatering.

## Getting Started

1. Clone or download this repository.
2. Open `smart_irrigation_system.ino` in the Arduino IDE.
3. Select your board and port under **Tools**.
4. Wire the hardware as described above.
5. Upload the sketch, then open the Serial Monitor (9600 baud) and follow the calibration steps.

## Repository Structure

```
smart-irrigation-system/
├── smart_irrigation_system.ino   # Main Arduino sketch
├── README.md                     # This file
├── LICENSE                       # MIT License (firmware only)
└── .gitignore
```

## Future Improvements (V2 Roadmap)

- Transition to a capacitive soil moisture sensor (to prevent corrosion)
- Water level sensing for low-water alerts
- Rechargeable LiPo/Li-ion battery system with solar panel support
- OLED / LCD display or Wi-Fi (ESP32) integration for status monitoring
- Multi-plant support with solenoid valves or multiple pumps

## License

The **firmware in this repository** is released under the **MIT License** — free to use, modify, and share.

The **3D-printed enclosure and mounting parts** are published separately on MakerWorld under the **MakerWorld Exclusive License**, which does not permit redistribution outside the MakerWorld platform. Please download the print files directly from the [MakerWorld model page](https://makerworld.com/en/models/3344450-automatic-plant-watering-v1) rather than mirroring them here.

## Credits

Designed by [Etem Tarık Dereli](https://makerworld.com/en/@etemtarikdereli) — original hardware design and assembly guide published on MakerWorld.
