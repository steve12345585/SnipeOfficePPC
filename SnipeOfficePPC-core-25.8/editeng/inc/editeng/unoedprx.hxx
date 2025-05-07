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

#ifndef _SVX_UNOEDPRX_HXX
#define _SVX_UNOEDPRX_HXX

#include <memory>
#include <svl/brdcst.hxx>
#include <editeng/unoedsrc.hxx>

#include <editeng/editdata.hxx>
#include "editeng/editengdllapi.h"

class SvxAccessibleTextAdapter : public SvxTextForwarder
{
public:
    SvxAccessibleTextAdapter();
    virtual ~SvxAccessibleTextAdapter();

    virtual sal_uInt16          GetParagraphCount() const;
    virtual sal_uInt16          GetTextLen( sal_uInt16 nParagraph ) const;
    virtual String          GetText( const ESelection& rSel ) const;
    virtual SfxItemSet      GetAttribs( const ESelection& rSel, sal_Bool bOnlyHardAttrib = 0 ) const;
    virtual SfxItemSet      GetParaAttribs( sal_uInt16 nPara ) const;
    virtual void            SetParaAttribs( sal_uInt16 nPara, const SfxItemSet& rSet );
    virtual void            RemoveAttribs( const ESelection& rSelection, sal_Bool bRemoveParaAttribs, sal_uInt16 nWhich );
    virtual void            GetPortions( sal_uInt16 nPara, std::vector<sal_uInt16>& rList ) const;

    virtual sal_Int32       CalcLogicalIndex( sal_uInt16 nPara, sal_uInt16 nEEIndex );
    virtual sal_uInt16          CalcEditEngineIndex( sal_uInt16 nPara, sal_Int32 nLogicalIndex );

     virtual sal_uInt16         GetItemState( const ESelection& rSel, sal_uInt16 nWhich ) const;
    virtual sal_uInt16          GetItemState( sal_uInt16 nPara, sal_uInt16 nWhich ) const;

    virtual void            QuickInsertText( const String& rText, const ESelection& rSel );
    virtual void            QuickInsertField( const SvxFieldItem& rFld, const ESelection& rSel );
    virtual void            QuickSetAttribs( const SfxItemSet& rSet, const ESelection& rSel );
    virtual void            QuickInsertLineBreak( const ESelection& rSel );

    virtual SfxItemPool*    GetPool() const;

    virtual XubString       CalcFieldValue( const SvxFieldItem& rField, sal_uInt16 nPara, sal_uInt16 nPos, Color*& rpTxtColor, Color*& rpFldColor );
    virtual void            FieldClicked( const SvxFieldItem& rField, sal_uInt16 nPara, xub_StrLen nPos );

    virtual sal_Bool            IsValid() const;

    virtual LanguageType    GetLanguage( sal_uInt16, sal_uInt16 ) const;
    virtual sal_uInt16          GetFieldCount( sal_uInt16 nPara ) const;
    virtual EFieldInfo      GetFieldInfo( sal_uInt16 nPara, sal_uInt16 nField ) const;
    virtual EBulletInfo     GetBulletInfo( sal_uInt16 nPara ) const;
    virtual Rectangle       GetCharBounds( sal_uInt16 nPara, sal_uInt16 nIndex ) const;
    virtual Rectangle       GetParaBounds( sal_uInt16 nPara ) const;
    virtual MapMode         GetMapMode() const;
    virtual OutputDevice*   GetRefDevice() const;
    virtual sal_Bool        GetIndexAtPoint( const Point&, sal_uInt16& nPara, sal_uInt16& nIndex ) const;
    virtual sal_Bool        GetWordIndices( sal_uInt16 nPara, sal_uInt16 nIndex, sal_uInt16& nStart, sal_uInt16& nEnd ) const;
    virtual sal_Bool        GetAttributeRun( sal_uInt16& nStartIndex, sal_uInt16& nEndIndex, sal_uInt16 nPara, sal_uInt16 nIndex ) const;
    virtual sal_uInt16          GetLineCount( sal_uInt16 nPara ) const;
    virtual sal_uInt16          GetLineLen( sal_uInt16 nPara, sal_uInt16 nLine ) const;
    virtual void            GetLineBoundaries( /*out*/sal_uInt16 &rStart, /*out*/sal_uInt16 &rEnd, sal_uInt16 nParagraph, sal_uInt16 nLine ) const;
    virtual sal_uInt16          GetLineNumberAtIndex( sal_uInt16 nPara, sal_uInt16 nIndex ) const;

