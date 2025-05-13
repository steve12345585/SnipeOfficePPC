/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
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

#ifndef _SV_GENSYS_H
#define _SV_GENSYS_H

#include <salsys.hxx>
#include <salframe.hxx>
#include <vclpluginapi.h>
#include <list>

/*
 * Helps de-tangle the rather horrible ShowNativeMessageBox API
 */
class VCL_DLLPUBLIC SalGenericSystem : public SalSystem
{
 public:
    SalGenericSystem();
    virtual ~SalGenericSystem();
    virtual int ShowNativeDialog( const rtl::OUString& rTitle,
                                  const rtl::OUString& rMessage,
                                  const std::list< rtl::OUString >& rButtons,
                                  int nDefButton ) = 0;

    virtual int ShowNativeMessageBox( const rtl::OUString& rTitle,
                                      const rtl::OUString& rMessage,
                                      int nButtonCombination,
                                      int nDefaultButton, bool bUseResources);

    // simple helpers primarily for X Windowing W_CLASS hints
    static const char *getFrameResName();
    static const char *getFrameClassName();
    static rtl::OString getFrameResName( SalExtStyle nStyle );
};

#endif // _SV_GENSYS_H

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
