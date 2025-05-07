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

#include <osl/mutex.hxx>
#include <osl/diagnose.h>

#include <uno/mapping.hxx>

#include <cppuhelper/factory.hxx>
#include <cppuhelper/implbase1.hxx>

#include <tools/urlobj.hxx>
#include <ucbhelper/content.hxx>
#include <unotools/streamwrap.hxx>
#include <tools/stream.hxx>

#include <com/sun/star/beans/Property.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/container/XChild.hpp>
#include <com/sun/star/io/XActiveDataSink.hpp>
#include <com/sun/star/io/XActiveDataSource.hpp>
#include <com/sun/star/io/XActiveDataStreamer.hpp>
#include <com/sun/star/sdbc/XResultSet.hpp>
#include <com/sun/star/ucb/CommandFailedException.hpp>
#include <com/sun/star/ucb/ContentInfo.hpp>
#include <com/sun/star/ucb/ContentInfoAttribute.hpp>
#include <com/sun/star/ucb/InsertCommandArgument.hpp>
#include <com/sun/star/ucb/InteractiveIOException.hpp>
#include <com/sun/star/ucb/NameClash.hpp>
#include <com/sun/star/ucb/NameClashException.hpp>
#include <com/sun/star/ucb/OpenCommandArgument2.hpp>
#include <com/sun/star/ucb/OpenMode.hpp>
#include <com/sun/star/ucb/XCommandEnvironment.hpp>
#include <com/sun/star/ucb/XContent.hpp>
#include <com/sun/star/ucb/XContentAccess.hpp>
#include <com/sun/star/ucb/XSimpleFileAccess3.hpp>
#include <com/sun/star/util/XMacroExpander.hpp>

#include <vector>

#define IMPLEMENTATION_NAME "com.sun.star.comp.ucb.SimpleFileAccess"
#define SERVICE_NAME "com.sun.star.ucb.SimpleFileAccess"

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::io;
using namespace ::com::sun::star::ucb;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::task;
using namespace ::com::sun::star::util;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::registry;
using namespace ::com::sun::star::container;

using ::std::vector;

namespace io_FileAccess
{


//===========================================================================
// Implementation XSimpleFileAccess

typedef cppu::WeakImplHelper1< XSimpleFileAccess3 > FileAccessHelper;
class OCommandEnvironment;

class OFileAccess : public FileAccessHelper
{
    Reference< XMultiServiceFactory > mxSMgr;
    Reference< XCommandEnvironment > mxEnvironment;
    OCommandEnvironment* mpEnvironment;

    void transferImpl( const rtl::OUString& rSource, const rtl::OUString& rDest, sal_Bool bMoveData )
        throw(CommandAbortedException, Exception, RuntimeException);
    bool createNewFile( const rtl::OUString & rParentURL,
                        const rtl::OUString & rTitle,
                        const Reference< XInputStream >& data )
        throw ( Exception );

public:
    OFileAccess( const Reference< XMultiServiceFactory > & xSMgr )
        : mxSMgr( xSMgr), mpEnvironment( NULL ) {}

