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
#ifndef SW_UNDO_FIELD_HXX
#define SW_UNDO_FIELD_HXX

#include <undobj.hxx>

#include <com/sun/star/uno/Any.h>


class SwDoc;
class SwField;
class SwMsgPoolItem;

class SwUndoField : public SwUndo
{
    sal_uLong nNodeIndex;
    xub_StrLen nOffset;

protected:
    SwDoc * pDoc;
    SwPosition GetPosition();

public:
    SwUndoField(const SwPosition & rPos, SwUndoId nId = UNDO_FIELD );
    virtual ~SwUndoField();
};

class SwUndoFieldFromDoc : public SwUndoField
{
    SwField * pOldField, * pNewField;
    SwMsgPoolItem * pHnt;
    sal_Bool bUpdate;

    void DoImpl();

public:
    SwUndoFieldFromDoc(const SwPosition & rPos, const SwField & aOldField,
                       const SwField & aNewField,
                       SwMsgPoolItem * pHnt, sal_Bool bUpdate,
                       SwUndoId nId = UNDO_FIELD );

    virtual ~SwUndoFieldFromDoc();

    virtual void UndoImpl( ::sw::UndoRedoContext & );
    virtual void RedoImpl( ::sw::UndoRedoContext & );
    virtual void RepeatImpl( ::sw::RepeatContext & );
};

class SwUndoFieldFromAPI : public SwUndoField
{
    com::sun::star::uno::Any aOldVal, aNewVal;
    sal_uInt16 nWhich;

    void DoImpl();

public:
    SwUndoFieldFromAPI(const SwPosition & rPos,
                       const com::sun::star::uno::Any & rOldVal,
                       const com::sun::star::uno::Any & rNewVal,
                       sal_uInt16 nWhich);
    virtual ~SwUndoFieldFromAPI();

    virtual void UndoImpl( ::sw::UndoRedoContext & );
    virtual void RedoImpl( ::sw::UndoRedoContext & );
    virtual void RepeatImpl( ::sw::RepeatContext & );
};

#endif // SW_UNDO_FIELD_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
