#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <math.h>

#define GRAVITY 32.174 // ft/s^2
#define PI 3.14159
#define MAX_AIRPORTS 10

struct Airport {
    char name[20];
    float lat;
    float lon;
};

struct FlightData {
    float altitude;    // ft
    float speed;       // kt
    float fuel;        // gal
    float throttle;    // 0-1
    float heading;     // deg
    float bank_angle;  // deg
    float turn_radius; // nm
    float distance_remaining; // nm
    int phase;         // 0=Ground, 1=Takeoff, 2=Climb, 3=Cruise, 4=Descent, 5=Landing
};

struct Airport airports[MAX_AIRPORTS] = {
    {"Colombo (CMB)", 6.9271, 79.8612},
    {"Delhi (DEL)", 28.6139, 77.2090},
    {"Mumbai (BOM)", 19.0887, 72.8679},
    {"Bangalore (BLR)", 13.1979, 77.7063},
    {"Chennai (MAA)", 12.9900, 80.1634},
    {"Kathmandu (KTM)", 27.6966, 85.3591},
    {"Dhaka (DAC)", 23.8433, 90.4082},
    {"Singapore (SIN)", 1.3644, 103.9915},
    {"Kuala Lumpur (KUL)", 2.7456, 101.7071},
    {"Bangkok (BKK)", 13.9125, 100.6068}
};

float calculateDistance(float lat1, float lon1, float lat2, float lon2) {
    float dlat = (lat2 - lat1) * PI / 180.0;
    float dlon = (lon2 - lon1) * PI / 180.0;
    lat1 *= PI / 180.0; lat2 *= PI / 180.0;
    float a = sin(dlat / 2) * sin(dlat / 2) + cos(lat1) * cos(lat2) * sin(dlon / 2) * sin(dlon / 2);
    float c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return 3440.0 * c;
}

void initFlight(struct FlightData* plane, int dep_idx, int dest_idx) {
    float distance = calculateDistance(airports[dep_idx].lat, airports[dep_idx].lon,
                                       airports[dest_idx].lat, airports[dest_idx].lon);
    plane->distance_remaining = distance;
    plane->altitude = 0; // Start on ground
    plane->speed = 0;
    plane->fuel = distance * 3.0 + 500; // 3 gal/nm + reserve
    plane->throttle = 0.8;
    plane->heading = 0;
    plane->bank_angle = 0;
    plane->turn_radius = 0;
    plane->phase = 0; // Ground
}

void updateFlight(struct FlightData* plane) {
    switch (plane->phase) {
        case 0: // Ground - wait for takeoff
            break;
        case 1: // Takeoff
            plane->speed += plane->throttle * 10.0;
            plane->fuel -= plane->throttle * 0.3;
            if (plane->speed > 120) {
                plane->phase = 2; // Lift off to climb
                plane->altitude += 50;
            }
            break;
        case 2: // Climb
            plane->speed += plane->throttle * 2.0 - 0.5;
            plane->altitude += plane->speed * 0.5; // 500 ft/s climb rate
            plane->fuel -= plane->throttle * 0.2;
            plane->distance_remaining -= plane->speed / 3600.0;
            if (plane->altitude >= 30000) plane->phase = 3; // Cruise
            break;
        case 3: // Cruise
            plane->speed += (plane->throttle * 400 - plane->speed) * 0.05;
            plane->altitude += (30000 - plane->altitude) * 0.1;
            plane->fuel -= plane->throttle * 0.15;
            plane->distance_remaining -= plane->speed / 3600.0;
            if (plane->distance_remaining < 100) plane->phase = 4; // Descent
            break;
        case 4: // Descent
            plane->speed += (plane->throttle * 300 - plane->speed) * 0.05;
            plane->altitude -= plane->speed * 0.4; // 400 ft/s descent
            plane->fuel -= plane->throttle * 0.1;
            plane->distance_remaining -= plane->speed / 3600.0;
            if (plane->altitude <= 5000) plane->phase = 5; // Approach
            break;
        case 5: // Landing
            plane->speed -= 5.0;
            plane->altitude -= plane->speed * 0.2;
            plane->fuel -= plane->throttle * 0.05;
            plane->distance_remaining -= plane->speed / 3600.0;
            if (plane->altitude <= 0) plane->altitude = 0; // Landed
            break;
    }
}

void calculateTurn(struct FlightData* plane) {
    float speed_fps = plane->speed * 1.68781;
    float bank_rad = plane->bank_angle * PI / 180.0;
    plane->turn_radius = (speed_fps * speed_fps) / (GRAVITY * tan(bank_rad)) / 6076.0;
    plane->heading += 3.0;
    if (plane->heading >= 360) plane->heading -= 360;
}

