/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
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

#ifndef INCLUDED_DRAWINGLAYER_ATTRIBUTE_MATERIALATTRIBUTE3D_HXX
#define INCLUDED_DRAWINGLAYER_ATTRIBUTE_MATERIALATTRIBUTE3D_HXX

#include <drawinglayer/drawinglayerdllapi.h>

#include <sal/types.h>

//////////////////////////////////////////////////////////////////////////////
// predefines

namespace drawinglayer { namespace attribute {
    class ImpMaterialAttribute3D;
}}

namespace basegfx {
    class BColor;
}

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace attribute
    {
        class DRAWINGLAYER_DLLPUBLIC MaterialAttribute3D
        {
        private:
            ImpMaterialAttribute3D*                             mpMaterialAttribute3D;

        public:
            // constructors/destructor
            MaterialAttribute3D(
                const basegfx::BColor& rColor,
                const basegfx::BColor& rSpecular,
                const basegfx::BColor& rEmission,
                sal_uInt16 nSpecularIntensity);
            MaterialAttribute3D(const basegfx::BColor& rColor);
            MaterialAttribute3D();
            MaterialAttribute3D(const MaterialAttribute3D& rCandidate);
            ~MaterialAttribute3D();

            // assignment operator
            MaterialAttribute3D& operator=(const MaterialAttribute3D& rCandidate);

            // checks if the incarnation is default constructed
            bool isDefault() const;

            // compare operator
            bool operator==(const MaterialAttribute3D& rCandidate) const;

            // data read access
            const basegfx::BColor& getColor() const;
            const basegfx::BColor& getSpecular() const;
            const basegfx::BColor& getEmission() const;
            sal_uInt16 getSpecularIntensity() const;
        };
    } // end of namespace attribute
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

#endif //INCLUDED_DRAWINGLAYER_ATTRIBUTE_MATERIALATTRIBUTE3D_HXX

// eof

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
