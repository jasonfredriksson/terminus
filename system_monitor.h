#ifndef SYSTEM_MONITOR_H
#define SYSTEM_MONITOR_H

// Windows system monitoring functions
void InitializeSystemMonitoring();
void CleanupSystemMonitoring();
float GetRealCPUUsage();
float GetRealRAMUsage();
float GetRealDiskUsage();
unsigned long long GetTotalRAM_MB();
unsigned long long GetUsedRAM_MB();
unsigned long long GetTotalDisk_GB();
unsigned long long GetUsedDisk_GB();
int GetProcessCount();
unsigned long long GetSystemUptimeSeconds();
void GetHostName(char* buffer, int bufferSize);

#endif // SYSTEM_MONITOR_H
