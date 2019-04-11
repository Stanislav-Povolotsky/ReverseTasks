#include <windows.h>
#include <stdio.h>
#include <winternl.h>
#pragma comment(lib, "ntdll.lib")

/* Windows structures */
typedef struct _LDR_DATA_TABLE_ENTRY_M {
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderModuleList;
    PVOID DllBase;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    ULONG Flags;
    USHORT LoadCount;
    USHORT TlsIndex;
    union {
        LIST_ENTRY HashLinks;
        struct
        {
            PVOID SectionPointer;
            ULONG CheckSum;
        };
    };
    union {
        ULONG TimeDateStamp;
        PVOID LoadedImports;
    };
    PVOID EntryPointActivationContext;
    PVOID PatchInformation;
} LDR_DATA_TABLE_ENTRY_M, *PLDR_DATA_TABLE_ENTRY_M;


// MODULE_ENTRY contains basic information about a module
typedef struct _MODULE_ENTRY {
    UNICODE_STRING BaseName; // BaseName of the module
    UNICODE_STRING FullName; // FullName of the module
    ULONG SizeOfImage; // Size in bytes of the module
    PVOID BaseAddress; // Base address of the module
    PVOID EntryPoint; // Entrypoint of the module
} MODULE_ENTRY, *PMODULE_ENTRY;

// MODULE_INFORMATION_TABLE contains basic information about all the modules of a given process
typedef struct _MODULE_INFORMATION_TABLE {
    ULONG Pid; // PID of the process
    ULONG ModuleCount; // Modules count for the above pointer
    PMODULE_ENTRY Modules; // Pointer to 0...* modules
} MODULE_INFORMATION_TABLE, *PMODULE_INFORMATION_TABLE;

typedef NTSTATUS (NTAPI *pfnNtQueryInformationProcess)(
    IN  HANDLE ProcessHandle,
    IN  PROCESSINFOCLASS ProcessInformationClass,
    OUT PVOID ProcessInformation,
    IN  ULONG ProcessInformationLength,
    OUT PULONG ReturnLength    OPTIONAL
);

PVOID
QueryProcessInformation (
    IN HANDLE Process,
    IN PROCESSINFOCLASS ProcessInformationClass,
    IN DWORD ProcessInformationLength
) {
    PPROCESS_BASIC_INFORMATION pProcessInformation = NULL;
    ULONG ReturnLength = 0;
    NTSTATUS Status;

    // Allocate the memory for the requested structure
    if ((pProcessInformation = (PPROCESS_BASIC_INFORMATION)malloc(ProcessInformationLength)) == NULL) {
        printf ("ExAllocatePoolWithTag failed.\n");
        return NULL;
    }

    // Fill the requested structure
    if (!NT_SUCCESS (Status = NtQueryInformationProcess (Process, ProcessInformationClass, pProcessInformation, ProcessInformationLength, &ReturnLength))) {
        printf ("NtQueryInformationProcess should return NT_SUCCESS (Status = %#x).\n", Status);
        free (pProcessInformation);
        return NULL;
    }

    // Check the requested structure size with the one returned by NtQueryInformationProcess
    if (ReturnLength != ProcessInformationLength) {
        printf ("Warning : NtQueryInformationProcess ReturnLength is different than ProcessInformationLength\n");
        return NULL;
    }

    return pProcessInformation;
}

PPEB
GetPebProcess (
    HANDLE Process
) {
    PPROCESS_BASIC_INFORMATION pProcessInformation = NULL;
    DWORD ProcessInformationLength = sizeof (PROCESS_BASIC_INFORMATION);
    //HANDLE Process = GetCurrentProcess();
    PPEB pPeb = NULL;

    // ProcessBasicInformation returns information about the PebBaseAddress
    if ((pProcessInformation = (PPROCESS_BASIC_INFORMATION)QueryProcessInformation (Process, ProcessBasicInformation, ProcessInformationLength)) == NULL) {
        printf ("Handle=%p : QueryProcessInformation failed.\n", Process);
        return NULL;
    }

    // Check the correctness of the value returned
    if (pProcessInformation->PebBaseAddress == NULL) {
        printf ("Handle=%p : PEB address cannot be found.\n", Process);
        free (pProcessInformation);
        return NULL;
    }

    pPeb = pProcessInformation->PebBaseAddress;

    // Cleaning
    free (pProcessInformation);

    return pPeb;
}

bool CopyProcessMemory(HANDLE hProcess, void* pTarget, void* pRemoteProcessSource, size_t nRemoteSourceSize)
{
    SIZE_T nRead = 0;
    return FALSE != ::ReadProcessMemory(hProcess, pRemoteProcessSource, pTarget, nRemoteSourceSize, &nRead) && 
        nRead == nRemoteSourceSize;
}

template<class T>
T ReadProcessData(HANDLE hProcess, T& data)
{
    T val = {};
    CopyProcessMemory(hProcess, &val, &data, sizeof(val));
    return val;
}

