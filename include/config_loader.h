#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include <string>
#include <map>

/**
 * ConfigLoader class for loading and managing client configurations
 * Can read configurations from client.conf and apply them to HDFS Builder
 */
class ConfigLoader {
public:
    /**
     * Constructor
     */
    ConfigLoader();
    
    /**
     * Load configuration file from specified path
     * @param configPath Configuration file path
     * @return Whether loading was successful
     */
    bool loadFromFile(const std::string& configPath);
    
    /**
     * Get configuration value for specified key
     * @param key Configuration key
     * @param defaultValue Default value (if key doesn't exist)
     * @return Configuration value or default value
     */
    std::string getConfigValue(const std::string& key, const std::string& defaultValue = "") const;
    
    /**
     * Check if configuration key exists
     * @param key Configuration key
     * @return Whether key exists
     */
    bool hasConfig(const std::string& key) const;
    
    /**
     * Get all configuration items
     * @return All configuration key-value pairs
     */
    const std::map<std::string, std::string>& getAllConfigs() const;
    
    /**
     * Print all configuration items for debugging
     */
    void printConfigs() const;

private:
    /**
     * Parse configuration line
     * @param line Configuration line text
     * @return Whether parsing was successful
     */
    bool parseLine(const std::string& line);
    
    // Store configuration key-value pairs
    std::map<std::string, std::string> configs_;
};

#endif // CONFIG_LOADER_H 