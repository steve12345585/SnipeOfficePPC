/*
 * This file is part of the SnipeOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

package mod._sw;

import com.sun.star.beans.NamedValue;

import util.DBTools;
import util.utils;
import com.sun.star.beans.PropertyVetoException;
import com.sun.star.beans.UnknownPropertyException;
import com.sun.star.beans.XPropertySet;
import com.sun.star.container.NoSuchElementException;
import com.sun.star.container.XNameAccess;
import com.sun.star.lang.IllegalArgumentException;
import com.sun.star.lang.WrappedTargetException;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.sdb.CommandType;
import com.sun.star.sdbc.*;
import com.sun.star.sdbcx.XRowLocate;
import com.sun.star.task.XJob;
import com.sun.star.text.MailMergeType;
import com.sun.star.uno.Exception;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XInterface;
import java.io.PrintWriter;
import lib.StatusException;
import lib.TestCase;
import lib.TestEnvironment;
import lib.TestParameters;

/**
* Here <code>com.sun.star.text.MailMerge</code> service is tested.<p>
* @see com.sun.star.text.MailMerge
* @see com.sun.star.task.XJob
* @see com.sun.star.text.XMailMergeBroadcaster
*/
public class SwXMailMerge extends TestCase {

    public void initialize( TestParameters Param, PrintWriter log ) {
        if (! Param.containsKey("uniqueSuffix")){
            Param.put("uniqueSuffix", new Integer(0));
        }
    }

