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

#ifndef OOX_DRAWINGML_SLIDEMASTERTEXTSTYLESCONTEXT_HXX
#define OOX_DRAWINGML_SLIDEMASTERTEXTSTYLESCONTEXT_HXX

#include "oox/drawingml/theme.hxx"
#include "oox/core/contexthandler.hxx"
#include "oox/core/fragmenthandler2.hxx"
#include "oox/ppt/slidepersist.hxx"

namespace oox { namespace ppt {

class SlideMasterTextStylesContext : public oox::core::FragmentHandler2
{
public:
    SlideMasterTextStylesContext( ::oox::core::FragmentHandler2& rParent, SlidePersistPtr pSlidePersistPtr );
    ~SlideMasterTextStylesContext();
    virtual ::oox::core::ContextHandlerRef onCreateContext( sal_Int32 aElementToken, const AttributeList& rAttribs );

protected:
    SlidePersistPtr     mpSlidePersistPtr;
};

} }

#endif  //  OOX_DRAWINGML_SLIDEMASTERTEXTSTYLESCONTEXT_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
