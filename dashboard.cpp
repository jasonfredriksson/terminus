#include "dashboard.h"
#include "theme.h"
#include "system_monitor.h"
#include "raylib.h"
#include <string>
#include <vector>
#include <ctime>
#include <cmath>
#include <cstdio>

// ── Global definitions ────────────────────────────────────────────────────────
SystemStats      stats;
std::vector<LogEntry> logEntries;
float            timeAccumulator = 0.0f;
Font             retroFont;
bool             hasCustomFont   = false;
int              currentMenu     = MENU_DASHBOARD;
int              selectedOption  = 0;
bool             showMenu        = false;
float            menuBlinkTimer  = 0.0f;
DashboardWidgets widgets;
bool             showWidgetMenu  = false;
int              selectedWidget  = 0;
bool             isFirstRun      = true;

// ── Widget state helper ───────────────────────────────────────────────────────
void GetWidgetStates(bool* states[WIDGET_COUNT]) {
    states[WIDGET_CPU]          = &widgets.showCPU;
    states[WIDGET_RAM]          = &widgets.showRAM;
    states[WIDGET_DISK]         = &widgets.showDisk;
    states[WIDGET_NETWORK]      = &widgets.showNetwork;
    states[WIDGET_ANOMALY]      = &widgets.showAnomaly;
    states[WIDGET_LOG]          = &widgets.showSystemLog;
    states[WIDGET_TIME]         = &widgets.showSystemTime;
    states[WIDGET_PROCESSES]    = &widgets.showProcesses;
    states[WIDGET_UPTIME]       = &widgets.showUptime;
    states[WIDGET_COMPUTERNAME] = &widgets.showComputerName;
}

// ── Log ───────────────────────────────────────────────────────────────────────
void AddLogEntry(const std::string& message, Color entryColor) {
    logEntries.push_back({ message, GetTime(), entryColor });
    if (logEntries.size() > 40)
        logEntries.erase(logEntries.begin());
}

// ── Stats ─────────────────────────────────────────────────────────────────────
void InitializeStats() {
    srand(static_cast<unsigned>(time(nullptr)));
    stats = SystemStats{};
    stats.cpu      = stats.targetCpu      = 45.f;
    stats.ram      = stats.targetRam      = 60.f;
    stats.disk     = stats.targetDisk     = 70.f;
    stats.netDown  = stats.targetNetDown  = 15.f;
    stats.netUp    = stats.targetNetUp    = 3.f;
    stats.useRealData = false;
    stats.processCount  = 120;
    stats.uptimeSeconds = 3600;
    GetHostName(stats.computerName, sizeof(stats.computerName));
}

void UpdateStats(float deltaTime) {
    if (stats.useRealData) {
        stats.targetCpu     = GetRealCPUUsage();
        stats.targetRam     = GetRealRAMUsage();
        stats.targetDisk    = GetRealDiskUsage();
        stats.targetNetDown = static_cast<float>(GetRandomValue(5, 50));
        stats.targetNetUp   = static_cast<float>(GetRandomValue(1, 20));
        static float infoTimer = 0.f;
        infoTimer += deltaTime;
        if (infoTimer >= 2.f) {
            stats.processCount  = GetProcessCount();
            stats.uptimeSeconds = GetSystemUptimeSeconds();
            infoTimer = 0.f;
        }
    } else {
        if (GetRandomValue(0, 100) < 2) {
            stats.targetCpu     = static_cast<float>(GetRandomValue(20, 80));
            stats.targetRam     = static_cast<float>(GetRandomValue(40, 90));
            stats.targetDisk    = static_cast<float>(GetRandomValue(60, 95));
            stats.targetNetDown = static_cast<float>(GetRandomValue(5, 50));
            stats.targetNetUp   = static_cast<float>(GetRandomValue(1, 20));
        }
        stats.processCount  = 120 + GetRandomValue(-5, 5);
        stats.uptimeSeconds = static_cast<unsigned long long>(GetTime()) + 3600;
    }
    const float S = stats.useRealData ? 5.f : 2.f;
    stats.cpu     += (stats.targetCpu     - stats.cpu)     * deltaTime * S;
    stats.ram     += (stats.targetRam     - stats.ram)     * deltaTime * S;
    stats.disk    += (stats.targetDisk    - stats.disk)    * deltaTime * S;
    stats.netDown += (stats.targetNetDown - stats.netDown) * deltaTime * S;
    stats.netUp   += (stats.targetNetUp   - stats.netUp)   * deltaTime * S;
}

