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

#define _BASIC_TEXTPORTIONS

#include "helpid.hrc"
#include "baside2.hrc"

#include "baside2.hxx"
#include "brkdlg.hxx"
#include "iderdll.hxx"
#include "iderdll2.hxx"
#include "objdlg.hxx"

#include <basic/sbmeth.hxx>
#include <basic/sbuno.hxx>
#include <com/sun/star/script/XLibraryContainer2.hpp>
#include <com/sun/star/beans/XMultiPropertySet.hpp>
#include <com/sun/star/beans/XPropertiesChangeListener.hpp>
#include <comphelper/processfactory.hxx>
#include <comphelper/string.hxx>
#include <officecfg/Office/Common.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/viewfrm.hxx>
#include <vcl/msgbox.hxx>
#include <vcl/sound.hxx>
#include <svl/urihelper.hxx>
#include <svtools/xtextedt.hxx>
#include <svtools/txtattr.hxx>
#include <svtools/textwindowpeer.hxx>
#include <vcl/taskpanelist.hxx>
#include <vcl/help.hxx>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

long nVirtToolBoxHeight;    // inited in WatchWindow, used in Stackwindow
long nHeaderBarHeight;

#define SCROLL_LINE     12
#define SCROLL_PAGE     60
#define DWBORDER        3

static const char cSuffixes[] = "%&!#@$";

/**
 * Helper functions to get/set text in TextEngine using
 * the stream interface.
 *
 * get/setText() only supports tools Strings limited to 64K).
 */
::rtl::OUString getTextEngineText( ExtTextEngine* pEngine )
{
    SvMemoryStream aMemStream;
    aMemStream.SetStreamCharSet( RTL_TEXTENCODING_UTF8 );
    aMemStream.SetLineDelimiter( LINEEND_LF );
    pEngine->Write( aMemStream );
    sal_uLong nSize = aMemStream.Tell();
    ::rtl::OUString aText( (const sal_Char*)aMemStream.GetData(),
        nSize, RTL_TEXTENCODING_UTF8 );
    return aText;
}

void setTextEngineText( ExtTextEngine* pEngine, const ::rtl::OUString aStr )
{
    pEngine->SetText( String() );
    ::rtl::OString aUTF8Str = ::rtl::OUStringToOString( aStr, RTL_TEXTENCODING_UTF8 );
    SvMemoryStream aMemStream( (void*)aUTF8Str.getStr(), aUTF8Str.getLength(),
        STREAM_READ | STREAM_SEEK_TO_BEGIN );
    aMemStream.SetStreamCharSet( RTL_TEXTENCODING_UTF8 );
    aMemStream.SetLineDelimiter( LINEEND_LF );
    pEngine->Read( aMemStream );
}

void lcl_DrawIDEWindowFrame( DockingWindow* pWin )
{
    if ( pWin->IsFloatingMode() )
        return;

    Size aSz = pWin->GetOutputSizePixel();
    const Color aOldLineColor( pWin->GetLineColor() );
    pWin->SetLineColor( Color( COL_WHITE ) );
    // White line on top
    pWin->DrawLine( Point( 0, 0 ), Point( aSz.Width(), 0 ) );
    // Black line at bottom
    pWin->SetLineColor( Color( COL_BLACK ) );
    pWin->DrawLine( Point( 0, aSz.Height() - 1 ),
                    Point( aSz.Width(), aSz.Height() - 1 ) );
    pWin->SetLineColor( aOldLineColor );
}

void lcl_SeparateNameAndIndex( const String& rVName, String& rVar, String& rIndex )
{
    rVar = rVName;
    rIndex.Erase();
    sal_uInt16 nIndexStart = rVar.Search( '(' );
    if ( nIndexStart != STRING_NOTFOUND )
    {
        sal_uInt16 nIndexEnd = rVar.Search( ')', nIndexStart );
        if ( nIndexStart != STRING_NOTFOUND )
        {
            rIndex = rVar.Copy( nIndexStart+1, nIndexEnd-nIndexStart-1 );
            rVar.Erase( nIndexStart );
            rVar.EraseTrailingChars();
            rIndex.EraseLeadingChars();
            rIndex.EraseTrailingChars();
        }
    }

    if ( rVar.Len() )
    {
        sal_uInt16 nLastChar = rVar.Len()-1;
        if ( strchr( cSuffixes, rVar.GetChar( nLastChar ) ) )
            rVar.Erase( nLastChar, 1 );
    }
    if ( rIndex.Len() )
    {
        sal_uInt16 nLastChar = rIndex.Len()-1;
        if ( strchr( cSuffixes, rIndex.GetChar( nLastChar ) ) )
            rIndex.Erase( nLastChar, 1 );
    }
}

class EditorWindow::ChangesListener:
    public cppu::WeakImplHelper1< beans::XPropertiesChangeListener >
{
public:
    ChangesListener(EditorWindow & editor): editor_(editor) {}

private:
    virtual ~ChangesListener() {}

    virtual void SAL_CALL disposing(lang::EventObject const &) throw (RuntimeException)
    {
        osl::MutexGuard g(editor_.mutex_);
        editor_.notifier_.clear();
    }

    virtual void SAL_CALL propertiesChange(
        Sequence< beans::PropertyChangeEvent > const &) throw (RuntimeException)
    {
        SolarMutexGuard g;
        editor_.ImplSetFont();
    }

    EditorWindow & editor_;
};

EditorWindow::EditorWindow( Window* pParent ) :
    Window( pParent, WB_BORDER )
{
    bDoSyntaxHighlight = sal_True;
    bDelayHighlight = sal_True;
    pModulWindow = 0;
    pEditView = 0;
    pEditEngine = 0;
    bHighlightning = sal_False;
    pProgress = 0;
    nCurTextWidth = 0;
    SetBackground(
        Wallpaper(GetSettings().GetStyleSettings().GetFieldColor()));
    SetPointer( Pointer( POINTER_TEXT ) );

    SetHelpId( HID_BASICIDE_EDITORWINDOW );

    // Using "this" in ctor is a little fishy, but should work here at least as
    // long as there are no derivations:
    listener_ = new ChangesListener(*this);
    Reference< beans::XMultiPropertySet > n(
        officecfg::Office::Common::Font::SourceViewFont::get(),
        UNO_QUERY_THROW);
    {
        osl::MutexGuard g(mutex_);
        notifier_ = n;
    }
    Sequence< rtl::OUString > s(2);
    s[0] = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("FontHeight"));
    s[1] = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("FontName"));
    n->addPropertiesChangeListener(s, listener_.get());
}



EditorWindow::~EditorWindow()
{
    Reference< beans::XMultiPropertySet > n;
    {
        osl::MutexGuard g(mutex_);
        n = notifier_;
    }
    if (n.is()) {
        n->removePropertiesChangeListener(listener_.get());
    }

    aSyntaxIdleTimer.Stop();

    if ( pEditEngine )
    {
        EndListening( *pEditEngine );
        pEditEngine->RemoveView( pEditView );
        delete pEditView;
        delete pEditEngine;
    }
}

String EditorWindow::GetWordAtCursor()
{
    String aWord;

    if ( pEditView )
    {
        TextEngine* pTextEngine = pEditView->GetTextEngine();
        if ( pTextEngine )
        {
            // check first, if the cursor is at a help URL
            const TextSelection& rSelection = pEditView->GetSelection();
            const TextPaM& rSelStart = rSelection.GetStart();
            const TextPaM& rSelEnd = rSelection.GetEnd();
            String aText = pTextEngine->GetText( rSelEnd.GetPara() );
            CharClass aClass( ::comphelper::getProcessServiceFactory() , Application::GetSettings().GetLocale() );
            xub_StrLen nSelStart = static_cast< xub_StrLen >( rSelStart.GetIndex() );
            xub_StrLen nSelEnd = static_cast< xub_StrLen >( rSelEnd.GetIndex() );
            xub_StrLen nLength = static_cast< xub_StrLen >( aText.Len() );
            xub_StrLen nStart = 0;
            xub_StrLen nEnd = nLength;
            while ( nStart < nLength )
            {
                String aURL( URIHelper::FindFirstURLInText( aText, nStart, nEnd, aClass ) );
                INetURLObject aURLObj( aURL );
                if ( aURLObj.GetProtocol() == INET_PROT_VND_SUN_STAR_HELP
                     && nSelStart >= nStart && nSelStart <= nEnd && nSelEnd >= nStart && nSelEnd <= nEnd )
                {
                    aWord = aURL;
                    break;
                }
                nStart = nEnd;
                nEnd = nLength;
            }

            // Not the selected range, but at the CursorPosition,
            // if a word is partially selected.
            if ( !aWord.Len() )
                aWord = pTextEngine->GetWord( rSelEnd );

            // Can be empty when full word selected, as Cursor behing it
            if ( !aWord.Len() && pEditView->HasSelection() )
                aWord = pTextEngine->GetWord( rSelStart );
        }
    }

    return aWord;
}

void EditorWindow::RequestHelp( const HelpEvent& rHEvt )
{
    sal_Bool bDone = sal_False;

    // Should have been activated at some point
    if ( pEditEngine )
    {
        if ( rHEvt.GetMode() & HELPMODE_CONTEXT )
        {
            String aKeyword = GetWordAtCursor();
            Application::GetHelp()->SearchKeyword( aKeyword );
            bDone = sal_True;
        }
        else if ( rHEvt.GetMode() & HELPMODE_QUICK )
        {
            String aHelpText;
            Point aTopLeft;
            if ( StarBASIC::IsRunning() )
            {
                Point aWindowPos = rHEvt.GetMousePosPixel();
                aWindowPos = ScreenToOutputPixel( aWindowPos );
                Point aDocPos = GetEditView()->GetDocPos( aWindowPos );
                TextPaM aCursor = GetEditView()->GetTextEngine()->GetPaM( aDocPos, sal_False );
                TextPaM aStartOfWord;
                String aWord = GetEditView()->GetTextEngine()->GetWord( aCursor, &aStartOfWord );
                if ( aWord.Len() && !comphelper::string::isdigitAsciiString(aWord) )
                {
                    sal_uInt16 nLastChar =aWord.Len()-1;
                    if ( strchr( cSuffixes, aWord.GetChar( nLastChar ) ) )
                        aWord.Erase( nLastChar, 1 );
                    SbxBase* pSBX = StarBASIC::FindSBXInCurrentScope( aWord );
                    if ( pSBX && pSBX->ISA( SbxVariable ) && !pSBX->ISA( SbxMethod ) )
                    {
                        SbxVariable* pVar = (SbxVariable*)pSBX;
                        SbxDataType eType = pVar->GetType();
                        if ( (sal_uInt8)eType == (sal_uInt8)SbxOBJECT )
                            // might cause a crash e. g. at the selections-object
                            // Type == Object does not mean pVar == Object!
                            ; // aHelpText = ((SbxObject*)pVar)->GetClassName();
                        else if ( eType & SbxARRAY )
                            ; // aHelpText = "{...}";
                        else if ( (sal_uInt8)eType != (sal_uInt8)SbxEMPTY )
                        {
                            aHelpText = pVar->GetName();
                            if ( !aHelpText.Len() )     // name is not copied with the passed parameters
                                aHelpText = aWord;
                            aHelpText += '=';
                            aHelpText += pVar->GetString();
                        }
                    }
                    if ( aHelpText.Len() )
                    {
                        aTopLeft = GetEditView()->GetTextEngine()->PaMtoEditCursor( aStartOfWord ).BottomLeft();
                        aTopLeft = GetEditView()->GetWindowPos( aTopLeft );
                        aTopLeft.X() += 5;
                        aTopLeft.Y() += 5;
                        aTopLeft = OutputToScreenPixel( aTopLeft );
                    }
                }
            }
            Help::ShowQuickHelp( this, Rectangle( aTopLeft, Size( 1, 1 ) ), aHelpText, QUICKHELP_TOP|QUICKHELP_LEFT);
            bDone = sal_True;
        }
    }

    if ( !bDone )
        Window::RequestHelp( rHEvt );
}