    /**
     * Creating a Testenvironment for the interfaces to be tested. <p>
     * Creates <code>MailMerge</code> service     * Object relations created :
    * <ul>
    *  <li> <code>'executeArgs'</code> for
    *    {@link ifc.text._XMailMergeBroadcaster} : NamedValue[]</li>
    *  <li> <code>'Job'</code> for
    *    {@link ifc.text._XMailMergeBroadcaster} : XJob</li>
    *  <li> <code>'XJobArgs'</code> for
    *    {@link ifc.task._XJob} : Object[]</li>
    * </ul>
     */
    protected synchronized TestEnvironment createTestEnvironment(TestParameters Param, PrintWriter log) {

        XInterface oObj = null;
        XInterface oRowSet = null;
        XJob Job = null;

        log.println(" instantiate MailMerge service");
        try {
            oObj = (XInterface) ( (XMultiServiceFactory) Param.getMSF()).createInstance
                ("com.sun.star.text.MailMerge");
        } catch (Exception e) {
            throw new StatusException("Can't create object environment", e) ;
        }

        // <set some variables>
        String cTestDoc = utils.getFullTestURL("MailMerge.sxw");
        //cMailMerge_DocumentURL = cTestDoc
        String cOutputURL = utils.getOfficeTemp( (XMultiServiceFactory) Param.getMSF());
        String cDataSourceName  = "Bibliography";
        String cDataCommand = "biblio";
        Object[] sel = new Object[2];
        sel[0] = new int[2];
        sel[1] = new int[5];
        Object[] myBookMarks = new Object[2];
        // </set some variables>

        // <create XResultSet>
        log.println("create a XResultSet");
        try {
            oRowSet = (XInterface) ( (XMultiServiceFactory) Param.getMSF()).createInstance
                ("com.sun.star.sdb.RowSet");
        } catch (Exception e) {
            throw new StatusException("Can't create com.sun.star.sdb.RowSet", e);
        }
        XPropertySet oRowSetProps = UnoRuntime.queryInterface(XPropertySet.class, oRowSet);
        XRowSet xRowSet = UnoRuntime.queryInterface(XRowSet.class, oRowSet);
        try {
            oRowSetProps.setPropertyValue("DataSourceName",cDataSourceName);
            oRowSetProps.setPropertyValue("Command",cDataCommand);
            oRowSetProps.setPropertyValue("CommandType", new Integer(CommandType.TABLE));
        } catch (UnknownPropertyException e) {
            throw new StatusException("Can't set properties on oRowSet", e);
        } catch (PropertyVetoException e) {
            throw new StatusException("Can't set properties on oRowSet", e);
        } catch (IllegalArgumentException e) {
            throw new StatusException("Can't set properties on oRowSet", e);
        } catch (WrappedTargetException e) {
            throw new StatusException("Can't set properties on oRowSet", e);
        }
        try {
            xRowSet.execute();
        } catch (SQLException e) {
            throw new StatusException("Can't execute oRowSet", e);
        }

        XResultSet oResultSet = UnoRuntime.queryInterface(XResultSet.class, oRowSet);




        // <create Bookmarks>
        log.println("create bookmarks");
        try {
            XRowLocate oRowLocate = UnoRuntime.queryInterface(
                                                  XRowLocate.class, oResultSet);
            oResultSet.first();
            myBookMarks[0] = oRowLocate.getBookmark();
            oResultSet.next();
            myBookMarks[1] = oRowLocate.getBookmark();
        } catch (SQLException e) {
            throw new StatusException("Cant get Bookmarks", e);
        }
        // </create Bookmarks>

        // <fill object with values>

        log.println("fill MailMerge with default connection");

        XPropertySet oObjProps = UnoRuntime.queryInterface(XPropertySet.class, oObj);
        try {
            oObjProps.setPropertyValue("ActiveConnection", getLocalXConnection(Param));
            oObjProps.setPropertyValue("DataSourceName", cDataSourceName);
            oObjProps.setPropertyValue("Command", cDataCommand);
            oObjProps.setPropertyValue("CommandType", new Integer(CommandType.TABLE));
            oObjProps.setPropertyValue("OutputType", new Short(MailMergeType.FILE));
            oObjProps.setPropertyValue("DocumentURL", cTestDoc);
            oObjProps.setPropertyValue("OutputURL", cOutputURL);
            oObjProps.setPropertyValue("FileNamePrefix", "Author");
            oObjProps.setPropertyValue("FileNameFromColumn", new Boolean(false));
            oObjProps.setPropertyValue("Selection", new Object[0]);

        } catch (UnknownPropertyException e) {
            throw new StatusException("Can't set properties on oObj", e);
        } catch (PropertyVetoException e) {
            throw new StatusException("Can't set properties on oObj", e);
        } catch (IllegalArgumentException e) {
            throw new StatusException("Can't set properties on oObj", e);
        } catch (WrappedTargetException e) {
            throw new StatusException("Can't set properties on oObj", e);
        }
        // </fill object with values>


        // <create object relations>
        Object[] vXJobArgs = new Object[4];
        NamedValue[] vXJobArg0 = new NamedValue[8];
        NamedValue[] vXJobArg1 = new NamedValue[7];
        NamedValue[] vXJobArg2 = new NamedValue[10];
        NamedValue[] vXJobArg3 = new NamedValue[0];

        // first Arguments
        vXJobArg0[0] = new NamedValue("DataSourceName", cDataSourceName);
        vXJobArg0[1] = new NamedValue("Command", cDataCommand);
        vXJobArg0[2] = new NamedValue("CommandType",new Integer(CommandType.TABLE));
        vXJobArg0[3] = new NamedValue("OutputType",new Short(MailMergeType.FILE));
        vXJobArg0[4] = new NamedValue("DocumentURL", cTestDoc);
        vXJobArg0[5] = new NamedValue("OutputURL", cOutputURL);
        vXJobArg0[6] = new NamedValue("FileNamePrefix", "Identifier");
        vXJobArg0[7] = new NamedValue("FileNameFromColumn", new Boolean(true));

        //second Arguments
        vXJobArg1[0] = new NamedValue("DataSourceName", cDataSourceName);
        vXJobArg1[1] = new NamedValue("Command", cDataCommand);
        vXJobArg1[2] = new NamedValue("CommandType",new Integer(CommandType.TABLE));
        vXJobArg1[3] = new NamedValue("OutputType",
                             new Short(MailMergeType.PRINTER));
        vXJobArg1[4] = new NamedValue("DocumentURL", cTestDoc);
        vXJobArg1[5] = new NamedValue("FileNamePrefix", "Author");
        vXJobArg1[6] = new NamedValue("FileNameFromColumn", new Boolean(true));

        // third Arguments
        vXJobArg2[0] = new NamedValue("ActiveConnection", getLocalXConnection(Param));
        vXJobArg2[1] = new NamedValue("DataSourceName", cDataSourceName);
        vXJobArg2[2] = new NamedValue("Command", cDataCommand);
        vXJobArg2[3] = new NamedValue("CommandType",new Integer(CommandType.TABLE));
        vXJobArg2[4] = new NamedValue("OutputType",
                             new Short(MailMergeType.FILE));
        vXJobArg2[5] = new NamedValue("ResultSet", oResultSet);
        vXJobArg2[6] = new NamedValue("OutputURL", cOutputURL);
        vXJobArg2[7] = new NamedValue("FileNamePrefix", "Identifier");
        vXJobArg2[8] = new NamedValue("FileNameFromColumn", new Boolean(true));
        vXJobArg2[9] = new NamedValue("Selection", myBookMarks);

        vXJobArgs[0] = vXJobArg0;
        vXJobArgs[1] = vXJobArg1;
        vXJobArgs[2] = vXJobArg2;
        vXJobArgs[3] = vXJobArg3;


        Job = UnoRuntime.queryInterface(XJob.class, oObj);
        try{
            Job.execute(vXJobArg2);
        } catch ( IllegalArgumentException e){
            System.out.println(e.toString());
        } catch (  Exception e){
            System.out.println(e.toString());
        }


        // <create XResultSet>
        log.println("create XResultSet");

        try {
            oRowSet = (XInterface) ( (XMultiServiceFactory) Param.getMSF()).createInstance
                ("com.sun.star.sdb.RowSet");
        } catch (Exception e) {
            throw new StatusException("Can't create com.sun.star.sdb.RowSet", e);
        }
        oRowSetProps = UnoRuntime.queryInterface(XPropertySet.class, oRowSet);

        xRowSet = UnoRuntime.queryInterface(XRowSet.class, oRowSet);

        try {
            oRowSetProps.setPropertyValue("DataSourceName",cDataSourceName);
            oRowSetProps.setPropertyValue("Command",cDataCommand);
            oRowSetProps.setPropertyValue("CommandType", new Integer(CommandType.TABLE));
        } catch (UnknownPropertyException e) {
            throw new StatusException("Can't set properties on oRowSet", e);
        } catch (PropertyVetoException e) {
            throw new StatusException("Can't set properties on oRowSet", e);
        } catch (IllegalArgumentException e) {
            throw new StatusException("Can't set properties on oRowSet", e);
        } catch (WrappedTargetException e) {
            throw new StatusException("Can't set properties on oRowSet", e);
        }
        try {
            xRowSet.execute();
        } catch (SQLException e) {
            throw new StatusException("Can't execute oRowSet", e);
        }

        oResultSet = UnoRuntime.queryInterface(XResultSet.class, oRowSet);

        XResultSet oMMXResultSet = null;
        try {
            oMMXResultSet = UnoRuntime.queryInterface(XResultSet.class,
                   ( (XMultiServiceFactory)
                Param.getMSF()).createInstance("com.sun.star.sdb.RowSet"));

        } catch (Exception e) {
            throw new StatusException("Can't create com.sun.star.sdb.RowSet", e);
        }
        // </create object relations>

        TestEnvironment tEnv = new TestEnvironment(oObj) ;

        // <adding object relations>

        // com.sun.star.sdb.DataAccessDescriptor
        tEnv.addObjRelation("DataAccessDescriptor.XResultSet", oResultSet);
        tEnv.addObjRelation("DataAccessDescriptor.XConnection", getRemoteXConnection(Param));

        // com.sun.star.text.MailMaerge
        tEnv.addObjRelation("MailMerge.XConnection", getRemoteXConnection(Param));
        tEnv.addObjRelation("MailMerge.XResultSet", oMMXResultSet);

        // com.sun.star.text.XMailMergeBroadcaster
        tEnv.addObjRelation( "executeArgs", vXJobArg0);
        tEnv.addObjRelation( "Job", Job);

        // com.sun.star.task.XJob
        tEnv.addObjRelation("XJobArgs", vXJobArgs);

        // </adding object relations>

        return tEnv ;
    }

