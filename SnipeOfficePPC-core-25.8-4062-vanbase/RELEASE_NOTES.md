# SnipeOffice 25.8 for MacOS PPC - Release Notes

## Overview

SnipeOffice 25.8 represents a significant achievement in preserving and enhancing office productivity software for PowerPC Macintosh systems. This release is the result of an intensive integration effort that weaves together components from multiple LibreOffice versions and custom development.

**The Goal:** Support all document formats from modern LibreOffice releases while maintaining legacy format compatibility, with proper Unicode handling (BOM support) for CSV files containing Chinese, Japanese, and Russian characters.

**The Reality:** Achieving this required far more than just integrating filters. This release involved carefully extracting and integrating components from across the LibreOffice codebase:

- **Base:** LibreOffice 4.0.6 core
- **Filters:** Integrated filters from LibreOffice 7.x and 25.x
- **Dictionary Systems:** Dictionary components and language support from multiple versions
- **Help Systems:** Help documentation and support infrastructure
- **GUI Components:** User interface elements and dialogs from various LibreOffice versions
- **Custom Modules:** Completely custom modules developed specifically for SnipeOffice

This was an intense integration effort—a complex weave of components that required extensive work to make everything function together. The result is a hybrid architecture that provides the widest range of document format support available in any office suite for classic Mac systems.

## System Requirements

- **Hard Drive Space:** 750MB total (200MB for download, 500MB for installation)
- **RAM:** 64MB minimum
- **Operating System:** Mac OS X 10.4 through 10.6.8
- **Architecture Support:**
  - **PowerPC Macs:** Native execution
  - **Intel Macs:** Runs via Rosetta translation layer

## Key Features

### Comprehensive Document Format Support

SnipeOffice 25.8 includes **all filters from LibreOffice 7.x and 25.x, plus all legacy filters** that LibreOffice removed in later versions, combined with **custom modules** developed specifically for this release. This means you have access to the widest range of document format support available in any office suite for classic Mac systems.

### Enhanced CSV Handling

**Unique Feature:** SnipeOffice's CSV import and export functionality uses BOM (Byte Order Mark) by default. This ensures that when working with CSV files containing Chinese, Japanese, and Russian characters, the text encoding is properly preserved and characters do not become corrupted. This is a feature not found in other office suites available for PowerPC Macs.

## Supported Document Formats

### Text Documents (Writer)

#### Modern Formats
- **OpenDocument Text** (.odt, .ott, .odm)
- **Microsoft Word** (.doc, .dot, .docx, .dotx) - including Word 2007+ XML/VBA
- **Rich Text Format** (.rtf)
- **HTML/XHTML** (.html, .htm, .xhtml)
- **Plain Text** (.txt)
- **Markdown** (.md) - with dedicated round-trip filters
- **EPUB** (.epub) - eBook format
- **XML** - Generic XML support

#### Legacy and Classic Formats
- **AbiWord** (.abw) - Import/export support
- **Apple Pages** (.pages) - Apple's document format
- **ClarisWorks/ClarisWrite** - Classic Mac word processor
- **DOS Word** (.doc) - Pre-Windows Word documents
- **Mac Word** - Classic Macintosh Word format
- **MacWrite** - Original Mac word processor
- **Mac Works** - AppleWorks word processing
- **Mariner Write** - Classic Mac word processor
- **Microsoft Write** (.wri) - Windows Write format
- **Palm Text Document** (.pdb) - Palm OS documents
- **PalmDoc** - Palm eReader format
- **WriteNow** - Classic Mac word processor
- **Writer Global 8** (writerglobal8) - Template compatibility

#### eBook Formats
- **BroadBand eBook (BBeB)** - Sony eBook format
- **FictionBook 2** (.fb2) - Popular eBook format
- **Plucker** - Mobile eBook format

#### Export Formats
- **Writer Indexing Export** - XML archive export
- **Image Exports:** JPEG, PNG, SVG, WebP (including web-optimized variants)

### Spreadsheets (Calc)

