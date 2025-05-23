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
#include "vbaparagraph.hxx"
#include <vbahelper/vbahelper.hxx>
#include <tools/diagnose_ex.h>
#include "vbarange.hxx"
#include <com/sun/star/lang/XServiceInfo.hpp>

using namespace ::ooo::vba;
using namespace ::com::sun::star;

SwVbaParagraph::SwVbaParagraph( const uno::Reference< ooo::vba::XHelperInterface >& rParent, const uno::Reference< uno::XComponentContext >& rContext, const uno::Reference< text::XTextDocument >& xDocument, const uno::Reference< text::XTextRange >& xTextRange ) throw ( uno::RuntimeException ) :
    SwVbaParagraph_BASE( rParent, rContext ), mxTextDocument( xDocument ), mxTextRange( xTextRange )
{
}

SwVbaParagraph::~SwVbaParagraph()
{
}

uno::Reference< word::XRange > SAL_CALL
SwVbaParagraph::getRange( ) throw ( uno::RuntimeException )
{
    return uno::Reference< word::XRange >( new SwVbaRange( this, mxContext, mxTextDocument, mxTextRange->getStart(), mxTextRange->getEnd(), mxTextRange->getText(), sal_True ) );
}

uno::Any SAL_CALL
SwVbaParagraph::getStyle( ) throw ( uno::RuntimeException )
{
    uno::Reference< word::XRange > xRange = getRange();
    return xRange->getStyle();
}

void SAL_CALL
SwVbaParagraph::setStyle( const uno::Any& style ) throw ( uno::RuntimeException )
{
    uno::Reference< word::XRange > xRange = getRange();
    xRange->setStyle( style );
}

rtl::OUString
SwVbaParagraph::getServiceImplName()
{
    return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SwVbaParagraph"));
}

uno::Sequence< rtl::OUString >
SwVbaParagraph::getServiceNames()
{
    static uno::Sequence< rtl::OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        aServiceNames.realloc( 1 );
        aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.word.Paragraph" ) );
    }
    return aServiceNames;
}

typedef ::cppu::WeakImplHelper2< container::XIndexAccess, container::XEnumerationAccess > ParagraphCollectionHelper_BASE;

class ParagraphCollectionHelper : public ParagraphCollectionHelper_BASE
{
private:
    uno::Reference< text::XTextDocument > mxTextDocument;

    uno::Reference< container::XEnumeration > getEnumeration() throw (uno::RuntimeException)
    {
        uno::Reference< container::XEnumerationAccess > xParEnumAccess( mxTextDocument->getText(), uno::UNO_QUERY_THROW );
        return xParEnumAccess->createEnumeration();
    }

public:
    ParagraphCollectionHelper( const uno::Reference< text::XTextDocument >& xDocument ) throw (uno::RuntimeException): mxTextDocument( xDocument )
    {
    }
    // XElementAccess
    virtual uno::Type SAL_CALL getElementType(  ) throw (uno::RuntimeException) { return  text::XTextRange::static_type(0); }
    virtual ::sal_Bool SAL_CALL hasElements(  ) throw (uno::RuntimeException) { return sal_True; }
    // XIndexAccess
    virtual ::sal_Int32 SAL_CALL getCount(  ) throw (uno::RuntimeException)
    {
        sal_Int32 nCount = 0;
        uno::Reference< container::XEnumeration > xParEnum = getEnumeration();
        while( xParEnum->hasMoreElements() )
        {
            uno::Reference< lang::XServiceInfo > xServiceInfo( xParEnum->nextElement(), uno::UNO_QUERY_THROW );
            if( xServiceInfo->supportsService( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.Paragraph") ) ) )
            {
                nCount++;
            }
        }
        return nCount;
    }
    virtual uno::Any SAL_CALL getByIndex( ::sal_Int32 Index ) throw (lang::IndexOutOfBoundsException, lang::WrappedTargetException, uno::RuntimeException )
    {
        if( Index < getCount() )
        {
            sal_Int32 nCount = 0;
            uno::Reference< container::XEnumeration > xParEnum = getEnumeration();
            while( xParEnum->hasMoreElements() )
            {
                uno::Reference< lang::XServiceInfo > xServiceInfo( xParEnum->nextElement(), uno::UNO_QUERY_THROW );
                if( xServiceInfo->supportsService( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("com.sun.star.text.Paragraph") ) ) )
                {
                    if( Index == nCount )
                        return uno::makeAny( xServiceInfo );
                    nCount++;
                }
            }
        }
        throw lang::IndexOutOfBoundsException();
    }
    // XEnumerationAccess
    virtual uno::Reference< container::XEnumeration > SAL_CALL createEnumeration(  ) throw (uno::RuntimeException)
    {
        return getEnumeration();
    }
};

SwVbaParagraphs::SwVbaParagraphs( const uno::Reference< XHelperInterface >& xParent, const uno::Reference< ::com::sun::star::uno::XComponentContext > & xContext, const uno::Reference< text::XTextDocument >& xDocument ) throw (uno::RuntimeException) : SwVbaParagraphs_BASE( xParent, xContext, new ParagraphCollectionHelper( xDocument ) ), mxTextDocument( xDocument )
{
}

// XEnumerationAccess
uno::Type
SwVbaParagraphs::getElementType() throw (uno::RuntimeException)
{
    return word::XParagraph::static_type(0);
}
uno::Reference< container::XEnumeration >
SwVbaParagraphs::createEnumeration() throw (uno::RuntimeException)
{
    uno::Reference< container::XEnumerationAccess > xEnumerationAccess( m_xIndexAccess, uno::UNO_QUERY_THROW );
    return xEnumerationAccess->createEnumeration();
}

uno::Any
SwVbaParagraphs::createCollectionObject( const css::uno::Any& aSource )
{
    uno::Reference< text::XTextRange > xTextRange( aSource, uno::UNO_QUERY_THROW );
    return uno::makeAny( uno::Reference< word::XParagraph >( new SwVbaParagraph( this, mxContext, mxTextDocument, xTextRange ) ) );
}

rtl::OUString
SwVbaParagraphs::getServiceImplName()
{
    return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SwVbaParagraphs"));
}

css::uno::Sequence<rtl::OUString>
SwVbaParagraphs::getServiceNames()
{
    static uno::Sequence< rtl::OUString > sNames;
    if ( sNames.getLength() == 0 )
    {
        sNames.realloc( 1 );
        sNames[0] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.word.Paragraphs") );
    }
    return sNames;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
