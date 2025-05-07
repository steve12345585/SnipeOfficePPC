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

package helper;

import java.util.ArrayList;

import com.sun.star.beans.PropertyValue;

public class PropertyHelper
{
    /**
       Create a PropertyValue[] from a ArrayList
       @param _aArrayList
       @return a PropertyValue[]
    */
    public static PropertyValue[] createPropertyValueArrayFormArrayList(ArrayList _aPropertyList)
        {
            // copy the whole PropertyValue List to an PropertyValue Array
            PropertyValue[] aSaveProperties = null;

            if (_aPropertyList == null)
            {
                aSaveProperties = new PropertyValue[0];
            }
            else
            {
                aSaveProperties = (PropertyValue[])_aPropertyList.toArray(new PropertyValue[_aPropertyList.size()]);
                // old java 1.4
//                if (_aPropertyList.size() > 0)
//                {
//                    aSaveProperties = new PropertyValue[_aPropertyList.size()];
//                    for (int i = 0;i<_aPropertyList.size(); i++)
//                    {
//                        aSaveProperties[i] = (PropertyValue) _aPropertyList.get(i);
//                    }
//                }
//                else
//                {
//                    aSaveProperties = new PropertyValue[0];
//                }

// show properties?
//                 if (_aPropertyList.size() > 0)
//                 {
//                     // aSaveProperties = new PropertyValue[_aPropertyList.size()];
//                     for (int i = 0;i<_aPropertyList.size(); i++)
//                     {
//                         PropertyValue aProp = (PropertyValue) _aPropertyList.get(i);
//                         showProperty(aProp);
//                     }
//                 }

            }
            return aSaveProperties;
        }
}
