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

#include "vbahelper/vbadocumentsbase.hxx"

#include <comphelper/mediadescriptor.hxx>
#include <comphelper/processfactory.hxx>
#include <cppuhelper/implbase1.hxx>
#include <cppuhelper/implbase3.hxx>
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/lang/XComponent.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XTitle.hpp>
#include <com/sun/star/frame/FrameSearchFlag.hpp>
#include <com/sun/star/util/XModifiable.hpp>
#include <com/sun/star/frame/XStorable.hpp>
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/beans/PropertyVetoException.hpp>
#include <com/sun/star/util/XCloseable.hpp>
#include <com/sun/star/lang/IndexOutOfBoundsException.hpp>
#include <com/sun/star/document/XTypeDetection.hpp>
#include <com/sun/star/document/MacroExecMode.hpp>
#include <com/sun/star/uri/XUriReference.hpp>
#include <com/sun/star/uri/XUriReferenceFactory.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <sfx2/objsh.hxx>
#include <tools/urlobj.hxx>
#include <vbahelper/vbadocumentbase.hxx>
#include <boost/unordered_map.hpp>
#include <osl/file.hxx>

#include "vbahelper/vbahelper.hxx"
#include "vbahelper/vbaapplicationbase.hxx"

using namespace ::ooo::vba;
using namespace ::com::sun::star;

static const char aSpreadsheetDocument[] = "com.sun.star.sheet.SpreadsheetDocument";
static const char aTextDocument[] = "com.sun.star.text.TextDocument";

typedef  boost::unordered_map< rtl::OUString,
sal_Int32, ::rtl::OUStringHash,
::std::equal_to< ::rtl::OUString > > NameIndexHash;

typedef std::vector < uno::Reference< frame::XModel > > Documents;

typedef ::cppu::WeakImplHelper1< container::XEnumeration > DocumentsEnumImpl_BASE;

// #FIXME clearly this is a candidate for some sort of helper base class as
// this is a copy of SelectedSheetsEnum ( vbawindow.cxx )

class DocumentsEnumImpl : public DocumentsEnumImpl_BASE
{
    uno::Reference< uno::XComponentContext > m_xContext;
    Documents m_documents;
    Documents::const_iterator m_it;

public:
    DocumentsEnumImpl( const uno::Reference< uno::XComponentContext >& xContext, const Documents& docs ) throw ( uno::RuntimeException ) :  m_xContext( xContext ), m_documents( docs )
    {
        m_it = m_documents.begin();
    }
    DocumentsEnumImpl( const uno::Reference< uno::XComponentContext >& xContext ) throw ( uno::RuntimeException ) :  m_xContext( xContext )
    {
        uno::Reference< lang::XMultiComponentFactory > xSMgr(
            m_xContext->getServiceManager(), uno::UNO_QUERY_THROW );

        uno::Reference< frame::XDesktop > xDesktop
            (xSMgr->createInstanceWithContext(::rtl::OUString("com.sun.star.frame.Desktop"), m_xContext), uno::UNO_QUERY_THROW );
        uno::Reference< container::XEnumeration > mxComponents = xDesktop->getComponents()->createEnumeration();
        while( mxComponents->hasMoreElements() )
        {
            uno::Reference< frame::XModel > xNext( mxComponents->nextElement(), uno::UNO_QUERY );
            if ( xNext.is() )
                m_documents.push_back( xNext );
        }
        m_it = m_documents.begin();
    }
    // XEnumeration
    virtual ::sal_Bool SAL_CALL hasMoreElements(  ) throw (uno::RuntimeException)
    {
        return m_it != m_documents.end();
    }

    virtual uno::Any SAL_CALL nextElement(  ) throw (container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException)
    {
        if ( !hasMoreElements() )
        {
            throw container::NoSuchElementException();
        }
        return makeAny( *(m_it++) );
    }
};

