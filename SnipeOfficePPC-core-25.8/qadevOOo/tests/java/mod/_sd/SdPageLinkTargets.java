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

package mod._sd;

import java.io.PrintWriter;

import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.DrawTools;
import util.SOfficeFactory;

import com.sun.star.beans.XPropertySet;
import com.sun.star.document.XLinkTargetSupplier;
import com.sun.star.drawing.XDrawPage;
import com.sun.star.drawing.XShape;
import com.sun.star.drawing.XShapes;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;

/**
* Test for object which is represented by service
* <code>com.sun.star.document.LinkTargets</code>. <p>
* Object implements the following interfaces :
* <ul>
*  <li> <code>com::sun::star::container::XNameAccess</code></li>
*  <li> <code>com::sun::star::container::XElementAccess</code></li>
* </ul>
* @see com.sun.star.document.LinkTargets
* @see com.sun.star.container.XNameAccess
* @see com.sun.star.container.XElementAccess
* @see ifc.container._XNameAccess
* @see ifc.container._XElementAccess
*/
public class SdPageLinkTargets extends TestCase {
    XComponent xDrawDoc;

    /**
    * Creates Drawing document.
    */
    protected void initialize(TestParameters Param, PrintWriter log) {
       // get a soffice factory object
        SOfficeFactory SOF = SOfficeFactory.getFactory(
                                    (XMultiServiceFactory)Param.getMSF());

        try {
            log.println( "creating a draw document" );
            xDrawDoc = SOF.createDrawDoc(null);
        } catch (com.sun.star.uno.Exception e) {
            // Some exception occures.FAILED
            e.printStackTrace( log );
            throw new StatusException( "Couldn't create document", e );
        }
    }

    /**
    * Disposes Drawing document.
    */
    protected void cleanup( TestParameters Param, PrintWriter log) {
        log.println("disposing xDrawDoc");
        util.DesktopTools.closeDoc(xDrawDoc);;
    }

    /**
    * Creating a Testenvironment for the interfaces to be tested.
    * Retrieves the collection of draw pages and take one of them.
    * Obtains the collection of possible links using the interface
    * <code>XLinkTargetSupplier</code>. The obtained collection is
    * the instance of the service <code>com.sun.star.document.LinkTargets</code>.
    * Inserts some shapes into the document.
    * @see com.sun.star.document.XLinkTargetSupplier
    * @see com.sun.star.document.LinkTargets
    */
    public synchronized TestEnvironment createTestEnvironment(
        TestParameters Param, PrintWriter log) throws StatusException {

        // creation of testobject here
        // first we write what we are intend to do to log file
        log.println( "creating a test environment" );

        // create testobject here
        XDrawPage the_page = DrawTools.getDrawPage(xDrawDoc, 0);
        XLinkTargetSupplier oLTS = (XLinkTargetSupplier)
            UnoRuntime.queryInterface(XLinkTargetSupplier.class, the_page);
        XInterface oObj = oLTS.getLinks();

        SOfficeFactory SOF = SOfficeFactory.getFactory(
                                    (XMultiServiceFactory)Param.getMSF());
        log.println( "inserting some Shapes" );
        XShapes oShapes = (XShapes)
            UnoRuntime.queryInterface(XShapes.class,the_page);
        XShape oShape =
            SOF.createShape(xDrawDoc, 15000, 13500, 5000, 5000, "OLE2");
        oShapes.add(oShape);

        XPropertySet shape_props = (XPropertySet)
                        UnoRuntime.queryInterface(XPropertySet.class,oShape);

        log.println("Inserting a Chart");

        try {
            shape_props.
                setPropertyValue("CLSID","12DCAE26-281F-416F-a234-c3086127382e");
        } catch (com.sun.star.lang.WrappedTargetException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't change property", e);
        } catch (com.sun.star.lang.IllegalArgumentException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't change property", e);
        } catch (com.sun.star.beans.PropertyVetoException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't change property", e);
        } catch (com.sun.star.beans.UnknownPropertyException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't change property", e);
        }

        log.println( "creating a new environment for LinkTargets object" );
        TestEnvironment tEnv = new TestEnvironment( oObj );

        return tEnv;
    } // finish method createTestEnvironment

} // finish class SdPageLinkTargets

