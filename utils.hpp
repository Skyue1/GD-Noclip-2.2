#include <Windows.h>
#include <tlhelp32.h>
#include <vector>

DWORD GetPIDByName(std::string processName) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (!Process32First(snapshot, &entry)) {
        CloseHandle(snapshot);
        return 0;
    }

    do {
        if (_stricmp(entry.szExeFile, processName.c_str()) == 0) {
            DWORD processId = entry.th32ProcessID;
            CloseHandle(snapshot);
            return processId;
        }
    } while (Process32Next(snapshot, &entry));

    CloseHandle(snapshot);
    return 0;
}

uintptr_t GetModuleBaseAddress(DWORD processId, const char* moduleName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    MODULEENTRY32 moduleEntry;
    moduleEntry.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnapshot, &moduleEntry)) {
        do {
            if (strcmp(moduleEntry.szModule, moduleName) == 0) {
                CloseHandle(hSnapshot);
                return reinterpret_cast<uintptr_t>(moduleEntry.modBaseAddr);
            }
        } while (Module32Next(hSnapshot, &moduleEntry));
    }

    CloseHandle(hSnapshot);
    return 0;
}

void WriteBytesToProcess(HANDLE processHandle, LPVOID address, std::vector<uint8_t> const bytes)
{
    DWORD oldProtect;
    VirtualProtectEx(processHandle, address, bytes.size(), PAGE_EXECUTE_READWRITE, &oldProtect);
    WriteProcessMemory(processHandle, address, bytes.data(), bytes.size(), NULL);
    VirtualProtectEx(processHandle, address, bytes.size(), oldProtect, &oldProtect);
}