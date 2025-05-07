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

#include "excelvbahelper.hxx"

#include <comphelper/processfactory.hxx>
#include <com/sun/star/sheet/XSheetCellRange.hpp>
#include "docuno.hxx"
#include "tabvwsh.hxx"
#include "transobj.hxx"
#include "scmod.hxx"
#include "cellsuno.hxx"
#include "compiler.hxx"
#include "token.hxx"
#include "tokenarray.hxx"

#include <com/sun/star/script/vba/VBAEventId.hpp>
#include <com/sun/star/script/vba/XVBACompatibility.hpp>
#include <com/sun/star/script/vba/XVBAEventProcessor.hpp>
#include <com/sun/star/script/vba/XVBAModuleInfo.hpp>
#include <com/sun/star/script/ModuleInfo.hpp>
#include <com/sun/star/script/ModuleType.hpp>

using namespace ::com::sun::star;
using namespace ::ooo::vba;

namespace ooo {
namespace vba {
namespace excel {

// ============================================================================

uno::Reference< sheet::XUnnamedDatabaseRanges >
GetUnnamedDataBaseRanges( ScDocShell* pShell ) throw ( uno::RuntimeException )
{
    uno::Reference< frame::XModel > xModel;
    if ( pShell )
        xModel.set( pShell->GetModel(), uno::UNO_QUERY_THROW );
    uno::Reference< beans::XPropertySet > xModelProps( xModel, uno::UNO_QUERY_THROW );
    uno::Reference< sheet::XUnnamedDatabaseRanges > xUnnamedDBRanges( xModelProps->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("UnnamedDatabaseRanges") ) ), uno::UNO_QUERY_THROW );
    return xUnnamedDBRanges;
}

// returns the XDatabaseRange for the autofilter on sheet (nSheet)
// also populates sName with the name of range
uno::Reference< sheet::XDatabaseRange >
GetAutoFiltRange( ScDocShell* pShell, sal_Int16 nSheet ) throw ( uno::RuntimeException )
{
    uno::Reference< sheet::XUnnamedDatabaseRanges > xUnnamedDBRanges( GetUnnamedDataBaseRanges( pShell ), uno::UNO_QUERY_THROW );
    uno::Reference< sheet::XDatabaseRange > xDataBaseRange;
    if (xUnnamedDBRanges->hasByTable( nSheet ) )
    {
        uno::Reference< sheet::XDatabaseRange > xDBRange( xUnnamedDBRanges->getByTable( nSheet ) , uno::UNO_QUERY_THROW );
        sal_Bool bHasAuto = false;
        uno::Reference< beans::XPropertySet > xProps( xDBRange, uno::UNO_QUERY_THROW );
        xProps->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("AutoFilter") ) ) >>= bHasAuto;
        if ( bHasAuto )
        {
            xDataBaseRange=xDBRange;
        }
    }
    return xDataBaseRange;
}

ScDocShell* GetDocShellFromRange( const uno::Reference< uno::XInterface >& xRange ) throw ( uno::RuntimeException )
{
    ScCellRangesBase* pScCellRangesBase = ScCellRangesBase::getImplementation( xRange );
    if ( !pScCellRangesBase )
    {
        throw uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Failed to access underlying doc shell uno range object" ) ), uno::Reference< uno::XInterface >() );
    }
    return pScCellRangesBase->GetDocShell();
}

uno::Reference< XHelperInterface >
getUnoSheetModuleObj( const uno::Reference< table::XCellRange >& xRange ) throw ( uno::RuntimeException )
{
    uno::Reference< sheet::XSheetCellRange > xSheetRange( xRange, uno::UNO_QUERY_THROW );
    uno::Reference< sheet::XSpreadsheet > xSheet( xSheetRange->getSpreadsheet(), uno::UNO_SET_THROW );
    return getUnoSheetModuleObj( xSheet );
}

ScDocShell* GetDocShellFromRanges( const uno::Reference< sheet::XSheetCellRangeContainer >& xRanges ) throw ( uno::RuntimeException )
{
    // need the ScCellRangesBase to get docshell
    uno::Reference< uno::XInterface > xIf( xRanges, uno::UNO_QUERY_THROW );
    return GetDocShellFromRange( xIf );
}

ScDocument* GetDocumentFromRange( const uno::Reference< uno::XInterface >& xRange ) throw ( uno::RuntimeException )
{
        ScDocShell* pDocShell = GetDocShellFromRange( xRange );
        if ( !pDocShell )
        {
                throw uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Failed to access underlying document from uno range object" ) ), uno::Reference< uno::XInterface >() );
        }
        return pDocShell->GetDocument();
}

