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

#ifndef INCLUDED_NUMBERINGMANAGER_HXX
#define INCLUDED_NUMBERINGMANAGER_HXX

#include "PropertyMap.hxx"

#include <WriterFilterDllApi.hxx>
#include <dmapper/DomainMapper.hxx>
#include <resourcemodel/LoggedResources.hxx>

#include <editeng/numitem.hxx>

#include <com/sun/star/container/XIndexReplace.hpp>

namespace writerfilter {
namespace dmapper {

class DomainMapper;
class StyleSheetEntry;


/** Class representing the numbering level properties.
 */
class ListLevel : public PropertyMap
{
    sal_Int32                                     m_nIStartAt;       //LN_ISTARTAT
    sal_Int32                                     m_nNFC;            //LN_NFC
    sal_Int32                                     m_nJC;             //LN_JC
    sal_Int32                                     m_nFLegal;         //LN_FLEGAL
    sal_Int32                                     m_nFNoRestart;     //LN_FNORESTART
    sal_Int32                                     m_nFPrev;          //LN_FPREV
    sal_Int32                                     m_nFPrevSpace;     //LN_FPREVSPACE
    sal_Int32                                     m_nFWord6;         //LN_FWORD6
    OUString                               m_sRGBXchNums;     //LN_RGBXCHNUMS
    sal_Int16                                     m_nXChFollow;      //LN_IXCHFOLLOW
    OUString                               m_sBulletChar;
    sal_Int32                                     m_nTabstop;
    boost::shared_ptr< StyleSheetEntry >          m_pParaStyle;
    bool                                          m_outline;

public:

    typedef boost::shared_ptr< ListLevel > Pointer;

    ListLevel() :
        m_nIStartAt(-1)
        ,m_nNFC(-1)
        ,m_nJC(-1)
        ,m_nFLegal(-1)
        ,m_nFNoRestart(-1)
        ,m_nFPrev(-1)
        ,m_nFPrevSpace(-1)
        ,m_nFWord6(-1)
        ,m_nXChFollow(SvxNumberFormat::LISTTAB)
        ,m_nTabstop( 0 )
        ,m_outline(false)
        {}

    ~ListLevel( ){ }

    // Setters for the import
    void SetValue( Id nId, sal_Int32 nValue );
    void SetBulletChar( OUString sValue ) { m_sBulletChar = sValue; };
    void SetParaStyle( boost::shared_ptr< StyleSheetEntry > pStyle );
    void AddRGBXchNums( OUString sValue ) { m_sRGBXchNums += sValue; };

    // Getters
    OUString GetBulletChar( ) { return m_sBulletChar; };
    boost::shared_ptr< StyleSheetEntry > GetParaStyle( ) { return m_pParaStyle; };
    bool isOutlineNumbering() const { return m_outline; }

    // UNO mapping functions

    // rPrefix and rSuffix are out parameters
    static sal_Int16 GetParentNumbering( OUString sText, sal_Int16 nLevel,
        OUString& rPrefix, OUString& rSuffix );

    com::sun::star::uno::Sequence< com::sun::star::beans::PropertyValue >
        GetProperties(  );

    com::sun::star::uno::Sequence< com::sun::star::beans::PropertyValue>
        GetCharStyleProperties( );
private:

    com::sun::star::uno::Sequence< com::sun::star::beans::PropertyValue >
        GetLevelProperties(  );

    void AddParaProperties( com::sun::star::uno::Sequence< com::sun::star::beans::PropertyValue >* props );
};

class AbstractListDef
{
private:
    sal_Int32                            m_nTPLC;          //LN_TPLC
    OUString                      m_sRGISTD;        //LN_RGISTD
    sal_Int32                            m_nSimpleList;    //LN_FSIMPLELIST
    sal_Int32                            m_nRestart;       //LN_FRESTARTHDN
    sal_Int32                            m_nUnsigned;      //LN_UNSIGNED26_2

    // The ID member reflects either the abstractNumId or the numId
    // depending on the use of the class
    sal_Int32                            m_nId;

