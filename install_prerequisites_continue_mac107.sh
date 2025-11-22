#!/bin/bash

# SnipeOffice 25.8 - Prerequisites Installation Continuation Script
# This script continues from where the main script left off
# Use this if the main script stopped after building but before installing

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Set up logging
LOG_FILE="install_prerequisites_continue_$(date +%Y%m%d_%H%M%S).log"
exec > >(tee -a "$LOG_FILE") 2>&1

echo "=========================================="
echo "SnipeOffice Prerequisites Continuation Log"
echo "Started: $(date)"
echo "Log file: $LOG_FILE"
echo "=========================================="
echo

# Server URL for dependencies
DEPS_SERVER="http://dev-www.snipeoffice.org/mac-lion-depends"

# Find or create dependencies directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
DEPS_DIR=""

if [ -d "$SCRIPT_DIR/Intel_Dependencies" ]; then
    DEPS_DIR="$SCRIPT_DIR/Intel_Dependencies"
elif [ -d "$SCRIPT_DIR/../Intel_Dependencies" ]; then
    DEPS_DIR="$SCRIPT_DIR/../Intel_Dependencies"
elif [ -d "./Intel_Dependencies" ]; then
    DEPS_DIR="./Intel_Dependencies"
else
    # Create dependencies directory if it doesn't exist
    DEPS_DIR="$SCRIPT_DIR/Intel_Dependencies"
    mkdir -p "$DEPS_DIR"
    print_info "Created dependencies directory: $DEPS_DIR"
fi

print_info "Using dependencies directory: $DEPS_DIR"
cd "$DEPS_DIR"

# Function to download file from server if not found locally
download_if_missing() {
    local filename=$1
    if [ ! -f "$filename" ]; then
        print_info "Downloading $filename from server..."
        curl -O "$DEPS_SERVER/$filename" || {
            print_error "Failed to download $filename from $DEPS_SERVER"
            return 1
        }
    else
        print_info "Found local copy: $filename"
    fi
}

# Update PATH for current session
export PATH="/usr/local/bin:$PATH"

# Function to install a built package
install_built_package() {
    local package_name=$1
    local archive=$2
    local dir_name=$(basename "$archive" .tar.gz)
    if [[ "$dir_name" == *".src" ]]; then
        dir_name=$(basename "$dir_name" .src)
    fi
    
    if [ -d "$dir_name" ]; then
        print_info "Found built $package_name directory, installing..."
        cd "$dir_name"
        
        if [ -f "Makefile" ]; then
            print_info "  Running: sudo make install"
            sudo make install
            cd ..
            print_info "$package_name installed successfully!"
            echo
            return 0
        else
            print_warn "  No Makefile found, skipping installation"
            cd ..
            return 1
        fi
    else
        print_warn "$package_name directory not found, may need to build first"
        return 1
    fi
}

# Function to build and install if not already installed
build_and_install_package() {
    local package_name=$1
    local archive=$2
    local check_cmd=$3
    
    print_info "Processing $package_name..."
    
    # Check if already installed
    if [ -n "$check_cmd" ] && command -v "$check_cmd" >/dev/null 2>&1; then
        local version=$($check_cmd --version 2>/dev/null | head -n1 || echo "unknown")
        print_info "  $package_name appears to be installed: $version"
        read -p "  Reinstall? (y/N): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            print_info "  Skipping $package_name"
            return 0
        fi
    fi
    
    # Try to install if already built
    if install_built_package "$package_name" "$archive"; then
        return 0
    fi
    
    # Otherwise build from scratch - download if missing
    if [ ! -f "$archive" ]; then
        print_info "Archive not found locally, downloading from server..."
        download_if_missing "$archive" || {
            print_error "Failed to download $archive"
            return 1
        }
    fi
    
    print_info "  Extracting $archive..."
    tar xzf "$archive"
    
    local dir_name=$(basename "$archive" .tar.gz)
    if [[ "$dir_name" == *".src" ]]; then
        dir_name=$(basename "$dir_name" .src)
    fi
    
    cd "$dir_name"
    
    print_info "  Configuring $package_name..."
    ./configure
    
    print_info "  Building $package_name..."
    make
    
    print_info "  Installing $package_name..."
    sudo make install
    
    cd ..
    
    # Clean up source directory
    print_info "  Cleaning up $dir_name source directory..."
    rm -rf "$dir_name"
    
    print_info "$package_name installation complete!"
    echo
}

