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

#ifndef INCLUDED_DRAWINGLAYER_ATTRIBUTE_FILLBITMAPATTRIBUTE_HXX
#define INCLUDED_DRAWINGLAYER_ATTRIBUTE_FILLBITMAPATTRIBUTE_HXX

#include <drawinglayer/drawinglayerdllapi.h>

//////////////////////////////////////////////////////////////////////////////
// predefines

class BitmapEx;

namespace basegfx {
    class B2DPoint;
    class B2DVector;
}

namespace drawinglayer { namespace attribute {
    class ImpFillBitmapAttribute;
}}

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace attribute
    {
        class DRAWINGLAYER_DLLPUBLIC FillBitmapAttribute
        {
        private:
            ImpFillBitmapAttribute*             mpFillBitmapAttribute;

        public:
            /// constructors/assignmentoperator/destructor
            FillBitmapAttribute(
                const BitmapEx& rBitmapEx,
                const basegfx::B2DPoint& rTopLeft,
                const basegfx::B2DVector& rSize,
                bool bTiling);
            FillBitmapAttribute(const FillBitmapAttribute& rCandidate);
            FillBitmapAttribute& operator=(const FillBitmapAttribute& rCandidate);
            ~FillBitmapAttribute();

            // checks if the incarnation is default constructed
            bool isDefault() const;

            // compare operator
            bool operator==(const FillBitmapAttribute& rCandidate) const;

            // data read access
            const BitmapEx& getBitmapEx() const;
            const basegfx::B2DPoint& getTopLeft() const;
            const basegfx::B2DVector& getSize() const;
            bool getTiling() const;
        };
    } // end of namespace attribute
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

#endif //INCLUDED_DRAWINGLAYER_ATTRIBUTE_FILLBITMAPATTRIBUTE_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