uno::Reference< frame::XModel > GetModelFromRange( const uno::Reference< uno::XInterface >& xRange ) throw ( uno::RuntimeException )
{
    ScDocShell* pDocShell = GetDocShellFromRange( xRange );
    if ( !pDocShell )
    {
        throw uno::RuntimeException( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "Failed to access underlying model uno range object" ) ), uno::Reference< uno::XInterface >() );
    }
    return pDocShell->GetModel();
}

void implSetZoom( const uno::Reference< frame::XModel >& xModel, sal_Int16 nZoom, std::vector< SCTAB >& nTabs )
{
    ScTabViewShell* pViewSh = excel::getBestViewShell( xModel );
    Fraction aFract( nZoom, 100 );
    pViewSh->GetViewData()->SetZoom( aFract, aFract, nTabs );
    pViewSh->RefreshZoom();
}

const ::rtl::OUString REPLACE_CELLS_WARNING(  RTL_CONSTASCII_USTRINGPARAM( "ReplaceCellsWarning"));

class PasteCellsWarningReseter
{
private:
    bool bInitialWarningState;
    static uno::Reference< beans::XPropertySet > getGlobalSheetSettings() throw ( uno::RuntimeException )
    {
        static uno::Reference< beans::XPropertySet > xTmpProps( ::comphelper::getProcessServiceFactory(), uno::UNO_QUERY_THROW );
        static uno::Reference<uno::XComponentContext > xContext( xTmpProps->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "DefaultContext" ))), uno::UNO_QUERY_THROW );
        static uno::Reference<lang::XMultiComponentFactory > xServiceManager(
                xContext->getServiceManager(), uno::UNO_QUERY_THROW );
        static uno::Reference< beans::XPropertySet > xProps( xServiceManager->createInstanceWithContext( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.sheet.GlobalSheetSettings" ) ) ,xContext ), uno::UNO_QUERY_THROW );
        return xProps;
    }

    bool getReplaceCellsWarning() throw ( uno::RuntimeException )
    {
        sal_Bool res = false;
        getGlobalSheetSettings()->getPropertyValue( REPLACE_CELLS_WARNING ) >>= res;
        return ( res == sal_True );
    }

    void setReplaceCellsWarning( bool bState ) throw ( uno::RuntimeException )
    {
        getGlobalSheetSettings()->setPropertyValue( REPLACE_CELLS_WARNING, uno::makeAny( bState ) );
    }
public:
    PasteCellsWarningReseter() throw ( uno::RuntimeException )
    {
        bInitialWarningState = getReplaceCellsWarning();
        if ( bInitialWarningState )
            setReplaceCellsWarning( false );
    }
    ~PasteCellsWarningReseter()
    {
        if ( bInitialWarningState )
        {
            // don't allow dtor to throw
            try
            {
                setReplaceCellsWarning( true );
            }
            catch ( uno::Exception& /*e*/ ){}
        }
    }
};

void
implnPaste( const uno::Reference< frame::XModel>& xModel )
{
    PasteCellsWarningReseter resetWarningBox;
    ScTabViewShell* pViewShell = getBestViewShell( xModel );
    if ( pViewShell )
    {
        pViewShell->PasteFromSystem();
        pViewShell->CellContentChanged();
    }
}


void
implnCopy( const uno::Reference< frame::XModel>& xModel )
{
    ScTabViewShell* pViewShell = getBestViewShell( xModel );
    if ( pViewShell )
    {
        pViewShell->CopyToClip(NULL,false,false,true);

        // mark the copied transfer object so it is used in ScVbaRange::Insert
        ScTransferObj* pClipObj = ScTransferObj::GetOwnClipboard( NULL );
        if (pClipObj)
            pClipObj->SetUseInApi( true );
    }
}

void
implnCut( const uno::Reference< frame::XModel>& xModel )
{
    ScTabViewShell* pViewShell =  getBestViewShell( xModel );
    if ( pViewShell )
    {
        pViewShell->CutToClip( NULL, sal_True );

        // mark the copied transfer object so it is used in ScVbaRange::Insert
        ScTransferObj* pClipObj = ScTransferObj::GetOwnClipboard( NULL );
        if (pClipObj)
            pClipObj->SetUseInApi( true );
    }
}

