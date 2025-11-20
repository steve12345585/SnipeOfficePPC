# SnipeOffice 25.8 Intel Mac Build - Compatibility Guide

If you build SnipeOffice 25.8 on Intel Mac OS X 10.7.5 (Lion), here's what systems will be able to run it:

## Operating System Compatibility

### Minimum OS Version

**Default:** If built with the 10.7 SDK without specifying a minimum version, the default deployment target is **Mac OS X 10.6** (Snow Leopard).

**You can set it lower if desired:**
```bash
./configure --enable-macosx-sdk=10.7 --with-macosx-version-min-required=10.4
```

This would allow the binary to run on:
- **Mac OS X 10.4** (Tiger) - if set to 10.4
- **Mac OS X 10.5** (Leopard) - if set to 10.5
- **Mac OS X 10.6** (Snow Leopard) - default minimum
- **Mac OS X 10.7** (Lion) - if set to 10.7

**Note:** Setting the minimum to 10.4 or 10.5 may require additional compatibility work, as some APIs used in the codebase may not be available in those versions.

### Maximum OS Version (Upward Compatibility)

**Realistic Compatibility Range:** A binary built on OS X 10.7.5 is designed for legacy systems. While it may technically run on newer versions, compatibility becomes unreliable as macOS evolves.

✅ **Mac OS X 10.7.5** (Lion) - Native build platform  
✅ **Mac OS X 10.8** (Mountain Lion) - Should work  
✅ **Mac OS X 10.9** (Mavericks) - Should work  
✅ **Mac OS X 10.10** (Yosemite) - Should work  
✅ **Mac OS X 10.11** (El Capitan) - Should work  
✅ **Mac OS X 10.12** (Sierra) - Should work  
✅ **Mac OS X 10.13** (High Sierra) - Likely works, but may have issues  
⚠️ **Mac OS X 10.14** (Mojave) - May work, but not recommended  
❌ **Mac OS X 10.15+** (Catalina and later) - **Not supported** - Use the separate 10.15+ build instead

**Important:** This build is optimized for OS X 10.6-10.14. For macOS 10.15 (Catalina) and later, use the separate build compiled on 10.15+ which includes modern APIs and frameworks required by LibreOffice 25.8.

### OS Versions That Cannot Run It

❌ **Mac OS X 10.4 or earlier** - Too old (unless deployment target explicitly set to 10.4 and compatibility verified)  
❌ **Mac OS X 10.5** - May work if deployment target set to 10.5, but untested  
❌ **PowerPC Macs** - Completely different architecture (PowerPC vs Intel)

## CPU/Architecture Compatibility

### Supported CPUs

A build on OS X 10.7.5 will produce an **x86_64 (64-bit Intel)** binary that runs on:

✅ **All Intel Macs with 64-bit processors:**
- Core 2 Duo (2006+)
- Core 2 Quad
- Core i5
- Core i7
- Xeon (Mac Pro)
- Any Intel processor from 2006 onwards

### CPU Architectures That Cannot Run It

❌ **PowerPC processors** (G3, G4, G5) - Different instruction set  
❌ **32-bit only Intel processors** - Very rare, mostly early Intel Macs (2006)  
❌ **Apple Silicon (M1/M2/M3)** - Different architecture; use the 10.15+ build for Apple Silicon compatibility

## Architecture Details

### Default Build Configuration

When building on OS X 10.7.5 with `--enable-macosx-sdk=10.7`:

- **Architecture:** `x86_64` (64-bit Intel)
- **Platform identifier:** `unxmacxi` (Mac OS X Intel)
- **Deployment target:** `10.6` (default) or as specified
- **SDK version:** `10.7`
- **Binary format:** Mach-O 64-bit executable

### Setting Lower Compatibility

To maximize compatibility, you can build with a lower deployment target:

```bash
# Build for 10.6 minimum (default with 10.7 SDK)
./configure --enable-macosx-sdk=10.7

# Build for 10.5 minimum (may require compatibility testing)
./configure --enable-macosx-sdk=10.7 --with-macosx-version-min-required=10.5

# Build for 10.4 minimum (may require significant compatibility work)
./configure --enable-macosx-sdk=10.7 --with-macosx-version-min-required=10.4
```

**Warning:** Setting the minimum to 10.4 or 10.5 may cause build errors or runtime issues if the code uses APIs only available in 10.6+.

## Practical Compatibility Summary

### Recommended Build Configuration

For maximum compatibility while ensuring stability:

```bash
./configure --enable-macosx-sdk=10.7 --with-macosx-version-min-required=10.6
```

**This will produce a binary optimized for:**
- ✅ Mac OS X 10.6.8 (Snow Leopard) - Minimum
- ✅ Mac OS X 10.7.x (Lion) - Native build platform
- ✅ Mac OS X 10.8-10.13 (Mountain Lion through High Sierra) - Recommended range
- ⚠️ Mac OS X 10.14 (Mojave) - May work but not guaranteed
- ❌ Mac OS X 10.15+ (Catalina and later) - **Use separate 10.15+ build**
- ✅ All Intel Macs from 2006 onwards (Core 2 Duo+)

### Compatibility Matrix

| OS Version | Intel Mac (x86_64) | Notes |
|------------|-------------------|-------|
| 10.6.8     | ✅ (if min=10.6)   | Minimum supported |
| 10.7.5     | ✅ Native         | Build platform |
| 10.8-10.13 | ✅ Recommended    | Optimal compatibility range |
| 10.14      | ⚠️ May work       | Not guaranteed, may have issues |
| 10.15+     | ❌ Not supported  | Use separate 10.15+ build |
| macOS 11+  | ❌ Not supported  | Use separate 10.15+ build |

## Important Notes

1. **32-bit vs 64-bit:** OS X 10.7.5 builds will be 64-bit only. 32-bit support was deprecated and removed in macOS 10.15.

2. **Legacy Build Scope:** This build is specifically for legacy Intel Macs running OS X 10.6-10.13. It uses older APIs and frameworks that are compatible with these systems.

3. **Modern macOS (10.15+):** For macOS 10.15 (Catalina) and later, use the separate build compiled on 10.15+. LibreOffice 25.8 requires modern APIs, frameworks, and security models that don't exist in the 10.7 SDK. Attempting to use this legacy build on 10.15+ may result in missing features, runtime errors, or security issues.

4. **Backward Compatibility:** While you *can* set the deployment target to 10.4 or 10.5, the codebase may use APIs that don't exist in those versions. Extensive testing would be required.

5. **Why Not 10.15+?** LibreOffice 25.8's modern components (filters, dictionaries, help systems, GUI) require APIs and frameworks introduced in macOS 10.15+. The codebase has evolved significantly, and trying to span two decades of OS versions is not practical or reliable.

## Verification

After building, verify the architecture and minimum OS version:

```bash
# Check binary architecture
file instdir/program/soffice
# Should show: Mach-O 64-bit executable x86_64

# Check minimum OS version
otool -l instdir/program/soffice | grep -A 3 LC_VERSION_MIN_MACOSX
# Shows the minimum macOS version required

# Check linked libraries
otool -L instdir/program/soffice
# Should show x86_64 libraries
```

---

**Summary:** A build on OS X 10.7.5 produces a 64-bit Intel binary optimized for legacy Intel Macs running **Mac OS X 10.6.8 through 10.13** (Snow Leopard through High Sierra). This is a legacy build for older systems. For macOS 10.15 (Catalina) and later, use the separate build compiled on 10.15+. It will NOT run on PowerPC Macs.

