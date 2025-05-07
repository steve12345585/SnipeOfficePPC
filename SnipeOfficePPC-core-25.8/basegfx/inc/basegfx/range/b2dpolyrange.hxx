/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
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

#ifndef _BGFX_RANGE_B2DPOLYRANGE_HXX
#define _BGFX_RANGE_B2DPOLYRANGE_HXX

#include <o3tl/cow_wrapper.hxx>
#include <boost/tuple/tuple.hpp>
#include <basegfx/vector/b2enums.hxx>
#include <basegfx/basegfxdllapi.h>

namespace basegfx
{
    class B2DTuple;
    class B2DRange;
    class B2DPolyPolygon;
    class ImplB2DPolyRange;

    /** Multiple ranges in one object.

        This class combines multiple ranges in one object, providing a
        total, enclosing range for it.

        You can use this class e.g. when updating views containing
        rectangular objects. Add each modified object to a
        B2DMultiRange, then test each viewable object against
        intersection with the multi range.

        Similar in spirit to the poly-polygon vs. polygon relationship.

        Note that comparable to polygons, a poly-range can also
        contain 'holes' - this is encoded via polygon orientation at
        the poly-polygon, and via explicit flags for the poly-range.
     */
    class BASEGFX_DLLPUBLIC B2DPolyRange
    {
    public:
        typedef boost::tuple<B2DRange,B2VectorOrientation> ElementType ;

        B2DPolyRange();
        ~B2DPolyRange();

        /** Create a multi range with exactly one containing range
         */
        B2DPolyRange( const B2DPolyRange& );
        B2DPolyRange& operator=( const B2DPolyRange& );

        bool operator==(const B2DPolyRange&) const;
        bool operator!=(const B2DPolyRange&) const;

        /// Number of included ranges
        sal_uInt32 count() const;

        ElementType getElement(sal_uInt32 nIndex) const;

        // insert/append a single range
        void appendElement(const B2DRange& rRange, B2VectorOrientation eOrient, sal_uInt32 nCount = 1);

        void clear();

        /** Test whether given range overlaps one or more of the
            included ranges. Does *not* use overall range, but checks
            individually.
         */
        bool overlaps( const B2DRange& rRange ) const;

        /** Request a poly-polygon with solved cross-overs
         */
        B2DPolyPolygon solveCrossovers() const;

    private:
        o3tl::cow_wrapper< ImplB2DPolyRange > mpImpl;
    };
}

#endif /* _BGFX_RANGE_B2DPOLYRANGE_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
