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
#ifndef CHART2_CONTROLLERCOMMANDDISPATCH_HXX
#define CHART2_CONTROLLERCOMMANDDISPATCH_HXX

#include "CommandDispatch.hxx"
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/view/XSelectionSupplier.hpp>
#include <cppuhelper/implbase1.hxx>

#include <memory>

namespace chart
{

class ChartController;
class CommandDispatchContainer;

namespace impl
{
struct ModelState;
struct ControllerState;

// #i63017# : need to implement the XSelectionChangeListener in order
// to update the ControllerState when the selection changes.
typedef ::cppu::ImplInheritanceHelper1<
        CommandDispatch,
        ::com::sun::star::view::XSelectionChangeListener >
    ControllerCommandDispatch_Base;
}

/** This class is a CommandDispatch that is responsible for all commands that
    the ChartController supports.

    This class determines which commands are currently available (via the model
    state) and if an available command is called forwards it to the
    ChartController.
 */
class ControllerCommandDispatch : public impl::ControllerCommandDispatch_Base
{
public:
    explicit ControllerCommandDispatch(
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::uno::XComponentContext > & xContext,
        ChartController* pController, CommandDispatchContainer* pContainer );
    virtual ~ControllerCommandDispatch();

    // late initialisation, especially for adding as listener
    virtual void initialize();

protected:
    // ____ XDispatch ____
    virtual void SAL_CALL dispatch(
        const ::com::sun::star::util::URL& URL,
        const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& Arguments )
        throw (::com::sun::star::uno::RuntimeException);

    // ____ WeakComponentImplHelperBase ____
    /// is called when this is disposed
    virtual void SAL_CALL disposing();

    // ____ XEventListener (base of XModifyListener) ____
    virtual void SAL_CALL disposing(
        const ::com::sun::star::lang::EventObject& Source )
        throw (::com::sun::star::uno::RuntimeException);

    virtual void fireStatusEvent(
        const ::rtl::OUString & rURL,
        const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener > & xSingleListener );

    // ____ XModifyListener ____
    virtual void SAL_CALL modified(
        const ::com::sun::star::lang::EventObject& aEvent )
        throw (::com::sun::star::uno::RuntimeException);

    // ____ XSelectionChangeListener ____
    virtual void SAL_CALL selectionChanged(
        const ::com::sun::star::lang::EventObject& aEvent )
        throw (::com::sun::star::uno::RuntimeException);

private:
    void fireStatusEventForURLImpl(
        const ::rtl::OUString & rURL,
        const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener > & xSingleListener );

    bool commandAvailable( const ::rtl::OUString & rCommand );
    void updateCommandAvailability();

    bool isShapeControllerCommandAvailable( const ::rtl::OUString& rCommand );

    ChartController* m_pChartController;
    ::com::sun::star::uno::Reference<
            ::com::sun::star::frame::XController > m_xController;
    ::com::sun::star::uno::Reference<
            ::com::sun::star::view::XSelectionSupplier > m_xSelectionSupplier;
    ::com::sun::star::uno::Reference<
            ::com::sun::star::frame::XDispatch > m_xDispatch;

    ::std::auto_ptr< impl::ModelState > m_apModelState;
    ::std::auto_ptr< impl::ControllerState > m_apControllerState;

    mutable ::std::map< ::rtl::OUString, bool > m_aCommandAvailability;
    mutable ::std::map< ::rtl::OUString, ::com::sun::star::uno::Any > m_aCommandArguments;

    CommandDispatchContainer* m_pDispatchContainer;
};

} //  namespace chart

// CHART2_CONTROLLERCOMMANDDISPATCH_HXX
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
