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

#ifndef _EDITATTR_HXX
#define _EDITATTR_HXX

#include <editeng/eeitem.hxx>
#include <svl/poolitem.hxx>

#include <boost/noncopyable.hpp>

class SvxFont;
class SvxFontItem;
class SvxWeightItem;
class SvxPostureItem;
class SvxShadowedItem;
class SvxEscapementItem;
class SvxContourItem;
class SvxCrossedOutItem;
class SvxUnderlineItem;
class SvxOverlineItem;
class SvxFontHeightItem;
class SvxCharScaleWidthItem;
class SvxColorItem;
class SvxAutoKernItem;
class SvxKerningItem;
class SvxWordLineModeItem;
class SvxFieldItem;
class SvxLanguageItem;
class SvxEmphasisMarkItem;
class SvxCharReliefItem;
class SfxVoidItem;
class OutputDevice;

#define CH_FEATURE_OLD  (sal_uInt8)         0xFF
#define CH_FEATURE      (sal_Unicode)   0x01

// DEF_METRIC: For my pool, the DefMetric should always appear when
// GetMetric (nWhich)!
// => To determine the DefMetric simply use GetMetric(0)
#define DEF_METRIC  0

// -------------------------------------------------------------------------
// class EditCharAttrib
// -------------------------------------------------------------------------
// bFeature: Attribute must not expand/shrink, length is always 1
// bEdge: Attribute will not expand, if you want to expand just on the edge
class EditCharAttrib : private boost::noncopyable
{
protected:
    const SfxPoolItem*  pItem;

    sal_uInt16              nStart;
    sal_uInt16              nEnd;
    sal_Bool                bFeature    :1;
    sal_Bool                bEdge       :1;

public:
    EditCharAttrib( const SfxPoolItem& rAttr, sal_uInt16 nStart, sal_uInt16 nEnd );
    virtual ~EditCharAttrib();

    sal_uInt16          Which() const   { return pItem->Which(); }
    const SfxPoolItem*  GetItem() const { return pItem; }

    sal_uInt16&         GetStart()                  { return nStart; }
    sal_uInt16&         GetEnd()                    { return nEnd; }

    sal_uInt16          GetStart() const            { return nStart; }
    sal_uInt16          GetEnd() const              { return nEnd; }

    inline sal_uInt16   GetLen() const;

    inline void     MoveForward( sal_uInt16 nDiff );
    inline void     MoveBackward( sal_uInt16 nDiff );

    inline void     Expand( sal_uInt16 nDiff );
    inline void     Collaps( sal_uInt16 nDiff );

    virtual void    SetFont( SvxFont& rFont, OutputDevice* pOutDev );

    sal_Bool    IsIn( sal_uInt16 nIndex ) const
                { return ( ( nStart <= nIndex ) && ( nEnd >= nIndex ) ); }
    sal_Bool    IsInside( sal_uInt16 nIndex ) const
                { return ( ( nStart < nIndex ) && ( nEnd > nIndex ) ); }
    bool        IsEmpty() const
                { return nStart == nEnd; }

    bool    IsFeature() const   { return bFeature; }
    void    SetFeature( sal_Bool b) { bFeature = b; }

    sal_Bool    IsEdge() const      { return bEdge; }
    void    SetEdge( sal_Bool b )   { bEdge = b; }
};

inline sal_uInt16 EditCharAttrib::GetLen() const
{
    DBG_ASSERT( nEnd >= nStart, "EditCharAttrib: nEnd < nStart!" );
    return nEnd-nStart;
}

inline void EditCharAttrib::MoveForward( sal_uInt16 nDiff )
{
    DBG_ASSERT( ((long)nEnd + nDiff) <= 0xFFFF, "EditCharAttrib: MoveForward?!" );
    nStart = nStart + nDiff;
    nEnd = nEnd + nDiff;
}

inline void EditCharAttrib::MoveBackward( sal_uInt16 nDiff )
{
    DBG_ASSERT( ((long)nStart - nDiff) >= 0, "EditCharAttrib: MoveBackward?!" );
    nStart = nStart - nDiff;
    nEnd = nEnd - nDiff;
}

inline void EditCharAttrib::Expand( sal_uInt16 nDiff )
{
    DBG_ASSERT( ( ((long)nEnd + nDiff) <= (long)0xFFFF ), "EditCharAttrib: Expand?!" );
    DBG_ASSERT( !bFeature, "Please do not expand any features!" );
    nEnd = nEnd + nDiff;
}

inline void EditCharAttrib::Collaps( sal_uInt16 nDiff )
{
    DBG_ASSERT( (long)nEnd - nDiff >= (long)nStart, "EditCharAttrib: Collaps?!" );
    DBG_ASSERT( !bFeature, "Please do not shrink any Features!" );
    nEnd = nEnd - nDiff;
}

