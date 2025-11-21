# Building SnipeOffice 25.8 on Intel Mac OS X 10.7.5

This guide covers building SnipeOffice 25.8 on Intel Macs running OS X 10.7.5 (Lion). The same codebase and dependencies that work on PowerPC Macs can be built for Intel Macs with minor configuration adjustments.

## Prerequisites

- **Mac OS X 10.7.5** (Lion) on an Intel Mac
- **Xcode** - Install from the Mac App Store or from your OS X installation media
- **Command Line Tools** - Install via Xcode → Preferences → Downloads, or download separately from Apple Developer
- **Disk Space:** At least 10GB free (for build artifacts and dependencies)
- **RAM:** 4GB+ recommended for faster builds

## Step 1: Install Xcode and Command Line Tools

1. Install Xcode from the Mac App Store (or from your OS X installation media)
2. Open Xcode and accept the license agreement
3. Install Command Line Tools:
   - Xcode → Preferences → Downloads → Command Line Tools
   - Or download from: https://developer.apple.com/downloads

## Step 2: Install Source Dependencies

The project includes all necessary source dependencies in the `PPC Dependencies` folder. These are architecture-agnostic and will compile for Intel Macs.

### Option A: Automated Installation Script (Easiest)

**Quick Start:** Use the provided installation script:

```bash
cd /path/to/SnipeOfficePPC
chmod +x install_prerequisites_mac107.sh
./install_prerequisites_mac107.sh
```

The script will:
- Check for Xcode/Command Line Tools
- Automatically build and install all dependencies
- Set up PATH correctly
- Verify all installations

### Option B: Manual Build Dependencies from Source

If you prefer to build manually, open Terminal and navigate to the dependencies folder:

```bash
cd /path/to/SnipeOfficePPC/PPC\ Dependencies
```

Build and install each dependency:

#### 1. pkg-config
```bash
tar xzf pkg-config-0.25.tar.gz
cd pkg-config-0.25
./configure
make
sudo make install
cd ..
```

#### 2. gettext
```bash
tar xzf gettext-0.17.tar.gz
cd gettext-0.17
./configure
make
sudo make install
cd ..
```

#### 3. glib
```bash
tar xzf glib-2.16.6.tar.gz
cd glib-2.16.6
./configure
make
sudo make install
cd ..
```

#### 4. libIDL
```bash
tar xzf libIDL-0.8.14.tar.gz
cd libIDL-0.8.14
./configure
make
sudo make install
cd ..
```

#### 5. zip
```bash
tar xzf zip30.tar.gz
cd zip30
make -f unix/Makefile generic
sudo cp zip /usr/local/bin/
cd ..
```

#### 6. Set zip in PATH
```bash
export PATH="/usr/local/bin:$PATH"
echo 'export PATH="/usr/local/bin:$PATH"' >> ~/.bash_profile
source ~/.bash_profile
```

Verify:
```bash
which zip
zip -v
```

#### 7. doxygen
```bash
tar xzf doxygen-1.7.6.1.src.tar.gz
cd doxygen-1.7.6.1
./configure
make
sudo make install
cd ..
```

### Option B: Use Homebrew (Alternative)

If you prefer using Homebrew on OS X 10.7.5:

```bash
# Install Homebrew (if not already installed)
ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

# Install dependencies
brew install pkg-config gettext glib libidl zip doxygen
```

**Note:** Homebrew on OS X 10.7.5 may require some adjustments, and versions may differ from the source packages. The source build method (Option A) is recommended for consistency.

## Step 3: Prepare the Build Environment

Navigate to the SnipeOffice source directory:

```bash
cd /path/to/SnipeOfficePPC/SnipeOfficePPC-core-25.8-2
```

### Set Up Environment Variables

Add these to your `~/.bash_profile`:

```bash
export PATH="/usr/local/bin:$PATH"
export CC=clang
export CXX=clang++
```

Reload your profile:
```bash
source ~/.bash_profile
```

## Step 4: Bootstrap the Build System (Optional)

**Good News:** The `configure` script already exists in the codebase, so you can skip bootstrap and go straight to Step 5 (Configure).

The bootstrap script builds `dmake` (the build tool). If you want to try it:

```bash
./bootstrap
```

**If bootstrap fails** (common issues: path errors, missing aclocal/autoconf), you can skip it entirely and proceed directly to running `./configure` - the configure script is already present and ready to use.

**Note:** On OS X 10.7.5, `aclocal` and `autoconf` may not be installed by default. Since the configure script already exists, you don't need them unless you're modifying the build system configuration.

## Step 5: Configure the Build

**Important:** For Intel Mac OS X 10.7.5, use the 10.7 SDK instead of 10.5:

```bash
./configure --enable-macosx-sdk=10.7 --disable-windows-build --prefix=/usr/local --with-system-zlib
```

The build system will automatically detect:
- **Architecture:** `x86_64` (Intel 64-bit)
- **Platform:** `unxmacxi` (Mac OS X Intel)
- **Compiler:** Clang (or GCC if available)

### Alternative: Let configure auto-detect SDK

If you have multiple SDKs installed, you can let configure find the appropriate one:

```bash
./configure --disable-windows-build --prefix=/usr/local --with-system-zlib
```

### Verify Configuration

