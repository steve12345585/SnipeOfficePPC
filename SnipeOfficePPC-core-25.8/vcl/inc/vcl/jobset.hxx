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

#ifndef _SV_JOBSET_HXX
#define _SV_JOBSET_HXX

#include <vcl/sv.h>
#include <vcl/dllapi.h>
#include <vcl/prntypes.hxx>

namespace rtl
{
    class OUString;
}

class SvStream;
struct ImplJobSetup;

// ------------
// - JobSetup -
// ------------

class VCL_DLLPUBLIC JobSetup
{
    friend class Printer;

private:
    ImplJobSetup*       mpData;

public:
    SAL_DLLPRIVATE ImplJobSetup*        ImplGetData();
    SAL_DLLPRIVATE ImplJobSetup*        ImplGetConstData();
    SAL_DLLPRIVATE const ImplJobSetup*  ImplGetConstData() const;

public:
                        JobSetup();
                        JobSetup( const JobSetup& rJob );
                        ~JobSetup();

    rtl::OUString              GetPrinterName() const;
    rtl::OUString              GetDriverName() const;

    /*  Get/SetValue are used to read/store additional
     *  Parameters in the job setup that may be used
     *  by the printer driver. One possible use are phone
     *  numbers for faxes (which disguise as printers)
     */
    void                SetValue( const rtl::OUString& rKey, const rtl::OUString& rValue );

    JobSetup&           operator=( const JobSetup& rJob );

    sal_Bool                operator==( const JobSetup& rJobSetup ) const;
    sal_Bool                operator!=( const JobSetup& rJobSetup ) const
                            { return !(JobSetup::operator==( rJobSetup )); }

    friend VCL_DLLPUBLIC SvStream&  operator>>( SvStream& rIStream, JobSetup& rJobSetup );
    friend VCL_DLLPUBLIC SvStream&  operator<<( SvStream& rOStream, const JobSetup& rJobSetup );
};

#endif  // _SV_JOBSET_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
