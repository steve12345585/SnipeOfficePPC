#!/bin/bash

# SnipeOffice 25.8 - Prerequisites Installation Script for Mac OS X 10.7.5
# This script installs all required dependencies for building SnipeOffice on Intel Mac OS X 10.7.5

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

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to check if a package is already installed
check_installed() {
    local package=$1
    local check_cmd=$2
    
    if command_exists "$check_cmd"; then
        local version=$($check_cmd --version 2>/dev/null | head -n1 || echo "unknown")
        print_info "$package appears to be installed: $version"
        return 0
    else
        return 1
    fi
}

# Function to build and install a package
build_and_install() {
    local package_name=$1
    local archive=$2
    local build_cmd=$3
    local check_cmd=$4
    
    print_info "Processing $package_name..."
    
    # Check if already installed
    if [ -n "$check_cmd" ] && check_installed "$package_name" "$check_cmd"; then
        read -p "  $package_name appears to be installed. Reinstall? (y/N): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            print_info "  Skipping $package_name"
            return 0
        fi
    fi
    
    # Extract archive
    if [ ! -f "$archive" ]; then
        print_error "Archive not found: $archive"
        return 1
    fi
    
    print_info "  Extracting $archive..."
    tar xzf "$archive"
    
    # Get directory name (remove .tar.gz extension)
    local dir_name=$(basename "$archive" .tar.gz)
    # Handle .src.tar.gz case
    if [[ "$dir_name" == *".src" ]]; then
        dir_name=$(basename "$dir_name" .src)
    fi
    
    cd "$dir_name"
    
    # Build and install
    print_info "  Building $package_name..."
    eval "$build_cmd"
    
    cd ..
    
    # Clean up source directory
    print_info "  Cleaning up $dir_name source directory..."
    rm -rf "$dir_name"
    
    print_info "$package_name installation complete!"
    echo
}

# Check for sudo
if [ "$EUID" -ne 0 ] && ! sudo -n true 2>/dev/null; then
    print_warn "This script requires sudo privileges. You may be prompted for your password."
fi

# Check for Xcode/Command Line Tools
print_info "Checking for Xcode/Command Line Tools..."
if ! command_exists xcode-select || ! xcode-select -p >/dev/null 2>&1; then
    print_error "Xcode or Command Line Tools not found!"
    print_error "Please install Xcode from the Mac App Store or install Command Line Tools:"
    print_error "  xcode-select --install"
    exit 1
fi

if ! command_exists gcc && ! command_exists clang; then
    print_error "No C compiler found (gcc or clang)!"
    print_error "Please install Xcode Command Line Tools"
    exit 1
fi

print_info "Compiler found: $(which gcc 2>/dev/null || which clang 2>/dev/null)"
echo

# Find dependencies directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
DEPS_DIR=""

# Check common locations
if [ -d "$SCRIPT_DIR/PPC Dependencies" ]; then
    DEPS_DIR="$SCRIPT_DIR/PPC Dependencies"
elif [ -d "$SCRIPT_DIR/../PPC Dependencies" ]; then
    DEPS_DIR="$SCRIPT_DIR/../PPC Dependencies"
elif [ -d "./PPC Dependencies" ]; then
    DEPS_DIR="./PPC Dependencies"
else
    print_error "Could not find 'PPC Dependencies' directory!"
    print_error "Please run this script from the SnipeOfficePPC directory or specify the path:"
    print_error "  DEPS_DIR=/path/to/PPC\\ Dependencies $0"
    exit 1
fi

print_info "Using dependencies directory: $DEPS_DIR"
cd "$DEPS_DIR"

# Create /usr/local if it doesn't exist
if [ ! -d "/usr/local" ]; then
    print_info "Creating /usr/local directory..."
    sudo mkdir -p /usr/local
    sudo chown $(whoami):admin /usr/local
fi

# Create /usr/local/bin if it doesn't exist
if [ ! -d "/usr/local/bin" ]; then
    print_info "Creating /usr/local/bin directory..."
    sudo mkdir -p /usr/local/bin
    sudo chown $(whoami):admin /usr/local/bin
fi

# Update PATH for current session
export PATH="/usr/local/bin:$PATH"

# 1. pkg-config
build_and_install "pkg-config" \
    "pkg-config-0.25.tar.gz" \
    "./configure && make && sudo make install" \
    "pkg-config"

# 2. gettext
build_and_install "gettext" \
    "gettext-0.17.tar.gz" \
    "./configure && make && sudo make install" \
    "gettext"

# 3. glib
build_and_install "glib" \
    "glib-2.16.6.tar.gz" \
    "./configure && make && sudo make install" \
    "glib-config"

# 4. libIDL
build_and_install "libIDL" \
    "libIDL-0.8.14.tar.gz" \
    "./configure && make && sudo make install" \
    ""

# 5. zip
print_info "Processing zip..."
if check_installed "zip" "zip"; then
    ZIP_VERSION=$(zip -v 2>&1 | head -n1 | grep -o '[0-9]\+\.[0-9]\+' | head -n1)
    if [ "$(echo "$ZIP_VERSION >= 3.0" | bc 2>/dev/null || echo "0")" = "1" ]; then
        print_info "  zip 3.0+ already installed, skipping"
    else
        print_warn "  Old zip version found, installing zip 3.0..."
        build_and_install "zip" \
            "zip30.tar.gz" \
            "make -f unix/Makefile generic && sudo cp zip /usr/local/bin/" \
            "zip"
    fi
else
    build_and_install "zip" \
        "zip30.tar.gz" \
        "make -f unix/Makefile generic && sudo cp zip /usr/local/bin/" \
        "zip"
fi

# Verify zip installation
print_info "Verifying zip installation..."
if command_exists zip; then
    ZIP_PATH=$(which zip)
    ZIP_VERSION=$(zip -v 2>&1 | head -n1)
    print_info "  zip found at: $ZIP_PATH"
    print_info "  $ZIP_VERSION"
    
    if [ "$ZIP_PATH" != "/usr/local/bin/zip" ]; then
        print_warn "  zip is not in /usr/local/bin, PATH may need updating"
    fi
else
    print_error "zip installation failed!"
    exit 1
fi

# 6. doxygen
build_and_install "doxygen" \
    "doxygen-1.7.6.1.src.tar.gz" \
    "./configure && make && sudo make install" \
    "doxygen"

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
    
    if command_exists "$cmd"; then
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
    print_warn "Note: You may need to restart your terminal or run 'source $SHELL_PROFILE'"
    print_warn "      for PATH changes to take effect in new terminal sessions."
else
    print_error "Some packages failed verification. Please check the errors above."
    exit 1
fi

