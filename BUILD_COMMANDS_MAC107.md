# SnipeOffice 25.8 Build Commands for Mac OS X 10.7.5

This document provides ready-to-use commands for building SnipeOffice on Intel Mac OS X 10.7.5.

## Prerequisites

Before building, ensure you have installed all prerequisites:

```bash
cd /path/to/SnipeOfficePPC
chmod +x install_prerequisites_mac107.sh
./install_prerequisites_mac107.sh
```

## Initial Setup (First Time Only)

### 1. Navigate to Source Directory

```bash
cd /path/to/SnipeOfficePPC/SnipeOfficePPC-core-25.8-4062-vanbase
```

### 2. Bootstrap the Build System

```bash
# Run bootstrap (if it exists)
./bootstrap

# Or manually run:
aclocal -I m4
autoconf
```

### 3. Configure the Build

```bash
./configure --enable-macosx-sdk=10.7 --disable-windows-build --prefix=/usr/local --with-system-zlib
```

**Note:** This will take several minutes. The configure script will detect your system and set up the build environment.

## Build Commands

### Quick Build (Incremental - Recommended for Development)

Use this when you've already built once and just want to rebuild changed components:

#### Basic Quick Build with Log

```bash
make MD5SUM= V=1 2>&1 | tee build_quick.log
```

#### Quick Build with Parallel Jobs (Faster)

```bash
# For 4 cores
make -j4 MD5SUM= V=1 2>&1 | tee build_quick.log

# For 8 cores
make -j8 MD5SUM= V=1 2>&1 | tee build_quick.log

# Auto-detect cores (use all available)
make -j$(sysctl -n hw.ncpu) MD5SUM= V=1 2>&1 | tee build_quick.log
```

#### Quick Build (Silent - Less Output)

```bash
make MD5SUM= 2>&1 | tee build_quick.log
```

### Full Rebuild (Clean Build)

Use this when you want to rebuild everything from scratch:

#### Option 1: Clean Then Build

```bash
# Clean previous build
make clean

# Full rebuild with logs
make MD5SUM= V=1 2>&1 | tee build_full.log
```

#### Option 2: Clean Build with Parallel Jobs

```bash
# Clean previous build
make clean

# Full rebuild with 4 parallel jobs
make -j4 MD5SUM= V=1 2>&1 | tee build_full.log
```

#### Option 2: Complete Clean Rebuild (Most Thorough)

```bash
# Remove all build artifacts
rm -rf workdir solver instdir

# Reconfigure (optional, only if configure settings changed)
./configure --enable-macosx-sdk=10.7 --disable-windows-build --prefix=/usr/local --with-system-zlib

# Full rebuild
make MD5SUM= V=1 2>&1 | tee build_full_clean.log
```

### Verbose Build (Maximum Detail)

For debugging or seeing everything that happens:

```bash
make MD5SUM= V=1 VERBOSE=1 2>&1 | tee build_verbose.log
```

### Build Specific Module

If you only need to rebuild a specific component:

```bash
# Example: Rebuild only the filter module
cd filter
make MD5SUM= V=1 2>&1 | tee ../build_filter.log
cd ..
```

## Build Time Estimates

Based on system performance:

| System | Quick Build | Full Rebuild |
|--------|-------------|--------------|
| Single Core | 30-60 min | 4-6 hours |
| 2 Cores | 15-30 min | 2-3 hours |
| 4 Cores | 10-20 min | 1-2 hours |
| 8 Cores | 5-10 min | 30-60 min |

## Monitoring Build Progress

### Watch Build Log in Real-Time

In a separate terminal:

```bash
tail -f build_quick.log
```

### Check Build Status

```bash
# See what's currently building
tail -20 build_quick.log

# Count completed modules (if using verbose output)
grep -c "Building module" build_quick.log
```

## Common Build Scenarios

### Scenario 1: First Build Ever

```bash
# 1. Install prerequisites (if not done)
./install_prerequisites_mac107.sh

# 2. Navigate to source
cd SnipeOfficePPC-core-25.8-4062-vanbase

# 3. Bootstrap
./bootstrap || (aclocal -I m4 && autoconf)

# 4. Configure
./configure --enable-macosx-sdk=10.7 --disable-windows-build --prefix=/usr/local --with-system-zlib

# 5. Full build with logging
make -j4 MD5SUM= V=1 2>&1 | tee build_first.log
```

