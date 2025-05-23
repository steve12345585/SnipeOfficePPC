/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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
#ifndef _I18N_SCRIPTTYPEDETECTOR_HXX_
#define _I18N_SCRIPTTYPEDETECTOR_HXX_

#include "i18nutildllapi.h"
#include <rtl/ustring.hxx>

class I18NUTIL_DLLPUBLIC ScriptTypeDetector
{
public:
    static sal_Int32 beginOfScriptDirection( const rtl::OUString& Text, sal_Int32 nPos, sal_Int16 scriptDirection );
    static sal_Int32 endOfScriptDirection( const rtl::OUString& Text, sal_Int32 nPos, sal_Int16 scriptDirection );
    static sal_Int16 getScriptDirection( const rtl::OUString& Text, sal_Int32 nPos, sal_Int16 defaultScriptDirection );
    static sal_Int32 beginOfCTLScriptType( const rtl::OUString& Text, sal_Int32 nPos );
    static sal_Int32 endOfCTLScriptType( const rtl::OUString& Text, sal_Int32 nPos );
    static sal_Int16 getCTLScriptType( const rtl::OUString& Text, sal_Int32 nPos );
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
