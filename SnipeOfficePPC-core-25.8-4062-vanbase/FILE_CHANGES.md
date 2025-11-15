# Edited Files Summary

Comprehensive list of every file that has been modified or added since we began fixing the filter module issue. Paths are relative to the repository root.

1. `filter/Configuration_filter.mk`
   - Added a pattern rule so Make recognizes `.xcu` source files under `filter/source/config/fragments/filters/`.
   - Ensured dependency logic handles the writer global document UI file correctly.
   - **Key filter file:** `filter/source/config/fragments/filters/writer_globaldocument_StarOffice_XML_Writer_ui.xcu`

2. `autogen.sh`
   - Copies `dmake/config.guess` and `dmake/config.sub` into the root (if missing) before running `configure`, preventing download-stage failures.

3. `BUILD_FAILURE_ANALYSIS.md`
   - Living document capturing every build failure, investigation notes, fixes applied, and status updates.

4. `build_images.ps1`
   - New PowerShell utility located in the repo root.
   - Generates `IMAGES.md`, `IMAGES.html`, and `IMAGES_logo.html` inventories/galleries.

5. `IMAGES.md`
   - Markdown inventory of all image assets grouped by directory, including a curated “Logo & Branding Candidates” section.

6. `IMAGES.html`
   - Full gallery view of every image file in the repository, embedded with relative paths underneath.

7. `IMAGES_logo.html`
   - Focused gallery containing only the logo/branding subset highlighted at the top of `IMAGES.md`.

8. `icon-themes/galaxy/brand/`, `icon-themes/galaxy/brand_dev/`, and `icon-themes/hicontrast/brand/shell/` (directories updated)
   - Branding assets (about/intro screens, shell backdrops) refreshed and captured by the inventories/galleries.

9. `android/sdremote/res/drawable/libreoffice_logo.9.png`
   - Updated Android remote control logo asset.

10. `sc/source/ui/docshell/docsh.cxx`
    - CSV “Save As Text” export now prepends a UTF-8 BOM automatically whenever the user chooses UTF-8 encoding.

11. `sc/source/ui/docshell/impex.cxx`
    - Clipboard/UNO CSV exports (via `ScImportExport::Doc2Text`) now prepend a UTF-8 BOM when the stream charset is UTF-8, keeping behavior consistent everywhere.

12. `src/` contents updated (per `src/filter_added_src.txt`)
   - Added/refreshed upstream tarballs under `src/` (e.g. dictionaries, libraries, fonts) and regenerated `fetch.log`. Full file list appended below for traceability.

> Note: `IMAGES_raw.txt` was generated during processing but remains an intermediate artifact only; the primary deliverables are listed above.

### Additional filter assets added
Long-term maintenance requires us to know exactly which filter capabilities have been introduced. Compared with the archived `filter old/filter` snapshot, the active `filter/` tree now includes:
- New filter definitions enabling import/export of formats such as EPUB, Markdown, Parquet, APNG/WEBP, MOV, EMZ/WMZ, Apple iWork (Keynote/Pages/Numbers), ClarisWorks, MWAW documents, Orcus-powered spreadsheet/JSON/XML pipelines, etc.
- Expanded internal graphic filters (WEBP, APNG, SVGZ) and hybrid PDF pipelines.
- Comprehensive QA data and CppUnit suites for regression coverage across PDF, SVG, MSFilter, text filter detection, filter dialogs, etc.
- DocBook template infrastructure (used by the DocBook export feature) plus associated UI dialogs (.ui files).
- Filter storage/graphic components and helper sources (storage filter detection, `GraphicExportFilter.*`).

**Full list of newly added files** (relative to `filter/`):

