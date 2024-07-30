#include "MyPlugin.h"
#include <iostream>
#include <Windows.h>
#include <fstream>
#include <vector>
#include <sstream>

#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/core/client/ClientConfiguration.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/core/utils/Outcome.h>

typedef Module1* (__cdecl* MYPROCcreate)();
typedef void(__cdecl* MYPROCdelete)(Module1*);

void MyUploadS3(std::string dataCommand, std::string nameObject) {
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
        }
        else {
            std::cout << "Error uploading file " << std::endl;
        }
    }
    Aws::ShutdownAPI(options);
}

int main() {
	std::string pluginsDir = "C:/Users/Kirill/source/Work1/PLuginsSystem/MyPLugin/";
	std::string pluginFiles;
	//pluginFiles = (pluginsDir + "FilesMOD.dll");
	pluginFiles = (pluginsDir + "SystemMOD.dll");
	//pluginFiles = (pluginsDir + "NetworkMOD.dll");

	HMODULE MyPlugins = LoadLibraryA(pluginFiles.c_str());

	MYPROCcreate Mycreate = (MYPROCcreate)GetProcAddress(MyPlugins, "CreatePlugin");
	Module1* Plugin = Mycreate();
	if (!Plugin) {
		std::cout << "Error: Plugin" << pluginFiles << "not create" << std::endl;
		FreeLibrary(MyPlugins);
		return 1;
	}

	const DWORD bufferSize = 2048;
	char buffer[bufferSize] = { 0 };
	DWORD size;
	std::string Data;

	Plugin->Command(nullptr, &size);

	if (size < bufferSize) {
		Plugin->Command(buffer, &size);
		Data = buffer;
		std::cout << Data << std::endl;
	}
	else {
		std::cerr << "Buffer is too small!" << std::endl;
	}

    //MyUploadS3(Data, "FilesCommand");
    //MyUploadS3(Data, "NetworkCommand");
    MyUploadS3(Data, "SystemCommand");

	MYPROCdelete Mydelete = (MYPROCdelete)GetProcAddress(MyPlugins, "DeletePlugin");
	Mydelete(Plugin);

	return 0;
}
