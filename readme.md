# Ripple

A fast, lightweight directory synchronization tool written in C.

## Overview

Ripple is a command-line utility that recursively copies directory structures while maintaining file permissions.  
Built for performance and reliability, with support for parallel processing through `sync.sh`.

## Building

Compile the ripple binary:

```bash
gcc -o ripple ripple.c
```

Make sure the binary is in your PATH or note its location for use with `sync.sh`.

## Usage

### Standalone Mode

Copy a single directory to a destination:

```bash
./ripple <source_directory> <destination_directory>
```

**Requirements:**
- Both source and destination must exist and be valid directories
- The destination directory will be created if it doesn't exist (parent directories must exist)

### Batch Mode with sync.sh

The `sync.sh` script allows you to configure multiple directory pairs and run them in parallel.

**Configuration:**

Edit the `DIR_LIST` array in `sync.sh` to define your source/destination pairs:

```bash
declare -a DIR_LIST=(
    "/path/to/source_a"    "/path/to/backup_a"
    "/path/to/source_b"    "/path/to/backup_b"
    "/path/to/source_c"    "/path/to/backup_c"
)
```

**Running:**

```bash
# Run with default single-threaded mode
./sync.sh

# Run with parallel jobs (e.g., 4 concurrent operations)
./sync.sh -c 4
```

**Options:**

| Option | Description                                                |
|--------|------------------------------------------------------------|
| `-c`   | Maximum number of concurrent ripple processes (default: 1) |