    virtual sal_Bool        Delete( const ESelection& );
    virtual sal_Bool        InsertText( const String&, const ESelection& );
    virtual sal_Bool        QuickFormatDoc( sal_Bool bFull=sal_False );
    virtual sal_Int16       GetDepth( sal_uInt16 nPara ) const;
    virtual sal_Bool        SetDepth( sal_uInt16 nPara, sal_Int16 nNewDepth );

    virtual const SfxItemSet*   GetEmptyItemSetPtr();

    // implementation functions for XParagraphAppend and XTextPortionAppend
    // (not needed for accessibility, only for new import API)
    virtual void        AppendParagraph();
    virtual xub_StrLen  AppendTextPortion( sal_uInt16 nPara, const String &rText, const SfxItemSet &rSet );

    //XTextCopy
    virtual void        CopyText(const SvxTextForwarder& rSource);

    void                    SetForwarder( SvxTextForwarder& );
    sal_Bool                HaveImageBullet( sal_uInt16 nPara ) const;
    sal_Bool                HaveTextBullet( sal_uInt16 nPara ) const;

    /** Query whether all text in given selection is editable

        @return sal_True if every character in the given selection can
        be changed, and sal_False if e.g. a field or a bullet is
        contained therein.
     */
    sal_Bool                IsEditable( const ESelection& rSelection );

private:
    SvxTextForwarder* mrTextForwarder;
};

class SvxAccessibleTextEditViewAdapter : public SvxEditViewForwarder
{
public:

                        SvxAccessibleTextEditViewAdapter();
    virtual             ~SvxAccessibleTextEditViewAdapter();

    // SvxViewForwarder interface
    virtual sal_Bool        IsValid() const;
    virtual Rectangle   GetVisArea() const;
    virtual Point       LogicToPixel( const Point& rPoint, const MapMode& rMapMode ) const;
    virtual Point       PixelToLogic( const Point& rPoint, const MapMode& rMapMode ) const;

    // SvxEditViewForwarder interface
    virtual sal_Bool    GetSelection( ESelection& rSelection ) const;
    virtual sal_Bool    SetSelection( const ESelection& rSelection );
    virtual sal_Bool    Copy();
    virtual sal_Bool    Cut();
    virtual sal_Bool    Paste();

    void                SetForwarder( SvxEditViewForwarder&, SvxAccessibleTextAdapter& );

private:
    SvxEditViewForwarder*       mrViewForwarder;
    SvxAccessibleTextAdapter*   mrTextForwarder;
};

class EDITENG_DLLPUBLIC SvxEditSourceAdapter : public SvxEditSource
{
public:
    SvxEditSourceAdapter();
    ~SvxEditSourceAdapter();

    virtual SvxEditSource*                      Clone() const;
    virtual SvxTextForwarder*                   GetTextForwarder();
    SvxAccessibleTextAdapter*                   GetTextForwarderAdapter(); // covariant return types don't work on MSVC
     virtual SvxViewForwarder*                  GetViewForwarder();
     virtual SvxEditViewForwarder*              GetEditViewForwarder( sal_Bool bCreate = sal_False );
     SvxAccessibleTextEditViewAdapter*          GetEditViewForwarderAdapter( sal_Bool bCreate = sal_False ); // covariant return types don't work on MSVC
    virtual void                                UpdateData();
    virtual SfxBroadcaster&                     GetBroadcaster() const;

    SAL_WNODEPRECATED_DECLARATIONS_PUSH
    void        SetEditSource( ::std::auto_ptr< SvxEditSource > pAdaptee );
    SAL_WNODEPRECATED_DECLARATIONS_POP
    sal_Bool    IsValid() const;

private:
    // declared, but not defined
    SvxEditSourceAdapter( const SvxEditSourceAdapter& );
    SvxEditSourceAdapter& operator= ( const SvxEditSourceAdapter& );

    SAL_WNODEPRECATED_DECLARATIONS_PUSH
    ::std::auto_ptr< SvxEditSource >    mpAdaptee;
    SAL_WNODEPRECATED_DECLARATIONS_POP

    SvxAccessibleTextAdapter            maTextAdapter;
    SvxAccessibleTextEditViewAdapter    maEditViewAdapter;

    mutable SfxBroadcaster              maDummyBroadcaster;

    sal_Bool                            mbEditSourceValid;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
