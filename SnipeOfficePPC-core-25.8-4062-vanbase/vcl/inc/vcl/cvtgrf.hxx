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

#ifndef _SV_CVTGRF_HXX
#define _SV_CVTGRF_HXX

#include <vcl/dllapi.h>
#include <tools/link.hxx>
#include <vcl/salctype.hxx>

// --------------------
// - GraphicConverter -
// --------------------

struct  ConvertData;
class   Graphic;

class VCL_DLLPUBLIC GraphicConverter
{
private:

    Link                maFilterHdl;
    ConvertData*        mpConvertData;

public:

                        GraphicConverter();
                        ~GraphicConverter();

    static sal_uLong        Import( SvStream& rIStm, Graphic& rGraphic, sal_uLong nFormat = CVT_UNKNOWN );
    static sal_uLong        Export( SvStream& rOStm, const Graphic& rGraphic, sal_uLong nFormat );

    ConvertData*        GetConvertData() { return mpConvertData; }

    void                SetFilterHdl( const Link& rLink ) { maFilterHdl = rLink; }
    const Link&         GetFilterHdl() const { return maFilterHdl; }
};

#endif // _SV_CVTGRF_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
