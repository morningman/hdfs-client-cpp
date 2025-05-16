#include "config_loader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

ConfigLoader::ConfigLoader() {
    // Initialize configuration mapping
}

bool ConfigLoader::loadFromFile(const std::string& configPath) {
    std::ifstream configFile(configPath);
    if (!configFile.is_open()) {
        std::cerr << "Failed to open config file: " << configPath << std::endl;
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    bool success = true;
    
    while (std::getline(configFile, line)) {
        lineNumber++;
        
        // Skip empty lines and comment lines
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        // Parse configuration line
        if (!parseLine(line)) {
            std::cerr << "Error parsing line " << lineNumber << ": " << line << std::endl;
            success = false;
        }
    }
    
    configFile.close();
    
    std::cout << "Loaded " << configs_.size() << " configuration items from " << configPath << std::endl;
    return success;
}

bool ConfigLoader::parseLine(const std::string& line) {
    std::istringstream iss(line);
    std::string key, value;
    
    // Find equals sign position
    size_t equalsPos = line.find('=');
    if (equalsPos == std::string::npos) {
        return false;  // No equals sign, invalid configuration line
    }
    
    // Extract key and value
    key = line.substr(0, equalsPos);
    value = line.substr(equalsPos + 1);
    
    // Remove leading and trailing whitespace from key and value
    key.erase(0, key.find_first_not_of(" \t"));
    key.erase(key.find_last_not_of(" \t") + 1);
    value.erase(0, value.find_first_not_of(" \t"));
    value.erase(value.find_last_not_of(" \t") + 1);
    
    if (key.empty()) {
        return false;  // Key is empty, invalid configuration line
    }
    
    // Store configuration item
    configs_[key] = value;
    return true;
}

std::string ConfigLoader::getConfigValue(const std::string& key, const std::string& defaultValue) const {
    auto it = configs_.find(key);
    if (it != configs_.end()) {
        return it->second;
    }
    return defaultValue;
}

bool ConfigLoader::hasConfig(const std::string& key) const {
    return configs_.find(key) != configs_.end();
}

const std::map<std::string, std::string>& ConfigLoader::getAllConfigs() const {
    return configs_;
}

void ConfigLoader::printConfigs() const {
    std::cout << "Current configurations:" << std::endl;
    for (const auto& config : configs_) {
        std::cout << "  " << config.first << " = " << config.second << std::endl;
    }
} 