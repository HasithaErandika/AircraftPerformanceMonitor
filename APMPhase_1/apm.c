#include <stdio.h>
#include <windows.h>
#include <conio.h> 

// Flight data struct
struct FlightData {
    float altitude;    // feet
    float speed;       // knots
    float fuel;        // gallons
    float throttle;    // 0.0 to 1.0 (power setting)
    int phase;         // 0 = takeoff, 1 = climb, 2 = cruise
};

// Update flight based on throttle and phase
void updateFlight(struct FlightData* plane) {
    if (plane->phase == 0) { // Takeoff
        plane->speed += plane->throttle * 5.0; // Rapid acceleration
        plane->altitude += plane->speed * 0.1;  // Slow initial climb
        plane->fuel -= plane->throttle * 0.3;
        if (plane->speed > 80) plane->phase = 1; // Switch to climb
    } else if (plane->phase == 1) { // Climb
        plane->speed += plane->throttle * 2.0 - 0.5; // Slower accel, drag
        plane->altitude += plane->speed * 0.5;        // Speed drives climb
        plane->fuel -= plane->throttle * 0.2;
        if (plane->altitude > 10000) plane->phase = 2; // Cruise at 10k ft
    } else { // Cruise
        plane->speed += (plane->throttle * 250 - plane->speed) * 0.05; // Stabilize
        plane->altitude += (10000 - plane->altitude) * 0.1; // Hold altitude
        plane->fuel -= plane->throttle * 0.15;
    }
}

// Warnings
void checkWarnings(struct FlightData plane) {
    if (plane.altitude < 500 && plane.phase != 0) printf("WARNING: Low altitude!\n");
    if (plane.speed > 400) printf("WARNING: Overspeed!\n");
    if (plane.fuel < 50) printf("WARNING: Low fuel!\n");
    if (plane.speed < 60 && plane.phase != 2) printf("WARNING: Stall risk!\n");
    if (plane.altitude < 0 || plane.fuel < 0) printf("CRITICAL: Crash!\n");
}

// Phase name helper
const char* getPhaseName(int phase) {
    switch (phase) {
        case 0: return "Takeoff";
        case 1: return "Climb";
        case 2: return "Cruise";
        default: return "Unknown";
    }
}

int main() {
    struct FlightData plane = {0, 20, 500, 0.8, 0}; // Start on runway
    int time = 0;
    char input;

    printf("APM Phase 1 Advanced: 't' to set throttle (0-1), 'q' to quit\n");
    printf("Starting Takeoff...\n");

    while (plane.fuel > 0 && plane.altitude >= 0) {
        // Check for throttle input
        if (_kbhit()) { // Non-blocking input
            input = _getch();
            if (input == 't') {
                printf("\nEnter throttle (0-1): ");
                scanf("%f", &plane.throttle);
                if (plane.throttle < 0 || plane.throttle > 1) plane.throttle = 0.8;
            }
            if (input == 'q') break;
        }

        // Update and display
        updateFlight(&plane);
        printf("Time: %d s | Alt: %.0f ft | Speed: %.0f kt | Fuel: %.1f gal | Throttle: %.2f | Phase: %s\n",
               time, plane.altitude, plane.speed, plane.fuel, plane.throttle, getPhaseName(plane.phase));
        checkWarnings(plane);

        Sleep(1000); // 1-sec tick
        time++;
    }

    printf("Flight Ended. Final Stats:\n");
    printf("Alt: %.0f ft | Speed: %.0f kt | Fuel: %.1f gal | Phase: %s\n",
           plane.altitude, plane.speed, plane.fuel, getPhaseName(plane.phase));
    return 0;
}