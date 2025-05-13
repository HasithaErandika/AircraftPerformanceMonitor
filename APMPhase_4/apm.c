#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define GRAVITY 32.174 // ft/s^2
#define PI 3.14159
#define MAX_AIRPORTS 10
#define MAX_ALTITUDE 40000
#define MIN_ALTITUDE 0
#define MAX_SPEED 500
#define MIN_SPEED 0
#define MAX_FUEL 10000
#define MAX_BANK_ANGLE 30
#define MIN_BANK_ANGLE 0

// Aircraft types
typedef enum {
    AIRCRAFT_CESSNA,
    AIRCRAFT_BOEING737,
    AIRCRAFT_AIRBUS320
} AircraftType;

// Weather structure
struct Weather {
    float wind_speed;      // knots
    float wind_direction;  // degrees
    float temperature;     // Celsius
    float pressure;       // hPa
    float visibility;     // nm
    int precipitation;    // 0=none, 1=rain, 2=snow
};

// Airport structure
struct Airport {
    char name[20];
    char code[4];
    float lat;
    float lon;
    float elevation;      // ft
    float runway_length; // ft
    float runway_heading; // degrees
    int has_ils;         // 0 or 1
};

// FlightData structure
struct FlightData {
    float altitude;        // ft
    float speed;          // kt
    float fuel;           // gal
    float throttle;       // 0-1
    float heading;        // deg
    float bank_angle;     // deg
    float turn_radius;    // nm
    float distance_remaining; // nm
    float vertical_speed; // ft/min
    float ground_speed;   // kt
    float true_airspeed;  // kt
    float indicated_airspeed; // kt
    float mach_number;    // Mach
    float g_force;        // G
    float temperature;    // C
    float pressure;       // hPa
    float density_altitude; // ft
    float lat;           // latitude
    float lon;           // longitude
    float prev_altitude; // previous altitude
    int phase;           // 0=Ground, 1=Takeoff, 2=Climb, 3=Cruise, 4=Descent, 5=Landing
    int flaps;           // 0-40 degrees
    int gear;            // 0=up, 1=down
    int autopilot;       // 0=off, 1=on
    int transponder;     // 0=off, 1=on
    AircraftType type;   // Aircraft type
};

// Aircraft performance
struct AircraftPerformance {
    float max_speed;      // knots
    float max_altitude;   // ft
    float max_fuel;       // gal
    float empty_weight;   // lbs
    float max_weight;     // lbs
    float max_thrust;     // lbs
    float fuel_flow;      // gal/hr
    float stall_speed;    // knots
    float vne;           // knots
    float vno;           // knots
    float vfe;           // knots
};

// Global variables
struct Airport airports[MAX_AIRPORTS] = {
    {"Colombo (CMB)", "CMB", 6.9271, 79.8612, 7, 11000, 4, 1},
    {"Delhi (DEL)", "DEL", 28.6139, 77.2090, 777, 12500, 28, 1},
    {"Mumbai (BOM)", "BOM", 19.0887, 72.8679, 37, 11400, 9, 1},
    {"Bangalore (BLR)", "BLR", 13.1979, 77.7063, 3016, 12000, 9, 1},
    {"Chennai (MAA)", "MAA", 12.9900, 80.1634, 52, 12000, 7, 1},
    {"Kathmandu (KTM)", "KTM", 27.6966, 85.3591, 4390, 10000, 2, 0},
    {"Dhaka (DAC)", "DAC", 23.8433, 90.4082, 30, 10500, 14, 1},
    {"Singapore (SIN)", "SIN", 1.3644, 103.9915, 22, 13000, 2, 1},
    {"Kuala Lumpur (KUL)", "KUL", 2.7456, 101.7071, 69, 12400, 14, 1},
    {"Bangkok (BKK)", "BKK", 13.9125, 100.6068, 5, 12000, 19, 1}
};

struct Weather current_weather = {
    10.0, 270.0, 15.0, 1013.25, 10.0, 0
};

