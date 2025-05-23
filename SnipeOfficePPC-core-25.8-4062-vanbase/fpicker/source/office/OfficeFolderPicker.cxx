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

#include "OfficeFolderPicker.hxx"

#include "iodlg.hxx"

#include <list>
#include <tools/urlobj.hxx>
#include <com/sun/star/container/XContentEnumerationAccess.hpp>
#include <com/sun/star/container/XSet.hpp>
#include <com/sun/star/uno/Any.hxx>
#include <cppuhelper/factory.hxx>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <unotools/pathoptions.hxx>

using namespace     ::com::sun::star::container;
using namespace     ::com::sun::star::lang;
using namespace     ::com::sun::star::uno;
using namespace     ::com::sun::star::beans;

SvtFolderPicker::SvtFolderPicker( const Reference < XMultiServiceFactory >& _rxFactory )
    :SvtFolderPicker_Base( _rxFactory )
{
}

SvtFolderPicker::~SvtFolderPicker()
{
}

void SAL_CALL SvtFolderPicker::setTitle( const ::rtl::OUString& _rTitle ) throw (RuntimeException)
{
    OCommonPicker::setTitle( _rTitle );
}

sal_Int16 SAL_CALL SvtFolderPicker::execute(  ) throw (RuntimeException)
{
    return OCommonPicker::execute();
}

void SAL_CALL SvtFolderPicker::setDialogTitle( const ::rtl::OUString& _rTitle) throw (RuntimeException)
{
    setTitle( _rTitle );
}

void SAL_CALL SvtFolderPicker::startExecuteModal( const Reference< ::com::sun::star::ui::dialogs::XDialogClosedListener >& xListener ) throw (RuntimeException)
{
    m_xListener = xListener;
    prepareDialog();
    prepareExecute();
    getDialog()->EnableAutocompletion( sal_True );
    getDialog()->StartExecuteModal( LINK( this, SvtFolderPicker, DialogClosedHdl ) );
}

SvtFileDialog* SvtFolderPicker::implCreateDialog( Window* _pParent )
{
    return new SvtFileDialog( _pParent, SFXWB_PATHDIALOG );
}

sal_Int16 SvtFolderPicker::implExecutePicker( )
{
    prepareExecute();

    // now we are ready to execute the dialog
    getDialog()->EnableAutocompletion( sal_False );
    sal_Int16 nRet = getDialog()->Execute();

    return nRet;
}

void SvtFolderPicker::prepareExecute()
{
    // set the default directory
    if ( !m_aDisplayDirectory.isEmpty() )
        getDialog()->SetPath( m_aDisplayDirectory );
    else
    {
        // Default-Standard-Dir setzen
        INetURLObject aStdDirObj( SvtPathOptions().GetWorkPath() );
        getDialog()->SetPath( aStdDirObj.GetMainURL( INetURLObject::NO_DECODE) );
    }
}

IMPL_LINK( SvtFolderPicker, DialogClosedHdl, Dialog*, pDlg )
{
    if ( m_xListener.is() )
    {
        sal_Int16 nRet = static_cast< sal_Int16 >( pDlg->GetResult() );
        ::com::sun::star::ui::dialogs::DialogClosedEvent aEvent( *this, nRet );
        m_xListener->dialogClosed( aEvent );
        m_xListener.clear();
    }
    return 0;
  }

void SAL_CALL SvtFolderPicker::setDisplayDirectory( const ::rtl::OUString& aDirectory )
    throw( IllegalArgumentException, RuntimeException )
{
    m_aDisplayDirectory = aDirectory;
}

::rtl::OUString SAL_CALL SvtFolderPicker::getDisplayDirectory() throw( RuntimeException )
{
    if ( ! getDialog() )
        return m_aDisplayDirectory;

    std::vector<rtl::OUString> aPathList(getDialog()->GetPathList());

    if(!aPathList.empty())
        return aPathList[0];

    return rtl::OUString();
}

::rtl::OUString SAL_CALL SvtFolderPicker::getDirectory() throw( RuntimeException )
{
    if ( ! getDialog() )
        return m_aDisplayDirectory;

    std::vector<rtl::OUString> aPathList(getDialog()->GetPathList());

    if(!aPathList.empty())
        return aPathList[0];

    return rtl::OUString();
}

void SAL_CALL SvtFolderPicker::setDescription( const ::rtl::OUString& aDescription )
    throw( RuntimeException )
{
    m_aDescription = aDescription;
}

void SvtFolderPicker::cancel() throw (RuntimeException)
{
    OCommonPicker::cancel();
}

/* XServiceInfo */
::rtl::OUString SAL_CALL SvtFolderPicker::getImplementationName() throw( RuntimeException )
{
    return impl_getStaticImplementationName();
}

/* XServiceInfo */
sal_Bool SAL_CALL SvtFolderPicker::supportsService( const ::rtl::OUString& sServiceName ) throw( RuntimeException )
{
    Sequence< ::rtl::OUString > seqServiceNames = getSupportedServiceNames();
    const ::rtl::OUString* pArray = seqServiceNames.getConstArray();
    for ( sal_Int32 i = 0; i < seqServiceNames.getLength(); i++ )
    {
        if ( sServiceName == pArray[i] )
        {
            return sal_True ;
        }
    }
    return sal_False ;
}

/* XServiceInfo */
Sequence< ::rtl::OUString > SAL_CALL SvtFolderPicker::getSupportedServiceNames() throw( RuntimeException )
{
    return impl_getStaticSupportedServiceNames();
}

/* Helper for XServiceInfo */
Sequence< ::rtl::OUString > SvtFolderPicker::impl_getStaticSupportedServiceNames()
{
    Sequence< ::rtl::OUString > seqServiceNames(1);
    seqServiceNames[0] = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.ui.dialogs.OfficeFolderPicker" ));
    return seqServiceNames ;
}

/* Helper for XServiceInfo */
::rtl::OUString SvtFolderPicker::impl_getStaticImplementationName()
{
    return ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.svtools.OfficeFolderPicker" ));
}

/* Helper for registry */
Reference< XInterface > SAL_CALL SvtFolderPicker::impl_createInstance( const Reference< XComponentContext >& rxContext )
    throw( Exception )
{
    Reference< XMultiServiceFactory > xServiceManager (rxContext->getServiceManager(), UNO_QUERY_THROW);
    return Reference< XInterface >( *new SvtFolderPicker( xServiceManager ) );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
