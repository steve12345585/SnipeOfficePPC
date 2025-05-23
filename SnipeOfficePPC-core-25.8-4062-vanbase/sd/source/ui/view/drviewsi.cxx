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


#include "DrawViewShell.hxx"
#include <svx/xtable.hxx>
#include "sdattr.hxx"
#include <svl/aeitem.hxx>
#include <editeng/eeitem.hxx>
#include <sfx2/request.hxx>
#include <svx/svditer.hxx>
#include <editeng/colritem.hxx>
#include <sfx2/viewfrm.hxx>
#include <svx/svxids.hrc>
#include <svx/svdundo.hxx>
#include <svx/view3d.hxx>
#include <sfx2/dispatch.hxx>
#include <svx/float3d.hxx>
#include <svx/f3dchild.hxx>
#include <svx/dialogs.hrc>
#include <vcl/msgbox.hxx>


#include "app.hrc"
#include "strings.hrc"

#include "drawdoc.hxx"
#include "DrawDocShell.hxx"
#include "anminfo.hxx"
#include "unoaprms.hxx"                 // Undo-Action
#include "sdundogr.hxx"                 // Undo Gruppe
#include "drawview.hxx"
#include "Window.hxx"
#include "sdresid.hxx"

using namespace ::com::sun::star;

namespace sd {

#define ATTR_MISSING    0       // Attribut nicht verfuegbar
#define ATTR_MIXED      1       // Attribut uneindeutig (bei Mehrfachselektion)
#define ATTR_SET        2       // Attribut eindeutig

#define ITEMVALUE(ItemSet,Id,Cast)  ((const Cast&)(ItemSet).Get(Id)).GetValue()

/*************************************************************************
|*
|* SfxRequests fuer EffekteWindow bearbeiten
|*
\************************************************************************/

void DrawViewShell::ExecEffectWin( SfxRequest& rReq )
{
    CheckLineTo (rReq);

    sal_uInt16 nSId = rReq.GetSlot();

    switch( nSId )
    {
        case SID_3D_INIT:
        {
            sal_uInt16 nId = Svx3DChildWindow::GetChildWindowId();
            SfxChildWindow* pWindow = GetViewFrame()->GetChildWindow( nId );
            if( pWindow )
            {
                Svx3DWin* p3DWin = (Svx3DWin*)( pWindow->GetWindow() );
                if( p3DWin )
                    p3DWin->InitColorLB( GetDoc() );
            }
        }
        break;

        case SID_3D_STATE:
        {
            Update3DWindow();
        }
        break;

        case SID_3D_ASSIGN:
        {
            AssignFrom3DWindow();
        }
        break;

    }
}

/*************************************************************************
|*
|* 3D - Assign / Update
|*
\************************************************************************/
void DrawViewShell::Update3DWindow()
{
    sal_uInt16 nId = Svx3DChildWindow::GetChildWindowId();
    SfxChildWindow* pWindow = GetViewFrame()->GetChildWindow( nId );
    if( pWindow )
    {
        Svx3DWin* p3DWin = (Svx3DWin*) pWindow->GetWindow();
        if( p3DWin && p3DWin->IsUpdateMode() )
        {
            SfxItemSet aTmpItemSet = GetView()->Get3DAttributes();
            p3DWin->Update( aTmpItemSet );
        }
    }
}

/*----------------------------------------------------------------------------*/

void DrawViewShell::AssignFrom3DWindow()
{
    sal_uInt16 nId = Svx3DChildWindow::GetChildWindowId();
    SfxChildWindow* pWin = GetViewFrame()->GetChildWindow( nId );
    if( pWin )
    {
        Svx3DWin* p3DWin = (Svx3DWin*) pWin->GetWindow();
        if( p3DWin && GetView() )
        {
            if(!GetView()->IsPresObjSelected())
            {
                SfxItemSet aSet( GetDoc()->GetPool(),
                    SDRATTR_START,  SDRATTR_END,
                    0, 0);
                p3DWin->GetAttr( aSet );

                // Eigene UNDO-Klammerung auch um die Wandlung in 3D
                GetView()->BegUndo(String(SdResId(STR_UNDO_APPLY_3D_FAVOURITE)));

                if(GetView()->IsConvertTo3DObjPossible())
                {
                    // Nur TextAttribute zuweisen
                    SfxItemSet aTextSet( GetDoc()->GetPool(),
                        EE_ITEMS_START, EE_ITEMS_END, 0 );
                    aTextSet.Put( aSet, sal_False );
                    GetView()->SetAttributes( aTextSet );

                    // Text in 3D umwandeln
                    sal_uInt16 nSId = SID_CONVERT_TO_3D;
                    SfxBoolItem aItem( nSId, sal_True );
                    GetViewFrame()->GetDispatcher()->Execute(
                        nSId, SFX_CALLMODE_SYNCHRON | SFX_CALLMODE_RECORD, &aItem, 0L );

                    // Feststellen, ob ein FILL_Attribut gesetzt ist.
                    // Falls nicht, Fuellattribut hart setzen
                    XFillStyle eFillStyle = ITEMVALUE( aSet, XATTR_FILLSTYLE, XFillStyleItem );
                    if(eFillStyle == XFILL_NONE)
                        aSet.Put(XFillStyleItem (XFILL_SOLID));

                    // remove some 3DSCENE attributes since these were
                    // created by convert to 3D and may not be changed
                    // to the defaults again.
                    aSet.ClearItem(SDRATTR_3DSCENE_DISTANCE);
                    aSet.ClearItem(SDRATTR_3DSCENE_FOCAL_LENGTH);
                    aSet.ClearItem(SDRATTR_3DOBJ_DEPTH);
                }

                // Attribute zuweisen
                GetView()->Set3DAttributes( aSet );

                // Ende UNDO
                GetView()->EndUndo();
            }
            else
            {
                InfoBox aInfoBox (
                    GetActiveWindow(),
                    String(SdResId(STR_ACTION_NOTPOSSIBLE)));
                aInfoBox.Execute();
            }

            // Focus zurueckholen
            GetActiveWindow()->GrabFocus();
        }
    }
}

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
