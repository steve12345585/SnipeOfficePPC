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

#include <com/sun/star/embed/Aspects.hpp>
#include <com/sun/star/embed/EmbedStates.hpp>
#include <com/sun/star/datatransfer/XTransferable.hpp>
#include <com/sun/star/uno/Sequence.hxx>
#include <com/sun/star/lang/DisposedException.hpp>

#include <rtl/logfile.hxx>


#include <commonembobj.hxx>


using namespace ::com::sun::star;

void SAL_CALL OCommonEmbeddedObject::setVisualAreaSize( sal_Int64 nAspect, const awt::Size& aSize )
        throw ( lang::IllegalArgumentException,
                embed::WrongStateException,
                uno::Exception,
                uno::RuntimeException )
{
    RTL_LOGFILE_CONTEXT( aLog, "embeddedobj (mv76033) OCommonEmbeddedObject::setVisualAreaSize" );

    ::osl::MutexGuard aGuard( m_aMutex );
    if ( m_bDisposed )
        throw lang::DisposedException(); // TODO

    OSL_ENSURE( nAspect != embed::Aspects::MSOLE_ICON, "For iconified objects no graphical replacement is required!\n" );
    if ( nAspect == embed::Aspects::MSOLE_ICON )
        // no representation can be retrieved
        throw embed::WrongStateException( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Illegal call!\n" )),
                                    uno::Reference< uno::XInterface >( static_cast< ::cppu::OWeakObject* >(this) ) );

    if ( m_nObjectState == -1 )
        throw embed::WrongStateException( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "The own object has no persistence!\n" )),
                                    uno::Reference< uno::XInterface >( static_cast< ::cppu::OWeakObject* >(this) ) );

    m_bHasClonedSize = sal_False;

    sal_Bool bBackToLoaded = sal_False;
    if ( m_nObjectState == embed::EmbedStates::LOADED )
    {
        changeState( embed::EmbedStates::RUNNING );

        // the links should be switched back to loaded state for now to avoid locking problems
        bBackToLoaded = m_bIsLink;
    }

    sal_Bool bSuccess = m_pDocHolder->SetExtent( nAspect, aSize );

    if ( bBackToLoaded )
        changeState( embed::EmbedStates::LOADED );

    if ( !bSuccess )
        throw uno::Exception(); // TODO:
}

awt::Size SAL_CALL OCommonEmbeddedObject::getVisualAreaSize( sal_Int64 nAspect )
        throw ( lang::IllegalArgumentException,
                embed::WrongStateException,
                uno::Exception,
                uno::RuntimeException )
{
    RTL_LOGFILE_CONTEXT( aLog, "embeddedobj (mv76033) OCommonEmbeddedObject::getVisualAreaSize" );

    ::osl::MutexGuard aGuard( m_aMutex );
    if ( m_bDisposed )
        throw lang::DisposedException(); // TODO

    if ( m_nObjectState == -1 )
        throw embed::WrongStateException( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "The own object has no persistence!\n" )),
                                    uno::Reference< uno::XInterface >( static_cast< ::cppu::OWeakObject* >(this) ) );

    OSL_ENSURE( nAspect != embed::Aspects::MSOLE_ICON, "For iconified objects no graphical replacement is required!\n" );

    if ( m_bHasClonedSize )
        return m_aClonedSize;

    sal_Bool bBackToLoaded = sal_False;
    if ( m_nObjectState == embed::EmbedStates::LOADED )
    {
        changeState( embed::EmbedStates::RUNNING );

        // the links should be switched back to loaded state for now to avoid locking problems
        bBackToLoaded = m_bIsLink;
    }

    awt::Size aResult;
    sal_Bool bSuccess = m_pDocHolder->GetExtent( nAspect, &aResult );

    if ( bBackToLoaded )
        changeState( embed::EmbedStates::LOADED );

    if ( !bSuccess )
        throw uno::Exception(); // TODO:

    return aResult;
}

