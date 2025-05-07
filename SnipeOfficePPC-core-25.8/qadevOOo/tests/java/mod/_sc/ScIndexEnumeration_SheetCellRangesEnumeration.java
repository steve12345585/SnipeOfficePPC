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

package mod._sc;

import java.io.PrintWriter;

import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.SOfficeFactory;

import com.sun.star.container.XEnumerationAccess;
import com.sun.star.container.XIndexAccess;
import com.sun.star.container.XNameContainer;
import com.sun.star.lang.XComponent;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.sheet.XSpreadsheet;
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.sheet.XSpreadsheets;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.Type;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;

public class ScIndexEnumeration_SheetCellRangesEnumeration extends TestCase {
    static XSpreadsheetDocument xSheetDoc = null;

    /**
    * Creates Spreadsheet document.
    */
    protected void initialize( TestParameters tParam, PrintWriter log ) {

        // get a soffice factory object
        SOfficeFactory SOF = SOfficeFactory.getFactory( (XMultiServiceFactory)tParam.getMSF());

        try {
            log.println( "creating a sheetdocument" );
            xSheetDoc = SOF.createCalcDoc(null);;
        } catch (com.sun.star.uno.Exception e) {
            // Some exception occures.FAILED
            e.printStackTrace( log );
            throw new StatusException( "Couldn't create document", e );
        }
    }

    /**
    * Disposes Spreadsheet document.
    */
    protected void cleanup( TestParameters tParam, PrintWriter log ) {
        log.println( "    disposing xSheetDoc " );
        XComponent oComp =
            (XComponent) UnoRuntime.queryInterface (XComponent.class, xSheetDoc);
        util.DesktopTools.closeDoc(oComp);
    }

    protected synchronized TestEnvironment createTestEnvironment(TestParameters Param, PrintWriter log) {

        XInterface oObj = null;
        Object oRange = null ;

        // creation of testobject here
        // first we write what we are intend to do to log file
        log.println( "Creating a test environment" );

        // get a soffice factory object
        SOfficeFactory SOF = SOfficeFactory.getFactory( (XMultiServiceFactory)Param.getMSF() );

        log.println("Getting test object ");

        XComponent oComp = (XComponent)
                    UnoRuntime.queryInterface (XComponent.class, xSheetDoc);

        oObj = (XInterface)
            SOF.createInstance(oComp, "com.sun.star.sheet.SheetCellRanges");

        XSpreadsheets oSheets = xSheetDoc.getSheets() ;
        XIndexAccess oIndSheets = (XIndexAccess)
            UnoRuntime.queryInterface (XIndexAccess.class, oSheets);
        XSpreadsheet oSheet = null;
        try {
            oSheet = (XSpreadsheet) AnyConverter.toObject(
                    new Type(XSpreadsheet.class),oIndSheets.getByIndex(0));
            XNameContainer oRanges = (XNameContainer)
                UnoRuntime.queryInterface(XNameContainer.class, oObj);

            oRange = oSheet.getCellRangeByName("C1:D4");
            oRanges.insertByName("Range1", oRange);
            oRange = oSheet.getCellRangeByName("E2:F5");
            oRanges.insertByName("Range2", oRange);
            oRange = oSheet.getCellRangeByName("G2:H3");
            oRanges.insertByName("Range3", oRange);
            oRange = oSheet.getCellRangeByName("I7:J8");
            oRanges.insertByName("Range4", oRange);
        } catch(com.sun.star.lang.WrappedTargetException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't create test object", e);
        } catch(com.sun.star.lang.IndexOutOfBoundsException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't create test object", e);
        } catch(com.sun.star.container.ElementExistException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't create test object", e);
        } catch(com.sun.star.lang.IllegalArgumentException e) {
            e.printStackTrace(log);
            throw new StatusException("Couldn't create test object", e);
        }

        log.println("filling some cells");
        try {
            for (int i = 0; i < 10; i++) {
                for (int j = 0; j < 5; j++) {
                    oSheet.getCellByPosition(i, j).setFormula("a");
                }
            }
            for (int i = 0; i < 10; i++) {
                for (int j = 5; j < 10; j++) {
                    oSheet.getCellByPosition(i, j).setValue(i + j);
                }
            }
        } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
            e.printStackTrace(log);
            throw new StatusException (
                "Exception occurred while filling cells", e);
        }

        XEnumerationAccess ea = (XEnumerationAccess)
                    UnoRuntime.queryInterface(XEnumerationAccess.class,oObj);

        oObj = ea.createEnumeration();

        log.println("ImplementationName: "+util.utils.getImplName(oObj));
        // creating test environment
        TestEnvironment tEnv = new TestEnvironment( oObj );

        tEnv.addObjRelation("ENUM",ea);

        return tEnv ;
    }

}

