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


#include <svx/svxids.hrc>
#include <tools/stream.hxx>
#include <unotools/pathoptions.hxx>
#include <sot/storage.hxx>
#include <svl/intitem.hxx>
#include <editeng/forbiddencharacterstable.hxx>

#include <unotools/ucbstreamhelper.hxx>
#include <svx/xtable.hxx>
#include <svx/drawitem.hxx>
#include <viewsh.hxx>
#include <doc.hxx>
#include <rootfrm.hxx>
#include <drawdoc.hxx>
#include <dpage.hxx>
#include <docsh.hxx>
#include <shellio.hxx>
#include <hintids.hxx>
#include <com/sun/star/embed/ElementModes.hpp>

using namespace com::sun::star;

/*************************************************************************
|*
|* Konstruktor
|*
\************************************************************************/

const String GetPalettePath()
{
    SvtPathOptions aPathOpt;
    return aPathOpt.GetPalettePath();
}

SwDrawDocument::SwDrawDocument( SwDoc* pD ) :
    FmFormModel( ::GetPalettePath(), &pD->GetAttrPool(),
                 pD->GetDocShell(), sal_True ),
    pDoc( pD )
{
    SetScaleUnit( MAP_TWIP );
    SetSwapGraphics( sal_True );

    SwDocShell* pDocSh = pDoc->GetDocShell();
    if ( pDocSh )
    {
        SetObjectShell( pDocSh );
        SvxColorListItem* pColItem = ( SvxColorListItem* )
                                ( pDocSh->GetItem( SID_COLOR_TABLE ) );
        XColorListRef pXCol = pColItem ? pColItem->GetColorList() :
                                         XColorList::GetStdColorList();
        SetPropertyList( static_cast<XPropertyList *> (pXCol.get()) );

        if ( !pColItem )
            pDocSh->PutItem( SvxColorListItem( pXCol, SID_COLOR_TABLE ) );

        pDocSh->PutItem( SvxGradientListItem( GetGradientList(), SID_GRADIENT_LIST ));
        pDocSh->PutItem( SvxHatchListItem( GetHatchList(), SID_HATCH_LIST ) );
        pDocSh->PutItem( SvxBitmapListItem( GetBitmapList(), SID_BITMAP_LIST ) );
        pDocSh->PutItem( SvxDashListItem( GetDashList(), SID_DASH_LIST ) );
        pDocSh->PutItem( SvxLineEndListItem( GetLineEndList(), SID_LINEEND_LIST ) );
        pDocSh->PutItem( SfxUInt16Item(SID_ATTR_LINEEND_WIDTH_DEFAULT, 111) );
        SetObjectShell( pDocSh );
    }
    else
        SetPropertyList( static_cast<XPropertyList *> (XColorList::GetStdColorList().get()) );

    // copy all the default values to the SdrModel
    SfxItemPool* pSdrPool = pD->GetAttrPool().GetSecondaryPool();
    if( pSdrPool )
    {
        const sal_uInt16 aWhichRanges[] =
            {
                RES_CHRATR_BEGIN, RES_CHRATR_END,
                RES_PARATR_BEGIN, RES_PARATR_END,
                0
            };

        SfxItemPool& rDocPool = pD->GetAttrPool();
        sal_uInt16 nEdtWhich, nSlotId;
        const SfxPoolItem* pItem;
        for( const sal_uInt16* pRangeArr = aWhichRanges;
            *pRangeArr; pRangeArr += 2 )
            for( sal_uInt16 nW = *pRangeArr, nEnd = *(pRangeArr+1);
                    nW < nEnd; ++nW )
                if( 0 != (pItem = rDocPool.GetPoolDefaultItem( nW )) &&
                    0 != (nSlotId = rDocPool.GetSlotId( nW ) ) &&
                    nSlotId != nW &&
                    0 != (nEdtWhich = pSdrPool->GetWhich( nSlotId )) &&
                    nSlotId != nEdtWhich )
                {
                    SfxPoolItem* pCpy = pItem->Clone();
                    pCpy->SetWhich( nEdtWhich );
                    pSdrPool->SetPoolDefaultItem( *pCpy );
                    delete pCpy;
                }
    }

    SetForbiddenCharsTable( pD->getForbiddenCharacterTable() );
    // Implementation for asian compression
    SetCharCompressType( static_cast<sal_uInt16>(pD->getCharacterCompressionType() ));
}

/*************************************************************************
|*
|* Destruktor
|*
\************************************************************************/


SwDrawDocument::~SwDrawDocument()
{
    Broadcast(SdrHint(HINT_MODELCLEARED));

    ClearModel(sal_True);
}

/*************************************************************************
|*
|* Diese Methode erzeugt eine neue Seite (SdPage) und gibt einen Zeiger
|* darauf zurueck. Die Drawing Engine benutzt diese Methode beim Laden
|* zur Erzeugung von Seiten (deren Typ sie ja nicht kennt, da es ABLEITUNGEN
|* der SdrPage sind).
|*
\************************************************************************/


SdrPage* SwDrawDocument::AllocPage(bool bMasterPage)
{
    SwDPage* pPage = new SwDPage(*this, 0 != bMasterPage);
    pPage->SetName( String::CreateFromAscii(
                                    RTL_CONSTASCII_STRINGPARAM( "Controls" )) );
    return pPage;
}

uno::Reference<embed::XStorage> SwDrawDocument::GetDocumentStorage() const
{
    return pDoc->GetDocStorage();
}

SdrLayerID SwDrawDocument::GetControlExportLayerId( const SdrObject & ) const
{
    //fuer Versionen < 5.0, es gab nur Hell und Heaven
    return (SdrLayerID)pDoc->GetHeavenId();
}

uno::Reference< uno::XInterface > SwDrawDocument::createUnoModel()
{

    uno::Reference< uno::XInterface > xModel;

    try
    {
        if ( GetDoc().GetDocShell() )
        {
            xModel = GetDoc().GetDocShell()->GetModel();
        }
    }
    catch( uno::RuntimeException& )
    {
        OSL_FAIL( "<SwDrawDocument::createUnoModel()> - could *not* retrieve model at <SwDocShell>" );
    }

    return xModel;
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
