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

#include <drawinglayer/primitive3d/groupprimitive3d.hxx>
#include <drawinglayer/primitive3d/drawinglayer_primitivetypes3d.hxx>

//////////////////////////////////////////////////////////////////////////////

using namespace com::sun::star;

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace primitive3d
    {
        GroupPrimitive3D::GroupPrimitive3D( const Primitive3DSequence& rChildren )
        :   BasePrimitive3D(),
            maChildren(rChildren)
        {
        }

        /** The compare opertator uses the Sequence::==operator, so only checking if
            the rererences are equal. All non-equal references are interpreted as
            non-equal.
         */
        bool GroupPrimitive3D::operator==( const BasePrimitive3D& rPrimitive ) const
        {
            if(BasePrimitive3D::operator==(rPrimitive))
            {
                const GroupPrimitive3D& rCompare = static_cast< const GroupPrimitive3D& >(rPrimitive);

                return (arePrimitive3DSequencesEqual(getChildren(), rCompare.getChildren()));
            }

            return false;
        }

        /// default: just return children, so all renderers not supporting group will use it's content
        Primitive3DSequence GroupPrimitive3D::get3DDecomposition(const geometry::ViewInformation3D& /*rViewInformation*/) const
        {
            return getChildren();
        }

        // provide unique ID
        ImplPrimitrive3DIDBlock(GroupPrimitive3D, PRIMITIVE3D_ID_GROUPPRIMITIVE3D)

    } // end of namespace primitive3d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////
// eof

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
