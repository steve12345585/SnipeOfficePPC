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
package com.sun.star.report.util;

import com.sun.star.report.ParameterMap;

import java.util.HashMap;
import java.util.Map;


public class DefaultParameterMap implements ParameterMap
{

    private final Map<String,Object> backend;

    public DefaultParameterMap()
    {
        backend = new HashMap<String,Object>();
    }

    public void clear()
    {
        backend.clear();
    }

    /**
     * Retrieves the value stored for a key in this properties collection.
     *
     * @param key the property key.
     * @return The stored value, or <code>null</code> if the key does not exist in this
     *         collection.
     */
    public Object get(final String key)
    {
        if (key == null)
        {
            throw new NullPointerException("DefaultParameterMap.get (..): Parameter 'key' must not be null");
        }
        return backend.get(key);
    }

    /**
     * Retrieves the value stored for a key in this properties collection, and returning the
     * default value if the key was not stored in this properties collection.
     *
     * @param key          the property key.
     * @param defaultValue the default value to be returned when the key is not stored in
     *                     this properties collection.
     * @return The stored value, or the default value if the key does not exist in this
     *         collection.
     */
    public Object get(final String key, final Object defaultValue)
    {
        if (key == null)
        {
            throw new NullPointerException("DefaultParameterMap.get (..): Parameter 'key' must not be null");
        }
        final Object o = this.backend.get(key);
        if (o == null)
        {
            return defaultValue;
        }
        return o;
    }

    public String[] keys()
    {
        return this.backend.keySet().toArray(new String[backend.size()]);
    }

    /**
     * Adds a property to this properties collection. If a property with the given name
     * exist, the property will be replaced with the new value. If the value is null, the
     * property will be removed.
     *
     * @param key   the property key.
     * @param value the property value.
     */
    public void put(final String key, final Object value)
    {
        if (key == null)
        {
            throw new NullPointerException("ReportProperties.put (..): Parameter 'key' must not be null");
        }
        if (value == null)
        {
            this.backend.remove(key);
        }
        else
        {
            this.backend.put(key, value);
        }
    }

    public int size()
    {
        return this.backend.size();
    }
}
