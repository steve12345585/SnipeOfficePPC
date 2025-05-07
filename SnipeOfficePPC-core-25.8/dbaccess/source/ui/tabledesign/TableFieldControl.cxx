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

#include "TableFieldControl.hxx"
#include "TableController.hxx"
#include "TableDesignView.hxx"
#include "TEditControl.hxx"
#include "dbustrings.hrc"
#include <com/sun/star/sdbc/XDatabaseMetaData.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <comphelper/types.hxx>
#include "TypeInfo.hxx"
#include "UITools.hxx"

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::util;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::sdbc;
using namespace dbaui;
OTableFieldControl::OTableFieldControl( Window* pParent, OTableDesignHelpBar* pHelpBar) :OFieldDescControl(pParent,pHelpBar)
{
}
//------------------------------------------------------------------
void OTableFieldControl::CellModified(long nRow, sal_uInt16 nColId )
{
    GetCtrl()->CellModified(nRow,nColId);
}
//------------------------------------------------------------------
OTableEditorCtrl* OTableFieldControl::GetCtrl() const
{
    OTableDesignView* pDesignWin = static_cast<OTableDesignView*>(GetParent()->GetParent()->GetParent()->GetParent());
    OSL_ENSURE(pDesignWin,"no view!");
    return pDesignWin->GetEditorCtrl();
}
//------------------------------------------------------------------
sal_Bool OTableFieldControl::IsReadOnly()
{
    sal_Bool bRead(GetCtrl()->IsReadOnly());
    if( !bRead )
    {
        // Die Spalten einer ::com::sun::star::sdbcx::View k�nnen nicht ver�ndert werden
        Reference<XPropertySet> xTable = GetCtrl()->GetView()->getController().getTable();
        if(xTable.is() && ::comphelper::getString(xTable->getPropertyValue(PROPERTY_TYPE)) == ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("VIEW")))
            bRead = sal_True;
        else
        {
             ::boost::shared_ptr<OTableRow>  pCurRow = GetCtrl()->GetActRow();
            if( pCurRow )
                bRead = pCurRow->IsReadOnly();
        }
    }
    return bRead;
}
//------------------------------------------------------------------
void OTableFieldControl::ActivateAggregate( EControlType eType )
{
    switch(eType)
    {
        case tpColumnName:
        case tpType:
            break;
        default:
            OFieldDescControl::ActivateAggregate(eType);
    }
}
//------------------------------------------------------------------
void OTableFieldControl::DeactivateAggregate( EControlType eType )
{
    switch(eType)
    {
        case tpColumnName:
        case tpType:
            break;
        default:
            OFieldDescControl::DeactivateAggregate(eType);
    }
}
// -----------------------------------------------------------------------------
void OTableFieldControl::SetModified(sal_Bool bModified)
{
    GetCtrl()->GetView()->getController().setModified(bModified);
}
// -----------------------------------------------------------------------------
::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection> OTableFieldControl::getConnection()
{
    return GetCtrl()->GetView()->getController().getConnection();
}
// -----------------------------------------------------------------------------
::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDatabaseMetaData> OTableFieldControl::getMetaData()
{
    Reference<XConnection> xCon = GetCtrl()->GetView()->getController().getConnection();
    if(!xCon.is())
        return NULL;
    return xCon->getMetaData();
}
// -----------------------------------------------------------------------------
Reference< XNumberFormatter >   OTableFieldControl::GetFormatter() const
{
    return GetCtrl()->GetView()->getController().getNumberFormatter();
}
// -----------------------------------------------------------------------------
TOTypeInfoSP OTableFieldControl::getTypeInfo(sal_Int32 _nPos)
{
    return GetCtrl()->GetView()->getController().getTypeInfo(_nPos);
}
// -----------------------------------------------------------------------------
const OTypeInfoMap* OTableFieldControl::getTypeInfo() const
{
    return const_cast<OTableFieldControl*>(this)->GetCtrl()->GetView()->getController().getTypeInfo();
}
// -----------------------------------------------------------------------------
Locale OTableFieldControl::GetLocale() const
{
    return const_cast<OTableFieldControl*>(this)->GetCtrl()->GetView()->getLocale();
}
// -----------------------------------------------------------------------------
sal_Bool OTableFieldControl::isAutoIncrementValueEnabled() const
{
    return const_cast<OTableFieldControl*>(this)->GetCtrl()->GetView()->getController().isAutoIncrementValueEnabled();
}
// -----------------------------------------------------------------------------
::rtl::OUString OTableFieldControl::getAutoIncrementValue() const
{
    return const_cast<OTableFieldControl*>(this)->GetCtrl()->GetView()->getController().getAutoIncrementValue();
}
// -----------------------------------------------------------------------------

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
