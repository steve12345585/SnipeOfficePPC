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

#include <com/sun/star/embed/NoVisualAreaSizeException.hpp>
#include <com/sun/star/embed/Aspects.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>

//------------------------------------------------------------------------

#include <toolkit/helper/vclunohelper.hxx>
#include <sot/exchange.hxx>
#include <svl/globalnameitem.hxx>
#include <sfx2/viewfrm.hxx>
#include <sfx2/docfile.hxx>
#include <svl/stritem.hxx>
#include <svx/svdoole2.hxx>
#include <svx/pfiledlg.hxx>
#include <tools/urlobj.hxx>
#include <vcl/msgbox.hxx>
#include <svl/urihelper.hxx>
#include <unotools/moduleoptions.hxx>
#include <svtools/insdlg.hxx>
#include <svtools/soerr.hxx>
#include <svx/svxdlg.hxx>
#include <sot/clsids.hxx>
#include <svx/svdpagv.hxx>
#include <svx/svdpage.hxx>
#include <svx/svdundo.hxx>
#include <sfx2/msgpool.hxx>
#include <scmod.hxx>

// BM/IHA --
#include <cppuhelper/component_context.hxx>
#include <comphelper/processfactory.hxx>
#include <comphelper/storagehelper.hxx>
#include <com/sun/star/frame/XSynchronousFrameLoader.hpp>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/chart2/data/XDataProvider.hpp>
#include <com/sun/star/chart2/data/XDataReceiver.hpp>
#include <com/sun/star/ui/dialogs/XExecutableDialog.hpp>
#include <com/sun/star/ui/dialogs/ExecutableDialogResults.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/chart/ChartDataRowSource.hpp>
#include <cppuhelper/bootstrap.hxx>

using namespace ::com::sun::star;
// BM/IHA --

// erAck
#include "chart2uno.hxx"
// erAck

#include "fuinsert.hxx"
#include "tabvwsh.hxx"
#include "sc.hrc"
#include "chartarr.hxx"
#include "docsh.hxx"
#include "document.hxx"
#include "undotab.hxx"
#include "chartlis.hxx"
#include "uiitems.hxx"
#include "globstr.hrc"
#include "drawview.hxx"
#include "markdata.hxx"

extern SdrObject* pSkipPaintObj;            // output.cxx - dieses Objekt nicht zeichnen