    // Properties of each level. This can also reflect the overridden
    // levels of a numbering.
    ::std::vector< ListLevel::Pointer >  m_aLevels;

    // Only used during the numberings import
    ListLevel::Pointer                         m_pCurrentLevel;

public:
    typedef boost::shared_ptr< AbstractListDef > Pointer;

    AbstractListDef( );
    virtual ~AbstractListDef( );

    // Setters using during the import
    void SetId( sal_Int32 nId ) { m_nId = nId; };
    void SetValue( sal_uInt32 nSprmId, sal_Int32 nValue );
    void AddRGISTD( OUString sValue ) { m_sRGISTD += sValue; };

    // Accessors
    sal_Int32             GetId( ) { return m_nId; };

    sal_Int16             Size( ) { return sal_Int16( m_aLevels.size( ) ); };
    ListLevel::Pointer    GetLevel( sal_uInt16 nLvl );
    void                  AddLevel( );

    ListLevel::Pointer    GetCurrentLevel( ) { return m_pCurrentLevel; };

    virtual com::sun::star::uno::Sequence<
        com::sun::star::uno::Sequence<
            com::sun::star::beans::PropertyValue > > GetPropertyValues( );
};

class ListDef : public AbstractListDef
{
private:
    // Pointer to the abstract numbering
    AbstractListDef::Pointer             m_pAbstractDef;

    // Cache for the UNO numbering rules
    uno::Reference< container::XIndexReplace > m_xNumRules;

public:
    typedef boost::shared_ptr< ListDef > Pointer;

    ListDef( );
    virtual ~ListDef( );

    // Accessors
    void SetAbstractDefinition( AbstractListDef::Pointer pAbstract ) { m_pAbstractDef = pAbstract; };
    AbstractListDef::Pointer GetAbstractDefinition( ) { return m_pAbstractDef; };

    // Mapping functions
    static OUString GetStyleName( sal_Int32 nId );

    com::sun::star::uno::Sequence<
        com::sun::star::uno::Sequence<
            com::sun::star::beans::PropertyValue > > GetPropertyValues( );

    void CreateNumberingRules(
            DomainMapper& rDMapper,
            com::sun::star::uno::Reference<
                com::sun::star::lang::XMultiServiceFactory> xFactory );

    ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexReplace >
            GetNumberingRules( ) { return m_xNumRules; };

};

/** This class provides access to the defined numbering styles.
  */
class ListsManager :
    public LoggedProperties,
    public LoggedTable
{
private:

    DomainMapper&                                       m_rDMapper;
    com::sun::star::uno::Reference<
        com::sun::star::lang::XMultiServiceFactory >    m_xFactory;

    // The numbering entries
    std::vector< AbstractListDef::Pointer >             m_aAbstractLists;
    std::vector< ListDef::Pointer >                     m_aLists;


    // These members are used for import only
    AbstractListDef::Pointer                            m_pCurrentDefinition;
    bool                                                m_bIsLFOImport;

    AbstractListDef::Pointer    GetAbstractList( sal_Int32 nId );

    // Properties
    virtual void lcl_attribute( Id nName, Value & rVal );
    virtual void lcl_sprm(Sprm & sprm);

    // Table
    virtual void lcl_entry(int pos, writerfilter::Reference<Properties>::Pointer_t ref);

public:

    ListsManager(
            DomainMapper& rDMapper,
            const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > xFactory);
    virtual ~ListsManager();

    typedef boost::shared_ptr< ListsManager >  Pointer;

    // Config methods
    void SetLFOImport( bool bLFOImport ) { m_bIsLFOImport = bLFOImport; };

    // Numberings accessors
    AbstractListDef::Pointer GetCurrentDef( ) { return m_pCurrentDefinition; };

    sal_uInt32              Size() const
        { return sal_uInt32( m_aLists.size( ) ); };
    ListDef::Pointer        GetList( sal_Int32 nId );

    // Mapping methods
    void CreateNumberingRules( );
};

} }

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
