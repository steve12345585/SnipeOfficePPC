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
#ifndef INCLUDED_SLIDESHOW_SHAPEMANAGERIMPL_HXX
#define INCLUDED_SLIDESHOW_SHAPEMANAGERIMPL_HXX

#include <osl/mutex.hxx>
#include <cppuhelper/interfacecontainer.h>
#include <com/sun/star/presentation/XShapeEventListener.hpp>

#include "shape.hxx"
#include "subsettableshapemanager.hxx"
#include "eventmultiplexer.hxx"
#include "layermanager.hxx"
#include "viewupdate.hxx"
#include "shapemaps.hxx"
#include "cursormanager.hxx"
#include "hyperlinkarea.hxx"
#include "listenercontainer.hxx"
#include "shapelistenereventhandler.hxx"
#include "mouseeventhandler.hxx"

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <set>
#include <map>

namespace slideshow {
namespace internal {

/** Listener class for shape events

    This helper class registers itself on each view, and
    broadcasts the XShapeEventListener events. The mouse motion
    events are needed for setting the shape cursor.
*/
class ShapeManagerImpl : public SubsettableShapeManager,
                         public ShapeListenerEventHandler,
                         public MouseEventHandler,
                         public ViewUpdate,
                         public boost::enable_shared_from_this<ShapeManagerImpl>,
                         private boost::noncopyable
{
public:
    /** Create a shape event broadcaster

        @param rEventMultiplexer
        The slideshow-global event source, where this class
        registeres its event handlers.
    */
    ShapeManagerImpl( EventMultiplexer&            rMultiplexer,
                      LayerManagerSharedPtr const& rLayerManager,
                      CursorManager&               rCursorManager,
                      const ShapeEventListenerMap& rGlobalListenersMap,
                      const ShapeCursorMap&        rGlobalCursorMap );

    /** Enables event listening.

        @param bSlideBackgoundPainted
        When true, the initial slide content on the background layer
        is already rendered (e.g. from a previous slide
        transition). When false, slide renders initial content of
        slide.
     */
    void activate( bool bSlideBackgoundPainted );

    /** Disables event listening.
     */
    void deactivate();

    // Disposable interface
    // ---------------------------------------------------------------

    virtual void dispose();

private:

    // MouseEventHandler interface
    // ---------------------------------------------------------------

    virtual bool handleMousePressed(
        ::com::sun::star::awt::MouseEvent const& evt );
    virtual bool handleMouseReleased(
        ::com::sun::star::awt::MouseEvent const& evt );
    virtual bool handleMouseEntered(
        ::com::sun::star::awt::MouseEvent const& evt );
    virtual bool handleMouseExited(
        ::com::sun::star::awt::MouseEvent const& evt );
    virtual bool handleMouseDragged(
        ::com::sun::star::awt::MouseEvent const& evt );
    virtual bool handleMouseMoved(
        ::com::sun::star::awt::MouseEvent const& evt );


    // ViewUpdate interface
    // -------------------------------------------------------------------

    virtual bool update();
    virtual bool update( ViewSharedPtr const& rView );
    virtual bool needsUpdate() const;


    // ShapeManager interface
    // ---------------------------------------------------

    virtual void enterAnimationMode( const AnimatableShapeSharedPtr& rShape );
    virtual void leaveAnimationMode( const AnimatableShapeSharedPtr& rShape );
    virtual void notifyShapeUpdate( const ShapeSharedPtr& rShape );
    virtual ShapeSharedPtr lookupShape(
        ::com::sun::star::uno::Reference<
           ::com::sun::star::drawing::XShape > const & xShape ) const;
    virtual void addHyperlinkArea( const boost::shared_ptr<HyperlinkArea>& rArea );
    virtual void removeHyperlinkArea( const boost::shared_ptr<HyperlinkArea>& rArea );


    // SubsettableShapeManager interface
    // ---------------------------------------------------

    virtual boost::shared_ptr<AttributableShape> getSubsetShape(
        const boost::shared_ptr<AttributableShape>& rOrigShape,
        const DocTreeNode&                          rTreeNode );
    virtual void revokeSubset(
        const boost::shared_ptr<AttributableShape>& rOrigShape,
        const boost::shared_ptr<AttributableShape>& rSubsetShape );

    virtual void addIntrinsicAnimationHandler(
        const IntrinsicAnimationEventHandlerSharedPtr& rHandler );
    virtual void removeIntrinsicAnimationHandler(
        const IntrinsicAnimationEventHandlerSharedPtr& rHandler );
    virtual bool notifyIntrinsicAnimationsEnabled();
    virtual bool notifyIntrinsicAnimationsDisabled();


    // ShapeListenerEventHandler
    // ---------------------------------------------

    virtual bool listenerAdded( const ::com::sun::star::uno::Reference<
                                   ::com::sun::star::presentation::XShapeEventListener>& xListener,
                                const ::com::sun::star::uno::Reference<
                                   ::com::sun::star::drawing::XShape>&                   xShape );

    virtual bool listenerRemoved( const ::com::sun::star::uno::Reference<
                                     ::com::sun::star::presentation::XShapeEventListener>& xListener,
                                  const ::com::sun::star::uno::Reference<
                                     ::com::sun::star::drawing::XShape>&                   xShape );

    // ShapeCursorEventHandler interface
    // ---------------------------------------------------------------

    virtual bool cursorChanged( const ::com::sun::star::uno::Reference<
                                   ::com::sun::star::drawing::XShape>&   xShape,
                                sal_Int16                                nCursor );


    ::rtl::OUString checkForHyperlink( ::basegfx::B2DPoint const& hitPos )const;


    typedef std::map<ShapeSharedPtr,
                     boost::shared_ptr< ::cppu::OInterfaceContainerHelper >,
                     Shape::lessThanShape>        ShapeToListenersMap;
    typedef std::map<ShapeSharedPtr, sal_Int16,
                       Shape::lessThanShape>      ShapeToCursorMap;
    typedef std::set<HyperlinkAreaSharedPtr,
                     HyperlinkArea::lessThanArea> AreaSet;

    typedef ThreadUnsafeListenerContainer<
        IntrinsicAnimationEventHandlerSharedPtr,
        std::vector<IntrinsicAnimationEventHandlerSharedPtr> > ImplIntrinsicAnimationEventHandlers;

    EventMultiplexer&                   mrMultiplexer;
    LayerManagerSharedPtr               mpLayerManager;
    CursorManager&                      mrCursorManager;
    const ShapeEventListenerMap&        mrGlobalListenersMap;
    const ShapeCursorMap&               mrGlobalCursorMap;
    ShapeToListenersMap                 maShapeListenerMap;
    ShapeToCursorMap                    maShapeCursorMap;
    AreaSet                             maHyperlinkShapes;
    ImplIntrinsicAnimationEventHandlers maIntrinsicAnimationEventHandlers;
    bool                                mbEnabled;
};

} // namespace internal
} // namespace presentation

#endif /* INCLUDED_SLIDESHOW_SHAPEMANAGERIMPL_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