// #FIXME clearly this is also a candidate for some sort of helper base class as
// a very similar one is used in vbawindow ( SelectedSheetsEnumAccess )
// Maybe a template base class that does all of the operations on the hashmap
// and vector only, and the sub-class does everything else
// => ctor, createEnumeration & factory method need be defined ( to be called
// by getByIndex, getByName )
typedef ::cppu::WeakImplHelper3< container::XEnumerationAccess
    , com::sun::star::container::XIndexAccess
    , com::sun::star::container::XNameAccess
    > DocumentsAccessImpl_BASE;

class DocumentsAccessImpl : public DocumentsAccessImpl_BASE
{
    uno::Reference< uno::XComponentContext > m_xContext;
    Documents m_documents;
    NameIndexHash namesToIndices;
    VbaDocumentsBase::DOCUMENT_TYPE meDocType;
public:
    DocumentsAccessImpl( const uno::Reference< uno::XComponentContext >& xContext, VbaDocumentsBase::DOCUMENT_TYPE eDocType ) throw (uno::RuntimeException) :m_xContext( xContext ), meDocType( eDocType )
    {
        uno::Reference< container::XEnumeration > xEnum = new DocumentsEnumImpl( m_xContext );
        sal_Int32 nIndex=0;
        while( xEnum->hasMoreElements() )
        {
            uno::Reference< lang::XServiceInfo > xServiceInfo( xEnum->nextElement(), uno::UNO_QUERY );
            if ( xServiceInfo.is()
                && (  ( xServiceInfo->supportsService( rtl::OUString(aSpreadsheetDocument) ) && meDocType == VbaDocumentsBase::EXCEL_DOCUMENT )
                || ( xServiceInfo->supportsService( rtl::OUString(aTextDocument) ) && meDocType == VbaDocumentsBase::WORD_DOCUMENT ) ) )
            {
                uno::Reference< frame::XModel > xModel( xServiceInfo, uno::UNO_QUERY_THROW ); // that the spreadsheetdocument is a xmodel is a given
                m_documents.push_back( xModel );
                rtl::OUString sName;
                uno::Reference< ::ooo::vba::XDocumentBase > xVbaDocument = new VbaDocumentBase( uno::Reference< XHelperInterface >(), xContext, xModel );
                if ( xVbaDocument.is() )
                {
                    sName = xVbaDocument->getName();
                }
                namesToIndices[ sName ] = nIndex++;
            }
        }

    }

    //XEnumerationAccess
    virtual uno::Reference< container::XEnumeration > SAL_CALL createEnumeration(  ) throw (uno::RuntimeException)
    {
        return new DocumentsEnumImpl( m_xContext, m_documents );
    }
    // XIndexAccess
    virtual ::sal_Int32 SAL_CALL getCount(  ) throw (uno::RuntimeException)
    {
        return m_documents.size();
    }
    virtual uno::Any SAL_CALL getByIndex( ::sal_Int32 Index ) throw ( lang::IndexOutOfBoundsException, lang::WrappedTargetException, uno::RuntimeException)
    {
        if ( Index < 0
            || static_cast< Documents::size_type >(Index) >= m_documents.size() )
            throw lang::IndexOutOfBoundsException();
        return makeAny( m_documents[ Index ] ); // returns xspreadsheetdoc
    }

    //XElementAccess
    virtual uno::Type SAL_CALL getElementType(  ) throw (uno::RuntimeException)
    {
        return frame::XModel::static_type(0);
    }

    virtual ::sal_Bool SAL_CALL hasElements(  ) throw (uno::RuntimeException)
    {
        return (!m_documents.empty());
    }

    //XNameAccess
    virtual uno::Any SAL_CALL getByName( const ::rtl::OUString& aName ) throw (container::NoSuchElementException, lang::WrappedTargetException, uno::RuntimeException)
    {
        NameIndexHash::const_iterator it = namesToIndices.find( aName );
        if ( it == namesToIndices.end() )
            throw container::NoSuchElementException();
        return makeAny( m_documents[ it->second ] );

    }