#### Modern Formats
- **OpenDocument Spreadsheet** (.ods, .ots)
- **Microsoft Excel** (.xls, .xlsx, .xlt, .xlw) - including Excel 2003 XML and Excel 2007+ VBA/XML
- **CSV** (.csv) - **With BOM support for proper Unicode handling**
- **HTML** (.html, .htm)
- **DIF** (.dif) - Data Interchange Format
- **DBF** (.dbf) - Database format
- **RTF** (.rtf)
- **SYLK** (.slk) - Symbolic Link format
- **Lotus 1-2-3** (.wk1, .wks, .123)
- **XML** - Generic XML support
- **JSON** - Via Orcus-powered pipeline
- **Parquet** - Modern data format

#### Legacy Formats
- **Apple Numbers** (.numbers) - Apple's spreadsheet format
- **Claris Resolve** - Classic Mac spreadsheet
- **ClarisWorks Calc** - Classic Mac spreadsheet
- **Gnumeric** (.gnumeric) - Open source spreadsheet format
- **Mac Works Calc** - AppleWorks spreadsheet
- **Microsoft Multiplan** - Early Microsoft spreadsheet
- **Microsoft Works Calc** (.xlr) - Works spreadsheet format
- **WPS Lotus Calc** - WordPerfect Suite
- **WPS QPro Calc** - Quattro Pro format
- **StarOffice Spreadsheet** (.sdc, .stc, .vor)

#### Data Import/Export
- **ADO Rowset XML** - Database export format
- **Orcus-powered pipelines:** CSV, JSON, XML with enhanced processing
- **Image Exports:** JPEG, PNG, SVG, WebP

### Presentations (Impress)

#### Modern Formats
- **OpenDocument Presentation** (.odp, .otp)
- **Microsoft PowerPoint** (.ppt, .pps, .pot, .pptx) - including PowerPoint 2007+ XML/VBA
- **HTML** (.html, .htm)
- **RTF** (.rtf)

#### Legacy Formats
- **Apple Keynote** (.key) - Apple's presentation format
- **ClarisWorks Impress** - Classic Mac presentations
- **PowerPoint 3** - Early PowerPoint format
- **StarOffice Presentation** (.sdd, .sdp, .sdi, .sda, .vor)

#### Export Formats
- **WebP** - Modern image format export

### Drawings (Draw)

#### Modern Formats
- **OpenDocument Drawing** (.odg, .otg)
- **SVG** (.svg) - Scalable Vector Graphics
- **SVGZ** (.svgz) - Compressed SVG

#### Legacy Formats
- **ClarisWorks Draw** - Classic Mac drawing
- **Freehand Document** - Macromedia Freehand
- **PageMaker Document** - Adobe PageMaker
- **QXP Document** - QuarkXPress
- **StarOffice Drawing** (.sxd, .std, .sda, .sdd, .vor)
- **ZMF Document** - Zoner Draw format

#### Export Formats
- **APNG** (.apng) - Animated PNG
- **EMZ** (.emz) - Compressed Windows Metafile
- **SVGZ** (.svgz) - Compressed SVG
- **WebP** (.webp) - Modern image format
- **WMZ** (.wmz) - Compressed Windows Metafile

### Graphics and Images

#### Import/Export Support
- **APNG** (.apng) - Animated Portable Network Graphic
- **EMZ** (.emz) - Compressed MS Windows Metafile (import/export)
- **MOV** (.mov) - QuickTime Movie (import)
- **PDF** (.pdf) - Portable Document Format (import/export)
- **SVGZ** (.svgz) - Compressed Scalable Vector Graphics (import/export)
- **WebP** (.webp) - Modern web image format (import/export)
- **WMZ** (.wmz) - Compressed MS Windows Metafile (import/export)

### Database and Reports

- **StarBase Report** - Legacy database reports
- **StarBase Report Chart** - Database chart reports
- **MWAW Database** - Macintosh AppleWorks Database

### Legacy Mac Formats (MWAW - Macintosh AppleWorks)

The MWAW library provides support for various classic Mac formats:
- **MWAW Bitmap** - Classic Mac bitmap images
- **MWAW Database** - Classic Mac databases
- **MWAW Drawing** - Classic Mac drawings
- **MWAW Presentation** - Classic Mac presentations
- **MWAW Spreadsheet** - Classic Mac spreadsheets
- **MWAW Text Document** - Classic Mac text documents