void implnPasteSpecial( const uno::Reference< frame::XModel>& xModel, sal_uInt16 nFlags,sal_uInt16 nFunction,sal_Bool bSkipEmpty, sal_Bool bTranspose)
{
    PasteCellsWarningReseter resetWarningBox;
    sal_Bool bAsLink(false), bOtherDoc(false);
    InsCellCmd eMoveMode = INS_NONE;

    ScTabViewShell* pTabViewShell = getBestViewShell( xModel );
    if ( pTabViewShell )
    {
        ScViewData* pView = pTabViewShell->GetViewData();
        Window* pWin = ( pView != NULL ) ? pView->GetActiveWin() : NULL;
        if ( pView && pWin )
        {
            if ( bAsLink && bOtherDoc )
                pTabViewShell->PasteFromSystem(0);//SOT_FORMATSTR_ID_LINK
            else
            {
                ScTransferObj* pOwnClip = ScTransferObj::GetOwnClipboard( pWin );
                ScDocument* pDoc = NULL;
                if ( pOwnClip )
                    pDoc = pOwnClip->GetDocument();
                pTabViewShell->PasteFromClip( nFlags, pDoc,
                    nFunction, bSkipEmpty, bTranspose, bAsLink,
                    eMoveMode, IDF_NONE, sal_True );
                pTabViewShell->CellContentChanged();
            }
        }
    }

}

bool implnCopyRanges( const uno::Reference< frame::XModel>& xModel, ScRangeList& rRangeList )
{
    bool bResult = false;
    ScTabViewShell* pViewShell = getBestViewShell( xModel );
    if ( pViewShell )
    {
        bResult = pViewShell->CopyToClip( NULL, rRangeList, false, true, true );
    }
    return bResult;
}

bool implnCopyRange( const uno::Reference< frame::XModel>& xModel, const ScRange& rRange )
{
    ScRangeList aRanges;
    aRanges.Append( rRange );
    return implnCopyRanges( xModel, aRanges );
}

ScDocShell*
getDocShell( const css::uno::Reference< css::frame::XModel>& xModel )
{
    uno::Reference< uno::XInterface > xIf( xModel, uno::UNO_QUERY_THROW );
    ScModelObj* pModel = dynamic_cast< ScModelObj* >( xIf.get() );
    ScDocShell* pDocShell = NULL;
    if ( pModel )
        pDocShell = (ScDocShell*)pModel->GetEmbeddedObject();
    return pDocShell;

}

ScTabViewShell*
getBestViewShell( const css::uno::Reference< css::frame::XModel>& xModel )
{
    ScDocShell* pDocShell = getDocShell( xModel );
    if ( pDocShell )
        return pDocShell->GetBestViewShell();
    return NULL;
}

ScTabViewShell*
getCurrentBestViewShell(  const uno::Reference< uno::XComponentContext >& xContext )
{
    uno::Reference< frame::XModel > xModel = getCurrentExcelDoc( xContext );
    return getBestViewShell( xModel );
}

sal_Bool IsR1C1ReferFormat( ScDocument* pDoc, const rtl::OUString& sRangeStr )
{
    ScRangeList aCellRanges;
    String sAddress( sRangeStr );
    sal_uInt16 nMask = SCA_VALID;
    sal_uInt16 rResFlags = aCellRanges.Parse( sAddress, pDoc, nMask, formula::FormulaGrammar::CONV_XL_R1C1 );
    if ( rResFlags & SCA_VALID )
    {
        return sal_True;
    }
    return false;
}

uno::Reference< vba::XHelperInterface >
getUnoSheetModuleObj( const uno::Reference< sheet::XSpreadsheet >& xSheet ) throw ( uno::RuntimeException )
{
    uno::Reference< beans::XPropertySet > xProps( xSheet, uno::UNO_QUERY_THROW );
    rtl::OUString sCodeName;
    xProps->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("CodeName") ) ) >>= sCodeName;
    // #TODO #FIXME ideally we should 'throw' here if we don't get a valid parent, but... it is possible
    // to create a module ( and use 'Option VBASupport 1' ) for a calc document, in this scenario there
    // are *NO* special document module objects ( of course being able to switch between vba/non vba mode at
    // the document in the future could fix this, especially IF the switching of the vba mode takes care to
    // create the special document module objects if they don't exist.
    uno::Reference< XHelperInterface > xParent( ov::getUnoDocModule( sCodeName, GetDocShellFromRange( xSheet ) ), uno::UNO_QUERY );
    return xParent;
}

