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
            if(!fnOnFoundItem(mi)) break;
            pNewStartAddr = (char*)mi.BaseAddress + mi.RegionSize;
        }
        if (pNewStartAddr < pCurStartAddr) {
            break;
        }
        pCurStartAddr = pNewStartAddr;
    }
}

bool CopyProcessMemory(HANDLE hProcess, void* pTarget, void* pRemoteProcessSource, size_t nRemoteSourceSize)
{
    SIZE_T nRead = 0;
    return FALSE != ::ReadProcessMemory(hProcess, pRemoteProcessSource, pTarget, nRemoteSourceSize, &nRead) &&
        nRead == nRemoteSourceSize;
}

void* FindChildProcessKernelBase_v3(HANDLE hProc)
{
    HMODULE hModLocalKernelBase = GetModuleHandle(L"KernelBase.dll");

    char search_tpl[0x100] = {};
    memcpy(search_tpl, (void*)((ULONG_PTR)hModLocalKernelBase & ~(ULONG_PTR)3), sizeof(search_tpl));

    HMODULE hModRemoteProcessKernelBase = nullptr;
    if (hModLocalKernelBase)
    {
        EnumProcessBaseAddresses(hProc, [&](MEMORY_BASIC_INFORMATION& mi)->bool
        {
            HMODULE hMod = (HMODULE)mi.BaseAddress;
            char cur_bytes[sizeof(search_tpl)] = {};
            if (CopyProcessMemory(hProc, cur_bytes, hMod, sizeof(cur_bytes)))
            {
                if (0 == memcmp(search_tpl, cur_bytes, sizeof(search_tpl)))
                {
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
        printf("Parent Found Child KernelBase addr:\t0x%p\n", FindChildProcessKernelBase_v3(pi.hProcess));
        WaitForSingleObject(pi.hProcess, INFINITE);
    }
    return 0;
}