#include <windows.h>
#include <iostream>
#include <vector>
#include <string>

std::string GetData() {
    IsClipboardFormatAvailable(CF_UNICODETEXT);
    OpenClipboard(nullptr);

    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
    wchar_t* pData = static_cast<wchar_t*>(GlobalLock(hData));
    std::wstring MainData;
    if (pData != NULL) {
        MainData = pData;
    }
    int sizeStr = WideCharToMultiByte(CP_ACP, 0, MainData.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string strReturn(sizeStr - 1, 0);

    WideCharToMultiByte(CP_ACP, 0, MainData.c_str(), -1, &strReturn[0], sizeStr, nullptr, nullptr);

    GlobalUnlock(hData);
    CloseClipboard();
    return strReturn;
}

LRESULT CALLBACK Clipboard(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        AddClipboardFormatListener(hwnd);
        break;
    case WM_CLIPBOARDUPDATE:
        GetData();
        break;
    case WM_DESTROY:
        RemoveClipboardFormatListener(hwnd);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

std::string getTime() {
    time_t now = time(nullptr);
    tm localTime;
    localtime_s(&localTime, &now);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &localTime);
    std::string nameFile;
    for (int i = 0; i < 19; i += 1) {
        nameFile += buffer[i];
    }

    return nameFile;
}

int main() {
    setlocale(LC_ALL, "Russian");
    WNDCLASS WND = { 0 };
    WND.lpfnWndProc = Clipboard;
    WND.hInstance = GetModuleHandleA(nullptr);
    WND.lpszClassName = L"ClipboardListener";

    RegisterClass(&WND);
      
    HWND hwnd = CreateWindowEx(0, WND.lpszClassName, L"Clipboard History", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, GetModuleHandleA(nullptr), nullptr);
    if (hwnd == nullptr) {
        return 1;
    }

    MSG msg = { 0 };
    std::cout << "History buffer:" << std::endl;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        std::cout << getTime() << " : ";
        std::cout << GetData() << std::endl;
    }

    return 0;
}
