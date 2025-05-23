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


#include <boost/current_function.hpp>
#include <rtl/ustrbuf.hxx>
#include <vcl/svapp.hxx>
#include <vcl/gdimtf.hxx>
#include <vcl/virdev.hxx>
#include <vcl/metric.hxx>
#include <cppcanvas/vclfactory.hxx>
#include <cppcanvas/basegfxfactory.hxx>
#include <basegfx/range/b2drange.hxx>

#include <comphelper/anytostring.hxx>
#include <cppuhelper/exc_hlp.hxx>

#include <com/sun/star/awt/MouseButton.hpp>
#include <com/sun/star/awt/MouseEvent.hpp>
#include <com/sun/star/rendering/XBitmap.hpp>

#include "eventqueue.hxx"
#include "screenupdater.hxx"
#include "eventmultiplexer.hxx"
#include "activitiesqueue.hxx"
#include "slideshowcontext.hxx"
#include "mouseeventhandler.hxx"
#include "rehearsetimingsactivity.hxx"

#include <boost/bind.hpp>
#include <o3tl/compat_functional.hxx>
#include <algorithm>

using namespace com::sun::star;
using namespace com::sun::star::uno;

namespace slideshow {
namespace internal {

class RehearseTimingsActivity::WakeupEvent : public Event,
                                             private ::boost::noncopyable
{
public:
    WakeupEvent( boost::shared_ptr< ::canvas::tools::ElapsedTime > const& pTimeBase,
                 ActivitySharedPtr const&                                 rActivity,
                 ActivitiesQueue &                                        rActivityQueue ) :
        Event("WakeupEvent"),
        maTimer(pTimeBase),
        mnNextTime(0.0),
        mpActivity(rActivity),
        mrActivityQueue( rActivityQueue )
    {}

    virtual void dispose() {}
    virtual bool fire()
    {
        ActivitySharedPtr pActivity( mpActivity.lock() );
        if( !pActivity )
            return false;

        return mrActivityQueue.addActivity( pActivity );
    }

    virtual bool isCharged() const { return true; }
    virtual double getActivationTime( double nCurrentTime ) const
    {
        const double nElapsedTime( maTimer.getElapsedTime() );

        return ::std::max( nCurrentTime,
                           nCurrentTime - nElapsedTime + mnNextTime );
    }

    /// Start the internal timer
    void start() { maTimer.reset(); }

    /** Set the next timeout this object should generate.

        @param nextTime
        Absolute time, measured from the last start() call,
        when this event should wakeup the Activity again. If
        your time is relative, simply call start() just before
        every setNextTimeout() call.
    */
    void setNextTimeout( double nextTime ) { mnNextTime = nextTime; }

private:
    ::canvas::tools::ElapsedTime    maTimer;
    double                          mnNextTime;
    boost::weak_ptr<Activity>       mpActivity;
    ActivitiesQueue&                mrActivityQueue;
};

class RehearseTimingsActivity::MouseHandler : public MouseEventHandler,
                                              private boost::noncopyable
{
public:
    explicit MouseHandler( RehearseTimingsActivity& rta );

    void reset();
    bool hasBeenClicked() const { return mbHasBeenClicked; }

    // MouseEventHandler
    virtual bool handleMousePressed( awt::MouseEvent const & evt );
    virtual bool handleMouseReleased( awt::MouseEvent const & evt );
    virtual bool handleMouseEntered( awt::MouseEvent const & evt );
    virtual bool handleMouseExited( awt::MouseEvent const & evt );
    virtual bool handleMouseDragged( awt::MouseEvent const & evt );
    virtual bool handleMouseMoved( awt::MouseEvent const & evt );

private:
    bool isInArea( com::sun::star::awt::MouseEvent const & evt ) const;
    void updatePressedState( const bool pressedState ) const;

    RehearseTimingsActivity& mrActivity;
    bool                     mbHasBeenClicked;
    bool                     mbMouseStartedInArea;
};

const sal_Int32 LEFT_BORDER_SPACE  = 10;
const sal_Int32 LOWER_BORDER_SPACE = 30;

RehearseTimingsActivity::RehearseTimingsActivity( const SlideShowContext& rContext ) :
    mrEventQueue(rContext.mrEventQueue),
    mrScreenUpdater(rContext.mrScreenUpdater),
    mrEventMultiplexer(rContext.mrEventMultiplexer),
    mrActivitiesQueue(rContext.mrActivitiesQueue),
    maElapsedTime( rContext.mrEventQueue.getTimer() ),
    maViews(),
    maSpriteRectangle(),
    maFont( Application::GetSettings().GetStyleSettings().GetInfoFont() ),
    mpWakeUpEvent(),
    mpMouseHandler(),
    maSpriteSizePixel(),
    mnYOffset(0),
    mbActive(false),
    mbDrawPressed(false)
{
    maFont.SetHeight( maFont.GetHeight() * 2 );
    maFont.SetWidth( maFont.GetWidth() * 2 );
    maFont.SetAlign( ALIGN_BASELINE );
    maFont.SetColor( COL_BLACK );

    // determine sprite size (in pixel):
    VirtualDevice blackHole;
    blackHole.EnableOutput(false);
    blackHole.SetFont( maFont );
    blackHole.SetMapMode( MAP_PIXEL );
    Rectangle rect;
    const FontMetric metric( blackHole.GetFontMetric() );
    blackHole.GetTextBoundRect(
        rect, String("XX:XX:XX") );
    maSpriteSizePixel.setX( rect.getWidth() * 12 / 10 );
    maSpriteSizePixel.setY( metric.GetLineHeight() * 11 / 10 );
    mnYOffset = (metric.GetAscent() + (metric.GetLineHeight() / 20));

    std::for_each( rContext.mrViewContainer.begin(),
                   rContext.mrViewContainer.end(),
                   boost::bind( &RehearseTimingsActivity::viewAdded,
                                this,
                                _1 ));
}

RehearseTimingsActivity::~RehearseTimingsActivity()
{
    try
    {
        stop();
    }
    catch (uno::Exception &)
    {
        OSL_FAIL( rtl::OUStringToOString(
                        comphelper::anyToString(
                            cppu::getCaughtException() ),
                        RTL_TEXTENCODING_UTF8 ).getStr() );
    }
}

boost::shared_ptr<RehearseTimingsActivity> RehearseTimingsActivity::create(
    const SlideShowContext& rContext )
{
    boost::shared_ptr<RehearseTimingsActivity> pActivity(
        new RehearseTimingsActivity( rContext ));

    pActivity->mpMouseHandler.reset(
        new MouseHandler(*pActivity.get()) );
    pActivity->mpWakeUpEvent.reset(
        new WakeupEvent( rContext.mrEventQueue.getTimer(),
                         pActivity,
                         rContext.mrActivitiesQueue ));

    rContext.mrEventMultiplexer.addViewHandler( pActivity );

    return pActivity;
}

void RehearseTimingsActivity::start()
{
    maElapsedTime.reset();
    mbDrawPressed = false;
    mbActive = true;

    // paint and show all sprites:
    paintAllSprites();
    for_each_sprite( boost::bind( &cppcanvas::Sprite::show, _1 ) );

    mrActivitiesQueue.addActivity( shared_from_this() );

    mpMouseHandler->reset();
    mrEventMultiplexer.addClickHandler(
        mpMouseHandler, 42 /* highest prio of all, > 3.0 */ );
    mrEventMultiplexer.addMouseMoveHandler(
        mpMouseHandler, 42 /* highest prio of all, > 3.0 */ );
}

double RehearseTimingsActivity::stop()
{
    mrEventMultiplexer.removeMouseMoveHandler( mpMouseHandler );
    mrEventMultiplexer.removeClickHandler( mpMouseHandler );

    mbActive = false; // will be removed from queue

    for_each_sprite( boost::bind( &cppcanvas::Sprite::hide, _1 ) );

    return maElapsedTime.getElapsedTime();
}

bool RehearseTimingsActivity::hasBeenClicked() const
{
    if (mpMouseHandler)
        return mpMouseHandler->hasBeenClicked();
    return false;
}

// Disposable:
void RehearseTimingsActivity::dispose()
{
    stop();

    mpWakeUpEvent.reset();
    mpMouseHandler.reset();

    ViewsVecT().swap( maViews );
}

// Activity:
double RehearseTimingsActivity::calcTimeLag() const
{
    return 0.0;
}

bool RehearseTimingsActivity::perform()
{
    if( !isActive() )
        return false;

    if( !mpWakeUpEvent )
        return false;

    mpWakeUpEvent->start();
    mpWakeUpEvent->setNextTimeout( 0.5 );
    mrEventQueue.addEvent( mpWakeUpEvent );

    paintAllSprites();

    // sprites changed, need screen update
    mrScreenUpdater.notifyUpdate();

    return false; // don't reinsert, WakeupEvent will perform
                  // that after the given timeout
}

bool RehearseTimingsActivity::isActive() const
{
    return mbActive;
}

void RehearseTimingsActivity::dequeued()
{
    // not used here
}

void RehearseTimingsActivity::end()
{
    if (isActive())
    {
        stop();
        mbActive = false;
    }
}

basegfx::B2DRange RehearseTimingsActivity::calcSpriteRectangle( UnoViewSharedPtr const& rView ) const
{
    const Reference<rendering::XBitmap> xBitmap( rView->getCanvas()->getUNOCanvas(),
                                                 UNO_QUERY );
    if( !xBitmap.is() )
        return basegfx::B2DRange();

    const geometry::IntegerSize2D realSize( xBitmap->getSize() );
    // pixel:
    basegfx::B2DPoint spritePos(
        std::min<sal_Int32>( realSize.Width, LEFT_BORDER_SPACE ),
        std::max<sal_Int32>( 0, realSize.Height - maSpriteSizePixel.getY()
                                                - LOWER_BORDER_SPACE ) );
    basegfx::B2DHomMatrix transformation( rView->getTransformation() );
    transformation.invert();
    spritePos *= transformation;
    basegfx::B2DSize spriteSize( maSpriteSizePixel.getX(),
                                 maSpriteSizePixel.getY() );
    spriteSize *= transformation;
    return basegfx::B2DRange(
        spritePos.getX(), spritePos.getY(),
        spritePos.getX() + spriteSize.getX(),
        spritePos.getY() + spriteSize.getY() );
}

void RehearseTimingsActivity::viewAdded( const UnoViewSharedPtr& rView )
{
    cppcanvas::CustomSpriteSharedPtr sprite(
        rView->createSprite( basegfx::B2DSize(
                                 maSpriteSizePixel.getX()+2,
                                 maSpriteSizePixel.getY()+2 ),
                             1001.0 )); // sprite should be in front of all
                                        // other sprites
    sprite->setAlpha( 0.8 );
    const basegfx::B2DRange spriteRectangle(
        calcSpriteRectangle( rView ) );
    sprite->move( basegfx::B2DPoint(
                      spriteRectangle.getMinX(),
                      spriteRectangle.getMinY() ) );

    if( maViews.empty() )
        maSpriteRectangle = spriteRectangle;

    maViews.push_back( ViewsVecT::value_type( rView, sprite ) );

    if (isActive())
        sprite->show();
}

void RehearseTimingsActivity::viewRemoved( const UnoViewSharedPtr& rView )
{
    maViews.erase(
        std::remove_if(
            maViews.begin(), maViews.end(),
            boost::bind(
                std::equal_to<UnoViewSharedPtr>(),
                rView,
                // select view:
                boost::bind( o3tl::select1st<ViewsVecT::value_type>(), _1 ))),
        maViews.end() );
}

void RehearseTimingsActivity::viewChanged( const UnoViewSharedPtr& rView )
{
    // find entry corresponding to modified view
    ViewsVecT::iterator aModifiedEntry(
        std::find_if(
            maViews.begin(),
            maViews.end(),
            boost::bind(
                std::equal_to<UnoViewSharedPtr>(),
                rView,
                // select view:
                boost::bind( o3tl::select1st<ViewsVecT::value_type>(), _1 ))));

    OSL_ASSERT( aModifiedEntry != maViews.end() );
    if( aModifiedEntry == maViews.end() )
        return;

    // new sprite pos, transformation might have changed:
    maSpriteRectangle = calcSpriteRectangle( rView );

    // reposition sprite:
    aModifiedEntry->second->move( maSpriteRectangle.getMinimum() );

    // sprites changed, need screen update
    mrScreenUpdater.notifyUpdate( rView );
}

void RehearseTimingsActivity::viewsChanged()
{
    if( !maViews.empty() )
    {
        // new sprite pos, transformation might have changed:
        maSpriteRectangle = calcSpriteRectangle( maViews.front().first );

        // reposition sprites
        for_each_sprite( boost::bind( &cppcanvas::Sprite::move,
                                      _1,
                                      boost::cref(maSpriteRectangle.getMinimum())) );

        // sprites changed, need screen update
        mrScreenUpdater.notifyUpdate();
    }
}

void RehearseTimingsActivity::paintAllSprites() const
{
    for_each_sprite(
        boost::bind( &RehearseTimingsActivity::paint, this,
                     // call getContentCanvas() on each sprite:
                     boost::bind(
                         &cppcanvas::CustomSprite::getContentCanvas, _1 ) ) );
}

void RehearseTimingsActivity::paint( cppcanvas::CanvasSharedPtr const & canvas ) const
{
    // build timer string:
    const sal_Int32 nTimeSecs =
        static_cast<sal_Int32>(maElapsedTime.getElapsedTime());
    rtl::OUStringBuffer buf;
    sal_Int32 n = (nTimeSecs / 3600);
    if (n < 10)
        buf.append( static_cast<sal_Unicode>('0') );
    buf.append( n );
    buf.append( static_cast<sal_Unicode>(':') );
    n = ((nTimeSecs % 3600) / 60);
    if (n < 10)
        buf.append( static_cast<sal_Unicode>('0') );
    buf.append( n );
    buf.append( static_cast<sal_Unicode>(':') );
    n = (nTimeSecs % 60);
    if (n < 10)
        buf.append( static_cast<sal_Unicode>('0') );
    buf.append( n );
    const rtl::OUString time = buf.makeStringAndClear();

    // create the MetaFile:
    GDIMetaFile metaFile;
    VirtualDevice blackHole;
    metaFile.Record( &blackHole );
    metaFile.SetPrefSize( Size( 1, 1 ) );
    blackHole.EnableOutput(false);
    blackHole.SetMapMode( MAP_PIXEL );
    blackHole.SetFont( maFont );
    Rectangle rect = Rectangle( 0,0,
                                maSpriteSizePixel.getX(),
                                maSpriteSizePixel.getY());
    if (mbDrawPressed)
    {
        blackHole.SetTextColor( COL_BLACK );
        blackHole.SetFillColor( COL_LIGHTGRAY );
        blackHole.SetLineColor( COL_GRAY );
    }
    else
    {
        blackHole.SetTextColor( COL_BLACK );
        blackHole.SetFillColor( COL_WHITE );
        blackHole.SetLineColor( COL_GRAY );
    }
    blackHole.DrawRect( rect );
    blackHole.GetTextBoundRect( rect, time );
    blackHole.DrawText(
        Point( (maSpriteSizePixel.getX() - rect.getWidth()) / 2,
               mnYOffset ), time );

    metaFile.Stop();
    metaFile.WindStart();

    cppcanvas::RendererSharedPtr renderer(
        cppcanvas::VCLFactory::getInstance().createRenderer(
            canvas, metaFile, cppcanvas::Renderer::Parameters() ) );
    const bool succ = renderer->draw();
    OSL_ASSERT( succ );
    (void)succ;
}


RehearseTimingsActivity::MouseHandler::MouseHandler( RehearseTimingsActivity& rta ) :
    mrActivity(rta),
    mbHasBeenClicked(false),
    mbMouseStartedInArea(false)
{}

void RehearseTimingsActivity::MouseHandler::reset()
{
    mbHasBeenClicked = false;
    mbMouseStartedInArea = false;
}

bool RehearseTimingsActivity::MouseHandler::isInArea(
    awt::MouseEvent const & evt ) const
{
    return mrActivity.maSpriteRectangle.isInside(
        basegfx::B2DPoint( evt.X, evt.Y ) );
}

void RehearseTimingsActivity::MouseHandler::updatePressedState(
    const bool pressedState ) const
{
    if( pressedState != mrActivity.mbDrawPressed )
    {
        mrActivity.mbDrawPressed = pressedState;
        mrActivity.paintAllSprites();

        mrActivity.mrScreenUpdater.notifyUpdate();
    }
}

// MouseEventHandler
bool RehearseTimingsActivity::MouseHandler::handleMousePressed(
    awt::MouseEvent const & evt )
{
    if( evt.Buttons == awt::MouseButton::LEFT && isInArea(evt) )
    {
        mbMouseStartedInArea = true;
        updatePressedState(true);
        return true; // consume event
    }
    return false;
}

bool RehearseTimingsActivity::MouseHandler::handleMouseReleased(
    awt::MouseEvent const & evt )
{
    if( evt.Buttons == awt::MouseButton::LEFT && mbMouseStartedInArea )
    {
        mbHasBeenClicked = isInArea(evt); // fini if in
        mbMouseStartedInArea = false;
        updatePressedState(false);
        if( !mbHasBeenClicked )
            return true; // consume event, else next slide (manual advance)
    }
    return false;
}

bool RehearseTimingsActivity::MouseHandler::handleMouseEntered(
    awt::MouseEvent const & /*evt*/ )
{
    return false;
}

bool RehearseTimingsActivity::MouseHandler::handleMouseExited(
    awt::MouseEvent const & /*evt*/ )
{
    return false;
}

bool RehearseTimingsActivity::MouseHandler::handleMouseDragged(
    awt::MouseEvent const & evt )
{
    if( mbMouseStartedInArea )
        updatePressedState( isInArea(evt) );
    return false;
}

bool RehearseTimingsActivity::MouseHandler::handleMouseMoved(
    awt::MouseEvent const & /*evt*/ )
{
    return false;
}

} // namespace internal
} // namespace presentation

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
