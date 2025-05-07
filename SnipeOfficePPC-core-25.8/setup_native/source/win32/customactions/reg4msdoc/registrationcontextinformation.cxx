/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
//Implementierung der Klasse RegistrationContextInformation.

#include "registrationcontextinformation.hxx"
#include "msihelper.hxx"

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
#include <assert.h>
#include <algorithm>

namespace /* private */
{
    const int MAX_REGKEY_LENGTH_WIN9X = 16300;
}

RegistrationContextInformation::RegistrationContextInformation(MSIHANDLE hMsi, const std::wstring& OpenOfficeExecutablePath) :
    msihandle_(hMsi),
    m_OOExecPath(OpenOfficeExecutablePath)
{
    assert(m_OOExecPath.length());
    ExtractOpenOfficeExecNameFromPath();
    m_OOIconPath = m_OOExecPath.replace(m_OOExecPath.find(m_OOExecName), m_OOExecName.size(), std::wstring(TEXT("soffice.bin")));
}

std::wstring RegistrationContextInformation::GetWordDocumentDisplayName() const
{
    std::wstring str;
    GetMsiProp(msihandle_, TEXT("STR_MS_WORD_DOCUMENT"), str);
    return str;
}

std::wstring RegistrationContextInformation::GetWordDocumentFileExtension() const
{
    return std::wstring(TEXT(".doc"));
}

std::wstring RegistrationContextInformation::GetWordDocumentDefaultIconEntry() const
{
    return m_OOIconPath + std::wstring(TEXT(",1"));
}

std::wstring RegistrationContextInformation::GetWordDocumentDefaultShellCommand() const
{
    return std::wstring(TEXT("open"));
}

std::wstring RegistrationContextInformation::GetMOOXWordDocumentDisplayName() const
{
    std::wstring str;
    str = TEXT("Microsoft Word 2007 Document");
    return str;
}

std::wstring RegistrationContextInformation::GetMOOXWordDocumentFileExtension() const
{
    return std::wstring(TEXT(".docx"));
}

std::wstring RegistrationContextInformation::GetWordTemplateDisplayName() const
{
    std::wstring str;
    GetMsiProp(msihandle_, TEXT("STR_MS_WORD_TEMPLATE"), str);
    return str;
}

std::wstring RegistrationContextInformation::GetWordTemplateFileExtension() const
{
    return std::wstring(TEXT(".dot"));
}

std::wstring RegistrationContextInformation::GetWordTemplateDefaultIconEntry() const
{
    return m_OOIconPath + std::wstring(TEXT(",2"));
}

std::wstring RegistrationContextInformation::GetWordTemplateDefaultShellCommand() const
{
    return std::wstring(TEXT("new"));
}

std::wstring RegistrationContextInformation::GetRtfDocumentDisplayName() const
{
    std::wstring str;
    GetMsiProp(msihandle_, TEXT("STR_RTF_DOCUMENT"), str);
    return str;
}

std::wstring RegistrationContextInformation::GetRtfDocumentFileExtension() const
{
    return std::wstring(TEXT(".rtf"));
}

std::wstring RegistrationContextInformation::GetRtfDocumentDefaultIconEntry() const
{
    return m_OOIconPath + std::wstring(TEXT(",1"));
}

std::wstring RegistrationContextInformation::GetRtfDocumentDefaultShellCommand() const
{
    return std::wstring(TEXT("open"));
}

std::wstring RegistrationContextInformation::GetExcelSheetDisplayName() const
{
    std::wstring str;
    GetMsiProp(msihandle_, TEXT("STR_MS_EXCEL_WORKSHEET"), str);
    return str;
}

std::wstring RegistrationContextInformation::GetExcelSheetFileExtension() const
{
    return std::wstring(TEXT(".xls"));
}

std::wstring RegistrationContextInformation::GetExcelSheetDefaultIconEntry() const
{
    return m_OOIconPath + std::wstring(TEXT(",3"));
}

std::wstring RegistrationContextInformation::GetExcelSheetDefaultShellCommand() const
{
    return std::wstring(TEXT("open"));
}

std::wstring RegistrationContextInformation::GetMOOXExcelSheetDisplayName() const
{
    std::wstring str;
    str = TEXT("Microsoft Excel 2007 Spreadsheet");
    return str;
}

std::wstring RegistrationContextInformation::GetMOOXExcelSheetFileExtension() const
{
    return std::wstring(TEXT(".xlsx"));
}

std::wstring RegistrationContextInformation::GetExcelTemplateDisplayName() const
{
    std::wstring str;
    GetMsiProp(msihandle_, TEXT("STR_MS_EXCEL_TEMPLATE"), str);
    return str;
}

std::wstring RegistrationContextInformation::GetExcelTemplateFileExtension() const
{
    return std::wstring(TEXT(".xlt"));
}

std::wstring RegistrationContextInformation::GetExcelTemplateDefaultIconEntry() const
{
    return m_OOIconPath + std::wstring(TEXT(",4"));
}

std::wstring RegistrationContextInformation::GetExcelTemplateDefaultShellCommand() const
{
    return std::wstring(TEXT("new"));
}

std::wstring RegistrationContextInformation::GetPowerPointDocumentDisplayName() const
{
    std::wstring str;
    GetMsiProp(msihandle_, TEXT("STR_MS_POWERPOINT_PRESENTATION"), str);
    return str;
}

