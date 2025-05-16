#!/bin/bash

# Function to show detailed usage
show_usage() {
    echo "Usage: $0 [options] <command> [arguments]"
    echo ""
    echo "Options:"
    echo "  --hadoop-home=PATH    - Set HADOOP_HOME to PATH"
    echo "  --hadoop-conf=PATH    - Set HADOOP_CONF_DIR to PATH"
    echo "  --classpath=PATH      - Set CLASSPATH to PATH"
    echo "  --fs=URI              - Set fs.defaultFS to URI (e.g. hdfs://namenode:8020)"
    echo "  --help                - Show this help message"
    echo ""
    echo "Commands:"
    echo "  list <path>            - List files in directory"
    echo "  read <path>            - Read file content"
    echo "  write <path> <content> - Write content to file"
    echo "  delete <path>          - Delete file"
    echo ""
    echo "Examples:"
    echo "  $0 --hadoop-home=/path/to/hadoop list /user/hadoop"
    echo "  $0 --fs=hdfs://namenode:8020 list /user/hadoop"
    echo "  $0 --hadoop-conf=/path/to/conf list /user/hadoop"
    echo "  $0 --classpath=/path/to/hadoop/libs/* read /user/hadoop/file.txt"
    echo ""
    exit 1
}

# Set script directory and project root
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Set default HADOOP_CONF_DIR if not already set
if [ -z "$HADOOP_CONF_DIR" ]; then
    # Default to project's conf directory
    export HADOOP_CONF_DIR="${PROJECT_ROOT}/conf"
    echo "Setting default HADOOP_CONF_DIR: $HADOOP_CONF_DIR"
fi

# Set Java options for logging
if [ -f "$HADOOP_CONF_DIR/log4j.properties" ]; then
    export JAVA_OPTS="$JAVA_OPTS -Dlog4j.configuration=file://$HADOOP_CONF_DIR/log4j.properties"
    echo "Using log4j configuration: $HADOOP_CONF_DIR/log4j.properties"
fi

# Ensure conf directory exists
if [ ! -d "$HADOOP_CONF_DIR" ]; then
    echo "Creating HADOOP_CONF_DIR: $HADOOP_CONF_DIR"
    mkdir -p "$HADOOP_CONF_DIR"
fi

# Check if conf directory contains Hadoop configuration files
CORE_SITE="${HADOOP_CONF_DIR}/core-site.xml"
HDFS_SITE="${HADOOP_CONF_DIR}/hdfs-site.xml"

if [ ! -f "$CORE_SITE" ]; then
    echo "Warning: $CORE_SITE not found. HDFS connections may fail."
    echo "Consider adding core-site.xml to $HADOOP_CONF_DIR"
fi

if [ ! -f "$HDFS_SITE" ]; then
    echo "Warning: $HDFS_SITE not found. Some HDFS features may not work."
    echo "Consider adding hdfs-site.xml to $HADOOP_CONF_DIR"
fi

# Set default CLASSPATH with project-specific JAR directories
HADOOP_JAR_DIRS=(
    "${PROJECT_ROOT}/thirdparty/installed/lib/hadoop_hdfs/common"
    "${PROJECT_ROOT}/thirdparty/installed/lib/hadoop_hdfs/common/lib"
    "${PROJECT_ROOT}/thirdparty/installed/lib/hadoop_hdfs/hdfs"
    "${PROJECT_ROOT}/thirdparty/installed/lib/hadoop_hdfs/hdfs/lib"
)

# Initialize CLASSPATH with project JAR files if not already set
if [ -z "$CLASSPATH" ]; then
    # Add HADOOP_CONF_DIR to CLASSPATH first
    CLASSPATH="$HADOOP_CONF_DIR"
    
    # Build CLASSPATH from project JAR directories
    for dir in "${HADOOP_JAR_DIRS[@]}"; do
        if [ -d "$dir" ]; then
            echo "Adding JARs from $dir to CLASSPATH"
            # Add all jar files in the directory
            CLASSPATH="${CLASSPATH}:${dir}/*"
        else
            echo "Warning: Directory not found: $dir"
        fi
    done
    
    export CLASSPATH
fi

# Initialize default FileSystem URI
FS_DEFAULT_FS="hdfs://hdfs-cluster"