void EditorWindow::Resize()
{
    // ScrollBars, etc. happens in Adjust...
    if ( pEditView )
    {
        long nVisY = pEditView->GetStartDocPos().Y();

        pEditView->ShowCursor();
        Size aOutSz( GetOutputSizePixel() );
        long nMaxVisAreaStart = pEditView->GetTextEngine()->GetTextHeight() - aOutSz.Height();
        if ( nMaxVisAreaStart < 0 )
            nMaxVisAreaStart = 0;
        if ( pEditView->GetStartDocPos().Y() > nMaxVisAreaStart )
        {
            Point aStartDocPos( pEditView->GetStartDocPos() );
            aStartDocPos.Y() = nMaxVisAreaStart;
            pEditView->SetStartDocPos( aStartDocPos );
            pEditView->ShowCursor();
            pModulWindow->GetBreakPointWindow().GetCurYOffset() = aStartDocPos.Y();
            pModulWindow->GetLineNumberWindow().GetCurYOffset() = aStartDocPos.Y();
        }
        InitScrollBars();
        if ( nVisY != pEditView->GetStartDocPos().Y() )
            Invalidate();
    }
}


void EditorWindow::MouseMove( const MouseEvent &rEvt )
{
    if ( pEditView )
        pEditView->MouseMove( rEvt );
}


void EditorWindow::MouseButtonUp( const MouseEvent &rEvt )
{
    if ( pEditView )
    {
        pEditView->MouseButtonUp( rEvt );
        SfxBindings* pBindings = BasicIDE::GetBindingsPtr();
        if ( pBindings )
            pBindings->Invalidate( SID_BASICIDE_STAT_POS );
    }
}

void EditorWindow::MouseButtonDown( const MouseEvent &rEvt )
{
    GrabFocus();
    if ( pEditView )
    {
        pEditView->MouseButtonDown( rEvt );
    }
}

void EditorWindow::Command( const CommandEvent& rCEvt )
{
    if ( pEditView )
    {
        pEditView->Command( rCEvt );
        if ( ( rCEvt.GetCommand() == COMMAND_WHEEL ) ||
             ( rCEvt.GetCommand() == COMMAND_STARTAUTOSCROLL ) ||
             ( rCEvt.GetCommand() == COMMAND_AUTOSCROLL ) )
        {
            HandleScrollCommand( rCEvt, pModulWindow->GetHScrollBar(), &pModulWindow->GetEditVScrollBar() );
        }
    }
}

sal_Bool EditorWindow::ImpCanModify()
{
    sal_Bool bCanModify = sal_True;
    if ( StarBASIC::IsRunning() )
    {
        // If in Trace-mode, abort the trace or refuse input
        // Remove markers in the modules in Notify at Basic::Stoped
        if ( QueryBox( 0, WB_OK_CANCEL, String( IDEResId( RID_STR_WILLSTOPPRG ) ) ).Execute() == RET_OK )
        {
            pModulWindow->GetBasicStatus().bIsRunning = sal_False;
            BasicIDE::StopBasic();
        }
        else
            bCanModify = sal_False;
    }
    return bCanModify;
}

void EditorWindow::KeyInput( const KeyEvent& rKEvt )
{
    if ( !pEditView )   // Happens in Win95
        return;

#if OSL_DEBUG_LEVEL > 1
    Range aRange = pModulWindow->GetHScrollBar()->GetRange(); (void)aRange;
    long nVisSz = pModulWindow->GetHScrollBar()->GetVisibleSize(); (void)nVisSz;
    long nPapSz = pModulWindow->GetHScrollBar()->GetPageSize(); (void)nPapSz;
    long nLinSz = pModulWindow->GetHScrollBar()->GetLineSize(); (void)nLinSz;
    long nThumb = pModulWindow->GetHScrollBar()->GetThumbPos(); (void)nThumb;
#endif
    sal_Bool bWasModified = pEditEngine->IsModified();
    // see if there is an accelerator to be processed first
    sal_Bool bDone = SfxViewShell::Current()->KeyInput( rKEvt );

    if ( !bDone && ( !TextEngine::DoesKeyChangeText( rKEvt ) || ImpCanModify()  ) )
    {
        if ( ( rKEvt.GetKeyCode().GetCode() == KEY_Y ) && rKEvt.GetKeyCode().IsMod1() )
            bDone = sal_True;
        else
        {
            if ( ( rKEvt.GetKeyCode().GetCode() == KEY_TAB ) && !rKEvt.GetKeyCode().IsMod1() &&
                  !rKEvt.GetKeyCode().IsMod2() && !GetEditView()->IsReadOnly() )
            {
                TextSelection aSel( pEditView->GetSelection() );
                if ( aSel.GetStart().GetPara() != aSel.GetEnd().GetPara() )
                {
                    bDelayHighlight = sal_False;
                    if ( !rKEvt.GetKeyCode().IsShift() )
                        pEditView->IndentBlock();
                    else
                        pEditView->UnindentBlock();
                    bDelayHighlight = sal_True;
                    bDone = sal_True;
                }
            }
            if ( !bDone )
                bDone = pEditView->KeyInput( rKEvt );
        }
    }
    if ( !bDone )
    {
            Window::KeyInput( rKEvt );
    }
    else
    {
        SfxBindings* pBindings = BasicIDE::GetBindingsPtr();
        if ( pBindings )
        {
            pBindings->Invalidate( SID_BASICIDE_STAT_POS );
            if ( rKEvt.GetKeyCode().GetGroup() == KEYGROUP_CURSOR )
                pBindings->Update( SID_BASICIDE_STAT_POS );
            if ( !bWasModified && pEditEngine->IsModified() )
            {
                pBindings->Invalidate( SID_SAVEDOC );
                pBindings->Invalidate( SID_DOC_MODIFIED );
                pBindings->Invalidate( SID_UNDO );
            }
            if ( rKEvt.GetKeyCode().GetCode() == KEY_INSERT )
                pBindings->Invalidate( SID_ATTR_INSERT );
        }
    }
}

void EditorWindow::Paint( const Rectangle& rRect )
{
    if ( !pEditEngine )     // We need it now at latest
        CreateEditEngine();

    pEditView->Paint( rRect );
}

void EditorWindow::LoseFocus()
{
    SetSourceInBasic();
    Window::LoseFocus();
}

sal_Bool EditorWindow::SetSourceInBasic( sal_Bool bQuiet )
{
    (void)bQuiet;

    sal_Bool bChanged = sal_False;
    if ( pEditEngine && pEditEngine->IsModified()
        && !GetEditView()->IsReadOnly() )   // Added for #i60626, otherwise
            // any read only bug in the text engine could lead to a crash later
    {
        if ( !StarBASIC::IsRunning() ) // Not at runtime!
        {
            ::rtl::OUString aModule = getTextEngineText( pEditEngine );

            // update module in basic
#ifdef DBG_UTIL
            SbModule* pModule = pModulWindow->GetSbModule();
#endif
            DBG_ASSERT(pModule, "EditorWindow::SetSourceInBasic: No Module found!");

            // update module in module window
            pModulWindow->SetModule( aModule );

            // update module in library
            ScriptDocument aDocument( pModulWindow->GetDocument() );
            String aLibName = pModulWindow->GetLibName();
            String aName = pModulWindow->GetName();
            OSL_VERIFY( aDocument.updateModule( aLibName, aName, aModule ) );

            pEditEngine->SetModified( sal_False );
            BasicIDE::MarkDocumentModified( aDocument );
            bChanged = sal_True;
        }
    }
    return bChanged;
}


// Returns the position of the last character of any of the following
// EOL char combinations: CR, CR/LF, LF, return -1 if no EOL is found
sal_Int32 searchEOL( const ::rtl::OUString& rStr, sal_Int32 fromIndex )
{
    sal_Int32 iRetPos = -1;

    sal_Int32 iLF = rStr.indexOf( LINE_SEP, fromIndex );
    if( iLF != -1 )
    {
        iRetPos = iLF;
    }
    else
    {
        iRetPos = rStr.indexOf( LINE_SEP_CR, fromIndex );
    }
    return iRetPos;
}


