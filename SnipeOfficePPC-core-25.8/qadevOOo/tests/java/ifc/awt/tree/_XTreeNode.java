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

package ifc.awt.tree;

import com.sun.star.awt.tree.XTreeNode;
import com.sun.star.uno.AnyConverter;
import lib.MultiMethodTest;
import lib.Status;
import lib.StatusException;

/**
 * Testing <code>com.sun.star.awt.tree.XTreeDataModel</code>
 * interface methods :
 * <ul>
 *  <li><code> getChildAt()</code></li>
 *  <li><code> getChildCount()</code></li>
 *  <li><code> getParent()</code></li>
 *  <li><code> getIndex()</code></li>
 *  <li><code> hasChildrenOnDemand()</code></li>
 *  <li><code> getDisplayValue()</code></li>
 *  <li><code> getNodeGraphicURL()</code></li>
 *  <li><code> getExpandedGraphicURL()</code></li>
 *  <li><code> getCollapsedGraphicURL()</code></li> * </ul> <p>
 * Test is <b> NOT </b> multithread compilant. <p>
 *
 * @see com.sun.star.awt.tree.XTreeDataModel
 */
public class _XTreeNode extends MultiMethodTest {

    public XTreeNode oObj = null;

    private int mCount = 0;

    String msDisplayValue = null;
    String msExpandedGraphicURL = null;
    String msCollapsedGraphicURL = null;
    String msNodeGraphicURL = null;


    public void before(){
        msDisplayValue = (String) tEnv.getObjRelation("XTreeNode_DisplayValue");
        if (msDisplayValue == null){
            throw new StatusException(Status.failed(
                "Couldn't get relation 'XTreeNode_DisplayVlaue'"));
        }

        msExpandedGraphicURL = (String) tEnv.getObjRelation("XTreeNode_ExpandedGraphicURL");
        if (msExpandedGraphicURL == null){
            throw new StatusException(Status.failed(
                "Couldn't get relation 'XTreeNode_ExpandedGraphicURL'"));
        }

        msCollapsedGraphicURL = (String) tEnv.getObjRelation("XTreeNode_CollapsedGraphicURL");
        if (msCollapsedGraphicURL == null){
            throw new StatusException(Status.failed(
                "Couldn't get relation 'XTreeNode_CollapsedGraphicURL'"));
        }

        msNodeGraphicURL = (String) tEnv.getObjRelation("XTreeNode_NodeGraphicURL");
        if(msNodeGraphicURL == null){
            throw new StatusException(Status.failed(
                "Couldn't get relation 'XTreeNode_NodeGraphicURL'"));
        }

    }

    /**
     * Gets the title and compares it to the value set in
     * <code>setTitle</code> method test. <p>
     * Has <b>OK</b> status is set/get values are equal.
     * The following method tests are to be completed successfully before :
     * <ul>
     *  <li> <code> setTitle </code>  </li>
     * </ul>
     */
    public void _getChildAt(){
        this.requiredMethod("getChildCount()");
        boolean bOK = true;

        for (int i=0; i < mCount ; i++){
            XTreeNode xNode = null;
            try {
                xNode = oObj.getChildAt(i);
            } catch (com.sun.star.lang.IndexOutOfBoundsException ex) {
                log.println("ERROR: getChildAt(" + i + "): " + ex.toString());
            }
            if (xNode == null){
                log.println("ERROR: getChildAt(" + i + ") returns null => FAILED");
                bOK = false;
            }
        }

        tRes.tested("getChildAt()", bOK);
    }


    public void _getChildCount(){

        boolean bOK = true;
        mCount = oObj.getChildCount();
        log.println("got count '" + mCount + "' of children");
        if (mCount < 1 ) {
            log.println("ERROR: got a count < 1. The test object must be support morw then zero children => FAILED");
            bOK = false;
        }
        tRes.tested("getChildCount()", bOK);
    }