After running configure, check the output for:
- `Host CPU type: x86_64`
- `MacOSX SDK: 10.7` (or detected version)
- No errors about missing dependencies

## Step 6: Build the Project

**See [BUILD_COMMANDS_MAC107.md](BUILD_COMMANDS_MAC107.md) for comprehensive build commands with logging options.**

### Quick Reference

**Quick Build (Incremental):**
```bash
make -j4 MD5SUM= V=1 2>&1 | tee build_quick.log
```

**Full Rebuild (Clean):**
```bash
make clean && make -j4 MD5SUM= V=1 2>&1 | tee build_full.log
```

**Complete Fresh Start:**
```bash
rm -rf workdir solver instdir
./configure --enable-macosx-sdk=10.7 --disable-windows-build --prefix=/usr/local --with-system-zlib
make -j4 MD5SUM= V=1 2>&1 | tee build_fresh.log
```

### Build Time

Expect the build to take several hours depending on your system:
- **4-core Intel Mac:** 1-2 hours (full rebuild)
- **8-core Intel Mac:** 30-60 minutes (full rebuild)
- **Single core:** 4-6 hours (full rebuild)
- **Quick builds:** 10-30 minutes (incremental)

## Step 7: Create the Installer (Optional)

Once the build completes successfully, you can create a DMG installer:

```bash
make install
make installer_dmg
```

The installer will be created in:
```
instsetoo_native/util/LibreOffice/dmg/install/en-US/
```

## Step 8: Install (Optional)

To install the built application:

```bash
sudo make install
```

Or use the DMG installer created in Step 7.

## Troubleshooting

### Issue: "SDK not found" or "10.7 SDK missing"

**Solution:** Check available SDKs:
```bash
ls /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/
```

If 10.7 SDK is missing, you may need to:
1. Install Xcode 4.x which includes the 10.7 SDK
2. Or use a compatible SDK version (10.6 or 10.8 if available)
3. Or download the 10.7 SDK separately

### Issue: "Compiler not found" or "clang not found"

**Solution:** 
```bash
# Check if clang is available
which clang
clang --version

# If missing, install Command Line Tools via Xcode
# Or set CC to gcc:
export CC=gcc
export CXX=g++
```

### Issue: "pkg-config not found"

**Solution:**
```bash
# Verify installation
which pkg-config
pkg-config --version

# If missing, ensure /usr/local/bin is in PATH
export PATH="/usr/local/bin:$PATH"
```

### Issue: Build fails with architecture errors

**Solution:** Verify the build is targeting the correct architecture:
```bash
# Check what configure detected
cat config.log | grep -i "host\|target\|arch"

# Force x86_64 if needed
./configure --host=x86_64-apple-darwin11 --enable-macosx-sdk=10.7 ...
```

### Issue: "zip command not found" or wrong zip version

**Solution:**
```bash
# Ensure correct zip is in PATH
which zip
zip -v

# Should show version 3.0 from /usr/local/bin/zip
# If not, rebuild zip from source (see Step 2)
```

### Issue: Out of memory during build

**Solution:** Reduce parallel jobs:
```bash
# Use fewer parallel jobs
make -j2

# Or build single-threaded
make
```

## Architecture-Specific Notes

### Intel Mac (x86_64) vs PowerPC

- **Build identifier:** `unxmacxi` (Intel) vs `unxmacxp` (PowerPC)
- **SDK:** 10.7 (Intel) vs 10.5 (PowerPC)
- **Binary format:** x86_64 Mach-O vs PPC Mach-O
- **Performance:** Native Intel builds will be significantly faster than Rosetta-translated PPC builds

### Compatibility

**See [INTEL_MAC_COMPATIBILITY.md](INTEL_MAC_COMPATIBILITY.md) for detailed compatibility information.**

The built application is optimized for:
- **Legacy Intel Macs** running OS X 10.6.8 through 10.13 (Snow Leopard through High Sierra)
- **Default minimum:** Mac OS X 10.6.8 (Snow Leopard) - can be set lower
- **Recommended range:** OS X 10.7.5 through 10.13 (Lion through High Sierra)
- **Not recommended:** OS X 10.14 (Mojave) - may have compatibility issues
- **Not supported:** macOS 10.15+ (Catalina and later) - use the separate 10.15+ build instead
- **Not supported:** PowerPC Macs (different architecture)

## Differences from PowerPC Build

1. **SDK Version:** Use 10.7 instead of 10.5
2. **Architecture:** Automatically detected as x86_64
3. **Performance:** Much faster build and runtime (native Intel)
4. **Dependencies:** Same source packages, but compiled for Intel
5. **No Rosetta:** Native execution, no translation layer needed

## Verification

After building, verify the architecture:

```bash
# Check the built binaries
file instdir/program/soffice
# Should show: Mach-O 64-bit executable x86_64

# Check linked libraries
otool -L instdir/program/soffice
# Should show x86_64 libraries
```

## Next Steps

Once built successfully:
1. Test the application
2. Create a DMG installer for distribution
3. Consider creating both PPC and Intel builds for maximum compatibility

---

**Note:** This build process is essentially the same as the PowerPC build, but with the SDK version adjusted for Intel Mac OS X 10.7.5. All the integration work (filters, dictionaries, help systems, GUI components, custom modules) is architecture-independent and will work on both platforms.

