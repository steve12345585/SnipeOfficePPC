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

#include <com/sun/star/embed/EmbedStates.hpp>

#include <sfx2/ipclient.hxx>
#include <sfx2/viewsh.hxx>
#include <sfx2/app.hxx>
#include <unotools/moduleoptions.hxx>
#include <sfx2/viewfrm.hxx>

#include <sot/exchange.hxx>
#include <fmtcntnt.hxx>
#include <fmtanchr.hxx>
#include <fesh.hxx>
#include <cntfrm.hxx>
#include <frmfmt.hxx>
#include <flyfrm.hxx>
#include <pam.hxx>
#include <doc.hxx>
#include <ndtxt.hxx>
#include <notxtfrm.hxx>
#include <ndole.hxx>
#include <swcli.hxx>

using namespace com::sun::star;

SwFlyFrm *SwFEShell::FindFlyFrm( const uno::Reference < embed::XEmbeddedObject >& xObj ) const
{
    SwFlyFrm *pFly = FindFlyFrm();
    if ( pFly && pFly->Lower() && pFly->Lower()->IsNoTxtFrm() )
    {
        SwOLENode *pNd = ((SwNoTxtFrm*)pFly->Lower())->GetNode()->GetOLENode();
        if ( !pNd || pNd->GetOLEObj().GetOleRef() != xObj )
            pFly = 0;
    }
    else
        pFly = 0;

    if ( !pFly )
    {
        // No or wrong fly selected: we have to search.
        sal_Bool bExist = sal_False;
        SwStartNode *pStNd;
        sal_uLong nSttIdx = GetNodes().GetEndOfAutotext().StartOfSectionIndex() + 1,
              nEndIdx = GetNodes().GetEndOfAutotext().GetIndex();
        while( nSttIdx < nEndIdx &&
                0 != (pStNd = GetNodes()[ nSttIdx ]->GetStartNode()) )
        {
            SwNode *pNd = GetNodes()[ nSttIdx+1 ];
            if ( pNd->IsOLENode() &&
                 ((SwOLENode*)pNd)->GetOLEObj().GetOleRef() == xObj )
            {
                bExist = sal_True;
                SwFrm *pFrm = ((SwOLENode*)pNd)->getLayoutFrm( GetLayout() );
                if ( pFrm )
                    pFly = pFrm->FindFlyFrm();
                break;
            }
            nSttIdx = pStNd->EndOfSectionIndex() + 1;
        }

        OSL_ENSURE( bExist, "OLE-Object unknown and FlyFrm not found." );
        (void)bExist;
    }
    return pFly;
}


String SwFEShell::GetUniqueOLEName() const
{
    return GetDoc()->GetUniqueOLEName();
}


String SwFEShell::GetUniqueFrameName() const
{
    return GetDoc()->GetUniqueFrameName();
}


void SwFEShell::MakeObjVisible( const uno::Reference < embed::XEmbeddedObject >& xObj ) const
{
    SwFlyFrm *pFly = FindFlyFrm( xObj );
    if ( pFly )
    {
        SwRect aTmp( pFly->Prt() );
        aTmp += pFly->Frm().Pos();
        if ( !aTmp.IsOver( VisArea() ) )
        {
            ((SwFEShell*)this)->StartAction();
            ((SwFEShell*)this)->MakeVisible( aTmp );
            ((SwFEShell*)this)->EndAction();
        }
    }
}

sal_Bool SwFEShell::FinishOLEObj()                      // Server is terminated
{
    SfxInPlaceClient* pIPClient = GetSfxViewShell()->GetIPClient();
    if ( !pIPClient )
        return sal_False;

    sal_Bool bRet = pIPClient->IsObjectInPlaceActive();
    if( bRet )
    {
        uno::Reference < embed::XEmbeddedObject > xObj = pIPClient->GetObject();
        if( CNT_OLE == GetCntType() )
            ClearAutomaticContour();

        if( ((SwOleClient*)pIPClient)->IsCheckForOLEInCaption() !=
            IsCheckForOLEInCaption() )
            SetCheckForOLEInCaption( !IsCheckForOLEInCaption() );

        // leave UIActive state
        pIPClient->DeactivateObject();
    }
    return bRet;
}




/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
