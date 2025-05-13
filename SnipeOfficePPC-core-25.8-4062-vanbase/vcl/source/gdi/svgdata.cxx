/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
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

#include <tools/stream.hxx>
#include <vcl/svgdata.hxx>
#include <comphelper/processfactory.hxx>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/graphic/XSvgParser.hpp>
#include <com/sun/star/graphic/XPrimitive2DRenderer.hpp>
#include <com/sun/star/rendering/XIntegerReadOnlyBitmap.hpp>
#include <vcl/canvastools.hxx>
#include <comphelper/seqstream.hxx>
#include <vcl/svapp.hxx>
#include <vcl/outdev.hxx>

//////////////////////////////////////////////////////////////////////////////

using namespace ::com::sun::star;

//////////////////////////////////////////////////////////////////////////////

void SvgData::ensureReplacement()
{
    ensureSequenceAndRange();

    if(maReplacement.IsEmpty() && maSequence.hasElements())
    {
        // create replacement graphic from maSequence
        // create XPrimitive2DRenderer
        uno::Reference< lang::XMultiServiceFactory > xFactory(::comphelper::getProcessServiceFactory());
        const rtl::OUString aServiceName(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.graphic.Primitive2DTools"));

        try
        {
            const uno::Reference< graphic::XPrimitive2DRenderer > xPrimitive2DRenderer(xFactory->createInstance(aServiceName), uno::UNO_QUERY_THROW);

            if(xPrimitive2DRenderer.is())
            {
                uno::Sequence< beans::PropertyValue > aViewParameters;
                const basegfx::B2DRange& rRange(getRange());
                geometry::RealRectangle2D aRealRect;

                aRealRect.X1 = rRange.getMinX();
                aRealRect.Y1 = rRange.getMinY();
                aRealRect.X2 = rRange.getMaxX();
                aRealRect.Y2 = rRange.getMaxY();

                // get system DPI
                const Size aDPI(Application::GetDefaultDevice()->LogicToPixel(Size(1, 1), MAP_INCH));

                const uno::Reference< rendering::XBitmap > xBitmap(
                    xPrimitive2DRenderer->rasterize(
                        maSequence,
                        aViewParameters,
                        aDPI.getWidth(),
                        aDPI.getHeight(),
                        aRealRect,
                        500000));

                if(xBitmap.is())
                {
                    const uno::Reference< rendering::XIntegerReadOnlyBitmap> xIntBmp(xBitmap, uno::UNO_QUERY_THROW);

                    if(xIntBmp.is())
                    {
                        maReplacement = vcl::unotools::bitmapExFromXBitmap(xIntBmp);
                    }
                }
            }
        }
        catch(const uno::Exception&)
        {
            OSL_ENSURE(sal_False, "Got no graphic::XPrimitive2DRenderer (!)" );
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

void SvgData::ensureSequenceAndRange()
{
    if(!maSequence.hasElements() && mnSvgDataArrayLength)
    {
        // import SVG to maSequence, also set maRange
        maRange.reset();

        // create stream
        const uno::Sequence< sal_Int8 > aPostData((sal_Int8*)maSvgDataArray.get(), mnSvgDataArrayLength);
        const uno::Reference< io::XInputStream > myInputStream(new comphelper::SequenceInputStream(aPostData));

        if(myInputStream.is())
        {
            // create SVG interpreter
            uno::Reference< lang::XMultiServiceFactory > xFactory(::comphelper::getProcessServiceFactory());
            const rtl::OUString aServiceName(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.graphic.SvgTools"));

            try
            {
                const uno::Reference< graphic::XSvgParser > xSvgParser(xFactory->createInstance(aServiceName), uno::UNO_QUERY_THROW);

                if(xSvgParser.is())
                {
                    maSequence = xSvgParser->getDecomposition(myInputStream, maPath);
                }
            }
            catch(const uno::Exception&)
            {
                OSL_ENSURE(sal_False, "Got no graphic::XSvgParser (!)" );
            }
        }

        if(maSequence.hasElements())
        {
            const sal_Int32 nCount(maSequence.getLength());
            geometry::RealRectangle2D aRealRect;
            uno::Sequence< beans::PropertyValue > aViewParameters;

            for(sal_Int32 a(0L); a < nCount; a++)
            {
                // get reference
                const Primitive2DReference xReference(maSequence[a]);

                if(xReference.is())
                {
                    aRealRect = xReference->getRange(aViewParameters);

                    maRange.expand(
                        basegfx::B2DRange(
                            aRealRect.X1,
                            aRealRect.Y1,
                            aRealRect.X2,
                            aRealRect.Y2));
                }
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

SvgData::SvgData(const SvgDataArray& rSvgDataArray, sal_uInt32 nSvgDataArrayLength, const rtl::OUString& rPath)
:   maSvgDataArray(rSvgDataArray),
    mnSvgDataArrayLength(nSvgDataArrayLength),
    maPath(rPath),
    maRange(),
    maSequence(),
    maReplacement()
{
}

//////////////////////////////////////////////////////////////////////////////
SvgData::SvgData(const OUString& rPath):
    maSvgDataArray(),
    mnSvgDataArrayLength(0),
    maPath(rPath),
    maRange(),
    maSequence(),
    maReplacement()
{
    SvFileStream rIStm(rPath, STREAM_STD_READ);
    if(rIStm.GetError())
        return;
    const sal_uInt32 nStmPos(rIStm.Tell());
    const sal_uInt32 nStmLen(rIStm.Seek(STREAM_SEEK_TO_END) - nStmPos);
    if(nStmLen)
    {
        SvgDataArray aNewData(new sal_uInt8[nStmLen]);
        rIStm.Seek(nStmPos);
        rIStm.Read(aNewData.get(), nStmLen);

        if(!rIStm.GetError())
        {
            maSvgDataArray = aNewData;
            mnSvgDataArrayLength = nStmLen;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

const basegfx::B2DRange& SvgData::getRange() const
{
    const_cast< SvgData* >(this)->ensureSequenceAndRange();

    return maRange;
}

//////////////////////////////////////////////////////////////////////////////

const Primitive2DSequence& SvgData::getPrimitive2DSequence() const
{
    const_cast< SvgData* >(this)->ensureSequenceAndRange();

    return maSequence;
}

//////////////////////////////////////////////////////////////////////////////

const BitmapEx& SvgData::getReplacement() const
{
    const_cast< SvgData* >(this)->ensureReplacement();

    return maReplacement;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
