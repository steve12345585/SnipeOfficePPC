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

#ifndef INCLUDED_DRAWINGLAYER_PRIMITIVE2D_FILLGRADIENTPRIMITIVE2D_HXX
#define INCLUDED_DRAWINGLAYER_PRIMITIVE2D_FILLGRADIENTPRIMITIVE2D_HXX

#include <drawinglayer/drawinglayerdllapi.h>

#include <drawinglayer/primitive2d/baseprimitive2d.hxx>
#include <drawinglayer/attribute/fillgradientattribute.hxx>

//////////////////////////////////////////////////////////////////////////////
// predefines

namespace basegfx { class B2DPolygon; }

//////////////////////////////////////////////////////////////////////////////
// FillGradientPrimitive2D class

namespace drawinglayer
{
    namespace primitive2d
    {
        /** FillGradientPrimitive2D class

            This class defines a gradient filling for a rectangular area. The
            Range is defined by the Transformation, the gradient by the FillGradientAttribute.

            The decomposition will deliver the decomposed gradient, e.g. for an ellipse
            gradient the various ellipses in various color steps will be created.

            I have added functionality to create both versions of filled decompositions:
            Those who overlap and non-overlapping ones. The overlapping version is the
            default one since it works with and without AntiAliasing. The non-overlapping
            version is used in the MetafilePrimitive2D decomposition when the old XOR
            paint was recorded.
         */
        class DRAWINGLAYER_DLLPUBLIC FillGradientPrimitive2D : public BufferedDecompositionPrimitive2D
        {
        private:
            /// the geometric definition
            basegfx::B2DRange                       maObjectRange;

            /// the gradient definition
            attribute::FillGradientAttribute        maFillGradient;

            /// local helpers
            void generateMatricesAndColors(
                std::vector< basegfx::B2DHomMatrix >& rMatrices,
                std::vector< basegfx::BColor >& rColors) const;
            Primitive2DSequence createOverlappingFill(
                const std::vector< basegfx::B2DHomMatrix >& rMatrices,
                const std::vector< basegfx::BColor >& rColors,
                const basegfx::B2DPolygon& rUnitPolygon) const;
            Primitive2DSequence createNonOverlappingFill(
                const std::vector< basegfx::B2DHomMatrix >& rMatrices,
                const std::vector< basegfx::BColor >& rColors,
                const basegfx::B2DPolygon& rUnitPolygon) const;

        protected:
            /// local helper
            Primitive2DSequence createFill(bool bOverlapping) const;

            /// local decomposition.
            virtual Primitive2DSequence create2DDecomposition(const geometry::ViewInformation2D& rViewInformation) const;

        public:
            /// constructor
            FillGradientPrimitive2D(
                const basegfx::B2DRange& rObjectRange,
                const attribute::FillGradientAttribute& rFillGradient);

            /// data read access
            const basegfx::B2DRange& getObjectRange() const { return maObjectRange; }
            const attribute::FillGradientAttribute& getFillGradient() const { return maFillGradient; }

            /// compare operator
            virtual bool operator==(const BasePrimitive2D& rPrimitive) const;

            /// get range
            virtual basegfx::B2DRange getB2DRange(const geometry::ViewInformation2D& rViewInformation) const;

            /// provide unique ID
            DeclPrimitrive2DIDBlock()
        };
    } // end of namespace primitive2d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

#endif //INCLUDED_DRAWINGLAYER_PRIMITIVE2D_FILLGRADIENTPRIMITIVE2D_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
