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

#ifndef ADC_TOKEN_HXX
#define ADC_TOKEN_HXX

// USED SERVICES
    // BASE CLASSES
    // COMPONENTS
    // PARAMETRS


class TokenDealer;

/**
*/
class TextToken
{
  public:
    typedef TextToken * (*F_CRTOK)(const char*);

    // LIFECYCLE
    virtual             ~TextToken() {}


    // INQUIRY
    virtual const char* Text() const = 0;

    virtual void        DealOut(
                            ::TokenDealer &     o_rDealer ) = 0;
};

class Tok_Eof : public TextToken
{
    virtual void        DealOut(                // Implemented in tokdeal.cxx
                            TokenDealer &       o_rDealer );
    virtual const char* Text() const;
};

#endif


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
