#pragma once
#include <string>
#include <vector>

extern bool showTerminal;
extern std::string terminalInput;
extern std::vector<std::string> terminalOutput;
extern std::string terminalCwd;

void RunTerminalCommand(const std::string& cmd);
void DrawTerminal();