    public void _getParent(){
        this.requiredMethod("getChildAt()");

        boolean bOK = true;
        XTreeNode xNode = null;
        try {
            log.println("try to getChildAt(0)");
            xNode = oObj.getChildAt(0);
        } catch (com.sun.star.lang.IndexOutOfBoundsException ex) {
            log.println("ERROR: getChildAt(0): " + ex.toString());
        }

        log.println("try to get parrent of children");
        XTreeNode xParrent = xNode.getParent();


        bOK = oObj.equals(xParrent);
        log.println("original object and parrent should be the same: " + bOK);
        tRes.tested("getParent()", bOK);
    }


    public void _getIndex(){
        this.requiredMethod("getChildAt()");

        boolean bOK = true;
        XTreeNode xNode = null;
        try {
            log.println("try to getChildAt(0)");
            xNode = oObj.getChildAt(0);
        } catch (com.sun.star.lang.IndexOutOfBoundsException ex) {
            log.println("ERROR: getChildAt(0): " + ex.toString());
        }

        log.println("try to get index from child...");
        int index = oObj.getIndex(xNode);

        if (index != 0){
            log.println("ERROR: getIndex() does not return '0' => FAILED");
            bOK = false;
        }

        tRes.tested("getIndex()", bOK);
    }


    public void _hasChildrenOnDemand(){

        boolean bOK = true;

        bOK = oObj.hasChildrenOnDemand();
        tRes.tested("hasChildrenOnDemand()", bOK);
    }


    public void _getDisplayValue(){

        boolean bOK = true;
        String DisplayValue = null;
        Object dispVal = oObj.getDisplayValue();

        try {
            DisplayValue = AnyConverter.toString(dispVal);
        } catch (com.sun.star.lang.IllegalArgumentException ex) {
            log.println("ERROR: could not convert the returned object of 'getDisplyValue()' " +
                "to String with AnyConverter: " + ex.toString());
        }

        if ( ! this.msDisplayValue.equals(DisplayValue)){
            log.println("ERROR: getNodeGraphicURL() does not return expected value:\n" +
                "\tExpected: " + this.msDisplayValue +"\n" +
                "\tGot: " + DisplayValue);
            bOK = false;
        }

        tRes.tested("getDisplayValue()", bOK);
    }


    public void _getNodeGraphicURL(){

        boolean bOK = true;
        String graphicURL = oObj.getNodeGraphicURL();

        if ( ! this.msNodeGraphicURL.equals(graphicURL)){
            log.println("ERROR: getNodeGraphicURL() does not return expected value:\n" +
                "\tExpected: " + this.msNodeGraphicURL +"\n" +
                "\tGot: " + graphicURL);
            bOK = false;
        }
        tRes.tested("getNodeGraphicURL()", bOK);
    }


    public void _getExpandedGraphicURL(){

        boolean bOK = true;
        String ExpandedGraphicURL = oObj.getExpandedGraphicURL();

        if ( ! this.msExpandedGraphicURL.equals(ExpandedGraphicURL)){
            log.println("ERROR: getExpandedGraphicURL() does not return expected value:\n" +
                "\tExpected: " + this.msExpandedGraphicURL +"\n" +
                "\tGot: " + ExpandedGraphicURL);
            bOK = false;
        }

        tRes.tested("getExpandedGraphicURL()", bOK);
    }


    public void _getCollapsedGraphicURL(){

        boolean bOK = true;

        String CollapsedGraphicURL = oObj.getCollapsedGraphicURL();

        if ( ! this.msCollapsedGraphicURL.equals(CollapsedGraphicURL)){
            log.println("ERROR: getExpandedGraphicURL() does not return expected value:\n" +
                "\tExpected: " + this.msCollapsedGraphicURL +"\n" +
                "\tGot: " + CollapsedGraphicURL);
            bOK = false;
        }

        tRes.tested("getCollapsedGraphicURL()", bOK);
    }

}