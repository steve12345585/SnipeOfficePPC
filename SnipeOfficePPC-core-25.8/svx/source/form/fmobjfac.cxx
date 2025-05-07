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

#include <comphelper/stl_types.hxx>
#include <svx/svdobj.hxx>
#include "svx/fmtools.hxx"
#include "fmservs.hxx"

#include "svx/fmobjfac.hxx"

#include <svx/fmglob.hxx>

#include "fmobj.hxx"
#include "fmshimp.hxx"

#include <svx/fmshell.hxx>

#include <svx/svxids.hrc>
#include "tbxform.hxx"
#include <tools/resid.hxx>

#include "svx/fmresids.hrc"
#include <tools/shl.hxx>
#include <svx/dialmgr.hxx>
#include "tabwin.hxx"
#include "fmexpl.hxx"
#include "filtnav.hxx"

#include "fmprop.hrc"
#include "fmPropBrw.hxx"
#include "datanavi.hxx"

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::svxform;

static sal_Bool bInit = sal_False;

/*************************************************************************
|*
|* Ctor
|*
\************************************************************************/
FmFormObjFactory::FmFormObjFactory()
{
    if ( !bInit )
    {
        SdrObjFactory::InsertMakeObjectHdl(LINK(this, FmFormObjFactory, MakeObject));

        //////////////////////////////////////////////////////////////////////
        // Konfigurations-::com::sun::star::frame::Controller und NavigationBar registrieren
        SvxFmTbxCtlConfig::RegisterControl( SID_FM_CONFIG );
        SvxFmTbxCtlAbsRec::RegisterControl( SID_FM_RECORD_ABSOLUTE );
        SvxFmTbxCtlRecText::RegisterControl( SID_FM_RECORD_TEXT );
        SvxFmTbxCtlRecFromText::RegisterControl( SID_FM_RECORD_FROM_TEXT );
        SvxFmTbxCtlRecTotal::RegisterControl( SID_FM_RECORD_TOTAL );
        SvxFmTbxPrevRec::RegisterControl( SID_FM_RECORD_PREV );
        SvxFmTbxNextRec::RegisterControl( SID_FM_RECORD_NEXT );
        ControlConversionMenuController::RegisterControl(SID_FM_CHANGECONTROLTYPE);

        // Registrieung von globalen fenstern
        FmFieldWinMgr::RegisterChildWindow();
        FmPropBrwMgr::RegisterChildWindow();
        NavigatorFrameManager::RegisterChildWindow();
        DataNavigatorManager::RegisterChildWindow();
        FmFilterNavigatorWinMgr::RegisterChildWindow();

        //////////////////////////////////////////////////////////////////////
        // Interface fuer die Formshell registrieren
        FmFormShell::RegisterInterface(0);

        ImplSmartRegisterUnoServices();
        bInit = sal_True;
    }
}


/*************************************************************************
|*
|* Dtor
|*
\************************************************************************/
FmFormObjFactory::~FmFormObjFactory()
{
}


/*************************************************************************
|*
|* ::com::sun::star::form::Form-Objekte erzeugen
|*
\************************************************************************/
namespace
{
    void    lcl_initProperty( FmFormObj* _pObject, const ::rtl::OUString& _rPropName, const Any& _rValue )
    {
        try
        {
            Reference< XPropertySet >  xModelSet( _pObject->GetUnoControlModel(), UNO_QUERY );
            if ( xModelSet.is() )
                xModelSet->setPropertyValue( _rPropName, _rValue );
        }
        catch( const Exception& )
        {
            OSL_FAIL( "lcl_initProperty: caught an exception!" );
        }
    }
}

