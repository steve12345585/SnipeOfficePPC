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
 * Date field with date style.
 ************************************************************************/
#ifndef     _XFDATE_HXX
#define     _XFDATE_HXX

#include    "xfglobal.hxx"
#include    "xfcontent.hxx"

/**
 * @descr   Date obejct.
 */
class XFDate : public XFContent
{
public:
    XFDate();

    virtual ~XFDate();

public:
    void    SetDate(XFDateTime& dt);

    void    SetDate(const rtl::OUString& date);

    void    SetText(rtl::OUString& text);

    void    SetFixed(sal_Bool fixed = sal_True);

    virtual void    ToXml(IXFStream *pStrm);

private:
    sal_Bool    m_bFixed;
    rtl::OUString m_strText;
    XFDateTime  m_aDateTime;
    rtl::OUString   m_strDate;
    sal_Bool    m_bValued;
};

inline  void XFDate::SetDate(XFDateTime& dt)
{
    m_aDateTime = dt;
    m_strDate = DateTimeToOUString(m_aDateTime);
    m_bValued = sal_True;
}

inline void XFDate::SetDate(const rtl::OUString& date)
{
    m_strDate = date;
    m_bValued = sal_True;
}

inline  void XFDate::SetText(rtl::OUString& text)
{
    m_strText = text;
}

inline  void XFDate::SetFixed(sal_Bool fixed)
{
    m_bFixed = fixed;
}

class XFDateStart : public XFDate
{
public:
    virtual void    ToXml(IXFStream *pStrm);
};

class XFDateEnd : public XFContent
{
public:
    virtual void    ToXml(IXFStream *pStrm);
};


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
