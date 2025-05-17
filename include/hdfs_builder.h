#ifndef HDFS_BUILDER_H
#define HDFS_BUILDER_H

#include <string>
#include <hdfs.h>
#include "config_loader.h"

/**
 * HdfsBuilder class is responsible for building and configuring HDFS connections
 * Encapsulates the libhdfs hdfsBuilder mechanism
 */
class HdfsBuilder {
public:
    /**
     * Constructor - Creates a new hdfsBuilder instance
     */
    HdfsBuilder();
    
    /**
     * Destructor - Ensures all resources are released
     */
    ~HdfsBuilder();
    
    /**
     * Set NameNode hostname
     * @param namenode Hostname or IP address
     * @return Reference to the current builder instance for chained calls
     */
    HdfsBuilder& setNameNode(const std::string& namenode);
    
    /**
     * Set NameNode port
     * @param port Port number
     * @return Reference to the current builder instance for chained calls
     */
    HdfsBuilder& setPort(int port);
    
    /**
     * Set to use default HDFS configuration (read from classpath)
     * @return Reference to the current builder instance for chained calls
     */
    HdfsBuilder& useDefaultConfig();
    
    /**
     * Read configuration directly from filesystem path
     * @param path Configuration file path
     * @return Reference to the current builder instance for chained calls
     */
    HdfsBuilder& configFromFile(const std::string& path);
    
    /**
     * Set a specific configuration option
     * @param key Configuration key
     * @param value Configuration value
     * @return Reference to the current builder instance for chained calls
     */
    HdfsBuilder& setConfig(const std::string& key, const std::string& value);
    
    /**
     * Apply all configurations from ConfigLoader
     * @param configLoader ConfigLoader instance
     * @return Reference to the current builder instance for chained calls
     */
    HdfsBuilder& applyConfigs(const ConfigLoader& configLoader);
    
    /**
     * Set principal for Kerberos authentication
     * @param principal Kerberos principal
     * @return Reference to the current builder instance for chained calls
     */
    HdfsBuilder& setPrincipal(const std::string& principal);

    /**
     * Set krb5.conf file path for Kerberos authentication
     * @param krb5Conf krb5.conf file path
     * @return Reference to the current builder instance for chained calls
     */
    HdfsBuilder& setKrb5Conf(const std::string& krb5Conf);
    
    /**
     * Set keytab file path for Kerberos authentication
     * @param keytabFile keytab file path
     * @return Reference to the current builder instance for chained calls
     */
    HdfsBuilder& setKeyTabFile(const std::string& keytabFile);
    
    /**
     * Connect to HDFS and return file system handle
     * @return HDFS file system handle, or nullptr if connection fails
     */
    hdfsFS connect();

    hdfsBuilder* get() { return builder_; }
    
private:
    // hdfsBuilder instance
    struct hdfsBuilder* builder_;
    // Whether default configuration has been set
    bool useDefault_;
};

#endif // HDFS_BUILDER_H 