const char* getPhaseName(int phase) {
    switch (phase) {
        case 0: return "Ground";
        case 1: return "Takeoff";
        case 2: return "Climb";
        case 3: return "Cruise";
        case 4: return "Descent";
        case 5: return "Landing";
        default: return "Unknown";
    }
}

void logData(FILE* log, int time, struct FlightData plane, const char* dep, const char* dest) {
    fprintf(log, "[%d s] Alt: %.0f ft, Speed: %.0f kt, Fuel: %.1f gal, Heading: %.0f deg, Dist Remain: %.0f nm, Phase: %s [%s -> %s]\n",
            time, plane.altitude, plane.speed, plane.fuel, plane.heading, plane.distance_remaining, getPhaseName(plane.phase), dep, dest);
}

int main() {
    struct FlightData plane = {0};
    int time = 0, dep_idx, dest_idx;
    char input;
    FILE* log = fopen("APMPhase_3/flight_log.txt", "w");
    if (!log) {
        printf("ERROR: Could not open flight_log.txt!\n");
        return 1;
    }

    // Airport Selection
    printf("Select Departure Airport (0-%d):\n", MAX_AIRPORTS - 1);
    for (int i = 0; i < MAX_AIRPORTS; i++) printf("%d: %s\n", i, airports[i].name);
    scanf("%d", &dep_idx);
    printf("Select Destination Airport (0-%d, != %d):\n", MAX_AIRPORTS - 1, dep_idx);
    do {
        scanf("%d", &dest_idx);
    } while (dest_idx == dep_idx);

    initFlight(&plane, dep_idx, dest_idx);
    printf("\nFlight Plan: %s to %s\n", airports[dep_idx].name, airports[dest_idx].name);
    printf("Distance: %.0f nm | Fuel: %.0f gal\n", plane.distance_remaining, plane.fuel);
    printf("Ready on runway. Request takeoff? (y/n): ");

    // Takeoff Prompt
    while (plane.phase == 0) {
        if (_kbhit()) {
            input = _getch();
            if (input == 'y') {
                plane.phase = 1;
                fprintf(log, "[TAKEOFF] Cleared for takeoff\n");
                printf("\nCleared for takeoff!\n");
            } else if (input == 'n') {
                printf("\nFlight cancelled.\n");
                fclose(log);
                return 0;
            }
        }
    }

    // Main Flight Loop
    while (plane.fuel > 0 && (plane.phase < 5 || plane.altitude > 0)) {
        if (_kbhit()) {
            input = _getch();
            if (input == 't') {
                printf("\nEnter throttle (0-1): ");
                scanf("%f", &plane.throttle);
                if (plane.throttle < 0 || plane.throttle > 1) plane.throttle = 0.8;
                fprintf(log, "[INPUT] Throttle set to %.2f\n", plane.throttle);
            } else if (input == 'b') {
                printf("\nEnter bank angle (5-30 deg): ");
                scanf("%f", &plane.bank_angle);
                if (plane.bank_angle < 5 || plane.bank_angle > 30) plane.bank_angle = 15;
                fprintf(log, "[TURN] Bank angle set to %.0f deg\n", plane.bank_angle);
            } else if (input == 'q') break;
        }

        // Landing Prompt
        if (plane.phase == 5 && plane.distance_remaining <= 10) {
            printf("\nApproaching %s. Request landing? (y/n): ", airports[dest_idx].name);
            while (plane.phase == 5) {
                if (_kbhit()) {
                    input = _getch();
                    if (input == 'y') {
                        fprintf(log, "[LANDING] Cleared to land\n");
                        printf("\nCleared to land!\n");
                        break;
                    } else if (input == 'n') {
                        printf("\nGo-around initiated.\n");
                        plane.phase = 3; // Back to cruise
                        break;
                    }
                }
            }
        }

        updateFlight(&plane);
        if (plane.bank_angle > 0) calculateTurn(&plane);
        printf("Time: %d s | Alt: %.0f ft | Speed: %.0f kt | Fuel: %.1f gal | Dist: %.0f nm | Phase: %s\n",
               time, plane.altitude, plane.speed, plane.fuel, plane.distance_remaining, getPhaseName(plane.phase));
        logData(log, time, plane, airports[dep_idx].name, airports[dest_idx].name);

        Sleep(1000);
        time++;
    }

    printf("Flight Ended: %s\n", plane.altitude <= 0 ? "Landed" : "Fuel Out");
    fprintf(log, "[END] Alt: %.0f ft, Speed: %.0f kt, Fuel: %.1f gal, Dist Remain: %.0f nm\n",
            plane.altitude, plane.speed, plane.fuel, plane.distance_remaining);
    fclose(log);
    return 0;
}