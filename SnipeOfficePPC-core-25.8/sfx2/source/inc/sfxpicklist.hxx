/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#ifndef _SFX_PICKLIST_HXX_
#define _SFX_PICKLIST_HXX_

#include <osl/mutex.hxx>
#include <vcl/menu.hxx>
#include <svl/lstner.hxx>
#include <com/sun/star/util/XStringWidth.hpp>

#include <vector>

#define PICKLIST_MAXSIZE  100

class SfxPickList : public SfxListener
{
    struct PickListEntry
    {
        PickListEntry( const ::rtl::OUString& _aName, const ::rtl::OUString& _aFilter, const ::rtl::OUString& _aTitle ) :
            aName( _aName ), aFilter( _aFilter ), aTitle( _aTitle ) {}

        ::rtl::OUString aName;
        ::rtl::OUString aFilter;
        ::rtl::OUString aTitle;
        ::rtl::OUString aOptions;
    };

    std::vector< PickListEntry* >   m_aPicklistVector;
    sal_uInt32                      m_nAllowedMenuSize;
    ::com::sun::star::uno::Reference< ::com::sun::star::util::XStringWidth > m_xStringLength;

                            SfxPickList( sal_uInt32 nMenuSize );
                            ~SfxPickList();

    void                    CreatePicklistMenuTitle( Menu* pMenu, sal_uInt16 nItemId, const ::rtl::OUString& aURL, sal_uInt32 nNo );
    PickListEntry*          GetPickListEntry( sal_uInt32 nIndex );
    void                    CreatePickListEntries();
    void                    RemovePickListEntries();
    /**
     * Adds the given document to the pick list (recent documents) if it satisfies
       certain requirements, e.g. being writable. Check implementation for requirement
       details.
     */
    void                    AddDocumentToPickList( SfxObjectShell* pDocShell );

    public:
        static SfxPickList& Get();
        static void ensure() { Get(); }

        sal_uInt32          GetAllowedMenuSize() { return m_nAllowedMenuSize; }
        sal_uInt32          GetNumOfEntries() const { return m_aPicklistVector.size(); }
        void                CreateMenuEntries( Menu* pMenu );
        void                ExecuteMenuEntry( sal_uInt16 nId );
        void                ExecuteEntry( sal_uInt32 nIndex );

        virtual void        Notify( SfxBroadcaster& rBC, const SfxHint& rHint );
};

#endif // _SFX_PICKLIST_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
