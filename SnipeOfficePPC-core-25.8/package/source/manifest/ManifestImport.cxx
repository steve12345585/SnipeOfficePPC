/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

// MARKER( update_precomp.py ): autogen include statement, do not remove
#include <ManifestImport.hxx>
#include <ManifestDefines.hxx>
#include <sax/tools/converter.hxx>
#include <com/sun/star/xml/sax/XAttributeList.hpp>
#include <com/sun/star/xml/crypto/DigestID.hpp>
#include <com/sun/star/xml/crypto/CipherID.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>

using namespace com::sun::star::uno;
using namespace com::sun::star::beans;
using namespace com::sun::star;
using namespace std;

using ::rtl::OUString;

// ---------------------------------------------------
ManifestImport::ManifestImport( vector < Sequence < PropertyValue > > & rNewManVector )
: bIgnoreEncryptData    ( sal_False )
, nDerivedKeySize( 0 )
, rManVector ( rNewManVector )

, sFileEntryElement     ( RTL_CONSTASCII_USTRINGPARAM ( ELEMENT_FILE_ENTRY ) )
, sManifestElement      ( RTL_CONSTASCII_USTRINGPARAM ( ELEMENT_MANIFEST ) )
, sEncryptionDataElement( RTL_CONSTASCII_USTRINGPARAM ( ELEMENT_ENCRYPTION_DATA ) )
, sAlgorithmElement ( RTL_CONSTASCII_USTRINGPARAM ( ELEMENT_ALGORITHM ) )
, sStartKeyAlgElement   ( RTL_CONSTASCII_USTRINGPARAM ( ELEMENT_START_KEY_GENERATION ) )
, sKeyDerivationElement( RTL_CONSTASCII_USTRINGPARAM ( ELEMENT_KEY_DERIVATION ) )

, sCdataAttribute               ( RTL_CONSTASCII_USTRINGPARAM ( ATTRIBUTE_CDATA ) )
, sMediaTypeAttribute           ( RTL_CONSTASCII_USTRINGPARAM ( ATTRIBUTE_MEDIA_TYPE ) )
, sVersionAttribute             ( RTL_CONSTASCII_USTRINGPARAM ( ATTRIBUTE_VERSION ) )
, sFullPathAttribute            ( RTL_CONSTASCII_USTRINGPARAM ( ATTRIBUTE_FULL_PATH ) )
, sSizeAttribute                ( RTL_CONSTASCII_USTRINGPARAM ( ATTRIBUTE_SIZE ) )
, sSaltAttribute                ( RTL_CONSTASCII_USTRINGPARAM ( ATTRIBUTE_SALT ) )
, sInitialisationVectorAttribute ( RTL_CONSTASCII_USTRINGPARAM ( ATTRIBUTE_INITIALISATION_VECTOR ) )
, sIterationCountAttribute      ( RTL_CONSTASCII_USTRINGPARAM ( ATTRIBUTE_ITERATION_COUNT ) )
, sKeySizeAttribute             ( RTL_CONSTASCII_USTRINGPARAM ( ATTRIBUTE_KEY_SIZE ) )
, sAlgorithmNameAttribute       ( RTL_CONSTASCII_USTRINGPARAM ( ATTRIBUTE_ALGORITHM_NAME ) )
, sStartKeyAlgNameAttribute     ( RTL_CONSTASCII_USTRINGPARAM ( ATTRIBUTE_START_KEY_GENERATION_NAME ) )
, sKeyDerivationNameAttribute   ( RTL_CONSTASCII_USTRINGPARAM ( ATTRIBUTE_KEY_DERIVATION_NAME ) )
, sChecksumAttribute            ( RTL_CONSTASCII_USTRINGPARAM ( ATTRIBUTE_CHECKSUM ) )
, sChecksumTypeAttribute        ( RTL_CONSTASCII_USTRINGPARAM ( ATTRIBUTE_CHECKSUM_TYPE ) )

