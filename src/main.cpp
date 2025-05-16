#include "hdfs_client.h"
#include <iostream>
#include <string>
#include <cstdlib> // For using getenv function

#define VERSION "1.0.0"

void printVersion() {
    std::cout << "HDFS Client Version " << VERSION << std::endl;
    std::cout << "A C++ client for interacting with Apache Hadoop HDFS" << std::endl;
    std::cout << "Using libhdfs native library" << std::endl;
}

void printUsage() {
    std::cout << "Usage: hdfs_client <command> [arguments]" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  list <path>            - List files in directory" << std::endl;
    std::cout << "  read <path>            - Read file content" << std::endl;
    std::cout << "  write <path> <content> - Write content to file" << std::endl;
    std::cout << "  delete <path>          - Delete file" << std::endl;
    std::cout << "  version                - Show version information" << std::endl;
    std::cout << "  help                   - Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Environment Variables:" << std::endl;
    std::cout << "  HDFS_DEFAULT_FS        - Default FileSystem URI (e.g. hdfs://namenode:8020)" << std::endl;
    std::cout << "  CLASSPATH              - Java classpath for HDFS libraries" << std::endl;
    std::cout << "  HADOOP_CONF_DIR        - Directory containing Hadoop configuration files" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 1;
    }

    std::string command = argv[1];
    
    // Handle special commands that don't require HDFS connection
    if (command == "version") {
        printVersion();
        return 0;
    } else if (command == "help") {
        printUsage();
        return 0;
    }

    // Show environment information for debugging
    const char* defaultFs = std::getenv("HDFS_DEFAULT_FS");
    if (defaultFs != nullptr) {
        std::cout << "Using HDFS_DEFAULT_FS: " << defaultFs << std::endl;
    }
    
    HdfsClient client;
    
    // Connect to HDFS
    if (!client.connect()) {
        std::cerr << "Failed to connect to HDFS" << std::endl;
        return 1;
    }

    if (command == "list" && argc >= 3) {
        std::string path = argv[2];
        std::vector<std::string> files = client.listDirectory(path);
        
        std::cout << "Files in " << path << ":" << std::endl;
        for (const auto& file : files) {
            std::cout << "  " << file << std::endl;
        }
    }
    else if (command == "read" && argc >= 3) {
        std::string path = argv[2];
        std::string content;
        
        if (client.readFile(path, content)) {
            std::cout << "Successfully read " << content.size() << " bytes from " << path << std::endl;
        } else {
            std::cerr << "Failed to read file: " << path << std::endl;
        }
    }
    else if (command == "write" && argc >= 4) {
        std::string path = argv[2];
        std::string content = argv[3];
        
        if (client.writeFile(path, content)) {
            std::cout << "Successfully wrote " << content.size() << " bytes to file: " << path << std::endl;
        } else {
            std::cerr << "Failed to write to file: " << path << std::endl;
        }
    }
    else if (command == "delete" && argc >= 3) {
        std::string path = argv[2];
        
        if (client.deleteFile(path)) {
            std::cout << "Successfully deleted file: " << path << std::endl;
        } else {
            std::cerr << "Failed to delete file: " << path << std::endl;
        }
    }
    else {
        printUsage();
        return 1;
    }

    // Disconnect from HDFS
    client.disconnect();
    
    return 0;
} 