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

#ifndef INCLUDED_SVGIO_SVGREADER_SVGCIRCLENODE_HXX
#define INCLUDED_SVGIO_SVGREADER_SVGCIRCLENODE_HXX

#include <svgio/svgreader/svgnode.hxx>
#include <svgio/svgreader/svgstyleattributes.hxx>

//////////////////////////////////////////////////////////////////////////////

namespace svgio
{
    namespace svgreader
    {
        class SvgCircleNode : public SvgNode
        {
        private:
            /// use styles
            SvgStyleAttributes      maSvgStyleAttributes;

            /// variable scan values, dependent of given XAttributeList
            SvgNumber               maCx;
            SvgNumber               maCy;
            SvgNumber               maR;
            basegfx::B2DHomMatrix*  mpaTransform;

        public:
            SvgCircleNode(
                SvgDocument& rDocument,
                SvgNode* pParent);
            virtual ~SvgCircleNode();

            virtual const SvgStyleAttributes* getSvgStyleAttributes() const;
            virtual void parseAttribute(const rtl::OUString& rTokenName, SVGToken aSVGToken, const rtl::OUString& aContent);
            virtual void decomposeSvgNode(drawinglayer::primitive2d::Primitive2DSequence& rTarget, bool bReferenced) const;

            /// Cx content, set if found in current context
            const SvgNumber& getCx() const { return maCx; }
            void setCx(const SvgNumber& rCx = SvgNumber()) { maCx = rCx; }

            /// Cy content, set if found in current context
            const SvgNumber& getCy() const { return maCy; }
            void setCy(const SvgNumber& rCy = SvgNumber()) { maCy = rCy; }

            /// R content, set if found in current context
            const SvgNumber& getR() const { return maR; }
            void setR(const SvgNumber& rR = SvgNumber()) { maR = rR; }

            /// transform content, set if found in current context
            const basegfx::B2DHomMatrix* getTransform() const { return mpaTransform; }
            void setTransform(const basegfx::B2DHomMatrix* pMatrix = 0) { if(mpaTransform) delete mpaTransform; mpaTransform = 0; if(pMatrix) mpaTransform = new basegfx::B2DHomMatrix(*pMatrix); }
        };
    } // end of namespace svgreader
} // end of namespace svgio

//////////////////////////////////////////////////////////////////////////////

#endif //INCLUDED_SVGIO_SVGREADER_SVGCIRCLENODE_HXX

// eof

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
