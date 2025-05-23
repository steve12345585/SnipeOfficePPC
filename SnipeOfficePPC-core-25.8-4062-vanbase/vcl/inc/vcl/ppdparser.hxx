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
#ifndef _PSPRINT_PPDPARSER_HXX_
#define _PSPRINT_PPDPARSER_HXX_

#include <list>
#include <vector>
#include <boost/unordered_map.hpp>

#include "tools/string.hxx"
#include "tools/stream.hxx"
#include "rtl/string.hxx"

#include "vcl/dllapi.h"

#include "com/sun/star/lang/Locale.hpp"

#define PRINTER_PPDDIR "driver"

namespace psp {

class PPDParser;
class PPDTranslator;

enum PPDValueType { eInvocation, eQuoted, eSymbol, eString, eNo };

struct VCL_DLLPUBLIC PPDValue
{
    PPDValueType    m_eType;
    String          m_aOption;
    String          m_aValue;
};

// ----------------------------------------------------------------------

/*
 * PPDKey - a container for the available options (=values) of a PPD keyword
 */

class VCL_DLLPUBLIC PPDKey
{
    friend class PPDParser;

    typedef ::boost::unordered_map< ::rtl::OUString, PPDValue, ::rtl::OUStringHash > hash_type;
    typedef ::std::vector< PPDValue* > value_type;

    String              m_aKey;
    hash_type           m_aValues;
    value_type          m_aOrderedValues;
    const PPDValue*     m_pDefaultValue;
    bool                m_bQueryValue;
    PPDValue            m_aQueryValue;

public:
    enum UIType { PickOne, PickMany, Boolean };
    enum SetupType { ExitServer, Prolog, DocumentSetup, PageSetup, JCLSetup, AnySetup };
private:

    bool                m_bUIOption;
    UIType              m_eUIType;
    int                 m_nOrderDependency;
    SetupType           m_eSetupType;

    void eraseValue( const String& rOption );
public:
    PPDKey( const String& rKey );
    ~PPDKey();

    PPDValue*           insertValue( const String& rOption );
    int                 countValues() const
    { return m_aValues.size(); }
    // neither getValue will return the query option
    const PPDValue*         getValue( int n ) const;
    const PPDValue*         getValue( const String& rOption ) const;
    const PPDValue*         getValueCaseInsensitive( const String& rOption ) const;
    const PPDValue*         getDefaultValue() const { return m_pDefaultValue; }
    const PPDValue*     getQueryValue() const { return m_bQueryValue ? &m_aQueryValue : NULL; }

    const String&       getKey() const { return m_aKey; }
    bool                isUIKey() const { return m_bUIOption; }
    UIType              getUIType() const { return m_eUIType; }
    SetupType           getSetupType() const { return m_eSetupType; }
    int                 getOrderDependency() const { return m_nOrderDependency; }
};

// define a hash for PPDKey
struct PPDKeyhash
{
    size_t operator()( const PPDKey * pKey) const
        { return (size_t)pKey; }
};

// ----------------------------------------------------------------------

/*
 * PPDParser - parses a PPD file and contains all available keys from it
 */

class PPDContext;
class CUPSManager;

class VCL_DLLPUBLIC PPDParser
{
    friend class PPDContext;
    friend class CUPSManager;
    friend class PPDCache;

    typedef ::boost::unordered_map< ::rtl::OUString, PPDKey*, ::rtl::OUStringHash > hash_type;
    typedef ::std::vector< PPDKey* > value_type;

    void insertKey( const String& rKey, PPDKey* pKey );
public:
    struct PPDConstraint
    {
        const PPDKey*       m_pKey1;
        const PPDValue*     m_pOption1;
        const PPDKey*       m_pKey2;
        const PPDValue*     m_pOption2;

        PPDConstraint() : m_pKey1( NULL ), m_pOption1( NULL ), m_pKey2( NULL ), m_pOption2( NULL ) {}
    };
private:
    hash_type                                   m_aKeys;
    value_type                                  m_aOrderedKeys;
    ::std::list< PPDConstraint >                m_aConstraints;

    // some identifying fields
    String                                      m_aPrinterName;
    String                                      m_aNickName;
    // the full path of the PPD file
    String                                      m_aFile;
    // some basic attributes
    bool                                        m_bColorDevice;
    bool                                        m_bType42Capable;
    sal_uLong                                       m_nLanguageLevel;
    rtl_TextEncoding                            m_aFileEncoding;


    // shortcuts to important keys and their default values
    // imageable area
    const PPDValue*                             m_pDefaultImageableArea;
    const PPDKey*                               m_pImageableAreas;
    // paper dimensions
    const PPDValue*                             m_pDefaultPaperDimension;
    const PPDKey*                               m_pPaperDimensions;
    // paper trays
    const PPDValue*                             m_pDefaultInputSlot;
    const PPDKey*                               m_pInputSlots;
    // resolutions
    const PPDValue*                             m_pDefaultResolution;
    const PPDKey*                               m_pResolutions;
    // duplex commands
    const PPDValue*                             m_pDefaultDuplexType;
    const PPDKey*                               m_pDuplexTypes;

    // fonts
    const PPDKey*                               m_pFontList;

    // translations
    PPDTranslator*                              m_pTranslator;

