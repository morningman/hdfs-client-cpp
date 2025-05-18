#!/bin/bash

# Exit on error
set -e

# Display commands as they are executed
set -x

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DOWNLOAD_URL="https://github.com/apache/doris-thirdparty/releases/download/automation/doris-thirdparty-prebuilt-linux-x86_64.tar.xz"
DOWNLOAD_FILE="doris-thirdparty-prebuilt.tar.xz"
DOWNLOAD_URL_HADOOP_341="https://github.com/apache/doris-thirdparty/releases/download/automation/hadoop-3.4.1-jar-dependency-x86.tar.gz"
DOWNLOAD_FILE_HADOOP_341="hadoop-3.4.1-jar-dependency-x86.tar.gz"

# Check if installed directory already exists
if [ -d "${SCRIPT_DIR}/installed/lib/hadoop_hdfs-3.4.1" ]; then
    echo "The 'installed' directory already exists. No need to download."
    exit 0
fi

echo "The 'installed' directory doesn't exist. Downloading thirdparty dependencies..."

# Create a temporary directory for download
TMP_DIR="${SCRIPT_DIR}/tmp"
mkdir -p "${TMP_DIR}"

# Download the thirdparty package
echo "Downloading from ${DOWNLOAD_URL}..."
curl -L "${DOWNLOAD_URL}" -o "${TMP_DIR}/${DOWNLOAD_FILE}"

# Extract the package
echo "Extracting package..."
tar -xf "${TMP_DIR}/${DOWNLOAD_FILE}" -C "${SCRIPT_DIR}"

# Download the hadoop 3.4.1 jar dependency
echo "Downloading from ${DOWNLOAD_URL_HADOOP_341}..."
curl -L "${DOWNLOAD_URL_HADOOP_341}" -o "${TMP_DIR}/${DOWNLOAD_FILE_HADOOP_341}"

# Extract the package
echo "Extracting package..."
tar -xzf "${TMP_DIR}/${DOWNLOAD_FILE_HADOOP_341}" -C "${SCRIPT_DIR}"
mv "${SCRIPT_DIR}/hadoop_hdfs" "${SCRIPT_DIR}/installed/lib/hadoop_hdfs-3.4.1"

# Clean up
rm -rf "${TMP_DIR}"

echo "Download and extraction completed successfully."

# Verify the installed directory exists
if [ -d "${SCRIPT_DIR}/installed/lib/hadoop_hdfs-3.4.1" ]; then
    echo "Verification: 'installed' directory is now present."
else
    echo "Error: 'installed' directory was not created after extraction."
    exit 1
fi

echo "Thirdparty dependencies are ready." 
