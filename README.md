# HDFS Client

A C++ client for interacting with Hadoop HDFS using libhdfs.

## Prerequisites

- CMake (version 3.10 or higher)
- C++ Compiler with C++14 support
- Java JDK (for JNI)

## Building

### Using the build script

The simplest way to build the project is to use the provided build script:

```bash
./build.sh
```

To clean the build:

```bash
./clean.sh
```

### Manual build

Alternatively, you can build manually:

```bash
mkdir -p build
cd build
cmake ..
make
```

## Usage

### Using the run script

The easiest way to run the client is using the provided run script:

```bash
# List files in a directory
./run.sh list /path/to/directory

# Read a file
./run.sh read /path/to/file

# Write content to a file
./run.sh write /path/to/file "content to write"

# Delete a file
./run.sh delete /path/to/file

# Show version
./run.sh version

# Show help
./run.sh help
```

### Specifying HDFS Cluster

You can specify the HDFS cluster using the `--fs` option:

```bash
# Connect to a specific HDFS cluster
./run.sh --fs=hdfs://namenode:8020 list /

# Connect to HDFS and list a directory
./run.sh --fs=hdfs://namenode:9000 list /user/hadoop
```

### Other Configuration Options

```bash
# Set custom Hadoop home directory
./run.sh --hadoop-home=/path/to/hadoop list /

# Set custom config directory
./run.sh --hadoop-conf=/path/to/conf list /

# Set custom classpath
./run.sh --classpath=/path/to/jars/* list /
```

### CLASSPATH Configuration

The run script has several options to help set up the CLASSPATH required by libhdfs:

```bash
# Use a specific Hadoop installation
./run.sh --hadoop-home=/path/to/hadoop list /path/to/directory

# Directly specify the CLASSPATH
./run.sh --classpath=/path/to/hadoop/jars/* read /path/to/file

# Show help
./run.sh --help
```

You can also set the CLASSPATH manually before running:

```bash
export HADOOP_HOME=/path/to/hadoop
export CLASSPATH=$(hadoop classpath --glob)
./run.sh list /path/to/directory
```

### Manual execution

You can also run the executable directly:

```bash
# Make sure CLASSPATH is set
export CLASSPATH=$(hadoop classpath --glob)

# Set the HDFS cluster (optional)
export HDFS_DEFAULT_FS=hdfs://namenode:8020

# Run the client
./build/hdfs_client list /path/to/directory
./build/hdfs_client read /path/to/file
./build/hdfs_client write /path/to/file "content to write"
./build/hdfs_client delete /path/to/file
```

## Configuration

By default, the client connects to the default HDFS configuration. You may need to set the following environment variables:

```bash
export CLASSPATH=$(hadoop classpath --glob)
```

## Dependencies

- libhdfs (statically linked)
- JNI (Java Native Interface)
- pthreads (POSIX Threads) 

## Examples

`sh run.sh --fs=hdfs://hdfs-cluster list hdfs://hdfs-cluster/tmp/`

