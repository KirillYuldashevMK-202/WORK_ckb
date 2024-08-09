#include <windows.h>
#include <iostream>
#include <string>
#include <vector>

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

LRESULT CALLBACK Clipboard(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static std::string MainHistory;

    switch (uMsg) {
    case WM_CREATE:
        AddClipboardFormatListener(hwnd);
        break;
    case WM_CLIPBOARDUPDATE: {
        std::string Time = getTime();
        std::string Data = GetData();
        MainHistory += Time + " : " + Data + "\r\n";

        HWND TextWindow = GetDlgItem(hwnd, 1);
        SetWindowTextA(TextWindow, MainHistory.c_str());
        break;
    }
    case WM_DESTROY:
        RemoveClipboardFormatListener(hwnd);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    setlocale(LC_ALL, "Russian");
    WNDCLASS WND = { 0 };
    WND.lpfnWndProc = Clipboard;
    WND.hInstance = GetModuleHandleA(nullptr);
    WND.lpszClassName = L"ClipboardListener";

    RegisterClass(&WND);

    HWND hwnd = CreateWindowEx(0, WND.lpszClassName, L"Clipboard History", WS_OVERLAPPEDWINDOW,CW_USEDEFAULT, CW_USEDEFAULT, 500, 900, nullptr, nullptr, GetModuleHandleA(nullptr), nullptr);
    if (hwnd == nullptr) {
        return 1;
    }

    HWND TextWindow = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", nullptr, WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY, 0, 0, 500, 900, hwnd, (HMENU)1, hInstance, nullptr);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = { 0 };
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