# Process options
ARGS=()
while [ $# -gt 0 ]; do
    case "$1" in
        --hadoop-home=*)
            HADOOP_HOME="${1#*=}"
            export HADOOP_HOME
            shift
            ;;
        --hadoop-conf=*)
            # Override the HADOOP_CONF_DIR
            HADOOP_CONF_DIR="${1#*=}"
            export HADOOP_CONF_DIR
            
            # Update CLASSPATH to include new HADOOP_CONF_DIR
            if [ -n "$CLASSPATH" ]; then
                # Replace current conf dir with new one at the beginning of CLASSPATH
                CLASSPATH=$(echo $CLASSPATH | sed -E "s|^[^:]+|$HADOOP_CONF_DIR|")
            else
                # Set CLASSPATH to HADOOP_CONF_DIR if it's empty
                CLASSPATH="$HADOOP_CONF_DIR"
            fi
            export CLASSPATH
            
            echo "Set HADOOP_CONF_DIR to: $HADOOP_CONF_DIR"
            echo "Updated CLASSPATH: $CLASSPATH"
            shift
            ;;
        --classpath=*)
            # Append to existing CLASSPATH instead of replacing
            if [ -z "$CLASSPATH" ]; then
                export CLASSPATH="${1#*=}"
            else
                export CLASSPATH="$CLASSPATH:${1#*=}"
            fi
            shift
            ;;
        --fs=*)
            # Set the default filesystem URI
            FS_DEFAULT_FS="${1#*=}"
            echo "Setting fs.defaultFS to: $FS_DEFAULT_FS"
            # Add to Java system properties
            export JAVA_OPTS="$JAVA_OPTS -Dfs.defaultFS=$FS_DEFAULT_FS"
            # Set environment variable for the client
            export HDFS_DEFAULT_FS="$FS_DEFAULT_FS"
            shift
            ;;
        --help)
            show_usage
            ;;
        *)
            ARGS+=("$1")
            shift
            ;;
    esac
done

# Restore the processed arguments
set -- "${ARGS[@]}"

# Try various methods to set CLASSPATH if still empty
if [ -z "$CLASSPATH" ]; then
    # Method 1: Use hadoop command if available
    if command -v hadoop > /dev/null 2>&1; then
        echo "Setting CLASSPATH using hadoop command..."
        export CLASSPATH=$(hadoop classpath --glob)
    # Method 2: Use HADOOP_HOME if set
    elif [ -n "$HADOOP_HOME" ]; then
        echo "Setting CLASSPATH using HADOOP_HOME: $HADOOP_HOME"
        if [ -f "$HADOOP_HOME/bin/hadoop" ]; then
            export CLASSPATH=$($HADOOP_HOME/bin/hadoop classpath --glob)
        else
            # Construct a basic CLASSPATH manually
            export CLASSPATH="$HADOOP_CONF_DIR:$HADOOP_HOME/share/hadoop/common/*:$HADOOP_HOME/share/hadoop/common/lib/*:$HADOOP_HOME/share/hadoop/hdfs/*:$HADOOP_HOME/share/hadoop/hdfs/lib/*"
            echo "Constructed CLASSPATH manually using HADOOP_HOME"
        fi
    else
        echo "WARNING: Could not find or set CLASSPATH."
        echo "The project JAR directories were not found. Please check if they exist."
        echo "You may need to set CLASSPATH manually for the client to work:"
        echo "  - Use option: --hadoop-home=/path/to/hadoop"
        echo "  - Use option: --classpath=/path/to/jars/*"
        echo "  - Or set environment: export CLASSPATH=\$(hadoop classpath --glob)"
        echo ""
        echo "Attempting to continue without CLASSPATH set..."
    fi
fi

# Show configuration paths
echo "Using HADOOP_CONF_DIR: $HADOOP_CONF_DIR"
echo "Using CLASSPATH: $CLASSPATH"
echo "Using fs.defaultFS: $FS_DEFAULT_FS"
echo "Java options: $JAVA_OPTS"

# Define the executable path
HDFS_CLIENT="./hdfs_client"

# Check if the client is built
if [ ! -f "$HDFS_CLIENT" ]; then
    echo "Error: $HDFS_CLIENT not found. Please build the project first."
    echo "Run: ./build.sh"
    exit 1
fi

# Display help if no arguments provided
if [ $# -eq 0 ]; then
    show_usage
fi

# Execute the client with all provided arguments
echo "Running: $HDFS_CLIENT $@"
env JAVA_OPTS="$JAVA_OPTS" $HDFS_CLIENT "$@" 
