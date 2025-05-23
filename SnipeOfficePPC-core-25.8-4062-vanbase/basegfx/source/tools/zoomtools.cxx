/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Version: MPL 1.1 / GPLv3+ / LGPLv3+
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License or as specified alternatively below. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * Major Contributor(s):
 * Copyright (C) 2012 Tim Hardeck <thardeck@suse.com>
 *
 * All Rights Reserved.
 *
 * For minor contributions see the git repository.
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPLv3+"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
 * in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
 * instead of those above.
 */

#include <basegfx/tools/zoomtools.hxx>

namespace basegfx
{
namespace zoomtools
{

/** 2^(1/6) as the default step

    This ensures (unless the rounding is used) that 6 steps lead
    to double / half zoom level.
*/
const double ZOOM_FACTOR = 1.12246205;

/**
* Round a value against a specified multiple. Values below half
* of the multiple are rounded down and all others are rounded up.
*
* @param nCurrent current value
* @param nMultiple multiple against which the current value is rounded
*/
static long roundMultiple(long nCurrent, int nMultiple)
{
    // round zoom to a multiple of nMultiple
    return (( nCurrent + nMultiple / 2 ) - ( nCurrent + nMultiple / 2 ) % nMultiple);
}

/**
* Convert geometric progression results into more common values by
* rounding them against certain multiples depending on the size.
* Beginning with 50 the multiple is 5, with 100, 10, and so on.
*
* @param nCurrent current zoom factor
*/
static long roundZoom(double nCurrent)
{
    // convert nCurrent properly to int
    long nNew = nCurrent + 0.5;

    // round to more common numbers above 50
    if (nNew > 1000) {
        nNew = roundMultiple(nNew, 100);
    } else if ( nNew > 500 ) {
        nNew = roundMultiple(nNew, 50);
    } else if ( nNew > 100 ) {
        nNew = roundMultiple(nNew, 10);
    } else if ( nNew > 50 ) {
        nNew = roundMultiple(nNew, 5);
    }

    return nNew;
}

/**
* Make sure that a certain step isn't skipped during the zooming
* progress.
*
* @param nCurrent current zoom factor
* @param nPrevious previous zoom factor
* @param nStep step which shouldn't be skipped
*/
static long enforceStep(long nCurrent, long nPrevious, int nStep)
{
    if ((( nCurrent > nStep ) && ( nPrevious < nStep ))
    || (( nCurrent < nStep ) && ( nPrevious > nStep )))
        return nStep;
    else
        return nCurrent;
}

/**
* Increasing the zoom level.
*
* @param nCurrent current zoom factor
*/
long zoomIn(long nCurrent)
{
    long nNew = roundZoom( nCurrent * ZOOM_FACTOR );
    // make sure some values are not skipped
    nNew = enforceStep(nNew, nCurrent, 200);
    nNew = enforceStep(nNew, nCurrent, 100);
    nNew = enforceStep(nNew, nCurrent, 75);
    nNew = enforceStep(nNew, nCurrent, 50);
    nNew = enforceStep(nNew, nCurrent, 25);
    return nNew;
}

/**
* Decreasing the zoom level.
*
* @param nCurrent current zoom factor
*/
long zoomOut(long nCurrent)
{
    long nNew = roundZoom( nCurrent / ZOOM_FACTOR );
    // make sure some values are not skipped
    nNew = enforceStep(nNew, nCurrent, 200);
    nNew = enforceStep(nNew, nCurrent, 100);
    nNew = enforceStep(nNew, nCurrent, 75);
    nNew = enforceStep(nNew, nCurrent, 50);
    nNew = enforceStep(nNew, nCurrent, 25);
    return nNew;
}
} // namespace zoomtools
} // namespace basegfx

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
