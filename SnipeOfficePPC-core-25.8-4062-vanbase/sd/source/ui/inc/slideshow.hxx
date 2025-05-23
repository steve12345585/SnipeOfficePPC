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

#ifndef _SD_SLIDESHOW_HXX
#define _SD_SLIDESHOW_HXX

#include <com/sun/star/presentation/XPresentation2.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>

#include <rtl/ref.hxx>

#include <tools/link.hxx>

#include <cppuhelper/compbase2.hxx>
#include <cppuhelper/basemutex.hxx>

#include <editeng/unoipset.hxx>

#include <memory>
#include <boost/shared_ptr.hpp>

namespace com { namespace sun { namespace star {

    namespace drawing {
        class XDrawPage;
    }
    namespace animations {
        class XAnimationNode;
    }
} } }

class SdDrawDocument;
class KeyEvent;
class Size;
class Rectangle;
class Window;
class SfxRequest;
class WorkWindow;

// TODO: Remove
#define PAGE_NO_END         65535

/* Definition of SlideShow class */

namespace sd
{

class SlideshowImpl;
class ShowWindow;
class Window;
class View;
class ViewShell;
class ViewShellBase;
struct PresentationSettingsEx;
class FrameView;

enum AnimationMode
{
    ANIMATIONMODE_SHOW,
    ANIMATIONMODE_VIEW,
    ANIMATIONMODE_PREVIEW
};

typedef ::cppu::WeakComponentImplHelper2< ::com::sun::star::presentation::XPresentation2, ::com::sun::star::lang::XServiceInfo > SlideshowBase;

class SlideShow : private ::cppu::BaseMutex, public SlideshowBase
{
public:
    /// used by the model to create a slideshow for it
    static rtl::Reference< SlideShow > Create( SdDrawDocument* pDoc );

    // static helper api
    static rtl::Reference< SlideShow > GetSlideShow( SdDrawDocument* pDocument );
    static rtl::Reference< SlideShow > GetSlideShow( SdDrawDocument& rDocument );
    static rtl::Reference< SlideShow > GetSlideShow( ViewShellBase& rBase );

    static ::com::sun::star::uno::Reference< ::com::sun::star::presentation::XSlideShowController > GetSlideShowController(ViewShellBase& rBase );

    static bool StartPreview( ViewShellBase& rBase,
        const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPage >& xDrawPage,
        const ::com::sun::star::uno::Reference< ::com::sun::star::animations::XAnimationNode >& xAnimationNode,
        ::Window* pParent = 0 );

    static void Stop( ViewShellBase& rBase );

    /// returns true if there is a running presentation for the given ViewShellBase
    static bool IsRunning( ViewShellBase& rBase );

    /// returns true if there is a running presentation inside the given ViewShell
    /// returns false even if there is a running presentation but in another ViewShell
    static bool IsRunning( ViewShell& rViewShell );

    // helper api

    bool startPreview(
        const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPage >& xDrawPage,
        const ::com::sun::star::uno::Reference< ::com::sun::star::animations::XAnimationNode >& xAnimationNode,
        ::Window* pParent = 0 );

    // uno api

        virtual void SAL_CALL disposing (void);

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  ) throw (::com::sun::star::uno::RuntimeException);

    // XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

    // XPresentation
    virtual void SAL_CALL start(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL end(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL rehearseTimings(  ) throw (::com::sun::star::uno::RuntimeException);

    // XPresentation2
    virtual void SAL_CALL startWithArguments( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& Arguments ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::sal_Bool SAL_CALL isRunning(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::presentation::XSlideShowController > SAL_CALL getController(  ) throw (::com::sun::star::uno::RuntimeException);

    // legacy api

    // actions
    void jumpToPageNumber( sal_Int32 nPage );               // a.k.a. FuSlideShow::JumpToPage()
    void jumpToPageIndex( sal_Int32 nIndex );
    void jumpToBookmark( const ::rtl::OUString& sBookmark );            // a.k.a. FuSlideShow::JumpToBookmark()

    /** sets or clears the pause state of the running slideshow.
        !!!! This should only be called by the SdShowWindow !!!!*/
    bool pause( bool bPause );


    // settings
    bool isFullScreen();                                // a.k.a. FuSlideShow::IsFullScreen()
    bool isAlwaysOnTop();                               // a.k.a. FuSlideShow::IsAlwaysOnTop();
    ShowWindow* getShowWindow();                        // a.k.a. FuSlideShow::GetShowWindow()
    int getAnimationMode();                             // a.k.a. FuSlideShow::GetAnimationMode()
    sal_Int32 getCurrentPageNumber();                   // a.k.a. FuSlideShow::GetCurrentPage()
    sal_Int32 getFirstPageNumber();
    sal_Int32 getLastPageNumber();
    bool isEndless();
    bool isDrawingPossible();

    // events
    void resize( const Size &rSize );
    void activate(ViewShellBase& rBase);
    void deactivate(ViewShellBase& rBase);
    void paint( const Rectangle& rRect );

    bool keyInput(const KeyEvent& rKEvt);

    void receiveRequest(SfxRequest& rReq);

    bool dependsOn( ViewShellBase* pViewShellBase );

    static sal_Int32 GetDisplay();

private:
    SlideShow( SdDrawDocument* pDoc );

    DECL_LINK( StartInPlacePresentationConfigurationHdl, void * );
    void StartInPlacePresentationConfigurationCallback();

    void StartInPlacePresentation();
    void StartFullscreenPresentation();

    void ThrowIfDisposed() throw (::com::sun::star::uno::RuntimeException);

    void CreateController( ViewShell* pViewSh, ::sd::View* pView, ::Window* pParentWindow );
    WorkWindow *GetWorkWindow();

    // default: disabled copy/assignment
    SlideShow(const SlideShow&);
    SlideShow& operator=( const SlideShow& );

    SvxItemPropertySet  maPropSet;

    rtl::Reference< SlideshowImpl > mxController;
    /** This flag is used together with mxController.is() to prevent
        multiple instances of the slide show for one document.  The flag
        covers the time before mxController is set.
    */
    bool mbIsInStartup;
    SdDrawDocument* mpDoc;

    boost::shared_ptr< PresentationSettingsEx > mxCurrentSettings;

    ViewShellBase* mpCurrentViewShellBase;
    ViewShellBase* mpFullScreenViewShellBase;
    FrameView* mpFullScreenFrameView;
    sal_Int32   mnInPlaceConfigEvent;
};

}

#endif /* _SD_SLIDESHOW_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
