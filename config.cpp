#include "config.h"
#include "theme.h"
#include "dashboard.h"
#include "raylib.h"
#include <fstream>
#include <string>

static std::string GetConfigPath() {
    return std::string(GetApplicationDirectory()) + "dashboard.cfg";
}

void SaveConfig() {
    std::ofstream f(GetConfigPath());
    if (!f.is_open()) return;
    f << "first_run=0\n";
    f << "theme=" << currentTheme << "\n";
    bool* ws[WIDGET_COUNT];
    GetWidgetStates(ws);
    for (int i = 0; i < WIDGET_COUNT; i++)
        f << "widget_" << i << "=" << (*ws[i] ? 1 : 0) << "\n";
}

void LoadConfig() {
    std::ifstream f(GetConfigPath());
    if (!f.is_open()) {
        // No config file = genuine first run
        isFirstRun = true;
        return;
    }
    bool* ws[WIDGET_COUNT];
    GetWidgetStates(ws);
    std::string line;
    while (std::getline(f, line)) {
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        std::string val = line.substr(eq + 1);
        if (val.empty()) continue;
        try {
            int v = std::stoi(val);
            if      (key == "first_run") isFirstRun = (v != 0);
            else if (key == "theme")     currentTheme = (v >= 0 && v < THEME_COUNT) ? v : 0;
            else if (key.size() > 7 && key.substr(0, 7) == "widget_") {
                int idx = std::stoi(key.substr(7));
                if (idx >= 0 && idx < WIDGET_COUNT) *ws[idx] = (v != 0);
            }
        } catch (...) { continue; }
    }
}
