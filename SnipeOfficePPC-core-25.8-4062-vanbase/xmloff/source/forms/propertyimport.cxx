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


#include "propertyimport.hxx"

#include <sax/tools/converter.hxx>

#include <xmloff/xmlimp.hxx>
#include <xmloff/xmluconv.hxx>
#include <xmloff/nmspmap.hxx>
#include <osl/diagnose.h>
#include <comphelper/extract.hxx>
#include "callbacks.hxx"
#include "xmloff/xmlnmspe.hxx"
#include <tools/date.hxx>
#include <com/sun/star/util/Date.hpp>
#include <com/sun/star/util/Time.hpp>
#include <com/sun/star/util/DateTime.hpp>
#include <unotools/datetime.hxx>
#include <rtl/logfile.hxx>
#include <rtl/strbuf.hxx>

#if OSL_DEBUG_LEVEL > 0
    #include <osl/thread.h>
#endif

//.........................................................................
namespace xmloff
{
//.........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::beans;
    using namespace ::com::sun::star::xml;
    using ::com::sun::star::xml::sax::XAttributeList;

    // NO using namespace ...util !!!
    // need a tools Date/Time/DateTime below, which would conflict with the uno types then

#define TYPE_DATE       1
#define TYPE_TIME       2
#define TYPE_DATETIME   3

//=====================================================================
//= PropertyConversion
//=====================================================================
namespace
{
    //---------------------------------------------------------------------
    ::com::sun::star::util::Time lcl_getTime(double _nValue)
    {
        ::com::sun::star::util::Time aTime;
        sal_uInt32 nIntValue = sal_Int32(_nValue * 8640000);
        nIntValue *= 8640000;
        aTime.HundredthSeconds = (sal_uInt16)( nIntValue % 100 );
        nIntValue /= 100;
        aTime.Seconds = (sal_uInt16)( nIntValue % 60 );
        nIntValue /= 60;
        aTime.Minutes = (sal_uInt16)( nIntValue % 60 );
        nIntValue /= 60;
        OSL_ENSURE(nIntValue < 24, "lcl_getTime: more than a day?");
        aTime.Hours = static_cast< sal_uInt16 >( nIntValue );

        return aTime;
    }

