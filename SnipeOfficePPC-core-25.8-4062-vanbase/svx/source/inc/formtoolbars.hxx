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

#ifndef SVX_SOURCE_INC_FORMTOOLBARS_HXX
#define SVX_SOURCE_INC_FORMTOOLBARS_HXX

#include "fmdocumentclassification.hxx"
#include <com/sun/star/frame/XLayoutManager.hpp>

#include <tools/solar.h>

#include <svx/svxdllapi.h>

//........................................................................
namespace svxform
{
//........................................................................

    //====================================================================
    //= FormToolboxes
    //====================================================================
    class FormToolboxes
    {
    private:
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XLayoutManager >
                        m_xLayouter;

    public:
        /** constructs an instance
            @param _rxFrame
                the frame to analyze
        */
        SVX_DLLPUBLIC FormToolboxes(
            const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& _rxFrame
        );

    public:
        /** retrieves the URI for the toolbox associated with the given slot, depending
            on the type of our document
        */
        SVX_DLLPUBLIC ::rtl::OUString
                getToolboxResourceName( sal_uInt16 _nSlotId ) const;

        /** toggles the toolbox associated with the given slot
        */
        SVX_DLLPUBLIC void    toggleToolbox( sal_uInt16 _nSlotId ) const;

        /** determines whether the toolbox associated with the given slot is currently visible
        */
        SVX_DLLPUBLIC bool    isToolboxVisible( sal_uInt16 _nSlotId ) const;

        /** ensures that a given toolbox is visible
        */
        inline void showToolbox( sal_uInt16 _SlotId ) const
        {
            if ( !isToolboxVisible( _SlotId ) )
                toggleToolbox( _SlotId );
        }
    };

//........................................................................
} // namespace svxform
//........................................................................

#endif // SVX_SOURCE_INC_FORMTOOLBARS_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
