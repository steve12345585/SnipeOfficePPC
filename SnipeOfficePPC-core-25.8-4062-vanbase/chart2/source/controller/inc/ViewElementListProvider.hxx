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
#ifndef _CHART2_VIEWELEMENTLISTPROVIDER_HXX
#define _CHART2_VIEWELEMENTLISTPROVIDER_HXX

#include <svx/xtable.hxx>
#include <svx/svdpage.hxx>

class FontList;

//.............................................................................
namespace chart
{
//.............................................................................

class DrawModelWrapper;

class ViewElementListProvider
{
public:
    ViewElementListProvider( DrawModelWrapper* pDrawModelWrapper );
    virtual ~ViewElementListProvider();

    XColorListRef     GetColorTable() const;
    XDashListRef      GetDashList() const;
    XLineEndListRef   GetLineEndList() const;
    XGradientListRef  GetGradientList() const;
    XHatchListRef     GetHatchList() const;
    XBitmapListRef    GetBitmapList() const;

    //create chartspecific symbols for linecharts
    SdrObjList*     GetSymbolList() const;
    Graphic         GetSymbolGraphic( sal_Int32 nStandardSymbol, const SfxItemSet* pSymbolShapeProperties ) const;

    FontList*       getFontList() const;
    //SfxPrinter*   getPrinter();

private:
    DrawModelWrapper*   m_pDrawModelWrapper;
    mutable FontList*   m_pFontList;
};

//.............................................................................
} //namespace chart
//.............................................................................
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
