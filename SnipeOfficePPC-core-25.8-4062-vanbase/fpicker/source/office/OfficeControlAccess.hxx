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

#ifndef SVTOOLS_CONTROLACCESS_HXX
#define SVTOOLS_CONTROLACCESS_HXX

#include <svtools/fileview.hxx>
#include <vcl/lstbox.hxx>
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#include "pickercallbacks.hxx"

//.........................................................................
namespace svt
{
//.........................................................................

    // --------------------------------------------------------------------
    namespace InternalFilePickerElementIds
    {
        static const sal_Int16 PUSHBUTTON_HELP = (sal_Int16)0x1000;
        static const sal_Int16 TOOLBOXBUTOON_DEFAULT_LOCATION = (sal_Int16)0x1001;
        static const sal_Int16 TOOLBOXBUTOON_LEVEL_UP = (sal_Int16)0x1002;
        static const sal_Int16 TOOLBOXBUTOON_NEW_FOLDER = (sal_Int16)0x1003;
        static const sal_Int16 FIXEDTEXT_CURRENTFOLDER = (sal_Int16)0x1004;
    }

    // --------------------------------------------------------------------
    /** implements the XControlAccess, XControlInformation and XFilePickerControlAccess for the file picker
    */
    class OControlAccess
    {
        IFilePickerController*  m_pFilePickerController;
        SvtFileView*            m_pFileView;

    public:
        OControlAccess( IFilePickerController* _pController, SvtFileView* _pFileView );

        // XControlAccess implementation
        void setControlProperty( const ::rtl::OUString& _rControlName, const ::rtl::OUString& _rControlProperty, const ::com::sun::star::uno::Any& _rValue );
        ::com::sun::star::uno::Any  getControlProperty( const ::rtl::OUString& _rControlName, const ::rtl::OUString& _rControlProperty );

        // XControlInformation implementation
        ::com::sun::star::uno::Sequence< ::rtl::OUString >  getSupportedControls(  );
        ::com::sun::star::uno::Sequence< ::rtl::OUString >  getSupportedControlProperties( const ::rtl::OUString& _rControlName );
        sal_Bool                                            isControlSupported( const ::rtl::OUString& _rControlName );
        sal_Bool                                            isControlPropertySupported( const ::rtl::OUString& _rControlName, const ::rtl::OUString& _rControlProperty );

        // XFilePickerControlAccess
        void                        setValue( sal_Int16 _nId, sal_Int16 _nCtrlAction, const ::com::sun::star::uno::Any& _rValue );
        ::com::sun::star::uno::Any  getValue( sal_Int16 _nId, sal_Int16 _nCtrlAction ) const;
        void                        setLabel( sal_Int16 _nId, const ::rtl::OUString& _rValue );
        ::rtl::OUString             getLabel( sal_Int16 _nId ) const;
        void                        enableControl( sal_Int16 _nId, sal_Bool _bEnable );

        static void             setHelpURL( Window* _pControl, const ::rtl::OUString& _rURL, sal_Bool _bFileView );
        static ::rtl::OUString  getHelpURL( Window* _pControl, sal_Bool _bFileView );

    private:
        /** implements the various methods for setting properties on controls

            @param _nControlId
                the id of the control
            @param _pControl
                the affected control. Must be the same as referred by <arg>_nControlId</arg>, or NULL.
            @param _nProperty
                the property to set
                See PROPERTY_FLAG_*
            @param _rValue
                the value to set
            @param _bIgnoreIllegalArgument
                if <FALSE/>, an exception will be thrown if the given value is of improper type
        */
        void                        implSetControlProperty(
                                        sal_Int16 _nControlId,
                                        Control* _pControl, sal_Int16 _nProperty, const ::com::sun::star::uno::Any& _rValue,
                                        sal_Bool _bIgnoreIllegalArgument = sal_True );

        Control* implGetControl( const ::rtl::OUString& _rControlName, sal_Int16* _pId = NULL, sal_Int32* _pPropertyMask = NULL ) const SAL_THROW( (::com::sun::star::lang::IllegalArgumentException) );

        /** implements the various methods for retrieving properties from controls

            @param _pControl
                the affected control
                @PRECOND not <NULL/>
            @param _nProperty
                the property to retrieve
                See PROPERTY_FLAG_*
            @return
        */
        ::com::sun::star::uno::Any  implGetControlProperty( Control* _pControl, sal_Int16 _nProperty ) const;

        void implDoListboxAction( ListBox* _pListbox, sal_Int16 _nCtrlAction, const ::com::sun::star::uno::Any& _rValue );

    };

//.........................................................................
}   // namespace svt
//.........................................................................

#endif // SVTOOLS_CONTROLACCESS_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