, sFullPathProperty             ( RTL_CONSTASCII_USTRINGPARAM ( "FullPath" ) )
, sMediaTypeProperty            ( RTL_CONSTASCII_USTRINGPARAM ( "MediaType" ) )
, sVersionProperty              ( RTL_CONSTASCII_USTRINGPARAM ( "Version" ) )
, sIterationCountProperty       ( RTL_CONSTASCII_USTRINGPARAM ( "IterationCount" ) )
, sDerivedKeySizeProperty       ( RTL_CONSTASCII_USTRINGPARAM ( "DerivedKeySize" ) )
, sSaltProperty                 ( RTL_CONSTASCII_USTRINGPARAM ( "Salt" ) )
, sInitialisationVectorProperty ( RTL_CONSTASCII_USTRINGPARAM ( "InitialisationVector" ) )
, sSizeProperty                 ( RTL_CONSTASCII_USTRINGPARAM ( "Size" ) )
, sDigestProperty               ( RTL_CONSTASCII_USTRINGPARAM ( "Digest" ) )
, sEncryptionAlgProperty        ( RTL_CONSTASCII_USTRINGPARAM ( "EncryptionAlgorithm" ) )
, sStartKeyAlgProperty          ( RTL_CONSTASCII_USTRINGPARAM ( "StartKeyAlgorithm" ) )
, sDigestAlgProperty            ( RTL_CONSTASCII_USTRINGPARAM ( "DigestAlgorithm" ) )

, sWhiteSpace                   ( RTL_CONSTASCII_USTRINGPARAM ( " " ) )

, sSHA256_URL                   ( RTL_CONSTASCII_USTRINGPARAM ( SHA256_URL ) )
, sSHA1_Name                    ( RTL_CONSTASCII_USTRINGPARAM ( SHA1_NAME ) )
, sSHA1_URL                     ( RTL_CONSTASCII_USTRINGPARAM ( SHA1_URL ) )

, sSHA256_1k_URL                ( RTL_CONSTASCII_USTRINGPARAM ( SHA256_1K_URL ) )
, sSHA1_1k_Name                 ( RTL_CONSTASCII_USTRINGPARAM ( SHA1_1K_NAME ) )
, sSHA1_1k_URL                  ( RTL_CONSTASCII_USTRINGPARAM ( SHA1_1K_URL ) )

, sBlowfish_Name                ( RTL_CONSTASCII_USTRINGPARAM ( BLOWFISH_NAME ) )
, sBlowfish_URL                 ( RTL_CONSTASCII_USTRINGPARAM ( BLOWFISH_URL ) )
, sAES128_URL                   ( RTL_CONSTASCII_USTRINGPARAM ( AES128_URL ) )
, sAES192_URL                   ( RTL_CONSTASCII_USTRINGPARAM ( AES192_URL ) )
, sAES256_URL                   ( RTL_CONSTASCII_USTRINGPARAM ( AES256_URL ) )

, sPBKDF2_Name                  ( RTL_CONSTASCII_USTRINGPARAM ( PBKDF2_NAME ) )
, sPBKDF2_URL                   ( RTL_CONSTASCII_USTRINGPARAM ( PBKDF2_URL ) )
{
    aStack.reserve( 10 );
}

// ---------------------------------------------------
ManifestImport::~ManifestImport ( void )
{
}

// ---------------------------------------------------
void SAL_CALL ManifestImport::startDocument(  )
        throw( xml::sax::SAXException, uno::RuntimeException )
{
}

// ---------------------------------------------------
void SAL_CALL ManifestImport::endDocument(  )
        throw( xml::sax::SAXException, uno::RuntimeException )
{
}

