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


// must be first
#include <canvas/debug.hxx>
#include <canvas/verbosetrace.hxx>
#include <com/sun/star/presentation/EffectCommands.hpp>
#include <com/sun/star/animations/XAnimate.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>

#include "animationcommandnode.hxx"
#include "delayevent.hxx"
#include "tools.hxx"
#include "nodetools.hxx"

#include <boost/bind.hpp>

using namespace com::sun::star;

namespace slideshow {
namespace internal {

namespace EffectCommands = com::sun::star::presentation::EffectCommands;

AnimationCommandNode::AnimationCommandNode( uno::Reference<animations::XAnimationNode> const& xNode,
                                             ::boost::shared_ptr<BaseContainerNode> const& pParent,
                                             NodeContext const& rContext ) :
    BaseNode( xNode, pParent, rContext ),
    mpShape(),
    mxCommandNode( xNode, ::com::sun::star::uno::UNO_QUERY_THROW )
{
    uno::Reference< drawing::XShape > xShape( mxCommandNode->getTarget(),
                                              uno::UNO_QUERY );
    ShapeSharedPtr pShape( getContext().mpSubsettableShapeManager->lookupShape( xShape ) );
    mpShape = ::boost::dynamic_pointer_cast< ExternalMediaShape >( pShape );
}

void AnimationCommandNode::dispose()
{
    mxCommandNode.clear();
    mpShape.reset();
    BaseNode::dispose();
}

void AnimationCommandNode::activate_st()
{
    switch( mxCommandNode->getCommand() ) {
        // the command is user defined
    case EffectCommands::CUSTOM: break;
        // the command is an ole verb.
    case EffectCommands::VERB: break;
        // the command starts playing on a media object
    case EffectCommands::PLAY:
    {
        double fMediaTime=0.0;
        beans::PropertyValue aMediaTime;
        if( (mxCommandNode->getParameter() >>= aMediaTime) && aMediaTime.Name == "MediaTime" )
        {
            aMediaTime.Value >>= fMediaTime;
        }
        if( mpShape )
        {
            mpShape->setMediaTime(fMediaTime/1000.0);
            mpShape->play();
        }
        break;
    }
        // the command toggles the pause status on a media object
    case EffectCommands::TOGGLEPAUSE:
    {
        if( mpShape )
        {
            if( mpShape->isPlaying() )
                mpShape->pause();
            else
                mpShape->play();
        }
        break;
    }
        // the command stops the animation on a media object
    case EffectCommands::STOP:
    {
        if( mpShape )
            mpShape->stop();
        break;
    }
        // the command stops all currently running sound effects
    case EffectCommands::STOPAUDIO:
        getContext().mrEventMultiplexer.notifyCommandStopAudio( getSelf() );
        break;
    }

    // deactivate ASAP:
    scheduleDeactivationEvent(
        makeEvent( boost::bind( &AnimationNode::deactivate, getSelf() ),
                   "AnimationCommandNode::deactivate" ) );
}

bool AnimationCommandNode::hasPendingAnimation() const
{
    return mxCommandNode->getCommand() == EffectCommands::STOPAUDIO || mpShape;
}

} // namespace internal
} // namespace slideshow

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
