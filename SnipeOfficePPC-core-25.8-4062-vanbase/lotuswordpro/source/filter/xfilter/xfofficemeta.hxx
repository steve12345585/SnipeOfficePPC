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
 * Meta info for the full document.
 * You can see this by click: File->Properties.
 ************************************************************************/
#ifndef     _XFOFFICEMETA_HXX
#define     _XFOFFICEMETA_HXX

#include    "xfglobal.hxx"

class XFOfficeMeta : public IXFObject
{
public:
    XFOfficeMeta(){}
    virtual ~XFOfficeMeta(){}

    void    SetCreator(rtl::OUString creator);
    void    SetDescription(rtl::OUString dsr);
    void SetKeywords(rtl::OUString keywords);
    void SetCreationTime(rtl::OUString crtime);
    void SetLastTime(rtl::OUString lstime);
    void SetEditTime(rtl::OUString edtime);

    virtual void ToXml(IXFStream *pStream);
private:
    rtl::OUString   m_strGenerator;
    rtl::OUString   m_strTitle;
    rtl::OUString   m_strCreator;
    rtl::OUString   m_strDsr;
    rtl::OUString   m_strKeywords;
    rtl::OUString   m_strCrtime;
    rtl::OUString   m_strLstime;
    rtl::OUString   m_strEdtime;

};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
