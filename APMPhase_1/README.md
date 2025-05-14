
---

# ✈️ APM Phase 1: Simple Flight Simulator

A lightweight console-based flight simulation written in **C**, modeling a basic aircraft system with throttle control, flight phases, fuel usage, and warnings.

## 🧩 Features

* **Flight Data Simulation**: Models altitude, speed, fuel, throttle, and phase.
* **Phase Transitions**: Automatically shifts from takeoff → climb → cruise.
* **Live Input**: Adjust throttle (`t`) or quit (`q`) in real-time.
* **Flight Warnings**: Alerts for low altitude, overspeed, low fuel, stall, and crash.
* **Realistic Dynamics**: Speed, altitude, and fuel consumption evolve with throttle and phase.

---

## 🚀 How to Run

Launch the simulator:

```bash
./apm.exe
```

You will see flight data updated every second. Use the following keys:

* `t`: Set a new throttle value (between `0.0` and `1.0`)
* `q`: Quit the simulation

---

## 🎮 Example Output

```
APM Phase 1 Advanced: 't' to set throttle (0-1), 'q' to quit
Starting Takeoff...
Time: 0 s | Alt: 0 ft | Speed: 20 kt | Fuel: 500.0 gal | Throttle: 0.80 | Phase: Takeoff
...
WARNING: Low fuel!
Time: 55 s | Alt: 11000 ft | Speed: 350 kt | Fuel: 48.7 gal | Throttle: 0.65 | Phase: Cruise
```

---

## ⚠️ Flight Warnings

* `WARNING: Low altitude!` — Too close to ground (not during takeoff)
* `WARNING: Overspeed!` — Above 400 knots
* `WARNING: Low fuel!` — Below 50 gallons
* `WARNING: Stall risk!` — Speed below 60 knots
* `CRITICAL: Crash!` — Negative altitude or fuel

---

## 🛠️ Program Structure

* **`struct FlightData`**: Holds aircraft state.
* **`updateFlight()`**: Simulates aircraft dynamics.
* **`checkWarnings()`**: Monitors for unsafe conditions.
* **`getPhaseName()`**: Converts phase integers to readable names.
* **`main()`**: Runs the simulation loop.

---
