# SnipeOffice 25.8 PPC Edition

**SnipeOffice 25.8 PPC Edition** is a special fork of SnipeOffice 25.8, based on LibreOffice, specifically tailored to build and run on legacy PowerPC (PPC) and 32-bit hardware and older operating systems. The core premise of SnipeOffice is better support for multilingual files—**ensuring a BOM (Byte Order Mark) is automatically included on TXT and CSV files** so that spreadsheets and documents containing Chinese, Russian, and other non-Latin characters do not corrupt when opened on different systems.

This PPC Edition aims to create a backwards-compatible version for PowerPC and 32-bit architectures, providing legacy access to modern office features. For the standard SnipeOffice 25.8 (for Windows 10/11 and modern Linux), visit [snipeoffice.org](https://snipeoffice.org).

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

## Build Instructions

1. **Prebuilt Dependencies:**  
   - A prebuilt dependency folder is provided.  
   - **If building on a PowerPC Mac:** Download the entire project on a modern system, copy it to a USB drive, and follow the instructions in the `PPC Dependencies` folder to set up your environment.  
   - See `INSTALL_MACOSX.txt` for Mac OS X 10.5.8 instructions (may also work on 10.3/10.4, untested).
   - All dependencies required by LibreOffice's preflight scripts are included, as they are no longer available from official archives or repositories.

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

- **Build Progress:**  
  Following the included build instructions, the process currently reaches:
  ```
  (12/17) Building module tail_build
  ```
  but terminates at:
  ```
  No rule to make target '.../writer_globaldocument_StarOffice_XML_Writer_ui.xcu', needed by '.../filter_ui.xcu'.  Stop.
  ```
  (See line 19729 in `build.log` for details.)

- **Known Issues:**  
  - The build fails at the above step due to a makefile or dependency issue with the `writer_globaldocument_StarOffice_XML_Writer_ui.xcu` filter file.
  - All other modules and dependencies appear to build successfully.

---

## Contributing & Troubleshooting

- **If you encounter missing files or build errors:**  
  - Double-check for case sensitivity, hidden characters, or permission issues.
  - Ensure all referenced files exist in the correct directories.
  - If you resolve a build issue, please submit a pull request or open an issue with your solution.

- **Testing on Other Platforms:**  
  - While the project is designed for Mac OS X PPC, it may also work on Windows 2000/XP and older Linux distributions.  
  - Testing and feedback on these platforms is welcome!

---

## Credits

- Based on LibreOffice, with thanks to the original developers and the open-source community.
- Additional files and patches sourced from the Internet Archive and other community resources.

---

## License

This project is licensed under the Mozilla Public License v2.0. See the LICENSE file for details. 