    //---------------------------------------------------------------------
    static ::com::sun::star::util::Date lcl_getDate( double _nValue )
    {
        Date aToolsDate((sal_uInt32)_nValue);
        ::com::sun::star::util::Date aDate;
        ::utl::typeConvert(aToolsDate, aDate);
        return aDate;
    }
}

//---------------------------------------------------------------------
Any PropertyConversion::convertString( SvXMLImport& _rImporter, const ::com::sun::star::uno::Type& _rExpectedType,
    const ::rtl::OUString& _rReadCharacters, const SvXMLEnumMapEntry* _pEnumMap, const sal_Bool _bInvertBoolean )
{
    Any aReturn;
    sal_Bool bEnumAsInt = sal_False;
    switch (_rExpectedType.getTypeClass())
    {
        case TypeClass_BOOLEAN:     // sal_Bool
        {
            bool bValue;
        #if OSL_DEBUG_LEVEL > 0
            sal_Bool bSuccess =
        #endif
            ::sax::Converter::convertBool(bValue, _rReadCharacters);
            OSL_ENSURE(bSuccess,
                    ::rtl::OStringBuffer("PropertyConversion::convertString: could not convert \"").
                append(::rtl::OUStringToOString(_rReadCharacters, RTL_TEXTENCODING_ASCII_US)).
                append("\" into a boolean!").getStr());
            aReturn = ::cppu::bool2any(_bInvertBoolean ? !bValue : bValue);
        }
        break;
        case TypeClass_SHORT:       // sal_Int16
        case TypeClass_LONG:        // sal_Int32
            if (!_pEnumMap)
            {   // it's a real int32/16 property
                sal_Int32 nValue(0);
        #if OSL_DEBUG_LEVEL > 0
                sal_Bool bSuccess =
        #endif
                ::sax::Converter::convertNumber(nValue, _rReadCharacters);
                OSL_ENSURE(bSuccess,
                        ::rtl::OStringBuffer("PropertyConversion::convertString: could not convert \"").
                    append(::rtl::OUStringToOString(_rReadCharacters, RTL_TEXTENCODING_ASCII_US)).
                    append("\" into an integer!").getStr());
                if (TypeClass_SHORT == _rExpectedType.getTypeClass())
                    aReturn <<= (sal_Int16)nValue;
                else
                    aReturn <<= (sal_Int32)nValue;
                break;
            }
            bEnumAsInt = sal_True;
            // NO BREAK! handle it as enum
        case TypeClass_ENUM:
        {
            sal_uInt16 nEnumValue(0);
        #if OSL_DEBUG_LEVEL > 0
            sal_Bool bSuccess =
        #endif
            _rImporter.GetMM100UnitConverter().convertEnum(nEnumValue, _rReadCharacters, _pEnumMap);
            OSL_ENSURE(bSuccess, "PropertyConversion::convertString: could not convert to an enum value!");
            if (bEnumAsInt)
                if (TypeClass_SHORT == _rExpectedType.getTypeClass())
                    aReturn <<= (sal_Int16)nEnumValue;
                else
                    aReturn <<= (sal_Int32)nEnumValue;
            else
                aReturn = ::cppu::int2enum((sal_Int32)nEnumValue, _rExpectedType);
        }
        break;
        case TypeClass_HYPER:
        {
            OSL_FAIL("PropertyConversion::convertString: 64-bit integers not implemented yet!");
        }
        break;
        case TypeClass_DOUBLE:
        {
            double nValue;
        #if OSL_DEBUG_LEVEL > 0
            sal_Bool bSuccess =
        #endif
            ::sax::Converter::convertDouble(nValue, _rReadCharacters);
            OSL_ENSURE(bSuccess,
                    ::rtl::OStringBuffer("PropertyConversion::convertString: could not convert \"").
                append(::rtl::OUStringToOString(_rReadCharacters, RTL_TEXTENCODING_ASCII_US)).
                append("\" into a double!").getStr());
            aReturn <<= (double)nValue;
        }
        break;
        case TypeClass_STRING:
            aReturn <<= _rReadCharacters;
            break;
        case TypeClass_STRUCT:
        {
            sal_Int32 nType = 0;
            if ( _rExpectedType.equals( ::cppu::UnoType< ::com::sun::star::util::Date >::get() ) )
                nType = TYPE_DATE;
            else if ( _rExpectedType.equals( ::cppu::UnoType< ::com::sun::star::util::Time >::get() ) )
                nType = TYPE_TIME;
            else  if ( _rExpectedType.equals( ::cppu::UnoType< ::com::sun::star::util::DateTime >::get() ) )
                nType = TYPE_DATETIME;

            if ( nType )
            {
                // first extract the double
                double nValue = 0;
            #if OSL_DEBUG_LEVEL > 0
                sal_Bool bSuccess =
            #endif
                ::sax::Converter::convertDouble(nValue, _rReadCharacters);
                OSL_ENSURE(bSuccess,
                        ::rtl::OStringBuffer("PropertyConversion::convertString: could not convert \"").
                    append(::rtl::OUStringToOString(_rReadCharacters, RTL_TEXTENCODING_ASCII_US)).
                    append("\" into a double!").getStr());

                // then convert it into the target type
                switch (nType)
                {
                    case TYPE_DATE:
                    {
                        OSL_ENSURE(((sal_uInt32)nValue) - nValue == 0,
                            "PropertyConversion::convertString: a Date value with a fractional part?");
                        aReturn <<= lcl_getDate(nValue);
                    }
                    break;
                    case TYPE_TIME:
                    {
                        OSL_ENSURE(((sal_uInt32)nValue) == 0,
                            "PropertyConversion::convertString: a Time value with more than a fractional part?");
                        aReturn <<= lcl_getTime(nValue);
                    }
                    break;
                    case TYPE_DATETIME:
                    {
                        ::com::sun::star::util::Time aTime = lcl_getTime(nValue);
                        ::com::sun::star::util::Date aDate = lcl_getDate(nValue);

                        ::com::sun::star::util::DateTime aDateTime;
                        aDateTime.HundredthSeconds = aTime.HundredthSeconds;
                        aDateTime.Seconds = aTime.Seconds;
                        aDateTime.Minutes = aTime.Minutes;
                        aDateTime.Hours = aTime.Hours;
                        aDateTime.Day = aDate.Day;
                        aDateTime.Month = aDate.Month;
                        aDateTime.Year = aDate.Year;
                        aReturn <<= aDateTime;
                    }
                    break;
                }
            }
            else
                OSL_FAIL("PropertyConversion::convertString: unsupported property type!");
        }
        break;
        default:
            OSL_FAIL("PropertyConversion::convertString: invalid type class!");
    }

    return aReturn;
}

//---------------------------------------------------------------------
Type PropertyConversion::xmlTypeToUnoType( const ::rtl::OUString& _rType )
{
    Type aUnoType( ::getVoidCppuType() );

    static std::map< OUString, css::uno::Type > s_aTypeNameMap;
    if ( s_aTypeNameMap.empty() )
    {
        s_aTypeNameMap[ token::GetXMLToken( token::XML_BOOLEAN ) ] = ::getBooleanCppuType();
        s_aTypeNameMap[ token::GetXMLToken( token::XML_FLOAT )   ] = ::getCppuType( static_cast< double* >(NULL) );
        s_aTypeNameMap[ token::GetXMLToken( token::XML_STRING )  ] = ::getCppuType( static_cast< ::rtl::OUString* >(NULL) );
        s_aTypeNameMap[ token::GetXMLToken( token::XML_VOID )    ] = ::getVoidCppuType();
    }

    const std::map< OUString, css::uno::Type >::iterator aTypePos = s_aTypeNameMap.find( _rType );
    OSL_ENSURE( s_aTypeNameMap.end() != aTypePos, "PropertyConversion::xmlTypeToUnoType: invalid property name!" );
    if ( s_aTypeNameMap.end() != aTypePos )
        aUnoType = aTypePos->second;

    return aUnoType;
}

//=====================================================================
//= OPropertyImport
//=====================================================================
//---------------------------------------------------------------------
OPropertyImport::OPropertyImport(OFormLayerXMLImport_Impl& _rImport, sal_uInt16 _nPrefix, const ::rtl::OUString& _rName)
    :SvXMLImportContext(_rImport.getGlobalContext(), _nPrefix, _rName)
    ,m_rContext(_rImport)
    ,m_bTrackAttributes(sal_False)
{
}

//---------------------------------------------------------------------
SvXMLImportContext* OPropertyImport::CreateChildContext(sal_uInt16 _nPrefix, const ::rtl::OUString& _rLocalName,
    const Reference< XAttributeList >& _rxAttrList)
{
    if( token::IsXMLToken( _rLocalName, token::XML_PROPERTIES) )
    {
        return new OPropertyElementsContext( m_rContext.getGlobalContext(),
                                             _nPrefix, _rLocalName, this);
    }
    else
    {
        OSL_FAIL(::rtl::OStringBuffer("OPropertyImport::CreateChildContext: unknown sub element (only \"properties\" is recognized, but it is ").
            append(rtl::OUStringToOString(_rLocalName, RTL_TEXTENCODING_ASCII_US)).
            append(")!").getStr());
        return SvXMLImportContext::CreateChildContext(_nPrefix, _rLocalName, _rxAttrList);
    }
}

//---------------------------------------------------------------------
void OPropertyImport::StartElement(const Reference< XAttributeList >& _rxAttrList)
{
    OSL_ENSURE(_rxAttrList.is(), "OPropertyImport::StartElement: invalid attribute list!");
    const sal_Int32 nAttributeCount = _rxAttrList->getLength();

    // assume the 'worst' case: all attributes describe properties. This should save our property array
    // some reallocs
    m_aValues.reserve(nAttributeCount);

    const SvXMLNamespaceMap& rMap = m_rContext.getGlobalContext().GetNamespaceMap();
    sal_uInt16 nNamespace;
    ::rtl::OUString sLocalName;
    for (sal_Int16 i=0; i<nAttributeCount; ++i)
    {
        nNamespace = rMap.GetKeyByAttrName(_rxAttrList->getNameByIndex(i), &sLocalName);
        handleAttribute(nNamespace, sLocalName, _rxAttrList->getValueByIndex(i));

        if (m_bTrackAttributes)
            m_aEncounteredAttributes.insert(sLocalName);
    }

    // TODO: create PropertyValues for all the attributes which were not present, because they were implied
    // this is necessary as soon as we have properties where the XML default is different from the property
    // default
}

//---------------------------------------------------------------------
sal_Bool OPropertyImport::encounteredAttribute(const ::rtl::OUString& _rAttributeName) const
{
    OSL_ENSURE(m_bTrackAttributes, "OPropertyImport::encounteredAttribute: attribute tracking not enabled!");
    return m_aEncounteredAttributes.end() != m_aEncounteredAttributes.find(_rAttributeName);
}

//---------------------------------------------------------------------
void OPropertyImport::Characters(const ::rtl::OUString&
#if OSL_DEBUG_LEVEL > 0
_rChars
#endif
)
{
    // ignore them (should be whitespaces only)
    OSL_ENSURE(_rChars.trim().isEmpty(), "OPropertyImport::Characters: non-whitespace characters!");
}

//---------------------------------------------------------------------
bool OPropertyImport::handleAttribute(sal_uInt16 /*_nNamespaceKey*/, const ::rtl::OUString& _rLocalName, const ::rtl::OUString& _rValue)
{
    const OAttribute2Property::AttributeAssignment* pProperty = m_rContext.getAttributeMap().getAttributeTranslation(_rLocalName);
    if (pProperty)
    {
        // create and store a new PropertyValue
        PropertyValue aNewValue;
        aNewValue.Name = pProperty->sPropertyName;

        // convert the value string into the target type
        aNewValue.Value = PropertyConversion::convertString(m_rContext.getGlobalContext(), pProperty->aPropertyType, _rValue, pProperty->pEnumMap, pProperty->bInverseSemantics);
        implPushBackPropertyValue( aNewValue );
        return true;
    }
    if (!token::IsXMLToken(_rLocalName, token::XML_TYPE))  // xlink:type is valid but ignored for <form:form>
    {
#if OSL_DEBUG_LEVEL > 0
        ::rtl::OString sMessage( "OPropertyImport::handleAttribute: Can't handle the following:\n" );
        sMessage += ::rtl::OString( "  Attribute name: " );
        sMessage += ::rtl::OString( _rLocalName.getStr(), _rLocalName.getLength(), osl_getThreadTextEncoding() );
        sMessage += ::rtl::OString( "\n  value: " );
        sMessage += ::rtl::OString( _rValue.getStr(), _rValue.getLength(), osl_getThreadTextEncoding() );
        OSL_FAIL( sMessage.getStr() );
#endif
        return false;
    }
    return true;
}

//=====================================================================
//= OPropertyElementsContext
//=====================================================================
//---------------------------------------------------------------------
OPropertyElementsContext::OPropertyElementsContext(SvXMLImport& _rImport, sal_uInt16 _nPrefix, const ::rtl::OUString& _rName,
        const OPropertyImportRef& _rPropertyImporter)
    :SvXMLImportContext(_rImport, _nPrefix, _rName)
    ,m_xPropertyImporter(_rPropertyImporter)
{
}

//---------------------------------------------------------------------
SvXMLImportContext* OPropertyElementsContext::CreateChildContext(sal_uInt16 _nPrefix, const ::rtl::OUString& _rLocalName,
    const Reference< XAttributeList >&)
{
    if( token::IsXMLToken( _rLocalName, token::XML_PROPERTY ) )
    {
        return new OSinglePropertyContext(GetImport(), _nPrefix, _rLocalName, m_xPropertyImporter);
    }
    else if( token::IsXMLToken( _rLocalName, token::XML_LIST_PROPERTY ) )
    {
        return new OListPropertyContext( GetImport(), _nPrefix, _rLocalName, m_xPropertyImporter );
    }
    else
    {
        OSL_FAIL(::rtl::OStringBuffer("OPropertyElementsContext::CreateChildContext: unknown child element (\"").
            append(::rtl::OUStringToOString(_rLocalName, RTL_TEXTENCODING_ASCII_US)).
            append("\")!").getStr());
        return new SvXMLImportContext(GetImport(), _nPrefix, _rLocalName);
    }
}

#if OSL_DEBUG_LEVEL > 0
    //---------------------------------------------------------------------
    void OPropertyElementsContext::StartElement(const Reference< XAttributeList >& _rxAttrList)
    {
        OSL_ENSURE(0 == _rxAttrList->getLength(), "OPropertyElementsContext::StartElement: the form:properties element should not have attributes!");
        SvXMLImportContext::StartElement(_rxAttrList);
    }

