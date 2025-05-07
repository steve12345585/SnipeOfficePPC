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

import com.sun.star.awt.tree.XMutableTreeNode;
import lib.MultiMethodTest;
import lib.Status;
import lib.StatusException;

/**
 * Testing <code>com.sun.star.awt.tree.XTreeDataModel</code>
 * interface methods :
 * <ul>
 *  <li><code> appendChild()</code></li>
 *  <li><code> insertChildByIndex()</code></li>
 *  <li><code> removeChildByIndex()</code></li>
 *  <li><code> setHasChildrenOnDemand()</code></li>
 *  <li><code> setDisplayValue()</code></li>
 *  <li><code> setNodeGraphicURL()</code></li>
 *  <li><code> setExpandedGraphicURL()</code></li>
 *  <li><code> setCollapsedGraphicURL()</code></li>
 *  <li><code> DataValue()</code></li>
 * </ul> <p>
 * Test is <b> NOT </b> multithread compilant. <p>
 *
 * @see com.sun.star.awt.tree.XTreeDataModel
 */
public class _XMutableTreeNode extends MultiMethodTest {

    public XMutableTreeNode oObj = null;

    private int mCount = 0;

    private XMutableTreeNodeCreator nodeCreator = null;

    public static interface XMutableTreeNodeCreator{
        public XMutableTreeNode createNode(String name);
    }

    public void before(){
        nodeCreator = (XMutableTreeNodeCreator) tEnv.getObjRelation("XMutableTreeNodeCreator");
        if (nodeCreator == null){
            throw new StatusException(Status.failed(
                "Couldn't get relation 'XMutableTreeNodeCreator'"));
        }

    }

     public void _appendChild(){
        boolean bOK = true;

        log.println("try to append a valid node...");

        XMutableTreeNode myNode = nodeCreator.createNode("myNodeToAppend");

        try {
            oObj.appendChild(myNode);
        } catch (com.sun.star.lang.IllegalArgumentException ex) {
            log.println("ERROR: could not appedn a valid node: " + ex.toString());
            bOK = false;
        }

        log.println("try to append the node a second time...");
        try {

            oObj.appendChild(myNode);
            log.println("ERROR: expected IllegalArgumentException was not thrown => FAILED");
            bOK = false;
        } catch (com.sun.star.lang.IllegalArgumentException ex) {
            log.println("expected IllegalArgumentException was thrown => OK");
        }

        log.println("try to append the object itself...");
        try {

            oObj.appendChild(oObj);
            log.println("ERROR: expected IllegalArgumentException was not thrown => FAILED");
            bOK = false;
        } catch (com.sun.star.lang.IllegalArgumentException ex) {
            log.println("expected IllegalArgumentException was thrown => OK");
        }

        tRes.tested("appendChild()", bOK);
    }

     public void _insertChildByIndex(){
        boolean bOK = true;

        XMutableTreeNode myNode = nodeCreator.createNode("myNodeToInsert");

        try {
            log.println("try to insert a valid node...");
            oObj.insertChildByIndex(0, myNode);
        } catch (com.sun.star.lang.IllegalArgumentException ex) {
            log.println("ERROR: could not insert a valid node: " + ex.toString());
            bOK = false;
        } catch (com.sun.star.lang.IndexOutOfBoundsException ex) {
            log.println("ERROR: could not insert a valid node on index '0': " + ex.toString());
            bOK = false;
        }

        try {
            log.println("try to insert a valid node a second time...");
            oObj.insertChildByIndex(0, myNode);
            log.println("ERROR: expected IllegalArgumentException was not thrown => FAILED");
            bOK = false;
        } catch (com.sun.star.lang.IllegalArgumentException ex) {
            log.println("expected IllegalArgumentException wa thrown => OK");
        } catch (com.sun.star.lang.IndexOutOfBoundsException ex) {
            log.println("ERROR: wrong IndexOutOfBoundsException was thrown. Expected is IllegalArgumentException => FAILED");
            bOK = false;
        }

        XMutableTreeNode myNode2 = nodeCreator.createNode("myNodeToInsert2");

        try {
            log.println("try to insert a valid node on invalid index '-3'...");
            oObj.insertChildByIndex(-3, myNode2);
            log.println("ERROR: expected IndexOutOfBoundsException was not thrown => FAILED");
            bOK = false;
        } catch (com.sun.star.lang.IllegalArgumentException ex) {
            log.println("ERROR: wrong IllegalArgumentException was thrown. Expeced is IndexOutOfBoundsException => FAILED");
            bOK = false;
        } catch (com.sun.star.lang.IndexOutOfBoundsException ex) {
            log.println("Expected IndexOutOfBoundsException was thrown => OK");
        }

        try {
            log.println("try to insert the object itself...");
            oObj.insertChildByIndex(0, oObj);
            log.println("ERROR: expected IllegalArgumentException was not thrown => FAILED");
            bOK = false;
        } catch (com.sun.star.lang.IllegalArgumentException ex) {
            log.println("expected IllegalArgumentException was thrown => OK");
        } catch (com.sun.star.lang.IndexOutOfBoundsException ex) {
            log.println("unexpected IndexOutOfBoundsException was thrown, expected was IllegalArgumentException => FAILED");
            bOK = false;
        }

        tRes.tested("insertChildByIndex()", bOK);
    }

