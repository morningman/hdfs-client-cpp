#!/bin/bash

# Exit on error
set -e

# Display commands being executed
set -x

# Get the script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Download and prepare third-party libraries
echo "==== Preparing Third-party Libraries ===="
if [ -f "${SCRIPT_DIR}/thirdparty/download.sh" ]; then
    echo "Running third-party download script..."
    bash "${SCRIPT_DIR}/thirdparty/download.sh"
else
    echo "Warning: Third-party download script not found at ${SCRIPT_DIR}/thirdparty/download.sh"
fi

# Create build directory if it doesn't exist
mkdir -p build

# Navigate to build directory
cd build

# Configure with CMake
cmake ..

# Build the project
make -j$(nproc)

cp $(pwd)/hdfs_client $(pwd)/../

# Display success message
echo "==== Build Completed Successfully ===="
echo "Executable location: ./hdfs_client"

# Optional: Run tests if they exist
# make test

# Return to the original directory
cd .. 
