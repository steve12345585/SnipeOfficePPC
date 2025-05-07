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

#include <precomp.h>
#include <x_docu.hxx>

// NOT FULLY DECLARED SERVICES



X_Docu::X_Docu( const char *        i_tag,
                const char *        i_explanation  )
    :   sTagName(i_tag),
        sExplanation(i_explanation)
{
}

X_Docu::~X_Docu()
{
}

X_Docu::E_Event
X_Docu::GetEvent() const
{
     return x_Any;
}

void
X_Docu::GetInfo( std::ostream & o_rOutputMedium ) const
{
    o_rOutputMedium
        << "Error in tag '"
        << sTagName
        << "': "
        << sExplanation
        << Endl();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
