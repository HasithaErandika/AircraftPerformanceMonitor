# ✈️ APM Phase 2 - Flight Monitor with Data Logging

The **Aircraft Performance Monitor (APM) - Phase 2** builds upon Phase 1 by introducing **flight data logging**, effectively simulating an aircraft's **black box** in C.

---

## 🚀 Features
- **Includes All Phase 1 Features**:
  - Automatic flight phases: **Takeoff, Climb, Cruise**
  - **Throttle Control** via keyboard (`t` key)
  - **Warnings & Alerts** (stall, overspeed, low altitude, crash detection)
- **Real-Time Data Logging**:
  - Records every second: **Time, Altitude, Speed, Fuel, Throttle, Flight Phase**
  - Saves to `flight_log.txt`
- **Warning Logs**: Captures alerts (e.g., "⚠️ Low Fuel!") in the log file.
- **Input Tracking**: Logs throttle changes for traceability.
- **Robust Logging System**:
  - **Error Handling** for file operations—avionics-grade reliability.

---

## 📂 Source Code
- **`apm.c`** - Main C program with logging functionality.
- **`flight_log.txt`** - Output log file (generated in `APMPhase_2/`).

---

## 🔧 Prerequisites
- **GCC Compiler** (e.g., MinGW-w64 via MSYS2)
- **Windows OS** (uses `<windows.h>` and `<conio.h>` for real-time input and delays)

---

## 🏗️ Build & Run
1. Open a terminal in the `AircraftPerformanceMonitor` root directory (e.g., VS Code CMD).
2. Compile the program:
   ```bash
   gcc "APMPhase_2/apm.c" -o "APMPhase_2/apm"
   ```
3. Run the program:
   ```bash
   "APMPhase_2/apm"
   ```

---

## 🎮 How to Use
- **Start**: The simulation begins in **takeoff phase** and logs data to `flight_log.txt`.
- **Adjust Throttle**: Press `t`, enter a new throttle value (0-1), then press **Enter**.
- **Exit**: Press `q` to quit the simulation.
- **Monitor Data**: Console displays real-time flight stats, while `flight_log.txt` stores all recorded data.
- **Post-Flight Analysis**: Open `APMPhase_2/flight_log.txt` to review recorded events.

---

## 📊 Example Output
### **Console Output**:
```
APM Phase 2: 't' to set throttle (0-1), 'q' to quit
Starting Takeoff... Logging to flight_log.txt
Time: 0 s | Alt: 0 ft | Speed: 24 kt | Fuel: 499.8 gal | Throttle: 0.80 | Phase: Takeoff
Time: 50 s | Alt: 10000 ft | Speed: 230 kt | Fuel: 490.0 gal | Throttle: 0.80 | Phase: Cruise
WARNING: Low fuel!
```

### **`flight_log.txt` Output**:
```
[0 s] Alt: 0 ft, Speed: 24 kt, Fuel: 499.8 gal, Throttle: 0.80, Phase: Takeoff
[1 s] Alt: 2 ft, Speed: 28 kt, Fuel: 499.5 gal, Throttle: 0.80, Phase: Takeoff
...
[50 s] Alt: 10000 ft, Speed: 230 kt, Fuel: 490.0 gal, Throttle: 0.80, Phase: Cruise
[WARNING] Low fuel!
[END] Alt: 10000 ft, Speed: 230 kt, Fuel: 48.0 gal, Phase: Cruise
```

---

## 📝 Notes
- **Log format is human-readable**—Phase 3 will introduce data parsing (e.g., CSV format for analysis).
- **File Overwrites on Each Run**—can be modified to append mode (`"a"`).
- **Aerospace Connection**: Simulates real-world **Flight Data Recorders (FDRs)**.

---

## 🎯 Why This Project Stands Out
✅ **Comprehensive**: Real-time logging bridges software and aviation.  
✅ **Clear & Practical**: Build and execution instructions align with Phase 1.  
✅ **Future-Oriented**: Introduces data storage, setting up Phase 3 for analysis tools.

