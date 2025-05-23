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

#include <tools/urlobj.hxx>
#include <svx/fmglob.hxx>
#include <svx/svdouno.hxx>
#include <svx/svdpagv.hxx>
#include <sfx2/objsh.hxx>
#include <sfx2/docfile.hxx>

#include <com/sun/star/form/FormButtonType.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/awt/XControlModel.hpp>

using namespace com::sun::star;

#include "tabvwsh.hxx"
#include "document.hxx"
#include "drawview.hxx"
#include "globstr.hrc"
#include <avmedia/mediawindow.hxx>

//------------------------------------------------------------------------

void ScTabViewShell::InsertURLButton( const String& rName, const String& rURL,
                                        const String& rTarget,
                                        const Point* pInsPos )
{
    //  Tabelle geschuetzt ?

    ScViewData* pViewData = GetViewData();
    ScDocument* pDoc = pViewData->GetDocument();
    SCTAB nTab = pViewData->GetTabNo();
    if ( pDoc->IsTabProtected(nTab) )
    {
        ErrorMessage(STR_PROTECTIONERR);
        return;
    }

    MakeDrawLayer();

    ScTabView*  pView   = pViewData->GetView();
    ScDrawView* pDrView = pView->GetScDrawView();
    SdrModel*   pModel  = pDrView->GetModel();

    SdrObject* pObj = SdrObjFactory::MakeNewObject(FmFormInventor, OBJ_FM_BUTTON,
                               pDrView->GetSdrPageView()->GetPage(), pModel);
    SdrUnoObj* pUnoCtrl = PTR_CAST(SdrUnoObj, pObj);

    uno::Reference<awt::XControlModel> xControlModel = pUnoCtrl->GetUnoControlModel();
    OSL_ENSURE( xControlModel.is(), "UNO-Control ohne Model" );
    if( !xControlModel.is() )
        return;

    uno::Reference< beans::XPropertySet > xPropSet( xControlModel, uno::UNO_QUERY );
    uno::Any aAny;

    aAny <<= rtl::OUString(rName);
    xPropSet->setPropertyValue( rtl::OUString( "Label" ), aAny );

    ::rtl::OUString aTmp = INetURLObject::GetAbsURL( pDoc->GetDocumentShell()->GetMedium()->GetBaseURL(), rURL );
    aAny <<= aTmp;
    xPropSet->setPropertyValue( rtl::OUString( "TargetURL" ), aAny );

    if( rTarget.Len() )
    {
        aAny <<= rtl::OUString(rTarget);
        xPropSet->setPropertyValue( rtl::OUString( "TargetFrame" ), aAny );
    }

    form::FormButtonType eButtonType = form::FormButtonType_URL;
    aAny <<= eButtonType;
    xPropSet->setPropertyValue( rtl::OUString( "ButtonType" ), aAny );

        if ( ::avmedia::MediaWindow::isMediaURL( rURL ) )
    {
        aAny <<= sal_True;
        xPropSet->setPropertyValue( rtl::OUString(  "DispatchURLInternal" ), aAny );
    }

    Point aPos;
    if (pInsPos)
        aPos = *pInsPos;
    else
        aPos = GetInsertPos();

    // Groesse wie in 3.1:
    Size aSize = GetActiveWin()->PixelToLogic(Size(140, 20));

    if ( pDoc->IsNegativePage(nTab) )
        aPos.X() -= aSize.Width();

    pObj->SetLogicRect(Rectangle(aPos, aSize));

    //  am alten VC-Button musste die Position/Groesse nochmal explizit
    //  gesetzt werden - das scheint mit UnoControls nicht noetig zu sein

    //  nicht markieren wenn Ole
    pDrView->InsertObjectSafe( pObj, *pDrView->GetSdrPageView() );
}




/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
