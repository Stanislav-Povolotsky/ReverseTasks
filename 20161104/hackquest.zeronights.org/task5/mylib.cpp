// mylib.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "mylib.h"
#include <sstream>
#include "base64.h"

/*
.idata:0071F5D0                 extrn __imp_LoadLibraryW:dword ; DATA XREF: LoadLibraryWr
.idata:0071F51C                 extrn __imp_GetProcAddress:dword

.text:006EB484 LoadLibraryW    proc near               ; CODE XREF: sub_4825FC+47p
.text:006EB36A GetProcAddress  proc near               ; CODE XREF: sub_402274+25Cp
// Код инжектора:

6A6C         push   06C           ; 'l\0\0\0'
54           push   esp           ; L"l"
B884b46E00   mov    eax, 6EB484   ; LoadLibraryW
FFD0         call   eax           ; LoadLibraryW(L"l")
54           push   esp           ; L"l"
50           push   eax           ; HINSTANCE for l.dll
B86AB36E00   mov    eax, 6EB36A   ; GetProcAddress
FFD0         call   eax           ; GetProcAddress(hMod_l_dll, L"l")
83C404       add    esp,004       ; Clean stack
FFD0         call   eax           ; call l.dll!l()

eb eip 6A 6C 54 B8 84 B4 6E 00 FF D0 54 50 B8 6A B3 6E 00 FF D0 83 C4 04 FF D0

*/

bool check_license(const char *sEmail, const char *sKey)
{
    typedef bool (*pfn_check_license_t)(const char **sEmail, const char **sKey);
    union
    {
        DWORD_PTR pfnAddr;
        pfn_check_license_t pfn;
    } pfn = { 0x405EB0 };
    return (*pfn.pfn)(&sEmail, &sKey);
}

// Main function
extern "C" void __stdcall l_test1()
{
    MessageBoxW(0, L"Before", L"OK", MB_OK);

    bool bResult = check_license("stas.zn@povolotsky.info", "MDEyMzQ1Njc4OTAxMjM0NTY3ODkwMTIzNA==");

    if (bResult)
    {
        MessageBoxW(0, L"Correct!", L"Correct!", MB_OK);
    }

    MessageBoxW(0, L"Done", L"OK", MB_OK);
}

#pragma pack(push, 1)
struct SDataWrap
{
    int field_0;
    int field_4;
    int pData;
};
#pragma pack(pop)

void SetCallHook(void** pCallInstructionAddr, void* pNew)
{
    DWORD* dwAddr = (DWORD*)((char*)pCallInstructionAddr + 1);
    DWORD dwRelativeOffs = (DWORD_PTR)((char*)pNew - ((char*)(pCallInstructionAddr)+(1 + sizeof(DWORD))));
    //*dwAddr = dwRelativeOffs;
    DWORD_PTR dwWritten = 0;
    WriteProcessMemory(GetCurrentProcess(), dwAddr, &dwRelativeOffs, sizeof(dwRelativeOffs), &dwWritten);
}

///

bool __cdecl SDataWrap_AreEqualDWORDVectors(SDataWrap itBegin1, int a4, SDataWrap itEnd1, int a8, SDataWrap itBegin2)
{
    typedef decltype(&SDataWrap_AreEqualDWORDVectors) Pfn_t;
    union
    {
        DWORD_PTR pfnAddr;
        Pfn_t pfn;
    } pfn = { 0x040A898 };
    return (pfn.pfn)(itBegin1, a4, itEnd1, a8, itBegin2);
}

/// ------------------------------------------------------

std::ofstream g_of("log.txt", std::ofstream::out);

/// ------------------------------------------------------

