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
package com.sun.star.wizards.web.data;

import com.sun.star.wizards.common.PropertyNames;

public class CGStyle extends ConfigSetItem
{

    public String cp_Name;
    //public boolean         cp_Readonly; 
    public String cp_CssHref;
    public String cp_BackgroundImage;
    public String cp_IconSet;

    public String toString()
    {
        return cp_Name;
    }

    public String getBackgroundUrl()
    {
        if (cp_BackgroundImage == null || cp_BackgroundImage.equals(PropertyNames.EMPTY_STRING))
        {
            return null;
        }
        else
        {
            return cp_BackgroundImage;
        }
    }
}
