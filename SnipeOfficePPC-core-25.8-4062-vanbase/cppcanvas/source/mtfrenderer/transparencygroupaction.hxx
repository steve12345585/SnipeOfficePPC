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

#ifndef _CPPCANVAS_TRANSPARENCYGROUPACTION_HXX
#define _CPPCANVAS_TRANSPARENCYGROUPACTION_HXX

#include <cppcanvas/canvas.hxx>
#include <cppcanvas/renderer.hxx>
#include <action.hxx>

#include <memory> // auto_ptr

namespace basegfx {
    class B2DPoint;
    class B2DVector;
}

class GDIMetaFile;
class Gradient;


/* Definition of internal::TransparencyGroupActionFactory class */

namespace cppcanvas
{
    namespace internal
    {
        struct OutDevState;

        typedef ::std::auto_ptr< GDIMetaFile >  MtfAutoPtr;
        typedef ::std::auto_ptr< Gradient >     GradientAutoPtr;

        /** Transparency group action.

            This action groups a bunch of other actions, to be
            rendered with the given transparency setting against the
            background.

            Creates encapsulated converters between GDIMetaFile and
            XCanvas. The Canvas argument is deliberately placed at the
            constructor, to force reconstruction of this object for a
            new canvas. This considerably eases internal state
            handling, since a lot of the internal state (e.g. fonts,
            text layout) is Canvas-dependent.
         */
        class TransparencyGroupActionFactory
        {
        public:
            /** Create new transparency group action.

                @param rGroupMtf
                Metafile that groups all actions to be rendered
                transparent.

                @param rAlphaGradient
                VCL gradient, to be rendered into the action's alpha
                channel.

                @param rParms
                Render parameters

                @param rDstPoint
                Left, top edge of destination, in current state
                coordinate system

                @param rDstSize
                Size of the transparency group object, in current
                state coordinate system.
             */
            static ActionSharedPtr createTransparencyGroupAction( MtfAutoPtr&                   rGroupMtf,
                                                                  GradientAutoPtr&              rAlphaGradient,
                                                                  const Renderer::Parameters&   rParms,
                                                                  const ::basegfx::B2DPoint&    rDstPoint,
                                                                  const ::basegfx::B2DVector&   rDstSize,
                                                                  const CanvasSharedPtr&        rCanvas,
                                                                  const OutDevState&            rState );

        private:
            // static factory, disable big four
            TransparencyGroupActionFactory();
            ~TransparencyGroupActionFactory();
            TransparencyGroupActionFactory(const TransparencyGroupActionFactory&);
            TransparencyGroupActionFactory& operator=( const TransparencyGroupActionFactory& );
        };
    }
}

#endif /*_CPPCANVAS_TRANSPARENCYGROUPACTION_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
