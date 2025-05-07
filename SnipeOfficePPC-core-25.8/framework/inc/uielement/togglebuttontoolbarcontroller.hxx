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

#ifndef __FRAMEWORK_UIELEMENT_TOGGLEBUTTONTOOLBARCONTROLLER_HXX_
#define __FRAMEWORK_UIELEMENT_TOGGLEBUTTONTOOLBARCONTROLLER_HXX_

#include <com/sun/star/beans/NamedValue.hpp>
#include <com/sun/star/frame/XDispatch.hpp>
#include <com/sun/star/frame/ControlCommand.hpp>

//_________________________________________________________________________________________________________________
//  includes of other projects
//_________________________________________________________________________________________________________________

#include <uielement/complextoolbarcontroller.hxx>
#include <vcl/toolbox.hxx>
#include <vcl/image.hxx>

namespace framework
{

class ToolBar;
class ToggleButtonToolbarController : public ComplexToolbarController

{
    public:
        enum Style
        {
            STYLE_TOGGLEBUTTON,
            STYLE_DROPDOWNBUTTON,
            STYLE_TOGGLE_DROPDOWNBUTTON
        };

        ToggleButtonToolbarController( const com::sun::star::uno::Reference< com::sun::star::lang::XMultiServiceFactory >& rServiceManager,
                                       const com::sun::star::uno::Reference< com::sun::star::frame::XFrame >& rFrame,
                                       ToolBox*             pToolBar,
                                       sal_uInt16               nID,
                                       Style                eStyle,
                                       const rtl::OUString& aCommand );
        virtual ~ToggleButtonToolbarController();

        // XComponent
        virtual void SAL_CALL dispose() throw ( ::com::sun::star::uno::RuntimeException );

        // XToolbarController
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow > SAL_CALL createPopupWindow() throw (::com::sun::star::uno::RuntimeException);

    protected:
        virtual void executeControlCommand( const ::com::sun::star::frame::ControlCommand& rControlCommand );
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue> getExecuteArgs(sal_Int16 KeyModifier) const;

    private:
        DECL_LINK( MenuSelectHdl, Menu *);

        Style                        m_eStyle;
        rtl::OUString                m_aCurrentSelection;
        std::vector< rtl::OUString > m_aDropdownMenuList;
};

}

#endif // __FRAMEWORK_UIELEMENT_TOGGLEBUTTONTOOLBARCONTROLLER_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
