/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/***************************************************************************
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

#include "altstrfunc.hxx"

#include <sal/types.h>

int start(const std::string &s1, const std::string &s2){
    size_t i;
    int ret = 0;

    size_t min = s1.length();
    if (min > s2.length())
        min = s2.length();

    for(i = 0; i < min && s2[i] && s1[i] && !ret; i++){
        ret = toupper(s1[i]) - toupper(s2[i]);
        if(s1[i] == '.' || s2[i] == '.'){ret = 0;}//. is a neutral character
    }
    return ret;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
