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

package org.openoffice.xmerge.merger;

/**
 * This is the difference algorithm interface.  It is an interface so
 * that different algorithms may be plugged-in to actually compute
 * the differences.
 *
 * NOTE: this code may not be thread safe.
 */
public interface DiffAlgorithm {

    /**
     *  Returns a <code>Difference</code> array.  This method finds out
     *  the difference between two sequences.
     *
     *  @param  orgSeq  The original sequence of object.
     *  @param  modSeq  The modified (or changed) sequence to
     *                  compare against with the origial.
     *
     *  @return  A <code>Difference</code> array.
     */
    public Difference[] computeDiffs(Iterator orgSeq, Iterator modSeq);
}

