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

#ifndef SD_ACCESSIBILITY_ACCESSIBLE_DOCUMENT_VIEW_BASE_HXX
#define SD_ACCESSIBILITY_ACCESSIBLE_DOCUMENT_VIEW_BASE_HXX

#include <editeng/AccessibleContextBase.hxx>
#include <editeng/AccessibleComponentBase.hxx>
#include <editeng/AccessibleSelectionBase.hxx>
#include "AccessibleViewForwarder.hxx"
#include "AccessiblePageShape.hxx"
#include <svx/ChildrenManager.hxx>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/awt/XWindowListener.hpp>
#include <com/sun/star/awt/XFocusListener.hpp>
#include <com/sun/star/beans/XPropertyChangeListener.hpp>
#include <com/sun/star/accessibility/XAccessible.hpp>
#include <com/sun/star/lang/IndexOutOfBoundsException.hpp>
#include <tools/link.hxx>

namespace sd {
class ViewShell;
class Window;
}

class VclSimpleEvent;

namespace accessibility {


/** Base class for the various document views of the Draw and
    Impress applications.

    <p>The different view modes of the Draw and Impress applications
    are made accessible by derived classes.  When the view mode is
    changed than the object representing the document view is
    disposed and replaced by a new instance of the then appropriate
    derived class.</p>

    <p>This base class also manages an optionally active accessible OLE
    object.  If you overwrite the <member>getAccessibleChildCount</member>
    and <member>getAccessibleChild</member> methods then make sure to first
    call the corresponding method of this class and adapt your child count
    and indices accordingly.  Only one active OLE object is allowed at a
    time.  This class does not listen for disposing calls at the moment
    because it does not use the accessible OLE object directly and trusts on
    getting informed through VCL window events.</p>

    <p>This class implements three kinds of listeners:
    <ol><li>The property change listener is not used directly but exists as
    convenience for derived classes.  May be moved to those classes
    instead.</li>
    <li>As window listener it waits for changes of the window geometry and
    forwards those as view forwarder changes.</li>
    <li>As focus listener it keeps track of the focus to give this class and
    derived classes the oportunity to set and remove the focus to/from
    shapes.</li>
    </ol>
    </p>
*/
class AccessibleDocumentViewBase
    :   public AccessibleContextBase,
        public AccessibleComponentBase,
        public AccessibleSelectionBase,
        public IAccessibleViewForwarderListener,
        public ::com::sun::star::beans::XPropertyChangeListener,
        public ::com::sun::star::awt::XWindowListener,
        public ::com::sun::star::awt::XFocusListener
{
public:
    //=====  internal  ========================================================

    /** Create a new object.  Note that the caller has to call the
        Init method directly after this constructor has finished.
    @param pSdWindow
        The window whose content is to be made accessible.
    @param pViewShell
        The view shell associated with the given window.
    @param rxController
        The controller from which to get the model.
    @param rxParent
        The accessible parent of the new object.  Note that this parent does
        not necessarily correspond with the parent of the given window.
     */
    AccessibleDocumentViewBase (
        ::sd::Window* pSdWindow,
        ::sd::ViewShell* pViewShell,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::frame::XController>& rxController,
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::accessibility::XAccessible>& rxParent);

    virtual ~AccessibleDocumentViewBase (void);

    /** Initialize a new object.  Call this method directly after creating a
        new object.  It finished the initialization begun in the constructor
        but which needs a fully created object.
     */
    virtual void Init (void);

    /** Define callback for listening to window child events of VCL.
        Listen for creation or destruction of OLE objects.
    */
    DECL_LINK (WindowChildEventListener, VclSimpleEvent*);

    //=====  IAccessibleViewForwarderListener  ================================

    /** A view forwarder change is signalled for instance when any of the
        window events is recieved.  Thus, instead of overloading the four
        windowResized... methods it will be sufficient in most cases just to
        overload this method.
     */
    virtual void ViewForwarderChanged (ChangeType aChangeType,
        const IAccessibleViewForwarder* pViewForwarder);

    //=====  XAccessibleContext  ==============================================

    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::accessibility::XAccessible> SAL_CALL
        getAccessibleParent (void)
        throw (::com::sun::star::uno::RuntimeException);

    /** This implementation returns either 1 or 0 depending on whether there
        is an active accessible OLE object or not.
    */
    virtual sal_Int32 SAL_CALL
        getAccessibleChildCount (void)
        throw (::com::sun::star::uno::RuntimeException);

    /** This implementation either returns the active accessible OLE object
        if it exists and the given index is 0 or throws an exception.
    */
    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::accessibility::XAccessible> SAL_CALL
        getAccessibleChild (sal_Int32 nIndex)
        throw (::com::sun::star::uno::RuntimeException,
            ::com::sun::star::lang::IndexOutOfBoundsException);


    //=====  XAccessibleComponent  ============================================

    virtual ::com::sun::star::uno::Reference<
        ::com::sun::star::accessibility::XAccessible > SAL_CALL
        getAccessibleAtPoint (const ::com::sun::star::awt::Point& aPoint)
        throw (::com::sun::star::uno::RuntimeException);

    virtual ::com::sun::star::awt::Rectangle SAL_CALL getBounds (void)
        throw (::com::sun::star::uno::RuntimeException);

    virtual ::com::sun::star::awt::Point SAL_CALL getLocation (void)
        throw (::com::sun::star::uno::RuntimeException);

    virtual ::com::sun::star::awt::Point SAL_CALL getLocationOnScreen (void)
        throw (::com::sun::star::uno::RuntimeException);

    virtual ::com::sun::star::awt::Size SAL_CALL getSize (void)
        throw (::com::sun::star::uno::RuntimeException);


    //=====  XInterface  ======================================================

    virtual com::sun::star::uno::Any SAL_CALL
        queryInterface (const com::sun::star::uno::Type & rType)
        throw (::com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL
        acquire (void)
        throw ();

    virtual void SAL_CALL
        release (void)
        throw ();


    //=====  XServiceInfo  ====================================================

    /** Returns an identifier for the implementation of this object.
    */
    virtual ::rtl::OUString SAL_CALL
        getImplementationName (void)
        throw (::com::sun::star::uno::RuntimeException);

    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString> SAL_CALL
        getSupportedServiceNames (void)
        throw (::com::sun::star::uno::RuntimeException);


    //=====  XTypeProvider  ===================================================

    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type> SAL_CALL
        getTypes (void)
        throw (::com::sun::star::uno::RuntimeException);


    //=====  lang::XEventListener  ============================================

    virtual void SAL_CALL
        disposing (const ::com::sun::star::lang::EventObject& rEventObject)
        throw (::com::sun::star::uno::RuntimeException);


    //=====  XPropertyChangeListener  =========================================

    virtual void SAL_CALL
        propertyChange (const ::com::sun::star::beans::PropertyChangeEvent& rEventObject)
        throw (::com::sun::star::uno::RuntimeException);


    //=====  XWindowListener  =================================================

    virtual void SAL_CALL
        windowResized (const ::com::sun::star::awt::WindowEvent& e)
        throw (::com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL
        windowMoved (const ::com::sun::star::awt::WindowEvent& e)
        throw (::com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL
        windowShown (const ::com::sun::star::lang::EventObject& e)
        throw (::com::sun::star::uno::RuntimeException);

    virtual void SAL_CALL
        windowHidden (const ::com::sun::star::lang::EventObject& e)
        throw (::com::sun::star::uno::RuntimeException);

    //=====  XFocusListener  =================================================

    virtual void SAL_CALL focusGained (const ::com::sun::star::awt::FocusEvent& e)
        throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL focusLost (const ::com::sun::star::awt::FocusEvent& e)
        throw (::com::sun::star::uno::RuntimeException);

private:

    // return the member maMutex;
    virtual ::osl::Mutex&
        implGetMutex();

    // return ourself as context in default case
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::accessibility::XAccessibleContext >
        implGetAccessibleContext()
        throw ( ::com::sun::star::uno::RuntimeException );

    // return sal_False in default case
    virtual sal_Bool
        implIsSelected( sal_Int32 nAccessibleChildIndex )
        throw (::com::sun::star::uno::RuntimeException);

    // return nothing in default case
    virtual void
        implSelect( sal_Int32 nAccessibleChildIndex, sal_Bool bSelect )
        throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);

protected:
    /// The core window that is made accessible.
    ::sd::Window* mpWindow;

    /// The API window that is made accessible.
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow>
         mxWindow;

    /// The controller of the window in which this view is displayed.
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XController>
         mxController;

    /// Model of the document.
    ::com::sun::star::uno::Reference < ::com::sun::star::frame::XModel>
        mxModel;

    // Bundle of information that is passed down the shape tree.
    AccessibleShapeTreeInfo maShapeTreeInfo;

    /// The view forwarder passed to the children manager.
    AccessibleViewForwarder maViewForwarder;

    /** Accessible OLE object.  Set or removed by the
        <member>SetAccessibleOLEObject</member> method.
    */
    ::com::sun::star::uno::Reference <
        ::com::sun::star::accessibility::XAccessible>
        mxAccessibleOLEObject;

    Link maWindowLink;

    // This method is called from the component helper base class while
    // disposing.
    virtual void SAL_CALL disposing (void);

    /** Create a name string.  The current name is not modified and,
        therefore, no events are send.  This method is usually called once
        by the <member>getAccessibleName</member> method of the base class.
        @return
           A name string.
    */
    virtual ::rtl::OUString
        CreateAccessibleName ()
        throw (::com::sun::star::uno::RuntimeException);

    /** Create a description string.  The current description is not
        modified and, therefore, no events are send.  This method is usually
        called once by the <member>getAccessibleDescription</member> method
        of the base class.
        @return
           A description string.
    */
    virtual ::rtl::OUString
        CreateAccessibleDescription ()
        throw (::com::sun::star::uno::RuntimeException);

    /** This method is called when (after) the frame containing this
        document has been activated.  Can be used to send FOCUSED state
        changes for the currently selected element.

        Note: Currently used as a substitute for FocusGained.  Should be
        renamed in the future.
    */
    virtual void Activated (void);

    /** This method is called when (before or after?) the frame containing
        this document has been deactivated.  Can be used to send FOCUSED
        state changes for the currently selected element.

        Note: Currently used as a substitute for FocusLost.  Should be
        renamed in the future.
    */
    virtual void Deactivated (void);

    /** Set or remove the currently active accessible OLE object.
        @param xOLEObject
            If this is a valid reference then a child event is send that
            informs the listeners of a new child.  If there has already been
            an active accessible OLE object then this is removed first and
            appropriate events are send.

            If this is an empty reference then the currently active
            accessible OLE object (if there is one) is removed.
    */
    virtual void SetAccessibleOLEObject (
        const ::com::sun::star::uno::Reference <
        ::com::sun::star::accessibility::XAccessible>& xOLEObject);

    virtual void impl_dispose (void);
};

} // end of namespace accessibility

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
