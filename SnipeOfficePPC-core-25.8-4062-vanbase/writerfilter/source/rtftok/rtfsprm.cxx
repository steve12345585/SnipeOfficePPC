/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Version: MPL 1.1 / GPLv3+ / LGPLv3+
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Initial Developer of the Original Code is
 *       Miklos Vajna <vmiklos@frugalware.org>
 * Portions created by the Initial Developer are Copyright (C) 2011 the
 * Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPLv3+"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
 * in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
 * instead of those above.
 */

#include <rtfsprm.hxx>
#include <rtl/strbuf.hxx>

#include <resourcemodel/QNameToString.hxx>
#include <doctok/resourceids.hxx> // NS_rtf namespace

using rtl::OStringBuffer;

namespace writerfilter {
namespace rtftok {

RTFSprm::RTFSprm(Id nKeyword, RTFValue::Pointer_t& pValue)
    : m_nKeyword(nKeyword),
    m_pValue(pValue)
{
}

sal_uInt32 RTFSprm::getId() const
{
    return m_nKeyword;
}

Value::Pointer_t RTFSprm::getValue()
{
    return Value::Pointer_t(m_pValue->Clone());
}

writerfilter::Reference<BinaryObj>::Pointer_t RTFSprm::getBinary()
{
    return m_pValue->getBinary();
}

writerfilter::Reference<Stream>::Pointer_t RTFSprm::getStream()
{
    return m_pValue->getStream();
}

writerfilter::Reference<Properties>::Pointer_t RTFSprm::getProps()
{
    return m_pValue->getProperties();
}

Sprm::Kind RTFSprm::getKind()
{
    return Sprm::UNKNOWN;
}

std::string RTFSprm::getName() const
{
    return "RTFSprm";
}

std::string RTFSprm::toString() const
{
    OStringBuffer aBuf("RTFSprm");

    std::string sResult = (*QNameToString::Instance())(m_nKeyword);
    if (sResult.length() == 0)
        sResult = (*SprmIdToString::Instance())(m_nKeyword);

    aBuf.append(" ('");
    if (sResult.length() == 0)
        aBuf.append(sal_Int32(m_nKeyword));
    else
        aBuf.append(sResult.c_str());
    aBuf.append("', '");
    aBuf.append(m_pValue->toString().c_str());
    aBuf.append("')");

    return aBuf.makeStringAndClear().getStr();
}

RTFValue::Pointer_t RTFSprms::find(Id nKeyword, bool bFirst)
{
    RTFValue::Pointer_t pValue;
    for (RTFSprms::Iterator_t i = m_pSprms->begin(); i != m_pSprms->end(); ++i)
        if (i->first == nKeyword)
        {
            if (bFirst)
                return i->second;
            else
                pValue = i->second;
        }
    return pValue;
}

void RTFSprms::set(Id nKeyword, RTFValue::Pointer_t pValue, bool bOverwrite)
{
    ensureCopyBeforeWrite();
    if (bOverwrite)
    {
        for (RTFSprms::Iterator_t i = m_pSprms->begin(); i != m_pSprms->end(); ++i)
            if (i->first == nKeyword)
            {
                i->second = pValue;
                return;
            }
    }
    m_pSprms->push_back(std::make_pair(nKeyword, pValue));
}

bool RTFSprms::erase(Id nKeyword)
{
    ensureCopyBeforeWrite();
    for (RTFSprms::Iterator_t i = m_pSprms->begin(); i != m_pSprms->end(); ++i)
    {
        if (i->first == nKeyword)
        {
            m_pSprms->erase(i);
            return true;
        }
    }
    return false;
}

void RTFSprms::deduplicate(RTFSprms& rReference)
{
    ensureCopyBeforeWrite();

    RTFSprms::Iterator_t i = m_pSprms->begin();
    while (i != m_pSprms->end())
    {
        bool bIgnore = false;
        if (i->first != NS_rtf::LN_ISTD)
        {
            RTFValue::Pointer_t pValue(rReference.find(i->first));
            if (pValue.get() && i->second->equals(*pValue))
                bIgnore = true;
        }
        if (bIgnore)
            i = m_pSprms->erase(i);
        else
            ++i;
    }
}

void RTFSprms::ensureCopyBeforeWrite()
{
    if (m_pSprms->m_nRefCount > 1) {
        boost::intrusive_ptr<RTFSprmsImpl> pClone(new RTFSprmsImpl());
        for (std::vector< std::pair<Id, RTFValue::Pointer_t> >::const_iterator i = m_pSprms->begin(); i != m_pSprms->end(); ++i)
            pClone->push_back(std::make_pair(i->first, RTFValue::Pointer_t(i->second->Clone())));
        m_pSprms = pClone;
    }
}

RTFSprms::RTFSprms()
    : m_pSprms(new RTFSprmsImpl())
{
}

RTFSprms::~RTFSprms()
{
}

RTFSprms::RTFSprms(const RTFSprms& rSprms)
{
    *this = rSprms;
}

RTFSprms& RTFSprms::operator=(const RTFSprms& rOther)
{
    m_pSprms = rOther.m_pSprms;
    return *this;
}

void RTFSprms::swap(RTFSprms& rOther)
{
    boost::intrusive_ptr<RTFSprmsImpl> pTmp = rOther.m_pSprms;
    rOther.m_pSprms = m_pSprms;
    m_pSprms = pTmp;
}

void RTFSprms::clear()
{
    if (m_pSprms->m_nRefCount == 1)
        return m_pSprms->clear();
    else
        m_pSprms.reset(new RTFSprmsImpl());
}

} // namespace rtftok
} // namespace writerfilter

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
