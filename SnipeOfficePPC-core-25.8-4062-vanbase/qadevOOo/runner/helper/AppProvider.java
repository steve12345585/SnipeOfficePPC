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

package helper;

/**
 *
 * Interface to get a Manager to access the application to check
 */
public interface AppProvider {

    /**
     * Method to get the desired Manager
     */
    public Object getManager(lib.TestParameters param);

    /**
     * Method to dispose the desired Manager
     */
    public boolean disposeManager(lib.TestParameters param);

    /**
     * Close an office.
     * @param param The test parameters.
     * @param closeIfPossible If true, close even if
     * it was running before the test
     * @return True, if close worked.
     */
    public boolean closeExistingOffice(lib.TestParameters param,
                                                boolean closeIfPossible);
}