void GenerateRandomLog() {
    static const char* msgs[] = {
        "[INFO] Boot sequence complete",
        "[NET] Packet received from 192.168.1.1",
        "[CPU] Core 0 temperature normal",
        "[MEM] Cache flushed successfully",
        "[DISK] Defragmentation complete",
        "[SECURITY] Firewall active",
        "[NET] Connection established",
        "[SYSTEM] Heartbeat signal received",
        "[PROC] Task scheduler running",
        "[IO] Buffer cleared",
        "[NET] Data transmission complete",
        "[CPU] Frequency scaling active",
        "[TEMP] Thermal sensors calibrated",
        "[POWER] UPS status: ONLINE",
        "[LOG] Rotation complete"
    };
    AddLogEntry(msgs[GetRandomValue(0, 14)], GREEN_PHOSPHOR);
}

// ── Drawing helpers ───────────────────────────────────────────────────────────
void DrawProgressBar(int x, int y, int w, int h, float value, Color barColor) {
    DrawRectangle(x, y, w, h, COLOR_BLACK);
    DrawRectangleLines(x, y, w, h, DIM_GREEN);
    int fill = static_cast<int>((w - 4) * (value / 100.f));
    DrawRectangle(x + 2, y + 2, fill, h - 4, barColor);
    std::string txt = std::to_string(static_cast<int>(value)) + "%";
    int tw = MeasureText(txt.c_str(), 16);
    DrawText(txt.c_str(), x + w / 2 - tw / 2, y + h / 2 - 8, 16, COLOR_BLACK);
}

void DrawPanel(int x, int y, int w, int h, const char* title) {
    DrawRectangle(x, y, w, h, ColorAlpha(COLOR_BLACK, 0.6f));
    DrawRectangleLines(x, y, w, h, DIM_GREEN);
    if (title && title[0]) {
        int tw = MeasureText(title, 14);
        DrawRectangle(x + 10, y - 8, tw + 8, 16, COLOR_BLACK);
        DrawText(title, x + 14, y - 7, 14, GREEN_PHOSPHOR);
    }
}

// ── Main menu overlay (inside shader) ────────────────────────────────────────
void DrawMenu() {
    if (!showMenu) return;

    int mX = WINDOW_WIDTH - 440, mY = 80, mW = 400, mH = MENU_COUNT * 38 + 130;
    DrawRectangle(mX, mY, mW, mH, ColorAlpha(COLOR_BLACK, 0.9f));
    DrawRectangleLines(mX, mY, mW, mH, GREEN_PHOSPHOR);
    DrawText("CONTROL PANEL", mX + 20, mY + 16, 22, GREEN_PHOSPHOR);
    DrawLine(mX + 10, mY + 46, mX + mW - 10, mY + 46, DIM_GREEN);
    DrawText("UP/DOWN  ENTER select  TAB close", mX + 20, mY + mH - 28, 13, DIM_GREEN);

    int oY = mY + 56;
    for (int i = 0; i < MENU_COUNT; i++) {
        Color c = (i == selectedOption) ? CYAN_HIGHLIGHT : GREEN_PHOSPHOR;
        if (i == selectedOption && fmod(menuBlinkTimer, 1.f) < 0.5f)
            DrawText(">", mX + 14, oY + 2, 18, CYAN_HIGHLIGHT);
        DrawText(MENU_OPTIONS[i], mX + 36, oY + 2, 18, c);
        if (i == currentMenu)
            DrawText("[ACTIVE]", mX + mW - 90, oY + 4, 13, AMBER_PHOSPHOR);
        oY += 38;
    }
}

