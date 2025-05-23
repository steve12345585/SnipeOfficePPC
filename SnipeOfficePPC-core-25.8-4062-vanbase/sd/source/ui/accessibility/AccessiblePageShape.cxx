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

#include "AccessiblePageShape.hxx"
#include <svx/AccessibleShapeInfo.hxx>

#include <com/sun/star/accessibility/AccessibleRole.hpp>
#include <com/sun/star/accessibility/AccessibleStateType.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/container/XChild.hpp>
#include <com/sun/star/drawing/XShapes.hpp>
#include <com/sun/star/drawing/XShapeDescriptor.hpp>
#include <com/sun/star/drawing/XMasterPageTarget.hpp>
#include <com/sun/star/lang/IndexOutOfBoundsException.hpp>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::accessibility;
using ::com::sun::star::uno::Reference;
using ::rtl::OUString;


namespace accessibility {

//=====  internal  ============================================================

AccessiblePageShape::AccessiblePageShape (
    const uno::Reference<drawing::XDrawPage>& rxPage,
    const uno::Reference<XAccessible>& rxParent,
    const AccessibleShapeTreeInfo& rShapeTreeInfo,
    long nIndex)
    : AccessibleShape (AccessibleShapeInfo (NULL, rxParent, nIndex), rShapeTreeInfo),
      mxPage (rxPage)
{
    // The main part of the initialization is done in the init method which
    // has to be called from this constructor's caller.
}




AccessiblePageShape::~AccessiblePageShape (void)
{
    OSL_TRACE ("~AccessiblePageShape");
}




void AccessiblePageShape::Init (void)
{
    AccessibleShape::Init ();
}




//=====  XAccessibleContext  ==================================================

sal_Int32 SAL_CALL
       AccessiblePageShape::getAccessibleChildCount (void)
    throw ()
{
    return 0;
}




/** Forward the request to the shape.  Return the requested shape or throw
    an exception for a wrong index.
*/
uno::Reference<XAccessible> SAL_CALL
    AccessiblePageShape::getAccessibleChild( sal_Int32 )
    throw (::com::sun::star::uno::RuntimeException)
{
    throw lang::IndexOutOfBoundsException ("page shape has no children",
        static_cast<uno::XWeak*>(this));
}




//=====  XAccessibleComponent  ================================================

awt::Rectangle SAL_CALL AccessiblePageShape::getBounds (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    ThrowIfDisposed ();

    awt::Rectangle aBoundingBox;

    if (maShapeTreeInfo.GetViewForwarder() != NULL)
    {
        uno::Reference<beans::XPropertySet> xSet (mxPage, uno::UNO_QUERY);
        if (xSet.is())
        {
            uno::Any aValue;
            awt::Point aPosition;
            awt::Size aSize;

            aValue = xSet->getPropertyValue ("BorderLeft");
            aValue >>= aBoundingBox.X;
            aValue = xSet->getPropertyValue ("BorderTop");
            aValue >>= aBoundingBox.Y;

            aValue = xSet->getPropertyValue ("Width");
            aValue >>= aBoundingBox.Width;
            aValue = xSet->getPropertyValue ("Height");
            aValue >>= aBoundingBox.Height;
        }

        // Transform coordinates from internal to pixel.
        ::Size aPixelSize = maShapeTreeInfo.GetViewForwarder()->LogicToPixel (
            ::Size (aBoundingBox.Width, aBoundingBox.Height));
        ::Point aPixelPosition = maShapeTreeInfo.GetViewForwarder()->LogicToPixel (
            ::Point (aBoundingBox.X, aBoundingBox.Y));

        // Clip the shape's bounding box with the bounding box of its parent.
        Reference<XAccessibleComponent> xParentComponent (
            getAccessibleParent(), uno::UNO_QUERY);
        if (xParentComponent.is())
        {
            // Make the coordinates relative to the parent.
            awt::Point aParentLocation (xParentComponent->getLocationOnScreen());
            int x = aPixelPosition.getX() - aParentLocation.X;
            int y = aPixelPosition.getY() - aParentLocation.Y;


            // Clip with parent (with coordinates relative to itself).
            ::Rectangle aBBox (
                x, y, x + aPixelSize.getWidth(), y + aPixelSize.getHeight());
            awt::Size aParentSize (xParentComponent->getSize());
            ::Rectangle aParentBBox (0,0, aParentSize.Width, aParentSize.Height);
            aBBox = aBBox.GetIntersection (aParentBBox);
            aBoundingBox = awt::Rectangle (
                aBBox.getX(),
                aBBox.getY(),
                aBBox.getWidth(),
                aBBox.getHeight());
        }
        else
            aBoundingBox = awt::Rectangle (
                aPixelPosition.getX(), aPixelPosition.getY(),
                aPixelSize.getWidth(), aPixelSize.getHeight());
    }

    return aBoundingBox;
}




sal_Int32 SAL_CALL AccessiblePageShape::getForeground (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    ThrowIfDisposed ();
    sal_Int32 nColor (0x0ffffffL);

    try
    {
        uno::Reference<beans::XPropertySet> aSet (mxPage, uno::UNO_QUERY);
        if (aSet.is())
        {
            uno::Any aColor;
            aColor = aSet->getPropertyValue ("LineColor");
            aColor >>= nColor;
        }
    }
    catch (const ::com::sun::star::beans::UnknownPropertyException&)
    {
        // Ignore exception and return default color.
    }
    return nColor;
}




/** Extract the background color from the Background property of eithe the
    draw page or its master page.
*/
sal_Int32 SAL_CALL AccessiblePageShape::getBackground (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    ThrowIfDisposed ();
    sal_Int32 nColor (0x01020ffL);

    try
    {
        uno::Reference<beans::XPropertySet> xSet (mxPage, uno::UNO_QUERY);
        if (xSet.is())
        {
            uno::Any aBGSet;
            aBGSet = xSet->getPropertyValue ("Background");
            Reference<beans::XPropertySet> xBGSet (aBGSet, uno::UNO_QUERY);
            if ( ! xBGSet.is())
            {
                // Draw page has no Background property.  Try the master
                // page instead.
                Reference<drawing::XMasterPageTarget> xTarget (mxPage, uno::UNO_QUERY);
                if (xTarget.is())
                {
                    xSet = Reference<beans::XPropertySet> (xTarget->getMasterPage(),
                        uno::UNO_QUERY);
                    aBGSet = xSet->getPropertyValue ("Background");
                    xBGSet = Reference<beans::XPropertySet> (aBGSet, uno::UNO_QUERY);
                }
            }
            // Fetch the fill color.  Has to be extended to cope with
            // gradients, hashes, and bitmaps.
            if (xBGSet.is())
            {
                uno::Any aColor;
                aColor = xBGSet->getPropertyValue ("FillColor");
                aColor >>= nColor;
            }
            else
                OSL_TRACE ("no Background property in page");
        }
    }
    catch (const ::com::sun::star::beans::UnknownPropertyException&)
    {
        OSL_TRACE ("caught excption due to unknown property");
        // Ignore exception and return default color.
    }
    return nColor;
}




//=====  XServiceInfo  ========================================================

::rtl::OUString SAL_CALL
    AccessiblePageShape::getImplementationName (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    ThrowIfDisposed ();
    return ::rtl::OUString("AccessiblePageShape");
}




::com::sun::star::uno::Sequence< ::rtl::OUString> SAL_CALL
    AccessiblePageShape::getSupportedServiceNames (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    ThrowIfDisposed ();
    return AccessibleShape::getSupportedServiceNames();
}




//=====  lang::XEventListener  ================================================

void SAL_CALL
    AccessiblePageShape::disposing (const ::com::sun::star::lang::EventObject& aEvent)
    throw (::com::sun::star::uno::RuntimeException)
{
    ThrowIfDisposed ();
    AccessibleShape::disposing (aEvent);
}




//=====  XComponent  ==========================================================

void AccessiblePageShape::dispose (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    OSL_TRACE ("AccessiblePageShape::dispose");

    // Unregister listeners.
    Reference<lang::XComponent> xComponent (mxShape, uno::UNO_QUERY);
    if (xComponent.is())
        xComponent->removeEventListener (this);

    // Cleanup.
    mxShape = NULL;

    // Call base classes.
    AccessibleContextBase::dispose ();
}




//=====  protected internal  ==================================================

::rtl::OUString
    AccessiblePageShape::CreateAccessibleBaseName (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    return ::rtl::OUString ("PageShape");
}




::rtl::OUString
    AccessiblePageShape::CreateAccessibleName (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    Reference<beans::XPropertySet> xPageProperties (mxPage, UNO_QUERY);

    // Get name of the current slide.
    OUString sCurrentSlideName;
    try
    {
        if (xPageProperties.is())
        {
            xPageProperties->getPropertyValue( "LinkDisplayName" ) >>= sCurrentSlideName;
        }
    }
    catch (const beans::UnknownPropertyException&)
    {
    }

    return CreateAccessibleBaseName()+": "+sCurrentSlideName;
}




::rtl::OUString
    AccessiblePageShape::CreateAccessibleDescription (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    return ::rtl::OUString ("Page Shape");
}


} // end of namespace accessibility


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
