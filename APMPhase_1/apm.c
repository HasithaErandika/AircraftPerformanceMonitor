#include <stdio.h>
#include <windows.h> // Sleep() for Windows; swap to <unistd.h> and sleep() for Linux/Mac

// Struct for flight data
struct FlightData {
    float altitude;  // feet
    float speed;     // knots
    float fuel;      // gallons
};

// Check and print warnings
void checkWarnings(struct FlightData plane) {
    if (plane.altitude < 1000) {
        printf("WARNING: Altitude below 1000 ft!\n");
    }
    if (plane.speed > 400) {
        printf("WARNING: Speed exceeds 400 knots!\n");
    }
    if (plane.fuel < 50) {
        printf("WARNING: Fuel below 50 gallons!\n");
    }
    if (plane.altitude < 0 || plane.fuel < 0) {
        printf("CRITICAL: Crash detected!\n");
    }
}

int main() {
    // Initial conditions
    struct FlightData plane = {12000, 250, 500}; // 12k ft, 250 kt, 500 gal
    int time = 0;

    printf("Aircraft Performance Monitor Starting...\n");

    // Simulate 50 seconds
    while (time < 50 && plane.fuel > 0 && plane.altitude > 0) {
        plane.altitude -= 200;  // Descend 200 ft/sec
        plane.speed += 2;       // Speed up slightly
        plane.fuel -= 0.5;      // Burn fuel

        printf("Time: %d s | Alt: %.0f ft | Speed: %.0f kt | Fuel: %.1f gal\n",
               time, plane.altitude, plane.speed, plane.fuel);
        checkWarnings(plane);

        Sleep(1000); // 1-sec delay

        time++;
    }

    printf("Flight Ended. Final Stats:\n");
    printf("Alt: %.0f ft | Speed: %.0f kt | Fuel: %.1f gal\n",
           plane.altitude, plane.speed, plane.fuel);
    return 0;
}