struct FlightData plane = {0};
struct AircraftPerformance perf;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;
int flight_time = 0;
int dep_idx = 0, dest_idx = 1;
int running = 1;

// Function declarations
void initAircraftPerformance(AircraftType type);
void calculateAerodynamics(void);
void updateWeather(void);
void calculateWindEffect(void);
void updateNavigation(void);
void checkFlightEnvelope(void);
void updateInstruments(void);
void initFlight(void);
void updateFlight(void);
const char* getPhaseName(int phase);
float calculateDistance(float lat1, float lon1, float lat2, float lon2);
void logData(FILE* log);
void drawText(const char* text, int x, int y, SDL_Color color);
void drawAttitudeIndicator(int x, int y, int size);
void drawAirspeedIndicator(int x, int y, int size);
void drawAltimeter(int x, int y, int size);
void drawHeadingIndicator(int x, int y, int size);
void drawMap(int x, int y, int size);
void renderCockpit(void);

// SDL initialization
int initSDL(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL Init Error: %s\n", SDL_GetError());
        return 0;
    }
    if (TTF_Init() < 0) {
        printf("TTF Init Error: %s\n", TTF_GetError());
        return 0;
    }
    window = SDL_CreateWindow("Aircraft Performance Monitor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    if (!window) {
        printf("Window Error: %s\n", SDL_GetError());
        return 0;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer Error: %s\n", SDL_GetError());
        return 0;
    }
    font = TTF_OpenFont("OpenSans.ttf", 16);
    if (!font) {
        printf("Font Error: %s\n", TTF_GetError());
        return 0;
    }
    return 1;
}

// Cleanup SDL
void cleanupSDL(void) {
    if (font) TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

// Draw text
void drawText(const char* text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Draw attitude indicator
void drawAttitudeIndicator(int x, int y, int size) {
    SDL_SetRenderDrawColor(renderer, 0, 100, 255, 255); // Sky
    SDL_Rect sky = {x - size/2, y - size/2, size, size/2};
    SDL_RenderFillRect(renderer, &sky);
    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // Ground
    SDL_Rect ground = {x - size/2, y, size, size/2};
    SDL_RenderFillRect(renderer, &ground);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, x - size/2, y, x + size/2, y); // Horizon
    char label[20];
    snprintf(label, 20, "Attitude");
    drawText(label, x - 30, y + size/2 + 10, (SDL_Color){255, 255, 255, 255});
}

// Draw airspeed indicator
void drawAirspeedIndicator(int x, int y, int size) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < 360; i += 10) {
        float rad = i * PI / 180;
        int x1 = x + (size/2 - 10) * cos(rad);
        int y1 = y + (size/2 - 10) * sin(rad);
        int x2 = x + size/2 * cos(rad);
        int y2 = y + size/2 * sin(rad);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
    float angle = (plane.speed / perf.max_speed) * 2 * PI - PI/2;
    int nx = x + (size/2 - 5) * cos(angle);
    int ny = y + (size/2 - 5) * sin(angle);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawLine(renderer, x, y, nx, ny);
    char speed[20];
    snprintf(speed, 20, "%d kt", (int)plane.speed);
    drawText(speed, x - 20, y - 10, (SDL_Color){0, 0, 0, 255});
    drawText("Airspeed", x - 30, y + size/2 + 10, (SDL_Color){255, 255, 255, 255});
}

// Draw altimeter
void drawAltimeter(int x, int y, int size) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < 360; i += 10) {
        float rad = i * PI / 180;
        int x1 = x + (size/2 - 10) * cos(rad);
        int y1 = y + (size/2 - 10) * sin(rad);
        int x2 = x + size/2 * cos(rad);
        int y2 = y + size/2 * sin(rad);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
    float angle = (plane.altitude / perf.max_altitude) * 2 * PI - PI/2;
    int nx = x + (size/2 - 5) * cos(angle);
    int ny = y + (size/2 - 5) * sin(angle);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderDrawLine(renderer, x, y, nx, ny);
    char alt[20];
    snprintf(alt, 20, "%d ft", (int)plane.altitude);
    drawText(alt, x - 20, y - 10, (SDL_Color){0, 0, 0, 255});
    drawText("Altitude", x - 30, y + size/2 + 10, (SDL_Color){255, 255, 255, 255});
}

