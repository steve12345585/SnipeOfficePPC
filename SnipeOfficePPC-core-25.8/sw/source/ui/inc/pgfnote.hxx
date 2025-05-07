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
#ifndef _PGFNOTE_HXX
#define _PGFNOTE_HXX

#include <sfx2/tabdlg.hxx>

#include <vcl/field.hxx>

#include <vcl/fixed.hxx>

#include <vcl/group.hxx>
#include <svtools/ctrlbox.hxx>

/*--------------------------------------------------------------------
    Description:    footnote settings TabPage
 --------------------------------------------------------------------*/
class SwFootNotePage: public SfxTabPage
{
public:
    static SfxTabPage *Create(Window *pParent, const SfxItemSet &rSet);
    static sal_uInt16* GetRanges();

    virtual sal_Bool FillItemSet(SfxItemSet &rSet);
    virtual void Reset(const SfxItemSet &rSet);

private:
    SwFootNotePage(Window *pParent, const SfxItemSet &rSet);
    ~SwFootNotePage();

    FixedLine       aPosHeader;
    RadioButton     aMaxHeightPageBtn;
    RadioButton     aMaxHeightBtn;
    MetricField     aMaxHeightEdit;
    FixedText       aDistLbl;
    MetricField     aDistEdit;

    FixedLine       aLineHeader;
    FixedText       aLinePosLbl;
    ListBox         aLinePosBox;
    FixedText       aLineTypeLbl;
    LineListBox     aLineTypeBox;
    FixedText       aLineWidthLbl;
    MetricField     aLineWidthEdit;
    FixedText       aLineColorLbl;
    ColorListBox    aLineColorBox;
    FixedText       aLineLengthLbl;
    MetricField     aLineLengthEdit;
    FixedText       aLineDistLbl;
    MetricField     aLineDistEdit;

    DECL_LINK(HeightPage, void *);
    DECL_LINK(HeightMetric, void *);
    DECL_LINK(HeightModify, void *);
    DECL_LINK( LineWidthChanged_Impl, void * );
    DECL_LINK( LineColorSelected_Impl, void * );

    long            lMaxHeight;

    using SfxTabPage::ActivatePage;
    using SfxTabPage::DeactivatePage;

    virtual void    ActivatePage( const SfxItemSet& rSet );
    virtual int     DeactivatePage( SfxItemSet* pSet = 0 );

};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
