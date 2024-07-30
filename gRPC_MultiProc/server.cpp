#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "com.grpc.pb.h"


class CommandService : public com::ComService::Service {
    grpc::Status GetCommand(grpc::ServerContext* context, const com::Empty* request, com::ComResponse* reply) override {
        std::string commandMain;
        getline(std::cin,commandMain);
        reply->set_com(commandMain);
        return grpc::Status::OK;
    }
    grpc::Status SendResponse(grpc::ServerContext* context, const com::Response* request, com::Empty* reply) override {
        std::string response = request->resp();
        std::cout << "Response received: " << response << std::endl;
        return grpc::Status::OK;
    }
};

int main(int argc, char** argv) {
    std::string ServAddres("192.168.44.130:50051");
    CommandService Service;
    grpc::ServerBuilder Build;

    Build.AddListeningPort(ServAddres, grpc::InsecureServerCredentials());
    Build.RegisterService(&Service);

    std::unique_ptr<grpc::Server> server(Build.BuildAndStart());
    std::cout << "Server created " << ServAddres << std::endl;
    server->Wait();
    return 0;
}