    //---------------------------------------------------------------------
    void OPropertyElementsContext::Characters(const ::rtl::OUString& _rChars)
    {
        OSL_ENSURE(0 == _rChars.trim(), "OPropertyElementsContext::Characters: non-whitespace characters detected!");
        SvXMLImportContext::Characters(_rChars);
    }

#endif

//=====================================================================
//= OSinglePropertyContext
//=====================================================================
//---------------------------------------------------------------------
OSinglePropertyContext::OSinglePropertyContext(SvXMLImport& _rImport, sal_uInt16 _nPrefix, const ::rtl::OUString& _rName,
        const OPropertyImportRef& _rPropertyImporter)
    :SvXMLImportContext(_rImport, _nPrefix, _rName)
    ,m_xPropertyImporter(_rPropertyImporter)
{
}

//---------------------------------------------------------------------
SvXMLImportContext* OSinglePropertyContext::CreateChildContext(sal_uInt16 _nPrefix, const ::rtl::OUString& _rLocalName,
        const Reference< XAttributeList >&)
{
    OSL_FAIL(::rtl::OStringBuffer("OSinglePropertyContext::CreateChildContext: unknown child element (\"").
        append(::rtl::OUStringToOString(_rLocalName, RTL_TEXTENCODING_ASCII_US)).
        append("\")!").getStr());
    return new SvXMLImportContext(GetImport(), _nPrefix, _rLocalName);
}

//---------------------------------------------------------------------
void OSinglePropertyContext::StartElement(const Reference< XAttributeList >& _rxAttrList)
{
    ::com::sun::star::beans::PropertyValue aPropValue;      // the property the instance imports currently
    ::com::sun::star::uno::Type aPropType;          // the type of the property the instance imports currently

    ::rtl::OUString sType, sValue;
    const SvXMLNamespaceMap& rMap = GetImport().GetNamespaceMap();
    const sal_Int16 nAttrCount = _rxAttrList.is() ? _rxAttrList->getLength() : 0;
    for( sal_Int16 i=0; i < nAttrCount; i++ )
    {
        const ::rtl::OUString& rAttrName = _rxAttrList->getNameByIndex( i );

        ::rtl::OUString aLocalName;
        sal_uInt16 nPrefix =
            rMap.GetKeyByAttrName( rAttrName,
                                                            &aLocalName );
        if( XML_NAMESPACE_FORM == nPrefix )
        {
            if( token::IsXMLToken( aLocalName, token::XML_PROPERTY_NAME ) )
                aPropValue.Name = _rxAttrList->getValueByIndex( i );

        }
        else if( XML_NAMESPACE_OFFICE == nPrefix )
        {
            if( token::IsXMLToken( aLocalName, token::XML_VALUE_TYPE ) )
                sType = _rxAttrList->getValueByIndex( i );
            else if( token::IsXMLToken( aLocalName,
                                        token::XML_VALUE ) ||
                        token::IsXMLToken( aLocalName,
                                        token::XML_BOOLEAN_VALUE ) ||
                     token::IsXMLToken( aLocalName,
                                        token::XML_STRING_VALUE ) )
                sValue = _rxAttrList->getValueByIndex( i );
        }
    }

    // the name of the property
    OSL_ENSURE(!aPropValue.Name.isEmpty(), "OSinglePropertyContext::StartElement: invalid property name!");

    // needs to be translated into a ::com::sun::star::uno::Type
    aPropType = PropertyConversion::xmlTypeToUnoType( sType );
    if( TypeClass_VOID == aPropType.getTypeClass() )
    {
        aPropValue.Value = Any();
    }
    else
    {
        aPropValue.Value =
            PropertyConversion::convertString(GetImport(), aPropType,
                                           sValue);
    }

    // now that we finally have our property value, add it to our parent object
    if( !aPropValue.Name.isEmpty() )
        m_xPropertyImporter->implPushBackGenericPropertyValue(aPropValue);
}

//=====================================================================
//= OListPropertyContext
//=====================================================================
//---------------------------------------------------------------------
OListPropertyContext::OListPropertyContext( SvXMLImport& _rImport, sal_uInt16 _nPrefix, const ::rtl::OUString& _rName,
    const OPropertyImportRef& _rPropertyImporter )
    :SvXMLImportContext( _rImport, _nPrefix, _rName )
    ,m_xPropertyImporter( _rPropertyImporter )
{
}

//---------------------------------------------------------------------
void OListPropertyContext::StartElement( const Reference< XAttributeList >& _rxAttrList )
{
    sal_Int32 nAttributeCount = _rxAttrList->getLength();

    sal_uInt16 nNamespace;
    ::rtl::OUString sAttributeName;
    const SvXMLNamespaceMap& rMap = GetImport().GetNamespaceMap();
    for ( sal_Int16 i = 0; i < nAttributeCount; ++i )
    {
        nNamespace = rMap.GetKeyByAttrName( _rxAttrList->getNameByIndex( i ), &sAttributeName );
        if  (   ( XML_NAMESPACE_FORM == nNamespace )
            &&  ( token::IsXMLToken( sAttributeName, token::XML_PROPERTY_NAME ) )
            )
        {
            m_sPropertyName = _rxAttrList->getValueByIndex( i );
        }
        else if (   ( XML_NAMESPACE_OFFICE == nNamespace )
                &&  ( token::IsXMLToken( sAttributeName, token::XML_VALUE_TYPE ) )
                )
        {
            m_sPropertyType = _rxAttrList->getValueByIndex( i );
        }
        else
        {
            OSL_FAIL( ::rtl::OStringBuffer( "OListPropertyContext::StartElement: unknown child element (\"").
                append(rtl::OUStringToOString(sAttributeName, RTL_TEXTENCODING_ASCII_US)).
                append("\")!").getStr() );
        }
    }
}

//---------------------------------------------------------------------
void OListPropertyContext::EndElement()
{
    OSL_ENSURE( !m_sPropertyName.isEmpty() && !m_sPropertyType.isEmpty(),
        "OListPropertyContext::EndElement: no property name or type!" );

    if ( m_sPropertyName.isEmpty() || m_sPropertyType.isEmpty() )
        return;

    Sequence< Any > aListElements( m_aListValues.size() );
    Any* pListElement = aListElements.getArray();
    com::sun::star::uno::Type aType = PropertyConversion::xmlTypeToUnoType( m_sPropertyType );
    for (   ::std::vector< ::rtl::OUString >::const_iterator values = m_aListValues.begin();
            values != m_aListValues.end();
            ++values, ++pListElement
        )
    {
        *pListElement = PropertyConversion::convertString( GetImport(), aType, *values );
    }

    PropertyValue aSequenceValue;
    aSequenceValue.Name = m_sPropertyName;
    aSequenceValue.Value <<= aListElements;

    m_xPropertyImporter->implPushBackGenericPropertyValue( aSequenceValue );
}

//---------------------------------------------------------------------
SvXMLImportContext* OListPropertyContext::CreateChildContext( sal_uInt16 _nPrefix, const ::rtl::OUString& _rLocalName, const Reference< XAttributeList >& /*_rxAttrList*/ )
{
    if ( token::IsXMLToken( _rLocalName, token::XML_LIST_VALUE ) )
    {
        m_aListValues.resize( m_aListValues.size() + 1 );
        return new OListValueContext( GetImport(), _nPrefix, _rLocalName, *m_aListValues.rbegin() );
    }
    else
    {
        OSL_FAIL( ::rtl::OStringBuffer("OListPropertyContext::CreateChildContext: unknown child element (\"").
            append(rtl::OUStringToOString(_rLocalName.getStr(), RTL_TEXTENCODING_ASCII_US)).
            append("\")!").getStr() );
        return new SvXMLImportContext( GetImport(), _nPrefix, _rLocalName );
    }
}

//=====================================================================
//= OListValueContext
//=====================================================================
//---------------------------------------------------------------------
OListValueContext::OListValueContext( SvXMLImport& _rImport, sal_uInt16 _nPrefix, const ::rtl::OUString& _rName, ::rtl::OUString& _rListValueHolder )
    :SvXMLImportContext( _rImport, _nPrefix, _rName )
    ,m_rListValueHolder( _rListValueHolder )
{
}

//---------------------------------------------------------------------
void OListValueContext::StartElement( const Reference< XAttributeList >& _rxAttrList )
{
    const sal_Int32 nAttributeCount = _rxAttrList->getLength();

    sal_uInt16 nNamespace;
    ::rtl::OUString sAttributeName;
    const SvXMLNamespaceMap& rMap = GetImport().GetNamespaceMap();
    for ( sal_Int16 i = 0; i < nAttributeCount; ++i )
    {
        nNamespace = rMap.GetKeyByAttrName( _rxAttrList->getNameByIndex( i ), &sAttributeName );
        if ( XML_NAMESPACE_OFFICE == nNamespace )
        {
            if  (   token::IsXMLToken( sAttributeName, token::XML_VALUE )
               ||   token::IsXMLToken( sAttributeName, token::XML_STRING_VALUE )
               ||   token::IsXMLToken( sAttributeName, token::XML_BOOLEAN_VALUE )
                )
            {
                m_rListValueHolder = _rxAttrList->getValueByIndex( i );
                continue;
            }
        }

        OSL_FAIL( ::rtl::OStringBuffer( "OListValueContext::StartElement: unknown child element (\"").
            append(rtl::OUStringToOString(sAttributeName, RTL_TEXTENCODING_ASCII_US)).
            append("\")!").getStr() );
    }
}

//.........................................................................
}   // namespace xmloff
//.........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
