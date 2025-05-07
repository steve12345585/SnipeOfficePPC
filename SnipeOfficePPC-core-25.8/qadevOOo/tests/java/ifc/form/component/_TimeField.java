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

package ifc.form.component;

import lib.MultiPropertyTest;
import util.utils;

/**
* Testing <code>com.sun.star.form.component.TimeField</code>
* service properties :
* <ul>
*  <li><code> DefaultTime</code></li>
* </ul> <p>
* Properties testing is automated by <code>lib.MultiPropertyTest</code>.
* @see com.sun.star.form.component.TimeField
*/
public class _TimeField extends MultiPropertyTest {

    /**
    * This property can be VOID, and in case if it is so new
    * value must defined.
    */
    public void _DefaultTime() {
        testProperty("DefaultTime", new PropertyTester() {
            protected Object getNewValue(String p, Object old) {
                return utils.isVoid(old) ? new Integer(4321) :
                    super.getNewValue(p, old) ;
            }
        }) ;
    }
}

