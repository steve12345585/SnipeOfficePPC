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

#include <svgio/svgreader/svgpathnode.hxx>
#include <basegfx/polygon/b2dpolypolygontools.hxx>

//////////////////////////////////////////////////////////////////////////////

namespace svgio
{
    namespace svgreader
    {
        SvgPathNode::SvgPathNode(
            SvgDocument& rDocument,
            SvgNode* pParent)
        :   SvgNode(SVGTokenPath, rDocument, pParent),
            maSvgStyleAttributes(*this),
            mpPolyPolygon(0),
            mpaTransform(0),
            maPathLength()
        {
        }

        SvgPathNode::~SvgPathNode()
        {
            if(mpPolyPolygon) delete mpPolyPolygon;
            if(mpaTransform) delete mpaTransform;
        }

        const SvgStyleAttributes* SvgPathNode::getSvgStyleAttributes() const
        {
            static rtl::OUString aClassStr(rtl::OUString::createFromAscii("path"));
            maSvgStyleAttributes.checkForCssStyle(aClassStr);

            return &maSvgStyleAttributes;
        }

        void SvgPathNode::parseAttribute(const rtl::OUString& rTokenName, SVGToken aSVGToken, const rtl::OUString& aContent)
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
                case SVGTokenD:
                {
                    basegfx::B2DPolyPolygon aPath;

                    if(basegfx::tools::importFromSvgD(aPath, aContent))
                    {
                        if(aPath.count())
                        {
                            setPath(&aPath);
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
                case SVGTokenPathLength:
                {
                    SvgNumber aNum;

                    if(readSingleNumber(aContent, aNum))
                    {
                        setPathLength(aNum);
                    }
                    break;
                }
                default:
                {
                    break;
                }
            }
        }

        void SvgPathNode::decomposeSvgNode(drawinglayer::primitive2d::Primitive2DSequence& rTarget, bool /*bReferenced*/) const
        {
            // fill and/or stroke needed, also a path
            const SvgStyleAttributes* pStyle = getSvgStyleAttributes();

            if(pStyle && getPath())
            {
                drawinglayer::primitive2d::Primitive2DSequence aNewTarget;

                pStyle->add_path(*getPath(), aNewTarget);

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