// -------------------------------------------------------------------------
// class EditCharAttribFont
// -------------------------------------------------------------------------
class EditCharAttribFont: public EditCharAttrib
{
public:
    EditCharAttribFont( const SvxFontItem& rAttr, sal_uInt16 nStart, sal_uInt16 nEnd );

    virtual void    SetFont( SvxFont& rFont, OutputDevice* pOutDev );
};

// -------------------------------------------------------------------------
// class EditCharAttribWeight
// -------------------------------------------------------------------------
class EditCharAttribWeight : public EditCharAttrib
{
public:
    EditCharAttribWeight( const SvxWeightItem& rAttr, sal_uInt16 nStart, sal_uInt16 nEnd );

    virtual void    SetFont( SvxFont& rFont, OutputDevice* pOutDev );
};
// -------------------------------------------------------------------------
// class EditCharAttribItalic
// -------------------------------------------------------------------------
class EditCharAttribItalic : public EditCharAttrib
{
public:
    EditCharAttribItalic( const SvxPostureItem& rAttr, sal_uInt16 nStart, sal_uInt16 nEnd );

    virtual void    SetFont( SvxFont& rFont, OutputDevice* pOutDev );
};

// -------------------------------------------------------------------------
// class EditCharAttribShadow
// -------------------------------------------------------------------------
class EditCharAttribShadow : public EditCharAttrib
{
public:
    EditCharAttribShadow( const SvxShadowedItem& rAttr, sal_uInt16 nStart, sal_uInt16 nEnd );

    virtual void    SetFont( SvxFont& rFont, OutputDevice* pOutDev );
};

// -------------------------------------------------------------------------
// class EditCharAttribEscapement
// -------------------------------------------------------------------------
class EditCharAttribEscapement : public EditCharAttrib
{
public:
    EditCharAttribEscapement( const SvxEscapementItem& rAttr, sal_uInt16 nStart, sal_uInt16 nEnd );

    virtual void    SetFont( SvxFont& rFont, OutputDevice* pOutDev );
};

// -------------------------------------------------------------------------
// class EditCharAttribOutline
// -------------------------------------------------------------------------
class EditCharAttribOutline : public EditCharAttrib
{
public:
    EditCharAttribOutline( const SvxContourItem& rAttr, sal_uInt16 nStart, sal_uInt16 nEnd );

    virtual void    SetFont( SvxFont& rFont, OutputDevice* pOutDev );
};

// -------------------------------------------------------------------------
// class EditCharAttribStrikeout
// -------------------------------------------------------------------------
class EditCharAttribStrikeout : public EditCharAttrib
{
public:
    EditCharAttribStrikeout( const SvxCrossedOutItem& rAttr, sal_uInt16 nStart, sal_uInt16 nEnd );

    virtual void    SetFont( SvxFont& rFont, OutputDevice* pOutDev );
};

// -------------------------------------------------------------------------
// class EditCharAttribUnderline
// -------------------------------------------------------------------------
class EditCharAttribUnderline : public EditCharAttrib
{
public:
    EditCharAttribUnderline( const SvxUnderlineItem& rAttr, sal_uInt16 nStart, sal_uInt16 nEnd );

    virtual void    SetFont( SvxFont& rFont, OutputDevice* pOutDev );
};

// -------------------------------------------------------------------------
// class EditCharAttribOverline
// -------------------------------------------------------------------------
class EditCharAttribOverline : public EditCharAttrib
{
public:
    EditCharAttribOverline( const SvxOverlineItem& rAttr, sal_uInt16 nStart, sal_uInt16 nEnd );

    virtual void    SetFont( SvxFont& rFont, OutputDevice* pOutDev );
};

// -------------------------------------------------------------------------
// class EditCharAttribEmphasisMark
// -------------------------------------------------------------------------
class EditCharAttribEmphasisMark : public EditCharAttrib
{
public:
    EditCharAttribEmphasisMark( const SvxEmphasisMarkItem& rAttr, sal_uInt16 nStart, sal_uInt16 nEnd );

    virtual void    SetFont( SvxFont& rFont, OutputDevice* pOutDev );
};

// -------------------------------------------------------------------------
// class EditCharAttribRelief
// -------------------------------------------------------------------------
class EditCharAttribRelief : public EditCharAttrib
{
public:
    EditCharAttribRelief( const SvxCharReliefItem& rAttr, sal_uInt16 nStart, sal_uInt16 nEnd );

    virtual void    SetFont( SvxFont& rFont, OutputDevice* pOutDev );
};

