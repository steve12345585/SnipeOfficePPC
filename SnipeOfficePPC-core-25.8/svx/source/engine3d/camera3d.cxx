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

#include <svx/camera3d.hxx>
#include <tools/stream.hxx>

Camera3D::Camera3D(const basegfx::B3DPoint& rPos, const basegfx::B3DPoint& rLookAt,
                   double fFocalLen, double fBankAng) :
    aResetPos(rPos),
    aResetLookAt(rLookAt),
    fResetFocalLength(fFocalLen),
    fResetBankAngle(fBankAng),
    fBankAngle(fBankAng),
    bAutoAdjustProjection(sal_True)
{
    SetVPD(0);
    SetPosition(rPos);
    SetLookAt(rLookAt);
    SetFocalLength(fFocalLen);
}

Camera3D::Camera3D()
{
    basegfx::B3DPoint aVector3D(0.0 ,0.0 ,1.0);
    Camera3D(aVector3D, basegfx::B3DPoint());
}

// Set default values for reset

void Camera3D::SetDefaults(const basegfx::B3DPoint& rPos, const basegfx::B3DPoint& rLookAt,
                            double fFocalLen, double fBankAng)
{
    aResetPos           = rPos;
    aResetLookAt        = rLookAt;
    fResetFocalLength   = fFocalLen;
    fResetBankAngle     = fBankAng;
}

// Set ViewWindow and adjust PRP

void Camera3D::SetViewWindow(double fX, double fY, double fW, double fH)
{
    Viewport3D::SetViewWindow(fX, fY, fW, fH);
    if ( bAutoAdjustProjection )
        SetFocalLength(fFocalLength);
}

void Camera3D::SetPosition(const basegfx::B3DPoint& rNewPos)
{
    if ( rNewPos != aPosition )
    {
        aPosition = rNewPos;
        SetVRP(aPosition);
        SetVPN(aPosition - aLookAt);
        SetBankAngle(fBankAngle);
    }
}

void Camera3D::SetLookAt(const basegfx::B3DPoint& rNewLookAt)
{
    if ( rNewLookAt != aLookAt )
    {
        aLookAt = rNewLookAt;
        SetVPN(aPosition - aLookAt);
        SetBankAngle(fBankAngle);
    }
}

void Camera3D::SetPosAndLookAt(const basegfx::B3DPoint& rNewPos,
                               const basegfx::B3DPoint& rNewLookAt)
{
    if ( rNewPos != aPosition || rNewLookAt != aLookAt )
    {
        aPosition = rNewPos;
        aLookAt = rNewLookAt;

        SetVRP(aPosition);
        SetVPN(aPosition - aLookAt);
        SetBankAngle(fBankAngle);
    }
}

void Camera3D::SetBankAngle(double fAngle)
{
    basegfx::B3DVector aDiff(aPosition - aLookAt);
    basegfx::B3DVector aPrj(aDiff);
    fBankAngle = fAngle;

    if ( aDiff.getY() == 0 )
    {
        aPrj.setY(-1.0);
    }
    else
    {   // aPrj = Projection from aDiff on the XZ-plane
        aPrj.setY(0.0);

        if ( aDiff.getY() < 0.0 )
        {
            aPrj = -aPrj;
        }
    }

    // Calculate from aDiff to uppwards pointing View-Up-Vector
    aPrj = aPrj.getPerpendicular(aDiff);
    aPrj = aPrj.getPerpendicular(aDiff);
    aDiff.normalize();

    // Rotate on Z axis, to rotate the BankAngle and back
    basegfx::B3DHomMatrix aTf;
    const double fV(sqrt(aDiff.getY() * aDiff.getY() + aDiff.getZ() * aDiff.getZ()));

    if ( fV != 0.0 )
    {
        basegfx::B3DHomMatrix aTemp;
        const double fSin(aDiff.getY() / fV);
        const double fCos(aDiff.getZ() / fV);

        aTemp.set(1, 1, fCos);
        aTemp.set(2, 2, fCos);
        aTemp.set(2, 1, fSin);
        aTemp.set(1, 2, -fSin);

        aTf *= aTemp;
    }

    {
        basegfx::B3DHomMatrix aTemp;
        const double fSin(-aDiff.getX());
        const double fCos(fV);

        aTemp.set(0, 0, fCos);
        aTemp.set(2, 2, fCos);
        aTemp.set(0, 2, fSin);
        aTemp.set(2, 0, -fSin);

        aTf *= aTemp;
    }

    aTf.rotate(0.0, 0.0, fBankAngle);

    {
        basegfx::B3DHomMatrix aTemp;
        const double fSin(aDiff.getX());
        const double fCos(fV);

        aTemp.set(0, 0, fCos);
        aTemp.set(2, 2, fCos);
        aTemp.set(0, 2, fSin);
        aTemp.set(2, 0, -fSin);

        aTf *= aTemp;
    }

    if ( fV != 0.0 )
    {
        basegfx::B3DHomMatrix aTemp;
        const double fSin(-aDiff.getY() / fV);
        const double fCos(aDiff.getZ() / fV);

        aTemp.set(1, 1, fCos);
        aTemp.set(2, 2, fCos);
        aTemp.set(2, 1, fSin);
        aTemp.set(1, 2, -fSin);

        aTf *= aTemp;
    }

    SetVUV(aTf * aPrj);
}

void Camera3D::SetFocalLength(double fLen)
{
    if ( fLen < 5 )
        fLen = 5;
    SetPRP(basegfx::B3DPoint(0.0, 0.0, fLen / 35.0 * aViewWin.W));
    fFocalLength = fLen;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
