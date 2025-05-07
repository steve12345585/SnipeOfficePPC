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
package com.sun.star.report.pentaho;

import org.pentaho.reporting.libraries.base.config.Configuration;
import org.pentaho.reporting.libraries.formula.ContextEvaluationException;
import org.pentaho.reporting.libraries.formula.FormulaContext;
import org.pentaho.reporting.libraries.formula.LocalizationContext;
import org.pentaho.reporting.libraries.formula.function.FunctionRegistry;
import org.pentaho.reporting.libraries.formula.operators.OperatorFactory;
import org.pentaho.reporting.libraries.formula.typing.Type;
import org.pentaho.reporting.libraries.formula.typing.TypeRegistry;

/**
 *
 * @author Ocke Janssen
 */
public class PentahoFormulaContext implements FormulaContext
{

    final private FormulaContext backend;
    final private Configuration config;

    public PentahoFormulaContext(final FormulaContext backend, final Configuration _config)
    {
        this.backend = backend;
        config = _config;
    }

    public LocalizationContext getLocalizationContext()
    {
        return backend.getLocalizationContext();
    }

    public Configuration getConfiguration()
    {
        return config;
    }

    public FunctionRegistry getFunctionRegistry()
    {
        return backend.getFunctionRegistry();
    }

    public TypeRegistry getTypeRegistry()
    {
        return backend.getTypeRegistry();
    }

    public OperatorFactory getOperatorFactory()
    {
        return backend.getOperatorFactory();
    }

    public Type resolveReferenceType(final Object name) throws ContextEvaluationException
    {
        return backend.resolveReferenceType(name);
    }

    public Object resolveReference(final Object name) throws ContextEvaluationException
    {
        if (name == null)
        {
            throw new NullPointerException();
        }
        return backend.resolveReference(name);
    }

    public boolean isReferenceDirty(final Object name) throws ContextEvaluationException
    {
        return backend.isReferenceDirty(name);
    }
}
