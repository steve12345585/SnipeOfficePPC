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

#ifndef _PARALIST_HXX
#define _PARALIST_HXX

#include <sal/config.h>
#include <sal/log.hxx>

#include <vector>

#include <tools/link.hxx>

class Paragraph;

class ParagraphList
{
public:
    void            Clear( sal_Bool bDestroyParagraphs );

    sal_Int32       GetParagraphCount() const
    {
        size_t nSize = maEntries.size();
        if (nSize > SAL_MAX_INT32)
        {
            SAL_WARN( "editeng", "ParagraphList::GetParagraphCount - overflow " << nSize);
            return SAL_MAX_INT32;
        }
        return nSize;
    }

    Paragraph*      GetParagraph( sal_Int32 nPos ) const
    {
        return 0 <= nPos && static_cast<size_t>(nPos) < maEntries.size() ? maEntries[nPos] : NULL;
    }

    sal_Int32       GetAbsPos( Paragraph* pParent ) const;

    void            Append( Paragraph *pPara);
    void            Insert( Paragraph* pPara, sal_Int32 nAbsPos);
    void            Remove( sal_Int32 nPara );
    void            MoveParagraphs( sal_Int32 nStart, sal_Int32 nDest, sal_Int32 nCount );

    Paragraph*      GetParent( Paragraph* pParagraph /*, sal_uInt16& rRelPos */ ) const;
    sal_Bool            HasChildren( Paragraph* pParagraph ) const;
    sal_Bool            HasHiddenChildren( Paragraph* pParagraph ) const;
    sal_Bool            HasVisibleChildren( Paragraph* pParagraph ) const;
    sal_Int32       GetChildCount( Paragraph* pParagraph ) const;

    void            Expand( Paragraph* pParent );
    void            Collapse( Paragraph* pParent );

    void            SetVisibleStateChangedHdl( const Link& rLink ) { aVisibleStateChangedHdl = rLink; }
    Link            GetVisibleStateChangedHdl() const { return aVisibleStateChangedHdl; }

private:

    Link aVisibleStateChangedHdl;
    std::vector<Paragraph*> maEntries;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
