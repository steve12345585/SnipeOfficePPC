/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the SnipeOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */
#ifndef _IDLC_IDLC_HXX_
#define _IDLC_IDLC_HXX_

#include <idlc/idlctypes.hxx>
#include <idlc/aststack.hxx>
#include <idlc/options.hxx>

#ifdef SAL_UNX
#define SEPARATOR '/'
#define PATH_SEPARATOR "/"
#else
#define SEPARATOR '\\'
#define PATH_SEPARATOR "\\"
#endif

class AstInterface;
class AstModule;
class AstType;
class Options;
class ErrorHandler;

class Idlc
{
public:
    Idlc(Options* pOptions);
    virtual ~Idlc();

    void init();

    bool dumpDeps(::rtl::OString const& rDepFile,
                  ::rtl::OString const& rTarget);

    Options* getOptions()
        { return m_pOptions; }
    AstStack* scopes()
        { return m_pScopes; }
    AstModule* getRoot()
        { return m_pRoot; }
    ErrorHandler* error()
        { return m_pErrorHandler; }
    const ::rtl::OString& getFileName()
        { return m_fileName; }
    void setFileName(const ::rtl::OString& fileName)
        { m_fileName = fileName; addInclude(fileName); }
    const ::rtl::OString& getMainFileName()
        { return m_mainFileName; }
    void setMainFileName(const ::rtl::OString& mainFileName)
        { m_mainFileName = mainFileName; }
    const ::rtl::OString& getRealFileName()
        { return m_realFileName; }
    void setRealFileName(const ::rtl::OString& realFileName)
        { m_realFileName = realFileName; }
    const ::rtl::OString& getDocumentation()
        {
            m_bIsDocValid = sal_False;
            return m_documentation;
        }
    void setDocumentation(const ::rtl::OString& documentation)
        {
            m_documentation = documentation;
            m_bIsDocValid = sal_True;
        }
    OUString processDocumentation();
    sal_Bool isInMainFile()
        { return m_bIsInMainfile; }
    void setInMainfile(sal_Bool bInMainfile)
        { m_bIsInMainfile = bInMainfile; }
    sal_uInt32 getErrorCount()
        { return m_errorCount; }
    void setErrorCount(sal_uInt32 errorCount)
        { m_errorCount = errorCount; }
    void incErrorCount()
        { m_errorCount++; }
    sal_uInt32 getWarningCount()
        { return m_warningCount; }
    void setWarningCount(sal_uInt32 warningCount)
        { m_warningCount = warningCount; }
    void incWarningCount()
        { m_warningCount++; }
    sal_uInt32 getLineNumber()
        { return m_lineNumber; }
    sal_uInt32 getOffsetStart()
        { return m_offsetStart; }
    sal_uInt32 getOffsetEnd()
        { return m_offsetEnd; }
    void setOffset( sal_uInt32 start, sal_uInt32 end)
        { m_offsetStart = start; m_offsetEnd = end; }
    void setLineNumber(sal_uInt32 lineNumber)
        { m_lineNumber = lineNumber; }
    void incLineNumber()
        { m_lineNumber++; }
    ParseState getParseState()
        { return m_parseState; }
    void setParseState(ParseState parseState)
        { m_parseState = parseState; }

    void addInclude(const ::rtl::OString& inc)
        { m_includes.insert(inc); }
    StringSet* getIncludes()
        { return &m_includes; }

    void setPublished(bool published) { m_published = published; }
    bool isPublished() const { return m_published; }

    void reset();
private:
    Options*            m_pOptions;
    AstStack*           m_pScopes;
    AstModule*          m_pRoot;
    ErrorHandler*       m_pErrorHandler;
    ::rtl::OString      m_fileName;
    ::rtl::OString      m_mainFileName;
    ::rtl::OString      m_realFileName;
    ::rtl::OString      m_documentation;
    sal_Bool            m_bIsDocValid;
    sal_Bool            m_bGenerateDoc;
    sal_Bool            m_bIsInMainfile;
    bool                m_published;
    sal_uInt32          m_errorCount;
    sal_uInt32          m_warningCount;
    sal_uInt32          m_lineNumber;
    sal_uInt32          m_offsetStart;
    sal_uInt32          m_offsetEnd;
    ParseState          m_parseState;
    StringSet           m_includes;
};


typedef ::std::pair< ::rtl::OString, ::rtl::OString > sPair_t;
sal_Int32 compileFile(const ::rtl::OString * pathname);
    // a null pathname means stdin
sal_Int32 produceFile(const ::rtl::OString& filenameBase,
        sPair_t const*const pDepFile);
    // filenameBase is filename without ".idl"
void removeIfExists(const ::rtl::OString& pathname);

::rtl::OString makeTempName(const ::rtl::OString& prefix, const ::rtl::OString& postfix);
sal_Bool copyFile(const ::rtl::OString* source, const ::rtl::OString& target);
    // a null source means stdin

sal_Bool isFileUrl(const ::rtl::OString& fileName);
::rtl::OString convertToAbsoluteSystemPath(const ::rtl::OString& fileName);
::rtl::OString convertToFileUrl(const ::rtl::OString& fileName);

Idlc* SAL_CALL idlc();
Idlc* SAL_CALL setIdlc(Options* pOptions);

AstDeclaration const * resolveTypedefs(AstDeclaration const * type);

AstDeclaration const * deconstructAndResolveTypedefs(
    AstDeclaration const * type, sal_Int32 * rank);

AstInterface const * resolveInterfaceTypedefs(AstType const * type);

#endif // _IDLC_IDLC_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