std::wstring RegistrationContextInformation::GetPowerPointDocumentFileExtension() const
{
    return std::wstring(TEXT(".ppt"));
}

std::wstring RegistrationContextInformation::GetPowerPointDocumentDefaultIconEntry() const
{
    return m_OOIconPath + std::wstring(TEXT(",7"));
}

std::wstring RegistrationContextInformation::GetPowerPointDocumentDefaultShellCommand() const
{
    return std::wstring(TEXT("open"));
}

std::wstring RegistrationContextInformation::GetMOOXPowerPointDocumentDisplayName() const
{
    std::wstring str;
    str = TEXT("Microsoft PowerPoint 2007 Presentation");
    return str;
}

std::wstring RegistrationContextInformation::GetMOOXPowerPointDocumentFileExtension() const
{
    return std::wstring(TEXT(".pptx"));
}

std::wstring RegistrationContextInformation::GetPowerPointTemplateDisplayName() const
{
    std::wstring str;
    GetMsiProp(msihandle_, TEXT("STR_MS_POWERPOINT_TEMPLATE"), str);
    return str;
}

std::wstring RegistrationContextInformation::GetPowerPointTemplateFileExtension() const
{
    return std::wstring(TEXT(".pot"));
}

std::wstring RegistrationContextInformation::GetPowerPointTemplateDefaultIconEntry() const
{
    return m_OOIconPath + std::wstring(TEXT(",8"));
}

std::wstring RegistrationContextInformation::GetPowerPointTemplateDefaultShellCommand() const
{
    return std::wstring(TEXT("new"));
}

std::wstring RegistrationContextInformation::GetPowerPointShowDisplayName() const
{
    std::wstring str;
    GetMsiProp(msihandle_, TEXT("STR_MS_POWERPOINT_SHOW"), str);
    return str;
}

std::wstring RegistrationContextInformation::GetPowerPointShowFileExtension() const
{
    return std::wstring(TEXT(".pps"));
}

std::wstring RegistrationContextInformation::GetPowerPointShowDefaultIconEntry() const
{
    return m_OOIconPath + std::wstring(TEXT(",7"));
}

std::wstring RegistrationContextInformation::GetPowerPointShowDefaultShellCommand() const
{
    return std::wstring(TEXT("open"));
}

//----------------------------------------------
/** The string for the "New" command that should appear
    in the Explorer context menu when someone right
    clicks a Microsoft document
*/
std::wstring RegistrationContextInformation::ShellNewCommandDisplayName() const
{
    std::wstring str;
    GetMsiProp(msihandle_, TEXT("STR_NEW_DISPLAY_NAME"), str);
    std::wstring::size_type idx = str.find(TEXT("~"));

    if(std::wstring::npos != idx)
        str.replace(idx, 1, TEXT("&"));

    return str;
}

/** The string for the "Edit" command that should
    appear in the Explorer context menu when someone
    right clicks a document
*/
std::wstring RegistrationContextInformation::ShellEditCommandDisplayName() const
{
    std::wstring str;
    GetMsiProp(msihandle_, TEXT("STR_EDIT"), str);
    std::wstring::size_type idx = str.find(TEXT("~"));

    if(std::wstring::npos != idx)
        str.replace(idx, 1, TEXT("&"));

    return str;
}

std::wstring RegistrationContextInformation::GetOpenOfficeFriendlyAppName() const
{
    std::wstring str;
    GetMsiProp(msihandle_, TEXT("ProductName"), str);
    return str;
}

std::wstring RegistrationContextInformation::GetOpenOfficeExecutablePath() const
{
    return m_OOExecPath;
}

//----------------------------------------------
/** The name of the executable (currently "soffice.exe"
    but may change in the future, who knows) */
std::wstring RegistrationContextInformation::GetOpenOfficeExecutableName() const
{
    return m_OOExecName;
}

/** A command line for the specified shell command */
std::wstring RegistrationContextInformation::GetOpenOfficeCommandline(SHELL_COMMAND ShellCommand,
                                                                      OFFICE_APPLICATION OfficeApp) const
{
    // quote the path to OpenOffice, this is important
    // for Windows 9x
    std::wstring cmd_line = std::wstring(TEXT("\"")) + m_OOExecPath + std::wstring(TEXT("\""));

    switch( OfficeApp )
    {
    case Writer:
        cmd_line += std::wstring( TEXT( " -writer" ) );
        break;
    case Calc:
        cmd_line += std::wstring( TEXT( " -calc" ) );
        break;
    case Impress:
        cmd_line += std::wstring( TEXT( " -impress" ) );
        break;
    case Office: // default to std command line
        break;
    // default: no default to find new added enums at compile time
    }
    switch(ShellCommand)
    {
    case New:
        cmd_line += std::wstring(TEXT(" -n \"%1\""));
        break;
    case Open:
        cmd_line += std::wstring(TEXT(" -o \"%1\""));
        break;
    case Print:
        cmd_line += std::wstring(TEXT(" -p \"%1\""));
        break;
    case Printto:
        cmd_line += std::wstring(TEXT(" -pt \"%2\" \"%1\""));
        break;
    // default: no default to find new added enums at compile time
    }
    return cmd_line;
}

void RegistrationContextInformation::ExtractOpenOfficeExecNameFromPath()
{
    std::wstring::size_type idx = m_OOExecPath.find_last_of(TEXT('\\'));
    assert(idx != std::wstring::npos); // assert valid path
    m_OOExecName = m_OOExecPath.substr(idx + 1);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
