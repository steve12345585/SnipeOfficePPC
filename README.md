# SnipeOffice 25.8 PPC Edition

**SnipeOffice 25.8 PPC Edition** is a fully functional office suite for legacy PowerPC Macintosh systems, based on LibreOffice. This release successfully builds and runs on PowerPC Macs and Intel Macs (via Rosetta) running Mac OS X 10.4 through 10.6.8.

## ✅ Status: Complete and Available

**SnipeOffice 25.8 PPC Edition is now fully compiled and available for download!** The application works natively on PowerPC Macs and via Rosetta on Intel Macs running OS X 10.4-10.6.8.

### Download

Pre-built installers are available in the [Releases](https://github.com/yourusername/SnipeOfficePPC/releases) section. Look for the latest release tagged `v25.8-PPC` or later.

**System Requirements:**
- Mac OS X 10.4 through 10.6.8
- 750MB hard drive space (200MB download, 500MB installation)
- 64MB RAM minimum
- PowerPC Macs: Native execution
- Intel Macs: Requires Rosetta (included with OS X 10.4-10.6.8)

### Key Features

The core premise of SnipeOffice is better support for multilingual files—**ensuring a BOM (Byte Order Mark) is automatically included on TXT and CSV files** so that spreadsheets and documents containing Chinese, Japanese, Russian, and other non-Latin characters do not corrupt when opened on different systems. This is a unique feature not found in other office suites for PowerPC Macs.

This PPC Edition provides backwards-compatible access to modern office features for PowerPC and Intel Mac architectures. For the standard SnipeOffice 25.8 (for Windows 10/11 and modern Linux), visit [snipeoffice.org](https://snipeoffice.org).

---

## Project Goals

- **PowerPC & 32-bit Support:**  
  Forked from the last LibreOffice version with PowerPC support, targeting Mac OS X 10.5.8 (Leopard) and potentially earlier (10.3, 10.4), as well as classic Linux PPC distributions and Windows 2000/XP.
- **Legacy OS Compatibility:**  
  Designed to build and run on older x86 and x64 systems, including Ubuntu 10.04–16.04, Windows 2000, Windows 2003, and Windows XP. Efforts are being made to support old PPC Linux and Windows as well, but these platforms are currently untested. Development has primarily been on a PowerBook G4.
- **Multilingual File Integrity:**  
  Ensures BOM is included on TXT/CSV files for robust support of multilingual data (e.g., Chinese, Russian), preventing character corruption.
- **Self-Contained Dependencies:**  
  All required build dependencies are included or mirrored on a custom dev server, bypassing modern SSL/TLS issues and dead upstream repositories.
- **Branding and Maintenance:**  
  LibreOffice references have been systematically replaced with SnipeOffice branding to clarify project scope and make future code searches and maintenance easier.
- **Modernized Dictionaries and Filters:**  
  Integrated updated dictionaries and file filters from newer LibreOffice releases, while maintaining backward compatibility.
- **Offline/Archive-First Approach:**  
  All files, folders, and filters no longer available from LibreOffice's official sources have been recovered (e.g., via the Internet Archive) and are included locally or on the dev server.

---

## New/Restored Format Coverage

Following the filter backport, SnipeOffice PPC now supports every import/export LibreOffice gained between 4.0.6 and 25.x—while retaining all legacy filters. Highlights below list *every* newly integrated format in user-facing terms:

### Documents & eBooks
- Added AbiWord document import/export.
- Added Apple Pages (`.pages`) document support.
- Added Broadband eBook (BBeB) publishing.
- Added ClarisWorks/ClarisWrite text document support.
- Added DOS Word (`.doc` pre-Win) compatibility.
- Added EPUB eBook pipelines.
- Added FictionBook 2 (`.fb2`) handling.
- Added Mac Word and MacWrite conversion filters.
- Added Mac Works word-processing support.
- Added Mariner Write document support.
- Added Markdown (`.md`) round-tripping via dedicated filters.
- Added Microsoft Word 2007+ XML/VBA (`.docx`, `.dotx`) handling.
- Added Microsoft Write (`.wri`) import/export.
- Added Palm Text Document (`.pdb`) and PalmDoc flows.
- Added Plucker eBook conversion.
- Added WriteNow document support.
- Added Writer indexing/XML archive export filter.
- Added Writer Global (`writerglobal8`) template compatibility.

### Spreadsheets & Data Pipelines
- Added ADO Rowset XML (`adorowset2ods`) import.
- Added Apple Numbers (`.numbers`) spreadsheet support.
- Added Claris Resolve and ClarisWorks Calc conversions.
- Added Gnumeric (`.gnumeric`) import.
- Added Orcus-powered CSV, JSON, and XML pipelines for Calc (`calc_csv/json/xml_Orcus`).
- Added JPEG/PNG/SVG/WEBP export targets for Calc sheets.
- Added Microsoft Excel 2003 XML (Orcus) handling.
- Added Microsoft Excel 2007+ (binary, XML, VBA, template) support.
- Added Microsoft Multiplan compatibility.
- Added Microsoft Works Calc conversions.
- Added Parquet spreadsheet export.
- Added WPS Lotus and WPS Quattro Pro Calc support.
- Added StarOffice/StarCalc XML templates to keep legacy spreadsheets functional.

### Presentations & Drawings
- Added Apple Keynote (`.key`) presentation support.
- Added ClarisWorks Impress and Draw conversions.
- Added FreeHand document import.
- Added PageMaker publication import.
- Added PowerPoint 3 (`.ppt`) legacy support.
- Added QuarkXPress (`.qxp`) document import.
- Added Microsoft PowerPoint 2007+ XML/VBA (including autoplay/template flows).
- Added StarOffice Draw/Impress XML templates plus writer-global variants.
- Added Zoner/ZMF drawing import.

### Graphics & Media Assets
- Added Animated PNG (APNG) import/export across Draw/Calc/Writer and internal graphic filters.
- Added EMZ/WMZ compressed metafile import/export (draw/internal).
- Added HTML/Web export targets (including Calc/Impress/Writer gallery outputs).
- Added MOV video thumbnail import via internal graphic filters.
- Added PDF import/export paths in the filter graph plus updated PDF UI dialogs.
- Added SVG/SVGZ export/import for Draw/Impress and internal filters.
- Added WebP import/export everywhere (Calc/Draw/Impress/Writer/internal graphic filter).
- Added JPEG/PNG/PBM/PPM/TIFF/WMF/XPM export targets for Draw/Impress/Writer where previously missing.

### Publishing, Reports & Templates
- Added StarBase Report and ReportChart filters.
- Added DocBook template assets plus XML filter UI pages for DocBook export.
- Added storage filter detection + GraphicExportFilter components for new pipelines.
- Added dedicated UI dialogs (PDF options, XML/XSLT filter tabs) so users can configure the new filters.

### Encoding Reliability
- Added automatic UTF-8 BOM prepending for CSV/TXT exports (Calc save, UNO clipboard, and related flows) to keep multilingual data intact across legacy systems.

---

## Installation

### Pre-built Application (Recommended)

**Download the ready-to-use application from the [Releases](https://github.com/yourusername/SnipeOfficePPC/releases) section.** The installer is approximately 200MB and requires 500MB for installation (750MB total).

Simply download, run the installer, and follow the on-screen instructions. No compilation required!

### Building from Source

If you want to build SnipeOffice from source, follow these instructions:

#### For PowerPC Macs (OS X 10.4-10.6.8)

1. **Prebuilt Dependencies:**  
   - A prebuilt dependency folder is provided.  
   - **If building on a PowerPC Mac:** Download the entire project on a modern system, copy it to a USB drive, and follow the instructions in the `PPC Dependencies` folder to set up your environment.  
   - See `INSTALL_MACOSX.txt` for Mac OS X 10.5.8 instructions (may also work on 10.3/10.4, untested).  
   - All dependencies required by LibreOffice's preflight scripts are included, as they are no longer available from official archives or repositories.

#### For Intel Macs (OS X 10.7.5+)

**See [BUILD_INTEL_MAC.md](BUILD_INTEL_MAC.md) for complete Intel Mac build instructions.**

The same codebase and dependencies should work on older Intel Macs—just use the 10.7 SDK instead of 10.5. The build system will automatically detect x86_64 architecture.

2. **Dev Server:**  
   - The build process is configured to fetch any required files from a custom SnipeOffice dev server, ensuring compatibility with legacy SSL/TLS stacks.

3. **Branding and Codebase:**  
   - Most LibreOffice branding has been replaced with SnipeOffice, both for clarity and to aid in codebase maintenance.  
   - When searching for legacy code, you can filter by "LibreOffice" or "SnipeOffice" to see which files have been updated.

4. **Dictionaries and Filters:**  
   - Updated dictionaries from newer LibreOffice builds have been integrated.  
   - File filters have been patched to support additional formats, while maintaining compatibility with older document types.

---

## Current Status

✅ **Build Complete and Successful!**

SnipeOffice 25.8 PPC Edition has been successfully built and tested. All build issues have been resolved, and the application is fully functional. The intensive integration work combining LibreOffice 4.0.6 core with filters, dictionaries, help systems, and GUI components from LibreOffice 7.x and 25.x, plus custom modules, has been completed.

**Release Information:**
- **Version:** 25.8
- **Build:** 4062
- **Release Date:** November 20, 2025
- **Status:** Stable release, ready for production use

See [RELEASE_NOTES.md](SnipeOfficePPC-core-25.8-4062-vanbase/RELEASE_NOTES.md) for complete release information, including the full list of supported document formats.

---

## Contributing & Support

- **Bug Reports:** If you encounter any issues with the pre-built application, please open an issue on GitHub with details about your system and the problem you're experiencing.

- **Feature Requests:** Suggestions for improvements are welcome! Please open an issue to discuss.

- **Building from Source:** If you're building from source and encounter issues:
  - Double-check for case sensitivity, hidden characters, or permission issues.
  - Ensure all referenced files exist in the correct directories.
  - If you resolve a build issue, please submit a pull request or open an issue with your solution.

- **Testing:** The application has been tested on PowerPC Macs (native) and Intel Macs (via Rosetta) running OS X 10.4-10.6.8. Testing and feedback on other configurations is welcome!

---

## Credits

- Based on LibreOffice, with thanks to the original developers and the open-source community.
- Additional files and patches sourced from the Internet Archive and other community resources.

---

## License

This project is licensed under the Mozilla Public License v2.0. See the LICENSE file for details. 