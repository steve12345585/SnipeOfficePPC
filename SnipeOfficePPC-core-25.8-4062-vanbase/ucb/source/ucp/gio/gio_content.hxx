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

#ifndef GIO_CONTENT_HXX
#define GIO_CONTENT_HXX

#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/io/XOutputStream.hpp>
#include <com/sun/star/ucb/ContentCreationException.hpp>
#include <com/sun/star/ucb/OpenCommandArgument2.hpp>
#include <com/sun/star/ucb/TransferInfo.hpp>
#include <com/sun/star/ucb/XContentCreator.hpp>
#include <ucbhelper/contenthelper.hxx>
#include <gio/gio.h>

#include <list>

namespace com { namespace sun { namespace star {
    namespace beans {
        struct Property;
        struct PropertyValue;
    }
    namespace sdbc {
        class XRow;
    }
}}}
namespace ucbhelper
{
    class Content;
}


namespace gio
{

#define GIO_CONTENT_SERVICE_NAME "com.sun.star.ucb.GIOContent"

#define GIO_FILE_TYPE   "application/vnd.sun.staroffice.gio-file"
#define GIO_FOLDER_TYPE "application/vnd.sun.staroffice.gio-folder"

com::sun::star::uno::Any convertToException(GError *pError,
    const com::sun::star::uno::Reference< com::sun::star::uno::XInterface >& rContext, bool bThrow=true);

class ContentProvider;
class Content : public ::ucbhelper::ContentImplHelper, public com::sun::star::ucb::XContentCreator
{
private:
    ContentProvider  *m_pProvider;
    GFile* mpFile;
    GFileInfo *mpInfo;
    bool mbTransient;

    GFileInfo *getGFileInfo(const com::sun::star::uno::Reference< com::sun::star::ucb::XCommandEnvironment >& xEnv,
        GError **ppError=NULL);
    bool isFolder(const com::sun::star::uno::Reference< com::sun::star::ucb::XCommandEnvironment >& xEnv);

    com::sun::star::uno::Any mapGIOError( GError *error );
    com::sun::star::uno::Any getBadArgExcept();

    com::sun::star::uno::Reference< com::sun::star::sdbc::XRow >
        getPropertyValues(
            const com::sun::star::uno::Sequence< com::sun::star::beans::Property >& rProperties,
            const com::sun::star::uno::Reference< com::sun::star::ucb::XCommandEnvironment >& xEnv );
private:
    typedef rtl::Reference< Content > ContentRef;
    typedef std::list< ContentRef > ContentRefList;

    void queryChildren( ContentRefList& rChildren );

    bool doSetFileInfo ( GFileInfo *pNewInfo );

    com::sun::star::uno::Any open(const com::sun::star::ucb::OpenCommandArgument2 & rArg,
        const com::sun::star::uno::Reference< com::sun::star::ucb::XCommandEnvironment > & xEnv )
            throw( com::sun::star::uno::Exception );

    void transfer( const com::sun::star::ucb::TransferInfo& rTransferInfo,
        const com::sun::star::uno::Reference< com::sun::star::ucb::XCommandEnvironment > & xEnv )
            throw( com::sun::star::uno::Exception );

    void insert( const com::sun::star::uno::Reference< com::sun::star::io::XInputStream > & xInputStream,
        sal_Bool bReplaceExisting, const com::sun::star::uno::Reference< com::sun::star::ucb::XCommandEnvironment >& xEnv )
            throw( com::sun::star::uno::Exception );

    void destroy( sal_Bool bDeletePhysical ) throw( com::sun::star::uno::Exception );

    void copyData( com::sun::star::uno::Reference< com::sun::star::io::XInputStream > xIn,
        com::sun::star::uno::Reference< com::sun::star::io::XOutputStream > xOut );

    com::sun::star::uno::Sequence< com::sun::star::uno::Any >
        setPropertyValues( const com::sun::star::uno::Sequence<
            com::sun::star::beans::PropertyValue >& rValues,
            const com::sun::star::uno::Reference<
            com::sun::star::ucb::XCommandEnvironment >& xEnv );

