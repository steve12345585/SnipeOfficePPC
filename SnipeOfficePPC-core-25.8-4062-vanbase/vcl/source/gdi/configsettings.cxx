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


#include <vcl/configsettings.hxx>

#include <svdata.hxx>

#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/beans/PropertyValue.hpp>

using namespace utl;
using namespace vcl;
using namespace com::sun::star::uno;
using namespace com::sun::star::lang;
using namespace com::sun::star::beans;
using namespace com::sun::star::container;

using ::rtl::OUString;

#define SETTINGS_CONFIGNODE "VCL/Settings"

/*
 *  SettingsConfigItem::get
 */

SettingsConfigItem* SettingsConfigItem::get()
{
    ImplSVData* pSVData = ImplGetSVData();
    if( ! pSVData->mpSettingsConfigItem )
        pSVData->mpSettingsConfigItem = new SettingsConfigItem();
    return pSVData->mpSettingsConfigItem;
}

/*
 *  SettignsConfigItem constructor
 */

SettingsConfigItem::SettingsConfigItem()
        :
        ConfigItem( OUString( RTL_CONSTASCII_USTRINGPARAM( SETTINGS_CONFIGNODE ) ),
                    CONFIG_MODE_DELAYED_UPDATE ),
    m_aSettings( 0 )
{
    getValues();
}

/*
 *  SettingsConfigItem destructor
 */

SettingsConfigItem::~SettingsConfigItem()
{
    if( IsModified() )
        Commit();
}

/*
 *  SettingsConfigItem::Commit
 */

void SettingsConfigItem::Commit()
{
    if( ! IsValidConfigMgr() )
        return;

    boost::unordered_map< OUString, SmallOUStrMap, rtl::OUStringHash >::const_iterator group;

    for( group = m_aSettings.begin(); group != m_aSettings.end(); ++group )
    {
        String aKeyName( group->first );
        /*sal_Bool bAdded =*/ AddNode( OUString(), aKeyName );
        Sequence< PropertyValue > aValues( group->second.size() );
        PropertyValue* pValues = aValues.getArray();
        int nIndex = 0;
        SmallOUStrMap::const_iterator it;
        for( it = group->second.begin(); it != group->second.end(); ++it )
        {
            String aName( aKeyName );
            aName.Append( '/' );
            aName.Append( String( it->first ) );
            pValues[nIndex].Name    = aName;
            pValues[nIndex].Handle  = 0;
            pValues[nIndex].Value <<= it->second;
            pValues[nIndex].State   = PropertyState_DIRECT_VALUE;
            nIndex++;
        }
        ReplaceSetProperties( aKeyName, aValues );
    }
}

/*
 *  SettingsConfigItem::Notify
 */

void SettingsConfigItem::Notify( const Sequence< OUString >& )
{
    getValues();
}

/*
 *  SettingsConfigItem::getValues
 */
void SettingsConfigItem::getValues()
{
    if( ! IsValidConfigMgr() )
        return;

    m_aSettings.clear();

    Sequence< OUString > aNames( GetNodeNames( OUString() ) );

    for( int j = 0; j < aNames.getLength(); j++ )
    {
#if OSL_DEBUG_LEVEL > 2
        OSL_TRACE( "found settings data for \"%s\"\n",
                 OUStringToOString( aNames.getConstArray()[j], RTL_TEXTENCODING_ASCII_US ).getStr()
                 );
#endif
        String aKeyName( aNames.getConstArray()[j] );
        Sequence< OUString > aKeys( GetNodeNames( aKeyName ) );
        Sequence< OUString > aSettingsKeys( aKeys.getLength() );
        const OUString* pFrom = aKeys.getConstArray();
        OUString* pTo = aSettingsKeys.getArray();
        for( int m = 0; m < aKeys.getLength(); m++ )
        {
            String aName( aKeyName );
            aName.Append( '/' );
            aName.Append( String( pFrom[m] ) );
            pTo[m] = aName;
        }
        Sequence< Any > aValues( GetProperties( aSettingsKeys ) );
        const Any* pValue = aValues.getConstArray();
        for( int i = 0; i < aValues.getLength(); i++, pValue++ )
        {
            if( pValue->getValueTypeClass() == TypeClass_STRING )
            {
                const OUString* pLine = (const OUString*)pValue->getValue();
                if( !pLine->isEmpty() )
                    m_aSettings[ aKeyName ][ pFrom[i] ] = *pLine;
#if OSL_DEBUG_LEVEL > 2
                OSL_TRACE( "   \"%s\"=\"%.30s\"\n",
                         OUStringToOString( aKeys.getConstArray()[i], RTL_TEXTENCODING_ASCII_US ).getStr(),
                         OUStringToOString( *pLine, RTL_TEXTENCODING_ASCII_US ).getStr()
                         );
#endif
            }
        }
    }
}

/*
 *  SettingsConfigItem::getDefaultFont
 */

const OUString& SettingsConfigItem::getValue( const OUString& rGroup, const OUString& rKey ) const
{
    ::boost::unordered_map< OUString, SmallOUStrMap, rtl::OUStringHash >::const_iterator group = m_aSettings.find( rGroup );
    if( group == m_aSettings.end() || group->second.find( rKey ) == group->second.end() )
    {
        static OUString aEmpty;
        return aEmpty;
    }
    return group->second.find(rKey)->second;
}

/*
 *  SettingsConfigItem::setDefaultFont
 */

void SettingsConfigItem::setValue( const OUString& rGroup, const OUString& rKey, const OUString& rValue )
{
    bool bModified = m_aSettings[ rGroup ][ rKey ] != rValue;
    if( bModified )
    {
        m_aSettings[ rGroup ][ rKey ] = rValue;
        SetModified();
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
