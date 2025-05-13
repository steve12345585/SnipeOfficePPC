/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
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


#include "appbaslib.hxx"

#include <sfx2/sfxuno.hxx>
#include "sfxtypes.hxx"
#include <sfx2/app.hxx>

#include <basic/basmgr.hxx>
#include <tools/diagnose_ex.h>
#include <comphelper/processfactory.hxx>

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::script;
using namespace ::com::sun::star::embed;
using ::rtl::OUString;
using ::osl::MutexGuard;
using ::osl::Mutex;

//============================================================================
SfxBasicManagerHolder::SfxBasicManagerHolder()
    :mpBasicManager( NULL )
{
}

void SfxBasicManagerHolder::reset( BasicManager* _pBasicManager )
{
    impl_releaseContainers();

#ifdef DISABLE_SCRIPTING
    (void) _pBasicManager;
#else
    // Note: we do not delete the old BasicManager. BasicManager instances are
    // nowadays obtained from the BasicManagerRepository, and the ownership is with
    // the repository.
    // @see basic::BasicManagerRepository::getApplicationBasicManager
    // @see basic::BasicManagerRepository::getDocumentBasicManager
    mpBasicManager = _pBasicManager;

    if ( mpBasicManager )
    {
        try
        {
            mxBasicContainer.set( mpBasicManager->GetScriptLibraryContainer(), UNO_QUERY_THROW );
            mxDialogContainer.set( mpBasicManager->GetDialogLibraryContainer(), UNO_QUERY_THROW  );
        }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
    }
#endif
}

void SfxBasicManagerHolder::storeAllLibraries()
{
#ifndef DISABLE_SCRIPTING
    OSL_PRECOND( isValid(), "SfxBasicManagerHolder::storeAllLibraries: not initialized!" );
    try
    {
        if ( mxBasicContainer.is() )
            mxBasicContainer->storeLibraries();
        if ( mxDialogContainer.is() )
            mxDialogContainer->storeLibraries();
    }
    catch( const Exception& )
    {
        DBG_UNHANDLED_EXCEPTION();
    }
#endif
}

void SfxBasicManagerHolder::setStorage( const Reference< XStorage >& _rxStorage )
{
#ifdef DISABLE_SCRIPTING
    (void) _rxStorage;
#else
    try
    {
        if ( mxBasicContainer.is() )
            mxBasicContainer->setRootStorage( _rxStorage );
        if ( mxDialogContainer.is() )
            mxDialogContainer->setRootStorage( _rxStorage );
    }
    catch( const Exception& )
    {
        DBG_UNHANDLED_EXCEPTION();
    }
#endif
}

void SfxBasicManagerHolder::storeLibrariesToStorage( const Reference< XStorage >& _rxStorage )
{
#ifdef DISABLE_SCRIPTING
    (void) _rxStorage;
#else
    OSL_PRECOND( isValid(), "SfxBasicManagerHolder::storeLibrariesToStorage: not initialized!" );

    if ( mxBasicContainer.is() )
        mxBasicContainer->storeLibrariesToStorage( _rxStorage );
    if ( mxDialogContainer.is() )
        mxDialogContainer->storeLibrariesToStorage( _rxStorage );
#endif
}

Reference< XLibraryContainer > SfxBasicManagerHolder::getLibraryContainer( ContainerType _eType )
{
    OSL_PRECOND( isValid(), "SfxBasicManagerHolder::getLibraryContainer: not initialized!" );

    switch ( _eType )
    {
    case SCRIPTS:   return mxBasicContainer.get();
    case DIALOGS:   return mxDialogContainer.get();
    }
    OSL_FAIL( "SfxBasicManagerHolder::getLibraryContainer: illegal container type!" );
    return NULL;
}

void SfxBasicManagerHolder::impl_releaseContainers()
{
    mxBasicContainer.clear();
    mxDialogContainer.clear();
}

sal_Bool
SfxBasicManagerHolder::LegacyPsswdBinaryLimitExceeded( Sequence< rtl::OUString >& sModules )
{
#ifdef DISABLE_SCRIPTING
    (void) sModules;
#else
    if ( mpBasicManager )
        return mpBasicManager->LegacyPsswdBinaryLimitExceeded( sModules );
#endif
    return sal_True;
}

//============================================================================
// Service for application library container
SFX_IMPL_ONEINSTANCEFACTORY( SfxApplicationDialogLibraryContainer )

Sequence< OUString > SfxApplicationDialogLibraryContainer::impl_getStaticSupportedServiceNames()
{
    static Sequence< OUString > seqServiceNames( 1 );
    static sal_Bool bNeedsInit = sal_True;

    MutexGuard aGuard( Mutex::getGlobalMutex() );
    if( bNeedsInit )
    {
        OUString* pSeq = seqServiceNames.getArray();
        pSeq[0] = OUString("com.sun.star.script.ApplicationDialogLibraryContainer");
        bNeedsInit = sal_False;
    }
    return seqServiceNames;
}

OUString SfxApplicationDialogLibraryContainer::impl_getStaticImplementationName()
{
    return OUString("com.sun.star.comp.sfx2.ApplicationDialogLibraryContainer");
}

Reference< XInterface > SAL_CALL SfxApplicationDialogLibraryContainer::impl_createInstance
    ( const Reference< XMultiServiceFactory >& )
        throw( Exception )
{
    SFX_APP()->GetBasicManager();
    Reference< XInterface > xRet =
        Reference< XInterface >( SFX_APP()->GetDialogContainer(), UNO_QUERY );
    return xRet;
}

//============================================================================
// Service for application library container
SFX_IMPL_ONEINSTANCEFACTORY( SfxApplicationScriptLibraryContainer )

Sequence< OUString > SfxApplicationScriptLibraryContainer::impl_getStaticSupportedServiceNames()
{
    Sequence< OUString > seqServiceNames( 1 );
    OUString* pSeq = seqServiceNames.getArray();
    pSeq[0] = OUString("com.sun.star.script.ApplicationScriptLibraryContainer");
    return seqServiceNames;
}

OUString SfxApplicationScriptLibraryContainer::impl_getStaticImplementationName()
{
    return OUString("com.sun.star.comp.sfx2.ApplicationScriptLibraryContainer");
}

Reference< XInterface > SAL_CALL SfxApplicationScriptLibraryContainer::impl_createInstance
    ( const Reference< XMultiServiceFactory >& )
        throw( Exception )
{
    SFX_APP()->GetBasicManager();
    Reference< XInterface > xRet =
        Reference< XInterface >( SFX_APP()->GetBasicContainer(), UNO_QUERY );
    return xRet;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