namespace {

void lcl_ChartInit( const uno::Reference < embed::XEmbeddedObject >& xObj, ScViewData* pViewData,
                    const rtl::OUString& rRangeParam )
{
    ScDocShell* pDocShell = pViewData->GetDocShell();
    ScDocument* pScDoc = pDocShell->GetDocument();

    rtl::OUString aRangeString( rRangeParam );
    if ( aRangeString.isEmpty() )
    {
        SCCOL nCol1 = 0;
        SCROW nRow1 = 0;
        SCTAB nTab1 = 0;
        SCCOL nCol2 = 0;
        SCROW nRow2 = 0;
        SCTAB nTab2 = 0;

        ScMarkData& rMark = pViewData->GetMarkData();
        if ( !rMark.IsMarked() )
            pViewData->GetView()->MarkDataArea( true );

        if ( pViewData->GetSimpleArea( nCol1,nRow1,nTab1, nCol2,nRow2,nTab2 ) == SC_MARK_SIMPLE )
        {
            PutInOrder( nCol1, nCol2 );
            PutInOrder( nRow1, nRow2 );
            if ( nCol2>nCol1 || nRow2>nRow1 )
            {
                ScDocument* pDoc = pViewData->GetDocument();
                pDoc->LimitChartArea( nTab1, nCol1,nRow1, nCol2,nRow2 );

                ScRange aRange( nCol1, nRow1, nTab1, nCol2, nRow2, nTab2 );
                aRange.Format( aRangeString, SCR_ABS_3D, pScDoc );
            }
        }
    }

    if ( !aRangeString.isEmpty() )
    {
        // connect to Calc data (if no range string, leave chart alone, with its own data)

        uno::Reference< ::com::sun::star::chart2::data::XDataReceiver > xReceiver;
        uno::Reference< embed::XComponentSupplier > xCompSupp( xObj, uno::UNO_QUERY );
        if( xCompSupp.is())
            xReceiver.set( xCompSupp->getComponent(), uno::UNO_QUERY );
        OSL_ASSERT( xReceiver.is());
        if( xReceiver.is() )
        {
            uno::Reference< chart2::data::XDataProvider > xDataProvider = new ScChart2DataProvider( pScDoc );
            xReceiver->attachDataProvider( xDataProvider );

            uno::Reference< util::XNumberFormatsSupplier > xNumberFormatsSupplier( pDocShell->GetModel(), uno::UNO_QUERY );
            xReceiver->attachNumberFormatsSupplier( xNumberFormatsSupplier );

            // Same behavior as with old chart: Always assume data series in columns
            chart::ChartDataRowSource eDataRowSource = chart::ChartDataRowSource_COLUMNS;
            bool bHasCategories = false;
            bool bFirstCellAsLabel = false;

            // use ScChartPositioner to auto-detect column/row headers (like ScChartArray in old version)
            ScRangeListRef aRangeListRef( new ScRangeList );
            aRangeListRef->Parse( aRangeString, pScDoc, SCA_VALID, pScDoc->GetAddressConvention() );
            if ( !aRangeListRef->empty() )
            {
                pScDoc->LimitChartIfAll( aRangeListRef );               // limit whole columns/rows to used area

                // update string from modified ranges.  The ranges must be in the current formula syntax.
                String aTmpStr;
                aRangeListRef->Format( aTmpStr, SCR_ABS_3D, pScDoc, pScDoc->GetAddressConvention() );
                aRangeString = aTmpStr;

                ScChartPositioner aChartPositioner( pScDoc, aRangeListRef );
                const ScChartPositionMap* pPositionMap( aChartPositioner.GetPositionMap() );
                if( pPositionMap )
                {
                    SCSIZE nRowCount = pPositionMap->GetRowCount();
                    if( 1==nRowCount )
                        eDataRowSource = chart::ChartDataRowSource_ROWS;
                }
                if ( eDataRowSource == chart::ChartDataRowSource_COLUMNS )
                {
                    bHasCategories = aChartPositioner.HasRowHeaders();
                    bFirstCellAsLabel = aChartPositioner.HasColHeaders();
                }
                else    // in case the default is changed
                {
                    bHasCategories = aChartPositioner.HasColHeaders();
                    bFirstCellAsLabel = aChartPositioner.HasRowHeaders();
                }
            }

            uno::Sequence< beans::PropertyValue > aArgs( 4 );
            aArgs[0] = beans::PropertyValue(
                ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("CellRangeRepresentation")), -1,
                uno::makeAny( aRangeString ), beans::PropertyState_DIRECT_VALUE );
            aArgs[1] = beans::PropertyValue(
                ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("HasCategories")), -1,
                uno::makeAny( bHasCategories ), beans::PropertyState_DIRECT_VALUE );
            aArgs[2] = beans::PropertyValue(
                ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("FirstCellAsLabel")), -1,
                uno::makeAny( bFirstCellAsLabel ), beans::PropertyState_DIRECT_VALUE );
            aArgs[3] = beans::PropertyValue(
                ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("DataRowSource")), -1,
                uno::makeAny( eDataRowSource ), beans::PropertyState_DIRECT_VALUE );
            xReceiver->setArguments( aArgs );

            // don't create chart listener here (range may be modified in chart dialog)
        }
    }
}

}

/*************************************************************************
|*
|* FuInsertOLE::Konstruktor
|*
\************************************************************************/

