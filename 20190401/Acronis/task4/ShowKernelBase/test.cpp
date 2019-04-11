#include <windows.h>
#include <stdio.h>

int main()
{
    printf("Child process KernelBase base addr:\t0x%p\n", ::GetModuleHandle(L"KernelBase.dll"));
    printf("Child process base addr:\t0x%p\n", ::GetModuleHandle(nullptr));
    printf("Press ENTER to EXIT child process\n");
    getc(stdin);
    return 0;
}