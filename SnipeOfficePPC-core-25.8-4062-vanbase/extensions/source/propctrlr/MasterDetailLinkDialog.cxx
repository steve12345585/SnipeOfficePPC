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

 #include "MasterDetailLinkDialog.hxx"
 #include "formlinkdialog.hxx"

 extern "C" void SAL_CALL createRegistryInfo_MasterDetailLinkDialog()
{
    ::pcr::OAutoRegistration< ::pcr::MasterDetailLinkDialog > aAutoRegistration;
}

//............................................................................
namespace pcr
{
//............................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::beans;

    //====================================================================
    //= MasterDetailLinkDialog
    //====================================================================
    //---------------------------------------------------------------------
    MasterDetailLinkDialog::MasterDetailLinkDialog(const Reference< XComponentContext >& _rxContext )
        :OGenericUnoDialog( _rxContext )
    {
    }
//---------------------------------------------------------------------
    Sequence<sal_Int8> SAL_CALL MasterDetailLinkDialog::getImplementationId(  ) throw(RuntimeException)
    {
        static ::cppu::OImplementationId aId;
        return aId.getImplementationId();
    }

    //---------------------------------------------------------------------
    Reference< XInterface > SAL_CALL MasterDetailLinkDialog::Create( const Reference< XComponentContext >& _rxContext )
    {
        return *( new MasterDetailLinkDialog( _rxContext ) );
    }

    //---------------------------------------------------------------------
    ::rtl::OUString SAL_CALL MasterDetailLinkDialog::getImplementationName() throw(RuntimeException)
    {
        return getImplementationName_static();
    }

    //---------------------------------------------------------------------
    ::rtl::OUString MasterDetailLinkDialog::getImplementationName_static() throw(RuntimeException)
    {
        return ::rtl::OUString("org.openoffice.comp.form.ui.MasterDetailLinkDialog");
    }

    //---------------------------------------------------------------------
    ::comphelper::StringSequence SAL_CALL MasterDetailLinkDialog::getSupportedServiceNames() throw(RuntimeException)
    {
        return getSupportedServiceNames_static();
    }

    //---------------------------------------------------------------------
    ::comphelper::StringSequence MasterDetailLinkDialog::getSupportedServiceNames_static() throw(RuntimeException)
    {
        ::comphelper::StringSequence aSupported(1);
        aSupported.getArray()[0] = ::rtl::OUString("com.sun.star.form.MasterDetailLinkDialog");
        return aSupported;
    }

    //---------------------------------------------------------------------
    Reference<XPropertySetInfo>  SAL_CALL MasterDetailLinkDialog::getPropertySetInfo() throw(RuntimeException)
    {
        Reference<XPropertySetInfo>  xInfo( createPropertySetInfo( getInfoHelper() ) );
        return xInfo;
    }

    //---------------------------------------------------------------------
    ::cppu::IPropertyArrayHelper& MasterDetailLinkDialog::getInfoHelper()
    {
        return *const_cast<MasterDetailLinkDialog*>(this)->getArrayHelper();
    }

    //--------------------------------------------------------------------------
    ::cppu::IPropertyArrayHelper* MasterDetailLinkDialog::createArrayHelper( ) const
    {
        Sequence< Property > aProps;
        describeProperties(aProps);
        return new ::cppu::OPropertyArrayHelper(aProps);
    }

    //--------------------------------------------------------------------------
    Dialog* MasterDetailLinkDialog::createDialog(Window* _pParent)
    {
        return new FormLinkDialog(_pParent,m_xDetail,m_xMaster,m_aContext.getLegacyServiceFactory()
            ,m_sExplanation,m_sDetailLabel,m_sMasterLabel);
    }
    //---------------------------------------------------------------------
    void MasterDetailLinkDialog::implInitialize(const Any& _rValue)
    {
        PropertyValue aProperty;
        if (_rValue >>= aProperty)
        {
            if (0 == aProperty.Name.compareToAscii("Detail"))
            {
                OSL_VERIFY( aProperty.Value >>= m_xDetail );
                return;
            }
            else if (0 == aProperty.Name.compareToAscii("Master"))
            {
                OSL_VERIFY( aProperty.Value >>= m_xMaster );
                return;
            }
            else if (0 == aProperty.Name.compareToAscii("Explanation"))
            {
                OSL_VERIFY( aProperty.Value >>= m_sExplanation );
                return;
            }
            else if (0 == aProperty.Name.compareToAscii("DetailLabel"))
            {
                OSL_VERIFY( aProperty.Value >>= m_sDetailLabel );
                return;
            }
            else if (0 == aProperty.Name.compareToAscii("MasterLabel"))
            {
                OSL_VERIFY( aProperty.Value >>= m_sMasterLabel );
                return;
            }
        }
        MasterDetailLinkDialog_DBase::implInitialize(_rValue);
    }

//............................................................................
}   // namespace pcr
//............................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
