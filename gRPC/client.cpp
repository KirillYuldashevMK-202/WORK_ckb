#include <iostream>
#include <memory>
#include <string>
#include <filesystem>
#include <fstream>
#include <grpcpp/grpcpp.h>
#include "com.grpc.pb.h"
#include <Windows.h>
#include <iphlpapi.h>
#include <winsock2.h>
#include <fstream>

#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/core/client/ClientConfiguration.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/core/utils/Outcome.h>

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

std::string ReportInfo() {
    std::string info;
    info += "\nCommand Files:\n";
    info += ReadFiles();
    info += "Command Network:\n";
    info += ReadNetwork();
    info += "Command SystemInfo:\n";
    info += ReadSystemInfo();
    return info;
}

void MyUploadS3() {
    Aws::SDKOptions options;
    Aws::InitAPI(options);
    {
        //Конфигурация клиента
        Aws::Client::ClientConfiguration Config;
        Config.scheme = Aws::Http::Scheme::HTTP;
        Config.endpointOverride = "localhost:9000";
        Aws::Auth::AWSCredentials credentials("qwerty123", "qwerty1234");
        Aws::S3::S3Client s3_client(credentials, Config, Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy::Never, false);

        //Запрос
        Aws::S3::Model::PutObjectRequest Request;
        Request.SetBucket("mybucket");
        Request.SetKey("Report");
        auto data = Aws::MakeShared<Aws::FStream>("SampleAllocationTag", "C:/Users/Kirill/source/Work2/MainClient/report.txt", std::ios_base::in | std::ios_base::binary);
        Request.SetBody(data);

        //Отправка запроса
        auto Send = s3_client.PutObject(Request);
        if (Send.IsSuccess()) {
            std::cout << "File uploaded successfully!" << std::endl;
        }
        else {
            std::cout << "Error uploading file " << std::endl;
        }
    }
    Aws::ShutdownAPI(options);
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
    else if (command == "Report") {
        response = ReportInfo();
        std::ofstream Report("C:/Users/Kirill/source/Work2/MainClient/report.txt", std::ios_base::binary);
        if (!Report.is_open()) {
            std::cerr << "Error opening file for writing." << std::endl;
            exit(1);
        }
        Report.write(response.c_str(), response.size());
        if (Report.fail()) {
            std::cerr << "Error writing to file." << std::endl;
            Report.close();
            exit(1);
        }
        Report.close();
    }
    else {
        response = "Unknown " + command;
    }
    client.SendResponse(response);
    MyUploadS3();
    return 0;
}