    sal_Bool feedSink( com::sun::star::uno::Reference< com::sun::star::uno::XInterface> aSink,
        const com::sun::star::uno::Reference< com::sun::star::ucb::XCommandEnvironment >& xEnv );

    com::sun::star::uno::Reference< com::sun::star::io::XInputStream >
        createInputStream(const com::sun::star::uno::Reference< com::sun::star::ucb::XCommandEnvironment >& xEnv )
            throw( com::sun::star::uno::Exception );

    sal_Bool exchangeIdentity(const com::sun::star::uno::Reference< com::sun::star::ucb::XContentIdentifier >&  xNewId);

public:
    Content( const com::sun::star::uno::Reference<
        com::sun::star::uno::XComponentContext >& rxContext, ContentProvider *pProvider,
        const com::sun::star::uno::Reference< com::sun::star::ucb::XContentIdentifier >& Identifier)
            throw ( com::sun::star::ucb::ContentCreationException );

    Content( const com::sun::star::uno::Reference<
        com::sun::star::uno::XComponentContext >& rxContext, ContentProvider *pProvider,
        const com::sun::star::uno::Reference< com::sun::star::ucb::XContentIdentifier >& Identifier,
        sal_Bool bIsFolder)
            throw ( com::sun::star::ucb::ContentCreationException );

    virtual ~Content();

    com::sun::star::uno::Reference< com::sun::star::sdbc::XRow > getPropertyValuesFromGFileInfo(
        GFileInfo *pInfo, const com::sun::star::uno::Reference< com::sun::star::uno::XComponentContext >& rxContext,
        const com::sun::star::uno::Reference< com::sun::star::ucb::XCommandEnvironment > & xEnv,
        const com::sun::star::uno::Sequence< com::sun::star::beans::Property >& rProperties);

    virtual com::sun::star::uno::Sequence< com::sun::star::beans::Property >
        getProperties( const com::sun::star::uno::Reference<
            com::sun::star::ucb::XCommandEnvironment > & xEnv );

    virtual com::sun::star::uno::Sequence< com::sun::star::ucb::CommandInfo >
        getCommands( const com::sun::star::uno::Reference<
            com::sun::star::ucb::XCommandEnvironment > & xEnv );

    virtual ::rtl::OUString getParentURL();

    XINTERFACE_DECL()

    XTYPEPROVIDER_DECL()

    virtual ::rtl::OUString SAL_CALL
    getImplementationName()
            throw( com::sun::star::uno::RuntimeException );

    virtual com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL
    getSupportedServiceNames()
            throw( com::sun::star::uno::RuntimeException );

    virtual rtl::OUString SAL_CALL
    getContentType()
            throw( com::sun::star::uno::RuntimeException );

    virtual com::sun::star::uno::Any SAL_CALL
        execute( const com::sun::star::ucb::Command& aCommand,
        sal_Int32 CommandId,
        const com::sun::star::uno::Reference< com::sun::star::ucb::XCommandEnvironment >& Environment )
            throw( com::sun::star::uno::Exception, com::sun::star::ucb::CommandAbortedException, com::sun::star::uno::RuntimeException );

    virtual void SAL_CALL abort( sal_Int32 CommandId )
            throw( com::sun::star::uno::RuntimeException );

    virtual com::sun::star::uno::Sequence< com::sun::star::ucb::ContentInfo >
        SAL_CALL queryCreatableContentsInfo()
            throw( com::sun::star::uno::RuntimeException );
    virtual com::sun::star::uno::Reference< com::sun::star::ucb::XContent >
        SAL_CALL createNewContent( const com::sun::star::ucb::ContentInfo& Info )
            throw( com::sun::star::uno::RuntimeException );

    com::sun::star::uno::Sequence< com::sun::star::ucb::ContentInfo >
        queryCreatableContentsInfo(
        const com::sun::star::uno::Reference< com::sun::star::ucb::XCommandEnvironment >& xEnv)
            throw( com::sun::star::uno::RuntimeException );

    GFile* getGFile();
};

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