# Check for sudo
if [ "$EUID" -ne 0 ] && ! sudo -n true 2>/dev/null; then
    print_warn "This script requires sudo privileges. You will be prompted for your password."
    echo
    print_info "Testing sudo access..."
    if ! sudo -v; then
        print_error "Cannot get sudo access. Please run this script and enter your password when prompted."
        exit 1
    fi
    print_info "Sudo access confirmed!"
    echo
fi

# 1. pkg-config - install if built
print_info "Checking pkg-config..."
download_if_missing "pkg-config-0.25.tar.gz"
if install_built_package "pkg-config" "pkg-config-0.25.tar.gz"; then
    print_info "pkg-config installed"
else
    build_and_install_package "pkg-config" "pkg-config-0.25.tar.gz" "pkg-config"
fi

# 2. gettext - Make sure this is fully installed
print_info "Processing gettext (required for msgfmt)..."
if command -v msgfmt >/dev/null 2>&1; then
    print_info "  msgfmt found, gettext appears installed"
    GETTEXT_VERSION=$(gettext --version 2>/dev/null | head -n1 || echo "installed")
    print_info "  Version: $GETTEXT_VERSION"
else
    print_info "  msgfmt not found, need to install gettext"
    download_if_missing "gettext-0.18.3.2.tar.gz"
    build_and_install_package "gettext" "gettext-0.18.3.2.tar.gz" "gettext"
    
    # Verify msgfmt was installed
    if ! command -v msgfmt >/dev/null 2>&1; then
        print_error "msgfmt still not found after installation!"
        print_error "You may need to reload your PATH: source ~/.bash_profile"
        print_error "Or check if gettext was installed to /usr/local/bin"
        exit 1
    fi
fi
echo

# 3. glib
download_if_missing "glib-2.16.6.tar.gz"
build_and_install_package "glib" "glib-2.16.6.tar.gz" "glib-config"

# 4. libIDL
download_if_missing "libIDL-0.8.14.tar.gz"
build_and_install_package "libIDL" "libIDL-0.8.14.tar.gz" "idl-compiler"

# 5. zip
print_info "Processing zip..."
if command -v zip >/dev/null 2>&1; then
    ZIP_VERSION=$(zip -v 2>&1 | head -n1 | grep -o '[0-9]\+\.[0-9]\+' | head -n1)
    if [ -n "$ZIP_VERSION" ] && [ "$(echo "$ZIP_VERSION >= 3.0" | bc 2>/dev/null || echo "0")" = "1" ]; then
        print_info "  zip 3.0+ already installed, skipping"
    else
        print_warn "  Old zip version found, installing zip 3.0..."
        if [ -d "zip30" ]; then
            cd zip30
            if [ -f "zip" ]; then
                print_info "  Found built zip binary, installing..."
                sudo cp zip /usr/local/bin/
                cd ..
            else
                cd ..
                download_if_missing "zip30.tar.gz"
                build_and_install_package "zip" "zip30.tar.gz" "zip"
            fi
        else
            download_if_missing "zip30.tar.gz"
            build_and_install_package "zip" "zip30.tar.gz" "zip"
        fi
    fi