FuInsertOLE::FuInsertOLE(ScTabViewShell* pViewSh, Window* pWin, ScDrawView* pViewP,
           SdrModel* pDoc, SfxRequest& rReq)
    : FuPoor(pViewSh, pWin, pViewP, pDoc, rReq)
{
    if( ! rReq.IsAPI() )
        rReq.Done();

    //! hier DLL's initalisieren, damit die Factories existieren?

    uno::Reference < embed::XEmbeddedObject > xObj;
    uno::Reference < embed::XStorage > xStorage = comphelper::OStorageHelper::GetTemporaryStorage();
    sal_Bool bIsFromFile = false;
    ::rtl::OUString aName;

    sal_Int64 nAspect = embed::Aspects::MSOLE_CONTENT;
    ::rtl::OUString aIconMediaType;
    uno::Reference< io::XInputStream > xIconMetaFile;


    sal_uInt16 nSlot = rReq.GetSlot();
    SFX_REQUEST_ARG( rReq, pNameItem, SfxGlobalNameItem, SID_INSERT_OBJECT, false );
    if ( nSlot == SID_INSERT_OBJECT && pNameItem )
    {
        SvGlobalName aClassName = pNameItem->GetValue();
        xObj = pViewShell->GetViewFrame()->GetObjectShell()->GetEmbeddedObjectContainer().CreateEmbeddedObject( aClassName.GetByteSequence(), aName );
    }
    else if ( nSlot == SID_INSERT_SMATH )
    {
        if ( SvtModuleOptions().IsMath() )
        {
            nSlot = SID_INSERT_OBJECT;
            xObj = pViewShell->GetViewFrame()->GetObjectShell()->GetEmbeddedObjectContainer().CreateEmbeddedObject( SvGlobalName( SO3_SM_CLASSID_60 ).GetByteSequence(), aName );
            rReq.AppendItem( SfxGlobalNameItem( SID_INSERT_OBJECT, SvGlobalName( SO3_SM_CLASSID_60 ) ) );
        }
    }
    else
    {
        SvObjectServerList aServerLst;
        switch ( nSlot )
        {
            case SID_INSERT_OBJECT :
                aServerLst.FillInsertObjects();
                aServerLst.Remove( ScDocShell::Factory().GetClassId() );   // Starcalc nicht anzeigen
                //TODO/LATER: currently no inserting of ClassId into SfxRequest!
            case SID_INSERT_PLUGIN :
            case SID_INSERT_FLOATINGFRAME :
            {
                SvxAbstractDialogFactory* pFact = SvxAbstractDialogFactory::Create();
                SfxAbstractInsertObjectDialog* pDlg =
                        pFact->CreateInsertObjectDialog( pViewShell->GetWindow(), SC_MOD()->GetSlotPool()->GetSlot(nSlot)->GetCommandString(),
                        xStorage, &aServerLst );
                if ( pDlg )
                {
                    pDlg->Execute();
                    xObj = pDlg->GetObject();

                    xIconMetaFile = pDlg->GetIconIfIconified( &aIconMediaType );
                    if ( xIconMetaFile.is() )
                        nAspect = embed::Aspects::MSOLE_ICON;

                    if ( xObj.is() )
                        pViewSh->GetObjectShell()->GetEmbeddedObjectContainer().InsertEmbeddedObject( xObj, aName );
                    // damit DrawShell eingeschaltet wird (Objekt aktivieren ist unnoetig):
                    bIsFromFile = !pDlg->IsCreateNew();
                    DELETEZ( pDlg );
                }

                break;
            }
            case SID_INSERT_SOUND :
            case SID_INSERT_VIDEO :
            {
                // create special filedialog for plugins
                SvxPluginFileDlg aPluginFileDialog(pWin, nSlot);

                // open filedlg
                if ( ERRCODE_NONE == aPluginFileDialog.Execute() )
                {
                    // get URL
                    INetURLObject aURL;
                    aURL.SetSmartProtocol( INET_PROT_FILE );
                    if ( aURL.SetURL( aPluginFileDialog.GetPath() ) )
                    {
                        // create a plugin object
                        ::rtl::OUString aObjName;
                        SvGlobalName aClassId( SO3_PLUGIN_CLASSID );
                        comphelper::EmbeddedObjectContainer aCnt( xStorage );
                        xObj = aCnt.CreateEmbeddedObject( aClassId.GetByteSequence(), aObjName );
                        if ( xObj.is() && svt::EmbeddedObjectRef::TryRunningState( xObj ) )
                        {
                            // set properties from dialog
                            uno::Reference < beans::XPropertySet > xSet( xObj->getComponent(), uno::UNO_QUERY );
                            if ( xSet.is() )
                            {
                                xSet->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("PluginURL")),
                                        uno::makeAny( ::rtl::OUString( aURL.GetMainURL( INetURLObject::NO_DECODE ) ) ) );
                            }
                        }
                    }
                    else
                    {
                        OSL_FAIL("Invalid URL!");
                        //! error message
                        //! can this happen???
                    }
                }
            }
        }
    }

    //  SvInsertObjectDialog (alles in einem Dialog) wird nicht mehr benutzt
    if (xObj.is())
    {
        pView->UnmarkAll();

        try
        {
            ::svt::EmbeddedObjectRef aObjRef( xObj, nAspect );
            Size aSize;
            MapMode aMap100( MAP_100TH_MM );
            MapUnit aMapUnit = MAP_100TH_MM;

            if ( nAspect == embed::Aspects::MSOLE_ICON )
            {
                aObjRef.SetGraphicStream( xIconMetaFile, aIconMediaType );
                aSize = aObjRef.GetSize( &aMap100 );
            }
            else
            {
                awt::Size aSz;
                try
                {
                    aSz = xObj->getVisualAreaSize( nAspect );
                }
                catch( embed::NoVisualAreaSizeException& )
                {
                    // the default size will be set later
                }

                aSize = Size( aSz.Width, aSz.Height );

                aMapUnit = VCLUnoHelper::UnoEmbed2VCLMapUnit( xObj->getMapUnit( nAspect ) );
                if (aSize.Height() == 0 || aSize.Width() == 0)
                {
                    // Rechteck mit ausgewogenem Kantenverhaeltnis
                    aSize.Width() = 5000;
                    aSize.Height() = 5000;
                    Size aTmp = OutputDevice::LogicToLogic( aSize, MAP_100TH_MM, aMapUnit );
                    aSz.Width = aTmp.Width();
                    aSz.Height = aTmp.Height();
                    xObj->setVisualAreaSize( nAspect, aSz );

                    //  re-convert aSize to 1/100th mm to avoid rounding errors in comparison below
                    aSize = Window::LogicToLogic( aTmp,
                                    MapMode( aMapUnit ), aMap100 );
                }
                else
                    aSize = Window::LogicToLogic( aSize,
                                    MapMode( aMapUnit ), aMap100 );
            }

            //  Chart initialisieren ?
            if ( SvtModuleOptions().IsChart() && SotExchange::IsChart( SvGlobalName( xObj->getClassID() ) ) )
                lcl_ChartInit( xObj, pViewSh->GetViewData(), rtl::OUString() );

            ScViewData* pData = pViewSh->GetViewData();

            Point aPnt = pViewSh->GetInsertPos();
            if ( pData->GetDocument()->IsNegativePage( pData->GetTabNo() ) )
                aPnt.X() -= aSize.Width();      // move position to left edge
            Rectangle aRect (aPnt, aSize);
            SdrOle2Obj* pObj = new SdrOle2Obj( aObjRef, aName, aRect);

                // Dieses Objekt nicht vor dem Aktivieren zeichnen
                // (in MarkListHasChanged kommt ein Update)
            if (!bIsFromFile)
                pSkipPaintObj = pObj;

            SdrPageView* pPV = pView->GetSdrPageView();
            pView->InsertObjectAtView(pObj, *pPV);

            if ( nAspect != embed::Aspects::MSOLE_ICON )
            {
                //  Math objects change their object size during InsertObject.
                //  New size must be set in SdrObject, or a wrong scale will be set at
                //  ActivateObject.

                try
                {
                    awt::Size aSz = xObj->getVisualAreaSize( nAspect );

                    Size aNewSize( aSz.Width, aSz.Height );
                    aNewSize = OutputDevice::LogicToLogic( aNewSize, aMapUnit, MAP_100TH_MM );

                    if ( aNewSize != aSize )
                    {
                        aRect.SetSize( aNewSize );
                        pObj->SetLogicRect( aRect );
                    }
                }
                catch( embed::NoVisualAreaSizeException& )
                {}
            }

            if ( !rReq.IsAPI() )
            {
                // XXX Activate aus Makro ist toedlich !!! ???
                if (bIsFromFile)
                {
                    // Objekt ist selektiert, also Draw-Shell aktivieren
                    pViewShell->SetDrawShell( true );
                }
                else
                {
                    pViewShell->ActivateObject( (SdrOle2Obj*) pObj, SVVERB_SHOW );
                    pSkipPaintObj = NULL;
                }
            }

            rReq.Done();
        }
        catch( uno::Exception& )
        {
            OSL_FAIL( "May need error handling here!\n" );
        }
    }
    else
        rReq.Ignore();
}

