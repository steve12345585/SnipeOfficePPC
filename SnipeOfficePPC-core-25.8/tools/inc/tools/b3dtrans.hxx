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

#ifndef _B3D_B3DTRANS_HXX
#define _B3D_B3DTRANS_HXX

// Zu verwendender DephRange des Z-Buffers
#define ZBUFFER_DEPTH_RANGE         ((double)(256L * 256L * 256L))

#include <basegfx/matrix/b3dhommatrix.hxx>
#include <basegfx/range/b3drange.hxx>
#include <tools/gen.hxx>
#include <basegfx/matrix/b2dhommatrix.hxx>
#include <basegfx/point/b2dpoint.hxx>
#include <tools/toolsdllapi.h>

// Vorausdeklarationen

/*************************************************************************
|*
|* Unterstuetzte Methoden, um das Seitenverhaeltnis einzuhalten
|*
\************************************************************************/

enum Base3DRatio
{
    Base3DRatioGrow = 1,
    Base3DRatioShrink,
    Base3DRatioMiddle
};

/*************************************************************************
|*
|* Typ der Projektion
|*
\************************************************************************/

enum Base3DProjectionType
{
    Base3DProjectionTypeParallel = 1,
    Base3DProjectionTypePerspective
};

/*************************************************************************
|*
|* Transformationen fuer alle 3D Ausgaben
|*
\************************************************************************/

class TOOLS_DLLPUBLIC B3dTransformationSet
{
private:
    // Object Matrix Object -> World
    basegfx::B3DHomMatrix           maObjectTrans;
    basegfx::B3DHomMatrix           maInvObjectTrans;

    // Orientation Matrix
    basegfx::B3DHomMatrix           maOrientation;
    basegfx::B3DHomMatrix           maInvOrientation;

    // Projection Matrix
    basegfx::B3DHomMatrix           maProjection;
    basegfx::B3DHomMatrix           maInvProjection;

    // Texture Matrices
    basegfx::B2DHomMatrix           maTexture;

    // Speziell zum Umwandeln von Punkten Objekt -> Device
    basegfx::B3DHomMatrix           maObjectToDevice;

    // Transponierte Inverse fuer Vectortransformationen
    basegfx::B3DHomMatrix           maInvTransObjectToEye;

    // Transformation World->View
    basegfx::B3DHomMatrix           maMatFromWorldToView;
    basegfx::B3DHomMatrix           maInvMatFromWorldToView;

    // Parameters for ViewportTransformation
    basegfx::B3DVector          maScale;
    basegfx::B3DVector          maTranslate;

    // ViewPlane DeviceRectangle (vom Benutzer gesetzt)
    double                          mfLeftBound;
    double                          mfRightBound;
    double                          mfBottomBound;
    double                          mfTopBound;

    // Near and far clipping planes
    double                          mfNearBound;
    double                          mfFarBound;

    // Seitenverhaeltnis der 3D Abbildung (Y / X)
    // default ist 1:1 -> 1.0
    // Deaktivieren mit 0.0 als Wert
    double                          mfRatio;

    // Der gesetzte Ausgabebereich (in logischen Koordinaten)
    // und der dazugehoerige sichtbare Bereich
    Rectangle                       maViewportRectangle;
    Rectangle                       maVisibleRectangle;

    // Die tatsaechlich von CalcViewport gesetzten Abmessungen
    // des sichtbaren Bereichs (in logischen Koordinaten)
    Rectangle                       maSetBound;

    // Methode zur Aufrechterhaltung des Seitenverhaeltnisses
    // default ist Base3DRatioGrow
    Base3DRatio                     meRatio;

    // Flags
    unsigned                        mbPerspective               : 1;
    unsigned                        mbWorldToViewValid          : 1;
    unsigned                        mbInvTransObjectToEyeValid  : 1;
    unsigned                        mbObjectToDeviceValid       : 1;
    unsigned                        mbProjectionValid           : 1;

public:
    B3dTransformationSet();
    virtual ~B3dTransformationSet();

    // Zurueck auf Standard
    void Reset();

    // ObjectTrans
    const basegfx::B3DHomMatrix& GetObjectTrans() { return maObjectTrans; }
    const basegfx::B3DHomMatrix& GetInvObjectTrans() { return maInvObjectTrans; }

    // Orientation
    void SetOrientation(
        basegfx::B3DPoint aVRP = basegfx::B3DPoint(0.0,0.0,1.0),
        basegfx::B3DVector aVPN = basegfx::B3DVector(0.0,0.0,1.0),
        basegfx::B3DVector aVUP = basegfx::B3DVector(0.0,1.0,0.0));
    const basegfx::B3DHomMatrix& GetOrientation() { return maOrientation; }
    const basegfx::B3DHomMatrix& GetInvOrientation() { return maInvOrientation; }

    // Projection
    void SetProjection(const basegfx::B3DHomMatrix& mProject);
    const basegfx::B3DHomMatrix& GetProjection();

    // Texture
    const basegfx::B2DHomMatrix& GetTexture() { return maTexture; }