void EditorWindow::CreateEditEngine()
{
    if ( pEditEngine )
        return;

    pEditEngine = new ExtTextEngine;
    pEditView = new ExtTextView( pEditEngine, this );
    pEditView->SetAutoIndentMode( sal_True );
    pEditEngine->SetUpdateMode( sal_False );
    pEditEngine->InsertView( pEditView );

    ImplSetFont();

    aSyntaxIdleTimer.SetTimeout( 200 );
    aSyntaxIdleTimer.SetTimeoutHdl( LINK( this, EditorWindow, SyntaxTimerHdl ) );

    aHighlighter.initialize( HIGHLIGHT_BASIC );

    sal_Bool bWasDoSyntaxHighlight = bDoSyntaxHighlight;
    bDoSyntaxHighlight = sal_False; // too slow for large texts...
    ::rtl::OUString aOUSource( pModulWindow->GetModule() );
    sal_Int32 nLines = 0;
    sal_Int32 nIndex = -1;
    do
    {
        nLines++;
        nIndex = searchEOL( aOUSource, nIndex+1 );
    }
    while ( nIndex >= 0 );

    // nLines*4: SetText+Formatting+DoHighlight+Formatting
    // it could be cut down on one formatting but you would wait even longer
    // for the text then if the source code is long...
    pProgress = new ProgressInfo( BasicIDEGlobals::GetShell()->GetViewFrame()->GetObjectShell(), String( IDEResId( RID_STR_GENERATESOURCE ) ), nLines*4 );
    setTextEngineText( pEditEngine, aOUSource );

    pEditView->SetStartDocPos( Point( 0, 0 ) );
    pEditView->SetSelection( TextSelection() );
    pModulWindow->GetBreakPointWindow().GetCurYOffset() = 0;
    pModulWindow->GetLineNumberWindow().GetCurYOffset() = 0;
    pEditEngine->SetUpdateMode( sal_True );
    Update();   // has only been invalidated at UpdateMode = sal_True

    pModulWindow->GetLayout()->GetWatchWindow().Update();
    pModulWindow->GetLayout()->GetStackWindow().Update();
    pModulWindow->GetBreakPointWindow().Update();
    pModulWindow->GetLineNumberWindow().Update();

    pEditView->ShowCursor( sal_True, sal_True );

    StartListening( *pEditEngine );

    aSyntaxIdleTimer.Stop();
    bDoSyntaxHighlight = bWasDoSyntaxHighlight;


    for ( sal_uInt16 nLine = 0; nLine < nLines; nLine++ )
        aSyntaxLineTable.insert( nLine );
    ForceSyntaxTimeout();

    DELETEZ( pProgress );

    pEditView->EraseVirtualDevice();
    pEditEngine->SetModified( sal_False );
    pEditEngine->EnableUndo( sal_True );

    InitScrollBars();

    SfxBindings* pBindings = BasicIDE::GetBindingsPtr();
    if ( pBindings )
        pBindings->Invalidate( SID_BASICIDE_STAT_POS );

    DBG_ASSERT( pModulWindow->GetBreakPointWindow().GetCurYOffset() == 0, "CreateEditEngine: Brechpunkte verschoben?" );

    // set readonly mode for readonly libraries
    ScriptDocument aDocument( pModulWindow->GetDocument() );
    ::rtl::OUString aOULibName( pModulWindow->GetLibName() );
    Reference< script::XLibraryContainer2 > xModLibContainer( aDocument.getLibraryContainer( E_SCRIPTS ), UNO_QUERY );
    if ( xModLibContainer.is() && xModLibContainer->hasByName( aOULibName ) && xModLibContainer->isLibraryReadOnly( aOULibName ) )
    {
        pModulWindow->SetReadOnly( sal_True );
    }

    if ( aDocument.isDocument() && aDocument.isReadOnly() )
        pModulWindow->SetReadOnly( sal_True );
}

// virtual
void EditorWindow::DataChanged(DataChangedEvent const & rDCEvt)
{
    Window::DataChanged(rDCEvt);
    if (rDCEvt.GetType() == DATACHANGED_SETTINGS
        && (rDCEvt.GetFlags() & SETTINGS_STYLE) != 0)
    {
        Color aColor(GetSettings().GetStyleSettings().GetFieldColor());
        if (aColor
            != rDCEvt.GetOldSettings()->GetStyleSettings().GetFieldColor())
        {
            SetBackground(Wallpaper(aColor));
            Invalidate();
        }
        if (pEditEngine != 0)
        {
            aColor = GetSettings().GetStyleSettings().GetFieldTextColor();
            if (aColor != rDCEvt.GetOldSettings()->
                GetStyleSettings().GetFieldTextColor())
            {
                Font aFont(pEditEngine->GetFont());
                aFont.SetColor(aColor);
                pEditEngine->SetFont(aFont);
            }
        }
    }
}

void EditorWindow::Notify( SfxBroadcaster& /*rBC*/, const SfxHint& rHint )
{
    if ( rHint.ISA( TextHint ) )
    {
        const TextHint& rTextHint = (const TextHint&)rHint;
        if( rTextHint.GetId() == TEXT_HINT_VIEWSCROLLED )
        {
            if ( pModulWindow->GetHScrollBar() )
                pModulWindow->GetHScrollBar()->SetThumbPos( pEditView->GetStartDocPos().X() );
            pModulWindow->GetEditVScrollBar().SetThumbPos( pEditView->GetStartDocPos().Y() );
            pModulWindow->GetBreakPointWindow().DoScroll
                ( 0, pModulWindow->GetBreakPointWindow().GetCurYOffset() - pEditView->GetStartDocPos().Y() );
            pModulWindow->GetLineNumberWindow().DoScroll
                ( 0, pModulWindow->GetLineNumberWindow().GetCurYOffset() - pEditView->GetStartDocPos().Y() );
            pModulWindow->Invalidate();
        }
        else if( rTextHint.GetId() == TEXT_HINT_TEXTHEIGHTCHANGED )
        {
            if ( pEditView->GetStartDocPos().Y() )
            {
                long nOutHeight = GetOutputSizePixel().Height();
                long nTextHeight = pEditEngine->GetTextHeight();
                if ( nTextHeight < nOutHeight )
                    pEditView->Scroll( 0, pEditView->GetStartDocPos().Y() );

                pModulWindow->GetLineNumberWindow().Invalidate();
            }

            SetScrollBarRanges();
        }
        else if( rTextHint.GetId() == TEXT_HINT_TEXTFORMATTED )
        {
            if ( pModulWindow->GetHScrollBar() )
            {
                sal_uLong nWidth = pEditEngine->CalcTextWidth();
                if ( (long)nWidth != nCurTextWidth )
                {
                    nCurTextWidth = nWidth;
                    pModulWindow->GetHScrollBar()->SetRange( Range( 0, (long)nCurTextWidth-1) );
                    pModulWindow->GetHScrollBar()->SetThumbPos( pEditView->GetStartDocPos().X() );
                }
            }
            long nPrevTextWidth = nCurTextWidth;
            nCurTextWidth = pEditEngine->CalcTextWidth();
            if ( nCurTextWidth != nPrevTextWidth )
                SetScrollBarRanges();
        }
        else if( rTextHint.GetId() == TEXT_HINT_PARAINSERTED )
        {
            ParagraphInsertedDeleted( rTextHint.GetValue(), sal_True );
            DoDelayedSyntaxHighlight( rTextHint.GetValue() );
        }
        else if( rTextHint.GetId() == TEXT_HINT_PARAREMOVED )
        {
            ParagraphInsertedDeleted( rTextHint.GetValue(), sal_False );
        }
        else if( rTextHint.GetId() == TEXT_HINT_PARACONTENTCHANGED )
        {
            DoDelayedSyntaxHighlight( rTextHint.GetValue() );
        }
    }
}

void EditorWindow::SetScrollBarRanges()
{
    // extra method, not InitScrollBars, because for EditEngine events too
    if ( !pEditEngine )
        return;

    if ( pModulWindow->GetHScrollBar() )
        pModulWindow->GetHScrollBar()->SetRange( Range( 0, nCurTextWidth-1 ) );

    pModulWindow->GetEditVScrollBar().SetRange( Range( 0, pEditEngine->GetTextHeight()-1 ) );
}

void EditorWindow::InitScrollBars()
{
    if ( !pEditEngine )
        return;

    SetScrollBarRanges();
    Size aOutSz( GetOutputSizePixel() );
    pModulWindow->GetEditVScrollBar().SetVisibleSize( aOutSz.Height() );
    pModulWindow->GetEditVScrollBar().SetPageSize( aOutSz.Height() * 8 / 10 );
    pModulWindow->GetEditVScrollBar().SetLineSize( GetTextHeight() );
    pModulWindow->GetEditVScrollBar().SetThumbPos( pEditView->GetStartDocPos().Y() );
    pModulWindow->GetEditVScrollBar().Show();

    if ( pModulWindow->GetHScrollBar() )
    {
        pModulWindow->GetHScrollBar()->SetVisibleSize( aOutSz.Width() );
        pModulWindow->GetHScrollBar()->SetPageSize( aOutSz.Width() * 8 / 10 );
        pModulWindow->GetHScrollBar()->SetLineSize( GetTextWidth( 'x' ) );
        pModulWindow->GetHScrollBar()->SetThumbPos( pEditView->GetStartDocPos().X() );
        pModulWindow->GetHScrollBar()->Show();
    }
}

void EditorWindow::ImpDoHighlight( sal_uLong nLine )
{
    if ( bDoSyntaxHighlight )
    {
        String aLine( pEditEngine->GetText( nLine ) );
        Range aChanges = aHighlighter.notifyChange( nLine, 0, &aLine, 1 );
        if ( aChanges.Len() )
        {
            for ( long n = aChanges.Min() + 1; n <= aChanges.Max(); n++ )
                aSyntaxLineTable.insert( n );
            aSyntaxIdleTimer.Start();
        }

        sal_Bool bWasModified = pEditEngine->IsModified();
        pEditEngine->RemoveAttribs( nLine, sal_True );
        HighlightPortions aPortions;
        aHighlighter.getHighlightPortions( nLine, aLine, aPortions );

        for ( size_t i = 0; i < aPortions.size(); i++ )
        {
            HighlightPortion& r = aPortions[i];
            const Color& rColor = dynamic_cast<ModulWindowLayout*>(pModulWindow->GetLayoutWindow())->getSyntaxColor(r.tokenType);
            pEditEngine->SetAttrib( TextAttribFontColor( rColor ), nLine, r.nBegin, r.nEnd, sal_True );
        }

        pEditEngine->SetModified( bWasModified );
    }
}

void EditorWindow::ImplSetFont()
{
    rtl::OUString sFontName(
        officecfg::Office::Common::Font::SourceViewFont::FontName::get().
        get_value_or( rtl::OUString() ) );
    if ( sFontName.isEmpty() )
    {
        Font aTmpFont( OutputDevice::GetDefaultFont( DEFAULTFONT_FIXED, Application::GetSettings().GetUILanguage(), 0 , this ) );
        sFontName = aTmpFont.GetName();
    }
    Size aFontSize(
        0, officecfg::Office::Common::Font::SourceViewFont::FontHeight::get() );
    Font aFont( sFontName, aFontSize );
    aFont.SetColor( GetSettings().GetStyleSettings().GetFieldTextColor() );
    SetPointFont( aFont );
    aFont = GetFont();

    if ( pModulWindow )
    {
        pModulWindow->GetBreakPointWindow().SetFont( aFont );
        pModulWindow->GetLineNumberWindow().SetFont( aFont );
    }

    if ( pEditEngine )
    {
        sal_Bool bModified = pEditEngine->IsModified();
        pEditEngine->SetFont( aFont );
        pEditEngine->SetModified( bModified );
    }
}

