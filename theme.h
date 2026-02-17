#pragma once
#include "raylib.h"
#include <string>

// ── Color theme definitions ───────────────────────────────────────────────────

enum ColorTheme {
    THEME_GREEN_PHOSPHOR = 0,
    THEME_AMBER_MONOCHROME,
    THEME_CYAN_TERMINAL,
    THEME_WHITE_CLASSIC,
    THEME_RED_ALERT,
    THEME_COUNT
};

inline const char* THEME_NAMES[] = {
    "Green Phosphor (Classic)",
    "Amber Monochrome",
    "Cyan Terminal",
    "White Classic",
    "Red Alert"
};

struct ColorPalette {
    Color primary;
    Color secondary;
    Color accent;
    Color dim;
    Color highlight;
};

inline const ColorPalette PALETTES[] = {
    { {0,   255, 100, 255}, {255, 200,  50, 255}, {255, 255,   0, 255}, {  0, 150,  50, 255}, {  0, 255, 255, 255} },
    { {255, 176,   0, 255}, {255, 140,   0, 255}, {255, 200, 100, 255}, {180, 120,   0, 255}, {255, 220, 150, 255} },
    { {  0, 255, 255, 255}, {  0, 200, 255, 255}, {100, 255, 255, 255}, {  0, 150, 180, 255}, {150, 255, 255, 255} },
    { {255, 255, 255, 255}, {200, 200, 200, 255}, {255, 255, 200, 255}, {150, 150, 150, 255}, {255, 255, 255, 255} },
    { {255,  50,  50, 255}, {255, 100,   0, 255}, {255, 200,   0, 255}, {180,  30,  30, 255}, {255, 150, 150, 255} }
};

// ── Active theme color variables (updated by ApplyColorTheme) ─────────────────
extern Color GREEN_PHOSPHOR;
extern Color AMBER_PHOSPHOR;
extern Color YELLOW_ALERT;
extern Color DIM_GREEN;
extern Color CYAN_HIGHLIGHT;
extern const Color COLOR_BLACK;
extern int currentTheme;

// ── Functions ─────────────────────────────────────────────────────────────────
void ApplyColorTheme(int theme);
