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




// INCLUDE ---------------------------------------------------------------
#include "XMLColumnRowGroupExport.hxx"
#include "xmlexprt.hxx"
#include <xmloff/nmspmap.hxx>
#include <xmloff/xmltoken.hxx>
#include <xmloff/xmlnmspe.hxx>

#include <algorithm>

using namespace xmloff::token;

ScMyColumnRowGroup::ScMyColumnRowGroup()
{
}

bool ScMyColumnRowGroup::operator<(const ScMyColumnRowGroup& rGroup) const
{
    if (rGroup.nField > nField)
        return true;
    else
        if (rGroup.nField == nField && rGroup.nLevel > nLevel)
            return true;
        else
            return false;
}

ScMyOpenCloseColumnRowGroup::ScMyOpenCloseColumnRowGroup(ScXMLExport& rTempExport, sal_uInt32 nToken)
    : rExport(rTempExport),
    rName(rExport.GetNamespaceMap().GetQNameByKey(XML_NAMESPACE_TABLE, GetXMLToken(XMLTokenEnum(nToken)))),
    aTableStart(),
    aTableEnd()
{
}

ScMyOpenCloseColumnRowGroup::~ScMyOpenCloseColumnRowGroup()
{
}

void ScMyOpenCloseColumnRowGroup::NewTable()
{
    aTableStart.clear();
    aTableEnd.clear();
}

void ScMyOpenCloseColumnRowGroup::AddGroup(const ScMyColumnRowGroup& aGroup, const sal_Int32 nEndField)
{
    aTableStart.push_back(aGroup);
    aTableEnd.push_back(nEndField);
}

bool ScMyOpenCloseColumnRowGroup::IsGroupStart(const sal_Int32 nField)
{
    bool bGroupStart(false);
    if (!aTableStart.empty())
    {
        ScMyColumnRowGroupVec::iterator aItr(aTableStart.begin());
        sal_Int32 nItrField = aItr->nField;
        if ( nItrField < nField )
        {
            //  when used to find repeated rows at the beginning of a group,
            //  aTableStart may contain entries before nField. They must be skipped here
            //  (they will be used for OpenGroups later in the right order).

            ScMyColumnRowGroupVec::iterator aEnd(aTableStart.end());
            while ( aItr != aEnd && nItrField < nField )
            {
                ++aItr;
                if ( aItr != aEnd )
                    nItrField = aItr->nField;
            }
        }

        if (nItrField == nField)
            bGroupStart = true;
    }
    return bGroupStart;
}

void ScMyOpenCloseColumnRowGroup::OpenGroup(const ScMyColumnRowGroup& rGroup)
{
    if (!rGroup.bDisplay)
        rExport.AddAttribute(XML_NAMESPACE_TABLE, XML_DISPLAY, XML_FALSE);
    rExport.StartElement( rName, true);
}

void ScMyOpenCloseColumnRowGroup::OpenGroups(const sal_Int32 nField)
{
    ScMyColumnRowGroupVec::iterator aItr(aTableStart.begin());
    ScMyColumnRowGroupVec::iterator aEndItr(aTableStart.end());
    bool bReady(false);
    while(!bReady && aItr != aEndItr)
    {
        if (aItr->nField == nField)
        {
            OpenGroup(*aItr);
            aItr = aTableStart.erase(aItr);
        }
        else
            bReady = true;
    }
}

bool ScMyOpenCloseColumnRowGroup::IsGroupEnd(const sal_Int32 nField)
{
    bool bGroupEnd(false);
    if (!aTableEnd.empty())
    {
        if (*(aTableEnd.begin()) == nField)
            bGroupEnd = true;
    }
    return bGroupEnd;
}

void ScMyOpenCloseColumnRowGroup::CloseGroup()
{
    rExport.EndElement( rName, true );
}

void ScMyOpenCloseColumnRowGroup::CloseGroups(const sal_Int32 nField)
{
    ScMyFieldGroupVec::iterator aItr(aTableEnd.begin());
    ScMyFieldGroupVec::iterator aEndItr(aTableEnd.end());
    bool bReady(false);
    while(!bReady && aItr != aEndItr)
    {
        if (*aItr == nField)
        {
            CloseGroup();
            aItr = aTableEnd.erase(aItr);
        }
        else
            bReady = true;
    }
}

sal_Int32 ScMyOpenCloseColumnRowGroup::GetLast()
{
    sal_Int32 maximum(-1);
    ScMyFieldGroupVec::iterator i(aTableEnd.begin());
    ScMyFieldGroupVec::iterator endi(aTableEnd.end());
    while (i != endi)
    {
        if (*i > maximum)
            maximum = *i;
        ++i;
    }
    return maximum;
}

void ScMyOpenCloseColumnRowGroup::Sort()
{
    aTableStart.sort();
    aTableEnd.sort();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