### Other Formats

- **DocBook** - Technical documentation format
- **XHTML** - Extended HTML with XML structure
- **Generic XML** - Universal XML document support
- **Generic JSON** - JSON document support
- **Generic Markdown** - Markdown document support

## Technical Details

### Filter Architecture

SnipeOffice 25.8 maintains a comprehensive filter architecture that includes:

1. **Base Core:** LibreOffice 4.0.6 foundation for stability and PowerPC compatibility
2. **LibreOffice 7.x Filters:** Intermediate generation filters providing enhanced format support
3. **LibreOffice 25.x Filters:** Latest generation filters with cutting-edge format support
4. **Legacy Filters:** All filters that were present in LibreOffice 4.0.6 but removed in later versions
5. **Custom Modules:** Completely custom modules developed specifically for SnipeOffice, including enhanced CSV handling with BOM support
6. **Enhanced Processing:** Improved handling through Orcus-powered pipelines for CSV, JSON, and XML

### Compatibility Notes

- **PowerPC Macs:** Full native support with optimized performance
- **Intel Macs:** Full compatibility via Rosetta, allowing Intel Mac users running OS X 10.4-10.6.8 to use the suite
- **Cross-Platform:** Documents created in SnipeOffice are compatible with modern LibreOffice and other OpenDocument-compliant office suites

## Installation

1. Download the SnipeOffice 25.8 installer (approximately 200MB)
2. Ensure you have at least 500MB of free disk space
3. Run the installer and follow the on-screen instructions
4. The complete installation requires approximately 750MB total disk space

## Known Limitations

- Requires Mac OS X 10.4 or later (up to 10.6.8)
- Intel Macs require Rosetta to be installed (standard on OS X 10.4-10.6.8)
- While based on LibreOffice 4.0.6 core, modern filters from 7.x and 25.x provide extensive format support, and custom modules add unique functionality

## Architecture Details

SnipeOffice 25.8 is built on a unique hybrid architecture that required extensive integration work across multiple system components. This was far more complex than simply adding filters—it involved carefully extracting and weaving together components from different LibreOffice versions:

### Core Components

- **Core Base:** LibreOffice 4.0.6 provides the stable foundation optimized for PowerPC Mac systems
- **Filter Integration:** Filters from LibreOffice 7.x and 25.x have been carefully backported and integrated, along with all legacy filters that were removed in later versions
- **Dictionary Systems:** Dictionary components and language support systems extracted and integrated from multiple LibreOffice versions
- **Help Systems:** Help documentation infrastructure and support systems integrated from various LibreOffice releases
- **GUI Components:** User interface elements, dialogs, and interaction components pulled from different LibreOffice versions to support the enhanced functionality
- **Custom Development:** Completely custom modules developed specifically for SnipeOffice, including the unique CSV BOM handling feature

### The Integration Challenge

Supporting all formats and implementing proper BOM handling was the goal, but making it happen required extensive work cutting components together from across the LibreOffice codebase. This integration effort touched filters, dictionaries, help systems, and GUI components—creating a complex weave that required careful coordination to ensure everything worked together properly.

This architecture ensures maximum compatibility while providing modern format support and custom enhancements not available in any other office suite for PowerPC Macs.

## Acknowledgments

This release represents an intensive integration effort to preserve and enhance office productivity software for classic Mac systems. The work involved extracting and carefully integrating components from LibreOffice 4.0.6, 7.x, and 25.x—not just filters, but dictionary systems, help systems, GUI components, and custom modules. This complex weave of components required extensive work to make everything function together, and the result is the most comprehensive document format support available for PowerPC Macintosh systems.

---

**Version:** 25.8  
**Build:** 4062  
**Base:** LibreOffice 4.0.6 core with integrated components from LibreOffice 7.x and 25.x (filters, dictionaries, help systems, GUI), plus custom modules  
**Release Date:** November 20, 2025  
**Platform:** MacOS PPC (10.4 - 10.6.8)

