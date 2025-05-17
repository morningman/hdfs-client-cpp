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

## Usage

### Configuration

Put `core-site.xml` and `hdfs-site.xml` in `conf/` (Optional).

Can add more config in `conf/client.conf`, it will overwrite config in xml file.

### Using the run script

The easiest way to run the client is using the provided run script:

```bash
# List files in a directory
./run.sh --fs=hdfs://hdfs-cluster list /path/to/directory

# Read a file
./run.sh --fs=hdfs://hdfs-cluster read /path/to/file

# Write content to a file
./run.sh --fs=hdfs://hdfs-cluster write /path/to/file "content to write"

# Delete a file
./run.sh --fs=hdfs://hdfs-cluster delete /path/to/file

# Show version
./run.sh version

# Show help
./run.sh help
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

## Dependencies

- libhdfs (statically linked)
- JNI (Java Native Interface)
- pthreads (POSIX Threads) 

## Examples

`sh run.sh --fs=hdfs://hdfs-cluster list hdfs://hdfs-cluster/tmp/`

