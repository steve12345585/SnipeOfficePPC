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
#include "lwpglobalmgr.hxx"
#include "lwpsilverbullet.hxx"
#include "lwpdoc.hxx"
#include "lwpdivinfo.hxx"
#include "lwpfoundry.hxx"
#include "lwpstory.hxx"
#include "lwppara.hxx"
#include "xfilter/xfliststyle.hxx"
#include "xfilter/xfstylemanager.hxx"

LwpSilverBullet::LwpSilverBullet(LwpObjectHeader& objHdr, LwpSvStream* pStrm)
    : LwpDLNFVList(objHdr, pStrm), m_pAtomHolder(new LwpAtomHolder), m_pBulletPara(NULL)
{
}

LwpSilverBullet::~LwpSilverBullet()
{
    if (m_pAtomHolder)
    {
        delete m_pAtomHolder;
    }
}

void LwpSilverBullet::Read()
{
    LwpDLNFVList::Read();

    m_nFlags = m_pObjStrm->QuickReaduInt16();
    m_aStory.ReadIndexed(m_pObjStrm);

    sal_uInt16 nNumPos = m_pObjStrm->QuickReaduInt16();

    for (sal_uInt8 nC = 0; nC < nNumPos; nC++)
        m_pResetPositionFlags[nC] = m_pObjStrm->QuickReaduInt8();

    m_nUseCount = m_pObjStrm->QuickReaduInt32();

    m_pAtomHolder->Read(m_pObjStrm);
}

/**
 * @short:   Register bullet or numbering style-list and store the returned
 *          name from XFStyleManager.
 * @descr:
 */
void LwpSilverBullet::RegisterStyle()
{
    XFListStyle* pListStyle = new XFListStyle();
    XFStyleManager* pXFStyleManager = LwpGlobalMgr::GetInstance()->GetXFStyleManager();

    this->GetBulletPara();

    if (this->IsBulletOrdered() && this->HasName())
    {
        //todo: find the flag in the file
        sal_Bool bCumulative = sal_False;

        for (sal_uInt8 nPos = 1; nPos < 10; nPos++)
        {
            ParaNumbering aParaNumbering;
            //get numbering format according to the position.
            m_pBulletPara->GetParaNumber(nPos, &aParaNumbering);
            LwpFribParaNumber* pParaNumber = aParaNumbering.pParaNumber;
            if (pParaNumber)
            {
                if (pParaNumber->GetStyleID() != NUMCHAR_other)
                {
                    m_pHideLevels[nPos] = aParaNumbering.nNumLevel;
                    sal_uInt16 nDisplayLevel = this->GetDisplayLevel(nPos);
                    bCumulative = (sal_Bool)(nDisplayLevel > 1);
                    rtl::OUString aPrefix = this->GetAdditionalName(nPos);

                    XFNumFmt aFmt;
                    if (!bCumulative && aParaNumbering.pPrefix)
                    {
                        aFmt.SetPrefix(aPrefix + aParaNumbering.pPrefix->GetText());
                    }

                    aFmt.SetFormat(this->GetNumCharByStyleID(pParaNumber));

                    if (aParaNumbering.pSuffix)
                    {
                        aFmt.SetSuffix(aParaNumbering.pSuffix->GetText());
                    }

                    //set numbering format into the style-list.
                    pListStyle->SetListNumber(nPos, aFmt, pParaNumber->GetStart()+1);

                    if (bCumulative && nPos > 1)
                    {
                        pListStyle->SetDisplayLevel(nPos, nDisplayLevel);
                    }

                }
                else
                {
                    rtl::OUString aPrefix, aSuffix;
                    if (aParaNumbering.pPrefix)
                    {
                        aPrefix = aParaNumbering.pPrefix->GetText();
                    }
                    if (aParaNumbering.pSuffix)
                    {
                        aSuffix = aParaNumbering.pSuffix->GetText();
                    }

                    pListStyle->SetListBullet(nPos, this->GetNumCharByStyleID(pParaNumber).toChar(),
                        rtl::OUString("Times New Roman"), aPrefix, aSuffix);
                }

                pListStyle->SetListPosition(nPos, 0.0, 0.635, 0.0);
                aParaNumbering.clear();
            }
        }
    }

    //add style-list to style manager.
    m_strStyleName = pXFStyleManager->AddStyle(pListStyle)->GetStyleName();
}

