/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Version: MPL 1.1 / GPLv3+ / LGPLv3+
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License or as specified alternatively below. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * Major Contributor(s):
 * [ Copyright (C) 2012 Red Hat, Inc., Michael Stahl <mstahl@redhat.com> (initial developer) ]
 *
 * All Rights Reserved.
 *
 * For minor contributions see the git repository.
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPLv3+"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
 * in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
 * instead of those above.
 */

package complex.writer;

import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XServiceInfo;
import com.sun.star.beans.XPropertySet;
import com.sun.star.beans.PropertyValue;
import com.sun.star.container.XEnumeration;
import com.sun.star.util.XCloseable;
import com.sun.star.text.XText;
import com.sun.star.text.XTextContent;
import com.sun.star.text.XTextDocument;
import com.sun.star.text.XTextField;
import com.sun.star.text.XTextFieldsSupplier;
import com.sun.star.text.XTextRange;
import com.sun.star.text.XTextCursor;

import org.openoffice.test.OfficeConnection;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;

import java.util.Set;
import java.util.HashSet;


public class CheckFields
{
    private static final OfficeConnection connection = new OfficeConnection();

    @BeforeClass public static void setUpConnection() throws Exception {
        connection.setUp();
//Thread.sleep(5000);
    }

    @AfterClass public static void tearDownConnection()
        throws InterruptedException, com.sun.star.uno.Exception
    {
        connection.tearDown();
    }

    private XMultiServiceFactory m_xMSF = null;
    private XComponentContext m_xContext = null;
    private XTextDocument m_xDoc = null;

    @Before public void before() throws Exception
    {
        m_xMSF = UnoRuntime.queryInterface(
            XMultiServiceFactory.class,
            connection.getComponentContext().getServiceManager());
        m_xContext = connection.getComponentContext();
        assertNotNull("could not get component context.", m_xContext);
        m_xDoc = util.WriterTools.createTextDoc(m_xMSF);
    }

    @After public void after()
    {
        util.DesktopTools.closeDoc(m_xDoc);
    }

    @Test
    public void test_fdo39694_load() throws Exception
    {
        PropertyValue[] loadProps = new PropertyValue[2];
        loadProps[0] = new PropertyValue();
        loadProps[0].Name = "AsTemplate";
        loadProps[0].Value = new Boolean(true);
        loadProps[1] = new PropertyValue();
        loadProps[1].Name = "Hidden";
        loadProps[1].Value = new Boolean(true);

        String file = TestDocument.getUrl("fdo39694.ott");
        XComponent xComp = util.DesktopTools.loadDoc(m_xMSF, file, loadProps);
        XTextDocument xTextDoc = (XTextDocument)
            UnoRuntime.queryInterface(XTextDocument.class, xComp);

        XTextFieldsSupplier xTFS = (XTextFieldsSupplier)
            UnoRuntime.queryInterface(XTextFieldsSupplier.class, xComp);
        XEnumeration xEnum = xTFS.getTextFields().createEnumeration();
        Set<String> placeholders = new HashSet<String>();
        placeholders.add("<Kadr1>");
        placeholders.add("<Kadr2>");
        placeholders.add("<Kadr3>");
        placeholders.add("<Kadr4>");
        placeholders.add("<Pnname>");
        placeholders.add("<Pvname>");
        placeholders.add("<Pgeboren>");

        while (xEnum.hasMoreElements())
        {
            Object xField = xEnum.nextElement();
            XServiceInfo xSI = (XServiceInfo)
                UnoRuntime.queryInterface(XServiceInfo.class, xField);
            if (xSI.supportsService("com.sun.star.text.TextField.JumpEdit"))
            {
                XTextContent xContent = (XTextContent)
                    UnoRuntime.queryInterface(XTextContent.class, xField);
                XTextRange xAnchor = xContent.getAnchor();
                String readContent = xAnchor.getString();
                if (!placeholders.remove(readContent))
                {
                    System.out.println("field not found: " + readContent);
                    assertFalse(true);
                }
            }
        }
        assertTrue(placeholders.isEmpty());
        XCloseable xClos = (XCloseable) UnoRuntime.queryInterface(
                    XCloseable.class, xComp);
        if (xClos != null) {
            xClos.close(true);
        }
    }

    @Test
    public void test_fdo42073() throws Exception
    {
        XMultiServiceFactory xDocFactory = (XMultiServiceFactory)
            UnoRuntime.queryInterface(XMultiServiceFactory.class, m_xDoc);
        Object xField =
            xDocFactory.createInstance("com.sun.star.text.TextField.Input");

        XText xBodyText = m_xDoc.getText();
        XTextCursor xCursor = xBodyText.createTextCursor();
        XTextField xTextField = (XTextField)
            UnoRuntime.queryInterface(XTextField.class, xField);
        xBodyText.insertTextContent(xCursor, xTextField, true);

        XPropertySet xPropSet = (XPropertySet)
            UnoRuntime.queryInterface(XPropertySet.class, xField);
        {
            Object readContent = xPropSet.getPropertyValue("Content");
            assertEquals("", readContent);
        }

        String content = "this is not surprising";
        xPropSet.setPropertyValue("Content", content);
        Object readContent = xPropSet.getPropertyValue("Content");
        assertEquals(content, readContent);
        readContent = xTextField.getPresentation(false);
        assertEquals(content, readContent);
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