/*
bool __cdecl SDataWrap_AreEqualDWORDVectors_My(SDataWrap itBegin1, int a4, SDataWrap itEnd1, int a8, SDataWrap itBegin2)
{
    static std::vector<DWORD> s_vecPrev1;
    static std::vector<DWORD> s_vecPrev2;

    g_of << "Comparing " << std::endl << "\t";
    DWORD* p1 = (DWORD*)itBegin1.pData;
    DWORD* p2 = (DWORD*)itBegin2.pData;
    std::vector<DWORD> vecCur1(p1, p1 + 20);
    std::vector<DWORD> vecCur2(p2, p2 + 20);
    int nChanged1 = 0;
    if (s_vecPrev1.size() == vecCur1.size()) {
        for (int i = 0; i < 20; ++i) {
            if (vecCur1[i] != s_vecPrev1[i]) nChanged1++;
        }
    }
    int nChanged2 = 0;
    if (s_vecPrev2.size() == vecCur2.size()) {
        for (int i = 0; i < 20; ++i) {
            if (vecCur2[i] != s_vecPrev2[i]) nChanged2++;
        }
    }

    for (int i = 0; i < 20; ++i) {
        g_of << std::hex << std::setw(8) << p1[i] << " ";
    }
    if (nChanged1) g_of << " total changed " << nChanged1;
    g_of << std::endl << "\t";
    for (int i = 0; i < 20; ++i) {
        g_of << std::hex << std::setw(8) << p2[i] << " ";
    }
    if(nChanged2) g_of << " total changed " << nChanged2;
    g_of << std::endl;

    s_vecPrev1.swap(vecCur1);
    s_vecPrev2.swap(vecCur2);

    return SDataWrap_AreEqualDWORDVectors(itBegin1, a4, itEnd1, a8, itBegin2);
}

// Bruteforce
extern "C" void __stdcall l()
{
    MessageBoxW(0, L"Before", L"OK", MB_OK);

    const int nLen = 25;
    char buffState[nLen] = {};
    int nPos = -1;
    
    srand(GetCurrentProcessId() + GetCurrentThreadId() + GetTickCount());

    SetCallHook((void**)(ULONG_PTR)0x408DBD, (void*)&SDataWrap_AreEqualDWORDVectors_My);

    while (1)
    {
        nPos = (nPos + 1) % nLen;
        buffState[nPos] = (char)rand();

        std::string s = base64_encode((const unsigned char*)buffState, nLen);

        g_of << "Processing " << std::setw(2) << nPos << " ";
        for (int i = 0; i < nLen; ++i) {
            g_of << std::hex << std::setfill('0') << std::setw(2) << (unsigned int)(unsigned char)buffState[i];
        }
        g_of << std::endl;
        bool bResult = check_license("stas.zn@povolotsky.info", s.c_str());

        if (bResult)
        {
            MessageBoxA(0, "Correct!", s.c_str(), MB_OK);
        }
    }


    MessageBoxW(0, L"Done", L"OK", MB_OK);
}
*/

int g_curPosChanging = 0;
std::map<int, std::set<int>> g_mapPosChangers;
std::map<int, std::vector<DWORD>> g_mapPosChangersBits;
volatile bool g_bStatMode = true;
volatile bool g_bExTrace = true;
static std::vector<DWORD> s_vecPrev1;
static std::vector<DWORD> s_vecPrev2;

// Looking for changers
bool __cdecl SDataWrap_AreEqualDWORDVectors_My(SDataWrap itBegin1, int a4, SDataWrap itEnd1, int a8, SDataWrap itBegin2)
{
    //g_of << "Comparing " << std::endl << "\t";
    DWORD* p1 = (DWORD*)itBegin1.pData;
    DWORD* p2 = (DWORD*)itBegin2.pData;
    std::vector<DWORD> vecCur1(p1, p1 + 20);
    std::vector<DWORD> vecCur2(p2, p2 + 20);

    if (g_bStatMode)
    {
        int nChanged1 = 0;
        if (s_vecPrev1.size() == vecCur1.size()) {
            for (int i = 0; i < 20; ++i) {
                if (vecCur1[i] != s_vecPrev1[i]) nChanged1++;
            }
        }
        int nChanged2 = 0;
        std::vector<DWORD> vecChanges;
        if (s_vecPrev2.size() == vecCur2.size()) {
            for (int i = 0; i < 20; ++i) {
                if (vecCur2[i] != s_vecPrev2[i]) {
                    nChanged2++;
                    vecChanges.push_back(i);
                    
                    g_mapPosChangersBits[g_curPosChanging].resize(20);
                    g_mapPosChangersBits[g_curPosChanging][i] |= vecCur2[i] ^ s_vecPrev2[i];
                }
            }
        }

        if (g_bExTrace)
        {
            g_of << "\t";
            for (int i = 0; i < 20; ++i) {
                g_of << std::hex << std::setw(2) << p1[i] << " ";
            }
            if (nChanged1) g_of << " total changed " << nChanged1;
            g_of << std::endl << "\t";
            for (int i = 0; i < 20; ++i) {
                g_of << std::hex << std::setw(2) << p2[i] << " ";
            }
            if (nChanged2) g_of << " total changed " << nChanged2;
        }

        g_of << "Changing byte " << g_curPosChanging << " causes " << nChanged2 << " changes: ";
        g_mapPosChangers[g_curPosChanging];
        for (int i = 0; i < vecChanges.size(); ++i) {
            g_of << vecChanges[i] << " ";
            g_mapPosChangers[g_curPosChanging].insert(vecChanges[i]);
        }
        g_of << std::endl;
    }

    s_vecPrev1.swap(vecCur1);
    s_vecPrev2.swap(vecCur2);

    return SDataWrap_AreEqualDWORDVectors(itBegin1, a4, itEnd1, a8, itBegin2);
}

