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

#ifndef _DRAWINGLAYER_PROCESSOR_CANVASPROCESSOR_HXX
#define _DRAWINGLAYER_PROCESSOR_CANVASPROCESSOR_HXX

#include <drawinglayer/drawinglayerdllapi.h>

#include <drawinglayer/processor2d/baseprocessor2d.hxx>
#include <basegfx/matrix/b2dhommatrix.hxx>
#include <basegfx/color/bcolormodifier.hxx>
#include <svtools/optionsdrawinglayer.hxx>
#include <com/sun/star/rendering/ViewState.hpp>
#include <com/sun/star/rendering/RenderState.hpp>
#include <i18npool/lang.h>
#include <basegfx/polygon/b2dpolypolygon.hxx>
#include <vcl/mapmod.hxx>

//////////////////////////////////////////////////////////////////////////////
// forward declaration

namespace basegfx {
    class BColor;
    class B2DPolygon;
}

namespace com { namespace sun { namespace star { namespace rendering {
    class XCanvas;
    class XPolyPolygon2D;
}}}}

namespace drawinglayer { namespace primitive2d {
    class MaskPrimitive2D;
    class MetafilePrimitive2D;
    class TextSimplePortionPrimitive2D;
    class BitmapPrimitive2D;
    class TransparencePrimitive2D;
    class PolygonStrokePrimitive2D;
    class FillBitmapPrimitive2D;
    class UnifiedTransparencePrimitive2D;
}}

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace processor2d
    {
        /** canvasProcessor2D class

            A basic implementation of a renderer for com::sun::star::rendering::XCanvas
            as a target
         */
        class DRAWINGLAYER_DLLPUBLIC canvasProcessor2D : public BaseProcessor2D
        {
        private:
            // the (current) destination OutDev and canvas
            OutputDevice*                                           mpOutputDevice;
            com::sun::star::uno::Reference< com::sun::star::rendering::XCanvas >    mxCanvas;
            com::sun::star::rendering::ViewState                    maViewState;
            com::sun::star::rendering::RenderState                  maRenderState;

            // the modifiedColorPrimitive stack
            basegfx::BColorModifierStack                            maBColorModifierStack;

            // SvtOptionsDrawinglayer incarnation to react on diverse settings
            const SvtOptionsDrawinglayer                            maDrawinglayerOpt;

            // the current clipping PolyPolygon from MaskPrimitive2D, always in
            // object coordinates
            basegfx::B2DPolyPolygon                                 maClipPolyPolygon;

            // determined LanguageType
            LanguageType                                            meLang;

            // as tooling, the process() implementation takes over API handling and calls this
            // virtual render method when the primitive implementation is BasePrimitive2D-based.
            virtual void processBasePrimitive2D(const primitive2d::BasePrimitive2D& rCandidate);

            // direct primitive renderer support
            void impRenderMaskPrimitive2D(const primitive2d::MaskPrimitive2D& rMaskCandidate);
            void impRenderMetafilePrimitive2D(const primitive2d::MetafilePrimitive2D& rMetaCandidate);
            void impRenderTextSimplePortionPrimitive2D(const primitive2d::TextSimplePortionPrimitive2D& rTextCandidate);
            void impRenderBitmapPrimitive2D(const primitive2d::BitmapPrimitive2D& rBitmapCandidate);
            void impRenderTransparencePrimitive2D(const primitive2d::TransparencePrimitive2D& rTransparenceCandidate);
            void impRenderPolygonStrokePrimitive2D(const primitive2d::PolygonStrokePrimitive2D& rPolygonStrokePrimitive);
            void impRenderFillBitmapPrimitive2D(const primitive2d::FillBitmapPrimitive2D& rFillBitmapPrimitive2D);
            void impRenderUnifiedTransparencePrimitive2D(const primitive2d::UnifiedTransparencePrimitive2D& rUniTransparenceCandidate);

        public:
            canvasProcessor2D(
                const geometry::ViewInformation2D& rViewInformation,
                OutputDevice& rOutDev);
            virtual ~canvasProcessor2D();

            // access to Drawinglayer configuration options
            const SvtOptionsDrawinglayer& getOptionsDrawinglayer() const { return maDrawinglayerOpt; }
        };
    } // end of namespace processor2d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

#endif //_DRAWINGLAYER_PROCESSOR_CANVASPROCESSOR_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