formula::FormulaGrammar::Grammar GetFormulaGrammar( ScDocument* pDoc, const ScAddress& sAddress, const css::uno::Any& aFormula )
{
    formula::FormulaGrammar::Grammar eGrammar = formula::FormulaGrammar::GRAM_NATIVE_XL_A1;
    if ( pDoc && aFormula.hasValue() && aFormula.getValueTypeClass() == uno::TypeClass_STRING )
    {
        rtl::OUString sFormula;
        aFormula >>= sFormula;

        ScCompiler aCompiler( pDoc, sAddress );
        aCompiler.SetGrammar( formula::FormulaGrammar::GRAM_NATIVE_XL_R1C1 );
        ScTokenArray* pCode = aCompiler.CompileString( sFormula );
        if ( pCode )
        {
            sal_uInt16 nLen = pCode->GetLen();
            formula::FormulaToken** pTokens = pCode->GetArray();
            for ( sal_uInt16 nPos = 0; nPos < nLen; nPos++ )
            {
                const formula::FormulaToken& rToken = *pTokens[nPos];
                switch ( rToken.GetType() )
                {
                    case formula::svSingleRef:
                    case formula::svDoubleRef:
                    {
                        return formula::FormulaGrammar::GRAM_NATIVE_XL_R1C1;
                    }
                    break;
                    default: break;
                }
            }
        }
    }
    return eGrammar;
}

void CompileExcelFormulaToODF( ScDocument* pDoc, const String& rOldFormula, String& rNewFormula )
{
    if ( !pDoc )
    {
        return;
    }
    ScCompiler aCompiler( pDoc, ScAddress() );
    aCompiler.SetGrammar( excel::GetFormulaGrammar( pDoc, ScAddress(), uno::Any( rtl::OUString( rOldFormula ) ) ) );
    aCompiler.CompileString( rOldFormula );
    aCompiler.SetGrammar( formula::FormulaGrammar::GRAM_PODF_A1 );
    aCompiler.CreateStringFromTokenArray( rNewFormula );
}

void CompileODFFormulaToExcel( ScDocument* pDoc, const String& rOldFormula, String& rNewFormula, const formula::FormulaGrammar::Grammar eGrammar )
{
    // eGrammar can be formula::FormulaGrammar::GRAM_NATIVE_XL_R1C1 and formula::FormulaGrammar::GRAM_NATIVE_XL_A1
    if ( !pDoc )
    {
        return;
    }
    ScCompiler aCompiler( pDoc, ScAddress() );
    aCompiler.SetGrammar( formula::FormulaGrammar::GRAM_PODF_A1 );
    ScTokenArray* pCode = aCompiler.CompileString( rOldFormula );
    aCompiler.SetGrammar( eGrammar );
    if ( !pCode )
    {
        return;
    }
    sal_uInt16 nLen = pCode->GetLen();
    formula::FormulaToken** pTokens = pCode->GetArray();
    for ( sal_uInt16 nPos = 0; nPos < nLen && pTokens[nPos]; nPos++ )
    {
        String rFormula;
        formula::FormulaToken* pToken = pTokens[nPos];
        aCompiler.CreateStringFromToken( rFormula, pToken, true );
        if ( pToken->GetOpCode() == ocSep )
        {
            // Excel formula separator is ",".
            rFormula = String::CreateFromAscii(",");
        }
        rNewFormula += rFormula;
    }
}

uno::Reference< XHelperInterface >
getUnoSheetModuleObj( const uno::Reference< sheet::XSheetCellRangeContainer >& xRanges ) throw ( uno::RuntimeException )
{
    uno::Reference< container::XEnumerationAccess > xEnumAccess( xRanges, uno::UNO_QUERY_THROW );
    uno::Reference< container::XEnumeration > xEnum = xEnumAccess->createEnumeration();
    uno::Reference< table::XCellRange > xRange( xEnum->nextElement(), uno::UNO_QUERY_THROW );
    return getUnoSheetModuleObj( xRange );
}

uno::Reference< XHelperInterface >
getUnoSheetModuleObj( const uno::Reference< table::XCell >& xCell ) throw ( uno::RuntimeException )
{
    uno::Reference< sheet::XSheetCellRange > xSheetRange( xCell, uno::UNO_QUERY_THROW );
    uno::Reference< sheet::XSpreadsheet > xSheet( xSheetRange->getSpreadsheet(), uno::UNO_SET_THROW );
    return getUnoSheetModuleObj( xSheet );
}

uno::Reference< XHelperInterface >
getUnoSheetModuleObj( const uno::Reference< frame::XModel >& xModel, SCTAB nTab ) throw ( uno::RuntimeException )
{
    uno::Reference< sheet::XSpreadsheetDocument > xDoc( xModel, uno::UNO_QUERY_THROW );
    uno::Reference< container::XIndexAccess > xSheets( xDoc->getSheets(), uno::UNO_QUERY_THROW );
    uno::Reference< sheet::XSpreadsheet > xSheet( xSheets->getByIndex( nTab ), uno::UNO_QUERY_THROW );
    return getUnoSheetModuleObj( xSheet );
}

