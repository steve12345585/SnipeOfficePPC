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

#ifndef SD_CLIENT_HXX
#define SD_CLIENT_HXX


#include <sfx2/ipclient.hxx>
class SdrGrafObj;
class SdrOle2Obj;
class OutlinerParaObject;

namespace sd {

class ViewShell;

/*************************************************************************
|*
|* Client
|*
\************************************************************************/

class Client : public SfxInPlaceClient
{
    ViewShell*      mpViewShell;
    SdrOle2Obj*     pSdrOle2Obj;
    SdrGrafObj*     pSdrGrafObj;
    OutlinerParaObject* pOutlinerParaObj;

    virtual void    ObjectAreaChanged();
    virtual void    RequestNewObjectArea( Rectangle& );
    virtual void    ViewChanged();
    virtual void    MakeVisible();

public:
    Client (SdrOle2Obj* pObj, ViewShell* pSdViewShell, ::Window* pWindow);
    virtual ~Client (void);

    SdrOle2Obj*     GetSdrOle2Obj() const { return pSdrOle2Obj; }
    void            SetSdrGrafObj(SdrGrafObj* pObj) { pSdrGrafObj = pObj; }
    SdrGrafObj*     GetSdrGrafObj() const { return pSdrGrafObj; }
    void            SetOutlinerParaObj (OutlinerParaObject* pObj) { pOutlinerParaObj = pObj; }
    OutlinerParaObject*     GetOutlinerParaObject () const { return pOutlinerParaObj; }
};

} // end of namespace sd

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