void ManifestImport::doFileEntry(StringHashMap &rConvertedAttribs)
        throw( uno::RuntimeException )
{
    aSequence.resize(PKG_SIZE_ENCR_MNFST);

    aSequence[PKG_MNFST_FULLPATH].Name = sFullPathProperty;
    aSequence[PKG_MNFST_FULLPATH].Value <<= rConvertedAttribs[sFullPathAttribute];
    aSequence[PKG_MNFST_MEDIATYPE].Name = sMediaTypeProperty;
    aSequence[PKG_MNFST_MEDIATYPE].Value <<= rConvertedAttribs[sMediaTypeAttribute];

    OUString sVersion = rConvertedAttribs[sVersionAttribute];
    if ( sVersion.getLength() )
    {
        aSequence[PKG_MNFST_VERSION].Name = sVersionProperty;
        aSequence[PKG_MNFST_VERSION].Value <<= sVersion;
    }

    OUString sSize = rConvertedAttribs[sSizeAttribute];
    if ( sSize.getLength() )
    {
        sal_Int32 nSize = sSize.toInt32();
        aSequence[PKG_MNFST_UCOMPSIZE].Name = sSizeProperty;
        aSequence[PKG_MNFST_UCOMPSIZE].Value <<= nSize;
    }
}

void ManifestImport::doEncryptionData(StringHashMap &rConvertedAttribs)
        throw( uno::RuntimeException )
{
    // If this element exists, then this stream is encrypted and we need
    // to import the initialisation vector, salt and iteration count used
    nDerivedKeySize = 0;
    OUString aString = rConvertedAttribs[sChecksumTypeAttribute];
    if ( !bIgnoreEncryptData )
    {
        if ( aString.equals( sSHA1_1k_Name ) || aString.equals( sSHA1_1k_URL ) )
        {
            aSequence[PKG_MNFST_DIGESTALG].Name = sDigestAlgProperty;
            aSequence[PKG_MNFST_DIGESTALG].Value <<= xml::crypto::DigestID::SHA1_1K;
        }
        else if ( aString.equals( sSHA256_1k_URL ) )
        {
            aSequence[PKG_MNFST_DIGESTALG].Name = sDigestAlgProperty;
            aSequence[PKG_MNFST_DIGESTALG].Value <<= xml::crypto::DigestID::SHA256_1K;
        }
        else
            bIgnoreEncryptData = sal_True;

        if ( !bIgnoreEncryptData )
        {
            aString = rConvertedAttribs[sChecksumAttribute];
            uno::Sequence < sal_Int8 > aDecodeBuffer;
            ::sax::Converter::decodeBase64(aDecodeBuffer, aString);
            aSequence[PKG_MNFST_DIGEST].Name = sDigestProperty;
            aSequence[PKG_MNFST_DIGEST].Value <<= aDecodeBuffer;
        }
    }
}

void ManifestImport::doAlgorithm(StringHashMap &rConvertedAttribs)
        throw( uno::RuntimeException )
{
    if ( !bIgnoreEncryptData )
    {
        OUString aString = rConvertedAttribs[sAlgorithmNameAttribute];
        if ( aString.equals( sBlowfish_Name ) || aString.equals( sBlowfish_URL ) )
        {
            aSequence[PKG_MNFST_ENCALG].Name = sEncryptionAlgProperty;
            aSequence[PKG_MNFST_ENCALG].Value <<= xml::crypto::CipherID::BLOWFISH_CFB_8;
        }
        else if ( aString.equals( sAES256_URL ) )
        {
            aSequence[PKG_MNFST_ENCALG].Name = sEncryptionAlgProperty;
            aSequence[PKG_MNFST_ENCALG].Value <<= xml::crypto::CipherID::AES_CBC_W3C_PADDING;
            OSL_ENSURE( !nDerivedKeySize || nDerivedKeySize == 32, "Unexpected derived key length!" );
            nDerivedKeySize = 32;
        }
        else if ( aString.equals( sAES192_URL ) )
        {
            aSequence[PKG_MNFST_ENCALG].Name = sEncryptionAlgProperty;
            aSequence[PKG_MNFST_ENCALG].Value <<= xml::crypto::CipherID::AES_CBC_W3C_PADDING;
            OSL_ENSURE( !nDerivedKeySize || nDerivedKeySize == 24, "Unexpected derived key length!" );
            nDerivedKeySize = 24;
        }
        else if ( aString.equals( sAES128_URL ) )
        {
            aSequence[PKG_MNFST_ENCALG].Name = sEncryptionAlgProperty;
            aSequence[PKG_MNFST_ENCALG].Value <<= xml::crypto::CipherID::AES_CBC_W3C_PADDING;
            OSL_ENSURE( !nDerivedKeySize || nDerivedKeySize == 16, "Unexpected derived key length!" );
            nDerivedKeySize = 16;
        }
        else
            bIgnoreEncryptData = sal_True;

        if ( !bIgnoreEncryptData )
        {
            aString = rConvertedAttribs[sInitialisationVectorAttribute];
            uno::Sequence < sal_Int8 > aDecodeBuffer;
            ::sax::Converter::decodeBase64(aDecodeBuffer, aString);
            aSequence[PKG_MNFST_INIVECTOR].Name = sInitialisationVectorProperty;
            aSequence[PKG_MNFST_INIVECTOR].Value <<= aDecodeBuffer;
        }
    }
}

