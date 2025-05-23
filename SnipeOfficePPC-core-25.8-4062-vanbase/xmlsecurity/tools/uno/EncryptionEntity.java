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

package com.sun.star.xml.security.uno;

/* uno classes */
import com.sun.star.uno.UnoRuntime;
import com.sun.star.lang.XMultiComponentFactory;
import com.sun.star.lang.XInitialization;
import com.sun.star.uno.XComponentContext;

import com.sun.star.xml.crypto.*;
import com.sun.star.xml.crypto.sax.*;

/*
 * this class maintains the data for an encryption operation.
 */
class EncryptionEntity extends SecurityEntity
{
    private int m_nEncryptionElementCollectorId;

    EncryptionEntity(
        XSecuritySAXEventKeeper xSAXEventKeeper,
        boolean isExporting,
        Object resultListener,
        XXMLSecurityContext xXMLSecurityContext,
        XXMLSignature xXMLSignature,
        XXMLEncryption xXMLEncryption,
        XMultiComponentFactory xRemoteServiceManager,
        XComponentContext xRemoteContext)
    {
        super(xSAXEventKeeper, xXMLSecurityContext, xXMLSignature,
             xXMLEncryption, xRemoteServiceManager, xRemoteContext);

        m_nEncryptionElementCollectorId = m_xSAXEventKeeper.addSecurityElementCollector(
            ElementMarkPriority.AFTERMODIFY,
            true);

        m_xSAXEventKeeper.setSecurityId(m_nEncryptionElementCollectorId, m_nSecurityId);

        if (isExporting)
        {
            try
            {
                /*
                 * creates a Encryptor.
                 */
                Object encryptor = m_xRemoteServiceManager.createInstanceWithContext(
                    TestTool.ENCRYPTOR_COMPONENT, m_xRemoteContext);

                m_xReferenceResolvedListener =
                    (XReferenceResolvedListener)UnoRuntime.queryInterface(
                        XReferenceResolvedListener.class, encryptor);

                /*
                 * initializes the Encryptor.
                 */
                XInitialization xInitialization =
                    (XInitialization)UnoRuntime.queryInterface(
                        XInitialization.class, m_xReferenceResolvedListener);
                Object args[]=new Object[5];
                args[0] = new Integer(m_nSecurityId).toString();
                args[1] = m_xSAXEventKeeper;
                args[2] = new Integer(m_nEncryptionElementCollectorId).toString();
                args[3] = m_xXMLSecurityContext.getSecurityEnvironment();
                args[4] = m_xXMLEncryption;
                xInitialization.initialize(args);

                /*
                 * sets encryption result listener.
                 */
                XEncryptionResultBroadcaster m_xEncryptionResultBroadcaster =
                    (XEncryptionResultBroadcaster)UnoRuntime.queryInterface(
                        XEncryptionResultBroadcaster.class, m_xReferenceResolvedListener);
                m_xEncryptionResultBroadcaster.addEncryptionResultListener(
                    (XEncryptionResultListener)UnoRuntime.queryInterface(
                        XEncryptionResultListener.class, resultListener));
            }
            catch( com.sun.star.uno.Exception e)
            {
                e.printStackTrace();
            }
        }
        else
        {
            try{
                /*
                 * creates a Decryptor.
                 */
                Object decryptor = m_xRemoteServiceManager.createInstanceWithContext(
                    TestTool.DECRYPTOR_COMPONENT, m_xRemoteContext);

                m_xReferenceResolvedListener =
                    (XReferenceResolvedListener)UnoRuntime.queryInterface(
                        XReferenceResolvedListener.class, decryptor);

                /*
                 * initializes the Decryptor.
                 */
                XInitialization xInitialization = (XInitialization)UnoRuntime.queryInterface(XInitialization.class, m_xReferenceResolvedListener);
                Object args[]=new Object[5];
                args[0] = new Integer(m_nSecurityId).toString();
                args[1] = m_xSAXEventKeeper;
                args[2] = new Integer(m_nEncryptionElementCollectorId).toString();
                args[3] = m_xXMLSecurityContext;
                args[4] = m_xXMLEncryption;
                xInitialization.initialize(args);

                /*
                 * sets decryption result listener.
                 */
                XDecryptionResultBroadcaster m_xDecryptionResultBroadcaster =
                    (XDecryptionResultBroadcaster)UnoRuntime.queryInterface(
                        XDecryptionResultBroadcaster.class, m_xReferenceResolvedListener);
                m_xDecryptionResultBroadcaster.addDecryptionResultListener(
                    (XDecryptionResultListener)UnoRuntime.queryInterface(
                        XDecryptionResultListener.class, resultListener));
            }
            catch( com.sun.star.uno.Exception e)
            {
                e.printStackTrace();
            }
        }

        /*
         * creates a Blocker.
         */
        int blockerId = m_xSAXEventKeeper.addBlocker();
        m_xSAXEventKeeper.setSecurityId(blockerId, m_nSecurityId);

        try
        {
            XBlockerMonitor xBlockerMonitor = (XBlockerMonitor)UnoRuntime.queryInterface(
                XBlockerMonitor.class, m_xReferenceResolvedListener);
            xBlockerMonitor.setBlockerId(blockerId);
        }
        catch( com.sun.star.uno.Exception e)
        {
            e.printStackTrace();
        }

        /*
         * configures the resolve listener for the encryption template.
         */
        XReferenceResolvedBroadcaster xReferenceResolvedBroadcaster =
            (XReferenceResolvedBroadcaster)UnoRuntime.queryInterface(
                XReferenceResolvedBroadcaster.class, m_xSAXEventKeeper);
        xReferenceResolvedBroadcaster.addReferenceResolvedListener(m_nEncryptionElementCollectorId, m_xReferenceResolvedListener);
    }

    /*
     * add the reference to this encryption.
     *
     * 1. askes the SAXEventKeeper to add a ElementCollector to for the new
     * referenced element;
     * 2. configures this ElementCollector's security id;
     * 3. tells the SAXEventKeeper which listener will receive the reference
     * resolved notification.
     * 4. notifies the SignatureCollector about the reference id.
     */
    protected boolean setReference(boolean isExporting)
    {
        boolean rc = false;

        int referenceId = m_xSAXEventKeeper.addSecurityElementCollector(
            isExporting?
            (ElementMarkPriority.AFTERMODIFY):(ElementMarkPriority.BEFOREMODIFY),
            true);

        m_xSAXEventKeeper.setSecurityId(referenceId, m_nSecurityId);

        XReferenceResolvedBroadcaster xReferenceResolvedBroadcaster =
            (XReferenceResolvedBroadcaster)UnoRuntime.queryInterface(
                XReferenceResolvedBroadcaster.class, m_xSAXEventKeeper);
        xReferenceResolvedBroadcaster.addReferenceResolvedListener(
            referenceId, m_xReferenceResolvedListener);

        try
        {
            XReferenceCollector xReferenceCollector =
                (XReferenceCollector)UnoRuntime.queryInterface(
                    XReferenceCollector.class, m_xReferenceResolvedListener);
            xReferenceCollector.setReferenceId(referenceId);
        }
        catch( com.sun.star.uno.Exception e)
        {
            e.printStackTrace();
            rc = false;
        }

        return rc;
    }
}