    virtual uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames(  ) throw (uno::RuntimeException)
    {
        uno::Sequence< ::rtl::OUString > names( namesToIndices.size() );
        ::rtl::OUString* pString = names.getArray();
        NameIndexHash::const_iterator it = namesToIndices.begin();
        NameIndexHash::const_iterator it_end = namesToIndices.end();
        for ( ; it != it_end; ++it, ++pString )
            *pString = it->first;
        return names;
    }

    virtual ::sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName ) throw (uno::RuntimeException)
    {
        NameIndexHash::const_iterator it = namesToIndices.find( aName );
        return (it != namesToIndices.end());
    }

};

VbaDocumentsBase::VbaDocumentsBase( const uno::Reference< XHelperInterface >& xParent, const uno::Reference< css::uno::XComponentContext >& xContext, DOCUMENT_TYPE eDocType ) throw (uno::RuntimeException) : VbaDocumentsBase_BASE( xParent, xContext, uno::Reference< container::XIndexAccess >( new DocumentsAccessImpl( xContext, eDocType ) ) ), meDocType( eDocType )
{
}

namespace {

void lclSetupComponent( const uno::Reference< lang::XComponent >& rxComponent, sal_Bool bScreenUpdating, sal_Bool bInteractive )
{
    if( !bScreenUpdating ) try
    {
        uno::Reference< frame::XModel >( rxComponent, uno::UNO_QUERY_THROW )->lockControllers();
    }
    catch( uno::Exception& )
    {
    }

    if( !bInteractive ) try
    {
        uno::Reference< frame::XModel > xModel( rxComponent, uno::UNO_QUERY_THROW );
        uno::Reference< frame::XController > xController( xModel->getCurrentController(), uno::UNO_SET_THROW );
        uno::Reference< frame::XFrame > xFrame( xController->getFrame(), uno::UNO_SET_THROW );
        uno::Reference< awt::XWindow >( xFrame->getContainerWindow(), uno::UNO_SET_THROW )->setEnable( sal_False );
    }
    catch( uno::Exception& )
    {
    }
}

} // namespace

uno::Any VbaDocumentsBase::createDocument() throw (uno::RuntimeException)
{
    // #163808# determine state of Application.ScreenUpdating and Application.Interactive symbols (before new document is opened)
    uno::Reference< XApplicationBase > xApplication( Application(), uno::UNO_QUERY );
    sal_Bool bScreenUpdating = !xApplication.is() || xApplication->getScreenUpdating();
    sal_Bool bInteractive = !xApplication.is() || xApplication->getInteractive();

     uno::Reference< lang::XMultiComponentFactory > xSMgr(
        mxContext->getServiceManager(), uno::UNO_QUERY_THROW );

     uno::Reference< frame::XComponentLoader > xLoader(
        xSMgr->createInstanceWithContext(
            ::rtl::OUString("com.sun.star.frame.Desktop"),
                mxContext), uno::UNO_QUERY_THROW );
    rtl::OUString sURL;
    if( meDocType == WORD_DOCUMENT )
        sURL = rtl::OUString( "private:factory/swriter" );
    else if( meDocType == EXCEL_DOCUMENT )
        sURL = rtl::OUString( "private:factory/scalc" );
    else
        throw uno::RuntimeException( rtl::OUString( "Not implemented" ), uno::Reference< uno::XInterface >() );

    // prepare the media descriptor
    ::comphelper::MediaDescriptor aMediaDesc;
    aMediaDesc[ ::comphelper::MediaDescriptor::PROP_MACROEXECUTIONMODE() ] <<= document::MacroExecMode::USE_CONFIG;
    aMediaDesc.setComponentDataEntry( ::rtl::OUString(  "ApplyFormDesignMode"  ), uno::Any( false ) );

    // craete the new document
    uno::Reference< lang::XComponent > xComponent = xLoader->loadComponentFromURL(
                                       sURL ,
                                       rtl::OUString( "_blank" ), 0,
                                       aMediaDesc.getAsConstPropertyValueList() );

    // #163808# lock document controllers and container window if specified by application
    lclSetupComponent( xComponent, bScreenUpdating, bInteractive );

    return uno::makeAny( xComponent );
}

