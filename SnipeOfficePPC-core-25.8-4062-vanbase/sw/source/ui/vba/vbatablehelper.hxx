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
#ifndef SW_WORD_VBA_TABLEHELPER_HXX
#define SW_WORD_VBA_TABLEHELPER_HXX

#include <com/sun/star/text/XTextTable.hpp>
#include "wordvbahelper.hxx"
#include <swtable.hxx>
#include <tabcol.hxx>

class SwVbaTableHelper
{
private:
    css::uno::Reference< css::text::XTextTable > mxTextTable;
    SwTable* pTable;

private:
    SwTableBox* GetTabBox( sal_Int32 nCol, sal_Int32 nRow ) throw (css::uno::RuntimeException);
    void    InitTabCols( SwTabCols& rCols, const SwTableBox *pStart, sal_Bool bCurRowOnly  = sal_False );
    sal_Int32 GetRightSeparator( SwTabCols& rCols, sal_Int32 nNum) const;
    sal_Int32 GetColCount( SwTabCols& rCols ) const;
    sal_Int32 GetColWidth( SwTabCols& rCols, sal_Int32 nNum ) throw (css::uno::RuntimeException);

public:
    SwVbaTableHelper( const css::uno::Reference< css::text::XTextTable >& xTextTable ) throw (css::uno::RuntimeException);
    ~SwVbaTableHelper() {}
    sal_Int32 getTabColumnsCount( sal_Int32 nRowIndex ) throw (css::uno::RuntimeException);
    sal_Int32 getTabColumnsMaxCount( ) throw (css::uno::RuntimeException);
    sal_Int32 getTabRowIndex( const rtl::OUString& sCellName ) throw (css::uno::RuntimeException);
    sal_Int32 getTabColIndex( const rtl::OUString& sCellName ) throw (css::uno::RuntimeException);
    sal_Int32 getTableWidth( ) throw (css::uno::RuntimeException);

    sal_Int32 GetColWidth( sal_Int32 nCol, sal_Int32 nRow = 0, sal_Bool bCurRowOnly  = sal_False ) throw (css::uno::RuntimeException);
    void SetColWidth( sal_Int32 _width, sal_Int32 nCol, sal_Int32 nRow = 0, sal_Bool bCurRowOnly  = sal_False ) throw (css::uno::RuntimeException);

    static SwTable* GetSwTable( const css::uno::Reference< css::text::XTextTable >& xTextTable ) throw (css::uno::RuntimeException);
    static rtl::OUString getColumnStr( sal_Int32 nCol );
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
