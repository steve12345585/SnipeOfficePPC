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

package ifc.form;

import lib.MultiMethodTest;
import lib.Status;
import lib.StatusException;

import com.sun.star.awt.XTextComponent;
import com.sun.star.awt.XWindow;
import com.sun.star.form.XChangeBroadcaster;
import com.sun.star.form.XChangeListener;
import com.sun.star.lang.EventObject;
import com.sun.star.uno.UnoRuntime;

/**
* Testing <code>com.sun.star.form.XChangeBroadcaster</code>
* interface methods:
* <ul>
*  <li><code> addChangeListener() </code></li>
*  <li><code> removeChangeListener() </code></li>
* </ul><p>
* This test needs the following object relations :
* <ul>
*  <li> <code>'Win1'</code> (of type <code>XWindow</code>):
*   used to change context when testing interface methods</li>
*  <li> <code>'Win2'</code> (of type <code>XWindow</code>):
*   used to change context when testing interface methods </li>
*  <li> <code>'CONTROL'</code> (of type <code>XControl</code> and
*   must implement <code>XTextComponent</code> interface):
*   used to change context when testing interface methods </li>
*  <li> <code>'XChangeBroadcaster.Changer'</code>
*   (of type <code>ifc.form._XChangeBroadcaster.Changer</code>)
*   <b>optional</b> : this relation <b>must be specified</b> when
*   <code>XTextComponent</code> is not supported by the tested
*   component. It is used to change some component content
*   which must cause listener call. </li>
* </ul> <p>
*
* <b>Prerequisites:</b> component must implement <code>XTextComponent</code>
* interface for changing component's text which must cause listener call.
* If the component cann't support the interface, then the relation
* <code>'XChangeBroadcaster.Changer'</code> must be passed. <p>
*
* Test is <b> NOT </b> multithread compilant. <p>
* @see com.sun.star.form.XChangeBroadcaster
*/
public class _XChangeBroadcaster extends MultiMethodTest {
    public static XChangeBroadcaster oObj = null;
    protected boolean changed = false;

    /**
     * This interface must be implemented by component and passed
     * in relation if it doesn't support <code>XTextComponent</code>
     * interface. It used to change the content of component.
     */
    public static interface Changer {
        /**
         * The method must change the component's content to
         * cause a listener call.
         */
        public void change() ;
    }

    /**
    * Class we need to test methods
    */
    protected class MyChangeListener implements XChangeListener {
        public void disposing ( EventObject oEvent ) {}
        public void changed ( EventObject oEvent ) {
            System.out.println("Listener called");
            changed = true;
        }
      }

    protected XChangeListener listener = new MyChangeListener();
    protected XTextComponent xText = null ;
    protected Changer changer = null ;

    /**
     * Tries to query the tested component for <code>XTextComponent</code>
     * interface and retrieves a relation
     * <code>'XChangeBroadcaster.Changer'</code>.
     * @throw StatusException If neither relation is found nor interface
     * is queried.
     */
    public void before() {
        xText = (XTextComponent)
            UnoRuntime.queryInterface(XTextComponent.class,oObj);
        changer = (Changer) tEnv.getObjRelation("XChangeBroadcaster.Changer") ;

        if (xText == null && changer == null)
            throw new StatusException(Status.failed
                ("Neither 'XChangeBroadcaster.Changer' relation found " +
                "nor XTextComponent is supported")) ;
    }

    /**
    * Test calls the method, then object relations 'Win1', 'Win2', 'CONTROL'
    * are obtained, and context is changed.<p>
    * Has <b> OK </b> status if listener was called after context has changed.
    */
    public void _addChangeListener() {
        log.println("Testing addChangeListener ...");
        oObj.addChangeListener( listener );
        XWindow win1 = (XWindow) tEnv.getObjRelation("Win1");
        XWindow win2 = (XWindow) tEnv.getObjRelation("Win2");
        win1.setFocus();

        changeContent() ;
        shortWait();

        win2.setFocus();
        XTextComponent TC = (XTextComponent)UnoRuntime.queryInterface
            (XTextComponent.class,tEnv.getObjRelation("CONTROL"));
        TC.setText("NOXChangeBroadcaster");
        shortWait();
        tRes.tested("addChangeListener()", changed);
    }

    /**
    * Test calls the method, then object relations 'Win1', 'Win2', 'CONTROL'
    * are obtained, and context is changed.<p>
    * Has <b> OK </b> status if listener was not called after context has
    * changed.<p>
    * The following method tests are to be completed successfully before :
    * <ul>
    *  <li> <code> addChangeListener() </code> : adds the specified listener
    *  to receive the "changed" event</li>
    * </ul>
    */
    public void _removeChangeListener() {
        requiredMethod("addChangeListener()");
        changed = false;
        log.println("Testing removeChangeListener ...");
        oObj.addChangeListener( listener );
        XWindow win2 = (XWindow) tEnv.getObjRelation("Win2");
        win2.setFocus();

        changeContent() ;

        win2.setFocus();
        shortWait();
        tRes.tested("removeChangeListener()", !changed);
    }

    /**
    * Sleeps for 0.2 sec. to allow StarOffice to react on <code>
    * reset</code> call.
    */
    private void shortWait() {
        try {
            Thread.sleep(2000) ;
        } catch (InterruptedException e) {
            log.println("While waiting :" + e) ;
        }
    }

    /**
     * Changes the content of the component depending on whether
     * <code>XTextComponent</code> is supported or not. If yes
     * then the text is chahged, if not the relation <code>change()</code>
     * method is used.
     */
    protected void changeContent() {
        if (xText != null) {
            xText.setText("XChangeBroadcaster".equals(xText.getText()) ?
                "NoXChangeBroadcaster" : "XChangeBroadcaster") ;
        } else {
            changer.change();
        }
    }

} // finished class _XChangeBroadcaster

