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



#include "fuscale.hxx"

#include <svx/dialogs.hrc>

#include "app.hrc"
#include "View.hxx"
#include "Window.hxx"
#include "OutlineViewShell.hxx"
#include "drawview.hxx"
#include "drawdoc.hxx"
#include "DrawViewShell.hxx"
#include "ViewShell.hxx"
#include "fuzoom.hxx" // wegen SidArrayZoom[]

#include <vcl/msgbox.hxx>
#include <svx/svdpagv.hxx>
#include <sfx2/viewfrm.hxx>
#include <sfx2/dispatch.hxx>
#include <svx/zoom_def.hxx>
#include <svx/zoomitem.hxx>
#include <sfx2/request.hxx>
#include <svx/svxdlg.hxx>

namespace sd {

TYPEINIT1( FuScale, FuPoor );

/*************************************************************************
|*
|* Konstruktor
|*
\************************************************************************/

FuScale::FuScale (
    ViewShell* pViewSh,
    ::sd::Window* pWin,
    ::sd::View* pView,
    SdDrawDocument* pDoc,
    SfxRequest& rReq)
    : FuPoor(pViewSh, pWin, pView, pDoc, rReq)
{
}

FunctionReference FuScale::Create( ViewShell* pViewSh, ::sd::Window* pWin, ::sd::View* pView, SdDrawDocument* pDoc, SfxRequest& rReq )
{
    FunctionReference xFunc( new FuScale( pViewSh, pWin, pView, pDoc, rReq ) );
    xFunc->DoExecute(rReq);
    return xFunc;
}

void FuScale::DoExecute( SfxRequest& rReq )
{
    sal_Int16 nValue;

    const SfxItemSet* pArgs = rReq.GetArgs();

    if( !pArgs )
    {
        SfxItemSet aNewAttr( mpDoc->GetPool(), SID_ATTR_ZOOM, SID_ATTR_ZOOM );
        SvxZoomItem* pZoomItem;
        sal_uInt16 nZoomValues = SVX_ZOOM_ENABLE_ALL;

        nValue = (sal_Int16) mpWindow->GetZoom();

        // Zoom auf Seitengroesse ?
        if( mpViewShell && mpViewShell->ISA( DrawViewShell ) &&
            static_cast<DrawViewShell*>(mpViewShell)->IsZoomOnPage() )
        {
            pZoomItem = new SvxZoomItem( SVX_ZOOM_WHOLEPAGE, nValue );
        }
        else
        {
            pZoomItem = new SvxZoomItem( SVX_ZOOM_PERCENT, nValue );
        }

        // Bereich einschraenken
        if( mpViewShell )
        {
            if( mpViewShell->ISA( DrawViewShell ) )
            {
                SdrPageView* pPageView = mpView->GetSdrPageView();
                if( ( pPageView && pPageView->GetObjList()->GetObjCount() == 0 ) )
                    // || ( mpView->GetMarkedObjectList().GetMarkCount() == 0 ) )
                {
                    nZoomValues &= ~SVX_ZOOM_ENABLE_OPTIMAL;
                }
            }
            else if( mpViewShell->ISA( OutlineViewShell ) )
            {
                nZoomValues &= ~SVX_ZOOM_ENABLE_OPTIMAL;
                nZoomValues &= ~SVX_ZOOM_ENABLE_WHOLEPAGE;
                nZoomValues &= ~SVX_ZOOM_ENABLE_PAGEWIDTH;
            }
        }

        pZoomItem->SetValueSet( nZoomValues );
        aNewAttr.Put( *pZoomItem );

        AbstractSvxZoomDialog* pDlg=NULL;
        SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
        if(pFact)
        {
            pDlg = pFact->CreateSvxZoomDialog(NULL, aNewAttr);
        }

        if( pDlg )
        {
            pDlg->SetLimits( (sal_uInt16)mpWindow->GetMinZoom(), (sal_uInt16)mpWindow->GetMaxZoom() );
            sal_uInt16 nResult = pDlg->Execute();
            switch( nResult )
            {
                case RET_CANCEL:
                {
                    delete pDlg;
                    delete pZoomItem;
                    rReq.Ignore ();
                    return; // Abbruch
                }
                default:
                {
                    rReq.Ignore ();
        /*
                        rReq.Done( *( pDlg->GetOutputItemSet() ) );
                        pArgs = rReq.GetArgs();*/
                }
                break;
            }

            const SfxItemSet aArgs (*(pDlg->GetOutputItemSet ()));

            delete pDlg;

            switch (((const SvxZoomItem &) aArgs.Get (SID_ATTR_ZOOM)).GetType ())
            {
                case SVX_ZOOM_PERCENT:
                {
                    nValue = ((const SvxZoomItem &) aArgs.Get (SID_ATTR_ZOOM)).GetValue ();

                    mpViewShell->SetZoom( nValue );

                    mpViewShell->GetViewFrame()->GetBindings().Invalidate( SidArrayZoom );
                }
                break;

                case SVX_ZOOM_OPTIMAL:
                {
                    if( mpViewShell->ISA( DrawViewShell ) )
                    {
                        // Namensverwirrung: SID_SIZE_ALL -> Zoom auf alle Objekte
                        // --> Wird als Optimal im Programm angeboten
                        mpViewShell->GetViewFrame()->GetDispatcher()->Execute( SID_SIZE_ALL, SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD);
                    }
                }
                break;

                case SVX_ZOOM_PAGEWIDTH:
                    mpViewShell->GetViewFrame()->GetDispatcher()->Execute( SID_SIZE_PAGE_WIDTH, SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD);
                    break;

                case SVX_ZOOM_WHOLEPAGE:
                    mpViewShell->GetViewFrame()->GetDispatcher()->Execute(SID_SIZE_PAGE, SFX_CALLMODE_ASYNCHRON | SFX_CALLMODE_RECORD);
                    break;
                default:
                    break;
            }
        }

        delete pZoomItem;
    }
    else if(mpViewShell && (pArgs->Count () == 1))
    {
        SFX_REQUEST_ARG (rReq, pScale, SfxUInt32Item, ID_VAL_ZOOM, sal_False);
        mpViewShell->SetZoom (pScale->GetValue ());

        mpViewShell->GetViewFrame()->GetBindings().Invalidate( SidArrayZoom );
    }

}

} // end of namespace sd

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