else
    if [ -d "zip30" ]; then
        cd zip30
        if [ -f "zip" ]; then
            print_info "  Found built zip binary, installing..."
            sudo cp zip /usr/local/bin/
            cd ..
        else
            cd ..
            download_if_missing "zip30.tar.gz"
            build_and_install_package "zip" "zip30.tar.gz" "zip"
        fi
    else
        if [ -d "zip30" ]; then
            cd zip30
            if [ -f "zip" ]; then
                print_info "  Found built zip binary, installing..."
                sudo cp zip /usr/local/bin/
                cd ..
            else
                cd ..
                download_if_missing "zip30.tar.gz"
                build_and_install_package "zip" "zip30.tar.gz" "zip"
            fi
        else
            download_if_missing "zip30.tar.gz"
            build_and_install_package "zip" "zip30.tar.gz" "zip"
        fi
    fi
fi

# Verify zip
print_info "Verifying zip installation..."
if command -v zip >/dev/null 2>&1; then
    ZIP_PATH=$(which zip)
    ZIP_VERSION=$(zip -v 2>&1 | head -n1)
    print_info "  zip found at: $ZIP_PATH"
    print_info "  $ZIP_VERSION"
else
    print_error "zip installation failed!"
    exit 1
fi

# 6. doxygen
download_if_missing "doxygen-1.7.6.1.src.tar.gz"
build_and_install_package "doxygen" "doxygen-1.7.6.1.src.tar.gz" "doxygen"

# Update PATH in shell profile
print_info "Updating PATH in shell profile..."
SHELL_PROFILE=""
if [ -f "$HOME/.bash_profile" ]; then
    SHELL_PROFILE="$HOME/.bash_profile"
elif [ -f "$HOME/.profile" ]; then
    SHELL_PROFILE="$HOME/.profile"
elif [ -f "$HOME/.bashrc" ]; then
    SHELL_PROFILE="$HOME/.bashrc"
else
    SHELL_PROFILE="$HOME/.bash_profile"
    touch "$SHELL_PROFILE"
fi

if ! grep -q '/usr/local/bin' "$SHELL_PROFILE"; then
    print_info "  Adding /usr/local/bin to PATH in $SHELL_PROFILE"
    echo 'export PATH="/usr/local/bin:$PATH"' >> "$SHELL_PROFILE"
else
    print_info "  PATH already configured in $SHELL_PROFILE"
fi

# Verify all installations
echo
print_info "Verifying installations..."
echo

VERIFY_FAILED=0

verify_package() {
    local name=$1
    local cmd=$2
    
    if command -v "$cmd" >/dev/null 2>&1; then
        local version=$($cmd --version 2>/dev/null | head -n1 || $cmd -v 2>/dev/null | head -n1 || echo "installed")
        print_info "  ✓ $name: $version"
        return 0
    else
        print_error "  ✗ $name: NOT FOUND"
        VERIFY_FAILED=1
        return 1
    fi
}

verify_package "pkg-config" "pkg-config"
verify_package "gettext" "gettext"
verify_package "glib" "glib-config"
verify_package "zip" "zip"
verify_package "doxygen" "doxygen"

echo

if [ $VERIFY_FAILED -eq 0 ]; then
    print_info "=========================================="
    print_info "All prerequisites installed successfully!"
    print_info "=========================================="
    echo
    print_info "Next steps:"
    print_info "1. Reload your shell profile: source $SHELL_PROFILE"
    print_info "2. Navigate to the SnipeOffice source directory"
    print_info "3. Run: ./configure --enable-macosx-sdk=10.7 --disable-windows-build --prefix=/usr/local --with-system-zlib"
    print_info "4. Run: make"
    echo
    print_info "=========================================="
    print_info "Installation completed: $(date)"
    print_info "Full log saved to: $LOG_FILE"
    print_info "=========================================="
else
    print_error "Some packages failed verification. Please check the errors above."
    echo
    print_error "=========================================="
    print_error "Installation failed: $(date)"
    print_error "Error log saved to: $LOG_FILE"
    print_error "=========================================="
    exit 1
fi

