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

#ifndef _E3D_POLYGON3D_HXX
#define _E3D_POLYGON3D_HXX

#include <svx/obj3d.hxx>
#include "svx/svxdllapi.h"

class SVX_DLLPUBLIC E3dPolygonObj : public E3dCompoundObject
{
private:
    // parameters
    basegfx::B3DPolyPolygon aPolyPoly3D;
    basegfx::B3DPolyPolygon aPolyNormals3D;
    basegfx::B2DPolyPolygon aPolyTexture2D;
    sal_Bool            bLineOnly;

    SVX_DLLPRIVATE void CreateDefaultNormals();
    SVX_DLLPRIVATE void CreateDefaultTexture();

protected:
    virtual sdr::contact::ViewContact* CreateObjectSpecificViewContact();

public:
    void SetPolyPolygon3D(const basegfx::B3DPolyPolygon& rNewPolyPoly3D);
    void SetPolyNormals3D(const basegfx::B3DPolyPolygon& rNewPolyPoly3D);
    void SetPolyTexture2D(const basegfx::B2DPolyPolygon& rNewPolyPoly2D);

    TYPEINFO();

    E3dPolygonObj(
        E3dDefaultAttributes& rDefault,
        const basegfx::B3DPolyPolygon& rPolyPoly3D,
        sal_Bool bLinOnly=sal_False);

    E3dPolygonObj();
    virtual ~E3dPolygonObj();

    const basegfx::B3DPolyPolygon& GetPolyPolygon3D() const { return aPolyPoly3D; }
    const basegfx::B3DPolyPolygon& GetPolyNormals3D() const { return aPolyNormals3D; }
    const basegfx::B2DPolyPolygon& GetPolyTexture2D() const { return aPolyTexture2D; }

    virtual sal_uInt16 GetObjIdentifier() const;
    virtual SdrObject* DoConvertToPolyObj(sal_Bool bBezier, bool bAddText) const;

    virtual E3dPolygonObj* Clone() const;

    // LineOnly?
    sal_Bool GetLineOnly() { return bLineOnly; }
    void SetLineOnly(sal_Bool bNew);
};

#endif          // _E3D_POLYGON3D_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
