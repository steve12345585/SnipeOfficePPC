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
#include <s2_luidl/tk_ident.hxx>


// NOT FULLY DECLARED SERVICES
#include <s2_luidl/tokintpr.hxx>



namespace csi
{
namespace uidl
{

void
TokIdentifier::Trigger( TokenInterpreter &  io_rInterpreter ) const
{
    io_rInterpreter.Process_Identifier(*this);
}

const char *
TokIdentifier::Text() const
{
    return sText;
}

void
TokNameSeparator::Trigger( TokenInterpreter &   io_rInterpreter ) const
{
    io_rInterpreter.Process_NameSeparator();
}

const char *
TokNameSeparator::Text() const
{
    return "::";
}

}   // namespace uidl
}   // namespace csi

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
