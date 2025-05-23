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

#include <basegfx/range/b2dpolyrange.hxx>

#include <basegfx/range/b2drange.hxx>
#include <basegfx/range/b2drangeclipper.hxx>
#include <basegfx/tuple/b2dtuple.hxx>
#include <basegfx/polygon/b2dpolypolygon.hxx>

#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>
#include <algorithm>
#include <vector>

namespace basegfx
{
    class ImplB2DPolyRange
    {
        void updateBounds()
        {
            maBounds.reset();
            std::for_each(maRanges.begin(),
                          maRanges.end(),
                          boost::bind(
                              (void (B2DRange::*)(const B2DRange&))(
                 &B2DRange::expand),
                              boost::ref(maBounds),
                              _1));
        }

    public:
        ImplB2DPolyRange() :
            maBounds(),
            maRanges(),
            maOrient()
        {}

        explicit ImplB2DPolyRange( const B2DRange& rRange, B2VectorOrientation eOrient ) :
            maBounds( rRange ),
            maRanges( 1, rRange ),
            maOrient( 1, eOrient )
        {}

        bool operator==(const ImplB2DPolyRange& rRHS) const
        {
            return maRanges == rRHS.maRanges && maOrient == rRHS.maOrient;
        }

        sal_uInt32 count() const
        {
            return maRanges.size();
        }

        B2DPolyRange::ElementType getElement(sal_uInt32 nIndex) const
        {
            return boost::make_tuple(maRanges[nIndex],
                                     maOrient[nIndex]);
        }

        void appendElement(const B2DRange& rRange, B2VectorOrientation eOrient, sal_uInt32 nCount)
        {
            maRanges.insert(maRanges.end(), nCount, rRange);
            maOrient.insert(maOrient.end(), nCount, eOrient);
            maBounds.expand(rRange);
        }

        void clear()
        {
            std::vector<B2DRange> aTmpRanges;
            std::vector<B2VectorOrientation> aTmpOrient;

            maRanges.swap(aTmpRanges);
            maOrient.swap(aTmpOrient);

            maBounds.reset();
        }

        bool overlaps( const B2DRange& rRange ) const
        {
            if( !maBounds.overlaps( rRange ) )
                return false;

            const std::vector<B2DRange>::const_iterator aEnd( maRanges.end() );
            return std::find_if( maRanges.begin(),
                                 aEnd,
                                 boost::bind<bool>( boost::mem_fn( &B2DRange::overlaps ),
                                                    _1,
                                                    boost::cref(rRange) ) ) != aEnd;
        }

        B2DPolyPolygon solveCrossovers() const
        {
            return tools::solveCrossovers(maRanges,maOrient);
        }

    private:
        B2DRange                         maBounds;
        std::vector<B2DRange>            maRanges;
        std::vector<B2VectorOrientation> maOrient;
    };

    B2DPolyRange::B2DPolyRange() :
        mpImpl()
    {}

    B2DPolyRange::~B2DPolyRange()
    {}

    B2DPolyRange::B2DPolyRange( const B2DPolyRange& rRange ) :
        mpImpl( rRange.mpImpl )
    {}

    B2DPolyRange& B2DPolyRange::operator=( const B2DPolyRange& rRange )
    {
        mpImpl = rRange.mpImpl;
        return *this;
    }

    bool B2DPolyRange::operator==(const B2DPolyRange& rRange) const
    {
        if(mpImpl.same_object(rRange.mpImpl))
            return true;

        return ((*mpImpl) == (*rRange.mpImpl));
    }

    bool B2DPolyRange::operator!=(const B2DPolyRange& rRange) const
    {
        return !(*this == rRange);
    }

    sal_uInt32 B2DPolyRange::count() const
    {
        return mpImpl->count();
    }

    B2DPolyRange::ElementType B2DPolyRange::getElement(sal_uInt32 nIndex) const
    {
        return mpImpl->getElement(nIndex);
    }

    void B2DPolyRange::appendElement(const B2DRange& rRange, B2VectorOrientation eOrient, sal_uInt32 nCount)
    {
        mpImpl->appendElement(rRange, eOrient, nCount );
    }

    void B2DPolyRange::clear()
    {
        mpImpl->clear();
    }

    bool B2DPolyRange::overlaps( const B2DRange& rRange ) const
    {
        return mpImpl->overlaps(rRange);
    }

    B2DPolyPolygon B2DPolyRange::solveCrossovers() const
    {
        return mpImpl->solveCrossovers();
    }
} // end of namespace basegfx

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