void ManifestImport::doKeyDerivation(StringHashMap &rConvertedAttribs)
        throw( uno::RuntimeException )
{
    if ( !bIgnoreEncryptData )
    {
        OUString aString = rConvertedAttribs[sKeyDerivationNameAttribute];
        if ( aString.equals( sPBKDF2_Name ) || aString.equals( sPBKDF2_URL ) )
        {
            aString = rConvertedAttribs[sSaltAttribute];
            uno::Sequence < sal_Int8 > aDecodeBuffer;
            ::sax::Converter::decodeBase64(aDecodeBuffer, aString);
            aSequence[PKG_MNFST_SALT].Name = sSaltProperty;
            aSequence[PKG_MNFST_SALT].Value <<= aDecodeBuffer;

            aString = rConvertedAttribs[sIterationCountAttribute];
            aSequence[PKG_MNFST_ITERATION].Name = sIterationCountProperty;
            aSequence[PKG_MNFST_ITERATION].Value <<= aString.toInt32();

            aString = rConvertedAttribs[sKeySizeAttribute];
            if ( aString.getLength() )
            {
                sal_Int32 nKey = aString.toInt32();
                OSL_ENSURE( !nDerivedKeySize || nKey == nDerivedKeySize , "Provided derived key length differs from the expected one!" );
                nDerivedKeySize = nKey;
            }
            else if ( !nDerivedKeySize )
                nDerivedKeySize = 16;
            else if ( nDerivedKeySize != 16 )
                OSL_ENSURE( sal_False, "Default derived key length differs from the expected one!" );

            aSequence[PKG_MNFST_DERKEYSIZE].Name = sDerivedKeySizeProperty;
            aSequence[PKG_MNFST_DERKEYSIZE].Value <<= nDerivedKeySize;
        }
        else
            bIgnoreEncryptData = sal_True;
    }
}

void ManifestImport::doStartKeyAlg(StringHashMap &rConvertedAttribs)
        throw( uno::RuntimeException )
{
    OUString aString = rConvertedAttribs[sStartKeyAlgNameAttribute];
    if ( aString.equals( sSHA256_URL ) )
    {
        aSequence[PKG_MNFST_STARTALG].Name = sStartKeyAlgProperty;
        aSequence[PKG_MNFST_STARTALG].Value <<= xml::crypto::DigestID::SHA256;
    }
    else if ( aString.equals( sSHA1_Name ) || aString.equals( sSHA1_URL ) )
    {
        aSequence[PKG_MNFST_STARTALG].Name = sStartKeyAlgProperty;
        aSequence[PKG_MNFST_STARTALG].Value <<= xml::crypto::DigestID::SHA1;
    }
    else
        bIgnoreEncryptData = sal_True;
}

