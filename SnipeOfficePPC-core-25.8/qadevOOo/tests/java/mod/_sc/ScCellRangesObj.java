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

import com.sun.star.beans.XPropertySet;
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
import ifc.sheet._XCellRangesQuery;


/**
* Test for object which is represented by service
* <code>com.sun.star.sheet.SheetCellRanges</code>. <p>
* Object implements the following interfaces :
* <ul>
*  <li> <code>com::sun::star::table::CellProperties</code></li>
*  <li> <code>com::sun::star::container::XNameReplace</code></li>
*  <li> <code>com::sun::star::container::XNameContainer</code></li>
*  <li> <code>com::sun::star::beans::XPropertySet</code></li>
*  <li> <code>com::sun::star::container::XIndexAccess</code></li>
*  <li> <code>com::sun::star::container::XElementAccess</code></li>
*  <li> <code>com::sun::star::container::XEnumerationAccess</code></li>
*  <li> <code>com::sun::star::sheet::XSheetCellRangeContainer</code></li>
*  <li> <code>com::sun::star::sheet::XSheetOperation</code></li>
*  <li> <code>com::sun::star::sheet::XSheetCellRanges</code></li>
*  <li> <code>com::sun::star::container::XNameAccess</code></li>
* </ul>
* @see com.sun.star.sheet.SheetCellRanges
* @see com.sun.star.table.CellProperties
* @see com.sun.star.container.XNameReplace
* @see com.sun.star.container.XNameContainer
* @see com.sun.star.beans.XPropertySet
* @see com.sun.star.container.XIndexAccess
* @see com.sun.star.container.XElementAccess
* @see com.sun.star.container.XEnumerationAccess
* @see com.sun.star.sheet.XSheetCellRangeContainer
* @see com.sun.star.sheet.XSheetOperation
* @see com.sun.star.sheet.XSheetCellRanges
* @see com.sun.star.container.XNameAccess
* @see ifc.table._CellProperties
* @see ifc.container._XNameReplace
* @see ifc.container._XNameContainer
* @see ifc.beans._XPropertySet
* @see ifc.container._XIndexAccess
* @see ifc.container._XElementAccess
* @see ifc.container._XEnumerationAccess
* @see ifc.sheet._XSheetCellRangeContainer
* @see ifc.sheet._XSheetOperation
* @see ifc.sheet._XSheetCellRanges
* @see ifc.container._XNameAccess
*/
public class ScCellRangesObj extends TestCase {
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

    /**
    * Creating a Testenvironment for the interfaces to be tested.
    * Creates an instance of the service
    * <code>com.sun.star.sheet.SheetCellRanges</code> and fills some cells.
    * Object relations created :
    * <ul>
    *  <li> <code>'INSTANCE1', ..., 'INSTANCEN'</code> for
    *      {@link ifc.container._XNameReplace},
    *      {@link ifc.container._XNameContainer} (type of
    *      <code>XCellRange</code>)</li>
    *  <li> <code>'THRCNT'</code> for
    *      {@link ifc.container._XNameReplace}(the number of the running threads
    *      that was retrieved from the method parameter <code>Param</code>)</li>
    *  <li> <code>'NameReplaceIndex'</code> for
    *      {@link ifc.container._XNameReplace} </li>
    * </ul>
    * @see com.sun.star.table.XCellRange
    */
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
                    new Type(XSpreadsheet.class), oIndSheets.getByIndex(0));
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


        TestEnvironment tEnv = new TestEnvironment( oObj );

        // NameReplaceIndex : _XNameReplace
        log.println( "adding NameReplaceIndex as mod relation to environment" );
        tEnv.addObjRelation("NameReplaceIndex", "0");

        // INSTANCEn : _XNameContainer; _XNameReplace
        log.println( "adding INSTANCEn as mod relation to environment" );

        int THRCNT = 1;
        if ((String)Param.get("THRCNT") != null) {
            THRCNT= Integer.parseInt((String)Param.get("THRCNT"));
        }
        int a = 0;
        int b = 0;
        for (int n = 1; n < (THRCNT + 1) ; n++) {
            a = n * 2;
            b = a + 1;
            oRange = oSheet.getCellRangeByName("A" + a + ":B" + b);
            log.println(
                "adding INSTANCE" + n + " as mod relation to environment" );

            tEnv.addObjRelation("INSTANCE" + n, oRange);
        }

        XPropertySet PropSet = (XPropertySet)
                    UnoRuntime.queryInterface(XPropertySet.class, oObj);
        tEnv.addObjRelation("PropSet",PropSet);
        tEnv.addObjRelation("SHEET", oSheet);
        // add expected results for the XCellRangesQuery interface test
        String[]expectedResults = new String[7];
        expectedResults[_XCellRangesQuery.QUERYCOLUMNDIFFERENCES] = "Sheet1.I7:J8";
        expectedResults[_XCellRangesQuery.QUERYCONTENTCELLS] = "";
        expectedResults[_XCellRangesQuery.QUERYEMPTYCELLS] = "";
        expectedResults[_XCellRangesQuery.QUERYFORMULACELLS] = "";
        expectedResults[_XCellRangesQuery.QUERYINTERSECTION] = "Sheet1.D4";
        expectedResults[_XCellRangesQuery.QUERYROWDIFFERENCES] = "Sheet1.I7:J8";
        expectedResults[_XCellRangesQuery.QUERYVISIBLECELLS] = "Sheet1.C2:D4"; // first range, first line invisible
        tEnv.addObjRelation("XCellRangesQuery.EXPECTEDRESULTS", expectedResults);

        // for XSearchable and XReplaceable interface test
        tEnv.addObjRelation("SEARCHSTRING", "15");

        // for XFormulaQuery interface test
        tEnv.addObjRelation("EXPECTEDDEPENDENTVALUES", new int[]{4,5,1,4});
        tEnv.addObjRelation("EXPECTEDPRECEDENTVALUES", new int[]{4,5,1,4});
        return tEnv ;
    }

}    // finish class ScCellRangesObj