// ── Dashboard ─────────────────────────────────────────────────────────────────
void DrawDashboard() {
    ClearBackground(COLOR_BLACK);

    const int PAD  = 10;
    const int HDR  = 55;
    const int BOT  = 30;
    const int COLW = 620;
    const int LX   = PAD;
    const int RX   = COLW + PAD * 3;
    const int RW   = WINDOW_WIDTH - RX - PAD;
    const int CT   = HDR + PAD * 2;
    const int CB   = WINDOW_HEIGHT - BOT - PAD * 2;
    const int CH   = CB - CT;

    // Header
    DrawRectangle(0, 0, WINDOW_WIDTH, HDR, ColorAlpha(COLOR_BLACK, 0.85f));
    DrawLine(0, HDR, WINDOW_WIDTH, HDR, DIM_GREEN);
    const char* title = stats.useRealData ? "MAINFRAME ONLINE [LIVE]" : "MAINFRAME ONLINE [SIM]";
    int tw = MeasureText(title, 36);
    DrawText(title, WINDOW_WIDTH / 2 - tw / 2, 10, 36, GREEN_PHOSPHOR);

    if (widgets.showComputerName && stats.computerName[0]) {
        std::string h = "HOST: " + std::string(stats.computerName);
        DrawText(h.c_str(), LX + PAD, 20, 16, DIM_GREEN);
    }
    if (widgets.showSystemTime) {
        time_t now = time(nullptr);
        struct tm* t = localtime(&now);
        char buf[32]; strftime(buf, sizeof(buf), "%H:%M:%S", t);
        std::string ts = std::string("TIME: ") + buf;
        int tsw = MeasureText(ts.c_str(), 16);
        DrawText(ts.c_str(), WINDOW_WIDTH - tsw - PAD * 2, 20, 16, DIM_GREEN);
    }

    // Left panel - metrics
    DrawPanel(LX, CT, COLW, CH, "SYSTEM METRICS");
    int rowH = 38, barX = LX + 110, barW = 280, barH = 22;
    int detX = barX + barW + 10, rowY = CT + 20;

    if (widgets.showCPU) {
        DrawText("CPU",  LX + 14, rowY + 2, 18, GREEN_PHOSPHOR);
        DrawProgressBar(barX, rowY, barW, barH, stats.cpu, GREEN_PHOSPHOR);
        rowY += rowH;
    }
    if (widgets.showRAM) {
        DrawText("RAM",  LX + 14, rowY + 2, 18, GREEN_PHOSPHOR);
        DrawProgressBar(barX, rowY, barW, barH, stats.ram, GREEN_PHOSPHOR);
        if (stats.useRealData) {
            std::string d = std::to_string(GetUsedRAM_MB()) + "/" + std::to_string(GetTotalRAM_MB()) + "MB";
            DrawText(d.c_str(), detX, rowY + 4, 14, DIM_GREEN);
        }
        rowY += rowH;
    }
    if (widgets.showDisk) {
        DrawText("DISK", LX + 14, rowY + 2, 18, GREEN_PHOSPHOR);
        DrawProgressBar(barX, rowY, barW, barH, stats.disk, AMBER_PHOSPHOR);
        if (stats.useRealData) {
            std::string d = std::to_string(GetUsedDisk_GB()) + "/" + std::to_string(GetTotalDisk_GB()) + "GB";
            DrawText(d.c_str(), detX, rowY + 4, 14, DIM_GREEN);
        }
        rowY += rowH;
    }

    bool hasText = widgets.showNetwork || widgets.showProcesses || widgets.showUptime || widgets.showAnomaly;
    if (hasText && rowY > CT + 20) {
        DrawLine(LX + 10, rowY, LX + COLW - 10, rowY, DIM_GREEN);
        rowY += 10;
    }
    if (widgets.showNetwork) {
        char buf[80];
        snprintf(buf, sizeof(buf), "NET  %.0f Mb/s DOWN   %.0f Mb/s UP", stats.netDown, stats.netUp);
        DrawText(buf, LX + 14, rowY, 18, GREEN_PHOSPHOR);
        rowY += rowH;
    }
    if (widgets.showProcesses) {
        std::string s = "PROC  " + std::to_string(stats.processCount) + " running";
        DrawText(s.c_str(), LX + 14, rowY, 18, GREEN_PHOSPHOR);
        rowY += rowH;
    }
    if (widgets.showUptime) {
        unsigned long long up = stats.uptimeSeconds;
        char buf[64];
        snprintf(buf, sizeof(buf), "UP    %llud %02lluh %02llum", up/86400, (up%86400)/3600, (up%3600)/60);
        DrawText(buf, LX + 14, rowY, 18, GREEN_PHOSPHOR);
        rowY += rowH;
    }
    if (widgets.showAnomaly) {
        static float flash = 0.f;
        flash += GetFrameTime() * 3.f;
        Color ac = YELLOW_ALERT;
        ac.a = static_cast<unsigned char>(160 + 95 * sinf(flash));
        DrawText("ANOMALY  NONE DETECTED", LX + 14, rowY, 18, ac);
    }

    // Right panel - log
    if (widgets.showSystemLog) {
        DrawPanel(RX, CT, RW, CH, "SYSTEM LOG");
        int logY = CT + 18, logMax = CT + CH - 10;
        for (int i = (int)logEntries.size() - 1; i >= 0 && logY + 18 < logMax; i--) {
            float age = GetTime() - logEntries[i].time;
            if (age < 60.f) {
                Color c = logEntries[i].color;
                c.a = static_cast<unsigned char>(255 * (1.f - age / 60.f));
                DrawText(logEntries[i].message.c_str(), RX + 10, logY, 14, c);
                logY += 18;
            }
        }
    }

    // Bottom bar
    DrawLine(0, WINDOW_HEIGHT - BOT, WINDOW_WIDTH, WINDOW_HEIGHT - BOT, DIM_GREEN);
    DrawText("TAB: Menu", PAD, WINDOW_HEIGHT - BOT + 8, 14, DIM_GREEN);
    const char* mode = stats.useRealData ? "MODE: LIVE" : "MODE: SIM";
    DrawText(mode, WINDOW_WIDTH / 2 - 40, WINDOW_HEIGHT - BOT + 8, 14, DIM_GREEN);
    int tnw = MeasureText(THEME_NAMES[currentTheme], 14);
    DrawText(THEME_NAMES[currentTheme], WINDOW_WIDTH - tnw - PAD, WINDOW_HEIGHT - BOT + 8, 14, DIM_GREEN);

    DrawMenu();
}