// Bruteforce
extern "C" void __stdcall l()
{
    //MessageBoxW(0, L"Before", L"OK", MB_OK);
    char chEmail[] = "stas.zn@povolotsky.info";

    const int nLen = 25;
    char buffState[nLen] = {};
    int nPos = -1;

    srand(GetCurrentProcessId() + GetCurrentThreadId() + GetTickCount());

    SetCallHook((void**)(ULONG_PTR)0x408DBD, (void*)&SDataWrap_AreEqualDWORDVectors_My);

    int nTry = 0;
    while (nTry++ < 10000)
    {
        nPos = (nPos + 1) % nLen;
        buffState[nPos] = (char)rand();
        g_curPosChanging = nPos;

        std::string s = base64_encode((const unsigned char*)buffState, nLen);

        g_of << "Processing " << std::setw(2) << nPos << " ";
        for (int i = 0; i < nLen; ++i) {
            g_of << std::hex << std::setfill('0') << std::setw(2) << (unsigned int)(unsigned char)buffState[i];
        }
        g_of << std::endl;
        bool bResult = check_license(chEmail, s.c_str());

        if (bResult)
        {
            MessageBoxA(0, "Correct!", s.c_str(), MB_OK);
        }
    }

    g_of << "MAP:" << std::endl;
    std::map<int, std::set<int>> mapBack; // Какая позиция в получившимся массиве зависит от каких исходных
    for (int i = 0; i < nLen; ++i)
    {
        g_of << i << ": ";
        for (auto& it : g_mapPosChangers[i])
        {
            g_of << dec << it << " ";
            mapBack[it].insert(i);
        }
        g_of << endl;
    }

    g_of << "Pos changer bits:" << std::endl;
    for (int i = 0; i < nLen; ++i) 
    {
        g_of << i << ": ";
        for (auto& it : g_mapPosChangersBits[i])
        {
            g_of << hex << std::hex << std::setw(2) << std::setfill('0') << it << " ";
        }
        g_of << endl;
    }

    std::map<int, std::pair<int,int>> mapBest; // позиция в получившимся массиве легче всего подвержена изменениям с помощью изменения исходного байта №N
    {
        g_of << "Back MAP:" << std::endl;
        for (auto& mit : mapBack)
        {
            int nBest = -1;
            int nBestSize = 1000;
            g_of << mit.first << ": ";
            for (auto& it : mit.second)
            {
                g_of << dec << it << " ";
                if (g_mapPosChangers[it].size() < nBestSize) {
                    nBestSize = g_mapPosChangers[it].size();
                    nBest = it;
                }
            }
            g_of << endl;
            mapBest[mit.first] = std::make_pair(nBest, g_mapPosChangers[nBest].size());
        }
    }

    {
        g_of << "Best MAP:" << std::endl;
        for (auto& mit : mapBest)
        {
            g_of << mit.first << ": " << mit.second.first << " (" << mit.second.second << ")" << endl;
        }
    }


/*
MAP:
0:  7
1:  0 1 2 3 4 5 7 8 9 10 11 12 13 14 15 16 17 18 19
2:  3
3:  6 9
4:  6
5:  12
6:  10 18
7:  0 1 2 3 4 5 7 8 9 10 11 12 13 14 15 16 17 18 19
8:  13 14 15
9:  8 16
10: 4 12
11: 1 10 18
12: 13
13: 0 1 2 3 4 5 7 8 9 10 11 12 13 14 15 16 17 18 19
14: 10
15: 0 4 11 12
16: 14
17: 11
18: 2 5
19: 19
20: 17
21: 0 1 2 3 4 5 7 8 9 10 11 12 13 14 15 16 17 18 19
22: 5
23: 16
24: 0 1 2 3 4 5 7 8 9 10 11 12 13 14 15 16 17 18 19
Back MAP:
0:  1 7 13 15 21 24 
1:  1 7 11 13 21 24 
2:  1 7 13 18 21 24 
3:  1 2 7 13 21 24 
4:  1 7 10 13 15 21 24 
5:  1 7 13 18 21 22 24 
6:  3 4 
7:  0 1 7 13 21 24 
8:  1 7 9 13 21 24 
9:  1 3 7 13 21 24 
10: 1 6 7 11 13 14 21 24 
11: 1 7 13 15 17 21 24 
12: 1 5 7 10 13 15 21 24 
13: 1 7 8 12 13 21 24 
14: 1 7 8 13 16 21 24 
15: 1 7 8 13 21 24 
16: 1 7 9 13 21 23 24 
17: 1 7 13 20 21 24 
18: 1 6 7 11 13 21 24 
19: 1 7 13 19 21 24 
Best MAP:
0:  15 (4)
1:  11 (3)
2:  18 (2)
3:  2 (1)
4:  10 (2)
5:  22 (1)
6:  4 (1)
7:  0 (1)
8:  9 (2)
9:  3 (2)
10: 14 (1)
11: 17 (1)
12: 5 (1)
13: 12 (1)
14: 16 (1)
15: 8 (3)
16: 23 (1)
17: 20 (1)
18: 6 (2)
19: 19 (1)
Order: 0 1 15 4 8 9 2 18 3 5 10 11 12 13 14 6 16 17 7 19 
*/


    //g_bStatMode = false;
    std::vector<DWORD> vecOrder;
    vecOrder.resize(20);
    for (int i = 0; i < 20; ++i) vecOrder[i] = i;
    for (int i = 0; i < 19; ++i)
    {
        for (int j = i + 1; j < 20; ++j)
        {
            if (mapBest[vecOrder[i]].second < mapBest[vecOrder[j]].second)
            {
                DWORD t = vecOrder[j];
                vecOrder[j] = vecOrder[i];
                vecOrder[i] = t;
            }
        }
    }
    g_of << "Order: ";
    for (int i = 0; i < 20; ++i) {
        g_of << std::dec << vecOrder[i] << " ";
    }
    g_of << endl;

    DWORD dwLastChanger = -1;
    DWORD dwLastChanger_Count = 0;
    
    // Manual fix
    //mapBest[0].first = 1;

    while (1)
    {
        int nBestChanger = -1;
        int nDiff = -1;
        for (int i = 0; i < 20; ++i)
        {
            int pos = vecOrder[i];
            if (s_vecPrev1[pos] != s_vecPrev2[pos])
            {
                nDiff = pos;
                nBestChanger = mapBest[pos].first;
                if (dwLastChanger == nBestChanger && dwLastChanger_Count >= 256)
                {
                    g_of << "Changer " << nBestChanger << " can't help, using another: ";
                    for (auto& k: mapBack[pos])
                    {
                        if (k != nBestChanger) {
                            nBestChanger = k;
                            break;
                        }
                    }
                    g_of << nBestChanger << endl;
                }
                
                if (nBestChanger == dwLastChanger) {
                    ++dwLastChanger_Count;
                }
                else {
                    dwLastChanger = nBestChanger;
                    dwLastChanger_Count = 1;
                }
                break;
            }
        }
        if (nBestChanger < 0) break;
        
        ++buffState[g_curPosChanging = nBestChanger];
        if (g_bExTrace)
        {
            g_of << "Detected mismatch at " << std::dec << nDiff << ", changing byte " << nBestChanger << ": ";
            for (int i = 0; i < nLen; ++i) {
                g_of << std::hex << std::setfill('0') << std::setw(2) << (unsigned int)(unsigned char)buffState[i];
            }
            g_of << endl;
        }

        std::string s = base64_encode((const unsigned char*)buffState, nLen);

        //g_of << "Processing " << std::setw(2) << nPos << " ";
        //for (int i = 0; i < nLen; ++i) {
        //    g_of << std::hex << std::setfill('0') << std::setw(2) << (unsigned int)(unsigned char)buffState[i];
        //}
        //g_of << std::endl;
        bool bResult = check_license(chEmail, s.c_str());

        if (bResult)
        {
            MessageBoxA(0, "Correct!", s.c_str(), MB_OK);
        }
    }



    MessageBoxW(0, L"Done", L"OK", MB_OK);
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
    )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