void setUpDocumentModules( const uno::Reference< sheet::XSpreadsheetDocument >& xDoc )
{
    uno::Reference< frame::XModel > xModel( xDoc, uno::UNO_QUERY );
    ScDocShell* pShell = excel::getDocShell( xModel );
    if ( pShell )
    {
        String aPrjName( RTL_CONSTASCII_USTRINGPARAM( "Standard" ) );
        pShell->GetBasicManager()->SetName( aPrjName );

        /*  Set library container to VBA compatibility mode. This will create
            the VBA Globals object and store it in the Basic manager of the
            document. */
        uno::Reference<script::XLibraryContainer> xLibContainer = pShell->GetBasicContainer();
        uno::Reference<script::vba::XVBACompatibility> xVBACompat( xLibContainer, uno::UNO_QUERY_THROW );
        xVBACompat->setVBACompatibilityMode( sal_True );

        if( xLibContainer.is() )
        {
            if( !xLibContainer->hasByName( aPrjName ) )
                xLibContainer->createLibrary( aPrjName );
            uno::Any aLibAny = xLibContainer->getByName( aPrjName );
            uno::Reference< container::XNameContainer > xLib;
            aLibAny >>= xLib;
            if( xLib.is()  )
            {
                uno::Reference< script::vba::XVBAModuleInfo > xVBAModuleInfo( xLib, uno::UNO_QUERY_THROW );
                uno::Reference< lang::XMultiServiceFactory> xSF( pShell->GetModel(), uno::UNO_QUERY_THROW);
                uno::Reference< container::XNameAccess > xVBACodeNamedObjectAccess( xSF->createInstance( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "ooo.vba.VBAObjectModuleObjectProvider"))), uno::UNO_QUERY_THROW );
                // set up the module info for the workbook and sheets in the nealy created
                // spreadsheet
                ScDocument* pDoc = pShell->GetDocument();
                String sCodeName = pDoc->GetCodeName();
                if ( sCodeName.Len() == 0 )
                {
                    sCodeName = String( RTL_CONSTASCII_USTRINGPARAM("ThisWorkbook") );
                    pDoc->SetCodeName( sCodeName );
                }

                std::vector< rtl::OUString > sDocModuleNames;
                sDocModuleNames.push_back( sCodeName );

                for ( SCTAB index = 0; index < pDoc->GetTableCount(); index++)
                {
                    rtl::OUString aName;
                    pDoc->GetCodeName( index, aName );
                    sDocModuleNames.push_back( aName );
                }

                std::vector<rtl::OUString>::iterator it_end = sDocModuleNames.end();

                for ( std::vector<rtl::OUString>::iterator it = sDocModuleNames.begin(); it != it_end; ++it )
                {
                    script::ModuleInfo sModuleInfo;

                    uno::Any aName= xVBACodeNamedObjectAccess->getByName( *it );
                    sModuleInfo.ModuleObject.set( aName, uno::UNO_QUERY );
                    sModuleInfo.ModuleType = script::ModuleType::DOCUMENT;
                    xVBAModuleInfo->insertModuleInfo( *it, sModuleInfo );
                    if( xLib->hasByName( *it ) )
                        xLib->replaceByName( *it, uno::makeAny( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Option VBASupport 1\n") ) ) );
                    else
                        xLib->insertByName( *it, uno::makeAny( rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "Option VBASupport 1\n" ) ) ) );
                }
            }
        }

        /*  Trigger the Workbook_Open event, event processor will register
            itself as listener for specific events. */
        try
        {
            uno::Reference< script::vba::XVBAEventProcessor > xVbaEvents( pShell->GetDocument()->GetVbaEventProcessor(), uno::UNO_SET_THROW );
            uno::Sequence< uno::Any > aArgs;
            xVbaEvents->processVbaEvent( script::vba::VBAEventId::WORKBOOK_OPEN, aArgs );
        }
        catch( uno::Exception& )
        {
        }
    }
}

SfxItemSet*
ScVbaCellRangeAccess::GetDataSet( ScCellRangesBase* pRangeObj )
{
    return pRangeObj ? pRangeObj->GetCurrentDataSet( true ) : 0;
}

// ============================================================================

} // namespace excel
} // namespace vba
} // namespace ooo