void EditorWindow::DoSyntaxHighlight( sal_uLong nPara )
{
    // because of the DelayedSyntaxHighlight it's possible
    // that this line does not exist anymore!
    if ( nPara < pEditEngine->GetParagraphCount() )
    {
        // unfortunately I'm not sure that excactly this line does Modified() ...
        if ( pProgress )
            pProgress->StepProgress();
        ImpDoHighlight( nPara );
    }
}

void EditorWindow::DoDelayedSyntaxHighlight( sal_uLong nPara )
{
    // line is only added to 'Liste' (list), processed in TimerHdl
    // => don't manipulate breaks while EditEngine is formatting
    if ( pProgress )
        pProgress->StepProgress();

    if ( !bHighlightning && bDoSyntaxHighlight )
    {
        if ( bDelayHighlight )
        {
            aSyntaxLineTable.insert( nPara );
            aSyntaxIdleTimer.Start();
        }
        else
            DoSyntaxHighlight( nPara );
    }
}

IMPL_LINK_NOARG(EditorWindow, SyntaxTimerHdl)
{
    DBG_ASSERT( pEditView, "Noch keine View, aber Syntax-Highlight ?!" );

    sal_Bool bWasModified = pEditEngine->IsModified();
    // pEditEngine->SetUpdateMode( sal_False );

    bHighlightning = sal_True;
    for ( SyntaxLineSet::const_iterator it = aSyntaxLineTable.begin();
          it != aSyntaxLineTable.end(); ++it )
    {
        sal_uInt16 nLine = *it;
        DoSyntaxHighlight( nLine );
    }

    // #i45572#
    if ( pEditView )
        pEditView->ShowCursor( sal_False, sal_True );

    pEditEngine->SetModified( bWasModified );

    aSyntaxLineTable.clear();
    bHighlightning = sal_False;

    return 0;
}

void EditorWindow::ParagraphInsertedDeleted( sal_uLong nPara, sal_Bool bInserted )
{
    if ( pProgress )
        pProgress->StepProgress();

    if ( !bInserted && ( nPara == TEXT_PARA_ALL ) )
    {
        pModulWindow->GetBreakPoints().reset();
        pModulWindow->GetBreakPointWindow().Invalidate();
        pModulWindow->GetLineNumberWindow().Invalidate();
        aHighlighter.initialize( HIGHLIGHT_BASIC );
    }
    else
    {
        pModulWindow->GetBreakPoints().AdjustBreakPoints( (sal_uInt16)nPara+1, bInserted );

        long nLineHeight = GetTextHeight();
        Size aSz = pModulWindow->GetBreakPointWindow().GetOutputSize();
        Rectangle aInvRec( Point( 0, 0 ), aSz );
        long nY = nPara*nLineHeight - pModulWindow->GetBreakPointWindow().GetCurYOffset();
        aInvRec.Top() = nY;
        pModulWindow->GetBreakPointWindow().Invalidate( aInvRec );

        Size aLnSz(pModulWindow->GetLineNumberWindow().GetWidth(),
                   GetOutputSizePixel().Height() - 2 * DWBORDER);
        pModulWindow->GetLineNumberWindow().SetPosSizePixel(Point(DWBORDER + 19, DWBORDER), aLnSz);
        pModulWindow->GetLineNumberWindow().Invalidate();

        if ( bDoSyntaxHighlight )
        {
            String aDummy;
            aHighlighter.notifyChange( nPara, bInserted ? 1 : (-1), &aDummy, 1 );
        }
    }
}

void EditorWindow::CreateProgress( const String& rText, sal_uLong nRange )
{
    DBG_ASSERT( !pProgress, "ProgressInfo existiert schon" );
    pProgress = new ProgressInfo( BasicIDEGlobals::GetShell()->GetViewFrame()->GetObjectShell(), rText, nRange );
}

void EditorWindow::DestroyProgress()
{
    DELETEZ( pProgress );
}

void EditorWindow::ForceSyntaxTimeout()
{
    aSyntaxIdleTimer.Stop();
    ((Link&)aSyntaxIdleTimer.GetTimeoutHdl()).Call( &aSyntaxIdleTimer );
}



BreakPointWindow::BreakPointWindow( Window* pParent ) :
    Window( pParent, WB_BORDER )
{
    pModulWindow = 0;
    nCurYOffset = 0;
    setBackgroundColor(GetSettings().GetStyleSettings().GetFieldColor());
    nMarkerPos = MARKER_NOMARKER;

    // memorize nCurYOffset and not take it from EditEngine

    SetHelpId( HID_BASICIDE_BREAKPOINTWINDOW );
}



BreakPointWindow::~BreakPointWindow()
{
}



void BreakPointWindow::Resize()
{
/// Invalidate();
}



void BreakPointWindow::Paint( const Rectangle& )
{
    if ( SyncYOffset() )
        return;

    Size aOutSz( GetOutputSize() );
    long nLineHeight = GetTextHeight();

    ModulWindowLayout* pModulWindowLayout = dynamic_cast<ModulWindowLayout*>(pModulWindow->GetLayoutWindow());

    Image aBrk1(pModulWindowLayout->getImage(IMGID_BRKENABLED));
    Image aBrk0(pModulWindowLayout->getImage(IMGID_BRKDISABLED));
    Size aBmpSz( aBrk1.GetSizePixel() );
    aBmpSz = PixelToLogic( aBmpSz );
    Point aBmpOff( 0, 0 );
    aBmpOff.X() = ( aOutSz.Width() - aBmpSz.Width() ) / 2;
    aBmpOff.Y() = ( nLineHeight - aBmpSz.Height() ) / 2;

    for ( size_t i = 0, n = GetBreakPoints().size(); i < n ; ++i )
    {
        BreakPoint* pBrk = GetBreakPoints().at( i );
        size_t nLine = pBrk->nLine-1;
        size_t nY = nLine*nLineHeight - nCurYOffset;
        DrawImage( Point( 0, nY ) + aBmpOff, pBrk->bEnabled ? aBrk1 : aBrk0 );
    }
    ShowMarker( sal_True );
}



void BreakPointWindow::DoScroll( long nHorzScroll, long nVertScroll )
{
    nCurYOffset -= nVertScroll;
    Window::Scroll( nHorzScroll, nVertScroll );
}



void BreakPointWindow::SetMarkerPos( sal_uInt16 nLine, sal_Bool bError )
{
    if ( SyncYOffset() )
        Update();

    ShowMarker( sal_False );
    nMarkerPos = nLine;
    bErrorMarker = bError;
    ShowMarker( sal_True );
}

void BreakPointWindow::ShowMarker( sal_Bool bShow )
{
    if ( nMarkerPos == MARKER_NOMARKER )
        return;

    Size aOutSz( GetOutputSize() );
    long nLineHeight = GetTextHeight();

    ModulWindowLayout* pModulWindowLayout = dynamic_cast<ModulWindowLayout*>(pModulWindow->GetLayoutWindow());
    Image aMarker(pModulWindowLayout->getImage(bErrorMarker ? IMGID_ERRORMARKER : IMGID_STEPMARKER));

    Size aMarkerSz( aMarker.GetSizePixel() );
    aMarkerSz = PixelToLogic( aMarkerSz );
    Point aMarkerOff( 0, 0 );
    aMarkerOff.X() = ( aOutSz.Width() - aMarkerSz.Width() ) / 2;
    aMarkerOff.Y() = ( nLineHeight - aMarkerSz.Height() ) / 2;

    sal_uLong nY = nMarkerPos*nLineHeight - nCurYOffset;
    Point aPos( 0, nY );
    aPos += aMarkerOff;
    if ( bShow )
        DrawImage( aPos, aMarker );
    else
        Invalidate( Rectangle( aPos, aMarkerSz ) );
}




BreakPoint* BreakPointWindow::FindBreakPoint( const Point& rMousePos )
{
    size_t nLineHeight = GetTextHeight();
    size_t nYPos = rMousePos.Y() + nCurYOffset;

    for ( size_t i = 0, n = GetBreakPoints().size(); i < n ; ++i )
    {
        BreakPoint* pBrk = GetBreakPoints().at( i );
        size_t nLine = pBrk->nLine-1;
        size_t nY = nLine*nLineHeight;
        if ( ( nYPos > nY ) && ( nYPos < ( nY + nLineHeight ) ) )
            return pBrk;
    }
    return 0;
}

void BreakPointWindow::MouseButtonDown( const MouseEvent& rMEvt )
{
    if ( rMEvt.GetClicks() == 2 )
    {
        Point aMousePos( PixelToLogic( rMEvt.GetPosPixel() ) );
        long nLineHeight = GetTextHeight();
        long nYPos = aMousePos.Y() + nCurYOffset;
        long nLine = nYPos / nLineHeight + 1;
        pModulWindow->ToggleBreakPoint( (sal_uLong)nLine );
        Invalidate();
    }
}



void BreakPointWindow::Command( const CommandEvent& rCEvt )
{
    if ( rCEvt.GetCommand() == COMMAND_CONTEXTMENU )
    {
        Point aPos( rCEvt.IsMouseEvent() ? rCEvt.GetMousePosPixel() : Point(1,1) );
        Point aEventPos( PixelToLogic( aPos ) );
        BreakPoint* pBrk = rCEvt.IsMouseEvent() ? FindBreakPoint( aEventPos ) : 0;
        if ( pBrk )
        {
            // test if break point is enabled...
            PopupMenu aBrkPropMenu( IDEResId( RID_POPUP_BRKPROPS ) );
            aBrkPropMenu.CheckItem( RID_ACTIV, pBrk->bEnabled );
            switch ( aBrkPropMenu.Execute( this, aPos ) )
            {
                case RID_ACTIV:
                {
                    pBrk->bEnabled = pBrk->bEnabled ? sal_False : sal_True;
                    pModulWindow->UpdateBreakPoint( *pBrk );
                    Invalidate();
                }
                break;
                case RID_BRKPROPS:
                {
                    BreakPointDialog aBrkDlg( this, GetBreakPoints() );
                    aBrkDlg.SetCurrentBreakPoint( pBrk );
                    aBrkDlg.Execute();
                    Invalidate();
                }
                break;
            }
        }
        else
        {
            PopupMenu aBrkListMenu( IDEResId( RID_POPUP_BRKDLG ) );
            switch ( aBrkListMenu.Execute( this, aPos ) )
            {
                case RID_BRKDLG:
                {
                    BreakPointDialog aBrkDlg( this, GetBreakPoints() );
                    aBrkDlg.Execute();
                    Invalidate();
                }
                break;
            }
        }
    }
}

