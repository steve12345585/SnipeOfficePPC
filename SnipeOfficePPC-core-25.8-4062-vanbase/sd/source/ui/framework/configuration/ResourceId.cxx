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


#include "framework/ResourceId.hxx"
#include "framework/FrameworkHelper.hxx"
#include "tools/SdGlobalResourceContainer.hxx"
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/util/URLTransformer.hpp>
#include <comphelper/processfactory.hxx>
#include <rtl/ref.hxx>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::drawing::framework;
using ::rtl::OUString;

/** When the USE_OPTIMIZATIONS symbol is defined then at some optimizations
    are activated that work only together with XResourceId objects that are
    implemented by the ResourceId class.  For other implementations of when
    the USE_OPTIMIZATIONS symbol is not defined then alternative code is
    used instead.
*/
#define USE_OPTIMIZATIONS

namespace sd { namespace framework {

Reference<XInterface> SAL_CALL ResourceId_createInstance (
    const Reference<XComponentContext>& rxContext)
{
    (void)rxContext;
    return Reference<XInterface>(static_cast<XWeak*>(new ::sd::framework::ResourceId()));
}




::rtl::OUString ResourceId_getImplementationName (void) throw(RuntimeException)
{
    return ::rtl::OUString("com.sun.star.comp.Draw.framework.ResourceId");
}




Sequence<rtl::OUString> SAL_CALL ResourceId_getSupportedServiceNames (void)
    throw (RuntimeException)
{
    static const ::rtl::OUString sServiceName("com.sun.star.drawing.framework.ResourceId");
    return Sequence<rtl::OUString>(&sServiceName, 1);
}




//===== ResourceId ============================================================

WeakReference<util::XURLTransformer> ResourceId::mxURLTransformerWeak;

ResourceId::ResourceId (void)
    : ResourceIdInterfaceBase(),
      maResourceURLs(0),
      mpURL()
{
}




ResourceId::ResourceId (
    const std::vector<OUString>& rResourceURLs)
    : ResourceIdInterfaceBase(),
      maResourceURLs(rResourceURLs),
      mpURL()
{
    ParseResourceURL();
}




ResourceId::ResourceId (
    const OUString& rsResourceURL)
    : ResourceIdInterfaceBase(),
      maResourceURLs(1, rsResourceURL),
      mpURL()
{
    // Handle the special case of an empty resource URL.
    if (rsResourceURL.isEmpty())
        maResourceURLs.clear();
    ParseResourceURL();
}




ResourceId::ResourceId (
    const OUString& rsResourceURL,
    const OUString& rsAnchorURL)
    : ResourceIdInterfaceBase(),
      maResourceURLs(2),
      mpURL()
{
    maResourceURLs[0] = rsResourceURL;
    maResourceURLs[1] = rsAnchorURL;
    ParseResourceURL();
}




ResourceId::ResourceId (
    const OUString& rsResourceURL,
    const ::std::vector<OUString>& rAnchorURLs)
    : ResourceIdInterfaceBase(),
      maResourceURLs(1+rAnchorURLs.size()),
      mpURL()
{
    maResourceURLs[0] = rsResourceURL;
    for (sal_uInt32 nIndex=0; nIndex<rAnchorURLs.size(); ++nIndex)
        maResourceURLs[nIndex+1] = rAnchorURLs[nIndex];
    ParseResourceURL();
}




ResourceId::ResourceId (
    const OUString& rsResourceURL,
    const OUString& rsFirstAnchorURL,
    const Sequence<OUString>& rAnchorURLs)
    : ResourceIdInterfaceBase(),
      maResourceURLs(2+rAnchorURLs.getLength()),
      mpURL()
{
    maResourceURLs[0] = rsResourceURL;
    maResourceURLs[1] = rsFirstAnchorURL;
    for (sal_Int32 nIndex=0; nIndex<rAnchorURLs.getLength(); ++nIndex)
        maResourceURLs[nIndex+2] = rAnchorURLs[nIndex];
    ParseResourceURL();
}




ResourceId::~ResourceId (void)
{
    mpURL.reset();
}




OUString SAL_CALL
    ResourceId::getResourceURL (void)
    throw(com::sun::star::uno::RuntimeException)
{
    if (!maResourceURLs.empty())
        return maResourceURLs[0];
    else
        return OUString();
}




util::URL SAL_CALL
    ResourceId::getFullResourceURL (void)
 throw(com::sun::star::uno::RuntimeException)
{
    if (mpURL.get() != NULL)
        return *mpURL;

    Reference<util::XURLTransformer> xURLTransformer (mxURLTransformerWeak);
    if (xURLTransformer.is() && !maResourceURLs.empty() )
    {
        mpURL.reset(new util::URL);
        mpURL->Complete = maResourceURLs[0];
        xURLTransformer->parseStrict(*mpURL);
        return *mpURL;
    }

    util::URL aURL;
    if (!maResourceURLs.empty())
        aURL.Complete = maResourceURLs[0];
    return aURL;
}




sal_Bool SAL_CALL
    ResourceId::hasAnchor (void)
    throw (RuntimeException)
{
    return maResourceURLs.size()>1;
}




Reference<XResourceId> SAL_CALL
    ResourceId::getAnchor (void)
    throw (RuntimeException)
{
    ::rtl::Reference<ResourceId> rResourceId (new ResourceId());
    const sal_Int32 nAnchorCount (maResourceURLs.size()-1);
    if (nAnchorCount > 0)
    {
        rResourceId->maResourceURLs.resize(nAnchorCount);
        for (sal_Int32 nIndex=0; nIndex<nAnchorCount; ++nIndex)
            rResourceId->maResourceURLs[nIndex] = maResourceURLs[nIndex+1];
    }
    return Reference<XResourceId>(rResourceId.get());
}




Sequence<OUString> SAL_CALL
    ResourceId::getAnchorURLs (void)
    throw (RuntimeException)
{
    const sal_Int32 nAnchorCount (maResourceURLs.size() - 1);
    if (nAnchorCount > 0)
    {
        Sequence<OUString> aAnchorURLs (nAnchorCount);
        for (sal_Int32 nIndex=0; nIndex<nAnchorCount; ++nIndex)
            aAnchorURLs[nIndex] = maResourceURLs[nIndex+1];
        return aAnchorURLs;
    }
    else
        return Sequence<OUString>();
}




OUString SAL_CALL
    ResourceId::getResourceTypePrefix (void)
    throw (RuntimeException)
{
    if (!maResourceURLs.empty() )
    {
        // Return the "private:resource/<type>/" prefix.

        // Get the the prefix that ends with the second "/".
        const OUString& rsResourceURL (maResourceURLs[0]);
        sal_Int32 nPrefixEnd (rsResourceURL.indexOf(sal_Unicode('/'), 0));
        if (nPrefixEnd >= 0)
            nPrefixEnd = rsResourceURL.indexOf(sal_Unicode('/'), nPrefixEnd+1) + 1;
        else
            nPrefixEnd = 0;

        return rsResourceURL.copy(0,nPrefixEnd);
    }
    else
        return OUString();
}




sal_Int16 SAL_CALL
    ResourceId::compareTo (const Reference<XResourceId>& rxResourceId)
    throw (RuntimeException)
{
    sal_Int16 nResult (0);

    if ( ! rxResourceId.is())
    {
        // The empty reference is interpreted as empty resource id object.
        if (!maResourceURLs.empty())
            nResult = +1;
        else
            nResult = 0;
    }
    else
    {
        ResourceId* pId = NULL;
#ifdef USE_OPTIMIZATIONS
        pId = dynamic_cast<ResourceId*>(rxResourceId.get());
#endif
        if (pId != NULL)
        {
            // We have direct access to the implementation of the given
            // resource id object.
            nResult = CompareToLocalImplementation(*pId);
        }
        else
        {
            // We have to do the comparison via the UNO interface of the
            // given resource id object.
            nResult = CompareToExternalImplementation(rxResourceId);
        }
    }

    return nResult;
}




sal_Int16 ResourceId::CompareToLocalImplementation (const ResourceId& rId) const
{
    sal_Int16 nResult (0);

    const sal_uInt32 nLocalURLCount (maResourceURLs.size());
    const sal_uInt32 nURLCount(rId.maResourceURLs.size());

    // Start comparison with the top most anchors.
    for (sal_Int32 nIndex=nURLCount-1,nLocalIndex=nLocalURLCount-1;
         nIndex>=0 && nLocalIndex>=0;
         --nIndex,--nLocalIndex)
    {
        const OUString sLocalURL (maResourceURLs[nLocalIndex]);
        const OUString sURL (rId.maResourceURLs[nIndex]);
        const sal_Int32 nLocalResult (sURL.compareTo(sLocalURL));
        if (nLocalResult != 0)
        {
            if (nLocalResult < 0)
                nResult = -1;
            else
                nResult = +1;
            break;
        }
    }

    if (nResult == 0)
    {
        // No difference found yet.  When the lengths are the same then the
        // two resource ids are equivalent.  Otherwise the shorter comes
        // first.
        if (nLocalURLCount != nURLCount)
        {
            if (nLocalURLCount < nURLCount)
                nResult = -1;
            else
                nResult = +1;
        }
    }

    return nResult;
}




sal_Int16 ResourceId::CompareToExternalImplementation (const Reference<XResourceId>& rxId) const
{
    sal_Int16 nResult (0);

    const Sequence<OUString> aAnchorURLs (rxId->getAnchorURLs());
    const sal_uInt32 nLocalURLCount (maResourceURLs.size());
    const sal_uInt32 nURLCount(1+aAnchorURLs.getLength());

    // Start comparison with the top most anchors.
    sal_Int32 nLocalResult (0);
    for (sal_Int32 nIndex=nURLCount-1,nLocalIndex=nLocalURLCount-1;
         nIndex>=0&&nLocalIndex>=0;
         --nIndex,--nLocalIndex)
    {
        if (nIndex == 0 )
            nLocalResult = maResourceURLs[nIndex].compareTo(rxId->getResourceURL());
        else
            nLocalResult = maResourceURLs[nIndex].compareTo(aAnchorURLs[nIndex-1]);
        if (nLocalResult != 0)
        {
            if (nLocalResult < 0)
                nResult = -1;
            else
                nResult = +1;
            break;
        }
    }

    if (nResult == 0)
    {
        // No difference found yet.  When the lengths are the same then the
        // two resource ids are equivalent.  Otherwise the shorter comes
        // first.
        if (nLocalURLCount != nURLCount)
        {
            if (nLocalURLCount < nURLCount)
                nResult = -1;
            else
                nResult = +1;
        }
    }

    return nResult;
}




sal_Bool SAL_CALL
    ResourceId::isBoundTo (
        const Reference<XResourceId>& rxResourceId,
        AnchorBindingMode eMode)
    throw (RuntimeException)
{
    if ( ! rxResourceId.is())
    {
        // An empty reference is interpreted as empty resource id.
        return IsBoundToAnchor(NULL, NULL, eMode);
    }

    ResourceId* pId = NULL;
#ifdef USE_OPTIMIZATIONS
    pId = dynamic_cast<ResourceId*>(rxResourceId.get());
#endif
    if (pId != NULL)
    {
        return IsBoundToAnchor(pId->maResourceURLs, eMode);
    }
    else
    {
        const OUString sResourceURL (rxResourceId->getResourceURL());
        const Sequence<OUString> aAnchorURLs (rxResourceId->getAnchorURLs());
        return IsBoundToAnchor(&sResourceURL, &aAnchorURLs, eMode);
    }
}




sal_Bool SAL_CALL
    ResourceId::isBoundToURL (
        const OUString& rsAnchorURL,
        AnchorBindingMode eMode)
    throw (RuntimeException)
{
    return IsBoundToAnchor(&rsAnchorURL, NULL, eMode);
}




Reference<XResourceId> SAL_CALL
    ResourceId::clone (void)
    throw(RuntimeException)
{
    return new ResourceId(maResourceURLs);
}




//----- XInitialization -------------------------------------------------------

void SAL_CALL ResourceId::initialize (const Sequence<Any>& aArguments)
    throw (RuntimeException)
{
    sal_uInt32 nCount (aArguments.getLength());
    for (sal_uInt32 nIndex=0; nIndex<nCount; ++nIndex)
    {
        OUString sResourceURL;
        if (aArguments[nIndex] >>= sResourceURL)
            maResourceURLs.push_back(sResourceURL);
        else
        {
            Reference<XResourceId> xAnchor;
            if (aArguments[nIndex] >>= xAnchor)
            {
                if (xAnchor.is())
                {
                    maResourceURLs.push_back(xAnchor->getResourceURL());
                    Sequence<OUString> aAnchorURLs (xAnchor->getAnchorURLs());
                    for (sal_Int32 nURLIndex=0; nURLIndex<aAnchorURLs.getLength(); ++nURLIndex)
                    {
                        maResourceURLs.push_back(aAnchorURLs[nURLIndex]);
                    }
                }
            }
        }
    }
    ParseResourceURL();
}




//-----------------------------------------------------------------------------

/** When eMode is DIRECTLY then the anchor of the called object and the
    anchor represented by the given sequence of anchor URLs have to be
    identical.   When eMode is RECURSIVE then the anchor of the called
    object has to start with the given anchor URLs.
*/
bool ResourceId::IsBoundToAnchor (
    const OUString* psFirstAnchorURL,
    const Sequence<OUString>* paAnchorURLs,
    AnchorBindingMode eMode) const
{
    const sal_uInt32 nLocalAnchorURLCount (maResourceURLs.size() - 1);
    const bool bHasFirstAnchorURL (psFirstAnchorURL!=NULL);
    const sal_uInt32 nAnchorURLCount ((bHasFirstAnchorURL?1:0)
        + (paAnchorURLs!=NULL ? paAnchorURLs->getLength() : 0));

    // Check the lengths.
    if (nLocalAnchorURLCount<nAnchorURLCount ||
        (eMode==AnchorBindingMode_DIRECT && nLocalAnchorURLCount!=nAnchorURLCount))
    {
        return false;
    }

    // Compare the nAnchorURLCount bottom-most anchor URLs of this resource
    // id and the given anchor.
    sal_uInt32 nOffset = 0;
    if (paAnchorURLs != NULL)
    {
        sal_uInt32 nCount = paAnchorURLs->getLength();
        while (nOffset < nCount)
        {
            if ( ! maResourceURLs[nLocalAnchorURLCount - nOffset].equals(
                (*paAnchorURLs)[nCount - 1 - nOffset]))
            {
                return false;
            }
            ++nOffset;
        }
    }
    if (bHasFirstAnchorURL)
    {
        if ( ! psFirstAnchorURL->equals(maResourceURLs[nLocalAnchorURLCount - nOffset]))
            return false;
    }

    return true;
}




bool ResourceId::IsBoundToAnchor (
    const ::std::vector<OUString>& rAnchorURLs,
    AnchorBindingMode eMode) const
{
    const sal_uInt32 nLocalAnchorURLCount (maResourceURLs.size() - 1);
    const sal_uInt32 nAnchorURLCount (rAnchorURLs.size());

    // Check the lengths.
    if (nLocalAnchorURLCount<nAnchorURLCount ||
        (eMode==AnchorBindingMode_DIRECT && nLocalAnchorURLCount!=nAnchorURLCount))
    {
        return false;
    }

    // Compare the nAnchorURLCount bottom-most anchor URLs of this resource
    // id and the given anchor.
    for (sal_uInt32 nOffset=0; nOffset<nAnchorURLCount; ++nOffset)
    {
        if ( ! maResourceURLs[nLocalAnchorURLCount - nOffset].equals(
            rAnchorURLs[nAnchorURLCount - 1 - nOffset]))
        {
            return false;
        }
    }

    return true;
}




void ResourceId::ParseResourceURL (void)
{
    ::osl::Guard< ::osl::Mutex > aGuard (::osl::Mutex::getGlobalMutex());
    Reference<util::XURLTransformer> xURLTransformer (mxURLTransformerWeak);
    if ( ! xURLTransformer.is())
    {
        // Create the URL transformer.
        Reference<uno::XComponentContext> xContext(::comphelper::getProcessComponentContext());
        xURLTransformer = Reference<util::XURLTransformer>(util::URLTransformer::create(xContext));
        mxURLTransformerWeak = xURLTransformer;
        SdGlobalResourceContainer::Instance().AddResource(
            Reference<XInterface>(xURLTransformer,UNO_QUERY));
    }

    if (xURLTransformer.is() && !maResourceURLs.empty() )
    {
        mpURL.reset(new util::URL);
        mpURL->Complete = maResourceURLs[0];
        xURLTransformer->parseStrict(*mpURL);
        if (mpURL->Main == maResourceURLs[0])
            mpURL.reset();
        else
            maResourceURLs[0] = mpURL->Main;
    }
}


} } // end of namespace sd::framework

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