sal_Int32 SAL_CALL OCommonEmbeddedObject::getMapUnit( sal_Int64 nAspect )
        throw ( uno::Exception,
                uno::RuntimeException)
{
    ::osl::MutexGuard aGuard( m_aMutex );
    if ( m_bDisposed )
        throw lang::DisposedException(); // TODO

    OSL_ENSURE( nAspect != embed::Aspects::MSOLE_ICON, "For iconified objects no graphical replacement is required!\n" );
    if ( nAspect == embed::Aspects::MSOLE_ICON )
        // no representation can be retrieved
        throw embed::WrongStateException( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Illegal call!\n" )),
                                    uno::Reference< uno::XInterface >( static_cast< ::cppu::OWeakObject* >(this) ) );

    if ( m_nObjectState == -1 )
        throw embed::WrongStateException( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "The own object has no persistence!\n" )),
                                    uno::Reference< uno::XInterface >( static_cast< ::cppu::OWeakObject* >(this) ) );

    if ( m_bHasClonedSize )
        return m_nClonedMapUnit;

    sal_Bool bBackToLoaded = sal_False;
    if ( m_nObjectState == embed::EmbedStates::LOADED )
    {
        changeState( embed::EmbedStates::RUNNING );

        // the links should be switched back to loaded state for now to avoid locking problems
        bBackToLoaded = m_bIsLink;
    }

    sal_Int32 nResult = m_pDocHolder->GetMapUnit( nAspect );

    if ( bBackToLoaded )
        changeState( embed::EmbedStates::LOADED );

    if ( nResult < 0  )
        throw uno::Exception(); // TODO:

    return nResult;
}

embed::VisualRepresentation SAL_CALL OCommonEmbeddedObject::getPreferredVisualRepresentation( sal_Int64 nAspect )
        throw ( lang::IllegalArgumentException,
                embed::WrongStateException,
                uno::Exception,
                uno::RuntimeException )
{
    RTL_LOGFILE_CONTEXT( aLog, "embeddedobj (mv76033) OCommonEmbeddedObject::getPrefferedVisualRepresentation" );

    ::osl::MutexGuard aGuard( m_aMutex );
    if ( m_bDisposed )
        throw lang::DisposedException(); // TODO

    if ( m_nObjectState == -1 )
        throw embed::WrongStateException( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "The own object has no persistence!\n" )),
                                    uno::Reference< uno::XInterface >( static_cast< ::cppu::OWeakObject* >(this) ) );


    OSL_ENSURE( nAspect != embed::Aspects::MSOLE_ICON, "For iconified objects no graphical replacement is required!\n" );
    if ( nAspect == embed::Aspects::MSOLE_ICON )
        // no representation can be retrieved
        throw embed::WrongStateException( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Illegal call!\n" )),
                                    uno::Reference< uno::XInterface >( static_cast< ::cppu::OWeakObject* >(this) ) );

    sal_Bool bBackToLoaded = sal_False;
    if ( m_nObjectState == embed::EmbedStates::LOADED )
    {
        changeState( embed::EmbedStates::RUNNING );

        // the links should be switched back to loaded state for now to avoid locking problems
        bBackToLoaded = m_bIsLink;
    }

    OSL_ENSURE( m_pDocHolder->GetComponent().is(), "Running or Active object has no component!\n" );

    // TODO: return for the aspect of the document
    embed::VisualRepresentation aVisualRepresentation;

    uno::Reference< embed::XVisualObject > xVisualObject( m_pDocHolder->GetComponent(), uno::UNO_QUERY );
    if( xVisualObject.is())
    {
        aVisualRepresentation = xVisualObject->getPreferredVisualRepresentation( nAspect );
    }
    else
    {
        uno::Reference< datatransfer::XTransferable > xTransferable( m_pDocHolder->GetComponent(), uno::UNO_QUERY );
        if (!xTransferable.is() )
            throw uno::RuntimeException();

        datatransfer::DataFlavor aDataFlavor(
                ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "application/x-openoffice-gdimetafile;windows_formatname=\"GDIMetaFile\"" )),
                ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "GDIMetaFile" )),
                ::getCppuType( (const uno::Sequence< sal_Int8 >*) NULL ) );

        if( xTransferable->isDataFlavorSupported( aDataFlavor ))
        {
            aVisualRepresentation.Data = xTransferable->getTransferData( aDataFlavor );
            aVisualRepresentation.Flavor = aDataFlavor;
        }
        else
            throw uno::RuntimeException();
    }

    if ( bBackToLoaded )
        changeState( embed::EmbedStates::LOADED );

    return aVisualRepresentation;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
