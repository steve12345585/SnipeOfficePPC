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
package com.sun.star.report.pentaho.parser.rpt;

import com.sun.star.report.pentaho.OfficeNamespaces;
import com.sun.star.report.pentaho.parser.ElementReadHandler;
import com.sun.star.report.pentaho.parser.table.TableReadHandler;

import org.jfree.report.structure.Element;
import org.jfree.report.structure.Section;

import org.pentaho.reporting.libraries.xmlns.parser.XmlReadHandler;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;

public class RootTableReadHandler extends ElementReadHandler
{

    private TableReadHandler sectionTableReadHandler;
    private final Section section;

    public RootTableReadHandler()
    {
        section = new Section();
    }

    protected RootTableReadHandler(final Section section)
    {
        if (section == null)
        {
            throw new NullPointerException();
        }
        this.section = section;
    }

    /**
     * Returns the handler for a child element.
     *
     * @param tagName the tag name.
     * @param atts    the attributes.
     * @return the handler or null, if the tagname is invalid.
     *
     * @throws org.xml.sax.SAXException if there is a parsing error.
     */
    protected XmlReadHandler getHandlerForChild(final String uri,
            final String tagName,
            final Attributes atts)
            throws SAXException
    {
        if (OfficeNamespaces.TABLE_NS.equals(uri) && "table".equals(tagName))
        {
            sectionTableReadHandler = new TableReadHandler();
            return sectionTableReadHandler;
        }
        if (OfficeNamespaces.OOREPORT_NS.equals(uri) && "conditional-print-expression".equals(tagName))
        {
            return new ConditionalPrintExpressionReadHandler(section);
        }
        return null;
    }

    /**
     * Done parsing.
     *
     * @throws org.xml.sax.SAXException if there is a parsing error.
     */
    protected void doneParsing()
            throws SAXException
    {
        if (sectionTableReadHandler != null)
        {
            section.addNode(sectionTableReadHandler.getElement());
        }
    }

    public Element getElement()
    {
        return section;
    }
}
