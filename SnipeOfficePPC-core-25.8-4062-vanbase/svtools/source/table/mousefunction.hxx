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

#ifndef SVTOOLS_MOUSEFUNCTION_HXX
#define SVTOOLS_MOUSEFUNCTION_HXX

#include "svtools/table/tabletypes.hxx"

#include <rtl/ref.hxx>

#include <boost/noncopyable.hpp>

class MouseEvent;

//......................................................................................................................
namespace svt { namespace table
{
//......................................................................................................................

    class ITableControl;

    //==================================================================================================================
    //= FunctionResult
    //==================================================================================================================
    enum FunctionResult
    {
        ActivateFunction,
        ContinueFunction,
        DeactivateFunction,

        SkipFunction
    };

    //==================================================================================================================
    //= IMouseFunction
    //==================================================================================================================
    class IMouseFunction : public ::rtl::IReference, public ::boost::noncopyable
    {
    public:
        virtual FunctionResult  handleMouseMove( ITableControl& i_tableControl, MouseEvent const & i_event ) = 0;
        virtual FunctionResult  handleMouseDown( ITableControl& i_tableControl, MouseEvent const & i_event ) = 0;
        virtual FunctionResult  handleMouseUp( ITableControl& i_tableControl, MouseEvent const & i_event ) = 0;

    protected:
        virtual ~IMouseFunction() { }
    };

    //==================================================================================================================
    //= MouseFunction
    //==================================================================================================================
    class MouseFunction : public IMouseFunction
    {
    public:
        MouseFunction()
            :m_refCount( 0 )
        {
        }
    protected:
        ~MouseFunction()
        {
        }

    public:
        virtual oslInterlockedCount SAL_CALL acquire();
        virtual oslInterlockedCount SAL_CALL release();

    private:
        oslInterlockedCount m_refCount;
    };

    //==================================================================================================================
    //= ColumnResize
    //==================================================================================================================
    class ColumnResize : public MouseFunction
    {
    public:
        ColumnResize()
            :m_nResizingColumn( COL_INVALID )
        {
        }

    public:
        // IMouseFunction
        virtual FunctionResult  handleMouseMove( ITableControl& i_tableControl, MouseEvent const & i_event );
        virtual FunctionResult  handleMouseDown( ITableControl& i_tableControl, MouseEvent const & i_event );
        virtual FunctionResult  handleMouseUp( ITableControl& i_tableControl, MouseEvent const & i_event );

    private:
        ColPos  m_nResizingColumn;
    };

    //==================================================================================================================
    //= RowSelection
    //==================================================================================================================
    class RowSelection : public MouseFunction
    {
    public:
        RowSelection()
            :m_bActive( false )
        {
        }

    public:
        // IMouseFunction
        virtual FunctionResult  handleMouseMove( ITableControl& i_tableControl, MouseEvent const & i_event );
        virtual FunctionResult  handleMouseDown( ITableControl& i_tableControl, MouseEvent const & i_event );
        virtual FunctionResult  handleMouseUp( ITableControl& i_tableControl, MouseEvent const & i_event );

    private:
        bool    m_bActive;
    };

    //==================================================================================================================
    //= ColumnSortHandler
    //==================================================================================================================
    class ColumnSortHandler : public MouseFunction
    {
    public:
        ColumnSortHandler()
            :m_nActiveColumn( COL_INVALID )
        {
        }

    public:
        // IMouseFunction
        virtual FunctionResult  handleMouseMove( ITableControl& i_tableControl, MouseEvent const & i_event );
        virtual FunctionResult  handleMouseDown( ITableControl& i_tableControl, MouseEvent const & i_event );
        virtual FunctionResult  handleMouseUp( ITableControl& i_tableControl, MouseEvent const & i_event );

    private:
        ColPos  m_nActiveColumn;
    };

//......................................................................................................................
} } // namespace svt::table
//......................................................................................................................

#endif // SVTOOLS_MOUSEFUNCTION_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
