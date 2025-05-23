/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: IBM Corporation
 *
 *  Copyright: 2008 by IBM Corporation
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/
/*************************************************************************
 * @file
 * Draw path object.
 ************************************************************************/
#ifndef     _XFDRAWPATH_HXX
#define     _XFDRAWPATH_HXX

#include    "xfdrawobj.hxx"
#include    <vector>

/**
 * @brief
 * SVG path segment wrapper.
 */
class XFSvgPathEntry
{
public:
    XFSvgPathEntry();

public:
    /**
     * @descr   Set svg path command,L for line,M for move,...
     */
    void    SetCommand(rtl::OUString cmd);

    /**
     * @descr   Set svg path point.
     */
    void    AddPoint(XFPoint pt);

    rtl::OUString ToString();

    friend class XFDrawPath;
private:
    rtl::OUString   m_strCommand;
    std::vector<XFPoint>    m_aPoints;
};

/**
 * @brief
 * For svg path wrapper.
 */
class XFDrawPath : public XFDrawObject
{
public:
    XFDrawPath();

public:
    /**
     * @descr   Move command.
     */
    void    MoveTo(XFPoint pt, sal_Bool absPosition = sal_True);

    /**
     * @descr   Line command.
     */
    void    LineTo(XFPoint pt, sal_Bool absPosition = sal_True);

    /**
     * @descr   Curve command.
     */
    void    CurveTo(XFPoint dest, XFPoint ctrl1, XFPoint ctrl2, sal_Bool absPosition = sal_True);

    /**
     * @descr   Close path command.
     */
    void    ClosePath(sal_Bool absPosition = sal_True);

    virtual void    ToXml(IXFStream *pStrm);

private:
    std::vector<XFSvgPathEntry> m_aPaths;
};

inline void XFSvgPathEntry::SetCommand(rtl::OUString cmd)
{
    m_strCommand = cmd;
}

inline void XFSvgPathEntry::AddPoint(XFPoint pt)
{
    m_aPoints.push_back(pt);
}


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
