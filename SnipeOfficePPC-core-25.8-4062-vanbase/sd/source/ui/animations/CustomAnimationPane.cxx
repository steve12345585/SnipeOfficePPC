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


#include <com/sun/star/presentation/EffectPresetClass.hpp>
#include <com/sun/star/animations/XAnimationNodeSupplier.hpp>
#include <com/sun/star/view/XSelectionSupplier.hpp>
#include <com/sun/star/drawing/XDrawView.hpp>
#include <com/sun/star/drawing/XShape.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/presentation/EffectNodeType.hpp>
#include <com/sun/star/presentation/EffectCommands.hpp>
#include <com/sun/star/animations/AnimationTransformType.hpp>
#include <com/sun/star/text/XTextRangeCompare.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <com/sun/star/presentation/ParagraphTarget.hpp>
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/drawing/LineStyle.hpp>
#include <com/sun/star/drawing/FillStyle.hpp>
#include <comphelper/processfactory.hxx>
#include <sfx2/dispatch.hxx>
#include "STLPropertySet.hxx"
#include "CustomAnimationPane.hxx"
#include "CustomAnimationDialog.hxx"
#include "CustomAnimationCreateDialog.hxx"
#include "CustomAnimationPane.hrc"
#include "CustomAnimation.hrc"
#include "CustomAnimationList.hxx"
#include <vcl/lstbox.hxx>
#include <vcl/fixed.hxx>

#include <vcl/button.hxx>
#include <vcl/combobox.hxx>
#include <vcl/scrbar.hxx>

#include <comphelper/sequence.hxx>
#include <sfx2/frame.hxx>

#include <svx/unoapi.hxx>
#include <svx/svxids.hrc>
#include <DrawDocShell.hxx>
#include <ViewShellBase.hxx>
#include "DrawViewShell.hxx"
#include "DrawController.hxx"
#include "sdresid.hxx"
#include "drawview.hxx"
#include "slideshow.hxx"
#include "undoanim.hxx"
#include "optsitem.hxx"
#include "sddll.hxx"
#include "framework/FrameworkHelper.hxx"

#include "EventMultiplexer.hxx"
#include "DialogListBox.hxx"

#include "glob.hrc"
#include "sdpage.hxx"
#include "drawdoc.hxx"
#include "app.hrc"

#include <memory>
#include <algorithm>

#include <basegfx/polygon/b2dpolypolygontools.hxx>
#include <basegfx/matrix/b2dhommatrix.hxx>
#include <basegfx/range/b2drange.hxx>

using namespace ::com::sun::star;
using namespace ::com::sun::star::animations;
using namespace ::com::sun::star::presentation;
using namespace ::com::sun::star::text;

using ::rtl::OUString;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::drawing;
using ::com::sun::star::view::XSelectionSupplier;
using ::com::sun::star::view::XSelectionChangeListener;
using ::com::sun::star::frame::XController;
using ::com::sun::star::frame::XModel;
using ::com::sun::star::beans::XPropertySet;
using ::com::sun::star::beans::XPropertyChangeListener;
using ::com::sun::star::container::XIndexAccess;
using ::com::sun::star::container::XEnumerationAccess;
using ::com::sun::star::container::XEnumeration;
using ::com::sun::star::text::XText;
using ::sd::framework::FrameworkHelper;

