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

#ifndef DSAPI_TK_DOCW2_HXX
#define DSAPI_TK_DOCW2_HXX

// USED SERVICES
    // BASE CLASSES
#include <s2_dsapi/dsapitok.hxx>
    // COMPONENTS
    // PARAMETERS

namespace csi
{
namespace dsapi
{


class Tok_Word : public Token
{
  public:
    // Spring and Fall
                        Tok_Word(
                            const char *        i_sText )
                                                :   sText(i_sText) {}
    // OPERATIONS
    virtual void        Trigger(
                            TokenInterpreter &  io_rInterpreter ) const;
    // INQUIRY
    virtual const char* Text() const;

  private:
    // DATA
    String              sText;
};

class Tok_Comma : public Token
{
  public:
    // OPERATIONS
    virtual void        Trigger(
                            TokenInterpreter &  io_rInterpreter ) const;
    // INQUIRY
    virtual const char* Text() const;
};

class Tok_DocuEnd : public Token
{
  public:
    // Spring and Fall
    // OPERATIONS
    virtual void        Trigger(
                            TokenInterpreter &  io_rInterpreter ) const;
    // INQUIRY
    virtual const char* Text() const;
};

class Tok_EOL : public Token
{
  public:
    // Spring and Fall
    // OPERATIONS
    virtual void        Trigger(
                            TokenInterpreter &  io_rInterpreter ) const;
    // INQUIRY
    virtual const char* Text() const;
};

class Tok_EOF : public Token
{
  public:
    // Spring and Fall
    // OPERATIONS
    virtual void        Trigger(
                            TokenInterpreter &  io_rInterpreter ) const;
    // INQUIRY
    virtual const char* Text() const;
};

class Tok_White : public Token
{
  public:
    // OPERATIONS
    virtual void        Trigger(
                            TokenInterpreter &  io_rInterpreter ) const;
    // INQUIRY
    virtual const char* Text() const;
};



}   // namespace dsapi
}   // namespace csi


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
