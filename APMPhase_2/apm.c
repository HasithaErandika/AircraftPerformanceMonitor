#include <stdio.h>
#include <windows.h>
#include <conio.h>

struct FlightData {
    float altitude;
    float speed;
    float fuel;
    float throttle;
    int phase;
};

void updateFlight(struct FlightData* plane) {
    if (plane->phase == 0) { // Takeoff
        plane->speed += plane->throttle * 5.0;
        plane->altitude += plane->speed * 0.1;
        plane->fuel -= plane->throttle * 0.3;
        if (plane->speed > 80) plane->phase = 1;
    } else if (plane->phase == 1) { // Climb
        plane->speed += plane->throttle * 2.0 - 0.5;
        plane->altitude += plane->speed * 0.5;
        plane->fuel -= plane->throttle * 0.2;
        if (plane->altitude > 10000) plane->phase = 2;
    } else { // Cruise
        plane->speed += (plane->throttle * 250 - plane->speed) * 0.05;
        plane->altitude += (10000 - plane->altitude) * 0.1;
        plane->fuel -= plane->throttle * 0.15;
    }
}

const char* getPhaseName(int phase) { // Moved up
    switch (phase) {
        case 0: return "Takeoff";
        case 1: return "Climb";
        case 2: return "Cruise";
        default: return "Unknown";
    }
}

void logData(FILE* log, int time, struct FlightData plane) {
    fprintf(log, "[%d s] Alt: %.0f ft, Speed: %.0f kt, Fuel: %.1f gal, Throttle: %.2f, Phase: %s\n",
            time, plane.altitude, plane.speed, plane.fuel, plane.throttle, getPhaseName(plane.phase));
}

void checkWarnings(struct FlightData plane, FILE* log) {
    if (plane.altitude < 500 && plane.phase != 0) {
        printf("WARNING: Low altitude!\n");
        fprintf(log, "[WARNING] Low altitude!\n");
    }
    if (plane.speed > 400) {
        printf("WARNING: Overspeed!\n");
        fprintf(log, "[WARNING] Overspeed!\n");
    }
    if (plane.fuel < 50) {
        printf("WARNING: Low fuel!\n");
        fprintf(log, "[WARNING] Low fuel!\n");
    }
    if (plane.speed < 60 && plane.phase != 2) {
        printf("WARNING: Stall risk!\n");
        fprintf(log, "[WARNING] Stall risk!\n");
    }
    if (plane.altitude < 0 || plane.fuel < 0) {
        printf("CRITICAL: Crash!\n");
        fprintf(log, "[CRITICAL] Crash!\n");
    }
}

int main() {
    struct FlightData plane = {0, 20, 500, 0.8, 0};
    int time = 0;
    char input;
    FILE* log = fopen("flight_log.txt", "w");

    if (log == NULL) {
        printf("ERROR: Could not open flight_log.txt!\n");
        return 1;
    }

    printf("APM Phase 2: 't' to set throttle (0-1), 'q' to quit\n");
    printf("Starting Takeoff... Logging to flight_log.txt\n");

    while (plane.fuel > 0 && plane.altitude >= 0) {
        if (_kbhit()) {
            input = _getch();
            if (input == 't') {
                printf("\nEnter throttle (0-1): ");
                scanf("%f", &plane.throttle);
                if (plane.throttle < 0 || plane.throttle > 1) plane.throttle = 0.8;
                fprintf(log, "[INPUT] Throttle set to %.2f\n", plane.throttle);
            }
            if (input == 'q') break;
        }

        updateFlight(&plane);
        printf("Time: %d s | Alt: %.0f ft | Speed: %.0f kt | Fuel: %.1f gal | Throttle: %.2f | Phase: %s\n",
               time, plane.altitude, plane.speed, plane.fuel, plane.throttle, getPhaseName(plane.phase));
        logData(log, time, plane);
        checkWarnings(plane, log);

        Sleep(1000);
        time++;
    }

    printf("Flight Ended. Final Stats:\n");
    printf("Alt: %.0f ft | Speed: %.0f kt | Fuel: %.1f gal | Phase: %s\n",
           plane.altitude, plane.speed, plane.fuel, getPhaseName(plane.phase));
    fprintf(log, "[END] Alt: %.0f ft, Speed: %.0f kt, Fuel: %.1f gal, Phase: %s\n",
            plane.altitude, plane.speed, plane.fuel, getPhaseName(plane.phase));

    fclose(log);
    return 0;
}