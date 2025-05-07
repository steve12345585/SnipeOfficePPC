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

#ifndef ADC_PARSER_FILECOLI_HXX
#define ADC_PARSER_FILECOLI_HXX


namespace autodoc
{

class FileCollector_Ifc
{
  public:
    // TYPES
    typedef StringVector::const_iterator const_iterator;

    enum E_SearchMode
    {
        flat,
        recursive
    };

    // LIFECYCLE
    virtual             ~FileCollector_Ifc() {}

    // OPERATIONS
    virtual uintt       AddFilesFrom(
                            const char *        i_sRootDir,
                            const char *        i_sFilter,
                            E_SearchMode        i_eSearchMode ) = 0;
    virtual uintt       AddFile(
                            const char *        i_sFilePath ) = 0;
    virtual void        EraseAll() = 0;

    // INQUIRY
    virtual const_iterator
                        Begin() const = 0;
    virtual const_iterator
                        End() const = 0;
    virtual uintt       Size() const = 0;
};


} // namespace autodoc


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
