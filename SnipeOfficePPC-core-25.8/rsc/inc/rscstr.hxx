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
#ifndef _RSCSTR_HXX
#define _RSCSTR_HXX

#include <rscall.h>
#include <rscerror.h>
#include <rschash.hxx>
#include <rsctop.hxx>

/******************* R s c S t r i n g ***********************************/
class RscString : public RscTop
{
    RscTop * pRefClass;
    struct RscStringInst {
        char *  pStr;   // Zeiger auf String
        sal_Bool    bDflt;  // Ist Default
        RscId   aRefId; // ReferenzName
    };
    sal_uInt32  nSize;
public:
                    RscString( Atom nId, sal_uInt32 nTypId );
    virtual RSCCLASS_TYPE   GetClassType() const;

    void            SetRefClass( RscTop * pClass )
    {
                        pRefClass = pClass;
                    };
    RSCINST         Create( RSCINST * pInst, const RSCINST & rDfltInst, sal_Bool );
                    // Der zulaessige Bereich wird gesetzt
    void            Destroy( const RSCINST & rInst );
    sal_uInt32          Size(){ return nSize; }
    void            SetToDefault( const RSCINST & rInst )
                    {
                        ((RscStringInst*)rInst.pData)->bDflt = sal_True;
                    }
    sal_Bool            IsDefault( const RSCINST & rInst)
                    {
                        return( ((RscStringInst*)rInst.pData)->bDflt );
                    };
                    // Als Default setzen
    sal_Bool            IsValueDefault( const RSCINST & rInst, CLASS_DATA pDef );
    ERRTYPE         SetString( const RSCINST &, const char * pStr );
    ERRTYPE         GetString( const RSCINST &, char ** ppStr );
    ERRTYPE         GetRef( const RSCINST & rInst, RscId * );
    ERRTYPE         SetRef( const RSCINST & rInst, const RscId & rRefId );
    void            WriteSrc( const RSCINST &, FILE * fOutput,
                              RscTypCont * pTC, sal_uInt32 nTab, const char * );
    ERRTYPE         WriteRc( const RSCINST &, RscWriteRc & aMem,
                             RscTypCont * pTC, sal_uInt32, sal_Bool bExtra );
    virtual void    WriteRcAccess( FILE * fOutput, RscTypCont * pTC,
                                    const char * );
};

#endif // _RSCSTR_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