// Draw heading indicator
void drawHeadingIndicator(int x, int y, int size) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < 360; i += 10) {
        float rad = i * PI / 180;
        int x1 = x + (size/2 - 10) * cos(rad);
        int y1 = y + (size/2 - 10) * sin(rad);
        int x2 = x + size/2 * cos(rad);
        int y2 = y + size/2 * sin(rad);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
    float angle = plane.heading * PI / 180 - PI/2;
    int nx = x + (size/2 - 5) * cos(angle);
    int ny = y + (size/2 - 5) * sin(angle);
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderDrawLine(renderer, x, y, nx, ny);
    char hdg[20];
    snprintf(hdg, 20, "%d°", (int)plane.heading);
    drawText(hdg, x - 20, y - 10, (SDL_Color){0, 0, 0, 255});
    drawText("Heading", x - 30, y + size/2 + 10, (SDL_Color){255, 255, 255, 255});
}

// Draw navigation map
void drawMap(int x, int y, int size) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect map = {x - size/2, y - size/2, size, size};
    SDL_RenderFillRect(renderer, &map);
    float scale = size / 30.0; // 30 degrees lat/lon
    for (int i = 0; i < MAX_AIRPORTS; i++) {
        float px = x + (airports[i].lon - plane.lon) * scale;
        float py = y - (airports[i].lat - plane.lat) * scale;
        if (px > x - size/2 && px < x + size/2 && py > y - size/2 && py < y + size/2) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_Rect dot = {(int)px - 2, (int)py - 2, 4, 4};
            SDL_RenderFillRect(renderer, &dot);
            drawText(airports[i].code, (int)px - 10, (int)py + 5, (SDL_Color){255, 255, 255, 255});
        }
    }
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Point plane_shape[] = {{x, y - 10}, {x - 5, y + 5}, {x + 5, y + 5}};
    SDL_RenderDrawLines(renderer, plane_shape, 3);
    drawText("Nav Map", x - 30, y + size/2 + 10, (SDL_Color){255, 255, 255, 255});
}

// Render cockpit
void renderCockpit(void) {
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);
    
    drawAttitudeIndicator(100, 100, 150);
    drawAirspeedIndicator(300, 100, 100);
    drawAltimeter(500, 100, 100);
    drawHeadingIndicator(300, 300, 100);
    drawMap(600, 400, 150);
    
    char info[512];
    snprintf(info, 512,
        "Aircraft Performance Monitor\n"
        "Time: %d s\n"
        "Phase: %s\n"
        "\nFlight Parameters:\n"
        "Altitude: %.0f ft\n"
        "Speed: %.0f kt (GS: %.0f kt)\n"
        "Heading: %.0f°\n"
        "Bank Angle: %.0f°\n"
        "Vertical Speed: %.0f ft/min\n"
        "Fuel: %.1f gal\n"
        "Distance Remaining: %.0f nm\n"
        "\nAircraft State:\n"
        "Flaps: %d°\n"
        "Gear: %s\n"
        "Autopilot: %s\n"
        "Transponder: %s\n"
        "\nWeather:\n"
        "Wind: %.0f kt from %.0f°\n"
        "Temperature: %.1f°C\n"
        "Pressure: %.1f hPa",
        flight_time, getPhaseName(plane.phase),
        plane.altitude, plane.speed, plane.ground_speed,
        plane.heading, plane.bank_angle, plane.vertical_speed,
        plane.fuel, plane.distance_remaining,
        plane.flaps, plane.gear ? "DOWN" : "UP",
        plane.autopilot ? "ON" : "OFF", plane.transponder ? "ON" : "OFF",
        current_weather.wind_speed, current_weather.wind_direction,
        current_weather.temperature, current_weather.pressure);
    drawText(info, 10, 10, (SDL_Color){255, 255, 255, 255});
    
    drawText("Controls:\nT: Throttle\nB: Bank Angle\nF: Flaps\nG: Gear\nA: Autopilot\nX: Transponder\nQ: Quit",
             10, 400, (SDL_Color){255, 255, 255, 255});
    
    SDL_RenderPresent(renderer);
}

