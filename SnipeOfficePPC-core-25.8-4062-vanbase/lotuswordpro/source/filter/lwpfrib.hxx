/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: IBM Corporation
 *
 *  Copyright: 2008 by IBM Corporation
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/
/*************************************************************************
 * @file
 *  For LWP filter architecture prototype
 ************************************************************************/
/*************************************************************************
 * Change History
 Jan 2005           Created
 ************************************************************************/
#ifndef _LWPFRIB_HXX_
#define _LWPFRIB_HXX_
#include "lwpobjid.hxx"
#include "lwpobjstrm.hxx"
#include "lwpbasetype.hxx"
#include "lwpoverride.hxx"
#include "lwpfoundry.hxx"
#include "xfilter/xfcolor.hxx"

struct ModifierInfo
{
    sal_uInt32 FontID;
    LwpObjectID CharStyleID;
    LwpTextLanguageOverride Language;
    //add by , 02/22/2005
    LwpTextAttributeOverride aTxtAttrOverride;
    //end add
    sal_uInt16 CodePage;
    sal_Bool HasCharStyle;
    sal_Bool HasLangOverride;
    sal_Bool HasHighLight;
    sal_uInt8 RevisionType;
    sal_Bool RevisionFlag;
};

class LwpPara;
class LwpHyperlinkMgr;
class LwpFrib
{
public:
    LwpFrib(LwpPara* pPara);
    virtual ~LwpFrib();
    static LwpFrib* CreateFrib(LwpPara* pPara, LwpObjectStream* pObjStrm, sal_uInt8 fribtag, sal_uInt8 editID);
    virtual void Read(LwpObjectStream* pObjStrm, sal_uInt16 len);
//  virtual void Parse(IXFStream* pOutputStream);
    LwpFrib* GetNext(){return m_pNext;}
    void SetNext(LwpFrib* next){m_pNext = next;}
    sal_uInt8 GetType() { return m_nFribType;}
    void SetType(sal_uInt8 type) { m_nFribType = type;}
    void SetEditor(sal_uInt8 editor) { m_nEditor = editor;}
    OUString GetEditor();
    XFColor GetHighLightColor();
protected:
    LwpPara* m_pPara;
    LwpFrib* m_pNext;
    sal_uInt8 m_nFribType;
    ModifierInfo* m_pModifiers;
    rtl::OUString m_StyleName;
public:
    sal_Bool m_ModFlag;
    rtl::OUString GetStyleName(){return m_StyleName;}//add by  1-10
//  void SetStyle(LwpFoundry* pFoundry);
    sal_Bool IsModified(){return m_ModFlag;}
    void SetModifiers(ModifierInfo* pModifiers);
    ModifierInfo* GetModifiers(){return m_pModifiers;}
    virtual void RegisterStyle(LwpFoundry* pFoundry);
    sal_Bool HasNextFrib();
    void ConvertChars(XFContentContainer* pXFPara,OUString text);
    void ConvertHyperLink(XFContentContainer* pXFPara,LwpHyperlinkMgr* pHyperlink,OUString text);
    XFFont* GetFont();

    sal_uInt8 GetRevisionType(){return m_nRevisionType;}
    sal_Bool GetRevisionFlag(){return m_bRevisionFlag;}
    enum{
        REV_INSERT =0,
        REV_DELETE = 1,
    };
    LwpPara* GetMyPara(){return m_pPara;}
private:
    static void ReadModifiers(LwpObjectStream* pObjStrm,ModifierInfo* pModInfo);
protected:
    sal_uInt8 m_nRevisionType;
    sal_Bool m_bRevisionFlag;
    sal_uInt8 m_nEditor;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
