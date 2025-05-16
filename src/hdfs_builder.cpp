#include "hdfs_builder.h"
#include <iostream>
#include <cstring>

HdfsBuilder::HdfsBuilder() : useDefault_(false) {
    // Create a new hdfsBuilder instance
    builder_ = hdfsNewBuilder();
    if (!builder_) {
        std::cerr << "Failed to create hdfsBuilder instance" << std::endl;
    }
}

HdfsBuilder::~HdfsBuilder() {
    // Note: When connect() calls hdfsBuilderConnect, the builder is automatically released
    // Therefore, manual release is only needed when builder_ is not null and connect hasn't been called
    if (builder_) {
        std::cout << "Freeing unused builder" << std::endl;
        hdfsFreeBuilder(builder_);
        builder_ = nullptr;
    }
}

HdfsBuilder& HdfsBuilder::setNameNode(const std::string& namenode) {
    if (builder_) {
        hdfsBuilderSetNameNode(builder_, namenode.c_str());
    } else {
        std::cerr << "Error: Builder not initialized" << std::endl;
    }
    return *this;
}

HdfsBuilder& HdfsBuilder::setPort(int port) {
    if (builder_) {
        hdfsBuilderSetNameNodePort(builder_, port);
    } else {
        std::cerr << "Error: Builder not initialized" << std::endl;
    }
    return *this;
}

HdfsBuilder& HdfsBuilder::useDefaultConfig() {
    if (builder_) {
        // Indicate to use default configuration
        useDefault_ = true;
    } else {
        std::cerr << "Error: Builder not initialized" << std::endl;
    }
    return *this;
}

HdfsBuilder& HdfsBuilder::configFromFile(const std::string& path) {
    if (builder_) {
        hdfsBuilderConfSetStr(builder_, "hadoop.conf.dir", path.c_str());
    } else {
        std::cerr << "Error: Builder not initialized" << std::endl;
    }
    return *this;
}

HdfsBuilder& HdfsBuilder::setConfig(const std::string& key, const std::string& value) {
    if (builder_) {
        hdfsBuilderConfSetStr(builder_, key.c_str(), value.c_str());
    } else {
        std::cerr << "Error: Builder not initialized" << std::endl;
    }
    return *this;
}

HdfsBuilder& HdfsBuilder::applyConfigs(const ConfigLoader& configLoader) {
    if (builder_) {
        const auto& configs = configLoader.getAllConfigs();
        for (const auto& config : configs) {
            std::cout << "Applying configuration: " << config.first << " = " << config.second << std::endl;
            hdfsBuilderConfSetStr(builder_, config.first.c_str(), config.second.c_str());
        }
    } else {
        std::cerr << "Error: Builder not initialized" << std::endl;
    }
    return *this;
}

HdfsBuilder& HdfsBuilder::setPrincipal(const std::string& principal) {
    if (builder_) {
        std::cout << "Setting Kerberos principal: " << principal << std::endl;
        hdfsBuilderSetPrincipal(builder_, principal.c_str());
    } else {
        std::cerr << "Error: Builder not initialized" << std::endl;
    }
    return *this;
}

HdfsBuilder& HdfsBuilder::setKrb5Conf(const std::string& krb5Conf) {
    if (builder_) {
        std::cout << "Setting Kerberos krb5.conf file: " << krb5Conf << std::endl;
        hdfsBuilderSetKerb5Conf(builder_, krb5Conf.c_str());
    } else {
        std::cerr << "Error: Builder not initialized" << std::endl;
    }
    return *this;
}

HdfsBuilder& HdfsBuilder::setKeyTabFile(const std::string& keytabFile) {
    if (builder_) {
        std::cout << "Setting Kerberos keytab file: " << keytabFile << std::endl;
        hdfsBuilderSetKeyTabFile(builder_, keytabFile.c_str());
    } else {
        std::cerr << "Error: Builder not initialized" << std::endl;
    }
    return *this;
}

hdfsFS HdfsBuilder::connect() {
    if (!builder_) {
        std::cerr << "Error: Builder not initialized" << std::endl;
        return nullptr;
    }
    
    // If set to use default configuration
    if (useDefault_) {
        std::cout << "Using default HDFS configuration" << std::endl;
    }

    // Try to connect to HDFS
    hdfsFS fs = hdfsBuilderConnect(builder_);
    
    // Important: After hdfsBuilderConnect is called, the builder is automatically released
    // Set builder_ to nullptr to avoid double free in destructor
    builder_ = nullptr;
    
    if (!fs) {
        std::cerr << "Failed to connect to HDFS" << std::endl;
    } else {
        std::cout << "Successfully connected to HDFS" << std::endl;
    }
    
    return fs;
} 