// Initialize aircraft performance
void initAircraftPerformance(AircraftType type) {
    switch(type) {
        case AIRCRAFT_CESSNA:
            perf.max_speed = 160; perf.max_altitude = 14000; perf.max_fuel = 56;
            perf.empty_weight = 1670; perf.max_weight = 2550; perf.max_thrust = 230;
            perf.fuel_flow = 10; perf.stall_speed = 47; perf.vne = 163;
            perf.vno = 126; perf.vfe = 85;
            break;
        case AIRCRAFT_BOEING737:
            perf.max_speed = 350; perf.max_altitude = 41000; perf.max_fuel = 6875;
            perf.empty_weight = 91000; perf.max_weight = 174200; perf.max_thrust = 27000;
            perf.fuel_flow = 2500; perf.stall_speed = 108; perf.vne = 350;
            perf.vno = 280; perf.vfe = 230;
            break;
        case AIRCRAFT_AIRBUS320:
            perf.max_speed = 350; perf.max_altitude = 39800; perf.max_fuel = 6875;
            perf.empty_weight = 93000; perf.max_weight = 170000; perf.max_thrust = 27000;
            perf.fuel_flow = 2500; perf.stall_speed = 108; perf.vne = 350;
            perf.vno = 280; perf.vfe = 230;
            break;
    }
}

// Calculate aerodynamics
void calculateAerodynamics(void) {
    float density_ratio = pow((288.15 - 0.0065 * plane.altitude) / 288.15, 4.256);
    float q = 0.5 * 0.002377 * density_ratio * pow(plane.true_airspeed * 1.68781, 2);
    float cl = 1.0;
    float wing_area = 174.0;
    float lift = q * cl * wing_area;
    float cd = 0.02 + 0.1 * pow(cl, 2);
    float drag = q * cd * wing_area;
    plane.g_force = lift / (perf.empty_weight * GRAVITY);
    plane.density_altitude = plane.altitude + (1013.25 - plane.pressure) * 30;
}

// Update weather
void updateWeather(void) {
    current_weather.wind_speed += (rand() % 3 - 1) * 0.5;
    current_weather.wind_direction += (rand() % 3 - 1) * 5.0;
    current_weather.temperature -= 0.0065 * 100;
    current_weather.pressure = 1013.25 * pow((288.15 - 0.0065 * 100) / 288.15, 5.256);
}

// Calculate wind effect
void calculateWindEffect(void) {
    float wind_x = current_weather.wind_speed * sin(current_weather.wind_direction * PI / 180.0);
    float wind_y = current_weather.wind_speed * cos(current_weather.wind_direction * PI / 180.0);
    float aircraft_x = plane.speed * sin(plane.heading * PI / 180.0);
    float aircraft_y = plane.speed * cos(plane.heading * PI / 180.0);
    float ground_x = aircraft_x + wind_x;
    float ground_y = aircraft_y + wind_y;
    plane.ground_speed = sqrt(ground_x * ground_x + ground_y * ground_y);
    plane.true_airspeed = plane.speed;
    plane.indicated_airspeed = plane.true_airspeed * sqrt(current_weather.pressure / 1013.25);
    plane.mach_number = plane.true_airspeed / 661.0;
}

// Update navigation
void updateNavigation(void) {
    float lat_rad = plane.lat * PI / 180.0;
    float lon_rad = plane.lon * PI / 180.0;
    float distance_nm = plane.ground_speed / 3600.0;
    float heading_rad = plane.heading * PI / 180.0;
    plane.lat += (distance_nm * cos(heading_rad)) / 60.0;
    plane.lon += (distance_nm * sin(heading_rad)) / (60.0 * cos(lat_rad));
}

