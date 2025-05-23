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
 * Tool for sax writer.
 ************************************************************************/
#include    "xfsaxstream.hxx"
#include    "xfsaxattrlist.hxx"

#include <cppuhelper/implbase1.hxx>
#include <cppuhelper/implbase3.hxx>
#include <cppuhelper/factory.hxx>

#include <com/sun/star/document/XFilter.hpp>
#include <com/sun/star/document/XImporter.hpp>
#include <com/sun/star/xml/sax/XDocumentHandler.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/io/XInputStream.hpp>

using namespace ::rtl;
using namespace ::cppu;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::io;
using namespace ::com::sun::star::registry;
using namespace ::com::sun::star::document;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::xml::sax;
using namespace ::com::sun::star::uno;

XFSaxStream::XFSaxStream(Reference<XDocumentHandler>& xHandler)
{
    m_aHandler = xHandler;
    m_pAttrList = new XFSaxAttrList();
}

XFSaxStream::~XFSaxStream()
{
    if( m_pAttrList )
        delete m_pAttrList;
}

void        XFSaxStream::StartDocument()
{
    if (m_aHandler.is())
        m_aHandler->startDocument();
}

void        XFSaxStream::EndDocument()
{
    if (m_aHandler.is())
        m_aHandler->endDocument();
}

void        XFSaxStream::StartElement(const rtl::OUString& oustr)
{
    if (m_aHandler.is())
        m_aHandler->startElement( oustr, m_pAttrList->GetAttributeList() );
    m_pAttrList->Clear();
}

void        XFSaxStream::EndElement(const rtl::OUString& oustr)
{
    if (m_aHandler.is())
        m_aHandler->endElement(oustr);

    if( m_pAttrList )
        m_pAttrList->Clear();
}

void        XFSaxStream::Characters(const rtl::OUString& oustr)
{
    if (m_aHandler.is())
        m_aHandler->characters(oustr);
}

IXFAttrList*    XFSaxStream::GetAttrList()
{
    return m_pAttrList;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
