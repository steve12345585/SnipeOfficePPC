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

#include <drawinglayer/attribute/sdrobjectattribute3d.hxx>
#include <drawinglayer/attribute/materialattribute3d.hxx>

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
    namespace attribute
    {
        class ImpSdr3DObjectAttribute
        {
        public:
            // refcounter
            sal_uInt32                              mnRefCount;

            // 3D object attribute definitions
            ::com::sun::star::drawing::NormalsKind              maNormalsKind;              // normals type (0..2)
            ::com::sun::star::drawing::TextureProjectionMode    maTextureProjectionX;       // texture projection type X (0..2)
            ::com::sun::star::drawing::TextureProjectionMode    maTextureProjectionY;       // texture projection type Y (0..2)
            ::com::sun::star::drawing::TextureKind2             maTextureKind;              // texture kind (see uno API)
            ::com::sun::star::drawing::TextureMode              maTextureMode;              // texture kind (see uno API)
            MaterialAttribute3D                                 maMaterial;                 // object, specular and emissive colors, SpecularIntensity

            // bitfield
            unsigned                                            mbNormalsInvert : 1;        // invert normals
            unsigned                                            mbDoubleSided : 1;          // surfaces are double sided
            unsigned                                            mbShadow3D : 1;             // display shadow in 3D (if on), params for that are at scene
            unsigned                                            mbTextureFilter : 1;        // filter texture to make more smooth
            unsigned                                            mbReducedLineGeometry : 1;  // use reduced line geometry (object specific)

            ImpSdr3DObjectAttribute(
                ::com::sun::star::drawing::NormalsKind  aNormalsKind,
                ::com::sun::star::drawing::TextureProjectionMode aTextureProjectionX,
                ::com::sun::star::drawing::TextureProjectionMode aTextureProjectionY,
                ::com::sun::star::drawing::TextureKind2 aTextureKind,
                ::com::sun::star::drawing::TextureMode aTextureMode,
                const MaterialAttribute3D& rMaterial,
                bool bNormalsInvert,
                bool bDoubleSided,
                bool bShadow3D,
                bool bTextureFilter,
                bool bReducedLineGeometry)
            :   mnRefCount(0),
                maNormalsKind(aNormalsKind),
                maTextureProjectionX(aTextureProjectionX),
                maTextureProjectionY(aTextureProjectionY),
                maTextureKind(aTextureKind),
                maTextureMode(aTextureMode),
                maMaterial(rMaterial),
                mbNormalsInvert(bNormalsInvert),
                mbDoubleSided(bDoubleSided),
                mbShadow3D(bShadow3D),
                mbTextureFilter(bTextureFilter),
                mbReducedLineGeometry(bReducedLineGeometry)
            {
            }

            // data read access
            ::com::sun::star::drawing::NormalsKind getNormalsKind() const { return maNormalsKind; }
            ::com::sun::star::drawing::TextureProjectionMode getTextureProjectionX() const { return maTextureProjectionX; }
            ::com::sun::star::drawing::TextureProjectionMode getTextureProjectionY() const { return maTextureProjectionY; }
            ::com::sun::star::drawing::TextureKind2 getTextureKind() const { return maTextureKind; }
            ::com::sun::star::drawing::TextureMode getTextureMode() const { return maTextureMode; }
            const MaterialAttribute3D& getMaterial() const { return maMaterial; }
            bool getNormalsInvert() const { return mbNormalsInvert; }
            bool getDoubleSided() const { return mbDoubleSided; }
            bool getShadow3D() const { return mbShadow3D; }
            bool getTextureFilter() const { return mbTextureFilter; }
            bool getReducedLineGeometry() const { return mbReducedLineGeometry; }

            bool operator==(const ImpSdr3DObjectAttribute& rCandidate) const
            {
                return (getNormalsKind() == rCandidate.getNormalsKind()
                    && getTextureProjectionX() == rCandidate.getTextureProjectionX()
                    && getTextureProjectionY() == rCandidate.getTextureProjectionY()
                    && getTextureKind() == rCandidate.getTextureKind()
                    && getTextureMode() == rCandidate.getTextureMode()
                    && getMaterial() == rCandidate.getMaterial()
                    && getNormalsInvert() == rCandidate.getNormalsInvert()
                    && getDoubleSided() == rCandidate.getDoubleSided()
                    && getShadow3D() == rCandidate.getShadow3D()
                    && getTextureFilter() == rCandidate.getTextureFilter()
                    && getReducedLineGeometry() == rCandidate.getReducedLineGeometry());
            }

            static ImpSdr3DObjectAttribute* get_global_default()
            {
                static ImpSdr3DObjectAttribute* pDefault = 0;

                if(!pDefault)
                {
                    pDefault = new ImpSdr3DObjectAttribute(
                        ::com::sun::star::drawing::NormalsKind_SPECIFIC,
                        ::com::sun::star::drawing::TextureProjectionMode_OBJECTSPECIFIC,
                        ::com::sun::star::drawing::TextureProjectionMode_OBJECTSPECIFIC,
                        ::com::sun::star::drawing::TextureKind2_LUMINANCE,
                        ::com::sun::star::drawing::TextureMode_REPLACE,
                        MaterialAttribute3D(),
                        false,
                        false,
                        false,
                        false,
                        false);

                    // never delete; start with RefCount 1, not 0
                    pDefault->mnRefCount++;
                }

                return pDefault;
            }
        };

        Sdr3DObjectAttribute::Sdr3DObjectAttribute(
            ::com::sun::star::drawing::NormalsKind  aNormalsKind,
            ::com::sun::star::drawing::TextureProjectionMode aTextureProjectionX,
            ::com::sun::star::drawing::TextureProjectionMode aTextureProjectionY,
            ::com::sun::star::drawing::TextureKind2 aTextureKind,
            ::com::sun::star::drawing::TextureMode aTextureMode,
            const MaterialAttribute3D& rMaterial,
            bool bNormalsInvert,
            bool bDoubleSided,
            bool bShadow3D,
            bool bTextureFilter,
            bool bReducedLineGeometry)
        :   mpSdr3DObjectAttribute(new ImpSdr3DObjectAttribute(
                aNormalsKind, aTextureProjectionX, aTextureProjectionY, aTextureKind, aTextureMode,
                rMaterial, bNormalsInvert, bDoubleSided, bShadow3D, bTextureFilter, bReducedLineGeometry))
        {
        }

        Sdr3DObjectAttribute::Sdr3DObjectAttribute(const Sdr3DObjectAttribute& rCandidate)
        :   mpSdr3DObjectAttribute(rCandidate.mpSdr3DObjectAttribute)
        {
            mpSdr3DObjectAttribute->mnRefCount++;
        }

        Sdr3DObjectAttribute::~Sdr3DObjectAttribute()
        {
            if(mpSdr3DObjectAttribute->mnRefCount)
            {
                mpSdr3DObjectAttribute->mnRefCount--;
            }
            else
            {
                delete mpSdr3DObjectAttribute;
            }
        }

        bool Sdr3DObjectAttribute::isDefault() const
        {
            return mpSdr3DObjectAttribute == ImpSdr3DObjectAttribute::get_global_default();
        }

        Sdr3DObjectAttribute& Sdr3DObjectAttribute::operator=(const Sdr3DObjectAttribute& rCandidate)
        {
            if(rCandidate.mpSdr3DObjectAttribute != mpSdr3DObjectAttribute)
            {
                if(mpSdr3DObjectAttribute->mnRefCount)
                {
                    mpSdr3DObjectAttribute->mnRefCount--;
                }
                else
                {
                    delete mpSdr3DObjectAttribute;
                }

                mpSdr3DObjectAttribute = rCandidate.mpSdr3DObjectAttribute;
                mpSdr3DObjectAttribute->mnRefCount++;
            }

            return *this;
        }

        bool Sdr3DObjectAttribute::operator==(const Sdr3DObjectAttribute& rCandidate) const
        {
            if(rCandidate.mpSdr3DObjectAttribute == mpSdr3DObjectAttribute)
            {
                return true;
            }

            if(rCandidate.isDefault() != isDefault())
            {
                return false;
            }

            return (*rCandidate.mpSdr3DObjectAttribute == *mpSdr3DObjectAttribute);
        }

        ::com::sun::star::drawing::NormalsKind Sdr3DObjectAttribute::getNormalsKind() const
        {
            return mpSdr3DObjectAttribute->getNormalsKind();
        }

        ::com::sun::star::drawing::TextureProjectionMode Sdr3DObjectAttribute::getTextureProjectionX() const
        {
            return mpSdr3DObjectAttribute->getTextureProjectionX();
        }

        ::com::sun::star::drawing::TextureProjectionMode Sdr3DObjectAttribute::getTextureProjectionY() const
        {
            return mpSdr3DObjectAttribute->getTextureProjectionY();
        }

        ::com::sun::star::drawing::TextureKind2 Sdr3DObjectAttribute::getTextureKind() const
        {
            return mpSdr3DObjectAttribute->getTextureKind();
        }

        ::com::sun::star::drawing::TextureMode Sdr3DObjectAttribute::getTextureMode() const
        {
            return mpSdr3DObjectAttribute->getTextureMode();
        }

        const MaterialAttribute3D& Sdr3DObjectAttribute::getMaterial() const
        {
            return mpSdr3DObjectAttribute->getMaterial();
        }

        bool Sdr3DObjectAttribute::getNormalsInvert() const
        {
            return mpSdr3DObjectAttribute->getNormalsInvert();
        }

        bool Sdr3DObjectAttribute::getDoubleSided() const
        {
            return mpSdr3DObjectAttribute->getDoubleSided();
        }

        bool Sdr3DObjectAttribute::getShadow3D() const
        {
            return mpSdr3DObjectAttribute->getShadow3D();
        }

        bool Sdr3DObjectAttribute::getTextureFilter() const
        {
            return mpSdr3DObjectAttribute->getTextureFilter();
        }

        bool Sdr3DObjectAttribute::getReducedLineGeometry() const
        {
            return mpSdr3DObjectAttribute->getReducedLineGeometry();
        }

    } // end of namespace attribute
} // end of namespace drawinglayer

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
