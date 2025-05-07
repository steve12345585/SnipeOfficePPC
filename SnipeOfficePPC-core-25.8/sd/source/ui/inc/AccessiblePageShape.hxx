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


#ifndef _SD_ACCESSIBILITY_ACCESSIBLE_PAGE_SHAPE_HXX
#define _SD_ACCESSIBILITY_ACCESSIBLE_PAGE_SHAPE_HXX

#include <svx/AccessibleShape.hxx>
#include <svx/AccessibleShapeTreeInfo.hxx>
#include <svx/IAccessibleViewForwarderListener.hxx>
#include <com/sun/star/accessibility/XAccessible.hpp>
#include <com/sun/star/accessibility/XAccessibleExtendedComponent.hpp>
#include <com/sun/star/accessibility/AccessibleRole.hpp>
#include <com/sun/star/drawing/XDrawPage.hpp>
#include <com/sun/star/lang/XEventListener.hpp>

#include <svx/AccessibleTextHelper.hxx>

namespace accessibility {

/** A page shape represents the actual page as seen on the screen.
*/
class AccessiblePageShape
    :   public AccessibleShape
{
public:
    //=====  internal  ========================================================

    /** Create a new accessible object that makes the given shape accessible.
        @param rxParent
            The accessible parent object.  It will be used, for example when
            the <member>getIndexInParent</member> method is called.
        @param rShapeTreeInfo
            Bundel of information passed to this shape and all of its desendants.
        @param nIndex
            Index used to disambiguate between objects that have the same
            name.  Passing a value of -1 leads to the use of the object's
            z-order instead.  Because that is not a good substitute, better
            pass an ever increasing counter.
        @attention
            Always call the <member>init</member> method after creating a
            new accessible shape.  This is one way to overcome the potential
            problem of registering the new object with e.g. event
            broadcasters.  That would delete the new object if a broadcaster
            would not keep a strong reference to the new object.
    */
    AccessiblePageShape (
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::drawing::XDrawPage>& rxPage,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::accessibility::XAccessible>& rxParent,
        const AccessibleShapeTreeInfo& rShapeTreeInfo,
        long nIndex = -1);

    virtual ~AccessiblePageShape (void);

    /** Initialize a new shape.  See the documentation of the constructor
        for the reason of this method's existence.
    */
    virtual void Init (void);

    //=====  XAccessibleContext  ==============================================

    /// Returns always 0 because there can be no children.
    virtual sal_Int32 SAL_CALL
        getAccessibleChildCount (void)
        throw ();

    /** Return the specified child.
        @param nIndex
            Index of the requested child.
        @return
            Reference of the requested child which is the accessible object
            of a visible shape.
        @raises IndexOutOfBoundsException
            Throws always an exception because there are no children.
    */
    virtual ::com::sun::star::uno::Reference<
            ::com::sun::star::accessibility::XAccessible> SAL_CALL
        getAccessibleChild (sal_Int32 nIndex)
        throw (::com::sun::star::uno::RuntimeException);


    //=====  XAccessibleComponent  ============================================

    virtual ::com::sun::star::awt::Rectangle SAL_CALL getBounds (void)
        throw (::com::sun::star::uno::RuntimeException);

    virtual sal_Int32 SAL_CALL getForeground (void)
        throw (::com::sun::star::uno::RuntimeException);

    virtual sal_Int32 SAL_CALL getBackground (void)
        throw (::com::sun::star::uno::RuntimeException);

    //=====  XComponent  ======================================================

    virtual void SAL_CALL
        dispose (void)
        throw (::com::sun::star::uno::RuntimeException);


    //=====  XServiceInfo  ====================================================

    virtual ::rtl::OUString SAL_CALL
        getImplementationName (void)
        throw (::com::sun::star::uno::RuntimeException);

    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString> SAL_CALL
        getSupportedServiceNames (void)
        throw (::com::sun::star::uno::RuntimeException);


    //=====  lang::XEventListener  ============================================

    virtual void SAL_CALL
        disposing (const ::com::sun::star::lang::EventObject& Source)
        throw (::com::sun::star::uno::RuntimeException);


    using AccessibleShape::disposing;

protected:
    /** Create a base name string that contains the accessible name.
    */
    virtual ::rtl::OUString
        CreateAccessibleBaseName (void)
        throw (::com::sun::star::uno::RuntimeException);

    virtual ::rtl::OUString
        CreateAccessibleName (void)
        throw (::com::sun::star::uno::RuntimeException);

    /// Create a description string that contains the accessible description.
    virtual ::rtl::OUString
        CreateAccessibleDescription (void)
        throw (::com::sun::star::uno::RuntimeException);

private:
    ::com::sun::star::uno::Reference<
        ::com::sun::star::drawing::XDrawPage> mxPage;

    /** Don't use the default constructor.  Use the public constructor that
        takes the original shape and the parent as arguments instead.
    */
    explicit AccessiblePageShape (void);
    /// Don't use the copy constructor.  Is there any use for it?
    explicit AccessiblePageShape (const AccessiblePageShape&);
    /// Don't use the assignment operator.  Do we need this?
    AccessibleShape& operator= (const AccessiblePageShape&);
};

} // end of namespace accessibility

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