sal_Bool BreakPointWindow::SyncYOffset()
{
    TextView* pView = pModulWindow->GetEditView();
    if ( pView )
    {
        long nViewYOffset = pView->GetStartDocPos().Y();
        if ( nCurYOffset != nViewYOffset )
        {
            nCurYOffset = nViewYOffset;
            Invalidate();
            return sal_True;
        }
    }
    return sal_False;
}

// virtual
void BreakPointWindow::DataChanged(DataChangedEvent const & rDCEvt)
{
    Window::DataChanged(rDCEvt);
    if (rDCEvt.GetType() == DATACHANGED_SETTINGS
        && (rDCEvt.GetFlags() & SETTINGS_STYLE) != 0)
    {
        Color aColor(GetSettings().GetStyleSettings().GetFieldColor());
        if (aColor
            != rDCEvt.GetOldSettings()->GetStyleSettings().GetFieldColor())
        {
            setBackgroundColor(aColor);
            Invalidate();
        }
    }
}

void BreakPointWindow::setBackgroundColor(Color aColor)
{
    SetBackground(Wallpaper(aColor));
}


const sal_uInt16 ITEM_ID_VARIABLE = 1;
const sal_uInt16 ITEM_ID_VALUE = 2;
const sal_uInt16 ITEM_ID_TYPE = 3;

WatchWindow::WatchWindow( Window* pParent ) :
    BasicDockingWindow( pParent ),
    aWatchStr( IDEResId( RID_STR_REMOVEWATCH ) ),
    aXEdit( this, IDEResId( RID_EDT_WATCHEDIT ) ),
    aRemoveWatchButton( this, IDEResId( RID_IMGBTN_REMOVEWATCH ) ),
    aTreeListBox( this, WB_BORDER | WB_3DLOOK | WB_HASBUTTONS | WB_HASLINES | WB_HSCROLL | WB_TABSTOP
                                  | WB_HASLINESATROOT | WB_HASBUTTONSATROOT ),
    aHeaderBar( this, WB_BUTTONSTYLE | WB_BORDER )
{
    aXEdit.SetAccessibleName(String(IDEResId( RID_STR_WATCHNAME)));
    aTreeListBox.SetAccessibleName(String(IDEResId(RID_STR_WATCHNAME)));

    long nTextLen = GetTextWidth( aWatchStr ) + DWBORDER + 3;
    aXEdit.SetPosPixel( Point( nTextLen, 3 ) );
    aXEdit.SetAccHdl( LINK( this, WatchWindow, EditAccHdl ) );
    aXEdit.GetAccelerator().InsertItem( 1, KeyCode( KEY_RETURN ) );
    aXEdit.GetAccelerator().InsertItem( 2, KeyCode( KEY_ESCAPE ) );
    aXEdit.Show();

    aRemoveWatchButton.Disable();
    aRemoveWatchButton.SetClickHdl( LINK( this, WatchWindow, ButtonHdl ) );
    aRemoveWatchButton.SetPosPixel( Point( nTextLen + aXEdit.GetSizePixel().Width() + 4, 2 ) );
    Size aSz( aRemoveWatchButton.GetModeImage().GetSizePixel() );
    aSz.Width() += 6;
    aSz.Height() += 6;
    aRemoveWatchButton.SetSizePixel( aSz );
    aRemoveWatchButton.Show();

    long nRWBtnSize = aRemoveWatchButton.GetModeImage().GetSizePixel().Height() + 10;
    nVirtToolBoxHeight = aXEdit.GetSizePixel().Height() + 7;

    if ( nRWBtnSize > nVirtToolBoxHeight )
        nVirtToolBoxHeight = nRWBtnSize;

    nHeaderBarHeight = 16;

    aTreeListBox.SetHelpId(HID_BASICIDE_WATCHWINDOW_LIST);
    aTreeListBox.EnableInplaceEditing( sal_True );
    aTreeListBox.SetSelectHdl( LINK( this, WatchWindow, TreeListHdl ) );
    aTreeListBox.SetPosPixel( Point( DWBORDER, nVirtToolBoxHeight + nHeaderBarHeight ) );
    aTreeListBox.SetHighlightRange( 1, 5 );

    Point aPnt( DWBORDER, nVirtToolBoxHeight + 1 );
    aHeaderBar.SetPosPixel( aPnt );
    aHeaderBar.SetEndDragHdl( LINK( this, WatchWindow, implEndDragHdl ) );

    long nVarTabWidth = 220;
    long nValueTabWidth = 100;
    long nTypeTabWidth = 1250;
    aHeaderBar.InsertItem( ITEM_ID_VARIABLE, String( IDEResId( RID_STR_WATCHVARIABLE ) ), nVarTabWidth );
    aHeaderBar.InsertItem( ITEM_ID_VALUE, String( IDEResId( RID_STR_WATCHVALUE ) ), nValueTabWidth );
    aHeaderBar.InsertItem( ITEM_ID_TYPE, String( IDEResId( RID_STR_WATCHTYPE ) ), nTypeTabWidth );

    long tabs[ 4 ];
    tabs[ 0 ] = 3; // two tabs
    tabs[ 1 ] = 0;
    tabs[ 2 ] = nVarTabWidth;
    tabs[ 3 ] = nVarTabWidth + nValueTabWidth;
    aTreeListBox.SvHeaderTabListBox::SetTabs( tabs, MAP_PIXEL );
    aTreeListBox.InitHeaderBar( &aHeaderBar );

    aTreeListBox.SetNodeDefaultImages( );

    aHeaderBar.Show();

    aTreeListBox.Show();

    SetText( String( IDEResId( RID_STR_WATCHNAME ) ) );

    SetHelpId( HID_BASICIDE_WATCHWINDOW );

    // make watch window keyboard accessible
    GetSystemWindow()->GetTaskPaneList()->AddWindow( this );
}



WatchWindow::~WatchWindow()
{
    GetSystemWindow()->GetTaskPaneList()->RemoveWindow( this );
}



void WatchWindow::Paint( const Rectangle& )
{
    DrawText( Point( DWBORDER, 7 ), aWatchStr );
    lcl_DrawIDEWindowFrame( this );
}



void WatchWindow::Resize()
{
    Size aSz = GetOutputSizePixel();
    Size aBoxSz( aSz.Width() - 2*DWBORDER, aSz.Height() - nVirtToolBoxHeight - DWBORDER );

    if ( aBoxSz.Width() < 4 )
        aBoxSz.Width() = 0;
    if ( aBoxSz.Height() < 4 )
        aBoxSz.Height() = 0;

    aBoxSz.Height() -= nHeaderBarHeight;
    aTreeListBox.SetSizePixel( aBoxSz );
    aTreeListBox.GetHScroll()->SetPageSize( aTreeListBox.GetHScroll()->GetVisibleSize() );

    aBoxSz.Height() = nHeaderBarHeight;
    aHeaderBar.SetSizePixel( aBoxSz );

    Invalidate();
}

struct MemberList
{
    String*         mpMemberNames;
    int             mnMemberCount;

    MemberList( void )
        : mpMemberNames( NULL )
        , mnMemberCount( 0 )
    {}
    ~MemberList()
    {
        clear();
    }

    void clear( void );
    void allocList( int nCount );
};

void MemberList::clear( void )
{
    if( mnMemberCount )
    {
        delete[] mpMemberNames;
        mnMemberCount = 0;
    }
}

void MemberList::allocList( int nCount )
{
    clear();
    if( nCount > 0 )
    {
        mnMemberCount = nCount;
        mpMemberNames = new String[ mnMemberCount ];
    }
}

struct WatchItem
{
    String          maName;
    String          maDisplayName;
    SbxObjectRef    mpObject;
    MemberList      maMemberList;

    SbxDimArrayRef  mpArray;
    int             nDimLevel;  // 0 = Root
    int             nDimCount;
    short*          pIndices;

    WatchItem*      mpArrayParentItem;

    WatchItem( void )
        : nDimLevel( 0 )
        , nDimCount( 0 )
        , pIndices( NULL )
        , mpArrayParentItem( NULL )
    {}
    ~WatchItem()
        { clearWatchItem(); }

    void clearWatchItem( bool bIncludeArrayData=true )
    {
        mpObject = NULL;
        maMemberList.clear();
        if( bIncludeArrayData )
        {
            mpArray = NULL;
            nDimLevel = 0;
            nDimCount = 0;
            delete[] pIndices;
            pIndices = NULL;
        }
    }

    WatchItem* GetRootItem( void );
    SbxDimArray* GetRootArray( void );
};

WatchItem* WatchItem::GetRootItem( void )
{
    WatchItem* pItem = mpArrayParentItem;
    while( pItem )
    {
        if( pItem->mpArray.Is() )
            break;
        pItem = pItem->mpArrayParentItem;
    }
    return pItem;
}

SbxDimArray* WatchItem::GetRootArray( void )
{
    WatchItem* pRootItem = GetRootItem();
    SbxDimArray* pRet = NULL;
    if( pRootItem )
        pRet = pRootItem->mpArray;
    return pRet;
}

void WatchWindow::AddWatch( const String& rVName )
{
    WatchItem* pWatchItem = new WatchItem;
    String aVar, aIndex;
    lcl_SeparateNameAndIndex( rVName, aVar, aIndex );
    pWatchItem->maName = aVar;

    String aWatchStr_( aVar );
    aWatchStr_ += String( RTL_CONSTASCII_USTRINGPARAM( "\t\t" ) );
    SvLBoxEntry* pNewEntry = aTreeListBox.InsertEntry( aWatchStr_, 0, sal_True, LIST_APPEND );
    pNewEntry->SetUserData( pWatchItem );

    aTreeListBox.Select( pNewEntry, sal_True );
    aTreeListBox.MakeVisible( pNewEntry );
    aRemoveWatchButton.Enable();
}

sal_Bool WatchWindow::RemoveSelectedWatch()
{
    SvLBoxEntry* pEntry = aTreeListBox.GetCurEntry();
    if ( pEntry )
    {
        aTreeListBox.GetModel()->Remove( pEntry );
        pEntry = aTreeListBox.GetCurEntry();
        if ( pEntry )
            aXEdit.SetText( ((WatchItem*)pEntry->GetUserData())->maName );
        else
            aXEdit.SetText( String() );
        if ( !aTreeListBox.GetEntryCount() )
            aRemoveWatchButton.Disable();
        return sal_True;
    }
    else
        return sal_False;
}


IMPL_LINK_INLINE_START( WatchWindow, ButtonHdl, ImageButton *, pButton )
{
    if ( pButton == &aRemoveWatchButton )
    {
        BasicIDEShell* pIDEShell = BasicIDEGlobals::GetShell();
        SfxViewFrame* pViewFrame = pIDEShell ? pIDEShell->GetViewFrame() : NULL;
        SfxDispatcher* pDispatcher = pViewFrame ? pViewFrame->GetDispatcher() : NULL;
        if( pDispatcher )
        {
            pDispatcher->Execute( SID_BASICIDE_REMOVEWATCH );
        }
    }
    return 0;
}
IMPL_LINK_INLINE_END( WatchWindow, ButtonHdl, ImageButton *, pButton )



