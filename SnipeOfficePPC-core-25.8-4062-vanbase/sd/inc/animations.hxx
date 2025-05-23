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

#ifndef _SD_ANIMATIONS_HXX_
#define _SD_ANIMATIONS_HXX_

#include <sddllapi.h>

namespace sd
{

/** stores the link between an after effect node and its master for later insertion
    into the timing hierarchie
*/
struct AfterEffectNode
{
    ::com::sun::star::uno::Reference< ::com::sun::star::animations::XAnimationNode > mxNode;
    ::com::sun::star::uno::Reference< ::com::sun::star::animations::XAnimationNode > mxMaster;
    bool mbOnNextEffect;

    AfterEffectNode( const ::com::sun::star::uno::Reference< ::com::sun::star::animations::XAnimationNode >& xNode, const ::com::sun::star::uno::Reference< ::com::sun::star::animations::XAnimationNode >& xMaster, bool bOnNextEffect )
        : mxNode( xNode ), mxMaster( xMaster ), mbOnNextEffect( bOnNextEffect ) {}
};

typedef std::list< AfterEffectNode > AfterEffectNodeList;

/** inserts the animation node in the given AfterEffectNode at the correct position
    in the timing hierarchie of its master */
SD_DLLPUBLIC void stl_process_after_effect_node_func(AfterEffectNode& rNode);

} // namespace sd;

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
