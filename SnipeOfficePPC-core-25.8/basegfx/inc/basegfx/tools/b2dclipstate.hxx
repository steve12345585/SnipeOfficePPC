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

#ifndef _BGFX_TOOLS_CLIPSTATE_HXX
#define _BGFX_TOOLS_CLIPSTATE_HXX

#include <sal/types.h>
#include <o3tl/cow_wrapper.hxx>
#include <basegfx/basegfxdllapi.h>

//////////////////////////////////////////////////////////////////////////////

namespace basegfx
{
    class B2DRange;
    class B2DPolyRange;
    class B2DPolygon;
    class B2DPolyPolygon;

    namespace tools
    {
        class ImplB2DClipState;

        /** This class provides an optimized, symbolic clip state for graphical output

            Having a current 'clip' state is a common attribute of
            almost all graphic output APIs, most of which internally
            represent it via a list of rectangular bands. In contrast,
            this implementation purely uses symbolic clips, but in a
            quite efficient manner, deferring actual evaluation until
            a clip representation is requested, and using faster code
            paths for common special cases (like all-rectangle clips)
         */
        class BASEGFX_DLLPUBLIC B2DClipState
        {
        public:
            typedef o3tl::cow_wrapper< ImplB2DClipState > ImplType;

        private:
            ImplType mpImpl;

        public:
            /// Init clip, in 'cleared' state - everything is visible
            B2DClipState();
            ~B2DClipState();
            B2DClipState( const B2DClipState& );
            explicit B2DClipState( const B2DPolyPolygon& );
            B2DClipState& operator=( const B2DClipState& );

            /// Set clip to 'null' - nothing is visible
            void makeNull();

            /// returns true when clip is 'cleared' - everything is visible
            bool isCleared() const;

            bool operator==(const B2DClipState&) const;
            bool operator!=(const B2DClipState&) const;

            void unionRange(const B2DRange& );
            void unionPolyPolygon(const B2DPolyPolygon& );

            void intersectRange(const B2DRange& );
            void intersectPolyPolygon(const B2DPolyPolygon& );

            void subtractRange(const B2DRange& );
            void subtractPolyPolygon(const B2DPolyPolygon& );

            void xorRange(const B2DRange& );
            void xorPolyPolygon(const B2DPolyPolygon& );

            B2DPolyPolygon getClipPoly() const;
        };
    }
}

#endif // _BGFX_TOOLS_CLIPSTATE_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
