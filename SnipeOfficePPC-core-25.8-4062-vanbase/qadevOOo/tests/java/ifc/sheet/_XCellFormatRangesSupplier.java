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


package ifc.sheet;

import lib.MultiMethodTest;

import com.sun.star.container.XIndexAccess;
import com.sun.star.sheet.XCellFormatRangesSupplier;

/**
 * Interface test to check the interface XCellFormatRangesSupplier
 *
 * methods:
 *
 *  getCellFormatRanges()
 *
 */

public class _XCellFormatRangesSupplier extends MultiMethodTest {

    public XCellFormatRangesSupplier oObj = null;

    /**
     * call the method getCellFormatRanges and returns OK result if
     * the gained XIndexAccess isn't null and the method checkIndexAccess
     * returns true.
     */

    public void _getCellFormatRanges() {
        boolean res = true;
        XIndexAccess xIA = oObj.getCellFormatRanges();
        if (xIA != null) {
            res = checkIndexAccess(xIA);
        } else {
            log.println("The gained IndexAccess is null");
            res = false;
        }
        tRes.tested("getCellFormatRanges()",res);
    }

    /**
     * calls the method getCount at the IndexAccess, returns true is it is >0
     * and getByIndex() doesn't throw an exception for Indexes between 0 and count
     */

    protected boolean checkIndexAccess(XIndexAccess xIA) {
        boolean res = true;
        int count = xIA.getCount();
        log.println("Found "+count+" Elements");
        res &= count>0;
        for (int k=0; k<count; k++) {
            try {
                Object element = xIA.getByIndex(k);
                log.println("Element "+k+" = "+element);
            } catch (com.sun.star.lang.IndexOutOfBoundsException e) {
                log.println("Unexpected Exception while getting by Index ("+k+")"+e.getMessage());
                res &=false;
            } catch (com.sun.star.lang.WrappedTargetException e) {
                log.println("Unexpected Exception while getting by Index ("+k+")"+e.getMessage());
                res &=false;
            }
        }
        return res;
    }

}
