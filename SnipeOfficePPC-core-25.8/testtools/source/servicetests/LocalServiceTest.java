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

package testtools.servicetests;

import com.sun.star.comp.helper.Bootstrap;
import com.sun.star.container.XSet;
import com.sun.star.lang.XMultiComponentFactory;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;

public final class LocalServiceTest extends TestBase {
    protected TestServiceFactory getTestServiceFactory() throws Exception {
        return new TestServiceFactory() {
                public Object get() throws Exception {
                    XComponentContext context
                        = Bootstrap.createInitialComponentContext(null);
                    XMultiComponentFactory serviceManager
                        = context.getServiceManager();
                    UnoRuntime.queryInterface(XSet.class, serviceManager).
                        insert(new TestService());
                    return serviceManager.createInstanceWithContext(
                        "testtools.servicetests.TestService2", context);
                }

                public void dispose() throws Exception {}
            };
    }
}