PMODULE_INFORMATION_TABLE
CreateModuleInformation (
    IN HANDLE hProc,
    IN PPEB pPeb
) {
    ULONG Count = 0;
    ULONG CurCount = 0;
    PLIST_ENTRY pEntry = NULL;
    PLIST_ENTRY pHeadEntry = NULL;
    PPEB_LDR_DATA pLdrData = NULL;
    PMODULE_ENTRY CurModule = NULL;
    PLDR_DATA_TABLE_ENTRY_M pLdrEntry = NULL;
    PMODULE_INFORMATION_TABLE pModuleInformationTable = NULL;

    pLdrData = ReadProcessData(hProc, pPeb->Ldr);
    pHeadEntry = &pLdrData->InMemoryOrderModuleList;

    // Count user modules : iterate through the entire list
    pEntry = ReadProcessData(hProc, pHeadEntry->Flink);
    while (pEntry != pHeadEntry) {
        Count++;
        pEntry = ReadProcessData(hProc, pEntry->Flink);
    }

    SIZE_T nSizeToAllocate =
        sizeof(MODULE_INFORMATION_TABLE) +
        (Count * sizeof(MODULE_ENTRY)) +
        (Count * 2 * (MAX_PATH * sizeof(WCHAR)));

    // Allocate a MODULE_INFORMATION_TABLE
    if ((pModuleInformationTable = (PMODULE_INFORMATION_TABLE)malloc(nSizeToAllocate)) == NULL) {
        printf ("Cannot allocate data for modules.\n");
        return NULL;
    }

    pModuleInformationTable->Modules = (PMODULE_ENTRY)(pModuleInformationTable + 1);
    wchar_t* pStrData = (wchar_t*)&pModuleInformationTable->Modules[Count];

    // Fill the basic information of MODULE_INFORMATION_TABLE
    pModuleInformationTable->ModuleCount = Count;

    // Fill all the modules information in the table
    pEntry = ReadProcessData(hProc, pHeadEntry->Flink);
    while (pEntry != pHeadEntry && CurCount < Count)
    {
        // Retrieve the current MODULE_ENTRY
        CurModule = &pModuleInformationTable->Modules[CurCount++];

        // Retrieve the current LDR_DATA_TABLE_ENTRY
        pLdrEntry = CONTAINING_RECORD (pEntry, LDR_DATA_TABLE_ENTRY_M, InMemoryOrderLinks);

        // Fill the MODULE_ENTRY with the LDR_DATA_TABLE_ENTRY information
        CurModule->BaseName = ReadProcessData(hProc, pLdrEntry->BaseDllName);
        CurModule->FullName = ReadProcessData(hProc, pLdrEntry->FullDllName);
        CurModule->SizeOfImage = ReadProcessData(hProc, pLdrEntry->SizeOfImage);
        CurModule->BaseAddress = ReadProcessData(hProc, pLdrEntry->DllBase);
        CurModule->EntryPoint = ReadProcessData(hProc, pLdrEntry->EntryPoint);

        CopyProcessMemory(hProc, pStrData, CurModule->BaseName.Buffer, CurModule->BaseName.Length);
        CurModule->BaseName.Buffer = pStrData;
        pStrData += CurModule->BaseName.Length / sizeof(wchar_t) + 1;

        CopyProcessMemory(hProc, pStrData, CurModule->FullName.Buffer, CurModule->FullName.Length);
        CurModule->FullName.Buffer = pStrData;
        pStrData += CurModule->FullName.Length / sizeof(wchar_t) + 1;

        // Iterate to the next entry
        pEntry = ReadProcessData(hProc, pEntry->Flink);
    }

    return pModuleInformationTable;
}

PMODULE_INFORMATION_TABLE
QueryModuleInformationProcess(HANDLE hProc) {
    PPEB pPeb = NULL;
    PMODULE_INFORMATION_TABLE pModuleInformationTable = NULL;

    // Read the PEB from the current process
    if ((pPeb = GetPebProcess(hProc)) == NULL) {
        printf ("GetPebCurrentProcess failed.\n");
        return NULL;
    }

    // Convert the PEB into a MODULE_INFORMATION_TABLE
    if ((pModuleInformationTable = CreateModuleInformation (hProc, pPeb)) == NULL) {
        printf ("CreateModuleInformation failed.");
        return NULL;
    }

    return pModuleInformationTable;
}

void* FindChildProcessKernelBase_v1(HANDLE hProc)
{
    // Retrieve modules from PEB
    MODULE_INFORMATION_TABLE * moduleTable = QueryModuleInformationProcess(hProc);

    if (!moduleTable) {
        printf("Module table not found.\n");
        return nullptr;
    }

    // Iterate through modules table
    size_t moduleIndex;
    for (moduleIndex = 0; moduleIndex < moduleTable->ModuleCount; moduleIndex++)
    {
        MODULE_ENTRY *moduleEntry = &moduleTable->Modules[moduleIndex];
        PVOID baseAddress = moduleEntry->BaseAddress;
        DWORD sizeOfModule = (DWORD)moduleEntry->SizeOfImage;

        printf("%-15S : 0x%016p -> 0x%016p (%S)\n", moduleEntry->BaseName.Buffer, baseAddress, 
            (char*)baseAddress + sizeOfModule, moduleEntry->FullName.Buffer);
        if (0 == _wcsicmp(moduleEntry->BaseName.Buffer, L"kernelbase.dll")) {
            return moduleEntry->BaseAddress;
        }
    }

    return nullptr;
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
        printf("Parent Found Child KernelBase addr:\t0x%p\n", FindChildProcessKernelBase_v1(pi.hProcess));
        WaitForSingleObject(pi.hProcess, INFINITE);
    }
    return 0;
}