#include <windows.h>
#include <ktmw32.h>
#pragma comment(lib, "KtmW32.lib")

int main()
{
    HANDLE hTrans = INVALID_HANDLE_VALUE;
    while (1)
    {
        hTrans = ::CreateTransaction(nullptr, nullptr, TRANSACTION_DO_NOT_PROMOTE, 0, 0, INFINITE, nullptr);
        if (hTrans == INVALID_HANDLE_VALUE) break;
        
        HKEY hkTest = 0;
        if (NO_ERROR != ::RegCreateKeyTransactedW(HKEY_CURRENT_USER, L"Software\\SP\\test", 0, nullptr, 0,
            KEY_ALL_ACCESS, nullptr, &hkTest, nullptr, hTrans, nullptr)) break;

        wchar_t wsOpt1[] = L"value1";
        wchar_t wsOpt2[] = L"value 2";
        if (NO_ERROR == ::RegSetKeyValueW(hkTest, nullptr, L"opt1", REG_SZ, wsOpt1, sizeof(wsOpt1)) &&
            NO_ERROR == ::RegSetKeyValueW(hkTest, nullptr, L"opt2", REG_SZ, wsOpt2, sizeof(wsOpt2)))
        {
            ::CommitTransaction(hTrans);
        }
        
        ::RegCloseKey(hkTest);
        break;
    }
    if (hTrans != INVALID_HANDLE_VALUE) ::CloseHandle(hTrans);
}