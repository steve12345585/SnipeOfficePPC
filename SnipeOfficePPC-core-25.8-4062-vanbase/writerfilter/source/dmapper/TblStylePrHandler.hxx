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

#ifndef INCLUDED_TBLSTYLEPRHANDLER_HXX
#define INCLUDED_TBLSTYLEPRHANDLER_HXX

#include "TablePropertiesHandler.hxx"

#include <dmapper/DomainMapper.hxx>
#include <WriterFilterDllApi.hxx>
#include <resourcemodel/LoggedResources.hxx>
#include <boost/shared_ptr.hpp>

namespace writerfilter {
namespace dmapper {

class DomainMapper;

enum TblStyleType
{
    TBL_STYLE_UNKNOWN,
    TBL_STYLE_WHOLETABLE,
    TBL_STYLE_FIRSTROW,
    TBL_STYLE_LASTROW,
    TBL_STYLE_FIRSTCOL,
    TBL_STYLE_LASTCOL,
    TBL_STYLE_BAND1VERT,
    TBL_STYLE_BAND2VERT,
    TBL_STYLE_BAND1HORZ,
    TBL_STYLE_BAND2HORZ,
    TBL_STYLE_NECELL,
    TBL_STYLE_NWCELL,
    TBL_STYLE_SECELL,
    TBL_STYLE_SWCELL
};

class WRITERFILTER_DLLPRIVATE TblStylePrHandler : public LoggedProperties
{
private:
    DomainMapper &              m_rDMapper;
    TablePropertiesHandler *    m_pTablePropsHandler;

    TblStyleType                m_nType;
    PropertyMapPtr              m_pProperties;

    // Properties
    virtual void lcl_attribute(Id Name, Value & val);
    virtual void lcl_sprm(Sprm & sprm);

public:
    TblStylePrHandler( DomainMapper & rDMapper );
    virtual ~TblStylePrHandler( );

    inline PropertyMapPtr       getProperties() { return m_pProperties; };
    inline TblStyleType         getType() { return m_nType; };

private:

    void resolveSprmProps(Sprm & rSprm);
};

typedef boost::shared_ptr< TblStylePrHandler > TblStylePrHandlerPtr;

}}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