IMPL_LINK_NOARG_INLINE_START(WatchWindow, TreeListHdl)
{
    SvLBoxEntry* pCurEntry = aTreeListBox.GetCurEntry();
    if ( pCurEntry && pCurEntry->GetUserData() )
        aXEdit.SetText( ((WatchItem*)pCurEntry->GetUserData())->maName );

    return 0;
}
IMPL_LINK_NOARG_INLINE_END(WatchWindow, TreeListHdl)


IMPL_LINK_INLINE_START( WatchWindow, implEndDragHdl, HeaderBar *, pBar )
{
    (void)pBar;

    const sal_Int32 TAB_WIDTH_MIN = 10;
    sal_Int32 nMaxWidth =
        aHeaderBar.GetSizePixel().getWidth() - 2 * TAB_WIDTH_MIN;

    sal_Int32 nVariableWith = aHeaderBar.GetItemSize( ITEM_ID_VARIABLE );
    if( nVariableWith < TAB_WIDTH_MIN )
        aHeaderBar.SetItemSize( ITEM_ID_VARIABLE, TAB_WIDTH_MIN );
    else if( nVariableWith > nMaxWidth )
        aHeaderBar.SetItemSize( ITEM_ID_VARIABLE, nMaxWidth );

    sal_Int32 nValueWith = aHeaderBar.GetItemSize( ITEM_ID_VALUE );
    if( nValueWith < TAB_WIDTH_MIN )
        aHeaderBar.SetItemSize( ITEM_ID_VALUE, TAB_WIDTH_MIN );
    else if( nValueWith > nMaxWidth )
        aHeaderBar.SetItemSize( ITEM_ID_VALUE, nMaxWidth );

    if (aHeaderBar.GetItemSize( ITEM_ID_TYPE ) < TAB_WIDTH_MIN)
        aHeaderBar.SetItemSize( ITEM_ID_TYPE, TAB_WIDTH_MIN );

    sal_Int32 nPos = 0;
    sal_uInt16 nTabs = aHeaderBar.GetItemCount();
    for( sal_uInt16 i = 1 ; i < nTabs ; ++i )
    {
        nPos += aHeaderBar.GetItemSize( i );
        aTreeListBox.SetTab( i, nPos, MAP_PIXEL );
    }
    return 0;
}
IMPL_LINK_INLINE_END( WatchWindow, implEndDragHdl, HeaderBar *, pBar )


IMPL_LINK( WatchWindow, EditAccHdl, Accelerator *, pAcc )
{
    switch ( pAcc->GetCurKeyCode().GetCode() )
    {
        case KEY_RETURN:
        {
            String aCurText( aXEdit.GetText() );
            if ( aCurText.Len() )
            {
                AddWatch( aCurText );
                aXEdit.SetSelection( Selection( 0, 0xFFFF ) );
                UpdateWatches();
            }
            else
                Sound::Beep();
        }
        break;
        case KEY_ESCAPE:
        {
            aXEdit.SetText( String() );
        }
        break;
    }

    return 0;
}

void WatchWindow::UpdateWatches( bool bBasicStopped )
{
    aTreeListBox.UpdateWatches( bBasicStopped );
}


StackWindow::StackWindow( Window* pParent ) :
    BasicDockingWindow( pParent ),
    aTreeListBox( this, WB_BORDER | WB_3DLOOK | WB_HSCROLL | WB_TABSTOP ),
    aStackStr( IDEResId( RID_STR_STACK ) )
{
       aTreeListBox.SetHelpId(HID_BASICIDE_STACKWINDOW_LIST);
    aTreeListBox.SetAccessibleName(String( IDEResId(RID_STR_STACKNAME)));
    aTreeListBox.SetPosPixel( Point( DWBORDER, nVirtToolBoxHeight ) );
    aTreeListBox.SetHighlightRange();
    aTreeListBox.SetSelectionMode( NO_SELECTION );
    aTreeListBox.InsertEntry( String(), 0, sal_False, LIST_APPEND );
    aTreeListBox.Show();

    SetText( String( IDEResId( RID_STR_STACKNAME ) ) );

    SetHelpId( HID_BASICIDE_STACKWINDOW );

    // make stack window keyboard accessible
    GetSystemWindow()->GetTaskPaneList()->AddWindow( this );
}



StackWindow::~StackWindow()
{
    GetSystemWindow()->GetTaskPaneList()->RemoveWindow( this );
}



void StackWindow::Paint( const Rectangle& )
{
    DrawText( Point( DWBORDER, 7 ), aStackStr );
    lcl_DrawIDEWindowFrame( this );
}



void StackWindow::Resize()
{
    Size aSz = GetOutputSizePixel();
    Size aBoxSz( aSz.Width() - 2*DWBORDER, aSz.Height() - nVirtToolBoxHeight - DWBORDER );

    if ( aBoxSz.Width() < 4 )
        aBoxSz.Width() = 0;
    if ( aBoxSz.Height() < 4 )
        aBoxSz.Height() = 0;

    aTreeListBox.SetSizePixel( aBoxSz );

    Invalidate();
}

void StackWindow::UpdateCalls()
{
    aTreeListBox.SetUpdateMode( sal_False );
    aTreeListBox.Clear();

    if ( StarBASIC::IsRunning() )
    {
        SbxError eOld = SbxBase::GetError();
        aTreeListBox.SetSelectionMode( SINGLE_SELECTION );

        sal_uInt16 nScope = 0;
        SbMethod* pMethod = StarBASIC::GetActiveMethod( nScope );
        while ( pMethod )
        {
            String aEntry( String::CreateFromInt32(nScope ));
            if ( aEntry.Len() < 2 )
                aEntry.Insert( ' ', 0 );
            aEntry += String( RTL_CONSTASCII_USTRINGPARAM( ": " ) );
            aEntry += pMethod->GetName();
            SbxArray* pParams = pMethod->GetParameters();
            SbxInfo* pInfo = pMethod->GetInfo();
            if ( pParams )
            {
                aEntry += '(';
                // 0 is the sub's name...
                for ( sal_uInt16 nParam = 1; nParam < pParams->Count(); nParam++ )
                {
                    SbxVariable* pVar = pParams->Get( nParam );
                    DBG_ASSERT( pVar, "Parameter?!" );
                    if ( pVar->GetName().Len() )
                        aEntry += pVar->GetName();
                    else if ( pInfo )
                    {
                        const SbxParamInfo* pParam = pInfo->GetParam( nParam );
                        if ( pParam )
                            aEntry += pParam->aName;
                    }
                    aEntry += '=';
                    SbxDataType eType = pVar->GetType();
                    if( eType & SbxARRAY )
                        aEntry += String( RTL_CONSTASCII_USTRINGPARAM( "..." ) );
                    else if( eType != SbxOBJECT )
                        aEntry += pVar->GetString();
                    if ( nParam < ( pParams->Count() - 1 ) )
                        aEntry += String( RTL_CONSTASCII_USTRINGPARAM( ", " ) );
                }
                aEntry += ')';
            }
            aTreeListBox.InsertEntry( aEntry, 0, sal_False, LIST_APPEND );
            nScope++;
            pMethod = StarBASIC::GetActiveMethod( nScope );
        }

        SbxBase::ResetError();
        if( eOld != SbxERR_OK )
            SbxBase::SetError( eOld );
    }
    else
    {
        aTreeListBox.SetSelectionMode( NO_SELECTION );
        aTreeListBox.InsertEntry( String(), 0, sal_False, LIST_APPEND );
    }

    aTreeListBox.SetUpdateMode( sal_True );
}

ComplexEditorWindow::ComplexEditorWindow( ModulWindow* pParent ) :
    Window( pParent, WB_3DLOOK | WB_CLIPCHILDREN ),
    aBrkWindow( this ),
    aLineNumberWindow( this, pParent ),
    aEdtWindow( this ),
    aEWVScrollBar( this, WB_VSCROLL | WB_DRAG ),
    bLineNumberDisplay(false),
    bObjectCatalogDisplay(true)
{
    aEdtWindow.SetModulWindow( pParent );
    aBrkWindow.SetModulWindow( pParent );
    aEdtWindow.Show();
    aBrkWindow.Show();

    aEWVScrollBar.SetLineSize( SCROLL_LINE );
    aEWVScrollBar.SetPageSize( SCROLL_PAGE );
    aEWVScrollBar.SetScrollHdl( LINK( this, ComplexEditorWindow, ScrollHdl ) );
    aEWVScrollBar.Show();
}

void ComplexEditorWindow::Resize()
{
    Size aOutSz = GetOutputSizePixel();
    Size aSz( aOutSz );
    aSz.Width() -= 2*DWBORDER;
    aSz.Height() -= 2*DWBORDER;
    long nBrkWidth = 20;
    long nSBWidth = aEWVScrollBar.GetSizePixel().Width();

    Size aBrkSz(nBrkWidth, aSz.Height());

    Size aLnSz(aLineNumberWindow.GetWidth(), aSz.Height());

    if(bLineNumberDisplay)
    {
        aBrkWindow.SetPosSizePixel( Point( DWBORDER, DWBORDER ), aBrkSz );
        aLineNumberWindow.SetPosSizePixel(Point(DWBORDER + aBrkSz.Width() - 1, DWBORDER), aLnSz);
        Size aEWSz(aSz.Width() - nBrkWidth - aLineNumberWindow.GetWidth() - nSBWidth + 2, aSz.Height());
        aEdtWindow.SetPosSizePixel( Point( DWBORDER + aBrkSz.Width() + aLnSz.Width() - 1, DWBORDER ), aEWSz );
    }
    else
    {
        aBrkWindow.SetPosSizePixel( Point( DWBORDER, DWBORDER ), aBrkSz );
        Size aEWSz(aSz.Width() - nBrkWidth - nSBWidth + 2, aSz.Height());
        aEdtWindow.SetPosSizePixel(Point(DWBORDER + aBrkSz.Width() - 1, DWBORDER), aEWSz);
    }

    aEWVScrollBar.SetPosSizePixel( Point( aOutSz.Width() - DWBORDER - nSBWidth, DWBORDER ), Size( nSBWidth, aSz.Height() ) );
}

