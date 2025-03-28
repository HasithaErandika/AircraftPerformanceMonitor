# ✈️ APM Phase 1 - Advanced Real-Time Flight Monitor

The **Aircraft Performance Monitor (APM)** is a real-time flight simulation built in C, designed to mimic avionics software. This first phase models dynamic flight phases and allows user interaction.

---

## 🚀 Features
- **Flight Phases**: Automatic transitions through takeoff, climb, and cruise.
  - Climb begins at **80 kt**, cruise starts at **10,000 ft**.
- **Real-Time Tracking**: Continuously monitors:
  - ✈️ **Altitude (ft)**
  - ⚡ **Speed (kt)**
  - ⛽ **Fuel (gal)**
  - 🎛️ **Throttle (0-1 range)**
- **User Interaction**: Adjust throttle mid-flight via keyboard (`t` key, 0-1 range).
- **Warnings & Alerts**:
  - ⚠️ **Low Altitude** (<500 ft)
  - 🚀 **Overspeed** (>400 kt)
  - 🛑 **Stall Risk** (<60 kt)
  - 💥 **Crash Detection** (altitude < 0)
- **Aerospace Touch**: Speed affects climb rate—integrating basic physics.

---

## 📂 Source Code
- **`apm.c`** - Single-file C program.

---

## 🔧 Prerequisites
- **GCC Compiler** (e.g., MinGW-w64 via MSYS2)
- **Windows OS** (uses `<windows.h>` for `Sleep`, `<conio.h>` for `_kbhit`/`_getch`)

---

## 🏗️ Build & Run
1. Open a terminal in the `AircraftPerformanceMonitor` root directory (e.g., VS Code CMD).
2. Compile the program:
   ```bash
   gcc "APMPhase_1/apm.c" -o "APMPhase_1/apm"
   ```
3. Run the program:
   ```bash
   "APMPhase_1/apm"
   ```

---

## 🎮 How to Use
- **Start**: The simulation begins in **takeoff phase** (0 ft, 20 kt, 500 gal, 0.8 throttle).
- **Adjust Throttle**: Press `t`, enter a new throttle value (0-1), then press **Enter**.
- **Exit**: Press `q` to quit the simulation.
- **Live Output**: Displays flight stats every second.

### ✈️ Example Output
```
APM Phase 1 Advanced: 't' to set throttle (0-1), 'q' to quit
Starting Takeoff...
Time: 0 s | Alt: 0 ft | Speed: 24 kt | Fuel: 499.8 gal | Throttle: 0.80 | Phase: Takeoff
Time: 10 s | Alt: 1500 ft | Speed: 90 kt | Fuel: 497.0 gal | Throttle: 0.80 | Phase: Climb
Time: 30 s | Alt: 10000 ft | Speed: 220 kt | Fuel: 492.5 gal | Throttle: 0.80 | Phase: Cruise
```

---

## 📝 Notes
- **Throttle impacts speed and fuel burn**—adjust values in `updateFlight` for realism.
- **Next Step**: Phase 2 introduces **flight logging** (see `APMPhase_2`).

---


## 🎯 Why This Project?
✅ **Comprehensive**: Covers core flight mechanics.  
✅ **Practical**: Simple build and execution steps.  
✅ **Aerospace-Inspired**: Ties into real-world flight dynamics.