/**
 * @short:   Get the font name of the bullet.
 * @descr:
 * @return:  Font name of the bullet.
 */
rtl::OUString LwpSilverBullet::GetBulletFontName()
{
    rtl::OUString aEmpty;

    //foundry has been set?
    if (!m_pFoundry)
    {
        return aEmpty;
    }

    LwpFontManager* pFontMgr = m_pFoundry->GetFontManger();
    if (!pFontMgr)
    {
        return aEmpty;
    }

    sal_uInt32 nBulletFontID = m_pBulletPara->GetBulletFontID();
    sal_uInt16 nFinalFont = static_cast<sal_uInt16>((nBulletFontID >> 16) & 0xFFFF);

    //final fontid is valid?
    if (nFinalFont > 255 || nFinalFont == 0)
    {
        return aEmpty;
    }

    //get font name from font manager.
    rtl::OUString aFontName = pFontMgr->GetNameByID(nBulletFontID);

    return aFontName;
}
/**
 * @short:   Get bullet character of the bullet vo_para.
 * @descr:
 * @return:  An UChar32 bulle character.
 */
UChar32 LwpSilverBullet::GetBulletChar()
{
    rtl::OUString aBulletChar = m_pBulletPara->GetBulletChar();

    return aBulletChar.toChar();
}

/**
 * @short:   Get the LwpPara object through story id.
 */
LwpPara* LwpSilverBullet::GetBulletPara()
{
    if (!m_pBulletPara)
    {
        LwpStory* pStory = dynamic_cast<LwpStory*>(m_aStory.obj(VO_STORY));
        if (!pStory)
        {
            return NULL;
        }

        m_pBulletPara = dynamic_cast<LwpPara*>(pStory->GetFirstPara()->obj(VO_PARA));
    }

    return m_pBulletPara;
}

/**
 * @short:   Get numbering character of the bullet vo_para.
 * @descr:
 * @param:   pParaNumber a pionter to the structure LwpFribParaNumber which
 *          includes numbering prefix, format and suffix.
 * @return:  An OUString object which store the numbering character.
 */
rtl::OUString LwpSilverBullet::GetNumCharByStyleID(LwpFribParaNumber* pParaNumber)
{
    rtl::OUString aEmpty;

    if (!pParaNumber)
    {
        assert(false);
        return aEmpty;
    }

    rtl::OUString strNumChar("1");
    sal_uInt16 nStyleID = pParaNumber->GetStyleID();
    UChar32 uC = 0x0000;

    switch (nStyleID)
    {
    case NUMCHAR_1:
    case NUMCHAR_01:
    case NUMCHAR_Chinese4:
        strNumChar =  rtl::OUString("1");
        break;
    case NUMCHAR_A :
        strNumChar =  rtl::OUString("A");
        break;
    case NUMCHAR_a:
        strNumChar = rtl::OUString("a");
        break;
    case NUMCHAR_I:
        strNumChar = rtl::OUString("I");
        break;
    case NUMCHAR_i:
        strNumChar = rtl::OUString("i");
        break;
    case NUMCHAR_other:
        uC = static_cast<UChar32>(pParaNumber->GetNumberChar());
        strNumChar = rtl::OUString(uC);
        break;
    case NUMCHAR_Chinese1:
        {
        sal_Unicode sBuf[13] = {0x58f9,0x002c,0x0020,0x8d30,0x002c,0x0020,0x53c1,0x002c,0x0020,0x002e,0x002e,0x002e,0x0};
        strNumChar = rtl::OUString(sBuf);
        }
        break;
    case NUMCHAR_Chinese2:
        {
        sal_Unicode sBuf[13] = {0x4e00,0x002c,0x0020,0x4e8c,0x002c,0x0020,0x4e09,0x002c,0x0020,0x002e,0x002e,0x002e,0x0};
        strNumChar = rtl::OUString(sBuf);
        }
        break;
    case NUMCHAR_Chinese3:
        {
        sal_Unicode sBuf[13] = {0x7532,0x002c,0x0020,0x4e59,0x002c,0x0020,0x4e19,0x002c,0x0020,0x002e,0x002e,0x002e,0x0};
        strNumChar = rtl::OUString(sBuf);
        }
        break;
    case NUMCHAR_none:
        strNumChar = aEmpty;
        break;
    default:
        break;
    }//mod end
    return strNumChar;
}

