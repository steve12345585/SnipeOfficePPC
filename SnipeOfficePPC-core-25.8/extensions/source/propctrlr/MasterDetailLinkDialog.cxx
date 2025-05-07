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
