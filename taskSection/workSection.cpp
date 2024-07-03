#include <iostream>
#include <string>
#include <cstring>
#include <Windows.h>

#pragma section(".mdata", read, write)
__declspec(allocate(".mdata")) char mdata[164] = "Test1\0\nTest2\0\nTest3\0\nTest4\0\nTest5\0\nTest6\0\nTest7\0\nTest8\0\nTest9\0\nTest10\0\n<TEMPLATE>Msgbox str\0\n1Test\0\n2Test\0\n3Test\0\n4Test\0\n5Test\0\n6Test\0\n7Test\0\n8Test\0\n9Test\0\n10Test\0";

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    std::cout << "Original data:" << std::endl;
    char* ptr = mdata;
    std::string msg;
    while (*ptr != '\0') {
        std::cout << ptr << std::endl;
        msg += ptr;
        msg += "\n";
        ptr += strlen(ptr) + 1;
    }
    MessageBoxA(NULL, msg.c_str(), "STR", MB_OK);
    return 0;
}