    private XConnection getRemoteXConnection(TestParameters Param){

        log.println("create remote connection");

        String databaseName = null ;
        XMultiServiceFactory xMSF = null ;
        int uniqueSuffix = Param.getInt("uniqueSuffix");

        try {
            xMSF = (XMultiServiceFactory)Param.getMSF();
            xMSF.createInstance( "com.sun.star.sdb.DatabaseContext" );

            // retrieving temp directory for database
            String tmpDatabaseUrl = utils.getOfficeTempDir((XMultiServiceFactory)Param.getMSF());

            databaseName = "NewDatabaseSource" + uniqueSuffix ;

            String tmpDatabaseFile = tmpDatabaseUrl + databaseName + ".odb";
            System.out.println("try to delete '"+tmpDatabaseFile+"'");
            utils.deleteFile(((XMultiServiceFactory) Param.getMSF()), tmpDatabaseFile);


            tmpDatabaseUrl = "sdbc:dbase:file:///" + tmpDatabaseUrl ;

            // Creating new DBase data source in the TEMP directory
            XInterface newSource = (XInterface) xMSF.createInstance
                ("com.sun.star.sdb.DataSource") ;

            XPropertySet xSrcProp = UnoRuntime.queryInterface(XPropertySet.class, newSource);

            xSrcProp.setPropertyValue("URL", tmpDatabaseUrl) ;

            DBTools dbt = new DBTools( (XMultiServiceFactory)Param.getMSF(), log );
            // registering source in DatabaseContext
            log.println("register database '"+tmpDatabaseUrl+"' as '"+databaseName+"'" );
            dbt.reRegisterDB(databaseName, newSource) ;

            uniqueSuffix++;
            Param.put("uniqueSuffix", new Integer(uniqueSuffix));

            return dbt.connectToSource(newSource);
        }
        catch( Exception e ) {
            uniqueSuffix++;
            Param.put("uniqueSuffix", new Integer(uniqueSuffix));
            log.println("could not register new database" );
            e.printStackTrace();
            throw new StatusException("could not register new database", e) ;
        }
    }

