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

#ifndef INCLUDED_SVGIO_SVGREADER_SVGSYMBOLNODE_HXX
#define INCLUDED_SVGIO_SVGREADER_SVGSYMBOLNODE_HXX

#include <svgio/svgreader/svgstyleattributes.hxx>

//////////////////////////////////////////////////////////////////////////////

namespace svgio
{
    namespace svgreader
    {
        class SvgSymbolNode : public SvgNode
        {
        private:
            /// use styles
            SvgStyleAttributes      maSvgStyleAttributes;

            /// variable scan values, dependent of given XAttributeList
            basegfx::B2DRange*      mpViewBox;
            SvgAspectRatio          maSvgAspectRatio;

        public:
            SvgSymbolNode(
                SvgDocument& rDocument,
                SvgNode* pParent);
            virtual ~SvgSymbolNode();

            virtual const SvgStyleAttributes* getSvgStyleAttributes() const;
            virtual void parseAttribute(const rtl::OUString& rTokenName, SVGToken aSVGToken, const rtl::OUString& aContent);

            /// viewBox content
            const basegfx::B2DRange* getViewBox() const { return mpViewBox; }
            void setViewBox(const basegfx::B2DRange* pViewBox = 0) { if(mpViewBox) delete mpViewBox; mpViewBox = 0; if(pViewBox) mpViewBox = new basegfx::B2DRange(*pViewBox); }

            /// SvgAspectRatio content
            const SvgAspectRatio& getSvgAspectRatio() const { return maSvgAspectRatio; }
            void setSvgAspectRatio(const SvgAspectRatio& rSvgAspectRatio = SvgAspectRatio()) { maSvgAspectRatio = rSvgAspectRatio; }
        };
    } // end of namespace svgreader
} // end of namespace svgio

//////////////////////////////////////////////////////////////////////////////

#endif //INCLUDED_SVGIO_SVGREADER_SVGSYMBOLNODE_HXX

// eof

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
