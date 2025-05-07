/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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

#ifndef ARY_CINFO_CI_TEXT2_HXX
#define ARY_CINFO_CI_TEXT2_HXX



// USED SERVICES
    // BASE CLASSES
    // COMPONENTS
    // PARAMETERS


namespace ary
{
namespace inf
{


class DocumentationDisplay;


class DocuToken
{
  public:
    virtual             ~DocuToken() {}
    virtual void        DisplayAt(
                            DocumentationDisplay &
                                                o_rDisplay ) const = 0;
    virtual bool        IsWhiteOnly() const = 0;
};


class DocuTex2
{
  public:
    typedef std::vector< DocuToken * >  TokenList;

                        DocuTex2();
    virtual             ~DocuTex2();

    virtual void        DisplayAt(
                            DocumentationDisplay &
                                                o_rDisplay ) const;
    void                AddToken(
                            DYN DocuToken &     let_drToken );

    const TokenList &   Tokens() const          { return aTokens; }
    bool                IsEmpty() const         { return aTokens.empty(); }
    const String &      TextOfFirstToken() const;

    String &            Access_TextOfFirstToken();

  private:
    TokenList           aTokens;
};



// IMPLEMENTATION

}   // namespace inf
}   // namespace ary


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