    PPDParser( const String& rFile );
    ~PPDParser();

    void parseOrderDependency(const rtl::OString& rLine);
    void parseOpenUI(const rtl::OString& rLine);
    void parseConstraint(const rtl::OString& rLine);
    void parse( std::list< rtl::OString >& rLines );

    String handleTranslation(const rtl::OString& i_rString, bool i_bIsGlobalized);

    static void scanPPDDir( const String& rDir );
    static void initPPDFiles();
    static String getPPDFile( const String& rFile );
public:
    static const PPDParser* getParser( const String& rFile );
    static String getPPDPrinterName( const String& rFile );
    static void freeAll();
    static void getKnownPPDDrivers( std::list< rtl::OUString >& o_rDrivers, bool bRefresh = false );

    const String&   getFilename() const { return m_aFile; }

    const PPDKey*   getKey( int n ) const;
    const PPDKey*   getKey( const String& rKey ) const;
    int             getKeys() const { return m_aKeys.size(); }
    bool            hasKey( const PPDKey* ) const;

    const ::std::list< PPDConstraint >& getConstraints() const { return m_aConstraints; }

    const String&   getPrinterName() const
    { return m_aPrinterName.Len() ? m_aPrinterName : m_aNickName; }
    const String&   getNickName() const
    { return m_aNickName.Len() ? m_aNickName : m_aPrinterName; }

    bool            isColorDevice() const { return m_bColorDevice; }
    bool            isType42Capable() const { return m_bType42Capable; }
    sal_uLong           getLanguageLevel() const { return m_nLanguageLevel; }

    String          getDefaultPaperDimension() const;
    void            getDefaultPaperDimension( int& rWidth, int& rHeight ) const
    { getPaperDimension( getDefaultPaperDimension(), rWidth, rHeight ); }
    bool getPaperDimension( const String& rPaperName,
                            int& rWidth, int& rHeight ) const;
    // width and height in pt
    // returns false if paper not found
    int             getPaperDimensions() const
    { return m_pPaperDimensions ? m_pPaperDimensions->countValues() : 0; }

    // match the best paper for width and height
    String          matchPaper( int nWidth, int nHeight ) const;

    bool getMargins( const String& rPaperName,
                     int &rLeft, int& rRight,
                     int &rUpper, int& rLower ) const;
    // values in pt
    // returns true if paper found

    // values int pt

    String          getDefaultInputSlot() const;
    int             getInputSlots() const
    { return m_pInputSlots ? m_pInputSlots->countValues() : 0; }

    void            getDefaultResolution( int& rXRes, int& rYRes ) const;
    // values in dpi
    void            getResolutionFromString( const String&, int&, int& ) const;
    // helper function

    int             getDuplexTypes() const
    { return m_pDuplexTypes ? m_pDuplexTypes->countValues() : 0; }

    int             getFonts() const
    { return m_pFontList ? m_pFontList->countValues() : 0; }
    String          getFont( int ) const;


    rtl::OUString   translateKey( const rtl::OUString& i_rKey,
                                  const com::sun::star::lang::Locale& i_rLocale = com::sun::star::lang::Locale() ) const;
    rtl::OUString   translateOption( const rtl::OUString& i_rKey,
                                     const rtl::OUString& i_rOption,
                                     const com::sun::star::lang::Locale& i_rLocale = com::sun::star::lang::Locale() ) const;
};

// ----------------------------------------------------------------------

/*
 * PPDContext - a class to manage user definable states based on the
 * contents of a PPDParser.
 */

class VCL_DLLPUBLIC PPDContext
{
    typedef ::boost::unordered_map< const PPDKey*, const PPDValue*, PPDKeyhash > hash_type;
    hash_type m_aCurrentValues;
    const PPDParser*                                    m_pParser;

    // returns false: check failed, new value is constrained
    //         true:  check succeded, new value can be set
    bool checkConstraints( const PPDKey*, const PPDValue*, bool bDoReset );
    bool resetValue( const PPDKey*, bool bDefaultable = false );
public:
    PPDContext( const PPDParser* pParser = NULL );
    PPDContext( const PPDContext& rContext ) { operator=( rContext ); }
    PPDContext& operator=( const PPDContext& rContext );
    ~PPDContext();

    void setParser( const PPDParser* );
    const PPDParser* getParser() const { return m_pParser; }

    const PPDValue* getValue( const PPDKey* ) const;
    const PPDValue* setValue( const PPDKey*, const PPDValue*, bool bDontCareForConstraints = false );

    int countValuesModified() const { return m_aCurrentValues.size(); }
    const PPDKey* getModifiedKey( int n ) const;

    // public wrapper for the private method
    bool checkConstraints( const PPDKey*, const PPDValue* );

    // for printer setup
    char*   getStreamableBuffer( sal_uLong& rBytes ) const;
    void    rebuildFromStreamBuffer( char* pBuffer, sal_uLong nBytes );

    // convenience
    int getRenderResolution() const;

    // width, height in points, paper will contain the name of the selected
    // paper after the call
    void getPageSize( rtl::OUString& rPaper, int& rWidth, int& rHeight ) const;
};

} // namespace

#endif // _PSPRINT_PPDPARSER_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
