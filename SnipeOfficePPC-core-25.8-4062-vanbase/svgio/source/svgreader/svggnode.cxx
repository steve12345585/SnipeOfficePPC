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

#include <svgio/svgreader/svggnode.hxx>
#include <drawinglayer/primitive2d/transformprimitive2d.hxx>
#include <drawinglayer/primitive2d/unifiedtransparenceprimitive2d.hxx>

//////////////////////////////////////////////////////////////////////////////

namespace svgio
{
    namespace svgreader
    {
        SvgGNode::SvgGNode(
            SVGToken aType,
            SvgDocument& rDocument,
            SvgNode* pParent)
        :   SvgNode(aType, rDocument, pParent),
            maSvgStyleAttributes(*this),
            mpaTransform(0)
        {
            OSL_ENSURE(aType == SVGTokenDefs || aType == SVGTokenG, "SvgGNode should ony be used for Group and Defs (!)");
        }

        SvgGNode::~SvgGNode()
        {
            if(mpaTransform) delete mpaTransform;
        }

        const SvgStyleAttributes* SvgGNode::getSvgStyleAttributes() const
        {
            static rtl::OUString aClassStr(rtl::OUString::createFromAscii("g"));
            maSvgStyleAttributes.checkForCssStyle(aClassStr);

            return &maSvgStyleAttributes;
        }

        void SvgGNode::parseAttribute(const rtl::OUString& rTokenName, SVGToken aSVGToken, const rtl::OUString& aContent)
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

        void SvgGNode::decomposeSvgNode(drawinglayer::primitive2d::Primitive2DSequence& rTarget, bool bReferenced) const
        {
            const SvgStyleAttributes* pStyle = getSvgStyleAttributes();

            if(pStyle)
            {
                const double fOpacity(pStyle->getOpacity().getNumber());

                if(fOpacity > 0.0)
                {
                    drawinglayer::primitive2d::Primitive2DSequence aContent;

                    // decompose childs
                    SvgNode::decomposeSvgNode(aContent, bReferenced);

                    if(aContent.hasElements())
                    {
                        pStyle->add_postProcess(rTarget, aContent, getTransform());
                    }
                }
            }
        }
    } // end of namespace svgreader
} // end of namespace svgio

//////////////////////////////////////////////////////////////////////////////
// eof

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