void VbaDocumentsBase::closeDocuments() throw (uno::RuntimeException)
{
// #FIXME this *MUST* be wrong documents::close surely closes ALL documents
// in the collection, use of getCurrentDocument here is totally wrong
/*
    uno::Reference< lang::XMultiComponentFactory > xSMgr(
        mxContext->getServiceManager(), uno::UNO_QUERY_THROW );
    uno::Reference< frame::XModel > xModel( getCurrentDocument(), uno::UNO_QUERY_THROW );
    rtl::OUString url = rtl::OUString(  ".uno:CloseDoc");
    dispatchRequests(xModel,url);
*/
}

// #TODO# #FIXME# can any of the unused params below be used?
uno::Any VbaDocumentsBase::openDocument( const rtl::OUString& rFileName, const uno::Any& ReadOnly, const uno::Sequence< beans::PropertyValue >& rProps ) throw (uno::RuntimeException)
{
    // #163808# determine state of Application.ScreenUpdating and Application.Interactive symbols (before new document is opened)
    uno::Reference< XApplicationBase > xApplication( Application(), uno::UNO_QUERY );
    sal_Bool bScreenUpdating = !xApplication.is() || xApplication->getScreenUpdating();
    sal_Bool bInteractive = !xApplication.is() || xApplication->getInteractive();

    // we need to detect if this is a URL, if not then assume its a file path
        rtl::OUString aURL;
        INetURLObject aObj;
    aObj.SetURL( rFileName );
    bool bIsURL = aObj.GetProtocol() != INET_PROT_NOT_VALID;
    if ( bIsURL )
        aURL = rFileName;
    else
        osl::FileBase::getFileURLFromSystemPath( rFileName, aURL );
    uno::Reference< lang::XMultiComponentFactory > xSMgr(
        mxContext->getServiceManager(), uno::UNO_QUERY_THROW );
    uno::Reference< frame::XDesktop > xDesktop
        (xSMgr->createInstanceWithContext(::rtl::OUString("com.sun.star.frame.Desktop")                    , mxContext),
        uno::UNO_QUERY_THROW );
    uno::Reference< frame::XComponentLoader > xLoader(
        xSMgr->createInstanceWithContext(
        ::rtl::OUString("com.sun.star.frame.Desktop"),
        mxContext),
        uno::UNO_QUERY_THROW );

    uno::Sequence< beans::PropertyValue > sProps( rProps );
    sProps.realloc( sProps.getLength() + 1 );
    sProps[ sProps.getLength() - 1 ].Name = rtl::OUString( "MacroExecutionMode" );
    sProps[ sProps.getLength() - 1 ].Value <<= document::MacroExecMode::ALWAYS_EXECUTE_NO_WARN;

    if ( ReadOnly.hasValue()  )
    {
        sal_Bool bIsReadOnly = sal_False; ReadOnly >>= bIsReadOnly;
        if ( bIsReadOnly )
        {
            sProps.realloc( sProps.getLength() + 1 );
            sProps[ sProps.getLength() - 1 ].Name = rtl::OUString( "ReadOnly" );
            sProps[ sProps.getLength() - 1 ].Value <<= true;
        }
    }

    uno::Reference< lang::XComponent > xComponent = xLoader->loadComponentFromURL( aURL,
        rtl::OUString( "_default" ),
        frame::FrameSearchFlag::CREATE,
        sProps);

    // #163808# lock document controllers and container window if specified by application
    lclSetupComponent( xComponent, bScreenUpdating, bInteractive );

    return uno::makeAny( xComponent );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
