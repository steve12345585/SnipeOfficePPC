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
package complex.tdoc;

import com.sun.star.beans.XPropertiesChangeListener;
import com.sun.star.beans.XPropertiesChangeNotifier;
import share.LogWriter;

/**
 * Check the XPropertiesChangeNotifier
 */
public class _XPropertiesChangeNotifier {
    public XPropertiesChangeNotifier oObj = null;
    public LogWriter log = null;

    PropertiesChangeListener listener = new PropertiesChangeListener();
    String[] args = null;


    public boolean _addPropertiesChangeListener() {
        oObj.addPropertiesChangeListener(args, listener);
        return true;
    }

    public boolean _removePropertiesChangeListener() {
        oObj.removePropertiesChangeListener(args, listener);
        return true;
    }

    private class PropertiesChangeListener implements XPropertiesChangeListener {
        public boolean disposed = false;
        public boolean propChanged = false;

        public void disposing(com.sun.star.lang.EventObject eventObject) {
            disposed = true;
        }

        public void propertiesChange(com.sun.star.beans.PropertyChangeEvent[] propertyChangeEvent) {
            propChanged = true;
        }

    }
}