    // Methods
    virtual void SAL_CALL copy( const ::rtl::OUString& SourceURL, const ::rtl::OUString& DestURL ) throw(::com::sun::star::ucb::CommandAbortedException, ::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL move( const ::rtl::OUString& SourceURL, const ::rtl::OUString& DestURL ) throw(::com::sun::star::ucb::CommandAbortedException, ::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL kill( const ::rtl::OUString& FileURL ) throw(::com::sun::star::ucb::CommandAbortedException, ::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL isFolder( const ::rtl::OUString& FileURL ) throw(::com::sun::star::ucb::CommandAbortedException, ::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL isReadOnly( const ::rtl::OUString& FileURL ) throw(::com::sun::star::ucb::CommandAbortedException, ::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setReadOnly( const ::rtl::OUString& FileURL, sal_Bool bReadOnly ) throw(::com::sun::star::ucb::CommandAbortedException, ::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL createFolder( const ::rtl::OUString& NewFolderURL ) throw(::com::sun::star::ucb::CommandAbortedException, ::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
    virtual sal_Int32 SAL_CALL getSize( const ::rtl::OUString& FileURL ) throw(::com::sun::star::ucb::CommandAbortedException, ::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getContentType( const ::rtl::OUString& FileURL ) throw(::com::sun::star::ucb::CommandAbortedException, ::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::util::DateTime SAL_CALL getDateTimeModified( const ::rtl::OUString& FileURL ) throw(::com::sun::star::ucb::CommandAbortedException, ::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getFolderContents( const ::rtl::OUString& FolderURL, sal_Bool bIncludeFolders ) throw(::com::sun::star::ucb::CommandAbortedException, ::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL exists( const ::rtl::OUString& FileURL ) throw(::com::sun::star::ucb::CommandAbortedException, ::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > SAL_CALL openFileRead( const ::rtl::OUString& FileURL ) throw(::com::sun::star::ucb::CommandAbortedException, ::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XOutputStream > SAL_CALL openFileWrite( const ::rtl::OUString& FileURL ) throw(::com::sun::star::ucb::CommandAbortedException, ::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::io::XStream > SAL_CALL openFileReadWrite( const ::rtl::OUString& FileURL ) throw(::com::sun::star::ucb::CommandAbortedException, ::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setInteractionHandler( const ::com::sun::star::uno::Reference< ::com::sun::star::task::XInteractionHandler >& Handler ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL writeFile( const ::rtl::OUString& FileURL, const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& data ) throw (::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL isHidden( const ::rtl::OUString& FileURL ) throw(::com::sun::star::ucb::CommandAbortedException, ::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setHidden( const ::rtl::OUString& FileURL, sal_Bool bHidden ) throw(::com::sun::star::ucb::CommandAbortedException, ::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);
};


//===========================================================================
// Implementation XActiveDataSink

typedef cppu::WeakImplHelper1< XActiveDataSink > ActiveDataSinkHelper;

class OActiveDataSink : public ActiveDataSinkHelper
{
    Reference< XInputStream > mxStream;

public:

    // Methods
    virtual void SAL_CALL setInputStream( const Reference< XInputStream >& aStream )
        throw(RuntimeException);
    virtual Reference< XInputStream > SAL_CALL getInputStream(  )
        throw(RuntimeException);
};

void OActiveDataSink::setInputStream( const Reference< XInputStream >& aStream )
    throw(RuntimeException)
{
    mxStream = aStream;
}

Reference< XInputStream > OActiveDataSink::getInputStream()
    throw(RuntimeException)
{
    return mxStream;
}


//===========================================================================
// Implementation XActiveDataSource

typedef cppu::WeakImplHelper1< XActiveDataSource > ActiveDataSourceHelper;

class OActiveDataSource : public ActiveDataSourceHelper
{
    Reference< XOutputStream > mxStream;

public:

    // Methods
    virtual void SAL_CALL setOutputStream( const Reference< XOutputStream >& aStream )
        throw(RuntimeException);
    virtual Reference< XOutputStream > SAL_CALL getOutputStream()
        throw(RuntimeException);
};

void OActiveDataSource::setOutputStream( const Reference< XOutputStream >& aStream )
    throw(RuntimeException)
{
    mxStream = aStream;
}

Reference< XOutputStream > OActiveDataSource::getOutputStream()
    throw(RuntimeException)
{
    return mxStream;
}


//===========================================================================
// Implementation XActiveDataStreamer

typedef cppu::WeakImplHelper1< XActiveDataStreamer > ActiveDataStreamerHelper;

class OActiveDataStreamer : public ActiveDataStreamerHelper
{
    Reference< XStream > mxStream;

public:

    // Methods
    virtual void SAL_CALL setStream( const Reference< XStream >& aStream )
        throw(RuntimeException);
    virtual Reference< XStream > SAL_CALL getStream()
        throw(RuntimeException);
};

void OActiveDataStreamer::setStream( const Reference< XStream >& aStream )
    throw(RuntimeException)
{
    mxStream = aStream;
}

Reference< XStream > OActiveDataStreamer::getStream()
    throw(RuntimeException)
{
    return mxStream;
}



//===========================================================================
// Implementation XCommandEnvironment

typedef cppu::WeakImplHelper1< XCommandEnvironment > CommandEnvironmentHelper;

class OCommandEnvironment : public CommandEnvironmentHelper
{
    Reference< XInteractionHandler > mxInteraction;

public:
    void setHandler( Reference< XInteractionHandler > xInteraction_ )
    {
        mxInteraction = xInteraction_;
    }

    // Methods
    virtual Reference< XInteractionHandler > SAL_CALL getInteractionHandler()
        throw(RuntimeException);
    virtual Reference< XProgressHandler > SAL_CALL getProgressHandler()
        throw(RuntimeException);
};

Reference< XInteractionHandler > OCommandEnvironment::getInteractionHandler()
    throw(RuntimeException)
{
    return mxInteraction;
}

Reference< XProgressHandler > OCommandEnvironment::getProgressHandler()
    throw(RuntimeException)
{
    Reference< XProgressHandler > xRet;
    return xRet;
}

//===========================================================================

void OFileAccess::transferImpl( const rtl::OUString& rSource,
                                const rtl::OUString& rDest,
                                sal_Bool bMoveData )
    throw(CommandAbortedException, Exception, RuntimeException)
{
    // SfxContentHelper::Transfer_Impl
    INetURLObject aSourceObj( rSource, INET_PROT_FILE );
    INetURLObject aDestObj( rDest, INET_PROT_FILE );
    String aName = aDestObj.getName(
        INetURLObject::LAST_SEGMENT, true, INetURLObject::DECODE_WITH_CHARSET );
    String aDestURL;
    String aSourceURL = aSourceObj.GetMainURL( INetURLObject::NO_DECODE );
    if ( aDestObj.removeSegment() )
    {
        // hierarchical URL.

        aDestObj.setFinalSlash();
        aDestURL = aDestObj.GetMainURL( INetURLObject::NO_DECODE );
    }
    else
    {
        // non-hierachical URL

        // #i29648#
        //

        if ( aDestObj.GetProtocol() == INET_PROT_VND_SUN_STAR_EXPAND )
        {
            // Hack: Expand destination URL using Macro Expander and try again
            //       with the hopefully hierarchical expanded URL...

            try
            {
                Reference< XComponentContext > xCtx;
                Reference< XPropertySet > xPropSet( mxSMgr, UNO_QUERY_THROW );
                if ( xPropSet.is() )
                {
                    xPropSet->getPropertyValue(
                        rtl::OUString(
                            "DefaultContext"  ) )
                                >>= xCtx;
                }

                Reference< XMacroExpander > xExpander;

                xCtx->getValueByName(
                    rtl::OUString( "/singletons/com.sun.star.util.theMacroExpander"  ) )
                            >>= xExpander;

                OSL_ENSURE( xExpander.is(),
                            "Unable to obtain macro expander singleton!" );

                aDestURL = xExpander->expandMacros(
                    aDestObj.GetURLPath( INetURLObject::DECODE_WITH_CHARSET ) );
            }
            catch ( Exception const & )
            {
                throw RuntimeException(
                    rtl::OUString( "OFileAccess::transferrImpl - Unable to obtain "
                            "destination folder URL!"  ),
                    static_cast< cppu::OWeakObject * >( this ) );
            }

            transferImpl( rSource, aDestURL, bMoveData );
            return;
        }

        throw RuntimeException(
            rtl::OUString( "OFileAccess::transferrImpl - Unable to obtain "
                    "destination folder URL!"  ),
                static_cast< cppu::OWeakObject * >( this ) );

    }

    ucbhelper::Content aDestPath( aDestURL,   mxEnvironment );
    ucbhelper::Content aSrc     ( aSourceURL, mxEnvironment );

    try
    {
        aDestPath.transferContent( aSrc,
                                   bMoveData
                                    ? ucbhelper::InsertOperation_MOVE
                                    : ucbhelper::InsertOperation_COPY,
                                   aName,
                                   ::com::sun::star::ucb::NameClash::OVERWRITE );
    }
    catch ( ::com::sun::star::ucb::CommandFailedException const & )
    {
        // Interaction Handler already handled the error that has occurred...
    }
}

void OFileAccess::copy( const rtl::OUString& SourceURL, const rtl::OUString& DestURL )
    throw(CommandAbortedException, Exception, RuntimeException)
{
    transferImpl( SourceURL, DestURL, sal_False );
}

void OFileAccess::move( const rtl::OUString& SourceURL, const rtl::OUString& DestURL )
    throw(CommandAbortedException, Exception, RuntimeException)
{
    transferImpl( SourceURL, DestURL, sal_True );
}

void OFileAccess::kill( const rtl::OUString& FileURL )
    throw(CommandAbortedException, Exception, RuntimeException)
{
    // SfxContentHelper::Kill
    INetURLObject aDeleteObj( FileURL, INET_PROT_FILE );
    ucbhelper::Content aCnt( aDeleteObj.GetMainURL( INetURLObject::NO_DECODE ), mxEnvironment );
    try
    {
        aCnt.executeCommand( rtl::OUString("delete" ), makeAny( sal_Bool( sal_True ) ) );
    }
    catch ( ::com::sun::star::ucb::CommandFailedException const & )
    {
        // Interaction Handler already handled the error that has occurred...
    }
}

sal_Bool OFileAccess::isFolder( const rtl::OUString& FileURL )
    throw(CommandAbortedException, Exception, RuntimeException)
{
    sal_Bool bRet = sal_False;
    try
    {
        INetURLObject aURLObj( FileURL, INET_PROT_FILE );
        ucbhelper::Content aCnt( aURLObj.GetMainURL( INetURLObject::NO_DECODE ), mxEnvironment );
        bRet = aCnt.isFolder();
    }
    catch (const Exception &) {}
    return bRet;
}

sal_Bool OFileAccess::isReadOnly( const rtl::OUString& FileURL )
    throw(CommandAbortedException, Exception, RuntimeException)
{
    INetURLObject aURLObj( FileURL, INET_PROT_FILE );
    ucbhelper::Content aCnt( aURLObj.GetMainURL( INetURLObject::NO_DECODE ), mxEnvironment );
    Any aRetAny = aCnt.getPropertyValue( rtl::OUString( "IsReadOnly"  ) );
    sal_Bool bRet = sal_False;
    aRetAny >>= bRet;
    return bRet;
}

void OFileAccess::setReadOnly( const rtl::OUString& FileURL, sal_Bool bReadOnly )
    throw(CommandAbortedException, Exception, RuntimeException)
{
    INetURLObject aURLObj( FileURL, INET_PROT_FILE );
    ucbhelper::Content aCnt( aURLObj.GetMainURL( INetURLObject::NO_DECODE ), mxEnvironment );
    Any aAny;
    aAny <<= bReadOnly;
    aCnt.setPropertyValue( rtl::OUString( "IsReadOnly"  ), aAny );
}

void OFileAccess::createFolder( const rtl::OUString& NewFolderURL )
    throw(CommandAbortedException, Exception, RuntimeException)
{
    // Does the folder already exist?
    if( NewFolderURL.isEmpty() || isFolder( NewFolderURL ) )
        return;

    // SfxContentHelper::MakeFolder
    INetURLObject aURL( NewFolderURL, INET_PROT_FILE );
    String aNewFolderURLStr = aURL.GetMainURL( INetURLObject::NO_DECODE );
    String aTitle = aURL.getName( INetURLObject::LAST_SEGMENT, true, INetURLObject::DECODE_WITH_CHARSET );
    if ( aTitle.Len() )
    {
        aURL.removeSegment();

        // Does the base folder exist? Otherwise create it first
        String aBaseFolderURLStr = aURL.GetMainURL( INetURLObject::NO_DECODE );
        if( !isFolder( aBaseFolderURLStr ) )
        {
            createFolder( aBaseFolderURLStr );
        }
    }

    ucbhelper::Content aCnt( aURL.GetMainURL( INetURLObject::NO_DECODE ), mxEnvironment );

    Sequence< ContentInfo > aInfo = aCnt.queryCreatableContentsInfo();
    sal_Int32 nCount = aInfo.getLength();
    if ( nCount == 0 )
        return;

    for ( sal_Int32 i = 0; i < nCount; ++i )
    {
        // Simply look for the first KIND_FOLDER...
        const ContentInfo & rCurr = aInfo[i];
        if ( rCurr.Attributes & ContentInfoAttribute::KIND_FOLDER )
        {
            // Make sure the only required bootstrap property is "Title",
            const Sequence< Property > & rProps = rCurr.Properties;
            if ( rProps.getLength() != 1 )
                continue;

            if ( rProps[ 0 ].Name != "Title" )
                continue;

            Sequence<rtl::OUString> aNames(1);
            rtl::OUString* pNames = aNames.getArray();
            pNames[0] = rtl::OUString( "Title"  );
            Sequence< Any > aValues(1);
            Any* pValues = aValues.getArray();
            pValues[0] = makeAny( rtl::OUString( aTitle ) );

            ucbhelper::Content aNew;
            try
            {
                if ( !aCnt.insertNewContent( rCurr.Type, aNames, aValues, aNew ) )
                    continue;

                // Success. We're done.
                return;
            }
            catch ( ::com::sun::star::ucb::CommandFailedException const & )
            {
                // Interaction Handler already handled the error that has occurred...
                continue;
            }
        }
    }
}

sal_Int32 OFileAccess::getSize( const rtl::OUString& FileURL )
    throw(CommandAbortedException, Exception, RuntimeException)
{
    // SfxContentHelper::GetSize
    sal_Int32 nSize = 0;
    sal_Int64 nTemp = 0;
    INetURLObject aObj( FileURL, INET_PROT_FILE );
    ucbhelper::Content aCnt( aObj.GetMainURL( INetURLObject::NO_DECODE ), mxEnvironment );
    aCnt.getPropertyValue( rtl::OUString("Size" ) ) >>= nTemp;
    nSize = (sal_Int32)nTemp;
    return nSize;
}

rtl::OUString OFileAccess::getContentType( const rtl::OUString& FileURL )
    throw(CommandAbortedException, Exception, RuntimeException)
{
    INetURLObject aObj( FileURL, INET_PROT_FILE );
    ucbhelper::Content aCnt( aObj.GetMainURL( INetURLObject::NO_DECODE ), mxEnvironment );

    Reference< XContent > xContent = aCnt.get();
    rtl::OUString aTypeStr = xContent->getContentType();
    return aTypeStr;
}

DateTime OFileAccess::getDateTimeModified( const rtl::OUString& FileURL )
    throw(CommandAbortedException, Exception, RuntimeException)
{
    INetURLObject aFileObj( FileURL, INET_PROT_FILE );
    DateTime aDateTime;

    Reference< XCommandEnvironment > aCmdEnv;
    ucbhelper::Content aYoung( aFileObj.GetMainURL( INetURLObject::NO_DECODE ), aCmdEnv );
    aYoung.getPropertyValue( rtl::OUString("DateModified" ) ) >>= aDateTime;
    return aDateTime;
}

typedef vector< rtl::OUString* > StringList_Impl;

Sequence< rtl::OUString > OFileAccess::getFolderContents( const rtl::OUString& FolderURL, sal_Bool bIncludeFolders )
    throw(CommandAbortedException, Exception, RuntimeException)
{
    // SfxContentHelper::GetFolderContents

    StringList_Impl* pFiles = NULL;
    INetURLObject aFolderObj( FolderURL, INET_PROT_FILE );

    ucbhelper::Content aCnt( aFolderObj.GetMainURL( INetURLObject::NO_DECODE ), mxEnvironment );
    Reference< XResultSet > xResultSet;
    Sequence< rtl::OUString > aProps(0);

    ucbhelper::ResultSetInclude eInclude = bIncludeFolders ? ucbhelper::INCLUDE_FOLDERS_AND_DOCUMENTS : ucbhelper::INCLUDE_DOCUMENTS_ONLY;

    try
    {
        xResultSet = aCnt.createCursor( aProps, eInclude );
    }
    catch ( ::com::sun::star::ucb::CommandFailedException const & )
    {
        // Interaction Handler already handled the error that has occurred...
    }

    if ( xResultSet.is() )
    {
        pFiles = new StringList_Impl();
        Reference< com::sun::star::ucb::XContentAccess > xContentAccess( xResultSet, UNO_QUERY );

        while ( xResultSet->next() )
        {
            rtl::OUString aId = xContentAccess->queryContentIdentifierString();
            INetURLObject aURL( aId, INET_PROT_FILE );
            rtl::OUString* pFile = new rtl::OUString( aURL.GetMainURL( INetURLObject::NO_DECODE ) );
            pFiles->push_back( pFile );
        }
    }

    if ( pFiles )
    {
        size_t nCount = pFiles->size();
        Sequence < rtl::OUString > aRet( nCount );
        rtl::OUString* pRet = aRet.getArray();
        for ( size_t i = 0; i < nCount; ++i )
        {
            rtl::OUString* pFile = pFiles->at( i );
            pRet[i] = *( pFile );
            delete pFile;
        }
        pFiles->clear();
        delete pFiles;
        return aRet;
    }
    else
        return Sequence < rtl::OUString > ();
}

sal_Bool OFileAccess::exists( const rtl::OUString& FileURL )
    throw(CommandAbortedException, Exception, RuntimeException)
{
    sal_Bool bRet = sal_False;
    try
    {
        bRet = isFolder( FileURL );
        if( !bRet )
        {
            Reference< XInputStream > xStream = openFileRead( FileURL );
            bRet = xStream.is();
            if( bRet )
                xStream->closeInput();
        }
    }
    catch (const Exception &) {}
    return bRet;
}

Reference< XInputStream > OFileAccess::openFileRead( const rtl::OUString& FileURL )
    throw(CommandAbortedException, Exception, RuntimeException)
{
    Reference< XInputStream > xRet;
    INetURLObject aObj( FileURL, INET_PROT_FILE );
    ucbhelper::Content aCnt( aObj.GetMainURL( INetURLObject::NO_DECODE ), mxEnvironment );

    Reference< XActiveDataSink > xSink = (XActiveDataSink*)(new OActiveDataSink());

    try
    {
        sal_Bool bRet = aCnt.openStream( xSink );
        if( bRet )
            xRet = xSink->getInputStream();
    }
    catch ( ::com::sun::star::ucb::CommandFailedException const & )
    {
        // Interaction Handler already handled the error that has occurred...
    }

    return xRet;
}

Reference< XOutputStream > OFileAccess::openFileWrite( const rtl::OUString& FileURL )
    throw(CommandAbortedException, Exception, RuntimeException)
{
    Reference< XOutputStream > xRet;
    Reference< XStream > xStream = OFileAccess::openFileReadWrite( FileURL );
    if( xStream.is() )
        xRet = xStream->getOutputStream();
    return xRet;
}

Reference< XStream > OFileAccess::openFileReadWrite( const rtl::OUString& FileURL )
    throw(CommandAbortedException, Exception, RuntimeException)
{
    Reference< XActiveDataStreamer > xSink = (XActiveDataStreamer*)new OActiveDataStreamer();
    Reference< XInterface > xSinkIface = Reference< XInterface >::query( xSink );

    OpenCommandArgument2 aArg;
    aArg.Mode       = OpenMode::DOCUMENT;
    aArg.Priority   = 0; // unused
    aArg.Sink       = xSink;
    aArg.Properties = Sequence< Property >( 0 ); // unused

    Any aCmdArg;
    aCmdArg <<= aArg;

    INetURLObject aFileObj( FileURL, INET_PROT_FILE );
    ucbhelper::Content aCnt( aFileObj.GetMainURL( INetURLObject::NO_DECODE ), mxEnvironment );

    // Be silent...
    Reference< XInteractionHandler > xIH;
    if ( mpEnvironment )
    {
        xIH = mpEnvironment->getInteractionHandler();
        mpEnvironment->setHandler( 0 );
    }

    try
    {
        aCnt.executeCommand( rtl::OUString("open" ), aCmdArg );
    }
    catch ( InteractiveIOException const & e )
    {
        if ( xIH.is() )
            mpEnvironment->setHandler( xIH );

        if ( e.Code == IOErrorCode_NOT_EXISTING )
        {
            // Create file...
            SvMemoryStream aStream(0,0);
            ::utl::OInputStreamWrapper* pInput = new ::utl::OInputStreamWrapper( aStream );
            Reference< XInputStream > xInput( pInput );
            InsertCommandArgument aInsertArg;
            aInsertArg.Data = xInput;
            aInsertArg.ReplaceExisting = sal_False;

            aCmdArg <<= aInsertArg;
            aCnt.executeCommand( rtl::OUString("insert" ), aCmdArg );

            // Retry...
            return openFileReadWrite( FileURL );
        }

        throw;
    }

    if ( xIH.is() )
        mpEnvironment->setHandler( xIH );

    Reference< XStream > xRet = xSink->getStream();
    return xRet;
}

void OFileAccess::setInteractionHandler( const Reference< XInteractionHandler >& Handler )
    throw(RuntimeException)
{
    if( !mpEnvironment )
    {
        mpEnvironment = new OCommandEnvironment();
        mxEnvironment = (XCommandEnvironment*)mpEnvironment;
    }
    mpEnvironment->setHandler( Handler );
}

bool OFileAccess::createNewFile( const rtl::OUString & rParentURL,
                                 const rtl::OUString & rTitle,
                                 const Reference< XInputStream >& data )
    throw ( Exception )
{
    ucbhelper::Content aParentCnt( rParentURL, mxEnvironment );

    Sequence< ContentInfo > aInfo = aParentCnt.queryCreatableContentsInfo();
    sal_Int32 nCount = aInfo.getLength();
    if ( nCount == 0 )
        return false;

    for ( sal_Int32 i = 0; i < nCount; ++i )
    {
        const ContentInfo & rCurr = aInfo[i];
        if ( ( rCurr.Attributes
               & ContentInfoAttribute::KIND_DOCUMENT ) &&
             ( rCurr.Attributes
               & ContentInfoAttribute::INSERT_WITH_INPUTSTREAM ) )
        {
            // Make sure the only required bootstrap property is
            // "Title",
            const Sequence< Property > & rProps = rCurr.Properties;
            if ( rProps.getLength() != 1 )
                continue;

            if ( rProps[ 0 ].Name != "Title" )
                continue;

            Sequence<rtl::OUString> aNames(1);
            rtl::OUString* pNames = aNames.getArray();
            pNames[0] = rtl::OUString(
                            "Title"  );
            Sequence< Any > aValues(1);
            Any* pValues = aValues.getArray();
            pValues[0] = makeAny( rtl::OUString( rTitle ) );

            try
            {
                ucbhelper::Content aNew;
                if ( aParentCnt.insertNewContent(
                         rCurr.Type, aNames, aValues, data, aNew ) )
                    return true; // success.
                else
                    continue;
            }
            catch ( CommandFailedException const & )
            {
                // Interaction Handler already handled the
                // error that has occurred...
                continue;
            }
        }
    }

    return false;
}

void SAL_CALL OFileAccess::writeFile( const rtl::OUString& FileURL,
                                      const Reference< XInputStream >& data )
    throw ( Exception, RuntimeException )
{
    INetURLObject aURL( FileURL, INET_PROT_FILE );
    try
    {
        ucbhelper::Content aCnt(
            aURL.GetMainURL( INetURLObject::NO_DECODE ), mxEnvironment );

        try
        {
            aCnt.writeStream( data, sal_True /* bReplaceExisting */ );
        }
        catch ( CommandFailedException const & )
        {
            // Interaction Handler already handled the error that has occurred...
        }
    }
    catch ( ContentCreationException const & e )
    {
        // Most probably file does not exist. Try to create.
        if ( e.eError == ContentCreationError_CONTENT_CREATION_FAILED )
        {
            INetURLObject aParentURLObj( aURL );
            if ( aParentURLObj.removeSegment() )
            {
                String aParentURL
                    = aParentURLObj.GetMainURL( INetURLObject::NO_DECODE );

                // ensure all parent folders exist.
                createFolder( aParentURL );

                // create the new file...
                String aTitle
                    = aURL.getName( INetURLObject::LAST_SEGMENT,
                                    true,
                                    INetURLObject::DECODE_WITH_CHARSET );
                if ( createNewFile( aParentURL, aTitle, data ) )
                {
                    // success
                    return;
                }
            }
        }

        throw;
    }
}

sal_Bool OFileAccess::isHidden( const ::rtl::OUString& FileURL )
    throw(CommandAbortedException, Exception, RuntimeException)
{
    INetURLObject aURLObj( FileURL, INET_PROT_FILE );
    ucbhelper::Content aCnt( aURLObj.GetMainURL( INetURLObject::NO_DECODE ), mxEnvironment );
    Any aRetAny = aCnt.getPropertyValue( rtl::OUString( "IsHidden"  ) );
    sal_Bool bRet = sal_False;
    aRetAny >>= bRet;
    return bRet;
}

void OFileAccess::setHidden( const ::rtl::OUString& FileURL, sal_Bool bHidden )
    throw(CommandAbortedException, Exception, RuntimeException)
{
    INetURLObject aURLObj( FileURL, INET_PROT_FILE );
    ucbhelper::Content aCnt( aURLObj.GetMainURL( INetURLObject::NO_DECODE ), mxEnvironment );
    Any aAny;
    aAny <<= bHidden;
    aCnt.setPropertyValue( rtl::OUString( "IsHidden"  ), aAny );
}

//==================================================================================================
//==================================================================================================
//==================================================================================================

Reference< XInterface > SAL_CALL FileAccess_CreateInstance( const Reference< XMultiServiceFactory > & xSMgr )
{
    return Reference < XInterface >( ( cppu::OWeakObject * ) new OFileAccess( xSMgr ) );
}


Sequence< rtl::OUString > FileAccess_getSupportedServiceNames()
{
    Sequence< rtl::OUString > seqNames(1);
    seqNames.getArray()[0] = rtl::OUString(SERVICE_NAME );
    return seqNames;
}


}

//==================================================================================================
// Component exports

extern "C"
{
SAL_DLLPUBLIC_EXPORT void * SAL_CALL fileacc_component_getFactory(
    const sal_Char * pImplName, void * pServiceManager, void * /*pRegistryKey*/ )
{
    void * pRet = 0;

    if (pServiceManager && rtl_str_compare( pImplName, IMPLEMENTATION_NAME ) == 0)
    {
        Reference< XSingleServiceFactory > xFactory( cppu::createSingleFactory(
            reinterpret_cast< XMultiServiceFactory * >( pServiceManager ),
            rtl::OUString::createFromAscii( pImplName ),
            io_FileAccess::FileAccess_CreateInstance,
            io_FileAccess::FileAccess_getSupportedServiceNames() ) );

        if (xFactory.is())
        {
            xFactory->acquire();
            pRet = xFactory.get();
        }
    }

    return pRet;
}
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