    private XConnection getLocalXConnection(TestParameters Param){

        log.println("create local connection");

        XInterface oDataCont = null;
        try {
            oDataCont = (XInterface)( (XMultiServiceFactory) Param.getMSF()).createInstance
                                   ("com.sun.star.sdb.DatabaseContext");
        } catch(Exception e) {
            throw new StatusException("Couldn't create instance of 'com.sun.star.sdb.DatabaseContext'", e);
        }
        XNameAccess xNADataCont = UnoRuntime.queryInterface(XNameAccess.class, oDataCont);

        String[] dataNames = xNADataCont.getElementNames();

        String dataName="";
        for (int i = 0; i < dataNames.length; i++){
            if (dataNames[i].startsWith("Biblio")) dataName=dataNames[i];
        }

        try{

            Object oDataBase = xNADataCont.getByName(dataName);
            XDataSource xDataSource = UnoRuntime.queryInterface(XDataSource.class, oDataBase);

            return xDataSource.getConnection("","");

        } catch ( NoSuchElementException e){
            throw new StatusException("Couldn't get registered data base", e);
        } catch (  WrappedTargetException e){
            throw new StatusException("Couldn't get registered data base", e);
        } catch (   SQLException e){
            throw new StatusException("Couldn't get XConnection from registered data base", e);
        }

    }

    protected void cleanup(TestParameters Param, PrintWriter log) {
        log.println("closing connections...");
        XMultiServiceFactory xMsf = (XMultiServiceFactory) Param.getMSF();
        DBTools dbt = new DBTools( xMsf, log );

        if (Param.containsKey("uniqueSuffix")){
            int uniqueSuffix = Param.getInt("uniqueSuffix");
            uniqueSuffix--;
            String databaseName =  "";
            while (uniqueSuffix >= 0){

                databaseName = "NewDatabaseSource" + uniqueSuffix ;

                log.println("revoke '"+databaseName+"'");

                try{
                    dbt.revokeDB(databaseName);
                } catch (com.sun.star.uno.Exception e){
                }

                uniqueSuffix--;
            }
        }

    }

}