/*************************************************************************
|*
|* FuInsertOLE::Destruktor
|*
\************************************************************************/

FuInsertOLE::~FuInsertOLE()
{
}

/*************************************************************************
|*
|* FuInsertOLE::Function aktivieren
|*
\************************************************************************/

void FuInsertOLE::Activate()
{
    FuPoor::Activate();
}

/*************************************************************************
|*
|* FuInsertOLE::Function deaktivieren
|*
\************************************************************************/

void FuInsertOLE::Deactivate()
{
    FuPoor::Deactivate();
}

/*************************************************************************
|*
|* FuInsertChart::Konstruktor
|*
\************************************************************************/

FuInsertChart::FuInsertChart(ScTabViewShell* pViewSh, Window* pWin, ScDrawView* pViewP,
           SdrModel* pDoc, SfxRequest& rReq)
    : FuPoor(pViewSh, pWin, pViewP, pDoc, rReq)
{
    const SfxItemSet*   pReqArgs    = rReq.GetArgs();

    if( ! rReq.IsAPI() )
        rReq.Done();

    if( SvtModuleOptions().IsChart() )
    {
        // ----------------------------------------
        // BM/IHA --

        // get range
        ::rtl::OUString aRangeString;
        ScRange aPositionRange;             // cell range for chart positioning
        if( pReqArgs )
        {
            const SfxPoolItem* pItem;
            if( pReqArgs->HasItem( FN_PARAM_5, &pItem ) )
            aRangeString = ::rtl::OUString( ((const SfxStringItem*)pItem)->GetValue());

            aPositionRange = pViewSh->GetViewData()->GetCurPos();
        }
        else
        {
            ScMarkData& rMark = pViewSh->GetViewData()->GetMarkData();
            bool bAutomaticMark = false;
            if ( !rMark.IsMarked() && !rMark.IsMultiMarked() )
            {
                pViewSh->GetViewData()->GetView()->MarkDataArea( sal_True );
                bAutomaticMark = true;
            }

            ScMarkData aMultiMark( rMark );
            aMultiMark.MarkToMulti();

            ScRangeList aRanges;
            aMultiMark.FillRangeListWithMarks( &aRanges, false );
            String aStr;
            ScDocument* pDocument = pViewSh->GetViewData()->GetDocument();
            aRanges.Format( aStr, SCR_ABS_3D, pDocument, pDocument->GetAddressConvention() );
            aRangeString = aStr;

            // get "total" range for positioning
            if ( !aRanges.empty() )
            {
                aPositionRange = *aRanges[ 0 ];
                for ( size_t i = 1, nCount = aRanges.size(); i < nCount; ++i )
                {
                    aPositionRange.ExtendTo( *aRanges[ i ] );
                }
            }

            if(bAutomaticMark)
                pViewSh->GetViewData()->GetView()->Unmark();
        }

        // ----------------------------------------
        // adapted old code
        pView->UnmarkAll();

        ::rtl::OUString aName;
        const sal_Int64 nAspect = embed::Aspects::MSOLE_CONTENT;

        uno::Reference < embed::XEmbeddedObject > xObj =
            pViewShell->GetObjectShell()->GetEmbeddedObjectContainer().CreateEmbeddedObject( SvGlobalName( SO3_SCH_CLASSID_60 ).GetByteSequence(), aName );

        uno::Reference< ::com::sun::star::chart2::data::XDataReceiver > xReceiver;
        uno::Reference< embed::XComponentSupplier > xCompSupp( xObj, uno::UNO_QUERY );
        if( xCompSupp.is())
            xReceiver.set( xCompSupp->getComponent(), uno::UNO_QUERY );

        // lock the model to suppress any internal updates
        uno::Reference< frame::XModel > xChartModel( xReceiver, uno::UNO_QUERY );
        if( xChartModel.is() )
            xChartModel->lockControllers();

        ScRangeListRef aDummy;
        Rectangle aMarkDest;
        SCTAB nMarkTab;
        sal_Bool bDrawRect = pViewShell->GetChartArea( aDummy, aMarkDest, nMarkTab );

        //  Objekt-Groesse
        awt::Size aSz = xObj->getVisualAreaSize( nAspect );
        Size aSize( aSz.Width, aSz.Height );

        MapUnit aMapUnit = VCLUnoHelper::UnoEmbed2VCLMapUnit( xObj->getMapUnit( nAspect ) );

        sal_Bool bSizeCh = false;
        if (bDrawRect && !aMarkDest.IsEmpty())
        {
            aSize = aMarkDest.GetSize();
            bSizeCh = sal_True;
        }
        if (aSize.Height() <= 0 || aSize.Width() <= 0)
        {
            aSize.Width() = 5000;
            aSize.Height() = 5000;
            bSizeCh = sal_True;
        }
        if (bSizeCh)
        {
            aSize = Window::LogicToLogic( aSize, MapMode( MAP_100TH_MM ), MapMode( aMapUnit ) );
            aSz.Width = aSize.Width();
            aSz.Height = aSize.Height();
            xObj->setVisualAreaSize( nAspect, aSz );
        }

        ScViewData* pData = pViewSh->GetViewData();
        ScDocShell* pScDocSh = pData->GetDocShell();
        ScDocument* pScDoc   = pScDocSh->GetDocument();
        sal_Bool bUndo (pScDoc->IsUndoEnabled());

        if( pReqArgs )
        {
            const SfxPoolItem* pItem;
            sal_uInt16 nToTable = 0;

            if( pReqArgs->HasItem( FN_PARAM_4, &pItem ) )
            {
                if ( pItem->ISA( SfxUInt16Item ) )
                    nToTable = ((const SfxUInt16Item*)pItem)->GetValue();
                else if ( pItem->ISA( SfxBoolItem ) )
                {
                    //  in der idl fuer Basic steht FN_PARAM_4 als SfxBoolItem
                    //  -> wenn gesetzt, neue Tabelle, sonst aktuelle Tabelle

                    if ( ((const SfxBoolItem*)pItem)->GetValue() )
                        nToTable = static_cast<sal_uInt16>(pScDoc->GetTableCount());
                    else
                        nToTable = static_cast<sal_uInt16>(pData->GetTabNo());
                }
            }
            else
            {
                if (bDrawRect)
                    nToTable = static_cast<sal_uInt16>(nMarkTab);
                rReq.AppendItem( SfxUInt16Item( FN_PARAM_4, nToTable ) );
            }

            // auf neue Tabelle ausgeben?
            if ( nToTable == pScDoc->GetTableCount() )
            {
                // dann los...
                rtl::OUString      aTabName;
                SCTAB       nNewTab = pScDoc->GetTableCount();

                pScDoc->CreateValidTabName( aTabName );

                if ( pScDoc->InsertTab( nNewTab, aTabName ) )
                {
                    sal_Bool bAppend = sal_True;

                    if (bUndo)
                    {
                        pScDocSh->GetUndoManager()->AddUndoAction(
                            new ScUndoInsertTab( pScDocSh, nNewTab,
                                                 bAppend, aTabName ) );
                    }

                    pScDocSh->Broadcast( ScTablesHint( SC_TAB_INSERTED, nNewTab ) );
                    pViewSh->SetTabNo( nNewTab, sal_True );
                    pScDocSh->PostPaintExtras();            //! erst hinterher ???
                }
                else
                {
                    OSL_FAIL( "Could not create new table :-/" );
                }
            }
            else if ( nToTable != pData->GetTabNo() )
            {
                pViewSh->SetTabNo( nToTable, sal_True );
            }
        }

        lcl_ChartInit( xObj, pData, aRangeString );         // set source range, auto-detect column/row headers

        //  Objekt-Position

        Point aStart;
        if ( bDrawRect )
            aStart = aMarkDest.TopLeft();                       // marked by hand
        else
        {
            // get chart position (from window size and data range)
            aStart = pViewSh->GetChartInsertPos( aSize, aPositionRange );
        }

        Rectangle aRect (aStart, aSize);
        SdrOle2Obj* pObj = new SdrOle2Obj( svt::EmbeddedObjectRef( xObj, nAspect ), aName, aRect);

        // Dieses Objekt nicht vor dem Aktivieren zeichnen
        // (in MarkListHasChanged kommt ein Update)
        pSkipPaintObj = pObj;

        SdrPageView* pPV = pView->GetSdrPageView();

//        pView->InsertObjectAtView(pObj, *pPV);//this call leads to an immidiate redraw and asks the chart for a visual representation

        // use the page instead of the view to insert, so no undo action is created yet
        SdrPage* pInsPage = pPV->GetPage();
        pInsPage->InsertObject( pObj );
        pView->UnmarkAllObj();
        pView->MarkObj( pObj, pPV );
        bool bAddUndo = true;               // add undo action later, unless the dialog is canceled

        if (rReq.IsAPI())
        {
            if( xChartModel.is() )
                xChartModel->unlockControllers();
        }
        else
        {
            //the controller will be unlocked by the dialog when the dialog is told to do so

            // only activate object if not called via API (e.g. macro)
            pViewShell->ActivateObject( (SdrOle2Obj*) pObj, SVVERB_SHOW );

            //open wizard
            //@todo get context from calc if that has one
            uno::Reference< uno::XComponentContext > xContext(
                ::cppu::defaultBootstrap_InitialComponentContext() );
            if(xContext.is())
            {
                uno::Reference< lang::XMultiComponentFactory > xMCF( xContext->getServiceManager() );
                if(xMCF.is())
                {
                    uno::Reference< ui::dialogs::XExecutableDialog > xDialog(
                        xMCF->createInstanceWithContext(
                            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.comp.chart2.WizardDialog"))
                            , xContext), uno::UNO_QUERY);
                    uno::Reference< lang::XInitialization > xInit( xDialog, uno::UNO_QUERY );
                    if( xChartModel.is() && xInit.is() )
                    {
                        uno::Reference< awt::XWindow > xDialogParentWindow(0);
                        //  initialize dialog
                        uno::Sequence<uno::Any> aSeq(2);
                        uno::Any* pArray = aSeq.getArray();
                        beans::PropertyValue aParam1;
                        aParam1.Name = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ParentWindow"));
                        aParam1.Value <<= uno::makeAny(xDialogParentWindow);
                        beans::PropertyValue aParam2;
                        aParam2.Name = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("ChartModel"));
                        aParam2.Value <<= uno::makeAny(xChartModel);
                        pArray[0] <<= uno::makeAny(aParam1);
                        pArray[1] <<= uno::makeAny(aParam2);
                        xInit->initialize( aSeq );

                        // try to set the dialog's position so it doesn't hide the chart
                        uno::Reference < beans::XPropertySet > xDialogProps( xDialog, uno::UNO_QUERY );
                        if ( xDialogProps.is() )
                        {
                            try
                            {
                                //get dialog size:
                                awt::Size aDialogAWTSize;
                                if( xDialogProps->getPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Size")) )
                                    >>= aDialogAWTSize )
                                {
                                    Size aDialogSize( aDialogAWTSize.Width, aDialogAWTSize.Height );
                                    if ( aDialogSize.Width() > 0 && aDialogSize.Height() > 0 )
                                    {
                                        //calculate and set new position
                                        Point aDialogPos = pViewShell->GetChartDialogPos( aDialogSize, aRect );
                                        xDialogProps->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Position")),
                                            uno::makeAny( awt::Point(aDialogPos.getX(),aDialogPos.getY()) ) );
                                    }
                                }
                                //tell the dialog to unlock controller
                                xDialogProps->setPropertyValue( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("UnlockControllersOnExecute")),
                                            uno::makeAny( sal_True ) );

                            }
                            catch( uno::Exception& )
                            {
                                OSL_FAIL( "Chart wizard couldn't be positioned automatically\n" );
                            }
                        }

                        sal_Int16 nDialogRet = xDialog->execute();
                        if( nDialogRet == ui::dialogs::ExecutableDialogResults::CANCEL )
                        {
                            // leave OLE inplace mode and unmark
                            OSL_ASSERT( pViewShell );
                            OSL_ASSERT( pView );
                            pViewShell->DeactivateOle();
                            pView->UnmarkAll();

                            // old page view pointer is invalid after switching sheets
                            pPV = pView->GetSdrPageView();

                            // remove the chart
                            OSL_ASSERT( pPV );
                            SdrPage * pPage( pPV->GetPage());
                            OSL_ASSERT( pPage );
                            OSL_ASSERT( pObj );
                            if( pPage )
                                pPage->RemoveObject( pObj->GetOrdNum());

                            bAddUndo = false;       // don't create the undo action for inserting

                            // leave the draw shell
                            pViewShell->SetDrawShell( false );
                        }
                        else
                        {
                            OSL_ASSERT( nDialogRet == ui::dialogs::ExecutableDialogResults::OK );
                            //@todo maybe move chart to different table
                        }
                    }
                    uno::Reference< lang::XComponent > xComponent( xDialog, uno::UNO_QUERY );
                    if( xComponent.is())
                        xComponent->dispose();
                }
            }
        }

        if ( bAddUndo )
        {
            // add undo action the same way as in SdrEditView::InsertObjectAtView
            // (using UndoActionHdl etc.)
            pView->AddUndo(pDoc->GetSdrUndoFactory().CreateUndoNewObject(*pObj));
        }

        // BM/IHA --
    }
}

/*************************************************************************
|*
|* FuInsertChart::Destruktor
|*
\************************************************************************/

FuInsertChart::~FuInsertChart()
{
}

/*************************************************************************
|*
|* FuInsertChart::Function aktivieren
|*
\************************************************************************/

void FuInsertChart::Activate()
{
    FuPoor::Activate();
}

/*************************************************************************
|*
|* FuInsertChart::Function deaktivieren
|*
\************************************************************************/

void FuInsertChart::Deactivate()
{
    FuPoor::Deactivate();
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
