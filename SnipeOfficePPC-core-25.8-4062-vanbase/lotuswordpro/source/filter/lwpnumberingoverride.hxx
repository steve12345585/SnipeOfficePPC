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
* Numbering override of Wordpro.
************************************************************************/
#ifndef     _LWPNUMBERINGOVERRIDE_HXX
#define     _LWPNUMBERINGOVERRIDE_HXX

#include    "lwpoverride.hxx"

class LwpObjectStream;
class LwpNumberingOverride : public LwpOverride
{
public:
    LwpNumberingOverride();
    virtual ~LwpNumberingOverride(){}

    virtual LwpNumberingOverride* clone() const;

    enum
    {
        NO_LEVEL        = 0x0001,
        NO_POSITION     = 0x0002,
        HEADING         = 0x0004,
        SMARTLEVEL      = 0x0008
    };
public:
    virtual void Read(LwpObjectStream *pStrm);

    inline sal_uInt16 GetLevel() const;
    inline sal_uInt16 GetPosition() const;

    inline sal_Bool IsLevelOverridden();
    inline sal_Bool IsPositionOverridden();
    inline sal_Bool IsHeadingOverridden();
    inline sal_Bool IsSmartLevelOverridden();
    inline sal_Bool IsHeading();
    inline sal_Bool IsSmartLevel();

    void OverrideLevel(sal_uInt16 nNewLv);

    inline void RevertLevel();
    inline void RevertPosition();
    inline void RevertHeading();
    inline void RevertSmartLevel();
    //end add

protected:
    LwpNumberingOverride(LwpNumberingOverride const& rOther);

private:
    LwpNumberingOverride& operator=(LwpNumberingOverride const& rOther); // not implemented

private:
    sal_uInt16  m_nLevel;
    sal_uInt16  m_nPosition;
};

inline sal_uInt16 LwpNumberingOverride::GetLevel() const
{
    return m_nLevel;
}

inline sal_uInt16 LwpNumberingOverride::GetPosition() const
{
    return m_nPosition;
}

inline sal_Bool LwpNumberingOverride::IsLevelOverridden()
{
    return (sal_Bool)((m_nOverride & NO_LEVEL) != 0);
}
inline sal_Bool LwpNumberingOverride::IsPositionOverridden()
{
    return (sal_Bool)((m_nOverride & NO_POSITION) != 0);
}
inline sal_Bool LwpNumberingOverride::IsHeadingOverridden()
{
    return (sal_Bool)((m_nOverride & HEADING) != 0);
}
inline sal_Bool LwpNumberingOverride::IsSmartLevelOverridden()
{
    return (sal_Bool)((m_nOverride & SMARTLEVEL) != 0);
}
inline sal_Bool LwpNumberingOverride::IsHeading()
{
    return (m_nValues & HEADING) != 0;
}
inline sal_Bool LwpNumberingOverride::IsSmartLevel()
{
    return (m_nValues & SMARTLEVEL) != 0;
}

inline void LwpNumberingOverride::RevertLevel()
{
    LwpOverride::Override(NO_LEVEL, STATE_STYLE);
}
inline void LwpNumberingOverride::RevertPosition()
{
    LwpOverride::Override(NO_POSITION, STATE_STYLE);
}
inline void LwpNumberingOverride::RevertHeading()
{
    LwpOverride::Override(HEADING, STATE_STYLE);
}
inline void LwpNumberingOverride::RevertSmartLevel()
{
    LwpOverride::Override(SMARTLEVEL, STATE_STYLE);
}

//end add

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
