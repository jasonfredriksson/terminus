#include "terminal.h"
#include "theme.h"
#include "dashboard.h"
#include "raylib.h"
#include <cstdio>
#include <cmath>

bool showTerminal = false;
std::string terminalInput;
std::vector<std::string> terminalOutput;
std::string terminalCwd = "C:\\";
static float termCursorBlink = 0.f;

void RunTerminalCommand(const std::string& cmd) {
    terminalOutput.push_back("> " + terminalCwd + " $ " + cmd);
    std::string full = "cd /d \"" + terminalCwd + "\" && " + cmd + " 2>&1";
    FILE* pipe = _popen(full.c_str(), "r");
    if (!pipe) { terminalOutput.push_back("[ERROR] Failed to run command"); return; }
    char buf[256];
    while (fgets(buf, sizeof(buf), pipe)) {
        std::string line(buf);
        while (!line.empty() && (line.back() == '\n' || line.back() == '\r')) line.pop_back();
        terminalOutput.push_back(line);
    }
    _pclose(pipe);
    if (terminalOutput.size() > 200)
        terminalOutput.erase(terminalOutput.begin(), terminalOutput.begin() + (terminalOutput.size() - 200));
}

void DrawTerminal() {
    if (!showTerminal) return;

    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, ColorAlpha(COLOR_BLACK, 0.88f));

    const int TX = 40, TY = 40, TW = WINDOW_WIDTH - 80, TH = WINDOW_HEIGHT - 100;
    const int INPUT_H = 34;
    const int OUT_H   = TH - INPUT_H - 30;

    DrawRectangle(TX, TY, TW, TH, {8, 8, 8, 255});
    DrawRectangleLines(TX, TY, TW, TH, GREEN_PHOSPHOR);

    // Title bar
    DrawRectangle(TX, TY, TW, 28, {20, 20, 20, 255});
    DrawText("TERMINAL", TX + 10, TY + 6, 16, GREEN_PHOSPHOR);
    DrawText("[ESC to close]", TX + 100, TY + 8, 13, DIM_GREEN);
    int cwdW = MeasureText(terminalCwd.c_str(), 14);
    DrawText(terminalCwd.c_str(), TX + TW - cwdW - 10, TY + 7, 14, DIM_GREEN);

    // Output area
    int lineH   = 16;
    int maxLines = OUT_H / lineH;
    int outY    = TY + 32;
    int start   = (int)terminalOutput.size() > maxLines
                  ? (int)terminalOutput.size() - maxLines : 0;
    for (int i = start; i < (int)terminalOutput.size(); i++) {
        Color c = GREEN_PHOSPHOR;
        if (!terminalOutput[i].empty() && terminalOutput[i][0] == '>')  c = CYAN_HIGHLIGHT;
        else if (terminalOutput[i].find("[ERROR]") != std::string::npos) c = YELLOW_ALERT;
        DrawText(terminalOutput[i].c_str(), TX + 8, outY, 14, c);
        outY += lineH;
    }

    // Input bar
    int inputY = TY + TH - INPUT_H;
    DrawRectangle(TX, inputY, TW, INPUT_H, {15, 15, 15, 255});
    DrawLine(TX, inputY, TX + TW, inputY, DIM_GREEN);
    std::string prompt = terminalCwd + " $ " + terminalInput;
    DrawText(prompt.c_str(), TX + 8, inputY + 8, 16, GREEN_PHOSPHOR);

    // Blinking cursor
    termCursorBlink += GetFrameTime() * 2.f;
    if (fmod(termCursorBlink, 1.f) < 0.5f) {
        int cx = TX + 8 + MeasureText(prompt.c_str(), 16);
        DrawRectangle(cx, inputY + 6, 10, 20, GREEN_PHOSPHOR);
    }
}