### Scenario 2: Rebuild After Code Changes

```bash
# Quick incremental build
make -j4 MD5SUM= V=1 2>&1 | tee build_incremental.log
```

### Scenario 3: Rebuild After Configuration Change

```bash
# Clean and reconfigure
make clean
./configure --enable-macosx-sdk=10.7 --disable-windows-build --prefix=/usr/local --with-system-zlib

# Full rebuild
make -j4 MD5SUM= V=1 2>&1 | tee build_reconfigure.log
```

### Scenario 4: Complete Fresh Start

```bash
# Remove everything
rm -rf workdir solver instdir

# Reconfigure
./configure --enable-macosx-sdk=10.7 --disable-windows-build --prefix=/usr/local --with-system-zlib

# Full rebuild
make -j4 MD5SUM= V=1 2>&1 | tee build_fresh.log
```

### Scenario 5: Build and Create Installer

```bash
# Build the application
make -j4 MD5SUM= V=1 2>&1 | tee build.log

# Create DMG installer
make installer_dmg 2>&1 | tee build_installer.log
```

## Build Output Locations

After a successful build:

- **Binaries:** `instdir/program/`
- **Libraries:** `instdir/program/`
- **Resources:** `instdir/share/`
- **Installer:** `instsetoo_native/util/LibreOffice/dmg/install/en-US/`

## Troubleshooting Build Issues

### Build Fails - Check Logs

```bash
# View last 50 lines of log
tail -50 build_quick.log

# Search for errors
grep -i error build_quick.log

# Search for warnings
grep -i warning build_quick.log
```

### Build Hangs - Check Process

```bash
# See if make is still running
ps aux | grep make

# Check system resources
top
```

### Out of Memory

```bash
# Use fewer parallel jobs
make -j2 MD5SUM= V=1 2>&1 | tee build.log

# Or single-threaded
make MD5SUM= V=1 2>&1 | tee build.log
```

### Clean and Retry

```bash
# Clean specific module
cd problematic_module
make clean
cd ..

# Or clean everything
make clean
make -j4 MD5SUM= V=1 2>&1 | tee build_retry.log
```

## Verification After Build

### Check Binary Architecture

```bash
file instdir/program/soffice
# Should show: Mach-O 64-bit executable x86_64
```

### Check Minimum OS Version

```bash
otool -l instdir/program/soffice | grep -A 3 LC_VERSION_MIN_MACOSX
```

### Test Run

```bash
# Run SnipeOffice
./instdir/program/soffice
```

## Quick Reference Card

```bash
# QUICK BUILD (Incremental)
make -j4 MD5SUM= V=1 2>&1 | tee build_quick.log

# FULL REBUILD (Clean)
make clean && make -j4 MD5SUM= V=1 2>&1 | tee build_full.log

# COMPLETE CLEAN REBUILD
rm -rf workdir solver instdir
./configure --enable-macosx-sdk=10.7 --disable-windows-build --prefix=/usr/local --with-system-zlib
make -j4 MD5SUM= V=1 2>&1 | tee build_fresh.log

# BUILD + INSTALLER
make -j4 MD5SUM= V=1 2>&1 | tee build.log
make installer_dmg 2>&1 | tee build_installer.log

# MONITOR PROGRESS
tail -f build_quick.log
```

## Environment Variables

You can set these before building to customize the process:

```bash
# Use specific number of jobs
export MAKEFLAGS="-j4"

# Verbose output
export V=1

# Skip MD5 checks (faster, but less safe)
export MD5SUM=

# Set compiler explicitly
export CC=clang
export CXX=clang++
```

## Notes

- **MD5SUM=**: Disables MD5 checksum verification during build (faster, but use with caution)
- **V=1**: Enables verbose output showing all commands
- **-jN**: Number of parallel build jobs (adjust based on CPU cores)
- **tee**: Saves output to file while also displaying on screen
- **2>&1**: Redirects stderr to stdout so errors are captured in log

## Build Log File Naming Convention

Suggested naming for different build types:

- `build_first.log` - First time build
- `build_quick.log` - Quick incremental build
- `build_full.log` - Full clean rebuild
- `build_fresh.log` - Complete fresh start
- `build_installer.log` - Installer creation
- `build_verbose.log` - Maximum verbosity
- `build_retry.log` - Retry after failure

---

**Tip:** Always keep build logs! They're invaluable for debugging issues and tracking build history.

