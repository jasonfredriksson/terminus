#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <psapi.h>

#pragma comment(lib, "pdh.lib")

#include "system_monitor.h"

// Windows system monitoring handles
static PDH_HQUERY cpuQuery;
static PDH_HCOUNTER cpuCounter;
static bool pdhInitialized = false;

void InitializeSystemMonitoring() {
    PDH_STATUS status = PdhOpenQuery(NULL, 0, &cpuQuery);
    if (status == ERROR_SUCCESS) {
        status = PdhAddEnglishCounter(cpuQuery, "\\Processor(_Total)\\% Processor Time", 0, &cpuCounter);
        if (status == ERROR_SUCCESS) {
            // Collect initial baseline data
            PdhCollectQueryData(cpuQuery);
            Sleep(100); // Wait for initial sample
            PdhCollectQueryData(cpuQuery);
            pdhInitialized = true;
        }
    }
}

void CleanupSystemMonitoring() {
    if (pdhInitialized) {
        PdhCloseQuery(cpuQuery);
        pdhInitialized = false;
    }
}

float GetRealCPUUsage() {
    if (!pdhInitialized) return 0.0f;
    
    PDH_FMT_COUNTERVALUE counterVal;
    PdhCollectQueryData(cpuQuery);
    PdhGetFormattedCounterValue(cpuCounter, PDH_FMT_DOUBLE, NULL, &counterVal);
    return static_cast<float>(counterVal.doubleValue);
}

float GetRealRAMUsage() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    return static_cast<float>(memInfo.dwMemoryLoad);
}

float GetRealDiskUsage() {
    ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
    if (GetDiskFreeSpaceExA("C:\\", &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes)) {
        double usedBytes = static_cast<double>(totalNumberOfBytes.QuadPart - totalNumberOfFreeBytes.QuadPart);
        double totalBytes = static_cast<double>(totalNumberOfBytes.QuadPart);
        return static_cast<float>((usedBytes / totalBytes) * 100.0);
    }
    return 0.0f;
}

unsigned long long GetTotalRAM_MB() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    return memInfo.ullTotalPhys / (1024 * 1024);
}

unsigned long long GetUsedRAM_MB() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    return (memInfo.ullTotalPhys - memInfo.ullAvailPhys) / (1024 * 1024);
}

unsigned long long GetTotalDisk_GB() {
    ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
    if (GetDiskFreeSpaceExA("C:\\", &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes)) {
        return totalNumberOfBytes.QuadPart / (1024 * 1024 * 1024);
    }
    return 0;
}

unsigned long long GetUsedDisk_GB() {
    ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
    if (GetDiskFreeSpaceExA("C:\\", &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes)) {
        return (totalNumberOfBytes.QuadPart - totalNumberOfFreeBytes.QuadPart) / (1024 * 1024 * 1024);
    }
    return 0;
}

int GetProcessCount() {
    DWORD processes[1024], bytesNeeded, processCount;
    if (EnumProcesses(processes, sizeof(processes), &bytesNeeded)) {
        processCount = bytesNeeded / sizeof(DWORD);
        return static_cast<int>(processCount);
    }
    return 0;
}

unsigned long long GetSystemUptimeSeconds() {
    return GetTickCount64() / 1000;
}

void GetHostName(char* buffer, int bufferSize) {
    DWORD size = bufferSize;
    if (!GetComputerNameA(buffer, &size)) {
        buffer[0] = '\0';
    }
}
