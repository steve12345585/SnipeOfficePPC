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

package com.sun.star.uno;

import org.junit.Test;
import static org.junit.Assert.*;

public final class Any_Test {
    @Test public void testAnyAny() {
        boolean caught = false;
        try {
            new Any(Type.ANY, null);
        } catch (IllegalArgumentException e) {
            caught = true;
        }
        assertTrue(caught);
    }

    @Test public void testComplete() {
        assertSame(Any.VOID, Any.complete(Any.VOID));
        assertEquals(
            new Any(Type.LONG, new Integer(10)), Any.complete(new Integer(10)));
        assertEquals(
            new Any(new Type(XInterface.class), null), Any.complete(null));
        XInterface x = new XInterface() {};
        assertEquals(new Any(new Type(XInterface.class), x), Any.complete(x));
    }
}