     public void _removeChildByIndex(){

         requiredMethod("insertChildByIndex()");
         boolean bOK = true;

        try {
            log.println("try to remove node at index '0'...");
            oObj.removeChildByIndex(0);
        } catch (com.sun.star.lang.IndexOutOfBoundsException ex) {
            log.println("ERROR: IndexOutOfBoundsException was thrown => FAILED");
            bOK = false;
        }

        try {
            log.println("try to remove node at invalid index '-3'");
            oObj.removeChildByIndex(-3);
            log.println("ERROR: expeced IndexOutOfBoundsException was not thrown => FAILED");
            bOK = false;
        } catch (com.sun.star.lang.IndexOutOfBoundsException ex) {
            log.println("expected IndexOutOfBoundsException was thrown => OK");
        }
        tRes.tested("removeChildByIndex()", bOK);
    }

     public void _setHasChildrenOnDemand(){
        boolean bOK = true;

        log.println("setHasChildrenOnDemand(true)");
        oObj.setHasChildrenOnDemand(true);

        log.println("setHasChildrenOnDemand(false)");
        oObj.setHasChildrenOnDemand(false);

        tRes.tested("setHasChildrenOnDemand()", bOK);
    }

     public void _setDisplayValue(){
        boolean bOK = true;

        log.println("setDisplayValue(\"MyTestValue\")");
        oObj.setDisplayValue("MyTestValue");

        log.println("setDisplayValue(null)");
        oObj.setDisplayValue(null);

        log.println("oObj.setDisplayValue(oObj)");
        oObj.setDisplayValue(oObj);

        tRes.tested("setDisplayValue()", bOK);
    }

     public void _setNodeGraphicURL(){
        boolean bOK = true;

        log.println("setNodeGraphicURL(\"MyTestURL\")");
        oObj.setNodeGraphicURL("MyTestURL");

        log.println("setNodeGraphicURL(null)");
        oObj.setNodeGraphicURL(null);

        tRes.tested("setNodeGraphicURL()", bOK);
    }

     public void _setExpandedGraphicURL(){
        boolean bOK = true;

        log.println("setExpandedGraphicURL(\"myExpandedURL\")");
        oObj.setExpandedGraphicURL("myExpandedURL");

        log.println("setExpandedGraphicURL(null)");
        oObj.setExpandedGraphicURL(null);

        tRes.tested("setExpandedGraphicURL()", bOK);
    }

     public void _setCollapsedGraphicURL(){
        boolean bOK = true;

        log.println("setCollapsedGraphicURL(\"myCollapsedURL\")");
        oObj.setCollapsedGraphicURL("myCollapsedURL");

        log.println("setCollapsedGraphicURL(null)");
        oObj.setCollapsedGraphicURL(null);

        tRes.tested("setCollapsedGraphicURL()", bOK);
    }

     public void _DataValue(){
        boolean bOK = true;

        log.println("setDataValue(\"myDataValue\")");
        oObj.setDataValue("myDataValue");

        String sDataValue = (String) oObj.getDataValue();

        if ( ! sDataValue.equals("myDataValue")) {
            log.println("ERROR: getDataVlaue does not return the value which is inserted before:\n" +
                "\texpected: myDataValue\n" +
                "\tgot: " + sDataValue);
            bOK = false;
        }

        log.println("setDataValue(null)");
        oObj.setDataValue(null);

        Object oDataValue = oObj.getDataValue();
        if ( oDataValue != null) {
            log.println("ERROR: getDataVlaue does not return the value which is inserted before:\n" +
                "\texpected: null\n" +
                "\tgot: " + oDataValue.toString());
            bOK = false;
        }

        log.println("oObj.setDisplayValue(oObj)");
        oObj.setDisplayValue(oObj);

        oDataValue = oObj.getDataValue();
        if ( oDataValue != null) {
            log.println("ERROR: getDataVlaue does not return the value which is inserted before:\n" +
                "\texpected: " + oObj.toString() +"\n" +
                "\tgot: " + oDataValue.toString());
            bOK = false;
        }

        tRes.tested("DataValue()", bOK);
    }

}
