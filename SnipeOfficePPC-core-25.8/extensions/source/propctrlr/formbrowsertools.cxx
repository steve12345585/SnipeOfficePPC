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

#include "formbrowsertools.hxx"
#include <com/sun/star/form/FormComponentType.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include "formresid.hrc"
#include "modulepcr.hxx"
#include <tools/string.hxx>
#include "formstrings.hxx"

//............................................................................
namespace pcr
{
//............................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::form;
    using namespace ::com::sun::star::lang;
    using namespace ::com::sun::star::beans;

    //------------------------------------------------------------------------
    ::rtl::OUString GetUIHeadlineName(sal_Int16 nClassId, const Any& aUnoObj)
    {
        PcrClient aResourceAccess;
            // this ensures that we have our resource file loaded

        ::rtl::OUString sClassName;
        switch (nClassId)
        {
            case FormComponentType::TEXTFIELD:
            {
                Reference< XInterface >  xIFace;
                aUnoObj >>= xIFace;
                sClassName = String(PcrRes(RID_STR_PROPTITLE_EDIT));
                if (xIFace.is())
                {   // we have a chance to check if it's a formatted field model
                    Reference< XServiceInfo >  xInfo(xIFace, UNO_QUERY);
                    if (xInfo.is() && (xInfo->supportsService(SERVICE_COMPONENT_FORMATTEDFIELD)))
                        sClassName = String(PcrRes(RID_STR_PROPTITLE_FORMATTED));
                    else if (!xInfo.is())
                    {
                        // couldn't distinguish between formatted and edit with the service name, so try with the properties
                        Reference< XPropertySet >  xProps(xIFace, UNO_QUERY);
                        if (xProps.is())
                        {
                            Reference< XPropertySetInfo >  xPropsInfo = xProps->getPropertySetInfo();
                            if (xPropsInfo.is() && xPropsInfo->hasPropertyByName(PROPERTY_FORMATSSUPPLIER))
                                sClassName = String(PcrRes(RID_STR_PROPTITLE_FORMATTED));
                        }
                    }
                }
            }
            break;

            case FormComponentType::COMMANDBUTTON:
                sClassName = String(PcrRes(RID_STR_PROPTITLE_PUSHBUTTON)); break;
            case FormComponentType::RADIOBUTTON:
                sClassName = String(PcrRes(RID_STR_PROPTITLE_RADIOBUTTON)); break;
            case FormComponentType::CHECKBOX:
                sClassName = String(PcrRes(RID_STR_PROPTITLE_CHECKBOX)); break;
            case FormComponentType::LISTBOX:
                sClassName = String(PcrRes(RID_STR_PROPTITLE_LISTBOX)); break;
            case FormComponentType::COMBOBOX:
                sClassName = String(PcrRes(RID_STR_PROPTITLE_COMBOBOX)); break;
            case FormComponentType::GROUPBOX:
                sClassName = String(PcrRes(RID_STR_PROPTITLE_GROUPBOX)); break;
            case FormComponentType::IMAGEBUTTON:
                sClassName = String(PcrRes(RID_STR_PROPTITLE_IMAGEBUTTON)); break;
            case FormComponentType::FIXEDTEXT:
                sClassName = String(PcrRes(RID_STR_PROPTITLE_FIXEDTEXT)); break;
            case FormComponentType::GRIDCONTROL:
                sClassName = String(PcrRes(RID_STR_PROPTITLE_DBGRID)); break;
            case FormComponentType::FILECONTROL:
                sClassName = String(PcrRes(RID_STR_PROPTITLE_FILECONTROL)); break;

            case FormComponentType::DATEFIELD:
                sClassName = String(PcrRes(RID_STR_PROPTITLE_DATEFIELD)); break;
            case FormComponentType::TIMEFIELD:
                sClassName = String(PcrRes(RID_STR_PROPTITLE_TIMEFIELD)); break;
            case FormComponentType::NUMERICFIELD:
                sClassName = String(PcrRes(RID_STR_PROPTITLE_NUMERICFIELD)); break;
            case FormComponentType::CURRENCYFIELD:
                sClassName = String(PcrRes(RID_STR_PROPTITLE_CURRENCYFIELD)); break;
            case FormComponentType::PATTERNFIELD:
                sClassName = String(PcrRes(RID_STR_PROPTITLE_PATTERNFIELD)); break;
            case FormComponentType::IMAGECONTROL:
                sClassName = String(PcrRes(RID_STR_PROPTITLE_IMAGECONTROL)); break;
            case FormComponentType::HIDDENCONTROL:
                sClassName = String(PcrRes(RID_STR_PROPTITLE_HIDDENCONTROL)); break;

            case FormComponentType::CONTROL:
            default:
                sClassName = String(PcrRes(RID_STR_PROPTITLE_UNKNOWNCONTROL)); break;
        }

        return sClassName;
    }

    //------------------------------------------------------------------------
    sal_Int16 classifyComponent( const Reference< XInterface >& _rxComponent )
    {
        Reference< XPropertySet > xComponentProps( _rxComponent, UNO_QUERY_THROW );
        Reference< XPropertySetInfo > xPSI( xComponentProps->getPropertySetInfo(), UNO_SET_THROW );

        sal_Int16 nControlType( FormComponentType::CONTROL );
        if ( xPSI->hasPropertyByName( PROPERTY_CLASSID ) )
        {
            OSL_VERIFY( xComponentProps->getPropertyValue( PROPERTY_CLASSID ) >>= nControlType );
        }
        return nControlType;
    }

//............................................................................
} // namespace pcr
//............................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