// ---------------------------------------------------
void SAL_CALL ManifestImport::startElement( const OUString& aName, const uno::Reference< xml::sax::XAttributeList >& xAttribs )
        throw( xml::sax::SAXException, uno::RuntimeException )
{
    StringHashMap aConvertedAttribs;
    ::rtl::OUString aConvertedName = PushNameAndNamespaces( aName, xAttribs, aConvertedAttribs );

    size_t nLevel = aStack.size();

    assert(nLevel >= 1);

    switch (nLevel)
    {
        case 1:
        {
            if (!aConvertedName.equalsAscii(ELEMENT_MANIFEST)) //manifest:manifest
                aStack.back().m_bValid = false;
            break;
        }
        case 2:
        {
            if (aConvertedName == sFileEntryElement) //manifest:file-entry
                doFileEntry(aConvertedAttribs);
            else
                aStack.back().m_bValid = false;
            break;
        }
        case 3:
        {
            ManifestStack::reverse_iterator aIter = aStack.rbegin();
            ++aIter;

            if (!aIter->m_bValid)
                aStack.back().m_bValid = false;
            else if (aConvertedName.equals(sEncryptionDataElement))   //manifest:encryption-data
                doEncryptionData(aConvertedAttribs);
            else
                aStack.back().m_bValid = false;
            break;
        }
        case 4:
        {
            ManifestStack::reverse_iterator aIter = aStack.rbegin();
            ++aIter;

            if (!aIter->m_bValid)
                aStack.back().m_bValid = false;
            else if (aConvertedName.equals(sAlgorithmElement))   //manifest:algorithm,
                doAlgorithm(aConvertedAttribs);
            else if (aConvertedName.equals(sKeyDerivationElement)) //manifest:key-derivation,
                doKeyDerivation(aConvertedAttribs);
            else if (aConvertedName.equals(sStartKeyAlgElement))   //manifest:start-key-generation
                doStartKeyAlg(aConvertedAttribs);
            else
                aStack.back().m_bValid = false;
            break;
        }
        default:
            aStack.back().m_bValid = false;
            break;
    }
}

namespace
{
    bool isEmpty(const com::sun::star::beans::PropertyValue &rProp)
    {
        return rProp.Name.isEmpty();
    }
}

// ---------------------------------------------------
void SAL_CALL ManifestImport::endElement( const OUString& aName )
    throw( xml::sax::SAXException, uno::RuntimeException )
{
    ::rtl::OUString aConvertedName = ConvertName( aName );
    if ( !aStack.empty() && aStack.rbegin()->m_aConvertedName.equals( aConvertedName ) )
    {
        if ( aConvertedName.equals( sFileEntryElement ) && aStack.back().m_bValid )
        {
            com::sun::star::beans::PropertyValue aEmpty;
            aSequence.erase(std::remove_if(aSequence.begin(), aSequence.end(),
                isEmpty), aSequence.end());

            bIgnoreEncryptData = sal_False;
            rManVector.push_back ( aSequence.getAsConstList() );

            aSequence.clear();
        }

        aStack.pop_back();
    }
}

// ---------------------------------------------------
void SAL_CALL ManifestImport::characters( const OUString& /*aChars*/ )
        throw( xml::sax::SAXException, uno::RuntimeException )
{
}

// ---------------------------------------------------
void SAL_CALL ManifestImport::ignorableWhitespace( const OUString& /*aWhitespaces*/ )
        throw( xml::sax::SAXException, uno::RuntimeException )
{
}

// ---------------------------------------------------
void SAL_CALL ManifestImport::processingInstruction( const OUString& /*aTarget*/, const OUString& /*aData*/ )
        throw( xml::sax::SAXException, uno::RuntimeException )
{
}

// ---------------------------------------------------
void SAL_CALL ManifestImport::setDocumentLocator( const uno::Reference< xml::sax::XLocator >& /*xLocator*/ )
        throw( xml::sax::SAXException, uno::RuntimeException )
{
}

