#include "raylib.h"
#include "theme.h"
#include "dashboard.h"
#include "ui_menus.h"
#include "terminal.h"
#include "config.h"
#include "system_monitor.h"
#include <string>
#include <cstdio>

// ── HandleMenuSelection ───────────────────────────────────────────────────────
// Defined here (not dashboard.cpp) to avoid circular includes with ui_menus/terminal
void HandleMenuSelection() {
    switch (selectedOption) {
        case MENU_DASHBOARD:
            currentMenu = MENU_DASHBOARD;
            stats.useRealData = false;
            showMenu = false;
            AddLogEntry("[MENU] Dashboard View (Simulated)", CYAN_HIGHLIGHT);
            break;

        case MENU_REAL_MONITORING:
            currentMenu = MENU_REAL_MONITORING;
            stats.useRealData = true;
            showMenu = false;
            AddLogEntry("[MENU] Real-time monitoring ENABLED", CYAN_HIGHLIGHT);
            stats.targetCpu  = GetRealCPUUsage();
            stats.targetRam  = GetRealRAMUsage();
            stats.targetDisk = GetRealDiskUsage();
            break;

        case MENU_NETWORK_TEST:
            currentMenu = MENU_NETWORK_TEST;
            showMenu = false;
            AddLogEntry("[NET] Network diagnostics - coming soon", YELLOW_ALERT);
            break;

        case MENU_SYSTEM_INFO:
            currentMenu = MENU_SYSTEM_INFO;
            showMenu = false;
            AddLogEntry("[MENU] System Information - coming soon", YELLOW_ALERT);
            break;

        case MENU_CUSTOMIZE_WIDGETS:
            showWidgetMenu = true;
            showMenu = false;
            selectedWidget = 0;
            AddLogEntry("[MENU] Widget customization opened", CYAN_HIGHLIGHT);
            break;

        case MENU_COLOR_THEMES:
            showColorMenu = true;
            showMenu = false;
            selectedTheme = currentTheme;   // pre-select current theme
            AddLogEntry("[MENU] Color theme selector opened", CYAN_HIGHLIGHT);
            break;

        case MENU_TERMINAL:
            showTerminal = true;
            showMenu = false;
            if (terminalOutput.empty())
                terminalOutput.push_back("CRT Dashboard Terminal  -  type commands and press ENTER");
            AddLogEntry("[TERMINAL] Terminal opened", CYAN_HIGHLIGHT);
            break;
    }
}