// Check flight envelope
void checkFlightEnvelope(void) {
    if (plane.speed > perf.vne) {
        printf("WARNING: Exceeding VNE!\n");
        plane.speed = perf.vne;
    }
    if (plane.speed < perf.stall_speed) {
        printf("WARNING: Below stall speed!\n");
        plane.speed = perf.stall_speed;
    }
    if (plane.altitude > perf.max_altitude) {
        printf("WARNING: Exceeding maximum altitude!\n");
        plane.altitude = perf.max_altitude;
    }
    if (plane.altitude < MIN_ALTITUDE) {
        printf("WARNING: Below minimum altitude!\n");
        plane.altitude = MIN_ALTITUDE;
    }
    if (plane.g_force > 2.5) {
        printf("WARNING: High G-force!\n");
    }
}

// Update instruments
void updateInstruments(void) {
    plane.vertical_speed = (plane.altitude - plane.prev_altitude) * 60.0;
    plane.prev_altitude = plane.altitude;
}

// Initialize flight
void initFlight(void) {
    float distance = calculateDistance(airports[dep_idx].lat, airports[dep_idx].lon,
                                      airports[dest_idx].lat, airports[dest_idx].lon);
    plane.distance_remaining = distance;
    plane.altitude = airports[dep_idx].elevation;
    plane.prev_altitude = plane.altitude;
    plane.speed = 0;
    plane.fuel = distance * 3.0 + 500;
    plane.throttle = 0.8;
    plane.heading = airports[dep_idx].runway_heading;
    plane.bank_angle = 0;
    plane.turn_radius = 0;
    plane.phase = 0;
    plane.lat = airports[dep_idx].lat;
    plane.lon = airports[dep_idx].lon;
    plane.flaps = 0;
    plane.gear = 1;
    plane.autopilot = 0;
    plane.transponder = 1;
    plane.temperature = current_weather.temperature;
    plane.pressure = current_weather.pressure;
    plane.type = AIRCRAFT_BOEING737;
}

// Update flight
void updateFlight(void) {
    switch (plane.phase) {
        case 0: // Ground
            if (plane.speed > 0) plane.speed -= 1.0;
            break;
        case 1: // Takeoff
            plane.speed += plane.throttle * 10.0;
            plane.fuel -= plane.throttle * 0.3;
            if (plane.speed > 120) {
                plane.phase = 2;
                plane.altitude += 50;
                plane.gear = 0;
            }
            break;
        case 2: // Climb
            plane.speed += plane.throttle * 2.0 - 0.5;
            plane.altitude += plane.speed * 0.5;
            plane.fuel -= plane.throttle * 0.2;
            plane.distance_remaining -= plane.speed / 3600.0;
            if (plane.altitude >= 30000) {
                plane.phase = 3;
                plane.flaps = 0;
            }
            break;
        case 3: // Cruise
            plane.speed += (plane.throttle * 400 - plane.speed) * 0.05;
            plane.altitude += (30000 - plane.altitude) * 0.1;
            plane.fuel -= plane.throttle * 0.15;
            plane.distance_remaining -= plane.speed / 3600.0;
            if (plane.distance_remaining < 100) {
                plane.phase = 4;
                plane.flaps = 10;
            }
            break;
        case 4: // Descent
            plane.speed += (plane.throttle * 300 - plane.speed) * 0.05;
            plane.altitude -= plane.speed * 0.4;
            plane.fuel -= plane.throttle * 0.1;
            plane.distance_remaining -= plane.speed / 3600.0;
            if (plane.altitude <= 5000) {
                plane.phase = 5;
                plane.flaps = 20;
                plane.gear = 1;
            }
            break;
        case 5: // Landing
            plane.speed -= 5.0;
            plane.altitude -= plane.speed * 0.2;
            plane.fuel -= plane.throttle * 0.05;
            plane.distance_remaining -= plane.speed / 3600.0;
            if (plane.altitude <= 0) {
                plane.altitude = 0;
                plane.flaps = 0;
            }
            break;
    }
}

