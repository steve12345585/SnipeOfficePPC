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

#ifndef INCLUDED_DRAWINGLAYER_PRIMITIVE3D_HATCHTEXTUREPRIMITIVE3D_HXX
#define INCLUDED_DRAWINGLAYER_PRIMITIVE3D_HATCHTEXTUREPRIMITIVE3D_HXX

#include <drawinglayer/drawinglayerdllapi.h>

#include <drawinglayer/primitive3d/textureprimitive3d.hxx>
#include <drawinglayer/attribute/fillhatchattribute.hxx>

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace primitive3d
    {
        /** HatchTexturePrimitive3D class

            HatchTexturePrimitive3D is derived from GroupPrimitive3D, but implements
            a decomposition which is complicated enough for buffering. Since the group
            primitive has no default buffering, it is necessary here to add a local
            buffering mechanism for the decomposition
         */
        class DRAWINGLAYER_DLLPUBLIC HatchTexturePrimitive3D : public TexturePrimitive3D
        {
        private:
            /// the hatch definition
            attribute::FillHatchAttribute                   maHatch;

            /// the buffered decomposed hatch
            Primitive3DSequence                             maBuffered3DDecomposition;

        protected:
            /// helper: local decomposition
            Primitive3DSequence impCreate3DDecomposition() const;

            /// local access methods to maBufferedDecomposition
            const Primitive3DSequence& getBuffered3DDecomposition() const { return maBuffered3DDecomposition; }
            void setBuffered3DDecomposition(const Primitive3DSequence& rNew) { maBuffered3DDecomposition = rNew; }

        public:
            /// constructor
            HatchTexturePrimitive3D(
                const attribute::FillHatchAttribute& rHatch,
                const Primitive3DSequence& rChildren,
                const basegfx::B2DVector& rTextureSize,
                bool bModulate,
                bool bFilter);

            /// data read access
            const attribute::FillHatchAttribute& getHatch() const { return maHatch; }

            /// compare operator
            virtual bool operator==(const BasePrimitive3D& rPrimitive) const;

            /// local decomposition.
            virtual Primitive3DSequence get3DDecomposition(const geometry::ViewInformation3D& rViewInformation) const;

            /// provide unique ID
            DeclPrimitrive3DIDBlock()
        };
    } // end of namespace primitive3d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

#endif //INCLUDED_DRAWINGLAYER_PRIMITIVE3D_HATCHTEXTUREPRIMITIVE3D_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
