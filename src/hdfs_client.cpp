#include "hdfs_client.h"
#include "hdfs_builder.h"
#include <iostream>
#include <fcntl.h>
#include <vector>
#include <cstring>
#include <cstdlib> // For using getenv function

HdfsClient::HdfsClient() : fs_(nullptr), connected_(false) {
}

HdfsClient::~HdfsClient() {
    disconnect();
}

bool HdfsClient::connect() {
        // Use ConfigLoader to load client.conf
    ConfigLoader configLoader;
    std::string confPath;
    // If configuration file path is not specified, use default path
    if (confPath.empty()) {
        // Try to get configuration directory from environment variable
        const char* confDir = std::getenv("HADOOP_CONF_DIR");
        if (confDir != nullptr) {
            confPath = std::string(confDir) + "/client.conf";
        } else {
            confPath = "conf/client.conf";
        }
    }
    
    std::cout << "Loading client configuration from " << confPath << std::endl;
    bool configLoaded = configLoader.loadFromFile(confPath);
    
    if (!configLoaded) {
        std::cout << "Warning: Could not load client configuration from " << confPath << std::endl;
    } else {
        // Print all loaded configurations
        configLoader.printConfigs();
    }
    
    // Use HdfsBuilder to create connection
    HdfsBuilder builder;
    
    // First try to read HDFS_DEFAULT_FS from environment variable
    const char* defaultFs = std::getenv("HDFS_DEFAULT_FS");
    if (defaultFs == nullptr || strlen(defaultFs) == 0) {
        std::cerr << "HDFS_DEFAULT_FS is not set" << std::endl;
        return false;
    }
    std::string hdfsUri = defaultFs;
    builder.setNameNode(hdfsUri);

    // Configure necessary filesystem implementation classes
    builder.setConfig("fs.hdfs.impl", "org.apache.hadoop.hdfs.DistributedFileSystem");
    builder.setConfig("fs.AbstractFileSystem.hdfs.impl", "org.apache.hadoop.fs.Hdfs");
    builder.setConfig("fs.hdfs.impl.disable.cache", "true");
    
    std::string principal;
    std::string keytabFile;
    std::string krb5Conf;

    // Apply configurations from client.conf - this will override previous configurations
    if (configLoaded) {
        builder.applyConfigs(configLoader);
        
        // Check if Kerberos authentication related configurations are set
        if (configLoader.hasConfig("hadoop.security.authentication") && 
            configLoader.getConfigValue("hadoop.security.authentication") == "kerberos") {
            std::cout << "Kerberos authentication is enabled in configuration" << std::endl;
            
            // Check if principal and keytab are provided
            if (configLoader.hasConfig("hadoop.kerberos.principal") && configLoader.hasConfig("hadoop.kerberos.keytab")) {
                principal = configLoader.getConfigValue("hadoop.kerberos.principal");
                keytabFile = configLoader.getConfigValue("hadoop.kerberos.keytab");
                
                std::cout << "Using Kerberos principal: " << principal << std::endl;
                std::cout << "Using Kerberos keytab file: " << keytabFile << std::endl;
                
                builder.setPrincipal(principal);
                builder.setKeyTabFile(keytabFile);
            } else {
                std::cerr << "Kerberos authentication is enabled, but principal or keytab is missing in configuration" << std::endl;
            }

            if (configLoader.hasConfig("hadoop.kerberos.krb5.conf")) {
                krb5Conf = configLoader.getConfigValue("hadoop.kerberos.krb5.conf");
                std::cout << "Using Kerberos krb5.conf file: " << krb5Conf << std::endl;
                builder.setKrb5Conf(krb5Conf);
            }
        }
    }
    
    std::cout << "FileSystem implementation set to: org.apache.hadoop.hdfs.DistributedFileSystem" << std::endl;
    
    // Connect to HDFS
    fs_ = builder.connect();
    connected_ = (fs_ != nullptr);
    
    if (!connected_) {
        std::cerr << "Failed to connect to HDFS using builder" << std::endl;
    }
    
    return connected_;
}

void HdfsClient::disconnect() {
    if (connected_ && fs_) {
        std::cout << "Disconnecting from HDFS" << std::endl;
        
        hdfsDisconnect(fs_);
        fs_ = nullptr;
        connected_ = false;
    }
}

