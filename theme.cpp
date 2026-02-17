#include "theme.h"
#include <string>

Color GREEN_PHOSPHOR  = {0, 255, 100, 255};
Color AMBER_PHOSPHOR  = {255, 200, 50, 255};
Color YELLOW_ALERT    = {255, 255, 0, 255};
Color DIM_GREEN       = {0, 150, 50, 255};
Color CYAN_HIGHLIGHT  = {0, 255, 255, 255};
const Color COLOR_BLACK = {0, 0, 0, 255};
int currentTheme = THEME_GREEN_PHOSPHOR;

// Forward declared here - defined in dashboard.cpp
void AddLogEntry(const std::string& message, Color entryColor);

void ApplyColorTheme(int theme) {
    if (theme < 0 || theme >= THEME_COUNT) theme = 0;
    currentTheme     = theme;
    GREEN_PHOSPHOR   = PALETTES[theme].primary;
    AMBER_PHOSPHOR   = PALETTES[theme].secondary;
    YELLOW_ALERT     = PALETTES[theme].accent;
    DIM_GREEN        = PALETTES[theme].dim;
    CYAN_HIGHLIGHT   = PALETTES[theme].highlight;
    AddLogEntry("[THEME] Applied: " + std::string(THEME_NAMES[theme]), CYAN_HIGHLIGHT);
}
