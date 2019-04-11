#include <windows.h>
#include <stdio.h>
#include <memory>
#include <functional>
#include <Psapi.h>

void EnumProcessBaseAddresses(HANDLE hProc, std::function<bool(MEMORY_BASIC_INFORMATION& mi)> fnOnFoundItem)
{
    char* pNewStartAddr = 0;
    char* pCurStartAddr = 0;
    char* pEndAddr = (char*)((ULONG_PTR)-1 ^ ((ULONG_PTR)-1 >> 1));

    SYSTEM_INFO si = {};
    ::GetSystemInfo(&si);
    pCurStartAddr = (char*)si.lpMinimumApplicationAddress;
    pEndAddr = (char*)si.lpMaximumApplicationAddress;

    while (pCurStartAddr < pEndAddr)
    {
        MEMORY_BASIC_INFORMATION mi = {};
        SIZE_T nRetBytes = VirtualQueryEx(hProc, pCurStartAddr, &mi, sizeof(mi));
        if (!nRetBytes) {
            pNewStartAddr = pCurStartAddr + 4096;
        }
        else {
            if (!fnOnFoundItem(mi)) break;
            pNewStartAddr = (char*)mi.BaseAddress + mi.RegionSize;
        }
        if (pNewStartAddr < pCurStartAddr) {
            break;
        }
        pCurStartAddr = pNewStartAddr;
    }
}

void* FindChildProcessKernelBase_v2(HANDLE hProc)
{
    wchar_t wsKernelBaseModulePath[MAX_PATH + 1] = {};
    HMODULE hModLocalKernelBase = GetModuleHandle(L"KernelBase.dll");
    ::GetModuleFileNameW(hModLocalKernelBase, wsKernelBaseModulePath, _countof(wsKernelBaseModulePath) - 1);

    HMODULE hModRemoteProcessKernelBase = nullptr;
    wchar_t wsModulePath[MAX_PATH + 1];
    if (hModLocalKernelBase && wsKernelBaseModulePath[0])
    {
        EnumProcessBaseAddresses(hProc, [&](MEMORY_BASIC_INFORMATION& mi)->bool
        {
            HMODULE hMod = (HMODULE)mi.BaseAddress;
            DWORD dwSize = GetModuleFileNameExW(hProc, hMod, wsModulePath, _countof(wsModulePath) - 1);
            if (dwSize) {
                wsModulePath[dwSize] = 0;
                if (0 == wcscmp(wsKernelBaseModulePath, wsModulePath)) {
                    hModRemoteProcessKernelBase = hMod;
                    return false;
                }
            }
            return true;
        });
    }
    return hModRemoteProcessKernelBase;
}

int main()
{
    printf("Parent KernelBase base addr:\t0x%p\n", ::GetModuleHandle(L"KernelBase.dll"));

    STARTUPINFOW si = {};
    PROCESS_INFORMATION pi = {};
    wchar_t wsProcessToRun[] = L"DisplayKernelBase.exe";
    if (::CreateProcessW(nullptr, wsProcessToRun, nullptr, nullptr,
        FALSE, 0, nullptr, nullptr, &si, &pi))
    {
        // Waiting child process loads all required DLLs
        WaitForSingleObject(pi.hProcess, 1000);
        printf("Parent Found Child KernelBase addr:\t0x%p\n", FindChildProcessKernelBase_v2(pi.hProcess));
        WaitForSingleObject(pi.hProcess, INFINITE);
    }
    return 0;
}