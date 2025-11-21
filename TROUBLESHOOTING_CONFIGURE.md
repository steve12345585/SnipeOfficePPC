# Troubleshooting: configure is a directory

If you get an error that `./configure` is a directory instead of a script, here's how to fix it:

## Problem

The `configure` script should be an executable file, but it appears as a directory. This can happen if:
1. A `configure` directory was accidentally created
2. The configure script wasn't properly generated
3. You're in the wrong directory

## Solution

### Step 1: Check what `configure` actually is

```bash
cd /Users/stephendriver/Desktop/SnipeOfficePPC/SnipeOfficePPC-core-25.8-2
ls -la configure
file configure
```

If it shows as a directory, remove it:

```bash
rm -rf configure
```

### Step 2: Generate the configure script

Since you don't have `aclocal`/`autoconf` installed, you have two options:

#### Option A: Use autogen.sh (requires aclocal/autoconf)

If you can install autotools:

```bash
# Install autotools (if possible on OS X 10.7.5)
# This may require Homebrew or building from source

# Then run:
./autogen.sh --enable-macosx-sdk=10.7 --disable-windows-build --prefix=/usr/local --with-system-zlib
```

#### Option B: Copy configure from the other directory (if it exists)

If the `configure` script exists in `SnipeOfficePPC-core-25.8-4062-vanbase`:

```bash
# Check if configure exists in the other directory
ls -la ../SnipeOfficePPC-core-25.8-4062-vanbase/configure

# If it exists and is a file (not directory), copy it:
cp ../SnipeOfficePPC-core-25.8-4062-vanbase/configure .
chmod +x configure
```

#### Option C: Check if configure script exists elsewhere

```bash
# Search for configure scripts
find . -name "configure" -type f -executable

# If found, check if it's the right one:
head -20 configure
```

### Step 3: Verify configure is a script

After fixing, verify:

```bash
file configure
# Should show: configure: Bourne-Again shell script, ASCII text executable

# Or:
head -5 configure
# Should show: #!/bin/sh or similar
```

### Step 4: Run configure

Once `configure` is a proper script:

```bash
./configure --enable-macosx-sdk=10.7 --disable-windows-build --prefix=/usr/local --with-system-zlib
```

## Alternative: Use the working directory

If `SnipeOfficePPC-core-25.8-4062-vanbase` has a working configure script, you might want to build from that directory instead:

```bash
cd ../SnipeOfficePPC-core-25.8-4062-vanbase
./configure --enable-macosx-sdk=10.7 --disable-windows-build --prefix=/usr/local --with-system-zlib
```

## Installing autotools on OS X 10.7.5

If you need to install `aclocal`/`autoconf`/`automake`:

1. **Try Homebrew** (if available):
   ```bash
   brew install automake autoconf
   ```

2. **Build from source** (if Homebrew not available):
   - Download autoconf and automake source packages
   - Build and install them (similar to how you installed other dependencies)

3. **Check if Xcode includes them**:
   ```bash
   # Some versions of Xcode include autotools
   which autoconf
   which automake
   ```

## Quick Diagnostic Commands

Run these to understand the situation:

```bash
# What is configure?
ls -ld configure
file configure

# Is there a configure script elsewhere?
find . -name "configure" -type f 2>/dev/null

# Check if autogen.sh exists
ls -la autogen.sh

# Check what autogen.sh needs
head -30 autogen.sh | grep -i aclocal
```

