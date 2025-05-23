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
#ifndef INCLUDED_SLIDESHOW_ANIMATIONAUDIONODE_HXX
#define INCLUDED_SLIDESHOW_ANIMATIONAUDIONODE_HXX

#include "basecontainernode.hxx"
#include "soundplayer.hxx"
#include "com/sun/star/animations/XAnimationNode.hpp"
#include "com/sun/star/animations/XAudio.hpp"

namespace slideshow {
namespace internal {

/** Audio node.

    This animation node contains an audio effect. Duration and
    start/stop behaviour is affected by the referenced audio
    file.
*/
class AnimationAudioNode : public BaseNode, public AnimationEventHandler
{
public:
    AnimationAudioNode(
        ::com::sun::star::uno::Reference<
        ::com::sun::star::animations::XAnimationNode> const& xNode,
        ::boost::shared_ptr<BaseContainerNode> const& pParent,
        NodeContext const& rContext );

protected:
    virtual void dispose();

private:
    virtual void activate_st();
    virtual void deactivate_st( NodeState eDestState );
    virtual bool hasPendingAnimation() const;

    /// overriden, because we need to deal with STOPAUDIO commands
    virtual bool handleAnimationEvent( const AnimationNodeSharedPtr& rNode );

private:
    ::com::sun::star::uno::Reference<
        ::com::sun::star::animations::XAudio >  mxAudioNode;
    ::rtl::OUString                             maSoundURL;
    mutable SoundPlayerSharedPtr                mpPlayer;

    void createPlayer() const;
    void resetPlayer() const;
};

} // namespace internal
} // namespace slideshow

#endif /* INCLUDED_SLIDESHOW_ANIMATIONAUDIONODE_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