namespace sd {

// --------------------------------------------------------------------

void fillDurationComboBox( ComboBox* pBox )
{
    static const double gdVerySlow = 5.0;
    static const double gdSlow = 3.0;
    static const double gdNormal = 2.0;
    static const double gdFast = 1.0;
    static const double gdVeryFast = 0.5;

    String aVerySlow( SdResId( STR_CUSTOMANIMATION_DURATION_VERY_SLOW ) );
    pBox->SetEntryData( pBox->InsertEntry( aVerySlow ), (void*)&gdVerySlow );

    String aSlow( SdResId( STR_CUSTOMANIMATION_DURATION_SLOW ) );
    pBox->SetEntryData( pBox->InsertEntry( aSlow ), (void*)&gdSlow );

    String aNormal( SdResId( STR_CUSTOMANIMATION_DURATION_NORMAL ) );
    pBox->SetEntryData( pBox->InsertEntry( aNormal ), (void*)&gdNormal );

    String aFast( SdResId( STR_CUSTOMANIMATION_DURATION_FAST ) );
    pBox->SetEntryData( pBox->InsertEntry( aFast ), (void*)&gdFast );

    String aVeryFast( SdResId( STR_CUSTOMANIMATION_DURATION_VERY_FAST ) );
    pBox->SetEntryData( pBox->InsertEntry( aVeryFast ), (void*)&gdVeryFast );
}

void fillRepeatComboBox( ComboBox* pBox )
{
    String aNone( SdResId( STR_CUSTOMANIMATION_REPEAT_NONE ) );
    pBox->SetEntryData( pBox->InsertEntry( aNone ), (void*)((sal_Int32)0) );

    pBox->SetEntryData( pBox->InsertEntry( String::CreateFromInt32( 2 ) ), (void*)((sal_Int32)1) );
    pBox->SetEntryData( pBox->InsertEntry( String::CreateFromInt32( 3 ) ), (void*)((sal_Int32)3) );
    pBox->SetEntryData( pBox->InsertEntry( String::CreateFromInt32( 4 ) ), (void*)((sal_Int32)4) );
    pBox->SetEntryData( pBox->InsertEntry( String::CreateFromInt32( 5 ) ), (void*)((sal_Int32)5) );
    pBox->SetEntryData( pBox->InsertEntry( String::CreateFromInt32( 10 ) ), (void*)((sal_Int32)10) );

    String aUntilClick( SdResId( STR_CUSTOMANIMATION_REPEAT_UNTIL_NEXT_CLICK ) );
    pBox->SetEntryData( pBox->InsertEntry( aUntilClick ), (void*)((sal_Int32)-1) );

    String aEndOfSlide( SdResId( STR_CUSTOMANIMATION_REPEAT_UNTIL_END_OF_SLIDE ) );
    pBox->SetEntryData( pBox->InsertEntry( aEndOfSlide ), (void*)((sal_Int32)-2) );
}

// --------------------------------------------------------------------

CustomAnimationPane::CustomAnimationPane( ::Window* pParent, ViewShellBase& rBase, const Size& rMinSize )
:   Control( pParent, SdResId(DLG_CUSTOMANIMATIONPANE) ),
    mrBase( rBase ),
    mpCustomAnimationPresets(NULL),
    mnPropertyType( nPropertyTypeNone ),
    maMinSize( rMinSize ),
    mxModel( rBase.GetDocShell()->GetDoc()->getUnoModel(), UNO_QUERY ),
    maLateInitTimer()
{
    // load resources
    mpFLEffect = new FixedLine( this, SdResId( FL_EFFECT ) );

    mpPBAddEffect = new PushButton( this, SdResId( PB_ADD_EFFECT ) );
    mpPBChangeEffect = new PushButton( this, SdResId( PB_CHANGE_EFFECT ) );
    mpPBRemoveEffect = new PushButton( this, SdResId( PB_REMOVE_EFFECT ) );

    mpFLModify = new FixedLine( this, SdResId( FL_MODIFY ) );

    mpFTStart = new FixedText( this, SdResId( FT_START ) );
    mpLBStart = new ListBox( this, SdResId( LB_START ) );
    mpFTProperty = new FixedText( this, SdResId( FT_PROPERTY ) );
    mpLBProperty = new PropertyControl( this, SdResId( LB_PROPERTY ) );
    mpPBPropertyMore = new PushButton( this, SdResId( PB_PROPERTY_MORE ) );

    mpFTSpeed = new FixedText( this, SdResId( FT_SPEED ) );
    mpCBSpeed = new ComboBox( this, SdResId( CB_SPEED ) );

    mpCustomAnimationList = new CustomAnimationList( this, SdResId( CT_CUSTOM_ANIMATION_LIST ), this );

    mpPBMoveUp = new PushButton( this, SdResId( PB_MOVE_UP ) );
    mpPBMoveDown = new PushButton( this, SdResId( PB_MOVE_DOWN ) );
    mpFTChangeOrder = new FixedText( this, SdResId( FT_CHANGE_ORDER ) );
    mpFLSeperator1 = new FixedLine( this, SdResId( FL_SEPERATOR1 ) );
    mpPBPlay = new PushButton( this, SdResId( PB_PLAY ) );
    mpPBSlideShow = new PushButton( this, SdResId( PB_SLIDE_SHOW ) );
    mpFLSeperator2 = new FixedLine( this, SdResId( FL_SEPERATOR2 ) );
    mpCBAutoPreview = new CheckBox( this, SdResId( CB_AUTOPREVIEW ) );

    maStrProperty = mpFTProperty->GetText();

    FreeResource();

    // use bold font for group headings (same font for all fixed lines):
    Font font( mpFLEffect->GetFont() );
    font.SetWeight( WEIGHT_BOLD );
    mpFLEffect->SetFont( font );
    mpFLModify->SetFont( font );

    fillDurationComboBox( mpCBSpeed );
    mpPBMoveUp->SetSymbol( SYMBOL_ARROW_UP );
    mpPBMoveDown->SetSymbol( SYMBOL_ARROW_DOWN );

    mpPBAddEffect->SetClickHdl( LINK( this, CustomAnimationPane, implControlHdl ) );
    mpPBChangeEffect->SetClickHdl( LINK( this, CustomAnimationPane, implControlHdl ) );
    mpPBRemoveEffect->SetClickHdl( LINK( this, CustomAnimationPane, implControlHdl ) );
    mpLBStart->SetSelectHdl( LINK( this, CustomAnimationPane, implControlHdl ) );
    mpCBSpeed->SetSelectHdl( LINK( this, CustomAnimationPane, implControlHdl ) );
    mpPBPropertyMore->SetClickHdl( LINK( this, CustomAnimationPane, implControlHdl ) );
    mpPBMoveUp->SetClickHdl( LINK( this, CustomAnimationPane, implControlHdl ) );
    mpPBMoveDown->SetClickHdl( LINK( this, CustomAnimationPane, implControlHdl ) );
    mpPBPlay->SetClickHdl( LINK( this, CustomAnimationPane, implControlHdl ) );
    mpPBSlideShow->SetClickHdl( LINK( this, CustomAnimationPane, implControlHdl ) );
    mpCBAutoPreview->SetClickHdl( LINK( this, CustomAnimationPane, implControlHdl ) );

    maStrModify = mpFLEffect->GetText();

    // resize controls according to current size
    updateLayout();

    // get current controller and initialize listeners
    try
    {
        mxView = Reference< XDrawView >::query(mrBase.GetController());
        addListener();
    }
    catch( Exception& )
    {
        OSL_FAIL( "sd::CustomAnimationPane::CustomAnimationPane(), Exception caught!" );
    }

    // get current page and update custom animation list
    onChangeCurrentPage();

    // Wait a short time before the presets list is created.  This gives the
    // system time to paint the control.
    maLateInitTimer.SetTimeout(100);
    maLateInitTimer.SetTimeoutHdl(LINK(this, CustomAnimationPane, lateInitCallback));
    maLateInitTimer.Start();
}

CustomAnimationPane::~CustomAnimationPane()
{
    maLateInitTimer.Stop();

    removeListener();

    MotionPathTagVector aTags;
    aTags.swap( maMotionPathTags );
    MotionPathTagVector::iterator aIter;
    for( aIter = aTags.begin(); aIter != aTags.end(); ++aIter )
        (*aIter)->Dispose();

    delete mpFLModify;
    delete mpPBAddEffect;
    delete mpPBChangeEffect;
    delete mpPBRemoveEffect;
    delete mpFLEffect;
    delete mpFTStart;
    delete mpLBStart;
    delete mpFTProperty;
    delete mpLBProperty;
    delete mpPBPropertyMore;
    delete mpFTSpeed;
    delete mpCBSpeed;
    delete mpCustomAnimationList;
    delete mpFTChangeOrder;
    delete mpPBMoveUp;
    delete mpPBMoveDown;
    delete mpFLSeperator1;
    delete mpPBPlay;
    delete mpPBSlideShow;
    delete mpFLSeperator2;
    delete mpCBAutoPreview;
}

void CustomAnimationPane::addUndo()
{
    ::svl::IUndoManager* pManager = mrBase.GetDocShell()->GetUndoManager();
    if( pManager )
    {
        SdPage* pPage = SdPage::getImplementation( mxCurrentPage );
        if( pPage )
            pManager->AddUndoAction( new UndoAnimation( mrBase.GetDocShell()->GetDoc(), pPage ) );
    }
}

void CustomAnimationPane::Resize()
{
    updateLayout();
}

void CustomAnimationPane::StateChanged( StateChangedType nStateChange )
{
    Control::StateChanged( nStateChange );

    if( nStateChange == STATE_CHANGE_VISIBLE )
        updateMotionPathTags();
}

void CustomAnimationPane::KeyInput( const KeyEvent& rKEvt )
{
    if( mpCustomAnimationList )
        mpCustomAnimationList->KeyInput( rKEvt );
}

void CustomAnimationPane::addListener()
{
    Link aLink( LINK(this,CustomAnimationPane,EventMultiplexerListener) );
    mrBase.GetEventMultiplexer()->AddEventListener (
        aLink,
        tools::EventMultiplexerEvent::EID_EDIT_VIEW_SELECTION
        | tools::EventMultiplexerEvent::EID_CURRENT_PAGE
        | tools::EventMultiplexerEvent::EID_MAIN_VIEW_REMOVED
        | tools::EventMultiplexerEvent::EID_MAIN_VIEW_ADDED
        | tools::EventMultiplexerEvent::EID_DISPOSING
        | tools::EventMultiplexerEvent::EID_END_TEXT_EDIT);
}

void CustomAnimationPane::removeListener()
{
    Link aLink( LINK(this,CustomAnimationPane,EventMultiplexerListener) );
    mrBase.GetEventMultiplexer()->RemoveEventListener( aLink );
}

IMPL_LINK(CustomAnimationPane,EventMultiplexerListener,
    tools::EventMultiplexerEvent*,pEvent)
{
    switch (pEvent->meEventId)
    {
        case tools::EventMultiplexerEvent::EID_EDIT_VIEW_SELECTION:
            onSelectionChanged();
            break;

        case tools::EventMultiplexerEvent::EID_CURRENT_PAGE:
            onChangeCurrentPage();
            break;

        case tools::EventMultiplexerEvent::EID_MAIN_VIEW_ADDED:
            // At this moment the controller may not yet been set at model
            // or ViewShellBase.  Take it from the view shell passed with
            // the event.
            if (mrBase.GetMainViewShell() != NULL)
            {
                if( mrBase.GetMainViewShell()->GetShellType() == ViewShell::ST_IMPRESS )
                {
                    mxView = Reference<XDrawView>::query(mrBase.GetDrawController());
                    onSelectionChanged();
                    onChangeCurrentPage();
                    break;
                }
            }
        // fall through intended
        case tools::EventMultiplexerEvent::EID_MAIN_VIEW_REMOVED:
            mxView = 0;
            mxCurrentPage = 0;
            updateControls();
            break;

        case tools::EventMultiplexerEvent::EID_DISPOSING:
            mxView = Reference<XDrawView>();
            onSelectionChanged();
            onChangeCurrentPage();
            break;
        case tools::EventMultiplexerEvent::EID_END_TEXT_EDIT:
            if( mpMainSequence.get() && pEvent->mpUserData )
                mpCustomAnimationList->update( mpMainSequence );
            break;
    }
    return 0;
}


void CustomAnimationPane::updateLayout()
{
    Size aPaneSize( GetSizePixel() );
    if( aPaneSize.Width() < maMinSize.Width() )
        aPaneSize.Width() = maMinSize.Width();

    if( aPaneSize.Height() < maMinSize.Height() )
        aPaneSize.Height() = maMinSize.Height();

    Point aOffset( LogicToPixel( Point(3,3), MAP_APPFONT ) );
    Point aCursor( aOffset );

    // place the modify fixed line

    // place the "modify effect" fixed line
    Size aSize( mpFLModify->GetSizePixel() );
    aSize.Width() = aPaneSize.Width() - 2 * aOffset.X();

    mpFLModify->SetPosSizePixel( aCursor, aSize );

    aCursor.Y() += aSize.Height() + aOffset.Y();

    const int nButtonExtraWidth = 4 * aOffset.X();

    // the "add effect" button is placed top-left
    Size aCtrlSize( mpPBAddEffect->GetSizePixel() );
    aCtrlSize.setWidth( mpPBAddEffect->CalcMinimumSize( aSize.Width() ).getWidth() + nButtonExtraWidth );
    mpPBAddEffect->SetPosSizePixel( aCursor, aCtrlSize );

    aCursor.X() += aOffset.X() + aCtrlSize.Width();

    // place the "change effect" button

    // if the "change" button does not fit right of the "add effect", put it on the next line
    aCtrlSize = mpPBChangeEffect->GetSizePixel();
    aCtrlSize.setWidth( mpPBChangeEffect->CalcMinimumSize( aSize.Width() ).getWidth() + nButtonExtraWidth );
    if( ( aCursor.X() + aCtrlSize.Width() + aOffset.X() ) > aPaneSize.Width() )
    {
        aCursor.X() = aOffset.X();
        aCursor.Y() += aCtrlSize.Height() + aOffset.Y();
    }
    mpPBChangeEffect->SetPosSizePixel( aCursor, aCtrlSize );

    aCursor.X() += aOffset.X() + aCtrlSize.Width();

    // place the "remove effect" button

    // if the "remove" button does not fit right of the "add effect", put it on the next line
    aCtrlSize = mpPBRemoveEffect->GetSizePixel();
    aCtrlSize.setWidth( mpPBRemoveEffect->CalcMinimumSize( aSize.Width() ).getWidth() + nButtonExtraWidth );
    if( ( aCursor.X() + aCtrlSize.Width() + aOffset.X() ) > aPaneSize.Width() )
    {
        aCursor.X() = aOffset.X();
        aCursor.Y() += aCtrlSize.Height() + aOffset.Y();
    }

    mpPBRemoveEffect->SetPosSizePixel( aCursor, aCtrlSize );

    aCursor.X() = aOffset.X();
    aCursor.Y() += aCtrlSize.Height() + 2 * aOffset.Y();

    // place the "modify effect" fixed line
    aSize = mpFLEffect->GetSizePixel();
    aSize.Width() = aPaneSize.Width() - 2 * aOffset.X();

    mpFLEffect->SetPosSizePixel( aCursor, aSize );

    aCursor.Y() += aSize.Height() + aOffset.Y();

    // ---------------------------------------------------------------------------
    // place the properties controls

    // calc minimum width for fixedtext

    Size aFixedTextSize( mpFTStart->CalcMinimumSize() );
    long nWidth = aFixedTextSize.Width();
    aFixedTextSize = mpFTProperty->CalcMinimumSize();
    nWidth = std::max( nWidth, aFixedTextSize.Width() );
    aFixedTextSize = mpFTSpeed->CalcMinimumSize();
    aFixedTextSize.Width() = std::max( nWidth, aFixedTextSize.Width() ) + aOffset.X();
    mpFTStart->SetSizePixel(aFixedTextSize);
    mpFTProperty->SetSizePixel(aFixedTextSize);
    mpFTSpeed->SetSizePixel(aFixedTextSize);

    aSize = mpPBPropertyMore->GetSizePixel();

    // place the "start" fixed text

    Point aFTPos( aCursor );
    Point aLBPos( aCursor );
    Size aListBoxSize( LogicToPixel( Size( 60, 12 ), MAP_APPFONT ) );
    long nDeltaY = aListBoxSize.Height() + aOffset.Y();

    // linebreak?
    if( (aFixedTextSize.Width() + aListBoxSize.Width() + aSize.Width() + 4 * aOffset.X()) > aPaneSize.Width() )
    {
        // y position for list box is below fixed text
        aLBPos.Y() += aFixedTextSize.Height() + aOffset.Y();

        // height of fixed text + list box + something = 2 * list box
        nDeltaY = aListBoxSize.Height() +  aFixedTextSize.Height() + 2*aOffset.Y();
    }
    else
    {
        // x position for list box is right of fixed text
        aLBPos.X() += aFixedTextSize.Width() + aOffset.X();

        if( aListBoxSize.Height() > aFixedTextSize.Height() )
            aFTPos.Y() = aLBPos.Y() + ((aListBoxSize.Height() - aFixedTextSize.Height()) >> 1);
        else
            aLBPos.Y() = aFTPos.Y() + ((aFixedTextSize.Height() - aListBoxSize.Height()) >> 1);
    }

    // width of the listbox is from its left side until end of pane
    aListBoxSize.Width() = aPaneSize.Width() - aLBPos.X() - aSize.Width() - 2 * aOffset.X();

    mpFTStart->SetPosPixel( aFTPos );
    mpLBStart->SetPosSizePixel( aLBPos, aListBoxSize );

    aFTPos.Y() += nDeltaY; aLBPos.Y() += nDeltaY;

    mpFTProperty->SetPosPixel( aFTPos );
    mpLBProperty->SetPosSizePixel( aLBPos, aListBoxSize );
    mpLBProperty->Resize();

    Point aMorePos( aLBPos );
    aMorePos.X() += aListBoxSize.Width() + aOffset.X();
    mpPBPropertyMore->SetPosPixel( aMorePos );

    aFTPos.Y() += nDeltaY; aLBPos.Y() += nDeltaY;

    mpFTSpeed->SetPosPixel( aFTPos );
    mpCBSpeed->SetPosSizePixel( aLBPos, aListBoxSize );

    aFTPos.Y() += nDeltaY + aOffset.Y();

    Point aListPos( aFTPos );

    // positionate the buttons on the bottom

    // place the auto preview checkbox
    aCursor = Point( aOffset.X(), aPaneSize.Height() - mpCBAutoPreview->GetSizePixel().Height() - aOffset.Y() );
    mpCBAutoPreview->SetPosPixel( aCursor );

    // place the seperator 2 fixed line
    aCursor.Y() -= /* aOffset.Y() + */ mpFLSeperator2->GetSizePixel().Height();
    aSize = mpFLSeperator2->GetSizePixel();
    aSize.Width() = aPaneSize.Width() - 2 * aOffset.X();
    mpFLSeperator2->SetPosSizePixel( aCursor, aSize );

    // next, layout and place the play and slide show buttons
    aCtrlSize = mpPBSlideShow->GetSizePixel();
    aCtrlSize.setWidth( mpPBSlideShow->CalcMinimumSize( aSize.Width() ).getWidth() + nButtonExtraWidth );

    Size aPlaySize( mpPBPlay->GetSizePixel() );
    aPlaySize.setWidth( mpPBPlay->CalcMinimumSize( aSize.Width() ).getWidth() + nButtonExtraWidth );

    aCursor.Y() -= aCtrlSize.Height() /* + aOffset.Y() */;

    // do we need two lines for the buttons?
    int aTestWidth = aCursor.X() + mpPBPlay->GetSizePixel().Width() + 2 * aOffset.X() + mpPBSlideShow->GetSizePixel().Width();
    if( aTestWidth > aPaneSize.Width() )
    {
        mpPBSlideShow->SetPosSizePixel( aCursor, aCtrlSize );
        aCursor.Y() -= aCtrlSize.Height() + aOffset.Y();
        mpPBPlay->SetPosSizePixel( aCursor, aPlaySize );
    }
    else
    {
        mpPBPlay->SetPosSizePixel( aCursor, aPlaySize );
        aCursor.X() += aPlaySize.Width() + aOffset.X();
        mpPBSlideShow->SetPosSizePixel( aCursor, aCtrlSize );
    }

    // place the seperator 1 fixed line
    aCursor.X() = aOffset.X();
    aCursor.Y() -= /* aOffset.Y() + */ mpFLSeperator1->GetSizePixel().Height();
    aSize = mpFLSeperator1->GetSizePixel();
    aSize.Width() = aPaneSize.Width() - 2 * aOffset.X();
    mpFLSeperator1->SetPosSizePixel( aCursor, aSize );

    // place the move down button
    aSize = mpPBMoveDown->GetSizePixel();

    aCursor.X() = aPaneSize.Width() - aOffset.X() - aSize.Width();
    aCursor.Y() -= aOffset.Y() + aSize.Height();
    mpPBMoveDown->SetPosPixel( aCursor );

    aCursor.X() -= aOffset.X() + aSize.Width();
    mpPBMoveUp->SetPosPixel( aCursor );

    // Place the change order label.
    // Its width has to be calculated dynamically so that is can be
    // displayed flush right without having too much space to the buttons
    // with some languages or truncated text with others.
    mpFTChangeOrder->SetSizePixel(mpFTChangeOrder->CalcMinimumSize());

    aCursor.X() -= aOffset.X() + mpFTChangeOrder->GetSizePixel().Width();
    aCursor.Y() += (aSize.Height() - mpFTChangeOrder->GetSizePixel().Height()) >> 1;
    mpFTChangeOrder->SetPosPixel( aCursor );

    // positionate the custom animation list control
    Size aCustomAnimationListSize( aPaneSize.Width() - aListPos.X() - aOffset.X(), aCursor.Y() - aListPos.Y() - 2 * aOffset.Y() );
    mpCustomAnimationList->SetPosSizePixel( aListPos, aCustomAnimationListSize );
}

static sal_Int32 getPropertyType( const OUString& rProperty )
{
    if ( rProperty == "Direction" )
        return nPropertyTypeDirection;

    if ( rProperty == "Spokes" )
        return nPropertyTypeSpokes;

    if ( rProperty == "Zoom" )
        return nPropertyTypeZoom;

    if ( rProperty == "Accelerate" )
        return nPropertyTypeAccelerate;

    if ( rProperty == "Decelerate" )
        return nPropertyTypeDecelerate;

    if ( rProperty == "Color1" )
        return nPropertyTypeFirstColor;

    if ( rProperty == "Color2" )
        return nPropertyTypeSecondColor;

    if ( rProperty == "FillColor" )
        return nPropertyTypeFillColor;

    if ( rProperty == "ColorStyle" )
        return nPropertyTypeColorStyle;

    if ( rProperty == "AutoReverse" )
        return nPropertyTypeAutoReverse;

    if ( rProperty == "FontStyle" )
        return nPropertyTypeFont;

    if ( rProperty == "CharColor" )
        return nPropertyTypeCharColor;

    if ( rProperty == "CharHeight" )
        return nPropertyTypeCharHeight;

    if ( rProperty == "CharDecoration" )
        return nPropertyTypeCharDecoration;

    if ( rProperty == "LineColor" )
        return nPropertyTypeLineColor;

    if ( rProperty == "Rotate" )
        return nPropertyTypeRotate;

    if ( rProperty == "Transparency" )
        return nPropertyTypeTransparency;

    if ( rProperty == "Color" )
        return nPropertyTypeColor;

    if ( rProperty == "Scale" )
        return nPropertyTypeScale;

    return nPropertyTypeNone;
}

OUString getPropertyName( sal_Int32 nPropertyType )
{
    switch( nPropertyType )
    {
    case nPropertyTypeDirection:
        return OUString( String( SdResId( STR_CUSTOMANIMATION_DIRECTION_PROPERTY ) ) );

    case nPropertyTypeSpokes:
        return OUString( String( SdResId( STR_CUSTOMANIMATION_SPOKES_PROPERTY ) ) );

    case nPropertyTypeFirstColor:
        return OUString( String( SdResId( STR_CUSTOMANIMATION_FIRST_COLOR_PROPERTY ) ) );

    case nPropertyTypeSecondColor:
        return OUString( String( SdResId( STR_CUSTOMANIMATION_SECOND_COLOR_PROPERTY ) ) );

    case nPropertyTypeZoom:
        return OUString( String( SdResId( STR_CUSTOMANIMATION_ZOOM_PROPERTY ) ) );

    case nPropertyTypeFillColor:
        return OUString( String( SdResId( STR_CUSTOMANIMATION_FILL_COLOR_PROPERTY ) ) );

    case nPropertyTypeColorStyle:
        return OUString( String( SdResId( STR_CUSTOMANIMATION_STYLE_PROPERTY ) ) );

    case nPropertyTypeFont:
        return OUString( String( SdResId( STR_CUSTOMANIMATION_FONT_PROPERTY ) ) );

    case nPropertyTypeCharHeight:
        return OUString( String( SdResId( STR_CUSTOMANIMATION_SIZE_PROPERTY ) ) );

    case nPropertyTypeCharColor:
        return OUString( String( SdResId( STR_CUSTOMANIMATION_FONT_COLOR_PROPERTY ) ) );

    case nPropertyTypeCharHeightStyle:
        return OUString( String( SdResId( STR_CUSTOMANIMATION_FONT_SIZE_STYLE_PROPERTY ) ) );

    case nPropertyTypeCharDecoration:
        return OUString( String( SdResId( STR_CUSTOMANIMATION_FONT_STYLE_PROPERTY ) ) );

    case nPropertyTypeLineColor:
        return OUString( String( SdResId( STR_CUSTOMANIMATION_LINE_COLOR_PROPERTY ) ) );

    case nPropertyTypeRotate:
        return OUString( String( SdResId( STR_CUSTOMANIMATION_AMOUNT_PROPERTY ) ) );

    case nPropertyTypeColor:
        return OUString( String( SdResId( STR_CUSTOMANIMATION_COLOR_PROPERTY ) ) );

    case nPropertyTypeTransparency:
        return OUString( String( SdResId( STR_CUSTOMANIMATION_AMOUNT_PROPERTY ) ) );

    case nPropertyTypeScale:
        return OUString( String( SdResId( STR_CUSTOMANIMATION_SCALE_PROPERTY ) ) );
    }

    OUString aStr;
    return aStr;
}

void CustomAnimationPane::updateControls()
{
    mpFLModify->Enable( mxView.is() );
    mpFTSpeed->Enable( mxView.is() );
    mpCBSpeed->Enable( mxView.is() );
    mpCustomAnimationList->Enable( mxView.is() );
    mpFTChangeOrder->Enable( mxView.is() );
    mpPBMoveUp->Enable( mxView.is() );
    mpPBMoveDown->Enable( mxView.is() );
    mpFLSeperator1->Enable( mxView.is() );
    mpPBPlay->Enable( mxView.is() );
    mpPBSlideShow->Enable( mxView.is() );
    mpFLSeperator2->Enable( mxView.is() );
    mpCBAutoPreview->Enable( mxView.is() );

    if( !mxView.is() )
    {
        mpPBAddEffect->Enable( sal_False );
        mpPBChangeEffect->Enable( sal_False );
        mpPBRemoveEffect->Enable( sal_False );
        mpFLEffect->Enable( sal_False );
        mpFTStart->Enable( sal_False );
        mpLBStart->Enable( sal_False );
        mpPBPropertyMore->Enable( sal_False );
        mpLBProperty->Enable( sal_False );
        mpFTProperty->Enable( sal_False );
        mpCustomAnimationList->clear();
        return;
    }

    const int nSelectionCount = maListSelection.size();

    mpPBAddEffect->Enable( maViewSelection.hasValue() );
    mpPBChangeEffect->Enable( nSelectionCount);
    mpPBRemoveEffect->Enable(nSelectionCount);

    mpFLEffect->Enable(nSelectionCount > 0);
    mpFTStart->Enable(nSelectionCount > 0);
    mpLBStart->Enable(nSelectionCount > 0);
    mpPBPropertyMore->Enable(nSelectionCount > 0);

    mpFTProperty->SetText( maStrProperty );

    mnPropertyType = nPropertyTypeNone;

    if( nSelectionCount == 1 )
    {
        CustomAnimationEffectPtr pEffect = maListSelection.front();

        OUString aUIName( getPresets().getUINameForPresetId( pEffect->getPresetId() ) );

        OUString aTemp( maStrModify );

        if( !aUIName.isEmpty() )
        {
            aTemp += OUString( (sal_Unicode)' ' );
            aTemp += aUIName;
        }
        mpFLEffect->SetText( aTemp );

        CustomAnimationPresetPtr pDescriptor = getPresets().getEffectDescriptor( pEffect->getPresetId() );
        if( pDescriptor.get() )
        {
            PropertySubControl* pSubControl = NULL;

            Any aValue;

            UStringList aProperties( pDescriptor->getProperties() );
            if( aProperties.size() >= 1 )
            {
                mnPropertyType = getPropertyType( aProperties.front() );

                mpFTProperty->SetText( getPropertyName( mnPropertyType )  );

                aValue = getProperty1Value( mnPropertyType, pEffect );
            }

            if( aValue.hasValue() )
            {
                pSubControl = mpLBProperty->getSubControl();
                if( !pSubControl || (pSubControl->getControlType() != mnPropertyType) )
                {
                    pSubControl = PropertySubControl::create( mnPropertyType, this, aValue, pEffect->getPresetId(), LINK( this, CustomAnimationPane, implPropertyHdl ) );
                    mpLBProperty->setSubControl( pSubControl );
                }
                else
                {
                    pSubControl->setValue( aValue, pEffect->getPresetId() );
                }
            }
            else
            {
                mpLBProperty->setSubControl( 0 );
            }

            bool bEnable = (pSubControl != 0) && (pSubControl->getControl()->IsEnabled());
            mpLBProperty->Enable( bEnable );
            mpFTProperty->Enable( bEnable );
        }
        else
        {
            mpLBProperty->setSubControl( 0 );
            mpFTProperty->Enable( sal_False );
            mpLBProperty->Enable( sal_False );
            mpPBPropertyMore->Enable( sal_False );
        }

        sal_uInt16 nPos = 0xffff;

        sal_Int16 nNodeType = pEffect->getNodeType();
        switch( nNodeType )
        {
        case EffectNodeType::ON_CLICK:          nPos = 0; break;
        case EffectNodeType::WITH_PREVIOUS:     nPos = 1; break;
        case EffectNodeType::AFTER_PREVIOUS:    nPos = 2; break;
        }

        mpLBStart->SelectEntryPos( nPos );

        double fDuration = pEffect->getDuration();
        const bool bHasSpeed = fDuration > 0.001;

        mpFTSpeed->Enable(bHasSpeed);
        mpCBSpeed->Enable(bHasSpeed);

        if( bHasSpeed )
        {
            if( fDuration == 5.0 )
                nPos = 0;
            else if( fDuration == 3.0 )
                nPos = 1;
            else if( fDuration == 2.0 )
                nPos = 2;
            else if( fDuration == 1.0 )
                nPos = 3;
            else if( fDuration == 0.5 )
                nPos = 4;
            else
                nPos = 0xffff;

            mpCBSpeed->SelectEntryPos( nPos );
        }

        mpPBPropertyMore->Enable( sal_True );

        mpFTChangeOrder->Enable( sal_True );
    }
    else
    {
        mpLBProperty->setSubControl( 0 );
        mpFTProperty->Enable( sal_False );
        mpLBProperty->Enable( sal_False );
        mpPBPropertyMore->Enable( sal_False );
        mpFTSpeed->Enable(sal_False);
        mpCBSpeed->Enable(sal_False);
        mpFTChangeOrder->Enable( sal_False );
        mpLBStart->SetNoSelection();
        mpCBSpeed->SetNoSelection();
        mpFLEffect->SetText( maStrModify );
    }

    bool bEnableUp = true;
    bool bEnableDown = true;
    if( nSelectionCount == 0 )
    {
        bEnableUp = false;
        bEnableDown = false;
    }
    else
    {
        if( mpMainSequence->find( maListSelection.front() ) == mpMainSequence->getBegin() )
            bEnableUp = false;

        EffectSequence::iterator aIter( mpMainSequence->find( maListSelection.back() ) );
        if( aIter == mpMainSequence->getEnd() )
        {
            bEnableDown = false;
        }
        else
        {
            do
            {
                ++aIter;
            }
            while( (aIter != mpMainSequence->getEnd()) && !(mpCustomAnimationList->isExpanded((*aIter)) ) );

            if( aIter == mpMainSequence->getEnd() )
                bEnableDown = false;
        }

        if( bEnableUp || bEnableDown )
        {
            MainSequenceRebuildGuard aGuard( mpMainSequence );

            EffectSequenceHelper* pSequence = 0;
            EffectSequence::iterator aRebuildIter( maListSelection.begin() );
            const EffectSequence::iterator aRebuildEnd( maListSelection.end() );
            while( aRebuildIter != aRebuildEnd )
            {
                CustomAnimationEffectPtr pEffect = (*aRebuildIter++);

                if( pEffect.get() )
                {
                    if( pSequence == 0 )
                    {
                        pSequence = pEffect->getEffectSequence();
                    }
                    else
                    {
                        if( pSequence != pEffect->getEffectSequence() )
                        {
                            bEnableUp = false;
                            bEnableDown = false;
                            break;
                        }
                    }
                }
            }
        }
    }

    mpPBMoveUp->Enable(bEnableUp);
    mpPBMoveDown->Enable(bEnableDown);

    SdOptions* pOptions = SD_MOD()->GetSdOptions(DOCUMENT_TYPE_IMPRESS);
    mpCBAutoPreview->Check( pOptions->IsPreviewChangedEffects() == sal_True );

    updateMotionPathTags();
}

static bool updateMotionPathImpl( CustomAnimationPane& rPane, ::sd::View& rView,  EffectSequence::iterator aIter, EffectSequence::iterator aEnd, MotionPathTagVector& rOldTags, MotionPathTagVector& rNewTags )
{
    bool bChanges = false;
    while( aIter != aEnd )
    {
        CustomAnimationEffectPtr pEffect( (*aIter++) );
        if( pEffect.get() && pEffect->getPresetClass() == ::com::sun::star::presentation::EffectPresetClass::MOTIONPATH )
        {
            rtl::Reference< MotionPathTag > xMotionPathTag;
            // first try to find if there is already a tag for this
            MotionPathTagVector::iterator aMIter( rOldTags.begin() );
            for( ; aMIter != rOldTags.end(); ++aMIter )
            {
                rtl::Reference< MotionPathTag > xTag( (*aMIter) );
                if( xTag->getEffect() == pEffect )
                {
                    if( !xTag->isDisposed() )
                    {
                        xMotionPathTag = xTag;
                        rOldTags.erase( aMIter );
                    }
                    break;
                }
            }

            // if not found, create new one
            if( !xMotionPathTag.is() )
            {
                xMotionPathTag.set( new MotionPathTag( rPane, rView, pEffect ) );
                bChanges = true;
            }

            if( xMotionPathTag.is() )
                rNewTags.push_back( xMotionPathTag );
        }
    }

    return bChanges;
}

void CustomAnimationPane::updateMotionPathTags()
{
    bool bChanges = false;

    MotionPathTagVector aTags;
    aTags.swap( maMotionPathTags );

    ::sd::View* pView = 0;

    if( mxView.is() )
    {
        ::boost::shared_ptr<ViewShell> xViewShell( mrBase.GetMainViewShell() );
        if( xViewShell.get() )
            pView = xViewShell->GetView();
    }

    if( IsVisible() && mpMainSequence.get() && pView )
    {
        bChanges = updateMotionPathImpl( *this, *pView, mpMainSequence->getBegin(), mpMainSequence->getEnd(), aTags, maMotionPathTags );

        const InteractiveSequenceList& rISL = mpMainSequence->getInteractiveSequenceList();
        InteractiveSequenceList::const_iterator aISI( rISL.begin() );
        while( aISI != rISL.end() )
        {
            InteractiveSequencePtr pIS( (*aISI++) );
            bChanges |= updateMotionPathImpl( *this, *pView, pIS->getBegin(), pIS->getEnd(), aTags, maMotionPathTags );
        }
    }

    if( !aTags.empty() )
    {
        bChanges = true;
        MotionPathTagVector::iterator aIter( aTags.begin() );
        while( aIter != aTags.end() )
        {
            rtl::Reference< MotionPathTag > xTag( (*aIter++) );
            xTag->Dispose();
        }
    }

    if( bChanges && pView )
        pView->updateHandles();
}

void CustomAnimationPane::onSelectionChanged()
{
    if( !maSelectionLock.isLocked() )
    {
        ScopeLockGuard aGuard( maSelectionLock );

        if( mxView.is() ) try
        {
            Reference< XSelectionSupplier >  xSel( mxView, UNO_QUERY_THROW );
            if (xSel.is())
            {
                maViewSelection = xSel->getSelection();
                mpCustomAnimationList->onSelectionChanged( maViewSelection );
                updateControls();
            }
        }
        catch( Exception& )
        {
            OSL_FAIL( "sd::CustomAnimationPane::onSelectionChanged(), Exception caught!" );
        }
    }
}

void CustomAnimationPane::onDoubleClick()
{
    showOptions();
}

void CustomAnimationPane::onContextMenu( sal_uInt16 nSelectedPopupEntry )
{
    switch( nSelectedPopupEntry )
    {
    case CM_WITH_CLICK:     onChangeStart( EffectNodeType::ON_CLICK ); break;
    case CM_WITH_PREVIOUS:  onChangeStart( EffectNodeType::WITH_PREVIOUS  ); break;
    case CM_AFTER_PREVIOUS: onChangeStart( EffectNodeType::AFTER_PREVIOUS ); break;
    case CM_OPTIONS:        showOptions(); break;
    case CM_DURATION:       showOptions(RID_TP_CUSTOMANIMATION_DURATION); break;
    case CM_REMOVE:         onRemove(); break;
    case CM_CREATE:         if( maViewSelection.hasValue() ) onChange( true ); break;
    }

    updateControls();
}

void addValue( STLPropertySet* pSet, sal_Int32 nHandle, const Any& rValue )
{
    switch( pSet->getPropertyState( nHandle ) )
    {
    case STLPropertyState_AMBIGUOUS:
        // value is already ambiguous, do nothing
        break;
    case STLPropertyState_DIRECT:
        // set to ambiguous if existing value is different
        if( rValue != pSet->getPropertyValue( nHandle ) )
            pSet->setPropertyState( nHandle, STLPropertyState_AMBIGUOUS );
        break;
    case STLPropertyState_DEFAULT:
        // just set new value
        pSet->setPropertyValue( nHandle, rValue );
        break;
    }
}

static sal_Int32 calcMaxParaDepth( Reference< XShape > xTargetShape )
{
    sal_Int32 nMaxParaDepth = -1;

    if( xTargetShape.is() )
    {
        Reference< XEnumerationAccess > xText( xTargetShape, UNO_QUERY );
        if( xText.is() )
        {
            Reference< XPropertySet > xParaSet;
            const OUString strNumberingLevel( "NumberingLevel" );

            Reference< XEnumeration > xEnumeration( xText->createEnumeration(), UNO_QUERY_THROW );
            while( xEnumeration->hasMoreElements() )
            {
                xEnumeration->nextElement() >>= xParaSet;
                if( xParaSet.is() )
                {
                    sal_Int32 nParaDepth = 0;
                    xParaSet->getPropertyValue( strNumberingLevel ) >>= nParaDepth;

                    if( nParaDepth > nMaxParaDepth )
                        nMaxParaDepth = nParaDepth;
                }
            }
        }
    }

    return nMaxParaDepth + 1;
}

Any CustomAnimationPane::getProperty1Value( sal_Int32 nType, CustomAnimationEffectPtr pEffect )
{
    switch( nType )
    {
    case nPropertyTypeDirection:
    case nPropertyTypeSpokes:
    case nPropertyTypeZoom:
        return makeAny( pEffect->getPresetSubType() );

    case nPropertyTypeColor:
    case nPropertyTypeFillColor:
    case nPropertyTypeFirstColor:
    case nPropertyTypeSecondColor:
    case nPropertyTypeCharColor:
    case nPropertyTypeLineColor:
        {
            const sal_Int32 nIndex = (nPropertyTypeFirstColor == nType) ? 0 : 1;
            return pEffect->getColor( nIndex );
        }

    case nPropertyTypeFont:
        return pEffect->getProperty( AnimationNodeType::SET, "CharFontName" , VALUE_TO );

    case nPropertyTypeCharHeight:
        {
            const OUString aAttributeName( "CharHeight" );
            Any aValue( pEffect->getProperty( AnimationNodeType::SET, aAttributeName, VALUE_TO ) );
            if( !aValue.hasValue() )
                aValue = pEffect->getProperty( AnimationNodeType::ANIMATE, aAttributeName, VALUE_TO );
            return aValue;
        }

    case nPropertyTypeRotate:
        return pEffect->getTransformationProperty( AnimationTransformType::ROTATE, VALUE_BY);

    case nPropertyTypeTransparency:
        return pEffect->getProperty( AnimationNodeType::SET, "Opacity" , VALUE_TO );

    case nPropertyTypeScale:
        return pEffect->getTransformationProperty( AnimationTransformType::SCALE, VALUE_BY );

    case nPropertyTypeCharDecoration:
        {
            Sequence< Any > aValues(3);
            aValues[0] = pEffect->getProperty( AnimationNodeType::SET, "CharWeight" , VALUE_TO );
            aValues[1] = pEffect->getProperty( AnimationNodeType::SET, "CharPosture" , VALUE_TO );
            aValues[2] = pEffect->getProperty( AnimationNodeType::SET, "CharUnderline" , VALUE_TO );
            return makeAny( aValues );
        }
    }

    Any aAny;
    return aAny;
}

bool CustomAnimationPane::setProperty1Value( sal_Int32 nType, CustomAnimationEffectPtr pEffect, const Any& rValue )
{
    bool bEffectChanged = false;
    switch( nType )
    {
    case nPropertyTypeDirection:
    case nPropertyTypeSpokes:
    case nPropertyTypeZoom:
        {
            OUString aPresetSubType;
            rValue >>= aPresetSubType;
            if( aPresetSubType != pEffect->getPresetSubType() )
            {
                getPresets().changePresetSubType( pEffect, aPresetSubType );
                bEffectChanged = true;
            }
        }
        break;

    case nPropertyTypeFillColor:
    case nPropertyTypeColor:
    case nPropertyTypeFirstColor:
    case nPropertyTypeSecondColor:
    case nPropertyTypeCharColor:
    case nPropertyTypeLineColor:
        {
            const sal_Int32 nIndex = (nPropertyTypeFirstColor == nType) ? 0 : 1;
            Any aOldColor( pEffect->getColor( nIndex ) );
            if( aOldColor != rValue )
            {
                pEffect->setColor( nIndex, rValue );
                bEffectChanged = true;
            }
        }
        break;

    case nPropertyTypeFont:
        bEffectChanged = pEffect->setProperty( AnimationNodeType::SET, "CharFontName" , VALUE_TO, rValue );
        break;

    case nPropertyTypeCharHeight:
        {
            const OUString aAttributeName( "CharHeight" );
            bEffectChanged = pEffect->setProperty( AnimationNodeType::SET, aAttributeName, VALUE_TO, rValue );
            if( !bEffectChanged )
                bEffectChanged = pEffect->setProperty( AnimationNodeType::ANIMATE, aAttributeName, VALUE_TO, rValue );
        }
        break;
    case nPropertyTypeRotate:
        bEffectChanged = pEffect->setTransformationProperty( AnimationTransformType::ROTATE, VALUE_BY , rValue );
        break;

    case nPropertyTypeTransparency:
        bEffectChanged = pEffect->setProperty( AnimationNodeType::SET, "Opacity" , VALUE_TO, rValue );
        break;

    case nPropertyTypeScale:
        bEffectChanged = pEffect->setTransformationProperty( AnimationTransformType::SCALE, VALUE_BY, rValue );
        break;

    case nPropertyTypeCharDecoration:
        {
            Sequence< Any > aValues(3);
            rValue >>= aValues;
            bEffectChanged = pEffect->setProperty( AnimationNodeType::SET, "CharWeight" , VALUE_TO, aValues[0] );
            bEffectChanged |= pEffect->setProperty( AnimationNodeType::SET, "CharPosture" , VALUE_TO, aValues[1] );
            bEffectChanged |= pEffect->setProperty( AnimationNodeType::SET, "CharUnderline" , VALUE_TO, aValues[2] );
        }
        break;

    }

    return bEffectChanged;
}

static sal_Bool hasVisibleShape( const Reference< XShape >& xShape )
{
    try
    {
        const OUString sShapeType( xShape->getShapeType() );

        if( sShapeType == "com.sun.star.presentation.TitleTextShape" || sShapeType == "com.sun.star.presentation.OutlinerShape" ||
            sShapeType == "com.sun.star.presentation.SubtitleShape" || sShapeType == "com.sun.star.drawing.TextShape" )
        {
            const OUString sFillStyle( "FillStyle" );
            const OUString sLineStyle( "LineStyle" );
            Reference< XPropertySet > xSet( xShape, UNO_QUERY_THROW );

            FillStyle eFillStyle;
            xSet->getPropertyValue( sFillStyle ) >>= eFillStyle;

            ::com::sun::star::drawing::LineStyle eLineStyle;
            xSet->getPropertyValue( sLineStyle ) >>= eLineStyle;

            return eFillStyle != FillStyle_NONE || eLineStyle != ::com::sun::star::drawing::LineStyle_NONE;
        }
    }
    catch( Exception& )
    {
    }
    return sal_True;
}

STLPropertySet* CustomAnimationPane::createSelectionSet()
{
    STLPropertySet* pSet = CustomAnimationDialog::createDefaultSet();

    pSet->setPropertyValue( nHandleCurrentPage, makeAny( mxCurrentPage ) );

    sal_Int32 nMaxParaDepth = 0;

    // get options from selected effects
    EffectSequence::iterator aIter( maListSelection.begin() );
    const EffectSequence::iterator aEnd( maListSelection.end() );
    const CustomAnimationPresets& rPresets (getPresets());
    while( aIter != aEnd )
    {
        CustomAnimationEffectPtr pEffect = (*aIter++);

        EffectSequenceHelper* pEffectSequence = pEffect->getEffectSequence();
        if( !pEffectSequence )
            pEffectSequence = mpMainSequence.get();

        if( pEffect->hasText() )
        {
            sal_Int32 n = calcMaxParaDepth(pEffect->getTargetShape());
            if( n > nMaxParaDepth )
                nMaxParaDepth = n;
        }

        addValue( pSet, nHandleHasAfterEffect, makeAny( pEffect->hasAfterEffect() ) );
        addValue( pSet, nHandleAfterEffectOnNextEffect, makeAny( pEffect->IsAfterEffectOnNext() ? sal_True : sal_False ) );
        addValue( pSet, nHandleDimColor, pEffect->getDimColor() );
        addValue( pSet, nHandleIterateType, makeAny( pEffect->getIterateType() ) );

        // convert absolute time to percentage value
        // This calculation is done in float to avoid some rounding artifacts.
        float fIterateInterval = (float)pEffect->getIterateInterval();
        if( pEffect->getDuration() )
            fIterateInterval = (float)(fIterateInterval / pEffect->getDuration() );
        fIterateInterval *= 100.0;
        addValue( pSet, nHandleIterateInterval, makeAny( (double)fIterateInterval ) );

        addValue( pSet, nHandleBegin, makeAny( pEffect->getBegin() ) );
        addValue( pSet, nHandleDuration, makeAny( pEffect->getDuration() ) );
        addValue( pSet, nHandleStart, makeAny( pEffect->getNodeType() ) );
        addValue( pSet, nHandleRepeat, makeAny( pEffect->getRepeatCount() ) );
        addValue( pSet, nHandleEnd, pEffect->getEnd() );
        addValue( pSet, nHandleRewind, makeAny( pEffect->getFill() ) );

        addValue( pSet, nHandlePresetId, makeAny( pEffect->getPresetId() ) );

        addValue( pSet, nHandleHasText, makeAny( (sal_Bool)pEffect->hasText() ) );

        addValue( pSet, nHandleHasVisibleShape, Any( hasVisibleShape( pEffect->getTargetShape() ) ) );

        Any aSoundSource;
        if( pEffect->getAudio().is() )
        {
            aSoundSource = pEffect->getAudio()->getSource();
            addValue( pSet, nHandleSoundVolumne, makeAny( pEffect->getAudio()->getVolume() ) );
// todo     addValue( pSet, nHandleSoundEndAfterSlide, makeAny( pEffect->getAudio()->getEndAfterSlide() ) );
// this is now stored at the XCommand parameter sequence
        }
        else if( pEffect->getCommand() == EffectCommands::STOPAUDIO )
        {
            aSoundSource = makeAny( (sal_Bool)sal_True );
        }
        addValue( pSet, nHandleSoundURL, aSoundSource );

        sal_Int32 nGroupId = pEffect->getGroupId();
        CustomAnimationTextGroupPtr pTextGroup;
        if( nGroupId != -1 )
            pTextGroup = pEffectSequence->findGroup( nGroupId );

        addValue( pSet, nHandleTextGrouping, makeAny( pTextGroup.get() ? pTextGroup->getTextGrouping() : (sal_Int32)-1 ) );
        addValue( pSet, nHandleAnimateForm, makeAny( pTextGroup.get() ? (sal_Bool)pTextGroup->getAnimateForm() : sal_True ) );
        addValue( pSet, nHandleTextGroupingAuto, makeAny( pTextGroup.get() ? pTextGroup->getTextGroupingAuto() : (double)-1.0 ) );
        addValue( pSet, nHandleTextReverse, makeAny( pTextGroup.get() ? (sal_Bool)pTextGroup->getTextReverse() : sal_False ) );

        if( pEffectSequence->getSequenceType() == EffectNodeType::INTERACTIVE_SEQUENCE  )
        {
            InteractiveSequence* pIS = static_cast< InteractiveSequence* >( pEffectSequence );
            addValue( pSet, nHandleTrigger, makeAny( pIS->getTriggerShape() ) );
        }

        CustomAnimationPresetPtr pDescriptor = rPresets.getEffectDescriptor( pEffect->getPresetId() );
        if( pDescriptor.get() )
        {
            sal_Int32 nType = nPropertyTypeNone;

            UStringList aProperties( pDescriptor->getProperties() );
            if( aProperties.size() >= 1 )
                nType = getPropertyType( aProperties.front() );

            if( nType != nPropertyTypeNone )
            {
                addValue( pSet, nHandleProperty1Type, makeAny( nType ) );
                addValue( pSet, nHandleProperty1Value, getProperty1Value( nType, pEffect ) );
            }

            if( pDescriptor->hasProperty( "Accelerate" ) )
            {
                addValue( pSet, nHandleAccelerate, makeAny( pEffect->getAcceleration() ) );
            }

            if( pDescriptor->hasProperty( "Decelerate" ) )
            {
                addValue( pSet, nHandleDecelerate, makeAny( pEffect->getDecelerate() ) );
            }

            if( pDescriptor->hasProperty( "AutoReverse" ) )
            {
                addValue( pSet, nHandleAutoReverse, makeAny( pEffect->getAutoReverse() ) );
            }
        }
    }

    addValue( pSet, nHandleMaxParaDepth, makeAny( nMaxParaDepth ) );

    return pSet;
}

void CustomAnimationPane::changeSelection( STLPropertySet* pResultSet, STLPropertySet* pOldSet )
{
    // change selected effect
    bool bChanged = false;

    MainSequenceRebuildGuard aGuard( mpMainSequence );

    EffectSequence::iterator aIter( maListSelection.begin() );
    const EffectSequence::iterator aEnd( maListSelection.end() );
    while( aIter != aEnd )
    {
        CustomAnimationEffectPtr pEffect = (*aIter++);

        DBG_ASSERT( pEffect->getEffectSequence(), "sd::CustomAnimationPane::changeSelection(), dead effect in selection!" );
        if( !pEffect->getEffectSequence() )
            continue;

        double fDuration = 0.0; // we might need this for iterate-interval
        if( pResultSet->getPropertyState( nHandleDuration ) == STLPropertyState_DIRECT )
        {
            pResultSet->getPropertyValue( nHandleDuration ) >>= fDuration;
        }
        else
        {
            fDuration = pEffect->getDuration();
        }

        if( pResultSet->getPropertyState( nHandleIterateType ) == STLPropertyState_DIRECT )
        {
            sal_Int16 nIterateType = 0;
            pResultSet->getPropertyValue( nHandleIterateType ) >>= nIterateType;
            if( pEffect->getIterateType() != nIterateType )
            {
                pEffect->setIterateType( nIterateType );
                bChanged = true;
            }
        }

        if( pEffect->getIterateType() )
        {
            if( pResultSet->getPropertyState( nHandleIterateInterval ) == STLPropertyState_DIRECT )
            {
                double fIterateInterval = 0.0;
                pResultSet->getPropertyValue( nHandleIterateInterval ) >>= fIterateInterval;
                if( pEffect->getIterateInterval() != fIterateInterval )
                {
                    const double f = fIterateInterval * pEffect->getDuration() / 100;
                    pEffect->setIterateInterval( f );
                    bChanged = true;
                }
            }
        }

        if( pResultSet->getPropertyState( nHandleBegin ) == STLPropertyState_DIRECT )
        {
            double fBegin = 0.0;
            pResultSet->getPropertyValue( nHandleBegin ) >>= fBegin;
            if( pEffect->getBegin() != fBegin )
            {
                pEffect->setBegin( fBegin );
                bChanged = true;
            }
        }

        if( pResultSet->getPropertyState( nHandleDuration ) == STLPropertyState_DIRECT )
        {
            if( pEffect->getDuration() != fDuration )
            {
                pEffect->setDuration( fDuration );
                bChanged = true;
            }
        }

        if( pResultSet->getPropertyState( nHandleStart ) == STLPropertyState_DIRECT )
        {
            sal_Int16 nNodeType = 0;
            pResultSet->getPropertyValue( nHandleStart ) >>= nNodeType;
            if( pEffect->getNodeType() != nNodeType )
            {
                pEffect->setNodeType( nNodeType );
                bChanged = true;
            }
        }

        if( pResultSet->getPropertyState( nHandleRepeat ) == STLPropertyState_DIRECT )
        {
            Any aRepeatCount( pResultSet->getPropertyValue( nHandleRepeat ) );
            if( aRepeatCount != pEffect->getRepeatCount() )
            {
                pEffect->setRepeatCount( aRepeatCount );
                bChanged = true;
            }
        }

        if( pResultSet->getPropertyState( nHandleEnd ) == STLPropertyState_DIRECT )
        {
            Any aEndValue( pResultSet->getPropertyValue( nHandleEnd ) );
            if( pEffect->getEnd() != aEndValue )
            {
                pEffect->setEnd( aEndValue );
                bChanged = true;
            }
        }

        if( pResultSet->getPropertyState( nHandleRewind ) == STLPropertyState_DIRECT )
        {
            sal_Int16 nFill = 0;
            pResultSet->getPropertyValue( nHandleRewind ) >>= nFill;
            if( pEffect->getFill() != nFill )
            {
                pEffect->setFill( nFill );
                bChanged = true;
            }
        }

        if( pResultSet->getPropertyState( nHandleHasAfterEffect ) == STLPropertyState_DIRECT )
        {
            sal_Bool bHasAfterEffect = sal_False;
            if( pResultSet->getPropertyValue( nHandleHasAfterEffect )  >>= bHasAfterEffect )
            {
                if( pEffect->hasAfterEffect() != bHasAfterEffect )
                {
                    pEffect->setHasAfterEffect( bHasAfterEffect );
                    bChanged = true;
                }
            }
        }

        if( pResultSet->getPropertyState( nHandleAfterEffectOnNextEffect ) == STLPropertyState_DIRECT )
        {
            sal_Bool bAfterEffectOnNextEffect = sal_False;
            if( (pResultSet->getPropertyValue( nHandleAfterEffectOnNextEffect ) >>= bAfterEffectOnNextEffect) && ((pEffect->IsAfterEffectOnNext() ? sal_True : sal_False) != bAfterEffectOnNextEffect) )
            {
                pEffect->setAfterEffectOnNext( bAfterEffectOnNextEffect );
                bChanged = true;
            }
        }

        if( pResultSet->getPropertyState( nHandleDimColor ) == STLPropertyState_DIRECT )
        {
            Any aDimColor( pResultSet->getPropertyValue( nHandleDimColor ) );
            if( pEffect->getDimColor() != aDimColor )
            {
                pEffect->setDimColor( aDimColor );
                bChanged = true;
            }
        }

        if( pResultSet->getPropertyState( nHandleAccelerate ) == STLPropertyState_DIRECT )
        {
            double fAccelerate = 0.0;
            pResultSet->getPropertyValue( nHandleAccelerate ) >>= fAccelerate;
            if( pEffect->getAcceleration() != fAccelerate )
            {
                pEffect->setAcceleration( fAccelerate );
                bChanged = true;
            }
        }

        if( pResultSet->getPropertyState( nHandleDecelerate ) == STLPropertyState_DIRECT )
        {
            double fDecelerate = 0.0;
            pResultSet->getPropertyValue( nHandleDecelerate ) >>= fDecelerate;
            if( pEffect->getDecelerate() != fDecelerate )
            {
                pEffect->setDecelerate( fDecelerate );
                bChanged = true;
            }
        }

        if( pResultSet->getPropertyState( nHandleAutoReverse ) == STLPropertyState_DIRECT )
        {
            sal_Bool bAutoReverse = sal_False;
            pResultSet->getPropertyValue( nHandleAutoReverse ) >>= bAutoReverse;
            if( pEffect->getAutoReverse() != bAutoReverse )
            {
                pEffect->setAutoReverse( bAutoReverse );
                bChanged = true;
            }
        }

        if( pResultSet->getPropertyState( nHandleProperty1Value ) == STLPropertyState_DIRECT )
        {
            sal_Int32 nType = 0;
            pOldSet->getPropertyValue( nHandleProperty1Type ) >>= nType;

            bChanged |= setProperty1Value( nType, pEffect, pResultSet->getPropertyValue( nHandleProperty1Value ) );
        }

        if( pResultSet->getPropertyState( nHandleSoundURL ) == STLPropertyState_DIRECT )
        {
            const Any aSoundSource( pResultSet->getPropertyValue( nHandleSoundURL ) );

            if( aSoundSource.getValueType() == ::getCppuType((const sal_Bool*)0) )
            {
                pEffect->setStopAudio();
                bChanged = true;
            }
            else
            {
                OUString aSoundURL;
                aSoundSource >>= aSoundURL;

                if( !aSoundURL.isEmpty() )
                {
                    if( !pEffect->getAudio().is() )
                    {
                        pEffect->createAudio( aSoundSource );
                        bChanged = true;
                    }
                    else
                    {
                        if( pEffect->getAudio()->getSource() != aSoundSource )
                        {
                            pEffect->getAudio()->setSource( aSoundSource );
                            bChanged = true;
                        }
                    }
                }
                else
                {
                    if( pEffect->getAudio().is() || pEffect->getStopAudio() )
                    {
                        pEffect->removeAudio();
                        bChanged = true;
                    }
                }
            }
        }

        if( pResultSet->getPropertyState( nHandleTrigger ) == STLPropertyState_DIRECT )
        {
            Reference< XShape > xTriggerShape;
            pResultSet->getPropertyValue( nHandleTrigger ) >>= xTriggerShape;
            bChanged |= mpMainSequence->setTrigger( pEffect, xTriggerShape );
        }
    }

    const bool bHasTextGrouping = pResultSet->getPropertyState( nHandleTextGrouping ) == STLPropertyState_DIRECT;
    const bool bHasAnimateForm = pResultSet->getPropertyState( nHandleAnimateForm ) == STLPropertyState_DIRECT;
    const bool bHasTextGroupingAuto = pResultSet->getPropertyState( nHandleTextGroupingAuto ) == STLPropertyState_DIRECT;
    const bool bHasTextReverse = pResultSet->getPropertyState( nHandleTextReverse ) == STLPropertyState_DIRECT;

    if( bHasTextGrouping || bHasAnimateForm || bHasTextGroupingAuto || bHasTextReverse )
    {
        // we need to do a second pass for text grouping options
        // since changing them can cause effects to be removed
        // or replaced, we do this after we aplied all other options
        // above

        sal_Int32 nTextGrouping = 0;
        sal_Bool bAnimateForm = sal_True, bTextReverse = sal_False;
        double fTextGroupingAuto = -1.0;

        if( bHasTextGrouping )
            pResultSet->getPropertyValue(nHandleTextGrouping) >>= nTextGrouping;

        if( bHasAnimateForm )
            pResultSet->getPropertyValue(nHandleAnimateForm) >>= bAnimateForm;

        if( bHasTextGroupingAuto )
            pResultSet->getPropertyValue(nHandleTextGroupingAuto) >>= fTextGroupingAuto;

        if( bHasTextReverse )
            pResultSet->getPropertyValue(nHandleTextReverse) >>= bTextReverse;

        EffectSequence const aSelectedEffects( maListSelection );
        EffectSequence::const_iterator iter( aSelectedEffects.begin() );
        const EffectSequence::const_iterator iEnd( aSelectedEffects.end() );
        while( iter != iEnd )
        {
            CustomAnimationEffectPtr const& pEffect = (*iter++);

            EffectSequenceHelper* pEffectSequence = pEffect->getEffectSequence();
            if( !pEffectSequence )
                pEffectSequence = mpMainSequence.get();

            sal_Int32 nGroupId = pEffect->getGroupId();
            CustomAnimationTextGroupPtr pTextGroup;
            if( (nGroupId != -1) )
            {
                // use existing group
                pTextGroup = pEffectSequence->findGroup( nGroupId );
            }
            else
            {
                // somethings changed so we need a group now
                pTextGroup = pEffectSequence->createTextGroup( pEffect, nTextGrouping, fTextGroupingAuto, bAnimateForm, bTextReverse );
                bChanged = true;
            }

            if( bHasTextGrouping )
            {
                if( (pTextGroup->getTextGrouping() != nTextGrouping) )
                {
                    pEffectSequence->setTextGrouping( pTextGroup, nTextGrouping );
                    bChanged = true;
                }
            }

            if( bHasAnimateForm )
            {
                if( pTextGroup->getAnimateForm() != bAnimateForm )
                {
                    pEffectSequence->setAnimateForm( pTextGroup, bAnimateForm );
                    bChanged = true;
                }
            }

            if( bHasTextGroupingAuto )
            {
                if( pTextGroup->getTextGroupingAuto() != fTextGroupingAuto )
                {
                    pEffectSequence->setTextGroupingAuto( pTextGroup, fTextGroupingAuto );
                    bChanged = true;
                }
            }

            if( bHasTextReverse )
            {
                if( pTextGroup->getTextReverse() != bTextReverse )
                {
                    pEffectSequence->setTextReverse( pTextGroup, bTextReverse );
                    bChanged = true;
                }
            }
        }
    }

    if( bChanged )
    {
        mpMainSequence->rebuild();
        updateControls();
        mrBase.GetDocShell()->SetModified();
    }
}

void CustomAnimationPane::showOptions( sal_uInt16 nPage /* = 0 */ )
{
    STLPropertySet* pSet = createSelectionSet();

    CustomAnimationDialog* pDlg = new CustomAnimationDialog( this, pSet, nPage );
    if( pDlg->Execute() )
    {
        addUndo();
        changeSelection( pDlg->getResultSet(), pSet );
        updateControls();
    }

    delete pDlg;
}

void CustomAnimationPane::onChangeCurrentPage()
{
    if( mxView.is() ) try
    {
        Reference< XDrawPage > xNewPage( mxView->getCurrentPage() );
        if( xNewPage != mxCurrentPage )
        {
            mxCurrentPage = xNewPage;
            SdPage* pPage = SdPage::getImplementation( mxCurrentPage );
            if( pPage )
            {
                mpMainSequence = pPage->getMainSequence();
                mpCustomAnimationList->update( mpMainSequence );
            }
            updateControls();
        }
    }
    catch( Exception& )
    {
        OSL_FAIL( "sd::CustomAnimationPane::onChangeCurrentPage(), exception caught!" );
    }
}

bool getTextSelection( const Any& rSelection, Reference< XShape >& xShape, std::list< sal_Int16 >& rParaList )
{
    Reference< XTextRange > xSelectedText;
    rSelection >>= xSelectedText;
    if( xSelectedText.is() ) try
    {
        xShape.set( xSelectedText->getText(), UNO_QUERY_THROW );

        Reference< XTextRangeCompare > xTextRangeCompare( xShape, UNO_QUERY_THROW );
        Reference< XEnumerationAccess > xParaEnumAccess( xShape, UNO_QUERY_THROW );
        Reference< XEnumeration > xParaEnum( xParaEnumAccess->createEnumeration(), UNO_QUERY_THROW );
        Reference< XTextRange > xRange;
        Reference< XTextRange > xStart( xSelectedText->getStart() );
        Reference< XTextRange > xEnd( xSelectedText->getEnd() );

        if( xTextRangeCompare->compareRegionEnds( xStart, xEnd ) < 0 )
        {
            Reference< XTextRange > xTemp( xStart );
            xStart = xEnd;
            xEnd = xTemp;
        }

        sal_Int16 nPara = 0;
        while( xParaEnum->hasMoreElements() )
        {
            xParaEnum->nextElement() >>= xRange;

            // break if start of selection is prior to end of current paragraph
            if( xRange.is() && (xTextRangeCompare->compareRegionEnds( xStart, xRange ) >= 0 ) )
                break;

            nPara++;
        }

        while( xRange.is() )
        {
            if( xRange.is() && !xRange->getString().isEmpty() )
                rParaList.push_back( nPara );

            // break if end of selection is before or at end of current paragraph
            if( xRange.is() && xTextRangeCompare->compareRegionEnds( xEnd, xRange ) >= 0 )
                break;

            nPara++;

            if( xParaEnum->hasMoreElements() )
                xParaEnum->nextElement() >>= xRange;
            else
                xRange.clear();
        }

        return true;
    }
    catch( Exception& )
    {
        OSL_FAIL( "sd::CustomAnimationPane::getTextSelection(), exception caught!" );
    }

    return false;
}

void CustomAnimationPane::onChange( bool bCreate )
{
    bool bHasText = true;

    // first create vector of targets for dialog preview
    std::vector< Any > aTargets;
    OUString sPresetId;
    double fDuration = 2.0f;

    if( bCreate )
    {
        // gather shapes from the selection
        Reference< XSelectionSupplier >  xSel( mxView, UNO_QUERY_THROW );
        maViewSelection = xSel->getSelection();

        if( maViewSelection.getValueType() == ::getCppuType((const Reference< XShapes >*)0) )
        {
            Reference< XIndexAccess > xShapes;
            maViewSelection >>= xShapes;

            sal_Int32 nCount = xShapes->getCount();
            sal_Int32 nIndex;
            for( nIndex = 0; nIndex < nCount; nIndex++ )
            {
                Any aTarget( xShapes->getByIndex( nIndex ) );
                aTargets.push_back( aTarget );
                if( bHasText )
                {
                    Reference< XText > xText;
                    aTarget >>= xText;
                    if( !xText.is() || xText->getString().isEmpty() )
                        bHasText = false;
                }
            }
        }
        else if ( maViewSelection.getValueType() == ::getCppuType((const Reference< XShape >*)0) )
        {
            aTargets.push_back( maViewSelection );
            Reference< XText > xText;
            maViewSelection >>= xText;
            if( !xText.is() || xText->getString().isEmpty() )
                bHasText = false;
        }
        else if ( maViewSelection.getValueType() == ::getCppuType((const Reference< XTextCursor >*)0) )
        {
            Reference< XShape > xShape;
            std::list< sal_Int16 > aParaList;
            if( getTextSelection( maViewSelection, xShape, aParaList ) )
            {
                ParagraphTarget aParaTarget;
                aParaTarget.Shape = xShape;

                std::list< sal_Int16 >::iterator aIter( aParaList.begin() );
                for( ; aIter != aParaList.end(); ++aIter )
                {
                    aParaTarget.Paragraph = (*aIter);
                    aTargets.push_back( makeAny( aParaTarget ) );
                   }
            }
        }
        else
        {
            OSL_FAIL("sd::CustomAnimationPane::onChange(), unknown view selection!" );
            return;
        }
    }
    else
    {
        // get selected effect
        EffectSequence::iterator aIter( maListSelection.begin() );
        const EffectSequence::iterator aEnd( maListSelection.end() );
        while( aIter != aEnd )
        {
            if( !bHasText || !(*aIter)->hasText() )
                bHasText = false;

            if( sPresetId.isEmpty() )
            {
                sPresetId = (*aIter)->getPresetId();
                fDuration = (*aIter)->getDuration();
            }

            aTargets.push_back( (*aIter++)->getTarget() );
        }
    }

    CustomAnimationCreateDialog* pDlg = new CustomAnimationCreateDialog( this, this, aTargets, bHasText, sPresetId, fDuration );
    if( pDlg->Execute() )
    {
        addUndo();
        fDuration = pDlg->getSelectedDuration();
        CustomAnimationPresetPtr pDescriptor = pDlg->getSelectedPreset();
        if( pDescriptor.get() )
        {
            if( bCreate )
            {
                mpCustomAnimationList->SelectAll( sal_False );

                // gather shapes from the selection
                std::vector< Any >::iterator aIter( aTargets.begin() );
                const std::vector< Any >::iterator aEnd( aTargets.end() );
                bool bFirst = true;
                for( ; aIter != aEnd; ++aIter )
                {
                    CustomAnimationEffectPtr pCreated = mpMainSequence->append( pDescriptor, (*aIter), fDuration );

                    // if only one shape with text and no fill or outline is selected, animate only by first level paragraphs
                    if( bHasText && (aTargets.size() == 1) )
                    {
                        Reference< XShape > xShape( (*aIter), UNO_QUERY );
                        if( xShape.is() && !hasVisibleShape( xShape ) )
                        {
                            mpMainSequence->createTextGroup( pCreated, 1, -1.0, sal_False, sal_False );
                        }
                    }

                    if( bFirst )
                        bFirst = false;
                    else
                        pCreated->setNodeType( EffectNodeType::WITH_PREVIOUS );

                    if( pCreated.get() )
                    {
                        mpCustomAnimationList->select( pCreated );
                    }
                }
            }
            else
            {
                MainSequenceRebuildGuard aGuard( mpMainSequence );

                // get selected effect
                EffectSequence::iterator aIter( maListSelection.begin() );
                const EffectSequence::iterator aEnd( maListSelection.end() );
                while( aIter != aEnd )
                {
                    CustomAnimationEffectPtr pEffect = (*aIter++);

                    EffectSequenceHelper* pEffectSequence = pEffect->getEffectSequence();
                    if( !pEffectSequence )
                        pEffectSequence = mpMainSequence.get();

                    pEffectSequence->replace( pEffect, pDescriptor, fDuration );
                }
            }
        }
        else
        {
            PathKind eKind = pDlg->getCreatePathKind();
            if( eKind != NONE )
                createPath( eKind, aTargets, fDuration );
        }
        mrBase.GetDocShell()->SetModified();
    }

    delete pDlg;

    updateControls();

    // stop running preview from dialog
    SlideShow::Stop( mrBase );
}

void CustomAnimationPane::createPath( PathKind eKind, std::vector< Any >& rTargets, double fDuration)
{
    sal_uInt16 nSID = 0;

    switch( eKind )
    {
    case CURVE:     nSID = SID_DRAW_BEZIER_NOFILL; break;
    case POLYGON:   nSID = SID_DRAW_POLYGON_NOFILL; break;
    case FREEFORM:  nSID = SID_DRAW_FREELINE_NOFILL; break;
    default: break;
    }

    if( nSID )
    {
        DrawViewShell* pViewShell = dynamic_cast< DrawViewShell* >(
            FrameworkHelper::Instance(mrBase)->GetViewShell(FrameworkHelper::msCenterPaneURL).get());

        if( pViewShell )
        {
            DrawView* pView = pViewShell->GetDrawView();
            if( pView )
                pView->UnmarkAllObj();

            std::vector< Any > aTargets( 1, Any( fDuration ) );
            aTargets.insert( aTargets.end(), rTargets.begin(), rTargets.end() );
            Sequence< Any > aTargetSequence( comphelper::containerToSequence( aTargets ) );
            const SfxUnoAnyItem aItem( SID_ADD_MOTION_PATH, Any( aTargetSequence ) );
            pViewShell->GetViewFrame()->GetDispatcher()->Execute( nSID, SFX_CALLMODE_ASYNCHRON, &aItem, 0 );
        }
    }
}

void CustomAnimationPane::onRemove()
{
   if( !maListSelection.empty() )
    {
        addUndo();

        MainSequenceRebuildGuard aGuard( mpMainSequence );

        EffectSequence aList( maListSelection );

        EffectSequence::iterator aIter( aList.begin() );
        const EffectSequence::iterator aEnd( aList.end() );
        while( aIter != aEnd )
        {
            CustomAnimationEffectPtr pEffect = (*aIter++);
            if( pEffect->getEffectSequence() )
                pEffect->getEffectSequence()->remove( pEffect );
        }

        maListSelection.clear();
        mrBase.GetDocShell()->SetModified();
    }
}

void CustomAnimationPane::remove( CustomAnimationEffectPtr& pEffect )
{
    if( pEffect->getEffectSequence() )
    {
        addUndo();
        pEffect->getEffectSequence()->remove( pEffect );
        mrBase.GetDocShell()->SetModified();
    }
}

void CustomAnimationPane::onChangeStart()
{
    if( mpLBStart->GetSelectEntryCount() == 1 )
    {
        sal_Int16 nNodeType;
        sal_uInt16 nPos= mpLBStart->GetSelectEntryPos();
        switch( nPos )
        {
        case 0: nNodeType = EffectNodeType::ON_CLICK; break;
        case 1: nNodeType = EffectNodeType::WITH_PREVIOUS; break;
        case 2: nNodeType = EffectNodeType::AFTER_PREVIOUS; break;
        default:
            return;
        }

        onChangeStart( nNodeType );
    }
}

void CustomAnimationPane::onChangeStart( sal_Int16 nNodeType )
{
    addUndo();

    MainSequenceRebuildGuard aGuard( mpMainSequence );

    bool bNeedRebuild = false;

    EffectSequence::iterator aIter( maListSelection.begin() );
    const EffectSequence::iterator aEnd( maListSelection.end() );
    while( aIter != aEnd )
    {
        CustomAnimationEffectPtr pEffect = (*aIter++);
        if( pEffect->getNodeType() != nNodeType )
        {
            pEffect->setNodeType( nNodeType );
            bNeedRebuild = true;
        }
    }

    if( bNeedRebuild )
    {
        mpMainSequence->rebuild();
        updateControls();
        mrBase.GetDocShell()->SetModified();
    }
}

void CustomAnimationPane::onChangeProperty()
{
    if( mpLBProperty->getSubControl() )
    {
        addUndo();

        MainSequenceRebuildGuard aGuard( mpMainSequence );

        const Any aValue( mpLBProperty->getSubControl()->getValue() );

        bool bNeedUpdate = false;

        // change selected effect
        EffectSequence::iterator aIter( maListSelection.begin() );
        const EffectSequence::iterator aEnd( maListSelection.end() );
        while( aIter != aEnd )
        {
            CustomAnimationEffectPtr pEffect = (*aIter++);

            if( setProperty1Value( mnPropertyType, pEffect, aValue ) )
                bNeedUpdate = true;
        }

        if( bNeedUpdate )
        {
            mpMainSequence->rebuild();
            updateControls();
            mrBase.GetDocShell()->SetModified();
        }

        onPreview( false );
    }
}

void CustomAnimationPane::onChangeSpeed()
{
    if( mpCBSpeed->GetSelectEntryCount() == 1 )
    {
        addUndo();

        MainSequenceRebuildGuard aGuard( mpMainSequence );

        double fDuration;

        sal_uInt16 nPos= mpCBSpeed->GetSelectEntryPos();

        switch( nPos )
        {
        case 0: fDuration = 5.0; break;
        case 1: fDuration = 3.0; break;
        case 2: fDuration = 2.0; break;
        case 3: fDuration = 1.0; break;
        case 4: fDuration = 0.5; break;
        default:
            return;
        }

        // change selected effect
        EffectSequence::iterator aIter( maListSelection.begin() );
        const EffectSequence::iterator aEnd( maListSelection.end() );
        while( aIter != aEnd )
        {
            CustomAnimationEffectPtr pEffect = (*aIter++);
            pEffect->setDuration( fDuration );
        }

        mpMainSequence->rebuild();
        updateControls();
        mrBase.GetDocShell()->SetModified();

        onPreview( false );
    }
}

/// this link is called when the property box is modified by the user
IMPL_LINK_NOARG(CustomAnimationPane, implPropertyHdl)
{
    onChangeProperty();
    return 0;
}

/// this link is called when one of the controls is modified
IMPL_LINK( CustomAnimationPane, implControlHdl, Control*, pControl )
{
    if( pControl == mpPBAddEffect )
        onChange(true);
    else if( pControl == mpPBChangeEffect )
        onChange(false);
    else if( pControl == mpPBRemoveEffect )
        onRemove();
    else if( pControl == mpLBStart )
        onChangeStart();
    else if( pControl == mpCBSpeed )
        onChangeSpeed();
    else if( pControl == mpPBPropertyMore )
        showOptions();
    else if( pControl == mpPBMoveUp )
        moveSelection( true );
    else if( pControl == mpPBMoveDown )
        moveSelection( false );
    else if( pControl == mpPBPlay )
        onPreview( true );
    else if( pControl == mpPBSlideShow )
    {
        mrBase.StartPresentation();
    }
    else if( pControl == mpCBAutoPreview )
    {
        SdOptions* pOptions = SD_MOD()->GetSdOptions(DOCUMENT_TYPE_IMPRESS);
        pOptions->SetPreviewChangedEffects( mpCBAutoPreview->IsChecked() ? sal_True : sal_False );
    }

    updateControls();

    return 0;
}

IMPL_LINK_NOARG(CustomAnimationPane, lateInitCallback)
{
    // Call getPresets() to initiate the (expensive) construction of the
    // presets list.
    getPresets();

    // update selection and control states
    onSelectionChanged();

    return 0;
}

void CustomAnimationPane::moveSelection( bool bUp )
{
    if( maListSelection.empty() )
        return;

    EffectSequenceHelper* pSequence = maListSelection.front()->getEffectSequence();
    if( pSequence == 0 )
        return;

    addUndo();

    bool bChanged = false;

    MainSequenceRebuildGuard aGuard( mpMainSequence );
    EffectSequence& rEffectSequence = pSequence->getSequence();

    if( bUp )
    {
        EffectSequence::iterator aIter( maListSelection.begin() );
        const EffectSequence::iterator aEnd( maListSelection.end() );

        while( aIter != aEnd )
        {
            CustomAnimationEffectPtr pEffect = (*aIter++);

            EffectSequence::iterator aEffectPos( pSequence->find( pEffect ) );
            if( aEffectPos != rEffectSequence.end() )
            {
                EffectSequence::iterator aInsertPos( rEffectSequence.erase( aEffectPos ) );

                if( aInsertPos != rEffectSequence.begin() )
                {
                    --aInsertPos;
                    while( (aInsertPos != rEffectSequence.begin()) && !mpCustomAnimationList->isExpanded(*aInsertPos))
                        --aInsertPos;

                    rEffectSequence.insert( aInsertPos, pEffect );
                }
                else
                {
                    rEffectSequence.push_front( pEffect );
                }
                bChanged = true;
            }
        }
    }
    else
    {
        EffectSequence::reverse_iterator aIter( maListSelection.rbegin() );
        const EffectSequence::reverse_iterator aEnd( maListSelection.rend() );

        while( aIter != aEnd )
        {
            CustomAnimationEffectPtr pEffect = (*aIter++);

            EffectSequence::iterator aEffectPos( pSequence->find( pEffect ) );
            if( aEffectPos != rEffectSequence.end() )
            {
                EffectSequence::iterator aInsertPos( rEffectSequence.erase( aEffectPos ) );

                if( aInsertPos != rEffectSequence.end() )
                {
                    ++aInsertPos;
                    while( (aInsertPos != rEffectSequence.end()) && !mpCustomAnimationList->isExpanded(*aInsertPos))
                        ++aInsertPos;

                    rEffectSequence.insert( aInsertPos, pEffect );
                }
                else
                {
                    rEffectSequence.push_back( pEffect );
                }
                bChanged = true;
            }
        }
    }

    if( bChanged )
    {
        mpMainSequence->rebuild();
        updateControls();
        mrBase.GetDocShell()->SetModified();
    }
}

void CustomAnimationPane::onPreview( bool bForcePreview )
{
    if( !bForcePreview && !mpCBAutoPreview->IsChecked() )
        return;

    if( maListSelection.empty() )
    {
        rtl::Reference< MotionPathTag > xMotionPathTag;
        MotionPathTagVector::iterator aIter;
        for( aIter = maMotionPathTags.begin(); aIter != maMotionPathTags.end(); ++aIter )
        {
            if( (*aIter)->isSelected() )
            {
                xMotionPathTag = (*aIter);
                break;
            }
        }

        if( xMotionPathTag.is() )
        {
            MainSequencePtr pSequence( new MainSequence() );
            pSequence->append( xMotionPathTag->getEffect()->clone() );
            preview( pSequence->getRootNode() );
        }
        else
        {
            Reference< XAnimationNodeSupplier > xNodeSupplier( mxCurrentPage, UNO_QUERY );
            if( !xNodeSupplier.is() )
                return;

            preview( xNodeSupplier->getAnimationNode() );
        }
    }
    else
    {
        MainSequencePtr pSequence( new MainSequence() );

        EffectSequence::iterator aIter( maListSelection.begin() );
        const EffectSequence::iterator aEnd( maListSelection.end() );

        while( aIter != aEnd )
        {
            CustomAnimationEffectPtr pEffect = (*aIter++);
            pSequence->append( pEffect->clone() );
        }

        preview( pSequence->getRootNode() );
    }
}

void CustomAnimationPane::preview( const Reference< XAnimationNode >& xAnimationNode )
{
    Reference< XTimeContainer > xRoot(::comphelper::getProcessServiceFactory()->createInstance("com.sun.star.animations.ParallelTimeContainer"), UNO_QUERY);
    if( xRoot.is() )
    {
        Sequence< ::com::sun::star::beans::NamedValue > aUserData( 1 );
        aUserData[0].Name = "node-type";
        aUserData[0].Value <<= ::com::sun::star::presentation::EffectNodeType::TIMING_ROOT;
        xRoot->setUserData( aUserData );
        xRoot->appendChild( xAnimationNode );

        Reference< XAnimationNode > xNode( xRoot, UNO_QUERY );
        SlideShow::StartPreview( mrBase, mxCurrentPage, xNode );
    }
}


// ICustomAnimationListController
void CustomAnimationPane::onSelect()
{
    maListSelection = mpCustomAnimationList->getSelection();
    updateControls();
    markShapesFromSelectedEffects();
}




const CustomAnimationPresets& CustomAnimationPane::getPresets (void)
{
    if (mpCustomAnimationPresets == NULL)
        mpCustomAnimationPresets = &CustomAnimationPresets::getCustomAnimationPresets();
    return *mpCustomAnimationPresets;
}



void CustomAnimationPane::markShapesFromSelectedEffects()
{
    if( !maSelectionLock.isLocked() )
    {
        ScopeLockGuard aGuard( maSelectionLock );
        DrawViewShell* pViewShell = dynamic_cast< DrawViewShell* >(
            FrameworkHelper::Instance(mrBase)->GetViewShell(FrameworkHelper::msCenterPaneURL).get());
        DrawView* pView = pViewShell ? pViewShell->GetDrawView() : NULL;

        if( pView )
        {
            pView->UnmarkAllObj();
            EffectSequence::iterator aIter( maListSelection.begin() );
            const EffectSequence::iterator aEnd( maListSelection.end() );
            while( aIter != aEnd )
            {
                CustomAnimationEffectPtr pEffect = (*aIter++);

                Reference< XShape > xShape( pEffect->getTargetShape() );
                SdrObject* pObj = GetSdrObjectFromXShape( xShape );
                if( pObj )
                    pView->MarkObj(pObj, pView->GetSdrPageView(), sal_False, sal_False);
            }
        }
    }
}


void CustomAnimationPane::updatePathFromMotionPathTag( const rtl::Reference< MotionPathTag >& xTag )
{
    MainSequenceRebuildGuard aGuard( mpMainSequence );
    if( xTag.is() )
    {
        SdrPathObj* pPathObj = xTag->getPathObj();
        CustomAnimationEffectPtr pEffect = xTag->getEffect();
        if( (pPathObj != 0) && pEffect.get() != 0 )
        {
            ::svl::IUndoManager* pManager = mrBase.GetDocShell()->GetUndoManager();
            if( pManager )
            {
                SdPage* pPage = SdPage::getImplementation( mxCurrentPage );
                if( pPage )
                    pManager->AddUndoAction( new UndoAnimationPath( mrBase.GetDocShell()->GetDoc(), pPage, pEffect->getNode() ) );
            }

            pEffect->updatePathFromSdrPathObj( *pPathObj );
        }
    }
}

// ====================================================================

::Window * createCustomAnimationPanel( ::Window* pParent, ViewShellBase& rBase )
{
    DialogListBox* pWindow = 0;

    DrawDocShell* pDocSh = rBase.GetDocShell();
    if( pDocSh )
    {
        pWindow = new DialogListBox( pParent, WB_CLIPCHILDREN|WB_TABSTOP|WB_AUTOHSCROLL );

        Size aMinSize( pWindow->LogicToPixel( Size( 80, 256 ), MAP_APPFONT ) );
        ::Window* pPaneWindow = new CustomAnimationPane( pWindow, rBase, aMinSize );
        pWindow->SetChildWindow( pPaneWindow, aMinSize );
        pWindow->SetText( pPaneWindow->GetText() );
    }

    return pWindow;
}



}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
