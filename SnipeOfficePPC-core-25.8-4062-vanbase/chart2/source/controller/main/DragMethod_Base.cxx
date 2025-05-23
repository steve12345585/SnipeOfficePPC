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


#include "DragMethod_Base.hxx"

#include "Strings.hrc"
#include "ResId.hxx"
#include "macros.hxx"
#include "ObjectNameProvider.hxx"
#include "ObjectIdentifier.hxx"
#include <rtl/math.hxx>
//header for class SdrPageView
#include <svx/svdpagv.hxx>
#include <svx/ActionDescriptionProvider.hxx>
#include <com/sun/star/beans/XPropertySet.hpp>

//.............................................................................
namespace chart
{
//.............................................................................

using namespace ::com::sun::star;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::WeakReference;

DragMethod_Base::DragMethod_Base( DrawViewWrapper& rDrawViewWrapper
                                             , const rtl::OUString& rObjectCID
                                             , const Reference< frame::XModel >& xChartModel
                                             , ActionDescriptionProvider::ActionType eActionType )
    : SdrDragMethod( rDrawViewWrapper )
    , m_rDrawViewWrapper(rDrawViewWrapper)
    , m_aObjectCID(rObjectCID)
    , m_eActionType( eActionType )
    , m_xChartModel( WeakReference< frame::XModel >(xChartModel) )
{
    setMoveOnly(true);
}
DragMethod_Base::~DragMethod_Base()
{
}

Reference< frame::XModel > DragMethod_Base::getChartModel() const
{
    return Reference< frame::XModel >( m_xChartModel );
}

rtl::OUString DragMethod_Base::getUndoDescription() const
{
    return ActionDescriptionProvider::createDescription(
                m_eActionType,
                ObjectNameProvider::getName( ObjectIdentifier::getObjectType( m_aObjectCID )));
}
void DragMethod_Base::TakeSdrDragComment(String& rStr) const
{
    rStr = String( getUndoDescription() );
}
Pointer DragMethod_Base::GetSdrDragPointer() const
{
    if( IsDraggingPoints() || IsDraggingGluePoints() )
        return Pointer(POINTER_MOVEPOINT);
    else
        return Pointer(POINTER_MOVE);
}
//.............................................................................
} //namespace chart
//.............................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
