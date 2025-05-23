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

#include <svgio/svgreader/svgpolynode.hxx>
#include <basegfx/polygon/b2dpolygon.hxx>
#include <basegfx/polygon/b2dpolypolygontools.hxx>

//////////////////////////////////////////////////////////////////////////////

namespace svgio
{
    namespace svgreader
    {
        SvgPolyNode::SvgPolyNode(
            SvgDocument& rDocument,
            SvgNode* pParent,
            bool bIsPolyline)
        :   SvgNode(SVGTokenPolygon, rDocument, pParent),
            maSvgStyleAttributes(*this),
            mpPolygon(0),
            mpaTransform(0),
            mbIsPolyline(bIsPolyline)
        {
        }

        SvgPolyNode::~SvgPolyNode()
        {
            if(mpaTransform) delete mpaTransform;
            if(mpPolygon) delete mpPolygon;
        }

        const SvgStyleAttributes* SvgPolyNode::getSvgStyleAttributes() const
        {
            static rtl::OUString aClassStrA(rtl::OUString::createFromAscii("polygon"));
            static rtl::OUString aClassStrB(rtl::OUString::createFromAscii("polyline"));
            maSvgStyleAttributes.checkForCssStyle(mbIsPolyline? aClassStrB : aClassStrA);

            return &maSvgStyleAttributes;
        }

        void SvgPolyNode::parseAttribute(const rtl::OUString& rTokenName, SVGToken aSVGToken, const rtl::OUString& aContent)
        {
            // call parent
            SvgNode::parseAttribute(rTokenName, aSVGToken, aContent);

            // read style attributes
            maSvgStyleAttributes.parseStyleAttribute(rTokenName, aSVGToken, aContent);

            // parse own
            switch(aSVGToken)
            {
                case SVGTokenStyle:
                {
                    maSvgStyleAttributes.readStyle(aContent);
                    break;
                }
                case SVGTokenPoints:
                {
                    basegfx::B2DPolygon aPath;

                    if(basegfx::tools::importFromSvgPoints(aPath, aContent))
                    {
                        if(aPath.count())
                        {
                            if(!isPolyline())
                            {
                                aPath.setClosed(true);
                            }

                            setPolygon(&aPath);
                        }
                    }
                    break;
                }
                case SVGTokenTransform:
                {
                    const basegfx::B2DHomMatrix aMatrix(readTransform(aContent, *this));

                    if(!aMatrix.isIdentity())
                    {
                        setTransform(&aMatrix);
                    }
                    break;
                }
                default:
                {
                    break;
                }
            }
        }

        void SvgPolyNode::decomposeSvgNode(drawinglayer::primitive2d::Primitive2DSequence& rTarget, bool /*bReferenced*/) const
        {
            const SvgStyleAttributes* pStyle = getSvgStyleAttributes();

            if(pStyle && getPolygon())
            {
                drawinglayer::primitive2d::Primitive2DSequence aNewTarget;

                pStyle->add_path(basegfx::B2DPolyPolygon(*getPolygon()), aNewTarget);

                if(aNewTarget.hasElements())
                {
                    pStyle->add_postProcess(rTarget, aNewTarget, getTransform());
                }
            }
        }
    } // end of namespace svgreader
} // end of namespace svgio

//////////////////////////////////////////////////////////////////////////////
// eof

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
