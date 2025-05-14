
---

# ✈️ APM Phase 2: Flight Simulator with Logging

This is **Phase 2** of the APM (Aircraft Performance Model) simulation. It builds on the original console-based aircraft system by adding **flight data logging** to a file (`flight_log.txt`) while maintaining real-time input and warnings.

---

## 🧩 Features

* ✅ **Simulates aircraft behavior**: throttle, speed, altitude, fuel, and flight phase
* ⏱ **Real-time simulation**: console updates every second
* 🔁 **Dynamic flight phases**:

    * `Takeoff` → `Climb` → `Cruise`
* 🧠 **Flight warnings** for:

    * Low altitude
    * Overspeed
    * Stall risk
    * Low fuel
    * Critical crash
* 📝 **Data logging**:

    * Writes flight data and warnings to `flight_log.txt`
    * Logs throttle changes and final stats


---

## 🚀 How to Run

```bash
./apm.exe
```

* Press `t` to set a new throttle (0.0 to 1.0)
* Press `q` to quit the simulation early

---

## 📝 Output Example

```
APM Phase 2: 't' to set throttle (0-1), 'q' to quit
Starting Takeoff... Logging to flight_log.txt
Time: 0 s | Alt: 0 ft | Speed: 20 kt | Fuel: 500.0 gal | Throttle: 0.80 | Phase: Takeoff
...
WARNING: Low fuel!
Time: 52 s | Alt: 10400 ft | Speed: 350 kt | Fuel: 48.3 gal | Throttle: 0.75 | Phase: Cruise
```

The log file `flight_log.txt` might look like:

```
[INPUT] Throttle set to 0.80
[0 s] Alt: 0 ft, Speed: 20 kt, Fuel: 500.0 gal, Throttle: 0.80, Phase: Takeoff
[1 s] Alt: 2 ft, Speed: 24 kt, Fuel: 499.8 gal, Throttle: 0.80, Phase: Takeoff
[WARNING] Low fuel!
...
[END] Alt: 10800 ft, Speed: 360 kt, Fuel: 45.2 gal, Phase: Cruise
```

---

## ⚠️ Flight Warnings

These are shown **in-console** and **logged to file**:

* `Low altitude!` — Altitude < 500 ft outside takeoff
* `Overspeed!` — Speed > 400 knots
* `Low fuel!` — Fuel < 50 gallons
* `Stall risk!` — Speed < 60 knots unless cruising
* `Crash!` — Negative fuel or altitude

---

## 🔍 Key Components

| Function          | Purpose                                  |
| ----------------- | ---------------------------------------- |
| `updateFlight()`  | Simulates physics based on current state |
| `getPhaseName()`  | Converts numeric phase to readable text  |
| `checkWarnings()` | Issues and logs safety alerts            |
| `logData()`       | Logs current flight state to file        |

---
