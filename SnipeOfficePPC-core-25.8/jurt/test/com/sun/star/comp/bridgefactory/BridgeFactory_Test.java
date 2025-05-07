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

package com.sun.star.comp.bridgefactory;

import com.sun.star.bridge.BridgeExistsException;
import com.sun.star.bridge.XBridge;
import com.sun.star.comp.connections.PipedConnection;
import com.sun.star.connection.XConnection;
import com.sun.star.lang.XComponent;
import com.sun.star.uno.UnoRuntime;
import org.junit.Test;
import static org.junit.Assert.*;

public final class BridgeFactory_Test {
    @Test public void test() throws Exception {
        PipedConnection rightSide = new PipedConnection(new Object[0]);
        PipedConnection leftSide = new PipedConnection(new Object[]{rightSide});

        BridgeFactory bridgeFactory = new BridgeFactory(); // create the needed bridgeFactory

        // create a bridge
        XBridge xBridge = bridgeFactory.createBridge("testbridge", "urp", (XConnection)leftSide, null);

        // test that we get the same bridge
        assertTrue(
            UnoRuntime.areSame(
                xBridge, bridgeFactory.getBridge("testbridge")));

        // test that we can not create another bridge with same name
        try {
            XBridge dummy = bridgeFactory.createBridge("testbridge", "urp", (XConnection)leftSide, null);

            fail();
        }
        catch(BridgeExistsException bridgeExistsException) {
        }


        // test getExistingBridges
        XBridge xBridges[] = bridgeFactory.getExistingBridges();
        assertTrue(UnoRuntime.areSame(xBridge, xBridges[0]));

        // dispose the bridge
        XComponent xComponent = UnoRuntime.queryInterface(XComponent.class, xBridge);
        xComponent.dispose();


        // test that the bridge has been removed
        assertTrue(bridgeFactory.getBridge("testbridge") == null);



        rightSide = new PipedConnection(new Object[0]);
        leftSide = new PipedConnection(new Object[]{rightSide});


        // test that we really get a new bridge
        XBridge xBridge_new = bridgeFactory.createBridge("testbridge", "urp", (XConnection)leftSide, null);
        assertFalse(UnoRuntime.areSame(xBridge, xBridge_new));

        for(int i = 0; i <10000; ++ i) {
            Object x[] = new Object[100];
        }

        // test getExistingBridges
        xBridges = bridgeFactory.getExistingBridges();
        assertEquals(1, xBridges.length);
        assertTrue(UnoRuntime.areSame(xBridge_new, xBridges[0]));

        // dispose the new bridge
        XComponent xComponent_new = UnoRuntime.queryInterface(XComponent.class, xBridge_new);
        xComponent_new.dispose();
    }
}