// ── main ──────────────────────────────────────────────────────────────────────
int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    // Font
    retroFont = LoadFont("resources/fonts/VGA.ttf");
    hasCustomFont = (retroFont.texture.id > 0);
    if (!hasCustomFont) retroFont = GetFontDefault();

    // Config (sets isFirstRun, currentTheme, widget states)
    LoadConfig();
    ApplyColorTheme(currentTheme);

    InitializeStats();
    InitializeSystemMonitoring();

    // Onboarding on first run
    if (isFirstRun) {
        showOnboarding = true;
        onboardingStep = 0;
        selectedTheme  = THEME_GREEN_PHOSPHOR;
    }

    // Shader
    const char* appDir = GetApplicationDirectory();
    std::string shaderPath = std::string(appDir) + "resources/shaders/crt.fsh";
    Shader crtShader = LoadShader(0, shaderPath.c_str());
    if (crtShader.id == 0)
        crtShader = LoadShader(0, "resources/shaders/crt.fsh");

    int resLoc  = GetShaderLocation(crtShader, "resolution");
    int timeLoc = GetShaderLocation(crtShader, "time");
    int tintLoc = GetShaderLocation(crtShader, "phosphorTint");

    RenderTexture2D target = LoadRenderTexture(WINDOW_WIDTH, WINDOW_HEIGHT);
    float res[2] = { (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT };
    SetShaderValue(crtShader, resLoc, res, SHADER_UNIFORM_VEC2);

    AddLogEntry("[SYSTEM] Initializing mainframe...", GREEN_PHOSPHOR);
    AddLogEntry("[BOOT]   Loading kernel modules...", GREEN_PHOSPHOR);
    AddLogEntry("[NET]    Network interface up",      GREEN_PHOSPHOR);
    AddLogEntry("[INPUT]  Press TAB to open menu",    CYAN_HIGHLIGHT);

    // ── Main loop ─────────────────────────────────────────────────────────────
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        timeAccumulator += dt;
        menuBlinkTimer  += dt;

        // ── Input: Terminal (highest priority when open) ───────────────────
        if (showTerminal) {
            int ch = GetCharPressed();
            while (ch > 0) {
                if (ch >= 32 && ch < 127) terminalInput += (char)ch;
                ch = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && !terminalInput.empty())
                terminalInput.pop_back();
            if (IsKeyPressed(KEY_ENTER) && !terminalInput.empty()) {
                if (terminalInput.size() >= 3 && terminalInput.substr(0, 3) == "cd ") {
                    std::string dir = terminalInput.substr(3);
                    std::string probe = "cd /d \"" + terminalCwd + "\" && cd \"" + dir + "\" && cd";
                    FILE* p = _popen(probe.c_str(), "r");
                    if (p) {
                        char buf[512] = {};
                        fgets(buf, sizeof(buf), p);
                        _pclose(p);
                        std::string nd(buf);
                        while (!nd.empty() && (nd.back()=='\n'||nd.back()=='\r')) nd.pop_back();
                        if (!nd.empty()) terminalCwd = nd;
                    }
                    terminalOutput.push_back("> " + terminalCwd + " $ " + terminalInput);
                } else {
                    RunTerminalCommand(terminalInput);
                }
                terminalInput.clear();
            }
            if (IsKeyPressed(KEY_ESCAPE)) showTerminal = false;
        }

        // ── Input: Onboarding ─────────────────────────────────────────────
        else if (showOnboarding) {
            if (onboardingStep == 0) {
                if (IsKeyPressed(KEY_ENTER))  onboardingStep = 1;
                if (IsKeyPressed(KEY_ESCAPE)) {
                    showOnboarding = false;
                    isFirstRun     = false;
                    ApplyColorTheme(THEME_GREEN_PHOSPHOR);
                    SaveConfig();
                }
            } else {
                if (IsKeyPressed(KEY_UP))   { selectedTheme = (selectedTheme - 1 + THEME_COUNT) % THEME_COUNT; menuBlinkTimer = 0; }
                if (IsKeyPressed(KEY_DOWN)) { selectedTheme = (selectedTheme + 1) % THEME_COUNT; menuBlinkTimer = 0; }
                if (IsKeyPressed(KEY_ENTER)) {
                    ApplyColorTheme(selectedTheme);
                    showOnboarding = false;
                    isFirstRun     = false;
                    SaveConfig();
                }
                if (IsKeyPressed(KEY_ESCAPE)) {
                    ApplyColorTheme(THEME_GREEN_PHOSPHOR);
                    showOnboarding = false;
                    isFirstRun     = false;
                    SaveConfig();
                }
            }
        }

        // ── Input: Color menu ─────────────────────────────────────────────
        else if (showColorMenu) {
            if (IsKeyPressed(KEY_UP))   { selectedTheme = (selectedTheme - 1 + THEME_COUNT) % THEME_COUNT; menuBlinkTimer = 0; }
            if (IsKeyPressed(KEY_DOWN)) { selectedTheme = (selectedTheme + 1) % THEME_COUNT; menuBlinkTimer = 0; }
            if (IsKeyPressed(KEY_ENTER)) {
                ApplyColorTheme(selectedTheme);
                SaveConfig();
                showColorMenu = false;
                AddLogEntry("[THEME] Applied: " + std::string(THEME_NAMES[selectedTheme]), CYAN_HIGHLIGHT);
            }
            if (IsKeyPressed(KEY_ESCAPE)) {
                showColorMenu = false;
                AddLogEntry("[THEME] Selection cancelled", DIM_GREEN);
            }
        }

        // ── Input: Widget menu ────────────────────────────────────────────
        else if (showWidgetMenu) {
            if (IsKeyPressed(KEY_UP))   { selectedWidget = (selectedWidget - 1 + WIDGET_COUNT) % WIDGET_COUNT; menuBlinkTimer = 0; }
            if (IsKeyPressed(KEY_DOWN)) { selectedWidget = (selectedWidget + 1) % WIDGET_COUNT; menuBlinkTimer = 0; }
            if (IsKeyPressed(KEY_SPACE)) {
                bool* ws[WIDGET_COUNT]; GetWidgetStates(ws);
                *ws[selectedWidget] = !(*ws[selectedWidget]);
                AddLogEntry(std::string("[WIDGET] ") + WIDGET_NAMES[selectedWidget] +
                            (*ws[selectedWidget] ? " ON" : " OFF"), CYAN_HIGHLIGHT);
                SaveConfig();
            }
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_TAB)) {
                showWidgetMenu = false;
                AddLogEntry("[WIDGETS] Customization closed", DIM_GREEN);
            }
        }

        // ── Input: Main menu ──────────────────────────────────────────────
        else {
            if (IsKeyPressed(KEY_TAB)) {
                showMenu = !showMenu;
                AddLogEntry(showMenu ? "[INPUT] Menu opened" : "[INPUT] Menu closed", DIM_GREEN);
            }
            if (showMenu) {
                if (IsKeyPressed(KEY_UP))     { selectedOption = (selectedOption - 1 + MENU_COUNT) % MENU_COUNT; menuBlinkTimer = 0; }
                if (IsKeyPressed(KEY_DOWN))   { selectedOption = (selectedOption + 1) % MENU_COUNT; menuBlinkTimer = 0; }
                if (IsKeyPressed(KEY_ENTER))  HandleMenuSelection();
                if (IsKeyPressed(KEY_ESCAPE)) showMenu = false;
            }
        }

        // ── Update ────────────────────────────────────────────────────────
        UpdateStats(dt);
        if (GetRandomValue(0, 60) == 0) GenerateRandomLog();

        // ── Shader uniforms ───────────────────────────────────────────────
        SetShaderValue(crtShader, timeLoc, &timeAccumulator, SHADER_UNIFORM_FLOAT);
        float tint[3] = {
            PALETTES[currentTheme].primary.r / 255.f,
            PALETTES[currentTheme].primary.g / 255.f,
            PALETTES[currentTheme].primary.b / 255.f
        };
        SetShaderValue(crtShader, tintLoc, tint, SHADER_UNIFORM_VEC3);

        // ── Render ────────────────────────────────────────────────────────
        BeginTextureMode(target);
            DrawDashboard();
        EndTextureMode();

        BeginDrawing();
            ClearBackground(COLOR_BLACK);
            BeginShaderMode(crtShader);
                Rectangle src = { 0, 0, (float)target.texture.width, (float)-target.texture.height };
                DrawTextureRec(target.texture, src, {0, 0}, WHITE);
            EndShaderMode();

            // Overlays drawn AFTER shader - no CRT tint applied to them
            DrawColorMenu();
            DrawWidgetMenu();
            DrawOnboarding();
            DrawTerminal();

            DrawFPS(10, 10);
        EndDrawing();
    }

    // ── Cleanup ───────────────────────────────────────────────────────────────
    CleanupSystemMonitoring();
    UnloadShader(crtShader);
    UnloadRenderTexture(target);
    if (hasCustomFont) UnloadFont(retroFont);
    CloseWindow();
    return 0;
}