IMPL_LINK( ComplexEditorWindow, ScrollHdl, ScrollBar *, pCurScrollBar )
{
    if ( aEdtWindow.GetEditView() )
    {
        DBG_ASSERT( pCurScrollBar == &aEWVScrollBar, "Wer scrollt hier ?" );
        long nDiff = aEdtWindow.GetEditView()->GetStartDocPos().Y() - pCurScrollBar->GetThumbPos();
        aEdtWindow.GetEditView()->Scroll( 0, nDiff );
        aBrkWindow.DoScroll( 0, nDiff );
        aLineNumberWindow.DoScroll(0, nDiff);
        aEdtWindow.GetEditView()->ShowCursor( sal_False, sal_True );
        pCurScrollBar->SetThumbPos( aEdtWindow.GetEditView()->GetStartDocPos().Y() );
    }

    return 0;
}

void ComplexEditorWindow::DataChanged(DataChangedEvent const & rDCEvt)
{
    Window::DataChanged(rDCEvt);
    if (rDCEvt.GetType() == DATACHANGED_SETTINGS
        && (rDCEvt.GetFlags() & SETTINGS_STYLE) != 0)
    {
        Color aColor(GetSettings().GetStyleSettings().GetFaceColor());
        if (aColor
            != rDCEvt.GetOldSettings()->GetStyleSettings().GetFaceColor())
        {
            SetBackground(Wallpaper(aColor));
            Invalidate();
        }
    }
}

void ComplexEditorWindow::SetLineNumberDisplay(bool b)
{
    bLineNumberDisplay = b;
    Resize();

    if(b)
        aLineNumberWindow.Show();
    else
        aLineNumberWindow.Hide();
}

void ComplexEditorWindow::SetObjectCatalogDisplay(bool b)
{
    bObjectCatalogDisplay = b;
    Resize();
}

uno::Reference< awt::XWindowPeer >
EditorWindow::GetComponentInterface(sal_Bool bCreate)
{
    uno::Reference< awt::XWindowPeer > xPeer(
        Window::GetComponentInterface(false));
    if (!xPeer.is() && bCreate)
    {
        // Make sure edit engine and view are available:
        if (!pEditEngine)
            CreateEditEngine();

        xPeer = new ::svt::TextWindowPeer(*GetEditView());
        SetComponentInterface(xPeer);
    }
    return xPeer;
}

WatchTreeListBox::WatchTreeListBox( Window* pParent, WinBits nWinBits )
    : SvHeaderTabListBox( pParent, nWinBits )
{}

WatchTreeListBox::~WatchTreeListBox()
{
    // Destroy user data
    SvLBoxEntry* pEntry = First();
    while ( pEntry )
    {
        delete (WatchItem*)pEntry->GetUserData();
        pEntry = Next( pEntry );
    }
}

void WatchTreeListBox::SetTabs()
{
    SvHeaderTabListBox::SetTabs();
    sal_uInt16 nTabCount_ = aTabs.Count();
    for( sal_uInt16 i = 0 ; i < nTabCount_ ; i++ )
    {
        SvLBoxTab* pTab = (SvLBoxTab*)aTabs.GetObject(i);
        if( i == 2 )
            pTab->nFlags |= SV_LBOXTAB_EDITABLE;
        else
            pTab->nFlags &= ~SV_LBOXTAB_EDITABLE;
    }
}

void WatchTreeListBox::RequestingChildren( SvLBoxEntry * pParent )
{
    if( !StarBASIC::IsRunning() )
        return;

    if( GetChildCount( pParent ) > 0 )
        return;

    SvLBoxEntry * pEntry = pParent;
    WatchItem* pItem = (WatchItem*)pEntry->GetUserData();

    SbxDimArray* pArray = pItem->mpArray;
    SbxDimArray* pRootArray = pItem->GetRootArray();
    bool bArrayIsRootArray = false;
    if( !pArray && pRootArray )
    {
        pArray = pRootArray;
        bArrayIsRootArray = true;
    }

    SbxObject* pObj = pItem->mpObject;
    if( pObj )
    {
        createAllObjectProperties( pObj );
         SbxArray* pProps = pObj->GetProperties();
        sal_uInt16 nPropCount = pProps->Count();
        pItem->maMemberList.allocList( nPropCount );

        for( sal_uInt16 i = 0 ; i < nPropCount - 3 ; i++ )
        {
            SbxVariable* pVar = pProps->Get( i );

            String aName( pVar->GetName() );
            pItem->maMemberList.mpMemberNames[i] = aName;
            SvLBoxEntry* pChildEntry = SvTreeListBox::InsertEntry( aName, pEntry );
            WatchItem* pChildItem = new WatchItem();
            pChildItem->maName = aName;
            pChildEntry->SetUserData( pChildItem );
        }
        if( nPropCount > 0 )
        {
            UpdateWatches();
        }
    }
    else if( pArray )
    {
        sal_uInt16 nElementCount = 0;

        // Loop through indices of current level
        int nParentLevel = bArrayIsRootArray ? pItem->nDimLevel : 0;
        int nThisLevel = nParentLevel + 1;
        sal_Int32 nMin, nMax;
        pArray->GetDim32( nThisLevel, nMin, nMax );
        for( sal_Int32 i = nMin ; i <= nMax ; i++ )
        {
            WatchItem* pChildItem = new WatchItem();

            // Copy data and create name
            String aBaseName( pItem->maName );
            pChildItem->maName = aBaseName;

            String aIndexStr = String( RTL_CONSTASCII_USTRINGPARAM( "(" ) );
            pChildItem->mpArrayParentItem = pItem;
            pChildItem->nDimLevel = nThisLevel;
            pChildItem->nDimCount = pItem->nDimCount;
            pChildItem->pIndices = new short[ pChildItem->nDimCount ];
            sal_uInt16 j;
            for( j = 0 ; j < nParentLevel ; j++ )
            {
                short n = pChildItem->pIndices[j] = pItem->pIndices[j];
                aIndexStr += String::CreateFromInt32( n );
                aIndexStr += String( RTL_CONSTASCII_USTRINGPARAM( "," ) );
            }
            pChildItem->pIndices[ nParentLevel ] = sal::static_int_cast<short>( i );
            aIndexStr += String::CreateFromInt32( i );
            aIndexStr += String( RTL_CONSTASCII_USTRINGPARAM( ")" ) );

            String aDisplayName;
            WatchItem* pArrayRootItem = pChildItem->GetRootItem();
            if( pArrayRootItem && pArrayRootItem->mpArrayParentItem )
                aDisplayName = pItem->maDisplayName;
            else
                aDisplayName = aBaseName;
            aDisplayName += aIndexStr;
            pChildItem->maDisplayName = aDisplayName;

            SvLBoxEntry* pChildEntry = SvTreeListBox::InsertEntry( aDisplayName, pEntry );
            nElementCount++;
            pChildEntry->SetUserData( pChildItem );
        }
        if( nElementCount > 0 )
        {
            UpdateWatches();
        }
    }
}

SbxBase* WatchTreeListBox::ImplGetSBXForEntry( SvLBoxEntry* pEntry, bool& rbArrayElement )
{
    SbxBase* pSBX = NULL;
    rbArrayElement = false;

    WatchItem* pItem = (WatchItem*)pEntry->GetUserData();
    String aVName( pItem->maName );

    SvLBoxEntry* pParentEntry = GetParent( pEntry );
    WatchItem* pParentItem = pParentEntry ? (WatchItem*)pParentEntry->GetUserData() : NULL;
    if( pParentItem )
    {
        SbxObject* pObj = pParentItem->mpObject;
        SbxDimArray* pArray;
        if( pObj )
        {
            pSBX = pObj->Find( aVName, SbxCLASS_DONTCARE );

            SbxVariable* pVar;
            if ( pSBX && (pVar = PTR_CAST( SbxVariable, pSBX )) != NULL
                        && !pSBX->ISA( SbxMethod ) )
            {
                // Force getting value
                SbxValues aRes;
                aRes.eType = SbxVOID;
                pVar->Get( aRes );
            }
        }
        // Array?
        else if( (pArray = pItem->GetRootArray()) != NULL )
        {
            rbArrayElement = true;
            if( pParentItem->nDimLevel + 1 == pParentItem->nDimCount )
                pSBX = pArray->Get( pItem->pIndices );
        }
    }
    else
    {
        pSBX = StarBASIC::FindSBXInCurrentScope( aVName );
    }
    return pSBX;
}

sal_Bool WatchTreeListBox::EditingEntry( SvLBoxEntry* pEntry, Selection& )
{
    WatchItem* pItem = (WatchItem*)pEntry->GetUserData();

    sal_Bool bEdit = sal_False;
    if ( StarBASIC::IsRunning() && StarBASIC::GetActiveMethod() && !SbxBase::IsError() )
    {
        // No out of scope entries
        bool bArrayElement;
        SbxBase* pSBX = ImplGetSBXForEntry( pEntry, bArrayElement );
        if ( ( pSBX && pSBX->ISA( SbxVariable ) && !pSBX->ISA( SbxMethod ) ) || bArrayElement )
        {
            // Accept no objects and only end nodes of arrays for editing
            if( !pItem->mpObject && (pItem->mpArray == NULL || pItem->nDimLevel == pItem->nDimCount) )
            {
                aEditingRes = SvHeaderTabListBox::GetEntryText( pEntry, ITEM_ID_VALUE-1 );
                aEditingRes.EraseLeadingChars();
                aEditingRes.EraseTrailingChars();
                bEdit = sal_True;
            }
        }
    }

    if ( !bEdit )
        Sound::Beep();

    return bEdit;
}

sal_Bool WatchTreeListBox::EditedEntry( SvLBoxEntry* pEntry, const rtl::OUString& rNewText )
{
    WatchItem* pItem = (WatchItem*)pEntry->GetUserData();
    String aVName( pItem->maName );

    String aResult = rNewText;
    aResult.EraseLeadingChars();
    aResult.EraseTrailingChars();

    sal_uInt16 nResultLen = aResult.Len();
    sal_Unicode cFirst = aResult.GetChar( 0 );
    sal_Unicode cLast  = aResult.GetChar( nResultLen - 1 );
    if( cFirst == '\"' && cLast == '\"' )
        aResult = aResult.Copy( 1, nResultLen - 2 );

    sal_Bool bResModified = ( aResult != aEditingRes ) ? sal_True : sal_False;
    sal_Bool bError = sal_False;
    if ( !aVName.Len() )
    {
        bError = sal_True;
    }

    sal_Bool bRet = sal_False;

    if ( bError )
    {
        Sound::Beep();
    }
    else if ( bResModified )
    {
        bRet = ImplBasicEntryEdited( pEntry, aResult );
    }

    return bRet;
}