/**
 * @short:   To judge the silverbullet list is ordered or not.
 * @descr:
 * @return:  sal_True if list is ordered, sal_False if list is unordered.
 */
sal_Bool LwpSilverBullet::IsBulletOrdered()
{
    if (!m_pBulletPara)
        return sal_False;

    LwpFribPtr* pFribs = m_pBulletPara->GetFribs();

    if (!pFribs)
        return sal_False;

    return (sal_Bool)(pFribs->HasFrib(FRIB_TAG_PARANUMBER) != NULL);
}

/**
 * @short:   Calculate the displaylevel according to the position and hidelevels.
 * @descr:
 * @param:   nPos position of the numbering.
 * @return:  displaylevel of the position passed in.
 */
sal_uInt16 LwpSilverBullet::GetDisplayLevel(sal_uInt8 nPos)
{
    if (nPos > 1)
    {
        sal_uInt16 nHideBit = (1 << nPos);
        for (sal_uInt8 nC = nPos-1; nC > 0; nC--)
        {
            sal_uInt16 nAttrMask = ~m_pHideLevels[nC];
            if (!(nAttrMask & nHideBit))
            {
                return static_cast<sal_uInt16>(nPos - nC);
            }
        }
    }

    return static_cast<sal_uInt16>(nPos);
}

/**
 * @descr:   Get the additional information, "Division name" or "Section Name" from document
 *      variable frib according to numbering position. Whether we should add a
 *      Division/Section name or not is determined by the hidelevels of the frib.
 * @param:   nPos position of the numbering.
 * @return:  Division or Section name.
 */
rtl::OUString LwpSilverBullet::GetAdditionalName(sal_uInt8 nPos)
{
    rtl::OUString aRet, aEmpty;
    sal_uInt16 nHideBit = (1 << nPos);
    sal_Bool bDivisionName = sal_False;
    sal_Bool bSectionName = sal_False;

    LwpFrib* pParaFrib = m_pBulletPara->GetFribs()->GetFribs();
    if (!pParaFrib)
    {
        return aEmpty;
    }

    while (pParaFrib)
    {
        if (pParaFrib->GetType() == FRIB_TAG_DOCVAR)
        {
            ModifierInfo* pMoInfo = pParaFrib->GetModifiers();
            if (!pMoInfo)
            {
                return aEmpty;
            }
            sal_uInt16 nHideLevels = pMoInfo->aTxtAttrOverride.GetHideLevels();
            sal_uInt16 nType = static_cast<LwpFribDocVar*>(pParaFrib)->GetType();

            if (~nHideLevels & nHideBit)
            {
                if (nType == 0x000D)
                {
                    bDivisionName = sal_True;
                }
                else if (nType == 0x000E)
                {
                    bSectionName= sal_True;
                }
            }
        }
        pParaFrib = pParaFrib->GetNext();
    }

    if (bDivisionName)
    {
        aRet += this->GetDivisionName();
    }
    if (bSectionName)
    {
        aRet += this->GetSectionName();
    }

    return aRet;
}

rtl::OUString LwpSilverBullet::GetDivisionName()
{
    rtl::OUString aRet;

    if (!m_pFoundry)
    {
        return aRet;
    }

    LwpDocument* pDoc = m_pFoundry->GetDocument();
    if (pDoc)
    {
        LwpObjectID* pID = pDoc->GetDivInfoID();
        if (!pID->IsNull())
        {
            LwpDivInfo *pInfo = dynamic_cast<LwpDivInfo*>(pID->obj(VO_DIVISIONINFO));
            if (pInfo)
                aRet = pInfo->GetDivName();
        }
    }

    return aRet;
}

rtl::OUString LwpSilverBullet::GetSectionName()
{
    rtl::OUString aEmpty;
    LwpStory* pStory = dynamic_cast<LwpStory*>(m_aStory.obj(VO_STORY));
    if (!pStory)
    {
        return aEmpty;
    }

    return pStory->GetSectionName();
}

sal_Bool LwpSilverBullet::HasName()
{
    LwpAtomHolder* pName = this->GetName();
    if (pName)
    {
        return (sal_Bool)(!pName->str().isEmpty());
    }
    else
    {
        return sal_False;
    }
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
