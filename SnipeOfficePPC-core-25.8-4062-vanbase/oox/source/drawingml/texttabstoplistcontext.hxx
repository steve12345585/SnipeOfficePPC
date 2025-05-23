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

#ifndef OOX_DRAWINGML_TEXTTABSTOPLISTCONTEXT_HXX
#define OOX_DRAWINGML_TEXTTABSTOPLISTCONTEXT_HXX

#include <list>

#include <com/sun/star/style/TabStop.hpp>

#include "oox/core/contexthandler.hxx"

namespace oox { namespace drawingml {

    class TextTabStopListContext : public ::oox::core::ContextHandler
    {
    public:
        TextTabStopListContext( ::oox::core::ContextHandler& rParent,
                ::std::list< ::com::sun::star::style::TabStop >  & aTabList );
        ~TextTabStopListContext();

        virtual void SAL_CALL endFastElement( ::sal_Int32 Element ) throw (::com::sun::star::xml::sax::SAXException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XFastContextHandler > SAL_CALL createFastChildContext( ::sal_Int32 Element, const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XFastAttributeList >& Attribs ) throw (::com::sun::star::xml::sax::SAXException, ::com::sun::star::uno::RuntimeException);

protected:
        ::std::list< ::com::sun::star::style::TabStop >  & maTabList;
    };


} }


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
