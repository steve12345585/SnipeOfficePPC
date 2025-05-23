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

#ifndef ARY_IDL_I_COMRELA_HXX
#define ARY_IDL_I_COMRELA_HXX

// USED SERVICES
#include <ary/idl/i_types4idl.hxx>

namespace ary
{
namespace doc
{
    class OldIdlDocu;
}
}




namespace ary
{
namespace idl
{


/** Contains data for an IDL code entity related to another one like a base of
    an interface or of a service or the supported interface of a service.
*/
class CommentedRelation
{
  public:
    // LIFECYCLE

                        CommentedRelation(
                            Type_id             i_nType,
                            doc::OldIdlDocu *   i_pInfo )
                                                :   nType(i_nType),
                                                    pInfo(i_pInfo)
                                                {}
    // INQUIRY
    Type_id             Type() const            { return nType; }
    doc::OldIdlDocu *   Info() const            { return pInfo; }

  private:
    // DATA
    Type_id             nType;
    doc::OldIdlDocu *   pInfo;
};




}   // namespace idl
}   // namespace ary
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