std::vector<std::string> HdfsClient::listDirectory(const std::string& path) {
    std::vector<std::string> result;
    
    if (!connected_ || !fs_) {
        std::cerr << "Not connected to HDFS" << std::endl;
        return result;
    }
    
    std::cout << "Listing directory: " << path << std::endl;
    
    int numEntries = 0;
    hdfsFileInfo* fileInfo = hdfsListDirectory(fs_, path.c_str(), &numEntries);
    
    if (fileInfo) {
        for (int i = 0; i < numEntries; i++) {
            result.push_back(fileInfo[i].mName);
        }
        hdfsFreeFileInfo(fileInfo, numEntries);
    } else {
        std::cerr << "Failed to list directory: " << path << std::endl;
    }
    
    return result;
}

bool HdfsClient::readFile(const std::string& path, std::string& content) {
    if (!connected_ || !fs_) {
        std::cerr << "Not connected to HDFS" << std::endl;
        return false;
    }
    
    std::cout << "Reading file: " << path << std::endl;
    
    hdfsFile file = hdfsOpenFile(fs_, path.c_str(), O_RDONLY, 0, 0, 0);
    if (!file) {
        std::cerr << "Failed to open file for reading: " << path << std::endl;
        return false;
    }
    
    hdfsFileInfo* fileInfo = hdfsGetPathInfo(fs_, path.c_str());
    if (!fileInfo) {
        std::cerr << "Failed to get file info: " << path << std::endl;
        hdfsCloseFile(fs_, file);
        return false;
    }
    
    tSize fileSize = fileInfo->mSize;
    hdfsFreeFileInfo(fileInfo, 1);
    
    // Limit reading to at most 4096 bytes
    const tSize maxReadSize = 4096;
    tSize bytesToRead = (fileSize > maxReadSize) ? maxReadSize : fileSize;
    
    std::vector<char> buffer(bytesToRead + 1, 0);
    
    tSize bytesRead = hdfsRead(fs_, file, buffer.data(), bytesToRead);
    hdfsCloseFile(fs_, file);
    
    if (bytesRead != bytesToRead) {
        std::cerr << "Failed to read requested bytes: " << path << std::endl;
        std::cerr << "Read " << bytesRead << " of " << bytesToRead << " bytes" << std::endl;
        return false;
    }
    
    content.assign(buffer.data(), bytesRead);
    
    // If file was larger than maxReadSize, log that we truncated it
    if (fileSize > maxReadSize) {
        std::cout << "Note: File size (" << fileSize << " bytes) exceeds maximum read size. Content truncated to " << maxReadSize << " bytes." << std::endl;
    }
    
    return true;
}

bool HdfsClient::writeFile(const std::string& path, const std::string& content) {
    if (!connected_ || !fs_) {
        std::cerr << "Not connected to HDFS" << std::endl;
        return false;
    }
    
    std::cout << "Writing to file: " << path << std::endl;
    
    hdfsFile file = hdfsOpenFile(fs_, path.c_str(), O_WRONLY | O_CREAT, 0, 0, 0);
    if (!file) {
        std::cerr << "Failed to open file for writing: " << path << std::endl;
        return false;
    }
    
    tSize bytesWritten = hdfsWrite(fs_, file, content.c_str(), content.length());
    
    hdfsFlush(fs_, file);
    hdfsCloseFile(fs_, file);
    
    if (bytesWritten != content.length()) {
        std::cerr << "Failed to write complete content: " << path << std::endl;
        std::cerr << "Wrote " << bytesWritten << " of " << content.length() << " bytes" << std::endl;
        return false;
    }
    
    return true;
}

bool HdfsClient::deleteFile(const std::string& path) {
    if (!connected_ || !fs_) {
        std::cerr << "Not connected to HDFS" << std::endl;
        return false;
    }
    
    std::cout << "Deleting file: " << path << std::endl;
    
    int result = hdfsDelete(fs_, path.c_str(), 0);
    if (result != 0) {
        std::cerr << "Failed to delete file: " << path << std::endl;
        return false;
    }
    
    return true;
}