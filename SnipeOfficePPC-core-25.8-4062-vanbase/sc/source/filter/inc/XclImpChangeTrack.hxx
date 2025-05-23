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

#ifndef SC_XCLIMPCHANGETRACK_HXX
#define SC_XCLIMPCHANGETRACK_HXX

#include <tools/datetime.hxx>
#include "xiroot.hxx"
#include "xistream.hxx"
#include "excform.hxx"
#include "imp_op.hxx"


//___________________________________________________________________

class ScBaseCell;
class ScChangeAction;
class ScChangeTrack;
class XclImpChTrFmlConverter;

//___________________________________________________________________

struct XclImpChTrRecHeader
{
    sal_uInt32                  nSize;
    sal_uInt32                  nIndex;
    sal_uInt16                  nOpCode;
    sal_uInt16                  nAccept;
};

inline XclImpStream& operator>>( XclImpStream& rStrm, XclImpChTrRecHeader& rRecHeader )
{
    rStrm >> rRecHeader.nSize >> rRecHeader.nIndex >> rRecHeader.nOpCode >> rRecHeader.nAccept;
    return rStrm;
}

//___________________________________________________________________

class XclImpChangeTrack : protected XclImpRoot
{
private:
    XclImpChTrRecHeader         aRecHeader;
    String                      sOldUsername;

    ScChangeTrack*              pChangeTrack;
    SotStorageStreamRef          xInStrm;        // input stream
    XclImpStream*               pStrm;          // stream import class
    sal_uInt16                  nTabIdCount;
    sal_Bool                    bGlobExit;      // global exit loop

    enum { nmBase, nmFound, nmNested }
                                eNestedMode;    // action with nested content actions

    inline sal_Bool             FoundNestedMode() { return eNestedMode == nmFound; }

    void                        DoAcceptRejectAction( ScChangeAction* pAction );
    void                        DoAcceptRejectAction( sal_uInt32 nFirst, sal_uInt32 nLast );

    void                        DoInsertRange( const ScRange& rRange );
    void                        DoDeleteRange( const ScRange& rRange );

    inline sal_uInt8            LookAtuInt8();
    inline double               ReadRK();
    inline sal_Bool             ReadBool();
    inline void                 Read2DAddress( ScAddress& rAddress );
    inline void                 Read2DRange( ScRange& rRange );
    SCTAB                       ReadTabNum();
    void                        ReadDateTime( DateTime& rDateTime );

    inline void                 ReadString( String& rString );

    sal_Bool                    CheckRecord( sal_uInt16 nOpCode );

    void                        ReadFormula(
                                    ScTokenArray*& rpTokenArray,
                                    const ScAddress& rPosition );
    void                        ReadCell(
                                    ScBaseCell*& rpCell,
                                    sal_uInt32& rFormat,
                                    sal_uInt16 nFlags,
                                    const ScAddress& rPosition );

    void                        ReadChTrInsert();           // 0x0137
    void                        ReadChTrInfo();             // 0x0138
    void                        ReadChTrCellContent();      // 0x013B
    void                        ReadChTrTabId();            // 0x013D
    void                        ReadChTrMoveRange();        // 0x0140
    void                        ReadChTrInsertTab();        // 0x014D
    void                        InitNestedMode();           // 0x014E, 0x0150
    void                        ReadNestedRecords();
    sal_Bool                    EndNestedMode();            // 0x014F, 0x0151

    void                        ReadRecords();

public:
                                XclImpChangeTrack( const XclImpRoot& rRoot, const XclImpStream& rBookStrm );
                                ~XclImpChangeTrack();

                                // reads extended 3D ref info following the formulas, returns sc tab nums
                                // ( called by XclImpChTrFmlConverter::Read3DTabReference() )
    sal_Bool                    Read3DTabRefInfo( SCTAB& rFirstTab, SCTAB& rLastTab, ExcelToSc8::ExternalTabInfo& rExtInfo );

    void                        Apply();
};

inline sal_uInt8 XclImpChangeTrack::LookAtuInt8()
{
    pStrm->PushPosition();
    sal_uInt8 nValue;
    *pStrm >> nValue;
    pStrm->PopPosition();
    return nValue;
}

inline double XclImpChangeTrack::ReadRK()
{
    return XclTools::GetDoubleFromRK( pStrm->ReadInt32() );
}

inline sal_Bool XclImpChangeTrack::ReadBool()
{
    return (pStrm->ReaduInt16() != 0);
}

inline void XclImpChangeTrack::Read2DAddress( ScAddress& rAddress )
{
    rAddress.SetRow( static_cast<SCROW>(pStrm->ReaduInt16()) );
    rAddress.SetCol( static_cast<SCCOL>(pStrm->ReaduInt16()) );
}

inline void XclImpChangeTrack::Read2DRange( ScRange& rRange )
{
    rRange.aStart.SetRow( static_cast<SCROW>(pStrm->ReaduInt16()) );
    rRange.aEnd.SetRow( static_cast<SCROW>(pStrm->ReaduInt16()) );
    rRange.aStart.SetCol( static_cast<SCCOL>(pStrm->ReaduInt16()) );
    rRange.aEnd.SetCol( static_cast<SCCOL>(pStrm->ReaduInt16()) );
}

inline void XclImpChangeTrack::ReadString( String& rString )
{
    rString = pStrm->ReadUniString();
}

//___________________________________________________________________
// derived class for special 3D ref handling

class XclImpChTrFmlConverter : public ExcelToSc8
{
private:
    XclImpChangeTrack&          rChangeTrack;

    virtual bool                Read3DTabReference( sal_uInt16 nIxti, SCTAB& rFirstTab, SCTAB& rLastTab, ExternalTabInfo& rExtInfo );

public:
    inline                      XclImpChTrFmlConverter(
                                    const XclImpRoot& rRoot,
                                    XclImpChangeTrack& rXclChTr );
    virtual                     ~XclImpChTrFmlConverter();
};

inline XclImpChTrFmlConverter::XclImpChTrFmlConverter(
        const XclImpRoot& rRoot,
        XclImpChangeTrack& rXclChTr ) :
    ExcelToSc8( rRoot ),
    rChangeTrack( rXclChTr )
{
}

//___________________________________________________________________

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