sal_Bool WatchTreeListBox::ImplBasicEntryEdited( SvLBoxEntry* pEntry, const String& rResult )
{
    WatchItem* pItem = (WatchItem*)pEntry->GetUserData();
    String aVName( pItem->maName );

    sal_Bool bError = sal_False;
    String aResult( rResult );
    String aIndex;
    bool bArrayElement;
    SbxBase* pSBX = ImplGetSBXForEntry( pEntry, bArrayElement );

    SbxBase* pToBeChanged = NULL;
    if ( pSBX && pSBX->ISA( SbxVariable ) && !pSBX->ISA( SbxMethod ) )
    {
        SbxVariable* pVar = (SbxVariable*)pSBX;
        SbxDataType eType = pVar->GetType();
        if ( (sal_uInt8)eType == (sal_uInt8)SbxOBJECT )
            bError = sal_True;
        else if ( eType & SbxARRAY )
            bError = sal_True;
        else
            pToBeChanged = pSBX;
    }

    if ( pToBeChanged )
    {
        if ( pToBeChanged->ISA( SbxVariable ) )
        {
            // If the type is variable, the conversion of the SBX does not matter,
            // else the string is converted.
            ((SbxVariable*)pToBeChanged)->PutStringExt( aResult );
        }
        else
            bError = sal_True;
    }

    if ( SbxBase::IsError() )
    {
        bError = sal_True;
        SbxBase::ResetError();
    }

    if ( bError )
        Sound::Beep();

    UpdateWatches();

    // The text should never be taken/copied 1:1,
    // as the UpdateWatches will be lost
    return sal_False;
}


static void implCollapseModifiedObjectEntry( SvLBoxEntry* pParent, WatchTreeListBox* pThis )
{
    pThis->Collapse( pParent );

    SvLBoxTreeList* pModel = pThis->GetModel();
    SvLBoxEntry* pDeleteEntry;
    while( (pDeleteEntry = pThis->SvTreeListBox::GetEntry( pParent, 0 )) != NULL )
    {
        implCollapseModifiedObjectEntry( pDeleteEntry, pThis );

        WatchItem* pItem = (WatchItem*)pDeleteEntry->GetUserData();
        delete pItem;
        pModel->Remove( pDeleteEntry );
    }
}

static String implCreateTypeStringForDimArray( WatchItem* pItem, SbxDataType eType )
{
    String aRetStr = getBasicTypeName( eType );

    SbxDimArray* pArray = pItem->mpArray;
    if( !pArray )
        pArray = pItem->GetRootArray();
    if( pArray )
    {
        int nDimLevel = pItem->nDimLevel;
        int nDims = pItem->nDimCount;
        if( nDimLevel < nDims )
        {
            aRetStr += '(';
            for( int i = nDimLevel ; i < nDims ; i++ )
            {
                short nMin, nMax;
                pArray->GetDim( sal::static_int_cast<short>( i+1 ), nMin, nMax );
                aRetStr += String::CreateFromInt32( nMin );
                aRetStr += String( RTL_CONSTASCII_USTRINGPARAM( " to " ) );
                aRetStr += String::CreateFromInt32( nMax );
                if( i < nDims - 1 )
                    aRetStr += String( RTL_CONSTASCII_USTRINGPARAM( ", " ) );
            }
            aRetStr += ')';
        }
    }
    return aRetStr;
}


void implEnableChildren( SvLBoxEntry* pEntry, bool bEnable )
{
    if( bEnable )
    {
        pEntry->SetFlags(
            (pEntry->GetFlags() &
            ~(SV_ENTRYFLAG_NO_NODEBMP | SV_ENTRYFLAG_HAD_CHILDREN))
            | SV_ENTRYFLAG_CHILDREN_ON_DEMAND );
    }
    else
    {
        pEntry->SetFlags(
            (pEntry->GetFlags() & ~(SV_ENTRYFLAG_CHILDREN_ON_DEMAND)) );
    }
}

void WatchTreeListBox::UpdateWatches( bool bBasicStopped )
{
    SbMethod* pCurMethod = StarBASIC::GetActiveMethod();

    SbxError eOld = SbxBase::GetError();
    setBasicWatchMode( true );

    SvLBoxEntry* pEntry = First();
    while ( pEntry )
    {
        WatchItem* pItem = (WatchItem*)pEntry->GetUserData();
        String aVName( pItem->maName );
        DBG_ASSERT( aVName.Len(), "Var? - Must not be empty!" );
        String aWatchStr;
        String aTypeStr;
        if ( pCurMethod )
        {
            bool bArrayElement;
            SbxBase* pSBX = ImplGetSBXForEntry( pEntry, bArrayElement );

            // Array? If no end node create type string
            if( bArrayElement && pItem->nDimLevel < pItem->nDimCount )
            {
                SbxDimArray* pRootArray = pItem->GetRootArray();
                SbxDataType eType = pRootArray->GetType();
                aTypeStr = implCreateTypeStringForDimArray( pItem, eType );
                implEnableChildren( pEntry, true );
            }

            bool bCollapse = false;
            if ( pSBX && pSBX->ISA( SbxVariable ) && !pSBX->ISA( SbxMethod ) )
            {
                SbxVariable* pVar = (SbxVariable*)pSBX;
                // extra treatment of arrays
                SbxDataType eType = pVar->GetType();
                if ( eType & SbxARRAY )
                {
                    // consider multidimensinal arrays!
                    SbxBase* pBase = pVar->GetObject();
                    if ( pBase && pBase->ISA( SbxDimArray ) )
                    {
                        SbxDimArray* pNewArray = (SbxDimArray*)pBase;
                        SbxDimArray* pOldArray = pItem->mpArray;

                        bool bArrayChanged = false;
                        if( pNewArray != NULL && pOldArray != NULL )
                        {
                            // Compare Array dimensions to see if array has changed
                            // Can be a copy, so comparing pointers does not work
                            sal_uInt16 nOldDims = pOldArray->GetDims();
                            sal_uInt16 nNewDims = pNewArray->GetDims();
                            if( nOldDims != nNewDims )
                            {
                                bArrayChanged = true;
                            }
                            else
                            {
                                for( int i = 0 ; i < nOldDims ; i++ )
                                {
                                    short nOldMin, nOldMax;
                                    short nNewMin, nNewMax;

                                    pOldArray->GetDim( sal::static_int_cast<short>( i+1 ), nOldMin, nOldMax );
                                    pNewArray->GetDim( sal::static_int_cast<short>( i+1 ), nNewMin, nNewMax );
                                    if( nOldMin != nNewMin || nOldMax != nNewMax )
                                    {
                                        bArrayChanged = true;
                                        break;
                                    }
                                }
                            }
                        }
                        else if( pNewArray == NULL || pOldArray == NULL )
                            bArrayChanged = true;

                        if( pNewArray )
                            implEnableChildren( pEntry, true );

                        // #i37227 Clear always and replace array
                        if( pNewArray != pOldArray )
                        {
                            pItem->clearWatchItem( false );
                            if( pNewArray )
                            {
                                implEnableChildren( pEntry, true );

                                pItem->mpArray = pNewArray;
                                sal_uInt16 nDims = pNewArray->GetDims();
                                pItem->nDimLevel = 0;
                                pItem->nDimCount = nDims;
                            }
                        }
                        if( bArrayChanged && pOldArray != NULL )
                            bCollapse = true;

                        aTypeStr = implCreateTypeStringForDimArray( pItem, eType );
                    }
                    else
                        aWatchStr += String( RTL_CONSTASCII_USTRINGPARAM( "<?>" ) );
                }
                else if ( (sal_uInt8)eType == (sal_uInt8)SbxOBJECT )
                {
                    SbxObject* pObj = NULL;
                    SbxBase* pBase = pVar->GetObject();
                    if( pBase && pBase->ISA( SbxObject ) )
                        pObj = (SbxObject*)pBase;

                    if( pObj )
                    {
                        // Check if member list has changed
                        bool bObjChanged = false;
                        if( pItem->mpObject != NULL && pItem->maMemberList.mpMemberNames != NULL )
                        {
                            SbxArray* pProps = pObj->GetProperties();
                            sal_uInt16 nPropCount = pProps->Count();
                            for( sal_uInt16 i = 0 ; i < nPropCount - 3 ; i++ )
                            {
                                SbxVariable* pVar_ = pProps->Get( i );
                                String aName( pVar_->GetName() );
                                if( pItem->maMemberList.mpMemberNames[i] != aName )
                                {
                                    bObjChanged = true;
                                    break;
                                }
                            }
                            if( bObjChanged )
                                bCollapse = true;
                        }

                        pItem->mpObject = pObj;
                        implEnableChildren( pEntry, true );
                        aTypeStr = getBasicObjectTypeName( pObj );
                    }
                    else
                    {
                        aWatchStr = String( RTL_CONSTASCII_USTRINGPARAM( "Null" ) );
                        if( pItem->mpObject != NULL )
                        {
                            bCollapse = true;
                            pItem->clearWatchItem( false );

                            implEnableChildren( pEntry, false );
                        }
                    }
                }
                else
                {
                    if( pItem->mpObject != NULL )
                    {
                        bCollapse = true;
                        pItem->clearWatchItem( false );

                        implEnableChildren( pEntry, false );
                    }

                    bool bString = ((sal_uInt8)eType == (sal_uInt8)SbxSTRING);
                    String aStrStr( RTL_CONSTASCII_USTRINGPARAM( "\"" ) );
                    if( bString )
                        aWatchStr += aStrStr;
                    aWatchStr += pVar->GetString();
                    if( bString )
                        aWatchStr += aStrStr;
                }
                if( !aTypeStr.Len() )
                {
                    if( !pVar->IsFixed() )
                        aTypeStr = String( RTL_CONSTASCII_USTRINGPARAM( "Variant/" ) );
                    aTypeStr += getBasicTypeName( pVar->GetType() );
                }
            }
            else if( !bArrayElement )
                aWatchStr += String( RTL_CONSTASCII_USTRINGPARAM( "<Out of Scope>" ) );

            if( bCollapse )
                implCollapseModifiedObjectEntry( pEntry, this );

        }
        else if( bBasicStopped )
        {
            if( pItem->mpObject || pItem->mpArray )
            {
                implCollapseModifiedObjectEntry( pEntry, this );
                pItem->mpObject = NULL;
            }
        }

        SvHeaderTabListBox::SetEntryText( aWatchStr, pEntry, ITEM_ID_VALUE-1 );
        SvHeaderTabListBox::SetEntryText( aTypeStr, pEntry, ITEM_ID_TYPE-1 );

        pEntry = Next( pEntry );
    }

    // Force redraw
    Invalidate();

    SbxBase::ResetError();
    if( eOld != SbxERR_OK )
        SbxBase::SetError( eOld );
    setBasicWatchMode( false );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
