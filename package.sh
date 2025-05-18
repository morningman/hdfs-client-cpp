#!/bin/bash

# Exit on error
set -e

# Display commands being executed
set -x

# Get the script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Always rebuild the project
echo "Rebuilding the project..."
${SCRIPT_DIR}/build.sh

# Create package version from date or tag if available
VERSION=1.0.0

# Create package name
PACKAGE_NAME="hdfs-client-cpp-${VERSION}"
PACKAGE_DIR="${SCRIPT_DIR}/package/${PACKAGE_NAME}"

# Clean previous package if exists
rm -rf "${SCRIPT_DIR}/package"
mkdir -p "${PACKAGE_DIR}"

# Create directory structure
mkdir -p "${PACKAGE_DIR}/conf"
mkdir -p "${PACKAGE_DIR}/thirdparty/installed/lib"

# Copy the executable
echo "Copying hdfs_client executable..."
cp "${SCRIPT_DIR}/build/hdfs_client" "${PACKAGE_DIR}/"

# Copy configuration files
echo "Copying configuration directory..."
cp -r "${SCRIPT_DIR}/conf/"*.xml "${PACKAGE_DIR}/conf/" 2>/dev/null || true
cp -r "${SCRIPT_DIR}/conf/"*.properties "${PACKAGE_DIR}/conf/" 2>/dev/null || true
cp -r "${SCRIPT_DIR}/conf/"client.conf "${PACKAGE_DIR}/conf/" 2>/dev/null || true

# Copy run.sh script
echo "Copying run.sh script..."
cp "${SCRIPT_DIR}/run.sh" "${PACKAGE_DIR}/"

# Copy README file
echo "Copying README file..."
cp "${SCRIPT_DIR}/README.md" "${PACKAGE_DIR}/"

# Copy required third-party libraries
echo "Copying Hadoop HDFS libraries..."
if [ -d "${SCRIPT_DIR}/thirdparty/installed/lib/hadoop_hdfs" ]; then
    cp -r "${SCRIPT_DIR}/thirdparty/installed/lib/hadoop_hdfs"* "${PACKAGE_DIR}/thirdparty/installed/lib/"
else
    echo "Warning: Hadoop HDFS libraries not found. Package may not work without them."
    echo "Please run thirdparty/download.sh first and then rebuild."
fi

# Create tarball
echo "Creating package archive..."
TARBALL="${SCRIPT_DIR}/package/hdfs-client-cpp-${VERSION}.tar.gz"
tar -czf "${TARBALL}" -C "${SCRIPT_DIR}/package" "${PACKAGE_NAME}"

echo "==== Packaging Completed Successfully ===="
echo "Package created at: ${TARBALL}"
echo "To use the package:"
echo "  1. Extract: tar -xzf $(basename ${TARBALL})"
echo "  2. Run: ${PACKAGE_NAME}/run.sh list /" 
