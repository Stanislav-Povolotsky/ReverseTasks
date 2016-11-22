#include "stdafx.h"
#include "mylib.h"
#include <sstream>

#pragma pack(push, 1)
struct SExtendedRandData
{
    int nCurTotalEncodedBytes;
    int field_4;
    DWORD ar256_DW[255];
    int field_404;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct CClass1
{
public:
    __int16 nSomeWord_val0[2];
    int nZero2;
    int nZero3;
    int nZero4;
    int nSomeData_val0;
    int nSomeData_val7;
    int rand_seed;
    char *pRandom16BytesStr;
    SExtendedRandData *pExtendedRandMap;
    int nZero5;
    int nSomeFixedVal1;
    int nSomeFixedVal2;
    int nByteValChangedInEncode1;
    int nByteValChangedInEncode2;
    int nTotalEncodedBytes;

public:
    int EncodeFileData1(char *pData, size_t nSize);
    int PrepareRandBytesBySeed();
    int TransformRandBytes();
    int CleanupEncCtx();
    ~CClass1()
    {
        CleanupEncCtx();
    }
};
#pragma pack(pop)


int CClass1::EncodeFileData1(char *pData, size_t nSize)
{
    typedef decltype(&CClass1::EncodeFileData1) Pfn_t;
    union
    {
        DWORD_PTR pfnAddr;
        Pfn_t pfn;
    } pfn = { 0x0040EC60 };
    return (this->*pfn.pfn)(pData, nSize);
}

int CClass1::PrepareRandBytesBySeed()
{
    typedef decltype(&CClass1::PrepareRandBytesBySeed) Pfn_t;
    union
    {
        DWORD_PTR pfnAddr;
        Pfn_t pfn;
    } pfn = { 0x40F700 };
    return (this->*pfn.pfn)();
}

int CClass1::TransformRandBytes()
{
    typedef decltype(&CClass1::TransformRandBytes) Pfn_t;
    union
    {
        DWORD_PTR pfnAddr;
        Pfn_t pfn;
    } pfn = { 0x40F740 };
    return (this->*pfn.pfn)();
}

int CClass1::CleanupEncCtx()
{
    typedef decltype(&CClass1::CleanupEncCtx) Pfn_t;
    union
    {
        DWORD_PTR pfnAddr;
        Pfn_t pfn;
    } pfn = { 0x40EBC0 };
    return (this->*pfn.pfn)();
}


/*
// Место, куда можно поместить лоадер:
manbecareful.mod02.no_reloc:0041CA5F __invoke_watson proc near; CODE XREF : sub_415733 : loc_415774p
// Полезные импортируемые функции
manbecareful.mod02.no_reloc:004431A0 off_4431A0      dd offset kernel32_LoadLibraryW
manbecareful.mod02.no_reloc:00443000 off_443000      dd offset kernel32_GetProcAddress
// Код инжектора:

6A6C                         push        06C
54                           push        esp
FF15A0314400                 call        d,[004431A0]
54                           push        esp
50                           push        eax
FF1500304400                 call        d,[00443000]
83C404                       add         esp,004 
FFD0                         call        eax

eb eip 6A 6C 54 FF 15 A0 31 44 00 54 50 FF 15 00 30 44 00 83 C4 04 FF D0

*/

char g_testFile1[] 
{
/*01900020*/0x52,0x61,0x72,0x21,0x1a,0x07,0x00,0xcf, 0x90,0x73,0x00,0x00,0x0d,0x00,0x00,0x00, /*Rar!.....s...... */
/*01900030*/0x00,0x00,0x00,0x00,0xa0,0x41,0x74,0x20, 0x90,0x38,0x00,0x1c,0x00,0x00,0x00,0x2c, /*.....At .8....., */
/*01900040*/0x00,0x00,0x00,0x02,0x2a,0xff,0x90,0x57, 0x77,0x10,0x61,0x49,0x1d,0x33,0x13,0x00, /*....*..Ww.aI.3.. */
/*01900050*/0x20,0x00,0x00,0x00,0x42,0x6c,0x61,0x63, 0x6b,0x41,0x63,0x63,0x6f,0x75,0x6e,0x74, /* ...BlackAccount */
/*01900060*/0x69,0x6e,0x67,0x2e,0x74,0x78,0x74,0x00, 0xb0,0x90,0x9f,0x64,0x00,0xc1,0x08,0xfe, /*ing.txt....d.... */
/*01900070*/0x0c,0x10,0x94,0xbd,0x4a,0x65,0xf0,0xc1, 0xfe,0xa6,0x12,0x22,0x0a,0x46,0xe0,0x1d, /*....Je.....".F.. */
/*01900080*/0x3c,0x5f,0x34,0x56,0x78,0x9a,0xbc,0xd4, 0xc4,0x3d,0x7b,0x00,0x40,0x07,0x00       /*<_4Vx.... = { .@.*/
};

#pragma warning(disable: 4996)
void WriteFileContent(LPCWSTR sFile, char *pData, size_t nSize)
{
    FILE* f = _wfopen(sFile, L"wb");
    fwrite(pData, nSize, 1, f);
    fclose(f);
}

// Main function
extern "C" void __stdcall l_test1()
{
    MessageBoxW(0, L"Stop", L"OK", MB_OK);

    CClass1 encCtx;
    memset(&encCtx, 0, 0x3Cu);
    //pFNHash = (int)&valHash_TickCountAtStart;
    //nRandSeed = (int)*GetAPIFn(&pFNHash);
    encCtx.nSomeData_val7 = 7;
    encCtx.nSomeData_val0 = 0;
    encCtx.nSomeWord_val0[0] = 0;
    //LOBYTE(v77) = 10;
    int nRandSeed = 0x0097a008;
    encCtx.rand_seed = nRandSeed;
    encCtx.PrepareRandBytesBySeed();
    encCtx.TransformRandBytes();

    auto pFileDataBuff = g_testFile1;
    auto nFileSize_ = sizeof(g_testFile1);

    WriteFileContent(L"SrcFile.bin", pFileDataBuff, nFileSize_);
    encCtx.EncodeFileData1(pFileDataBuff, nFileSize_);
    WriteFileContent(L"EncFile.bin", pFileDataBuff, nFileSize_);

    MessageBoxW(0, L"Done", L"OK", MB_OK);
}

extern "C" void __stdcall l_brute_tick()
{
    // Bruteforce tick counter
    MessageBoxW(0, L"Stop", L"OK", MB_OK);

    const char arReqPattern[] = {
        0x70, 0xB9, 0xA2, 0x21
    };

    for (DWORD dwTickTickCounter = 0; dwTickTickCounter < 0xFFFFFFFF; ++dwTickTickCounter)
    {
        CClass1 encCtx;
        memset(&encCtx, 0, 0x3Cu);
        //pFNHash = (int)&valHash_TickCountAtStart;
        //nRandSeed = (int)*GetAPIFn(&pFNHash);
        encCtx.nSomeData_val7 = 7;
        encCtx.nSomeData_val0 = 0;
        encCtx.nSomeWord_val0[0] = 0;
        //LOBYTE(v77) = 10;
        int nRandSeed = dwTickTickCounter;
        encCtx.rand_seed = nRandSeed;
        encCtx.PrepareRandBytesBySeed();
        encCtx.TransformRandBytes();

        char curData[sizeof(g_testFile1)];
        memcpy(curData, g_testFile1, sizeof(curData));

        auto pFileDataBuff = curData;
        auto nFileSize_ = sizeof(curData);

        //WriteFileContent(L"SrcFile.bin", pFileDataBuff, nFileSize_);
        encCtx.EncodeFileData1(pFileDataBuff, nFileSize_);

        if (0 == memcmp(pFileDataBuff, arReqPattern, sizeof(arReqPattern)))
        {
            std::wstringstream sName;
            sName << L"EncFile-0x" << std::hex << nRandSeed << L".bin";
            WriteFileContent(sName.str().c_str(), pFileDataBuff, nFileSize_);
        }

        encCtx.CleanupEncCtx(); 
    }

    MessageBoxW(0, L"Done", L"OK", MB_OK);
}

extern "C" void __stdcall l()
{
    // Bruteforce data
    MessageBoxW(0, L"Start", L"OK", MB_OK);

    const char arEncData[] = {
/*0000000000:*/0x70,0xB9,0xA2,0x21,0x4C,0x48,0xA9,0xEE,0x17,0x0D,0xE4,0xCA,0x99,0xEC,0x49,0xFC,
/*0000000010:*/0xFC,0x2A,0x58,0xCA,0x63,0x50,0x97,0x30,0x3E,0x4C,0xDF,0xE4,0xDD,0x36,0x26,0x95,
/*0000000020:*/0xD3,0x61,0x56,0xA8,0x2B,0x6A,0xBA,0x68,0xF6,0x91,0x2C,0x60,0x25,0x9B,0xB6,0xEB,
/*0000000030:*/0xA6,0xE8,0x57,0xF1,0x9E,0x38,0x7C,0xF5,0x0E,0x8B,0x9A,0xC8,0x5D,0xA8,0x44,0x13,
/*0000000040:*/0x59,0xA5,0xAA,0x83,0x4F,0x35,0x7C,0xA4,0xA6,0x34,0xE3,0xE9,0x91,0xA4,0xCB,0xF3,
/*0000000050:*/0xBA,0x42,0x46,0x2A,0x7B,0x75,0x72,0xFF,0x93,0xAF,0x4E,0xD3,0x98,0xB6,0x7D,0x51,
/*0000000060:*/0xE0,0x7F,0xFA,0x7D,0x88,0xEB,0xA3,0xC6,0x1C,0x87,0xDB,0x35,0x18,0x2B,0x04
    };

    char curBruteData[sizeof(arEncData)];
    size_t nPos = 0;

    DWORD dwTickCounterKey = 0xde2bc3;  // First of keys

    while (nPos < sizeof(curBruteData))
    {
        for (unsigned int val = 0; val <= 0xFF; ++val)
        {
            curBruteData[nPos] = val;

            CClass1 encCtx;
            memset(&encCtx, 0, 0x3Cu);
            //pFNHash = (int)&valHash_TickCountAtStart;
            //nRandSeed = (int)*GetAPIFn(&pFNHash);
            encCtx.nSomeData_val7 = 7;
            encCtx.nSomeData_val0 = 0;
            encCtx.nSomeWord_val0[0] = 0;
            //LOBYTE(v77) = 10;
            int nRandSeed = dwTickCounterKey;
            encCtx.rand_seed = nRandSeed;
            encCtx.PrepareRandBytesBySeed();
            encCtx.TransformRandBytes();

            char curData[sizeof(curBruteData)];
            memcpy(curData, curBruteData, sizeof(curData));

            auto pFileDataBuff = curData;
            auto nFileSize_ = sizeof(curData);

            //WriteFileContent(L"SrcFile.bin", pFileDataBuff, nFileSize_);
            encCtx.EncodeFileData1(pFileDataBuff, nFileSize_);

            if (0 == memcmp(arEncData, curData, sizeof(curData)))
            {
                std::wstringstream sName;
                sName << L"DecFile.bin";
                WriteFileContent(sName.str().c_str(), curBruteData, sizeof(curBruteData));
                MessageBoxW(0, L"FiniSH!!!", L"OK", MB_OK);
                break;
            }

            encCtx.CleanupEncCtx();

            if (curData[nPos] == arEncData[nPos])
            {
                std::wstringstream sDbg;
                sDbg << L"byte found " << nPos << L": " << std::hex << (ULONG)(unsigned char)curBruteData[nPos] << "\n";
                OutputDebugStringW(sDbg.str().c_str());
                // This byte was found
                break;
            }
        }
        nPos++;
    }

    /*
    {
        std::wstringstream sName;
        sName << L"LastState.bin";
        WriteFileContent(sName.str().c_str(), curBruteData, sizeof(curBruteData));
    }
    */

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
