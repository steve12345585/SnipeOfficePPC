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

package graphical;

import share.LogWriter;
import stats.SimpleLogWriter;

public class GlobalLogWriter
{
    private static LogWriter m_aGlobalLogWriter = null;

    /**
     * This is just a helper to get clearer code.
     * use this GlobalLogWriter.println(...)
     * @param _sMsg
     */
    protected static synchronized void println(String _sMsg)
    {
        get().println(_sMsg);
    }

    protected static synchronized LogWriter get()
        {
            if (m_aGlobalLogWriter == null)
            {
                SimpleLogWriter aLog = new SimpleLogWriter();
                m_aGlobalLogWriter = aLog;
            }
            return m_aGlobalLogWriter;
        }

//     public static synchronized void initialize()
//         {
//             get().initialize(null, true);
//         }

    protected static synchronized void set(LogWriter _aLog)
        {
            m_aGlobalLogWriter = _aLog;
        }

}

