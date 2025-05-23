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

package com.sun.star.lib.uno.environments.remote;

import com.sun.star.uno.UnoRuntime;
import java.io.UnsupportedEncodingException;
import java.math.BigInteger;
import java.util.Arrays;

public final class ThreadId {
    public static ThreadId createFresh() {
        BigInteger c;
        synchronized (PREFIX) {
            c = count;
            count = count.add(BigInteger.ONE);
        }
        try {
            return new ThreadId((PREFIX + c).getBytes("UTF-8"));
        } catch (UnsupportedEncodingException e) {
            throw new RuntimeException("this cannot happen: " + e);
        }
    }

    public ThreadId(byte[] id) {
        this.id = id;
    }

    public boolean equals(Object obj) {
        return obj instanceof ThreadId
            && Arrays.equals(id, ((ThreadId) obj).id);
    }

    public int hashCode() {
        int h = hash;
        if (h == 0) {
            // Same algorithm as java.util.List.hashCode (also see Java 1.5
            // java.util.Arrays.hashCode(byte[])):
            h = 1;
            for (int i = 0; i < id.length; ++i) {
                h = 31 * h + id[i];
            }
            hash = h;
        }
        return h;
    }

    public String toString() {
        StringBuffer b = new StringBuffer("[ThreadId:");
        for (int i = 0; i < id.length; ++i) {
            String n = Integer.toHexString(id[i] & 0xFF);
            if (n.length() == 1) {
                b.append('0');
            }
            b.append(n);
        }
        b.append(']');
        return b.toString();
    }

    public byte[] getBytes() {
        return id;
    }

    private static final String PREFIX
    = "java:" + UnoRuntime.getUniqueKey() + ":";
    private static BigInteger count = BigInteger.ZERO;

    private byte[] id;
    private int hash = 0;
}
