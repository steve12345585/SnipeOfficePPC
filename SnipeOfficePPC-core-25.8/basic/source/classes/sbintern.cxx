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

#include <tools/shl.hxx>

#include "sbintern.hxx"
#include "sbunoobj.hxx"
#include "token.hxx"
#include "symtbl.hxx"
#include "parser.hxx"
#include "codegen.hxx"
#include <basic/basmgr.hxx>

SbiGlobals* GetSbData()
{
    SbiGlobals** pp = (SbiGlobals**) ::GetAppData( SHL_SBC );
    SbiGlobals* p = *pp;
    if( !p )
        p = *pp = new SbiGlobals;
    return p;
}

SbiGlobals::SbiGlobals()
{
    pInst = NULL;
    pMod  = NULL;
    pSbFac= NULL;
    pUnoFac = NULL;
    pTypeFac = NULL;
    pOLEFac = NULL;
    pCompMod = NULL; // JSM
    nInst = 0;
    nCode = 0;
    nLine = 0;
    nCol1 = nCol2 = 0;
    bCompiler = sal_False;
    bGlobalInitErr = sal_False;
    bRunInit = sal_False;
    pTransliterationWrapper = NULL;
    bBlockCompilerError = sal_False;
    pAppBasMgr = NULL;
    pMSOMacroRuntimLib = NULL;
}

SbiGlobals::~SbiGlobals()
{
    delete pSbFac;
    delete pUnoFac;
    delete pTransliterationWrapper;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
