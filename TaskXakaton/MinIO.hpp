#ifndef MODULE_MINIO
#define MODULE_MINIO

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/core/auth/AWSCredentials.h>
#include <fstream>

class FileMinio {
public:
    FileMinio(std::string bucket_m, std::string nameFileBucket_m, std::string tmpFile_m, std::string loginMinio_m, std::string passMinio_m);

    int Download();

    ~FileMinio();

private:
    Aws::SDKOptions options;
    std::string bucket;
    std::string nameFileBucket;
    std::string tmpFile;
    std::string loginMinio;
    std::string passMinio;
};

#endif