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

import com.sun.star.awt.XTopWindowListener;
import com.sun.star.lang.EventObject;

class QueuedTopWindowListener
    implements XTopWindowListener
{
    public QueuedTopWindowListener (TopWindowListener aListener)
    {
        maListener = aListener;
    }

    public void windowOpened (final com.sun.star.lang.EventObject aEvent) throws RuntimeException
    {
        EventQueue.Instance().addEvent (new Runnable()
            {
                public void run()
                {
                    QueuedTopWindowListener.this.maListener.windowOpened (aEvent);
                }
            }
            );
    }

    public void windowClosing (final com.sun.star.lang.EventObject aEvent) throws RuntimeException
    {
        // Ignored.
    }

    public void windowClosed (final com.sun.star.lang.EventObject aEvent) throws RuntimeException
    {
        EventQueue.Instance().addEvent (new Runnable()
            {
                public void run()
                {
                    QueuedTopWindowListener.this.maListener.windowClosed (aEvent);
                }
            }
            );
    }

    public void windowMinimized (final com.sun.star.lang.EventObject aEvent)
        throws RuntimeException
    {
        System.out.println ("QueuedTopWindowListener: Top window minimized: " + aEvent);
    }

    public void windowNormalized (final com.sun.star.lang.EventObject aEvent)
        throws RuntimeException
    {
        System.out.println ("QueuedTopWindowListener: Top window normalized: " + aEvent);
    }

    public void windowActivated (final com.sun.star.lang.EventObject aEvent)
        throws RuntimeException
    {
        System.out.println ("QueuedTopWindowListener: Top window actived: " + aEvent);
    }

    public void windowDeactivated (final com.sun.star.lang.EventObject aEvent)
        throws RuntimeException
    {
        System.out.println ("QueuedTopWindowListener: Top window deactived: " + aEvent);
    }

    public void disposing( final EventObject aEvent)
    {
        EventQueue.Instance().addDisposingEvent (new Runnable()
            {
                public void run()
                {
                    if (QueuedTopWindowListener.this.maListener != null)
                        QueuedTopWindowListener.this.maListener.disposing (aEvent);
                }
            }
            );
    }

    private TopWindowListener maListener;
}
