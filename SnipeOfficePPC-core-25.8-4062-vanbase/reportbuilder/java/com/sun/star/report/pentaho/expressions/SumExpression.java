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
package com.sun.star.report.pentaho.expressions;

import com.sun.star.report.DataRow;
import com.sun.star.report.ReportExpression;

public class SumExpression implements ReportExpression
{

    private Object[] parameters;

    public SumExpression()
    {
    }

    public Object getParameters()
    {
        return parameters;
    }

    public Object getValue(final DataRow row)
    {

        return null;
    }

    public void setParameters(final Object[] parameters)
    {
        this.parameters = parameters;
    }
}