IMPL_LINK(FmFormObjFactory, MakeObject, SdrObjFactory*, pObjFactory)
{
    if (pObjFactory->nInventor == FmFormInventor)
    {
        ::rtl::OUString sServiceSpecifier;

        typedef ::std::vector< ::std::pair< ::rtl::OUString, Any > > PropertyValueArray;
        PropertyValueArray aInitialProperties;

        switch ( pObjFactory->nIdentifier )
        {
            case OBJ_FM_EDIT:
                sServiceSpecifier = FM_COMPONENT_EDIT;
                break;

            case OBJ_FM_BUTTON:
                sServiceSpecifier = FM_COMPONENT_COMMANDBUTTON;
                break;

            case OBJ_FM_FIXEDTEXT:
                sServiceSpecifier = FM_COMPONENT_FIXEDTEXT;
                break;

            case OBJ_FM_LISTBOX:
                sServiceSpecifier = FM_COMPONENT_LISTBOX;
                break;

            case OBJ_FM_CHECKBOX:
                sServiceSpecifier = FM_COMPONENT_CHECKBOX;
                break;

            case OBJ_FM_RADIOBUTTON:
                sServiceSpecifier = FM_COMPONENT_RADIOBUTTON;
                break;

            case OBJ_FM_GROUPBOX:
                sServiceSpecifier = FM_COMPONENT_GROUPBOX;
                break;

            case OBJ_FM_COMBOBOX:
                sServiceSpecifier = FM_COMPONENT_COMBOBOX;
                break;

            case OBJ_FM_GRID:
                sServiceSpecifier = FM_COMPONENT_GRID;
                break;

            case OBJ_FM_IMAGEBUTTON:
                sServiceSpecifier = FM_COMPONENT_IMAGEBUTTON;
                break;

            case OBJ_FM_FILECONTROL:
                sServiceSpecifier = FM_COMPONENT_FILECONTROL;
                break;

            case OBJ_FM_DATEFIELD:
                sServiceSpecifier = FM_COMPONENT_DATEFIELD;
                break;

            case OBJ_FM_TIMEFIELD:
                sServiceSpecifier = FM_COMPONENT_TIMEFIELD;
                aInitialProperties.push_back( PropertyValueArray::value_type( FM_PROP_TIMEMAX, makeAny( (sal_Int32)( Time( 23, 59, 59, 99 ).GetTime() ) ) ) );
                break;

            case OBJ_FM_NUMERICFIELD:
                sServiceSpecifier = FM_COMPONENT_NUMERICFIELD;
                break;

            case OBJ_FM_CURRENCYFIELD:
                sServiceSpecifier = FM_COMPONENT_CURRENCYFIELD;
                break;

            case OBJ_FM_PATTERNFIELD:
                sServiceSpecifier = FM_COMPONENT_PATTERNFIELD;
                break;

            case OBJ_FM_HIDDEN:
                sServiceSpecifier = FM_COMPONENT_HIDDEN;
                break;

            case OBJ_FM_IMAGECONTROL:
                sServiceSpecifier = FM_COMPONENT_IMAGECONTROL;
                break;

            case OBJ_FM_FORMATTEDFIELD:
                sServiceSpecifier = FM_COMPONENT_FORMATTEDFIELD;
                break;

            case OBJ_FM_NAVIGATIONBAR:
                sServiceSpecifier = FM_SUN_COMPONENT_NAVIGATIONBAR;
                break;

            case OBJ_FM_SCROLLBAR:
                sServiceSpecifier = FM_SUN_COMPONENT_SCROLLBAR;
                aInitialProperties.push_back( PropertyValueArray::value_type( FM_PROP_BORDER, makeAny( (sal_Int16)0 ) ) );
                break;

            case OBJ_FM_SPINBUTTON:
                sServiceSpecifier = FM_SUN_COMPONENT_SPINBUTTON;
                aInitialProperties.push_back( PropertyValueArray::value_type( FM_PROP_BORDER, makeAny( (sal_Int16)0 ) ) );
                break;
        }

        // create the actual object
        if ( !sServiceSpecifier.isEmpty() )
            pObjFactory->pNewObj = new FmFormObj(sServiceSpecifier);
        else
            pObjFactory->pNewObj = new FmFormObj();

        // initialize some properties which we want to differ from the defaults
        for (   PropertyValueArray::const_iterator aInitProp = aInitialProperties.begin();
                aInitProp != aInitialProperties.end();
                ++aInitProp
            )
        {
            lcl_initProperty(
                static_cast< FmFormObj* >( pObjFactory->pNewObj ),
                aInitProp->first,
                aInitProp->second
            );
        }
    }

    return 0;
}



/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
