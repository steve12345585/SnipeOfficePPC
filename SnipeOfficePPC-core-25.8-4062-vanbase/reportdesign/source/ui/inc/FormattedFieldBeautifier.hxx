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

#ifndef INCLUDED_FORMATTEDFIELDBEAUTIFIER_HXX
#define INCLUDED_FORMATTEDFIELDBEAUTIFIER_HXX

#include <com/sun/star/beans/PropertyChangeEvent.hpp>
#include <com/sun/star/container/XChild.hpp>
#include <com/sun/star/awt/XVclWindowPeer.hpp>
#include <com/sun/star/report/XReportComponent.hpp>

#include <vector>
#include <IReportControllerObserver.hxx>

namespace rptui
{
    class OReportController;

    class FormattedFieldBeautifier : public IReportControllerObserver
    {
        const OReportController& m_rReportController;
        sal_Int32 m_nTextColor;

        ::com::sun::star::uno::Reference< ::com::sun::star::awt::XVclWindowPeer > getVclWindowPeer(const ::com::sun::star::uno::Reference< ::com::sun::star::report::XReportComponent >& _xComponent) throw(::com::sun::star::uno::RuntimeException);

        void setPlaceholderText( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxComponent );
        void setPlaceholderText( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XVclWindowPeer >& _xVclWindowPeer, const ::rtl::OUString& _rText );

        sal_Int32 getTextColor();

    public:
        FormattedFieldBeautifier(const OReportController & _aObserver);
        virtual ~FormattedFieldBeautifier();

        void    notifyPropertyChange( const ::com::sun::star::beans::PropertyChangeEvent& _rEvent );
        void    notifyElementInserted( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxElement );
        void    handle( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxElement );
    };

} // namespace rptui


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
