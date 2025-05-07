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


//----------------------------------------------
//  includes of other projects
//----------------------------------------------
#include <cppuhelper/factory.hxx>
#include <com/sun/star/container/XSet.hpp>
#include <osl/diagnose.h>
#include "FilePicker.hxx"
#include "FPServiceInfo.hxx"

#ifdef _MSC_VER
#pragma warning (disable:4917)
#endif
#include "VistaFilePicker.hxx"
#include "../misc/WinImplHelper.hxx"
#include <stdio.h>

#include "../folderpicker/FolderPicker.hxx"
#include "../folderpicker/FOPServiceInfo.hxx"
#include "../folderpicker/WinFOPImpl.hxx"

//-----------------------------------------------
// namespace directives
//-----------------------------------------------

using namespace ::rtl                       ;
using namespace ::com::sun::star::uno       ;
using namespace ::com::sun::star::container ;
using namespace ::com::sun::star::lang      ;
using namespace ::com::sun::star::registry  ;
using namespace ::cppu                      ;
using ::com::sun::star::ui::dialogs::XFilePicker;
using ::com::sun::star::ui::dialogs::XFilePicker2;
using ::com::sun::star::ui::dialogs::XFolderPicker;

//------------------------------------------------
//
//------------------------------------------------

static Reference< XInterface > SAL_CALL createInstance(
    const Reference< XMultiServiceFactory >& rServiceManager )
{
    Reference< XInterface > xDlg;

#ifdef __IFileDialogCustomize_INTERFACE_DEFINED__
    bool                    bVistaOrNewer = IsWindowsVistaOrNewer();

    if (bVistaOrNewer)
    {
        OSL_TRACE("use special (vista) system file picker ...");
        xDlg.set(
            static_cast< XFilePicker2* >(
                new ::fpicker::win32::vista::VistaFilePicker( rServiceManager ) ) );
    }
    else
#endif
    {
        OSL_TRACE("use normal system file picker ...");
        xDlg.set(
            static_cast< XFilePicker2* >(
                new CFilePicker( rServiceManager ) ) );
    }

    return xDlg;
}

static Reference< XInterface > SAL_CALL
createInstance_fop( const Reference< XMultiServiceFactory >& rServiceManager )
{
    return Reference< XInterface >( static_cast< XFolderPicker* >( new CFolderPicker( rServiceManager ) ) );
}


extern "C"
{

SAL_DLLPUBLIC_EXPORT void* SAL_CALL component_getFactory(
    const sal_Char* pImplName, uno_Interface* pSrvManager, uno_Interface* )
{
    void* pRet = 0;

    if ( pSrvManager && ( 0 == rtl_str_compare( pImplName, FILE_PICKER_IMPL_NAME ) ) )
    {
        Sequence< OUString > aSNS( 1 );
        aSNS.getArray( )[0] = OUString(RTL_CONSTASCII_USTRINGPARAM( FILE_PICKER_SERVICE_NAME ));

        Reference< XSingleServiceFactory > xFactory ( createSingleFactory(
            reinterpret_cast< XMultiServiceFactory* > ( pSrvManager ),
            OUString::createFromAscii( pImplName ),
            createInstance,
            aSNS ) );
        if ( xFactory.is() )
        {
            xFactory->acquire();
            pRet = xFactory.get();
        }
    }

    if ( pSrvManager && ( 0 == rtl_str_compare( pImplName, FOLDER_PICKER_IMPL_NAME ) ) )
    {
        Sequence< OUString > aSNS( 1 );
        aSNS.getArray( )[0] = OUString( RTL_CONSTASCII_USTRINGPARAM( FOLDER_PICKER_SERVICE_NAME ) );

        Reference< XSingleServiceFactory > xFactory ( createSingleFactory(
            reinterpret_cast< XMultiServiceFactory* > ( pSrvManager ),
            OUString::createFromAscii( pImplName ),
            createInstance_fop,
            aSNS ) );
        if ( xFactory.is() )
        {
            xFactory->acquire();
            pRet = xFactory.get();
        }
    }

    return pRet;
}

} // extern "C"

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
