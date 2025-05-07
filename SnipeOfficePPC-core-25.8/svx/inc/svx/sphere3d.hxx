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

#ifndef _E3D_SPHERE3D_HXX
#define _E3D_SPHERE3D_HXX

#include <svx/obj3d.hxx>
#include "svx/svxdllapi.h"

/*************************************************************************
|*
|* Kugelobjekt mit Durchmesser r3DSize; Anzahl der Flaechen wird durch
|* die horizontale und vertikale Segmentanzahl vorgegeben
|*
\************************************************************************/

class SVX_DLLPUBLIC E3dSphereObj : public E3dCompoundObject
{
private:
    basegfx::B3DPoint               aCenter;
    basegfx::B3DVector              aSize;

protected:
    virtual sdr::contact::ViewContact* CreateObjectSpecificViewContact();
    virtual sdr::properties::BaseProperties* CreateObjectSpecificProperties();
    void SetDefaultAttributes(E3dDefaultAttributes& rDefault);

public:
    TYPEINFO();
    E3dSphereObj(E3dDefaultAttributes& rDefault, const basegfx::B3DPoint& rCenter, const basegfx::B3DVector& r3DSize);

    // FG: This constructor is only called from MakeObject from the 3d-Objectfactory
    //     when a document with a sphere is loaded.  This constructor does not call
    //     CreateSphere, or create any spheres.
    E3dSphereObj(int dummy);

    // horizontal segments:
    sal_uInt32 GetHorizontalSegments() const
        { return ((const Svx3DHorizontalSegmentsItem&)GetObjectItemSet().Get(SDRATTR_3DOBJ_HORZ_SEGS)).GetValue(); }

    // VerticalSegments:
    sal_uInt32 GetVerticalSegments() const
        { return ((const Svx3DVerticalSegmentsItem&)GetObjectItemSet().Get(SDRATTR_3DOBJ_VERT_SEGS)).GetValue(); }

    virtual sal_uInt16 GetObjIdentifier() const;
    virtual SdrObject* DoConvertToPolyObj(sal_Bool bBezier) const;

    virtual E3dSphereObj* Clone() const;

    const basegfx::B3DPoint& Center() const { return aCenter; }
    const basegfx::B3DVector& Size() const { return aSize; }

    // set local parameters when the geometry is recreated
    void SetCenter(const basegfx::B3DPoint& rNew);
    void SetSize(const basegfx::B3DVector& rNew);

    // TakeObjName...() is for displaying in the UI, eg "3 selected frames."
    virtual void TakeObjNameSingul(String& rName) const;
    virtual void TakeObjNamePlural(String& rName) const;
};

#endif          // _E3D_SPHERE3D_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
