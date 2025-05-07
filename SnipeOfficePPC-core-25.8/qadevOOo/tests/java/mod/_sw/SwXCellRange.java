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

package mod._sw;

import java.io.PrintWriter;

import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;
import util.SOfficeFactory;

import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.table.XCellRange;
import com.sun.star.text.XTextDocument;
import com.sun.star.text.XTextTable;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;

/**
 * Test for object which is represented by service
 * <code>com.sun.star.table.CellRange</code>. <p>
 * Object implements the following interfaces :
 * <ul>
 *  <li> <code>com::sun::star::text::CellProperties</code></li>
 *  <li> <code>com::sun::star::beans::XPropertySet</code></li>
 * </ul> <p>
 * This object test <b> is NOT </b> designed to be run in several
 * threads concurently.
 * @see com.sun.star.table.CellRange
 * @see com.sun.star.text.CellProperties
 * @see com.sun.star.beans.XPropertySet
 * @see ifc.text._CellProperties
 * @see ifc.beans._XPropertySet
 */
public class SwXCellRange extends TestCase {
    SOfficeFactory SOF;
    XTextDocument xTextDoc;

    /**
    * Creates text document.
    */
    protected void initialize( TestParameters tParam, PrintWriter log ) {
        SOF = SOfficeFactory.getFactory( (XMultiServiceFactory)tParam.getMSF() );
        try {
            log.println( "creating a textdocument" );
            xTextDoc = SOF.createTextDoc( null );
        } catch ( com.sun.star.uno.Exception e ) {
            e.printStackTrace( log );
            throw new StatusException( "Couldn't create document", e );
        }
    }

    /**
    * Disposes text document.
    */
    protected void cleanup( TestParameters tParam, PrintWriter log ) {
        log.println( "    disposing xTextDoc " );
        util.DesktopTools.closeDoc(xTextDoc);
    }

    /**
    * Creating a Testenvironment for the interfaces to be tested. At first
    * method creates and initializes the table, then if text document has no
    * tables, previously created table is inserted to text document. Finally,
    * custom cell range is gotten from the table created.
    *     Object relations created :
    * <ul>
    *  <li> <code>'CellProperties.TextSection'</code> for
    *    {@link ifc.text._CellProperties} : range of complete paragraphs
    *  within a text</li>
    * </ul>
    */
    protected synchronized TestEnvironment createTestEnvironment
            (TestParameters tParam, PrintWriter log) {

        XTextTable the_table = null;
        XInterface oObj = null;
        try {
            the_table = SOF.createTextTable( xTextDoc );
            the_table.initialize(5, 5);
        } catch ( com.sun.star.uno.Exception e ) {
            e.printStackTrace( log );
            throw new StatusException("Couldn't create testobj: "
                +e.getMessage(),e);
        }

        if( SOF.getTableCollection( xTextDoc ).getCount() == 0 ) {
            try {
                SOF.insertTextContent(xTextDoc, the_table );
            } catch( com.sun.star.uno.Exception e ) {
                e.printStackTrace( log );
                throw new StatusException("Couldn't create TextTable : "
                        + e.getMessage(), e);
            }
        }
        try {
            XCellRange the_Range = (XCellRange)
                    UnoRuntime.queryInterface(XCellRange.class, the_table);
            oObj = the_Range.getCellRangeByPosition(0, 0, 3, 4);
        } catch (com.sun.star.uno.Exception e) {
            e.printStackTrace( log );
            throw new StatusException("Couldn't get CellRange : "
                    + e.getMessage(), e);
        }

        log.println("Creating instance...");
        TestEnvironment tEnv = new TestEnvironment( oObj );
        log.println("ImplName: " + util.utils.getImplName(oObj));

        XMultiServiceFactory oDocMSF = (XMultiServiceFactory)
            UnoRuntime.queryInterface(XMultiServiceFactory.class, xTextDoc);
        try {
            XInterface oTS = (XInterface)
                oDocMSF.createInstance("com.sun.star.text.TextSection");
            log.println("  adding TextSection object");
            tEnv.addObjRelation("CellProperties.TextSection", oTS);
        } catch (com.sun.star.uno.Exception e) {
            log.println("Could not get instance of TextSection");
            e.printStackTrace(log);
        }

        Object[][] NewData = new Object[5][];
        NewData[0] = new Double[]
            {new Double(2.5),new Double(5),new Double(2.5),new Double(5)};
        NewData[1] = new Double[]
            {new Double(4),new Double(9),new Double(2.5),new Double(5)};
        NewData[2] = new Double[]
            {new Double(2.5),new Double(5),new Double(2.5),new Double(5)};
        NewData[3] = new Double[]
            {new Double(2.5),new Double(5),new Double(2.5),new Double(5)};
        NewData[4] = new Double[]
            {new Double(4),new Double(9),new Double(2.5),new Double(5)};
        tEnv.addObjRelation("NewData",NewData);

        // com::sun::star::chart::XChartDataArray
        tEnv.addObjRelation("CRDESC",
                 "Column and RowDescriptions can't be changed for this Object");


        return tEnv;
    }
}    // finish class SwXCellRange

