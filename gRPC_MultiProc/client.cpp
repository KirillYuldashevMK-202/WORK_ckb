#include <iostream>
#include <string>
#include <grpcpp/grpcpp.h>
#include "com.grpc.pb.h"
#include <Windows.h>
#include <winsock2.h>
#include <vector>
#include <shellapi.h>


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
            std::cout << "RPC Error: " << status.error_message() << std::endl;
        }
    }

private:
    std::unique_ptr<com::ComService::Stub> stub_;
};

void MyRunExe(const std::string& Myexe) {
    SHELLEXECUTEINFOA sei = { 0 }; 
    sei.cbSize = sizeof(sei);
    sei.lpVerb = NULL;
    sei.lpFile = Myexe.c_str();
    sei.nShow = SW_SHOWNORMAL;

    HINSTANCE result = ShellExecuteA(NULL, sei.lpVerb, sei.lpFile, NULL, NULL, sei.nShow);

    if ((int)result <= 32) {
        std::cerr << "ShellExecuteA error: " << (int)result << std::endl;
    }
}

int main(int argc, char** argv) {
    std::string target_str = "192.168.44.130:50051";
    CommandClient client(grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
    std::string nameObject;
    while (true) {
        std::string command = client.GetCommand();
        if (command == "Files") {
            MyRunExe("C:/Users/Kirill/source/Work2/MainClient/FilesProc.exe");
            nameObject = "FilesCommand";
            client.SendResponse("http://localhost:9000/mybucket/" + nameObject);
        }
        else if (command == "Network") {
            MyRunExe("C:/Users/Kirill/source/Work2/MainClient/NetworkProc.exe");
            nameObject = "NetworkCommand";
            client.SendResponse("http://localhost:9000/mybucket/" + nameObject);
        }
        else if (command == "SystemInfo") {
            MyRunExe("C:/Users/Kirill/source/Work2/MainClient/SystemProc.exe");
            nameObject = "SystemCommand";
            client.SendResponse("http://localhost:9000/mybucket/" + nameObject);
        }
        else {
            client.SendResponse("Unknown command");
        }
    }
    return 0;
}
