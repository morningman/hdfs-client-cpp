#ifndef HDFS_CLIENT_H
#define HDFS_CLIENT_H

#include <string>
#include <vector>
#include <hdfs.h>
#include "hdfs_builder.h"
#include "config_loader.h"

class HdfsClient {
public:
    HdfsClient();
    ~HdfsClient();

    // Connect to HDFS
    bool connect();

    // Disconnect from HDFS
    void disconnect();
    
    // List files in a directory
    std::vector<std::string> listDirectory(const std::string& path);
    
    // Read a file from HDFS
    bool readFile(const std::string& path, std::string& content);
    
    // Write a file to HDFS
    bool writeFile(const std::string& path, const std::string& content);
    
    // Delete a file from HDFS
    bool deleteFile(const std::string& path);

private:
    hdfsFS fs_;
    bool connected_;
};

#endif // HDFS_CLIENT_H 