- `AllLangMoTarget_flt.mk`
- `CppunitTest_filter_dialogs_test.mk`
- `CppunitTest_filter_msfilter.mk`
- `CppunitTest_filter_pdf.mk`
- `CppunitTest_filter_priority.mk`
- `CppunitTest_filter_svg.mk`
- `CppunitTest_filter_textfilterdetect.mk`
- `CppunitTest_filter_xslt.mk`
- `CustomTarget_docbook.mk`
- `inc/strings.hrc`
- `inc/strings.hxx`
- `IwyuFilter_filter.yaml`
- `Library_graphicfilter.mk`
- `Library_storagefd.mk`
- `Package_xhtml.mk`
- `qa/cppunit/data/xslt/copy.xslt`
- `qa/cppunit/msfilter-test.cxx`
- `qa/cppunit/priority-test.cxx`
- `qa/cppunit/xslt-test.cxx`
- `qa/data/ca.pem`
- `qa/data/cert.pem`
- `qa/data/commentsInMargin.odt`
- `qa/data/key.pem`
- `qa/data/picture.pdf`
- `qa/pdf.cxx`
- `qa/unit/data/attributeRedefinedTest.odp`
- `qa/unit/data/calc.ots`
- `qa/unit/data/custom-bullet.fodp`
- `qa/unit/data/empty.doc`
- `qa/unit/data/empty.odp`
- `qa/unit/data/empty.ods`
- `qa/unit/data/empty.odt`
- `qa/unit/data/empty.pptx`
- `qa/unit/data/filter-dialogs-test.txt`
- `qa/unit/data/fit-to-size-text.fodg`
- `qa/unit/data/hybrid_calc.pdf`
- `qa/unit/data/hybrid_impress.pdf`
- `qa/unit/data/hybrid_writer.pdf`
- `qa/unit/data/impress.otp`
- `qa/unit/data/preserve-jpg.odt`
- `qa/unit/data/sample-markdown.md`
- `qa/unit/data/semi-transparent-fill.odg`
- `qa/unit/data/semi-transparent-line.odg`
- `qa/unit/data/semi-transparent-text-bullet.odg`
- `qa/unit/data/shape-nographic.odp`
- `qa/unit/data/tdf114428.xhtml`
- `qa/unit/data/tdf91315.fodp`
- `qa/unit/data/test_pseudo_pwi.xml`
- `qa/unit/data/text-in-image.odp`
- `qa/unit/data/TransparentText.odg`
- `qa/unit/data/writer.ott`
- `qa/unit/data/YAxis.odg`
- `qa/unit/filter-dialogs-test.cxx`
- `qa/unit/svg.cxx`
- `qa/unit/textfilterdetect.cxx`
- `README.md`
- `source/config/fragments/filters/AbiWord.xcu`
- `source/config/fragments/filters/ADO_rowset_XML.xcu`
- `source/config/fragments/filters/APNG___Animated_Portable_Network_Graphic.xcu`
- `source/config/fragments/filters/AppleKeynote.xcu`
- `source/config/fragments/filters/AppleNumbers.xcu`
- `source/config/fragments/filters/ApplePages.xcu`
- `source/config/fragments/filters/BroadBand_eBook.xcu`
- `source/config/fragments/filters/calc_csv_Orcus.xcu`
- `source/config/fragments/filters/calc_Gnumeric.xcu`
- `source/config/fragments/filters/calc_jpg_Export.xcu`
- `source/config/fragments/filters/calc_json_Orcus.xcu`
- `source/config/fragments/filters/calc_MS_Excel_2007_VBA_XML.xcu`
- `source/config/fragments/filters/calc_Parquet.xcu`
- `source/config/fragments/filters/calc_png_Export.xcu`
- `source/config/fragments/filters/calc_svg_Export.xcu`
- `source/config/fragments/filters/calc_webp_Export.xcu`
- `source/config/fragments/filters/calc_xml_Orcus.xcu`
- `source/config/fragments/filters/Claris_Resolve_Calc.xcu`
- `source/config/fragments/filters/ClarisWorks.xcu`
- `source/config/fragments/filters/ClarisWorks_Calc.xcu`
- `source/config/fragments/filters/ClarisWorks_Draw.xcu`
- `source/config/fragments/filters/ClarisWorks_Impress.xcu`
- `source/config/fragments/filters/DosWord.xcu`
- `source/config/fragments/filters/draw_apng_Export.xcu`
- `source/config/fragments/filters/draw_emz_Export.xcu`
- `source/config/fragments/filters/draw_svgz_Export.xcu`
- `source/config/fragments/filters/draw_webp_Export.xcu`
- `source/config/fragments/filters/draw_wmz_Export.xcu`
- `source/config/fragments/filters/EMZ___Compressed_MS_Windows_Metafile.xcu`
- `source/config/fragments/filters/EPUB.xcu`
- `source/config/fragments/filters/FictionBook_2.xcu`
- `source/config/fragments/filters/FreehandDocument.xcu`
- `source/config/fragments/filters/impress_MS_PowerPoint_2007_XML_VBA.xcu`
- `source/config/fragments/filters/impress_webp_Export.xcu`
- `source/config/fragments/filters/Mac_Word.xcu`
- `source/config/fragments/filters/Mac_Works.xcu`
- `source/config/fragments/filters/Mac_Works_Calc.xcu`
- `source/config/fragments/filters/MacWrite.xcu`
- `source/config/fragments/filters/Mariner_Write.xcu`
- `source/config/fragments/filters/Markdown.xcu`
- `source/config/fragments/filters/mov__MOV.xcu`
- `source/config/fragments/filters/MS_Excel_2003_XML_Orcus.xcu`
- `source/config/fragments/filters/MS_Multiplan.xcu`
- `source/config/fragments/filters/MS_Word_2007_XML_VBA.xcu`
- `source/config/fragments/filters/MS_Works_Calc.xcu`
- `source/config/fragments/filters/MS_Write.xcu`
- `source/config/fragments/filters/MWAW_Bitmap.xcu`
- `source/config/fragments/filters/MWAW_Database.xcu`
- `source/config/fragments/filters/MWAW_Drawing.xcu`
- `source/config/fragments/filters/MWAW_Presentation.xcu`
- `source/config/fragments/filters/MWAW_Spreadsheet.xcu`
- `source/config/fragments/filters/MWAW_Text_Document.xcu`
- `source/config/fragments/filters/PageMakerDocument.xcu`
- `source/config/fragments/filters/Palm_Text_Document.xcu`
- `source/config/fragments/filters/PalmDoc.xcu`
- `source/config/fragments/filters/Plucker_eBook.xcu`
- `source/config/fragments/filters/PowerPoint3.xcu`
- `source/config/fragments/filters/QXPDocument.xcu`
- `source/config/fragments/filters/StarBaseReport.xcu`
- `source/config/fragments/filters/StarBaseReportChart.xcu`
- `source/config/fragments/filters/StarOffice_Drawing.xcu`
- `source/config/fragments/filters/StarOffice_Presentation.xcu`
- `source/config/fragments/filters/StarOffice_Spreadsheet.xcu`
- `source/config/fragments/filters/StarOffice_Writer.xcu`
- `source/config/fragments/filters/SVG___Scalable_Vector_Graphics_Draw.xcu`
- `source/config/fragments/filters/SVGZ___Compressed_Scalable_Vector_Graphics.xcu`
- `source/config/fragments/filters/WEBP___WebP.xcu`
- `source/config/fragments/filters/WMZ___Compressed_MS_Windows_Metafile.xcu`
- `source/config/fragments/filters/WPS_Lotus_Calc.xcu`
- `source/config/fragments/filters/WPS_QPro_Calc.xcu`
- `source/config/fragments/filters/WriteNow.xcu`
- `source/config/fragments/filters/writer_indexing_export.xcu`
- `source/config/fragments/filters/writer_jpg_Export.xcu`
- `source/config/fragments/filters/writer_png_Export.xcu`
- `source/config/fragments/filters/writer_svg_Export.xcu`
- `source/config/fragments/filters/writer_web_jpg_Export.xcu`
- `source/config/fragments/filters/writer_web_png_Export.xcu`
- `source/config/fragments/filters/writer_web_webp_Export.xcu`
- `source/config/fragments/filters/writer_webp_Export.xcu`
- `source/config/fragments/filters/writerglobal8_template.xcu`
- `source/config/fragments/filters/ZMFDocument.xcu`
- `source/config/fragments/internalgraphicfilters/apng_Export.xcu`
- `source/config/fragments/internalgraphicfilters/emz_Export.xcu`
- `source/config/fragments/internalgraphicfilters/emz_Import.xcu`
- `source/config/fragments/internalgraphicfilters/mov_Import.xcu`
- `source/config/fragments/internalgraphicfilters/pdf_Export.xcu`
- `source/config/fragments/internalgraphicfilters/pdf_Import.xcu`
- `source/config/fragments/internalgraphicfilters/svgz_Export.xcu`
- `source/config/fragments/internalgraphicfilters/svgz_Import.xcu`
- `source/config/fragments/internalgraphicfilters/webp_Export.xcu`
- `source/config/fragments/internalgraphicfilters/webp_Import.xcu`
- `source/config/fragments/internalgraphicfilters/wmz_Export.xcu`
- `source/config/fragments/internalgraphicfilters/wmz_Import.xcu`
- `source/config/fragments/types/apng_Animated_Portable_Network_Graphic.xcu`
- `source/config/fragments/types/calc_ADO_rowset_XML.xcu`
- `source/config/fragments/types/calc_AppleNumbers.xcu`
- `source/config/fragments/types/calc_Claris_Resolve.xcu`
- `source/config/fragments/types/calc_ClarisWorks.xcu`
- `source/config/fragments/types/calc_Gnumeric.xcu`
- `source/config/fragments/types/calc_HTML.xcu`
- `source/config/fragments/types/calc_Mac_Works.xcu`
- `source/config/fragments/types/calc_MS_Multiplan.xcu`
- `source/config/fragments/types/calc_MS_Works_Document.xcu`
- `source/config/fragments/types/calc_Parquet.xcu`
- `source/config/fragments/types/calc_WPS_Lotus_Document.xcu`
- `source/config/fragments/types/calc_WPS_QPro_Document.xcu`
- `source/config/fragments/types/draw_ClarisWorks.xcu`
- `source/config/fragments/types/draw_Freehand_Document.xcu`
- `source/config/fragments/types/draw_PageMaker_Document.xcu`
- `source/config/fragments/types/draw_QXP_Document.xcu`
- `source/config/fragments/types/draw_ZMF_Document.xcu`
- `source/config/fragments/types/emz_Compressed_MS_Windows_Metafile.xcu`
- `source/config/fragments/types/generic_JSON.xcu`
- `source/config/fragments/types/generic_Markdown.xcu`
- `source/config/fragments/types/generic_XML.xcu`
- `source/config/fragments/types/impress_AppleKeynote.xcu`
- `source/config/fragments/types/impress_ClarisWorks.xcu`
- `source/config/fragments/types/impress_PowerPoint3.xcu`
- `source/config/fragments/types/mov_MOV.xcu`
- `source/config/fragments/types/MS_Excel_2007_VBA_XML.xcu`
- `source/config/fragments/types/MS_PowerPoint_2007_XML_VBA.xcu`
- `source/config/fragments/types/MWAW_Bitmap.xcu`
- `source/config/fragments/types/MWAW_Database.xcu`
- `source/config/fragments/types/MWAW_Drawing.xcu`
- `source/config/fragments/types/MWAW_Presentation.xcu`
- `source/config/fragments/types/MWAW_Spreadsheet.xcu`
- `source/config/fragments/types/MWAW_Text_Document.xcu`
- `source/config/fragments/types/Palm_Text_Document.xcu`
- `source/config/fragments/types/StarBaseReport.xcu`
- `source/config/fragments/types/StarBaseReportChart.xcu`
- `source/config/fragments/types/StarOffice_Drawing.xcu`
- `source/config/fragments/types/StarOffice_Presentation.xcu`
- `source/config/fragments/types/StarOffice_Spreadsheet.xcu`
- `source/config/fragments/types/StarOffice_Writer.xcu`
- `source/config/fragments/types/svg_Scalable_Vector_Graphics_Draw.xcu`
- `source/config/fragments/types/svgz_Compressed_Scalable_Vector_Graphics.xcu`
- `source/config/fragments/types/webp_WebP.xcu`
- `source/config/fragments/types/wmz_Compressed_MS_Windows_Metafile.xcu`
- `source/config/fragments/types/writer_AbiWord_Document.xcu`
- `source/config/fragments/types/writer_ApplePages.xcu`
- `source/config/fragments/types/writer_BroadBand_eBook.xcu`
- `source/config/fragments/types/writer_ClarisWorks.xcu`
- `source/config/fragments/types/writer_DosWord.xcu`
- `source/config/fragments/types/writer_EPUB_Document.xcu`
- `source/config/fragments/types/writer_FictionBook_2.xcu`
- `source/config/fragments/types/writer_indexing_export_xml.xcu`
- `source/config/fragments/types/writer_Mac_Word.xcu`
- `source/config/fragments/types/writer_Mac_Works.xcu`
- `source/config/fragments/types/writer_MacWrite.xcu`
- `source/config/fragments/types/writer_Mariner_Write.xcu`
- `source/config/fragments/types/writer_MS_Word_2007_XML_VBA.xcu`
- `source/config/fragments/types/writer_MS_Write.xcu`
- `source/config/fragments/types/writer_PalmDoc.xcu`
- `source/config/fragments/types/writer_Plucker_eBook.xcu`
- `source/config/fragments/types/writer_WriteNow.xcu`
- `source/config/fragments/types/writerglobal8_template.xcu`
- `source/docbook/DocBookTemplate/manifest.rdf`
- `source/docbook/DocBookTemplate/META-INF/manifest.xml`
- `source/docbook/DocBookTemplate/mimetype`
- `source/docbook/DocBookTemplate/settings.xml`
- `source/docbook/DocBookTemplate/styles.xml`
- `source/graphic/GraphicExportFilter.cxx`
- `source/graphic/GraphicExportFilter.hxx`
- `source/graphic/graphicfilter.component`
- `source/pdf/pdfdecomposer.cxx`
- `source/storagefilterdetect/filterdetect.cxx`
- `source/storagefilterdetect/filterdetect.hxx`
- `source/storagefilterdetect/storagefd.component`
- `source/svg/.eslintrc.js`
- `source/svg/gentoken.py`
- `source/xslt/import/spreadsheetml/adorowset2ods.xsl`
- `uiconfig/ui/pdfgeneralpage.ui`
- `uiconfig/ui/pdflinkspage.ui`
- `uiconfig/ui/pdfoptionsdialog.ui`
- `uiconfig/ui/pdfsecuritypage.ui`
- `uiconfig/ui/pdfsignpage.ui`
- `uiconfig/ui/pdfuserinterfacepage.ui`
- `uiconfig/ui/pdfviewpage.ui`
- `uiconfig/ui/warnpdfdialog.ui`
- `uiconfig/ui/xmlfiltertabpagegeneral.ui`
- `uiconfig/ui/xmlfiltertabpagetransformation.ui`
- `uiconfig/ui/xsltfilterdialog.ui`
- `UIConfig_filter.mk`

