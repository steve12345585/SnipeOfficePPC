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

#ifndef _SV_ACCEL_HXX
#define _SV_ACCEL_HXX

#include <vcl/sv.h>
#include <vcl/dllapi.h>
#include <tools/link.hxx>
#include <tools/resid.hxx>
#include <tools/rc.hxx>
#include <vcl/keycod.hxx>

class ImplAccelData;
class ImplAccelEntry;

// ---------------
// - Accelerator -
// ---------------

class VCL_DLLPUBLIC Accelerator : public Resource
{
    friend class ImplAccelManager;

private:
    ImplAccelData*  mpData;
    XubString       maHelpStr;
    Link            maActivateHdl;
    Link            maDeactivateHdl;
    Link            maSelectHdl;

    // Werden vom AcceleratorManager gesetzt
    KeyCode         maCurKeyCode;
    sal_uInt16          mnCurId;
    sal_uInt16          mnCurRepeat;
    sal_Bool            mbIsCancel;
    sal_Bool*           mpDel;

    SAL_DLLPRIVATE  void        ImplInit();
    SAL_DLLPRIVATE  void        ImplCopyData( ImplAccelData& rAccelData );
    SAL_DLLPRIVATE  void        ImplDeleteData();
    SAL_DLLPRIVATE  void        ImplInsertAccel( sal_uInt16 nItemId, const KeyCode& rKeyCode,
                                     sal_Bool bEnable, Accelerator* pAutoAccel );

    SAL_DLLPRIVATE  ImplAccelEntry* ImplGetAccelData( const KeyCode& rKeyCode ) const;

protected:
    SAL_DLLPRIVATE  void        ImplLoadRes( const ResId& rResId );

public:
                    Accelerator();
                    Accelerator( const Accelerator& rAccel );
                    Accelerator( const ResId& rResId );
    virtual         ~Accelerator();

    virtual void    Activate();
    virtual void    Deactivate();
    virtual void    Select();

    void            InsertItem( sal_uInt16 nItemId, const KeyCode& rKeyCode );
    void            InsertItem( const ResId& rResId );

    sal_uInt16          GetCurItemId() const { return mnCurId; }
    const KeyCode&  GetCurKeyCode() const { return maCurKeyCode; }
    sal_uInt16          GetCurRepeat() const { return mnCurRepeat; }
    sal_Bool            IsCancel() const { return mbIsCancel; }

    sal_uInt16          GetItemCount() const;
    sal_uInt16          GetItemId( sal_uInt16 nPos ) const;
    KeyCode         GetKeyCode( sal_uInt16 nItemId ) const;

    Accelerator*    GetAccel( sal_uInt16 nItemId ) const;

    void            SetHelpText( const XubString& rHelpText ) { maHelpStr = rHelpText; }
    const XubString& GetHelpText() const { return maHelpStr; }

    void            SetActivateHdl( const Link& rLink ) { maActivateHdl = rLink; }
    const Link&     GetActivateHdl() const { return maActivateHdl; }
    void            SetDeactivateHdl( const Link& rLink ) { maDeactivateHdl = rLink; }
    const Link&     GetDeactivateHdl() const { return maDeactivateHdl; }
    void            SetSelectHdl( const Link& rLink ) { maSelectHdl = rLink; }
    const Link&     GetSelectHdl() const { return maSelectHdl; }

    Accelerator&    operator=( const Accelerator& rAccel );
};

#endif  // _SV_ACCEL_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
