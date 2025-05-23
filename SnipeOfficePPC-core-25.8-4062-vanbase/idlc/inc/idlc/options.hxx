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

#ifndef _IDLC_OPTIONS_HXX_
#define _IDLC_OPTIONS_HXX_

#include <idlc/idlctypes.hxx>

typedef ::boost::unordered_map< ::rtl::OString,
                         ::rtl::OString,
                         HashString,
                         EqualString > OptionMap;

class IllegalArgument
{
public:
    IllegalArgument(const ::rtl::OString& msg)
        : m_message(msg) {}

    ::rtl::OString  m_message;
};


class Options
{
public:
    explicit Options(char const * progname);
    ~Options();

    static bool checkArgument(std::vector< std::string > & rArgs, char const * arg, size_t len);
    static bool checkCommandFile(std::vector< std::string > & rArgs, char const * filename);

    bool initOptions(std::vector< std::string > & rArgs)
        throw(IllegalArgument);
    bool badOption(char const * reason, std::string const & rArg)
        throw(IllegalArgument);
    bool setOption(char const * option, std::string const & rArg);

#if 0  /* @@@ */
    sal_Bool initOptions(int ac, char* av[], sal_Bool bCmdFile=sal_False)
        throw( IllegalArgument );
#endif /* @@@ */

    ::rtl::OString prepareHelp();
    ::rtl::OString prepareVersion();

    const ::rtl::OString&   getProgramName() const;
    bool                isValid(const ::rtl::OString& option);
    const ::rtl::OString&   getOption(const ::rtl::OString& option)
        throw( IllegalArgument );

    const StringVector& getInputFiles() const { return m_inputFiles; }
    bool readStdin() const { return m_stdin; }
    bool verbose() const { return m_verbose; }
    bool quiet() const { return m_quiet; }

protected:
    ::rtl::OString  m_program;
    StringVector    m_inputFiles;
    bool            m_stdin;
    bool            m_verbose;
    bool            m_quiet;
    OptionMap       m_options;
};

#endif // _IDLC_OPTIONS_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