---

## Absolute Locations

1. `filter/Configuration_filter.mk`
2. `autogen.sh`
3. `BUILD_FAILURE_ANALYSIS.md`
4. `build_images.ps1`
5. `IMAGES.md`
6. `IMAGES.html`
7. `IMAGES_logo.html`
8. `icon-themes/galaxy/brand/`
9. `icon-themes/galaxy/brand_dev/`
10. `icon-themes/hicontrast/brand/shell/`
11. `android/sdremote/res/drawable/libreoffice_logo.9.png`
12. `sc/source/ui/docshell/docsh.cxx`
13. `sc/source/ui/docshell/impex.cxx`
14. `src/0168229624cfac409e766913506961a8-ucpp-1.3.2.tar.gz`
15. `src/0279a21fab6f245e85a6f85fea54f511-source-code-font-1.009.tar.gz`
16. `src/0be45d54cc5e1c2e3102e32b8c190346-liberation-fonts-ttf-1.07.1.tar.gz`
17. `src/17410483b5b5f267aa18b7e00b65e6e0-hsqldb_1_8_0.zip`
18. `src/1756c4fa6c616ae15973c104cd8cb256-Adobe-Core35_AFMs-314.tar.gz`
19. `src/185d60944ea767075d27247c3162b3bc-unowinreg.dll`
20. `src/1e9ddfe25ac9577da709d7b2ea36f939-source-sans-font-1.036.tar.gz`
21. `src/1f24ab1d39f4a51faf22244c94a6203f-xmlsec1-1.2.14.tar.gz`
22. `src/26b3e95ddf3d9c077c480ea45874b3b8-lp_solve_5.5.tar.gz`
23. `src/284e768eeda0e2898b0d5bf7e26a016e-raptor-1.4.18.tar.gz`
24. `src/2a177023f9ea8ec8bd00837605c5df1b-jakarta-tomcat-5.0.30-src.tar.gz`
25. `src/2ae988b339daec234019a7066f96733e-commons-lang-2.3-src.tar.gz`
26. `src/2c9b0f83ed5890af02c0df1c1776f39b-commons-httpclient-3.1-src.tar.gz`
27. `src/3121aaf3e13e5d88dfff13fb4a5f1ab8-hunspell-1.3.2.tar.gz`
28. `src/3404ab6b1792ae5f16bbd603bd1e1d03-libformula-1.1.7.zip`
29. `src/35c94d2df8893241173de1d16b6034c0-swingExSrc.zip`
30. `src/35efabc239af896dfb79be7ebdd6e6b9-gentiumbasic-fonts-1.10.zip`
31. `src/39bb3fcea1514f1369fcfc87542390fd-sacjava-1.3.zip`
32. `src/3bdf40c0d199af31923e900d082ca2dd-libfonts-1.1.6.zip`
33. `src/3c219630e4302863a9a83d0efde889db-commons-logging-1.1.1-src.tar.gz`
34. `src/46e92b68e31e858512b680b3b61dc4c1-mythes-1.2.3.tar.gz`
35. `src/48d647fbd8ef8889e5a7f422c1bfda94-clucene-core-2.3.3.4.tar.gz`
36. `src/4a660ce8466c9df01f19036435425c3a-glibc-2.1.3-stub.tar.gz`
37. `src/54e578c91b1b68e69c72be22adcb2195-liblangtag-0.4.0.tar.bz2`
38. `src/599dc4cc65a07ee868cf92a667a913d2-xpdf-3.02.tar.gz`
39. `src/5c781723a0d9ed6188960defba8e91cf-liberation-fonts-ttf-2.00.1.tar.gz`
40. `src/798b2ffdc8bcfe7bca2cf92b62caf685-rhino1_5R5.zip`
41. `src/7a15edea7d415ac5150ea403e27401fd-open-sans-font-ttf-1.10.tar.gz`
42. `src/7c53f83e0327343f4060c0eb83842daf-icu4c-49_1_1-src.tgz`
43. `src/804c6cb5698db30b75ad0ff1c25baefd-openldap-2.4.31.tgz`
44. `src/861ef15fa0bc018f9ddc932c4ad8b6dd-lcms2-2.4.tar.gz`
45. `src/8755aac23317494a9028569374dc87b2-liborcus_0.3.0.tar.bz2`
46. `src/8ce2fcd72becf06c41f7201d15373ed9-librepository-1.1.6.zip`
47. `src/972afb8fdf02d9e7517e258b7fa7f0eb-libwpd-0.9.8.tar.bz2`
48. `src/97b2d4dba862397f446b217e2b623e71-libloader-1.1.6.zip`
49. `src/9d283e02441d8cebdcd1e5d9df227d67-libwpg-0.2.1.tar.bz2`
50. `src/9e5d864bce8f06751bbd99962ecf4aad-libpng-1.5.10.tar.gz`
51. `src/9f9e15966b5624834157fe3d748312bc-mdds_0.6.1.tar.bz2`
52. `src/a0a861f539f0e7a91d05e6b9457e4db1-nss-3.13.5-with-nspr-4.9.1.tar.gz`
53. `src/a2c10c04f396a9ce72894beb18b4e1f9-jpeg-8c.tar.gz`
54. `src/a2f6010987e1c601274ab5d63b72c944-hyphen-2.8.4.tar.gz`
55. `src/ace6ab49184e329db254e454a010f56d-libxml-1.1.7.zip`
56. `src/af3c3acf618de6108d65fcdc92b492e1-commons-codec-1.3-src.tar.gz`
57. `src/b2371dc7cf4811c9d32146eec913d296-libcmis-0.3.0.tar.gz`
58. `src/b3b2524f72409d919a4137826a870a8f-Python-3.3.0.tar.bz2`
59. `src/ba2930200c9f019c2d93a8c88c651a0f-flow-engine-0.9.4.zip`
60. `src/c0b4799ea9850eae3ead14f0a60e9418-postgresql-9.2.1.tar.bz2`
61. `src/c3c1a8ba7452950636e871d25020ce0d-pt-serif-font-1.0000W.tar.gz`
62. `src/ca66e26082cab8bb817185a116db809b-redland-1.0.8.tar.gz`
63. `src/d197bd6211669a2fa4ca648faf04bcb1-libwps-0.2.7.tar.bz2`
64. `src/d8bd5eed178db6e2b18eeed243f85aa8-flute-1.1.6.zip`
65. `src/db60e4fde8dd6d6807523deb71ee34dc-liblayout-0.2.10.zip`
66. `src/dc3d21a3921931096d6e80f6701f6763-libexttextcat-3.4.0.tar.bz2`
67. `src/dd7dab7a5fea97d2a6a43f511449b7cd-expat-2.1.0.tar.gz`
68. `src/e7a384790b13c29113e22e596ade9687-LinLibertineG-20120116.zip`
69. `src/ea570af93c284aa9e5621cd563f54f4d-bsh-2.0b1-src.tar.gz`
70. `src/ea91f2fb4212a21d708aced277e6e85a-vigra1.4.0.tar.gz`
71. `src/eeb2c7ddf0d302fba4bfc6e97eac9624-libbase-1.1.6.zip`
72. `src/f02578f5218f217a9f20e9c30e119c6a-boost_1_44_0.tar.bz2`
73. `src/f872f4ac066433d8ff92f5e316b36ff9-dejavu-fonts-ttf-2.33.zip`
74. `src/f94d9870737518e3b597f9265f4e9803-libserializer-1.1.6.zip`
75. `src/fa9aa839145cdf860bf596532bb8af97-cppunit-1.13.1.tar.gz`
76. `src/fca8706f2c4619e2fa3f8f42f8fc1e9d-rasqal-0.9.16.tar.gz`
77. `src/fetch.log`
78. `src/ff369e69ef0f0143beb5626164e87ae2-neon-0.29.5.tar.gz`
79. `src/libcdr-0.0.14.tar.bz2`
80. `src/libmspub-0.0.6.tar.bz2`
81. `src/libreoffice-dictionaries-4.0.6.2.tar.xz`
82. `src/libreoffice-help-4.0.6.2.tar.xz`
83. `src/libvisio-0.0.31.tar.bz2`

