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

#ifndef FORMS_SOURCE_RICHTEXT_SPECIALDISPATCHERS_HXX
#define FORMS_SOURCE_RICHTEXT_SPECIALDISPATCHERS_HXX

#include "parametrizedattributedispatcher.hxx"

//........................................................................
namespace frm
{
//........................................................................

    //====================================================================
    //= OSelectAllDispatcher
    //====================================================================
    class OSelectAllDispatcher : public ORichTextFeatureDispatcher
    {
    public:
        OSelectAllDispatcher( EditView& _rView, const ::com::sun::star::util::URL&  _rURL );

    protected:
        ~OSelectAllDispatcher();

        // XDispatch
        virtual void SAL_CALL dispatch( const ::com::sun::star::util::URL& URL, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& Arguments ) throw (::com::sun::star::uno::RuntimeException);

        // ORichTextFeatureDispatcher
        virtual ::com::sun::star::frame::FeatureStateEvent  buildStatusEvent() const;
    };

    //====================================================================
    //= OParagraphDirectionDispatcher
    //====================================================================
    class OParagraphDirectionDispatcher : public OAttributeDispatcher
    {
    public:
        OParagraphDirectionDispatcher(
            EditView&                           _rView,
            AttributeId                         _nAttributeId,
            const ::com::sun::star::util::URL&  _rURL,
            IMultiAttributeDispatcher*          _pMasterDispatcher
        );

    protected:
        // ORichTextFeatureDispatcher
        virtual ::com::sun::star::frame::FeatureStateEvent  buildStatusEvent() const;
    };

    //====================================================================
    //= OTextDirectionDispatcher
    //====================================================================
    class OTextDirectionDispatcher : public ORichTextFeatureDispatcher
    {
    public:
        OTextDirectionDispatcher( EditView& _rView, const ::com::sun::star::util::URL&  _rURL );

    protected:
        // XDispatch
        virtual void SAL_CALL dispatch( const ::com::sun::star::util::URL& URL, const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& Arguments ) throw (::com::sun::star::uno::RuntimeException);

        // ORichTextFeatureDispatcher
        virtual ::com::sun::star::frame::FeatureStateEvent  buildStatusEvent() const;
    };

    //====================================================================
    //= OAsianFontLayoutDispatcher
    //====================================================================
    class OAsianFontLayoutDispatcher : public OParametrizedAttributeDispatcher
    {
    public:
        OAsianFontLayoutDispatcher(
            EditView&                           _rView,
            AttributeId                         _nAttributeId,
            const ::com::sun::star::util::URL&  _rURL,
            IMultiAttributeDispatcher*          _pMasterDispatcher
        );

    protected:
        // OParametrizedAttributeDispatcher
        virtual const SfxPoolItem* convertDispatchArgsToItem(
            const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& _rArguments );
    };

//........................................................................
} // namespace frm
//........................................................................

#endif // FORMS_SOURCE_RICHTEXT_SPECIALDISPATCHERS_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