// Log data
void logData(FILE* log) {
    fprintf(log, "[%d s] Phase: %s, Alt: %.0f ft, Speed: %.0f kt (GS: %.0f kt), "
                "Heading: %.0f°, Bank: %.0f°, VS: %.0f ft/min, "
                "Fuel: %.1f gal, Dist: %.0f nm, "
                "Flaps: %d°, Gear: %s, AP: %s, XPDR: %s "
                "[%s -> %s]\n",
            flight_time, getPhaseName(plane.phase),
            plane.altitude, plane.speed, plane.ground_speed,
            plane.heading, plane.bank_angle, plane.vertical_speed,
            plane.fuel, plane.distance_remaining,
            plane.flaps, plane.gear ? "DOWN" : "UP",
            plane.autopilot ? "ON" : "OFF",
            plane.transponder ? "ON" : "OFF",
            airports[dep_idx].name, airports[dest_idx].name);
}

// Get phase name
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

// Calculate distance
float calculateDistance(float lat1, float lon1, float lat2, float lon2) {
    float dlat = (lat2 - lat1) * PI / 180.0;
    float dlon = (lon2 - lon1) * PI / 180.0;
    lat1 *= PI / 180.0; lat2 *= PI / 180.0;
    float a = sin(dlat / 2) * sin(dlat / 2) + cos(lat1) * cos(lat2) * sin(dlon / 2) * sin(dlon / 2);
    float c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return 3440.0 * c;
}

// Main function
int main(int argc, char *argv[]) {
    srand(time(NULL));
    FILE* log = fopen("flight_log.txt", "w");
    if (!log) {
        printf("ERROR: Could not open flight_log.txt!\n");
        return 1;
    }

    if (!initSDL()) {
        fclose(log);
        return 1;
    }

    initAircraftPerformance(plane.type);
    initFlight();

    printf("Flight Plan: %s to %s\n", airports[dep_idx].name, airports[dest_idx].name);
    printf("Distance: %.0f nm | Fuel: %.0f gal\n", plane.distance_remaining, plane.fuel);

    SDL_Event event;
    Uint32 last_update = SDL_GetTicks();

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_t: {
                        printf("Enter throttle (0-1): ");
                        scanf("%f", &plane.throttle);
                        if (plane.throttle < 0 || plane.throttle > 1) plane.throttle = 0.8;
                        break;
                    }
                    case SDLK_b: {
                        printf("Enter bank angle (0-30 deg): ");
                        scanf("%f", &plane.bank_angle);
                        if (plane.bank_angle < MIN_BANK_ANGLE || plane.bank_angle > MAX_BANK_ANGLE)
                            plane.bank_angle = 15;
                        break;
                    }
                    case SDLK_f: {
                        printf("Enter flaps (0-40 deg): ");
                        scanf("%d", &plane.flaps);
                        if (plane.flaps < 0 || plane.flaps > 40) plane.flaps = 0;
                        break;
                    }
                    case SDLK_g:
                        plane.gear = !plane.gear;
                        break;
                    case SDLK_a:
                        plane.autopilot = !plane.autopilot;
                        break;
                    case SDLK_x:
                        plane.transponder = !plane.transponder;
                        break;
                    case SDLK_q:
                        running = 0;
                        break;
                }
            }
        }

        Uint32 current_time = SDL_GetTicks();
        if (current_time - last_update >= 1000) {
            updateFlight();
            calculateAerodynamics();
            calculateWindEffect();
            updateNavigation();
            checkFlightEnvelope();
            updateInstruments();
            updateWeather();
            logData(log);
            flight_time++;
            last_update = current_time;
        }

        renderCockpit();

        if (plane.fuel <= 0 || (plane.phase == 5 && plane.altitude <= 0)) {
            running = 0;
        }

        SDL_Delay(10);
    }

    printf("Flight Ended: %s\n", plane.altitude <= 0 ? "Landed" : "Fuel Out");
    fprintf(log, "[END] Alt: %.0f ft, Speed: %.0f kt, Fuel: %.1f gal, Dist Remain: %.0f nm\n",
            plane.altitude, plane.speed, plane.fuel, plane.distance_remaining);
    
    fclose(log);
    cleanupSDL();
    return 0;
}
