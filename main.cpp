#include <iostream>
#include "utils.hpp"

int main(int, char**){
    printf("noclipp 2.2 by qwix\n");
    DWORD process_id = 0;
    printf("waiting geometry dash\n");
    while (process_id == 0)
    {
        process_id = GetPIDByName("GeometryDash.exe");
        Sleep(1000);
    }
    printf("geometry dash found\n");
    HANDLE process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
    auto base = GetModuleBaseAddress(process_id, "GeometryDash.exe");
    uintptr_t address = base + 0x2E0AD9;
    uintptr_t address1 = base + 0x2E0ADE;
    printf("patching");
    WriteBytesToProcess(process_handle, (LPVOID)address, {0xE9, 0x9C, 0x07, 0x00, 0x00});
    WriteBytesToProcess(process_handle, (LPVOID)address1, {0x90});
    Sleep(3000);
    CloseHandle(process_handle);
    return 0;
}
