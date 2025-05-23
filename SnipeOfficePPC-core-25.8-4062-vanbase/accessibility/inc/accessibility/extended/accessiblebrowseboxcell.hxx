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

#ifndef ACCESSIBILITY_ACCESSIBLE_BROWSE_BOX_CELL_HXX
#define ACCESSIBILITY_ACCESSIBLE_BROWSE_BOX_CELL_HXX

#include "accessibility/extended/AccessibleBrowseBoxBase.hxx"
#include <svtools/AccessibleBrowseBoxObjType.hxx>

// .................................................................................
namespace accessibility
{
// .................................................................................

    // =============================================================================
    // = AccessibleBrowseBoxCell
    // =============================================================================
    /** common accessibility-functionality for browse box elements which occupy a cell
    */
    class AccessibleBrowseBoxCell : public AccessibleBrowseBoxBase
    {
    private:
        sal_Int32               m_nRowPos;      // the row number of the table cell
        sal_uInt16              m_nColPos;      // the column id of the table cell

    protected:
        // attribute access
        inline sal_Int32    getRowPos( ) const { return m_nRowPos; }
        inline sal_Int32    getColumnPos( ) const { return m_nColPos; }

    protected:
        // AccessibleBrowseBoxBase overridables
        virtual Rectangle implGetBoundingBox();
        virtual Rectangle implGetBoundingBoxOnScreen();

        // XAccessibleComponent
        virtual void SAL_CALL grabFocus() throw ( ::com::sun::star::uno::RuntimeException );

    protected:
        AccessibleBrowseBoxCell(
            const ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessible >& _rxParent,
            ::svt::IAccessibleTableProvider& _rBrowseBox,
            const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow >& _xFocusWindow,
            sal_Int32 _nRowPos,
            sal_uInt16 _nColPos,
            ::svt::AccessibleBrowseBoxObjType _eType = ::svt::BBTYPE_TABLECELL
        );

        virtual ~AccessibleBrowseBoxCell();

    private:
        AccessibleBrowseBoxCell();                                                  // never implemented
        AccessibleBrowseBoxCell( const AccessibleBrowseBoxCell& );              // never implemented
        AccessibleBrowseBoxCell& operator=( const AccessibleBrowseBoxCell& );   // never implemented
    };

// .................................................................................
}   // namespace accessibility
// .................................................................................


#endif // ACCESSIBILITY_ACCESSIBLE_BROWSE_BOX_CELL_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
