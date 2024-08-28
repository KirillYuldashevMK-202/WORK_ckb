#include "MinIO.hpp"
#include "Macros.hpp"

FileMinio::FileMinio(std::string bucket_m, std::string nameFileBucket_m, std::string tmpFile_m, std::string loginMinio_m, std::string passMinio_m) {
    this->bucket = bucket_m;
    this->nameFileBucket = nameFileBucket_m;
    this->tmpFile = tmpFile_m;
    this->loginMinio = loginMinio_m;
    this->passMinio = passMinio_m;
}

int FileMinio::Download() {
    Aws::InitAPI(this->options);
    {
        Aws::Auth::AWSCredentials credentials(this->loginMinio, this->passMinio);
        Aws::Client::ClientConfiguration clientConfig;
        clientConfig.scheme = Aws::Http::Scheme::HTTP;
        clientConfig.endpointOverride = "localhost:9000";

        Aws::S3::S3Client s3_client(credentials, clientConfig, Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy::Never, false);

        Aws::S3::Model::GetObjectRequest request;
        request.SetBucket(this->bucket.c_str());
        request.SetKey(this->nameFileBucket.c_str());

        auto outcome = s3_client.GetObject(request);
        if (outcome.IsSuccess()) {
            std::ofstream local_file(this->tmpFile, std::ios::binary);
            local_file << outcome.GetResult().GetBody().rdbuf();
            local_file.close();
            return OK;
        }
        else {
            return ERROR;
        }
    }
}

FileMinio::~FileMinio() {
    Aws::ShutdownAPI(this->options);
}

