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

#ifndef SC_FORMDATA_HXX
#define SC_FORMDATA_HXX

#include <tools/gen.hxx>
#include <formula/formdata.hxx>
class ScInputHandler;
class ScDocShell;

//============================================================================

class ScFormEditData : public formula::FormEditData
{
public:
                        ScFormEditData();
                        virtual ~ScFormEditData();

    ScInputHandler*     GetInputHandler()   { return pInputHandler;}
    ScDocShell*         GetDocShell()       { return pScDocShell;}

    void                SetInputHandler(ScInputHandler* pHdl)   { pInputHandler=pHdl;}
    void                SetDocShell(ScDocShell* pSds)           { pScDocShell=pSds;}


    virtual void            SaveValues();

private:
                        ScFormEditData( const ScFormEditData& );
//  const ScFormEditData& operator=( const ScFormEditData& r );


    ScInputHandler*     pInputHandler;
    ScDocShell*         pScDocShell;
};



#endif // SC_CRNRDLG_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
