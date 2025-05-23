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

#ifndef _FTP_FTPDYNRESULTSET_HXX
#define _FTP_FTPDYNRESULTSET_HXX

#include "rtl/ref.hxx"
#include "ucbhelper/resultsethelper.hxx"

#include "ftpcontent.hxx"

namespace ftp {

    class ResultSetFactory;

    class DynamicResultSet : public ::ucbhelper::ResultSetImplHelper
    {
        rtl::Reference< FTPContent > m_xContent;
        com::sun::star::uno::Reference<
            com::sun::star::ucb::XCommandEnvironment > m_xEnv;
        ResultSetFactory*                    m_pFactory;

    private:
        virtual void initStatic();
        virtual void initDynamic();

    public:
        DynamicResultSet(
            const com::sun::star::uno::Reference<
                com::sun::star::uno::XComponentContext >& rxContext,
            const rtl::Reference< FTPContent >& rxContent,
            const com::sun::star::ucb::OpenCommandArgument2& rCommand,
            const com::sun::star::uno::Reference<
                com::sun::star::ucb::XCommandEnvironment >& rxEnv,
            ResultSetFactory* pFactory );

        ~DynamicResultSet();
    };

}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
