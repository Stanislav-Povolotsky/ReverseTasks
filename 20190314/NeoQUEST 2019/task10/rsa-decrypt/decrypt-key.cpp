// decrypt-key.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <windows.h>
#include <iostream>
#include <wincrypt.h>
#include <vector>
#include <fstream>

std::vector<char> readFile(const char* filename)
{
    // open the file:
    std::streampos fileSize;
    std::ifstream file(filename, std::ios::binary);

    // get its size:
    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // read the data:
    std::vector<char> fileData((size_t)fileSize);
    file.read((char*)&fileData[0], (size_t)fileSize);
    return fileData;
}

void writeFile(const char* filename, std::vector<char>& data)
{
    // open the file:
    std::ofstream file(filename, std::ios::binary);
    file.write(data.data(), data.size());
}


int main()
{
    auto vecRSAPrivateKey = readFile("priv.rsa.bin");
    auto vecDataToDecrypt = readFile("aes.key-info.bin");

    std::cout << "Loaded RSA Key: " << vecRSAPrivateKey.size() << " bytes" << std::endl;
    std::cout << "Loaded encrypted data: " << vecDataToDecrypt.size() << " bytes" << std::endl;

    HCRYPTPROV phProv = 0;
    bool bOK = false;

    if (CryptAcquireContextW(&phProv, 0u, L"Microsoft Enhanced Cryptographic Provider v1.0", PROV_RSA_FULL, 0u))
    {
        HCRYPTKEY phKey = 0;
        if (CryptImportKey(phProv, (const BYTE*)vecRSAPrivateKey.data(), (DWORD)vecRSAPrivateKey.size(), 0, 0, &phKey))
        {
            DWORD dwDataLen = (DWORD)vecDataToDecrypt.size();
            if (CryptDecrypt(phKey, 0, TRUE, 0, (BYTE *)vecDataToDecrypt.data(), &dwDataLen))
            {
                vecDataToDecrypt.resize(dwDataLen);
                writeFile("aes.key.decrypted.bin", vecDataToDecrypt);
                //CryptDestroyKey(phKey);
                //CryptReleaseContext(phProv, 0);
                bOK = true;
            }
            else
            {
                std::cout << "CryptDecrypt Failed: " << ::GetLastError() << std::endl;
            }
        }
        else
        {
            std::cout << "CryptImportKey Failed: " << ::GetLastError() << std::endl;
        }
    }
    else
    {
        std::cout << "CryptAcquireContextW Failed: " << ::GetLastError() << std::endl;
    }
    std::cout << (bOK ? "Done." : "Failed") << std::endl;
}
