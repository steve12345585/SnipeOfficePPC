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

#ifndef _E3D_UNDO_HXX
#define _E3D_UNDO_HXX

#include <svx/svdundo.hxx>
#include <svx/scene3d.hxx>
#include "svx/svxdllapi.h"

/************************************************************************\
|*
|* Base class for all 3D undo actions.
|*
\************************************************************************/
class E3dUndoAction : public SdrUndoAction
{

    protected :
        E3dObject *pMy3DObj;

    public:
        TYPEINFO();
        E3dUndoAction (SdrModel  *pModel,
                       E3dObject *p3DObj) :
            SdrUndoAction (*pModel),
            pMy3DObj (p3DObj)
            {
            }

        virtual ~E3dUndoAction ();

        virtual sal_Bool CanRepeat(SfxRepeatTarget&) const;
};

/************************************************************************\
|*
|* Undo for 3D rotation through the rotation matrices
|*
\************************************************************************/
class E3dRotateUndoAction : public E3dUndoAction
{
        basegfx::B3DHomMatrix aMyOldRotation;
        basegfx::B3DHomMatrix aMyNewRotation;

    public:
        TYPEINFO();
        E3dRotateUndoAction (SdrModel       *pModel,
                             E3dObject      *p3DObj,
                             const basegfx::B3DHomMatrix &aOldRotation,
                             const basegfx::B3DHomMatrix &aNewRotation) :
            E3dUndoAction (pModel, p3DObj),
            aMyOldRotation (aOldRotation),
            aMyNewRotation (aNewRotation)
            {
            }

        virtual ~E3dRotateUndoAction ();

        virtual void Undo();
        virtual void Redo();

};

/************************************************************************\
|*
|* Undo for 3D attributes (implemented using Set3DAttributes())
|*
\************************************************************************/
class SVX_DLLPUBLIC E3dAttributesUndoAction : public SdrUndoAction
{
    using SdrUndoAction::Repeat;

    SdrObject*  pObject;

    const SfxItemSet aNewSet;
    const SfxItemSet aOldSet;

 public:
        TYPEINFO();
        E3dAttributesUndoAction( SdrModel &rModel,
            E3dObject* pInObject,
            const SfxItemSet& rNewSet,
            const SfxItemSet& rOldSet);

        virtual ~E3dAttributesUndoAction();

        virtual sal_Bool CanRepeat(SfxRepeatTarget& rView) const;
        virtual void Undo();
        virtual void Redo();
        virtual void Repeat();
};

#endif          // _E3D_CUBE3D_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