// -------------------------------------------------------------------------
// class EditCharAttribFontHeight
// -------------------------------------------------------------------------
class EditCharAttribFontHeight : public EditCharAttrib
{
public:
    EditCharAttribFontHeight( const SvxFontHeightItem& rAttr, sal_uInt16 nStart, sal_uInt16 nEnd );

    virtual void    SetFont( SvxFont& rFont, OutputDevice* pOutDev );
};

// -------------------------------------------------------------------------
// class EditCharAttribFontWidth
// -------------------------------------------------------------------------
class EditCharAttribFontWidth : public EditCharAttrib
{
public:
    EditCharAttribFontWidth( const SvxCharScaleWidthItem& rAttr, sal_uInt16 nStart, sal_uInt16 nEnd );

    virtual void    SetFont( SvxFont& rFont, OutputDevice* pOutDev );
};

// -------------------------------------------------------------------------
// class EditCharAttribColor
// -------------------------------------------------------------------------
class EditCharAttribColor : public EditCharAttrib
{
public:
    EditCharAttribColor( const SvxColorItem& rAttr, sal_uInt16 nStart, sal_uInt16 nEnd );

    virtual void    SetFont( SvxFont& rFont, OutputDevice* pOutDev );
};

// -------------------------------------------------------------------------
// class EditCharAttribLanguage
// -------------------------------------------------------------------------
class EditCharAttribLanguage : public EditCharAttrib
{
public:
    EditCharAttribLanguage( const SvxLanguageItem& rAttr, sal_uInt16 nStart, sal_uInt16 nEnd );

    virtual void    SetFont( SvxFont& rFont, OutputDevice* pOutDev );
};

// -------------------------------------------------------------------------
// class EditCharAttribTab
// -------------------------------------------------------------------------
class EditCharAttribTab : public EditCharAttrib
{
public:
    EditCharAttribTab( const SfxVoidItem& rAttr, sal_uInt16 nPos );

    virtual void    SetFont( SvxFont& rFont, OutputDevice* pOutDev );
};

// -------------------------------------------------------------------------
// class EditCharAttribLineBreak
// -------------------------------------------------------------------------
class EditCharAttribLineBreak : public EditCharAttrib
{
public:
    EditCharAttribLineBreak( const SfxVoidItem& rAttr, sal_uInt16 nPos );

    virtual void    SetFont( SvxFont& rFont, OutputDevice* pOutDev );
};

// -------------------------------------------------------------------------
// class EditCharAttribField
// -------------------------------------------------------------------------
class EditCharAttribField: public EditCharAttrib
{
    rtl::OUString   aFieldValue;
    Color*          pTxtColor;
    Color*          pFldColor;

    EditCharAttribField& operator = ( const EditCharAttribField& rAttr ) const;

public:
    EditCharAttribField( const SvxFieldItem& rAttr, sal_uInt16 nPos );
    EditCharAttribField( const EditCharAttribField& rAttr );
    ~EditCharAttribField();

    bool operator == ( const EditCharAttribField& rAttr ) const;
    bool operator != ( const EditCharAttribField& rAttr ) const
                                    { return !(operator == ( rAttr ) ); }

    virtual void    SetFont( SvxFont& rFont, OutputDevice* pOutDev );
    Color*&         GetTxtColor()           { return pTxtColor; }
    Color*&         GetFldColor()           { return pFldColor; }

    const rtl::OUString& GetFieldValue() const;
    void SetFieldValue(const rtl::OUString& rVal);

    void Reset();
};

// -------------------------------------------------------------------------
// class EditCharAttribPairKerning
// -------------------------------------------------------------------------
class EditCharAttribPairKerning : public EditCharAttrib
{
public:
    EditCharAttribPairKerning( const SvxAutoKernItem& rAttr, sal_uInt16 nStart, sal_uInt16 nEnd );

    virtual void    SetFont( SvxFont& rFont, OutputDevice* pOutDev );
};

// -------------------------------------------------------------------------
// class EditCharAttribKerning
// -------------------------------------------------------------------------
class EditCharAttribKerning : public EditCharAttrib
{
public:
    EditCharAttribKerning( const SvxKerningItem& rAttr, sal_uInt16 nStart, sal_uInt16 nEnd );

    virtual void    SetFont( SvxFont& rFont, OutputDevice* pOutDev );
};

// -------------------------------------------------------------------------
// class EditCharAttribWordLineMode
// -------------------------------------------------------------------------
class EditCharAttribWordLineMode: public EditCharAttrib
{
public:
    EditCharAttribWordLineMode( const SvxWordLineModeItem& rAttr, sal_uInt16 nStart, sal_uInt16 nEnd );

    virtual void    SetFont( SvxFont& rFont, OutputDevice* pOutDev );
};


#endif // _EDITATTR_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