// ---------------------------------------------------
::rtl::OUString ManifestImport::PushNameAndNamespaces( const ::rtl::OUString& aName, const uno::Reference< xml::sax::XAttributeList >& xAttribs, StringHashMap& o_aConvertedAttribs )
{
    StringHashMap aNamespaces;
    ::std::vector< ::std::pair< ::rtl::OUString, ::rtl::OUString > > aAttribsStrs;

    if ( xAttribs.is() )
    {
        sal_Int16 nAttrCount = xAttribs.is() ? xAttribs->getLength() : 0;
        aAttribsStrs.reserve( nAttrCount );

        for( sal_Int16 nInd = 0; nInd < nAttrCount; nInd++ )
        {
            ::rtl::OUString aAttrName = xAttribs->getNameByIndex( nInd );
            ::rtl::OUString aAttrValue = xAttribs->getValueByIndex( nInd );
            if ( aAttrName.getLength() >= 5
              && aAttrName.compareToAscii( "xmlns", 5 ) == 0
              && ( aAttrName.getLength() == 5 || aAttrName.getStr()[5] == ( sal_Unicode )':' ) )
            {
                // this is a namespace declaration
                ::rtl::OUString aNsName( ( aAttrName.getLength() == 5 ) ? ::rtl::OUString() : aAttrName.copy( 6 ) );
                aNamespaces[aNsName] = aAttrValue;
            }
            else
            {
                // this is no namespace declaration
                aAttribsStrs.push_back( pair< ::rtl::OUString, ::rtl::OUString >( aAttrName, aAttrValue ) );
            }
        }
    }

    ::rtl::OUString aConvertedName = ConvertNameWithNamespace( aName, aNamespaces );
    if ( !aConvertedName.getLength() )
        aConvertedName = ConvertName( aName );

    aStack.push_back( ManifestScopeEntry( aConvertedName, aNamespaces ) );

    for ( sal_uInt16 nInd = 0; nInd < aAttribsStrs.size(); nInd++ )
    {
        // convert the attribute names on filling
        o_aConvertedAttribs[ConvertName( aAttribsStrs[nInd].first )] = aAttribsStrs[nInd].second;
    }

    return aConvertedName;
}

// ---------------------------------------------------
::rtl::OUString ManifestImport::ConvertNameWithNamespace( const ::rtl::OUString& aName, const StringHashMap& aNamespaces )
{
    ::rtl::OUString aNsAlias;
    ::rtl::OUString aPureName = aName;

    sal_Int32 nInd = aName.indexOf( ( sal_Unicode )':' );
    if ( nInd != -1 && nInd < aName.getLength() )
    {
        aNsAlias = aName.copy( 0, nInd );
        aPureName = aName.copy( nInd + 1 );
    }

    ::rtl::OUString aResult;

    StringHashMap::const_iterator aIter = aNamespaces.find( aNsAlias );
    if ( aIter != aNamespaces.end()
      && ( aIter->second == MANIFEST_NAMESPACE || aIter->second == MANIFEST_OASIS_NAMESPACE ) )
    {
        // no check for manifest.xml consistency currently since the old versions have supported inconsistent documents as well
        aResult = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( MANIFEST_NSPREFIX ) );
        aResult += aPureName;
    }

    return aResult;
}

// ---------------------------------------------------
::rtl::OUString ManifestImport::ConvertName( const ::rtl::OUString& aName )
{
    ::rtl::OUString aConvertedName;
    for ( ManifestStack::reverse_iterator aIter = aStack.rbegin(); !aConvertedName.getLength() && aIter != aStack.rend(); ++aIter )
    {
        if ( !aIter->m_aNamespaces.empty() )
            aConvertedName = ConvertNameWithNamespace( aName, aIter->m_aNamespaces );
    }

    if ( !aConvertedName.getLength() )
        aConvertedName = aName;

    return aConvertedName;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
