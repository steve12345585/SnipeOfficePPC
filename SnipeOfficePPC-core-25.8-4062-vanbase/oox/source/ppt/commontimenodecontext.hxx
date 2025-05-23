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


#ifndef OOX_PPT_COMMONTIMENODECONTEXT
#define OOX_PPT_COMMONTIMENODECONTEXT


#include <com/sun/star/animations/XIterateContainer.hpp>
#include "oox/ppt/timenode.hxx"
#include "oox/ppt/timenodelistcontext.hxx"
#include "conditioncontext.hxx"


namespace oox { namespace ppt {

    /** CT_TLCommonTimeNodeData */
    class CommonTimeNodeContext
        : public TimeNodeContext
    {
    public:
        CommonTimeNodeContext( ::oox::core::FragmentHandler2& rParent, sal_Int32  aElement, const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XFastAttributeList >& xAttribs, const TimeNodePtr & pNode);
        ~CommonTimeNodeContext( ) throw( );

        virtual void onEndElement();

        virtual ::oox::core::ContextHandlerRef onCreateContext( sal_Int32 aElementToken, const AttributeList& rAttribs );

    private:
        bool mbIterate;
        ::com::sun::star::uno::Reference< ::com::sun::star::animations::XIterateContainer > mxIter;
    };


} }


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