    // Seitenverhaeltnis und Modus zu dessen Aufrechterhaltung
    double GetRatio() { return mfRatio; }
    void SetRatio(double fNew=1.0);
    Base3DRatio GetRatioMode() { return meRatio; }

    // Parameter der ViewportTransformation
    void SetDeviceRectangle(double fL=-1.0, double fR=1.0, double fB=-1.0, double fT=1.0, sal_Bool bBroadCastChange=sal_True);
    double GetDeviceRectangleWidth() const { return mfRightBound - mfLeftBound; }
    double GetDeviceRectangleHeight() const { return mfTopBound - mfBottomBound; }
    double GetFrontClippingPlane() { return mfNearBound; }
    double GetBackClippingPlane() { return mfFarBound; }
    void SetPerspective(sal_Bool bNew);
    sal_Bool GetPerspective() { return mbPerspective; }
    void SetViewportRectangle(Rectangle& rRect, Rectangle& rVisible);
    void SetViewportRectangle(Rectangle& rRect) { SetViewportRectangle(rRect, rRect); }
    const Rectangle& GetViewportRectangle() { return maViewportRectangle; }
    void CalcViewport();

    // Direkter Zugriff auf verschiedene Transformationen
    const basegfx::B3DPoint WorldToEyeCoor(const basegfx::B3DPoint& rVec);
    const basegfx::B3DPoint EyeToWorldCoor(const basegfx::B3DPoint& rVec);

    static void Frustum(
        basegfx::B3DHomMatrix& rTarget,
        double fLeft = -1.0, double fRight = 1.0,
        double fBottom = -1.0, double fTop = 1.0,
        double fNear = 0.001, double fFar = 1.0);
    static void Ortho(
        basegfx::B3DHomMatrix& rTarget,
        double fLeft = -1.0, double fRight = 1.0,
        double fBottom = -1.0, double fTop = 1.0,
        double fNear = 0.0, double fFar = 1.0);
    static void Orientation(
        basegfx::B3DHomMatrix& rTarget,
        basegfx::B3DPoint aVRP = basegfx::B3DPoint(0.0,0.0,1.0),
        basegfx::B3DVector aVPN = basegfx::B3DVector(0.0,0.0,1.0),
        basegfx::B3DVector aVUP = basegfx::B3DVector(0.0,1.0,0.0));

protected:
    void PostSetObjectTrans();
    void PostSetOrientation();
    void PostSetProjection();
    void PostSetViewport();

    virtual void DeviceRectangleChange();
};

/*************************************************************************
|*
|* Viewport fuer B3D
|*
|* Verwendet wird hier ein vereinfachtes System, bei dem der abzubildende
|* Punkt durch VRP repraesentiert wird
|*
\************************************************************************/

class TOOLS_DLLPUBLIC B3dViewport : public B3dTransformationSet
{
private:
    basegfx::B3DPoint               aVRP;           // View Reference Point
    basegfx::B3DVector          aVPN;           // View Plane Normal
    basegfx::B3DVector          aVUV;           // View Up Vector

public:
    B3dViewport();
    virtual ~B3dViewport();

    void SetVUV(const basegfx::B3DVector& rNewVUV);
    void SetViewportValues(
        const basegfx::B3DPoint& rNewVRP,
        const basegfx::B3DVector& rNewVPN,
        const basegfx::B3DVector& rNewVUV);

    const basegfx::B3DPoint&    GetVRP() const  { return aVRP; }
    const basegfx::B3DVector&   GetVPN() const  { return aVPN; }
    const basegfx::B3DVector&   GetVUV() const  { return aVUV; }

protected:
    void CalcOrientation();
};

/*************************************************************************
|*
|* Kamera fuer B3D
|*
\************************************************************************/

class TOOLS_DLLPUBLIC B3dCamera : public B3dViewport
{
private:
    basegfx::B3DPoint       aPosition;
    basegfx::B3DPoint       aCorrectedPosition;
    basegfx::B3DVector  aLookAt;
    double                  fFocalLength;
    double                  fBankAngle;

    unsigned                bUseFocalLength         : 1;

public:
    B3dCamera(
        const basegfx::B3DPoint& rPos = basegfx::B3DPoint(0.0, 0.0, 1.0),
        const basegfx::B3DVector& rLkAt = basegfx::B3DVector(0.0, 0.0, 0.0),
        double fFocLen = 35.0, double fBnkAng = 0.0,
        sal_Bool bUseFocLen = sal_False);
    virtual ~B3dCamera();

    // Positionen
    const basegfx::B3DPoint& GetPosition() const { return aPosition; }
    const basegfx::B3DVector& GetLookAt() const { return aLookAt; }

    // Brennweite in mm
    double GetFocalLength() const { return fFocalLength; }

    // Neigung links/rechts
    double GetBankAngle() const { return fBankAngle; }

    // FocalLength Flag
    sal_Bool GetUseFocalLength() const { return (sal_Bool)bUseFocalLength; }

protected:
    void CalcNewViewportValues();
    sal_Bool CalcFocalLength();

    virtual void DeviceRectangleChange();
};


#endif          // _B3D_B3DTRANS_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
