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


#include "DrawController.hxx"
#include "DrawDocShell.hxx"

#include "DrawSubController.hxx"
#include "sdpage.hxx"
#include "ViewShellBase.hxx"
#include "ViewShellManager.hxx"
#include "FormShellManager.hxx"
#include "Window.hxx"

#include <comphelper/anytostring.hxx>
#include <comphelper/processfactory.hxx>
#include <comphelper/sequence.hxx>
#include <comphelper/stl_types.hxx>
#include <comphelper/servicehelper.hxx>
#include <cppuhelper/exc_hlp.hxx>
#include <cppuhelper/bootstrap.hxx>

#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/drawing/framework/ConfigurationController.hpp>
#include <com/sun/star/drawing/framework/ModuleController.hpp>
#include <com/sun/star/lang/XInitialization.hpp>

#include "slideshow.hxx"

#include <svx/fmshell.hxx>
#include <osl/mutex.hxx>
#include <vcl/svapp.hxx>
#include <boost/shared_ptr.hpp>

using namespace ::std;
using ::rtl::OUString;
using namespace ::cppu;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::drawing::framework;

namespace sd {

DrawController::DrawController (ViewShellBase& rBase) throw()
    : DrawControllerInterfaceBase(&rBase),
      BroadcastHelperOwner(SfxBaseController::m_aMutex),
      OPropertySetHelper( static_cast<OBroadcastHelperVar<
          OMultiTypeInterfaceContainerHelper,
          OMultiTypeInterfaceContainerHelper::keyType>& >(
              BroadcastHelperOwner::maBroadcastHelper)),
      m_aSelectionTypeIdentifier(
        ::getCppuType( (Reference<view::XSelectionChangeListener > *)0 )),
      mpBase(&rBase),
      maLastVisArea(),
      mpCurrentPage(NULL),
      mbMasterPageMode(false),
      mbLayerMode(false),
      mbDisposing(false),
      mpPropertyArrayHelper(NULL),
      mxSubController(),
      mxConfigurationController(),
      mxModuleController()
{
    ProvideFrameworkControllers();
}




DrawController::~DrawController (void) throw()
{
}




void DrawController::SetSubController (
    const Reference<drawing::XDrawSubController>& rxSubController)
{
    // Update the internal state.
    mxSubController = rxSubController;
    mpPropertyArrayHelper.reset();
    maLastVisArea = Rectangle();

    // Inform listeners about the changed state.
    FireSelectionChangeListener();
}




// XInterface

IMPLEMENT_FORWARD_XINTERFACE2(
    DrawController,
    DrawControllerInterfaceBase,
    OPropertySetHelper);


// XTypeProvider

Sequence<Type> SAL_CALL DrawController::getTypes (void)
    throw (::com::sun::star::uno::RuntimeException)
{
    ThrowIfDisposed();
    // OPropertySetHelper does not provide getTypes, so we have to
    // implement this method manually and list its three interfaces.
    OTypeCollection aTypeCollection (
        ::getCppuType (( const Reference<beans::XMultiPropertySet>*)NULL),
        ::getCppuType (( const Reference<beans::XFastPropertySet>*)NULL),
        ::getCppuType (( const Reference<beans::XPropertySet>*)NULL));

    return ::comphelper::concatSequences(
        SfxBaseController::getTypes(),
        aTypeCollection.getTypes(),
        DrawControllerInterfaceBase::getTypes());
}

IMPLEMENT_GET_IMPLEMENTATION_ID(DrawController);



// XComponent


void SAL_CALL DrawController::dispose (void)
    throw( RuntimeException )
{
    if( !mbDisposing )
    {
        SolarMutexGuard aGuard;

        if( !mbDisposing )
        {
            mbDisposing = true;

            boost::shared_ptr<ViewShell> pViewShell;
            if (mpBase)
                pViewShell = mpBase->GetMainViewShell();
            if ( pViewShell )
            {
                pViewShell->DeactivateCurrentFunction();
                DrawDocShell* pDocShell = pViewShell->GetDocSh();
                if ( pDocShell != NULL )
                    pDocShell->SetDocShellFunction(0);
            }
            pViewShell.reset();

            // When the controller has not been detached from its view
            // shell, i.e. mpViewShell is not NULL, then tell PaneManager
            // and ViewShellManager to clear the shell stack.
            if (mxSubController.is() && mpBase!=NULL)
            {
                mpBase->DisconnectAllClients();
                mpBase->GetViewShellManager()->Shutdown();
            }

            OPropertySetHelper::disposing();

            DisposeFrameworkControllers();

            SfxBaseController::dispose();
        }
    }
}




void SAL_CALL DrawController::addEventListener(
    const Reference<lang::XEventListener >& xListener)
    throw (RuntimeException)
{
    ThrowIfDisposed();
    SfxBaseController::addEventListener( xListener );
}




void SAL_CALL DrawController::removeEventListener (
    const Reference<lang::XEventListener >& aListener)
    throw (RuntimeException)
{
    if(!rBHelper.bDisposed && !rBHelper.bInDispose && !mbDisposing)
        SfxBaseController::removeEventListener( aListener );
}

// XController
::sal_Bool SAL_CALL DrawController::suspend( ::sal_Bool Suspend ) throw (::com::sun::star::uno::RuntimeException)
{
    if( Suspend )
    {
        ViewShellBase* pViewShellBase = GetViewShellBase();
        if( pViewShellBase )
        {
            // do not allow suspend if a slideshow needs this controller!
            rtl::Reference< SlideShow > xSlideShow( SlideShow::GetSlideShow( *pViewShellBase ) );
            if( xSlideShow.is() && xSlideShow->dependsOn(pViewShellBase) )
                return sal_False;
        }
    }

    return SfxBaseController::suspend( Suspend );
}


// XServiceInfo

OUString SAL_CALL DrawController::getImplementationName(  ) throw(RuntimeException)
{
    // Do not throw an excepetion at the moment.  This leads to a crash
    // under Solaris on relead.  See issue i70929 for details.
    //    ThrowIfDisposed();
    return OUString("DrawController") ;
}



static OUString ssServiceName( "com.sun.star.drawing.DrawingDocumentDrawView");

sal_Bool SAL_CALL DrawController::supportsService (
    const OUString& rsServiceName)
    throw(RuntimeException)
{
    // Do not throw an exception at the moment.  This leads to a crash
    // under Solaris on relead.  See issue i70929 for details.
    //    ThrowIfDisposed();
    return rsServiceName.equals(ssServiceName);
}




Sequence<OUString> SAL_CALL DrawController::getSupportedServiceNames (void)
    throw(RuntimeException)
{
    ThrowIfDisposed();
    Sequence<OUString> aSupportedServices (1);
    OUString* pServices = aSupportedServices.getArray();
    pServices[0] = ssServiceName;
    return aSupportedServices;
}




//------ XSelectionSupplier --------------------------------------------

sal_Bool SAL_CALL DrawController::select (const Any& aSelection)
    throw(lang::IllegalArgumentException, RuntimeException)
{
    ThrowIfDisposed();
    SolarMutexGuard aGuard;

    if (mxSubController.is())
        return mxSubController->select(aSelection);
    else
        return false;
}




Any SAL_CALL DrawController::getSelection()
    throw(RuntimeException)
{
    ThrowIfDisposed();
    SolarMutexGuard aGuard;

    if (mxSubController.is())
        return mxSubController->getSelection();
    else
        return Any();
}




void SAL_CALL DrawController::addSelectionChangeListener(
    const Reference< view::XSelectionChangeListener >& xListener)
    throw(RuntimeException)
{
    if( mbDisposing )
        throw lang::DisposedException();

    BroadcastHelperOwner::maBroadcastHelper.addListener (m_aSelectionTypeIdentifier, xListener);
}




void SAL_CALL DrawController::removeSelectionChangeListener(
    const Reference< view::XSelectionChangeListener >& xListener )
    throw(RuntimeException)
{
    if (rBHelper.bDisposed)
        throw lang::DisposedException();

    BroadcastHelperOwner::maBroadcastHelper.removeListener (m_aSelectionTypeIdentifier, xListener);
}





//=====  lang::XEventListener  ================================================

void SAL_CALL
    DrawController::disposing (const lang::EventObject& )
    throw (uno::RuntimeException)
{
}




//=====  view::XSelectionChangeListener  ======================================

void  SAL_CALL
    DrawController::selectionChanged (const lang::EventObject& rEvent)
        throw (uno::RuntimeException)
{
    ThrowIfDisposed();
    // Have to forward the event to our selection change listeners.
    OInterfaceContainerHelper* pListeners = BroadcastHelperOwner::maBroadcastHelper.getContainer(
        ::getCppuType((Reference<view::XSelectionChangeListener>*)0));
    if (pListeners)
    {
        // Re-send the event to all of our listeners.
        OInterfaceIteratorHelper aIterator (*pListeners);
        while (aIterator.hasMoreElements())
        {
            try
            {
                view::XSelectionChangeListener* pListener =
                    static_cast<view::XSelectionChangeListener*>(
                        aIterator.next());
                if (pListener != NULL)
                    pListener->selectionChanged (rEvent);
            }
            catch (const RuntimeException&)
            {
            }
        }
    }
}




// XDrawView

void SAL_CALL DrawController::setCurrentPage( const Reference< drawing::XDrawPage >& xPage )
    throw(RuntimeException)
{
    ThrowIfDisposed();
    SolarMutexGuard aGuard;

    if (mxSubController.is())
        mxSubController->setCurrentPage(xPage);
}




Reference< drawing::XDrawPage > SAL_CALL DrawController::getCurrentPage (void)
    throw(RuntimeException)
{
    ThrowIfDisposed();
    SolarMutexGuard aGuard;
    Reference<drawing::XDrawPage> xPage;

    // Get current page from sub controller.
    if (mxSubController.is())
        xPage = mxSubController->getCurrentPage();

    // When there is not yet a sub controller (during initialization) then fall back
    // to the current page in mpCurrentPage.
    if ( ! xPage.is() && mpCurrentPage.is())
        xPage = Reference<drawing::XDrawPage>(mpCurrentPage->getUnoPage(), UNO_QUERY);

    return xPage;
}




void DrawController::FireVisAreaChanged (const Rectangle& rVisArea) throw()
{
    if( maLastVisArea != rVisArea )
    {
        Any aNewValue;
        aNewValue <<= awt::Rectangle(
            rVisArea.Left(),
            rVisArea.Top(),
            rVisArea.GetWidth(),
            rVisArea.GetHeight() );

        Any aOldValue;
        aOldValue <<= awt::Rectangle(
            maLastVisArea.Left(),
            maLastVisArea.Top(),
            maLastVisArea.GetWidth(),
            maLastVisArea.GetHeight() );

        FirePropertyChange (PROPERTY_WORKAREA, aNewValue, aOldValue);

        maLastVisArea = rVisArea;
    }
}




void DrawController::FireSelectionChangeListener() throw()
{
    OInterfaceContainerHelper * pLC = BroadcastHelperOwner::maBroadcastHelper.getContainer(
        m_aSelectionTypeIdentifier);
    if( pLC )
    {
        Reference< XInterface > xSource( (XWeak*)this );
        const lang::EventObject aEvent( xSource );

        // Ueber alle Listener iterieren und Events senden
        OInterfaceIteratorHelper aIt( *pLC);
        while( aIt.hasMoreElements() )
        {
            try
            {
                view::XSelectionChangeListener * pL =
                    static_cast<view::XSelectionChangeListener*>(aIt.next());
                if (pL != NULL)
                    pL->selectionChanged( aEvent );
            }
            catch (const RuntimeException&)
            {
            }
        }
    }
}




void DrawController::FireChangeEditMode (bool bMasterPageMode) throw()
{
    if (bMasterPageMode != mbMasterPageMode )
    {
        FirePropertyChange(
            PROPERTY_MASTERPAGEMODE,
            makeAny(bMasterPageMode),
            makeAny(mbMasterPageMode));

        mbMasterPageMode = bMasterPageMode;
    }
}




void DrawController::FireChangeLayerMode (bool bLayerMode) throw()
{
    if (bLayerMode != mbLayerMode)
    {
        FirePropertyChange(
            PROPERTY_LAYERMODE,
            makeAny(bLayerMode),
            makeAny(mbLayerMode));

        mbLayerMode = bLayerMode;
    }
}




void DrawController::FireSwitchCurrentPage (SdPage* pNewCurrentPage) throw()
{
    SdrPage* pCurrentPage  = mpCurrentPage.get();
    if (pNewCurrentPage != pCurrentPage)
    {
        try
        {
            Any aNewValue (
                makeAny(Reference<drawing::XDrawPage>(pNewCurrentPage->getUnoPage(), UNO_QUERY)));

            Any aOldValue;
            if (pCurrentPage != NULL)
            {
                Reference<drawing::XDrawPage> xOldPage (pCurrentPage->getUnoPage(), UNO_QUERY);
                aOldValue <<= xOldPage;
            }

            FirePropertyChange(PROPERTY_CURRENTPAGE, aNewValue, aOldValue);

            mpCurrentPage.reset(pNewCurrentPage);
        }
        catch (const uno::Exception&)
        {
            OSL_FAIL(
                ("sd::SdUnoDrawView::FireSwitchCurrentPage(), exception caught: " +
                    ::rtl::OUStringToOString(
                        comphelper::anyToString( cppu::getCaughtException() ),
                        RTL_TEXTENCODING_UTF8 )).getStr() );
        }
    }
}




void DrawController::FirePropertyChange (
    sal_Int32 nHandle,
    const Any& rNewValue,
    const Any& rOldValue)
{
    try
    {
        fire (&nHandle, &rNewValue, &rOldValue, 1, sal_False);
    }
    catch (const RuntimeException&)
    {
        // Ignore this exception.  Exceptions should be handled in the
        // fire() function so that all listeners are called.  This is
        // not the case at the moment, so we simply ignore the
        // exception.
    }

}




ViewShellBase* DrawController::GetViewShellBase (void)
{
    return mpBase;
}




void DrawController::ReleaseViewShellBase (void)
{
    DisposeFrameworkControllers();
    mpBase = NULL;
}




//===== XControllerManager ==============================================================

Reference<XConfigurationController> SAL_CALL
    DrawController::getConfigurationController (void)
    throw (RuntimeException)
{
    ThrowIfDisposed();

    return mxConfigurationController;
}




Reference<XModuleController> SAL_CALL
    DrawController::getModuleController (void)
    throw (RuntimeException)
{
    ThrowIfDisposed();

    return mxModuleController;
}




//===== XUnoTunnel ============================================================

namespace
{
    class theDrawControllerUnoTunnelId : public rtl::Static< UnoTunnelIdInit, theDrawControllerUnoTunnelId> {};
}

const Sequence<sal_Int8>& DrawController::getUnoTunnelId (void)
{
    return theDrawControllerUnoTunnelId::get().getSeq();
}




sal_Int64 SAL_CALL DrawController::getSomething (const Sequence<sal_Int8>& rId)
    throw (RuntimeException)
{
    sal_Int64 nResult = 0;

    if (rId.getLength() == 16
        && rtl_compareMemory(getUnoTunnelId().getConstArray(), rId.getConstArray(), 16) == 0)
    {
        nResult = sal::static_int_cast<sal_Int64>(reinterpret_cast<sal_IntPtr>(this));
    }

    return nResult;
}




//===== Properties ============================================================

void DrawController::FillPropertyTable (
    ::std::vector<beans::Property>& rProperties)
{
    rProperties.push_back(
        beans::Property("VisibleArea",
            PROPERTY_WORKAREA,
            ::getCppuType((const ::com::sun::star::awt::Rectangle*)0),
            beans::PropertyAttribute::BOUND | beans::PropertyAttribute::READONLY));
    rProperties.push_back(
        beans::Property(
            "SubController",
            PROPERTY_SUB_CONTROLLER,
            ::getCppuType((const Reference<drawing::XDrawSubController>*)0),
            beans::PropertyAttribute::BOUND));
    rProperties.push_back(
        beans::Property(
            "CurrentPage",
            PROPERTY_CURRENTPAGE,
            ::getCppuType((const Reference< drawing::XDrawPage > *)0),
            beans::PropertyAttribute::BOUND ));
    rProperties.push_back(
        beans::Property("IsLayerMode",
            PROPERTY_LAYERMODE,
            ::getCppuBooleanType(),
            beans::PropertyAttribute::BOUND ));
    rProperties.push_back(
        beans::Property("IsMasterPageMode",
            PROPERTY_MASTERPAGEMODE,
            ::getCppuBooleanType(),
            beans::PropertyAttribute::BOUND ));
    rProperties.push_back(
        beans::Property("ActiveLayer",
            PROPERTY_ACTIVE_LAYER,
            ::getCppuBooleanType(),
            beans::PropertyAttribute::BOUND ));
    rProperties.push_back(
        beans::Property("ZoomValue",
            PROPERTY_ZOOMVALUE,
            ::getCppuType((const sal_Int16*)0),
            beans::PropertyAttribute::BOUND ));
    rProperties.push_back(
        beans::Property("ZoomType",
            PROPERTY_ZOOMTYPE,
            ::getCppuType((const sal_Int16*)0),
            beans::PropertyAttribute::BOUND ));
    rProperties.push_back(
        beans::Property("ViewOffset",
            PROPERTY_VIEWOFFSET,
            ::getCppuType((const ::com::sun::star::awt::Point*)0),
            beans::PropertyAttribute::BOUND ));
    rProperties.push_back(
        beans::Property("DrawViewMode",
            PROPERTY_DRAWVIEWMODE,
            ::getCppuType((const ::com::sun::star::awt::Point*)0),
            beans::PropertyAttribute::BOUND|beans::PropertyAttribute::READONLY|beans::PropertyAttribute::MAYBEVOID ));
}




IPropertyArrayHelper & DrawController::getInfoHelper()
{
    SolarMutexGuard aGuard;

    if (mpPropertyArrayHelper.get() == NULL)
    {
        ::std::vector<beans::Property> aProperties;
        FillPropertyTable (aProperties);
        Sequence<beans::Property> aPropertySequence (aProperties.size());
        for (unsigned int i=0; i<aProperties.size(); i++)
            aPropertySequence[i] = aProperties[i];
        mpPropertyArrayHelper.reset(new OPropertyArrayHelper(aPropertySequence, sal_False));
    }

    return *mpPropertyArrayHelper.get();
}




Reference < beans::XPropertySetInfo >  DrawController::getPropertySetInfo()
        throw ( ::com::sun::star::uno::RuntimeException)
{
    SolarMutexGuard aGuard;

    static Reference < beans::XPropertySetInfo >  xInfo( createPropertySetInfo( getInfoHelper() ) );
    return xInfo;
}


uno::Reference< form::runtime::XFormController > SAL_CALL DrawController::getFormController( const uno::Reference< form::XForm >& Form ) throw (uno::RuntimeException)
{
    SolarMutexGuard aGuard;

    FmFormShell* pFormShell = mpBase->GetFormShellManager()->GetFormShell();
    SdrView* pSdrView = mpBase->GetDrawView();
    ::boost::shared_ptr<ViewShell> pViewShell = mpBase->GetMainViewShell();
    ::sd::Window* pWindow = pViewShell ? pViewShell->GetActiveWindow() : NULL;

    uno::Reference< form::runtime::XFormController > xController( NULL );
    if ( pFormShell && pSdrView && pWindow )
        xController = pFormShell->GetFormController( Form, *pSdrView, *pWindow );
    return xController;
}

::sal_Bool SAL_CALL DrawController::isFormDesignMode(  ) throw (uno::RuntimeException)
{
    SolarMutexGuard aGuard;

    sal_Bool bIsDesignMode = sal_True;

    FmFormShell* pFormShell = mpBase->GetFormShellManager()->GetFormShell();
    if ( pFormShell )
        bIsDesignMode = pFormShell->IsDesignMode();

    return bIsDesignMode;
}

void SAL_CALL DrawController::setFormDesignMode( ::sal_Bool _DesignMode ) throw (uno::RuntimeException)
{
    SolarMutexGuard aGuard;

    FmFormShell* pFormShell = mpBase->GetFormShellManager()->GetFormShell();
    if ( pFormShell )
        pFormShell->SetDesignMode( _DesignMode );
}

uno::Reference< awt::XControl > SAL_CALL DrawController::getControl( const uno::Reference< awt::XControlModel >& xModel ) throw (container::NoSuchElementException, uno::RuntimeException)
{
    SolarMutexGuard aGuard;

    FmFormShell* pFormShell = mpBase->GetFormShellManager()->GetFormShell();
    SdrView* pSdrView = mpBase->GetDrawView();
    ::boost::shared_ptr<ViewShell> pViewShell = mpBase->GetMainViewShell();
    ::sd::Window* pWindow = pViewShell ? pViewShell->GetActiveWindow() : NULL;

    uno::Reference< awt::XControl > xControl( NULL );
    if ( pFormShell && pSdrView && pWindow )
        pFormShell->GetFormControl( xModel, *pSdrView, *pWindow, xControl );
    return xControl;
}




sal_Bool DrawController::convertFastPropertyValue (
    Any & rConvertedValue,
    Any & rOldValue,
    sal_Int32 nHandle,
    const Any& rValue)
    throw ( com::sun::star::lang::IllegalArgumentException)
{
    sal_Bool bResult = sal_False;

    if (nHandle == PROPERTY_SUB_CONTROLLER)
    {
        rOldValue <<= mxSubController;
        rConvertedValue <<= Reference<drawing::XDrawSubController>(rValue, UNO_QUERY);
        bResult = (rOldValue != rConvertedValue);
    }
    else if (mxSubController.is())
    {
        rConvertedValue = rValue;
        try
        {
            rOldValue = mxSubController->getFastPropertyValue(nHandle);
            bResult = (rOldValue != rConvertedValue);
        }
        catch (const beans::UnknownPropertyException&)
        {
            // The property is unknown and thus an illegal argument to this method.
            throw com::sun::star::lang::IllegalArgumentException();
        }
    }

    return bResult;
}




void DrawController::setFastPropertyValue_NoBroadcast (
    sal_Int32 nHandle,
    const Any& rValue)
    throw ( com::sun::star::uno::Exception)
{
    SolarMutexGuard aGuard;
    if (nHandle == PROPERTY_SUB_CONTROLLER)
        SetSubController(Reference<drawing::XDrawSubController>(rValue, UNO_QUERY));
    else if (mxSubController.is())
        mxSubController->setFastPropertyValue(nHandle, rValue);
}




void DrawController::getFastPropertyValue (
    Any & rRet,
    sal_Int32 nHandle ) const
{
    SolarMutexGuard aGuard;

    switch( nHandle )
    {
        case PROPERTY_WORKAREA:
            rRet <<= awt::Rectangle(
                maLastVisArea.Left(),
                maLastVisArea.Top(),
                maLastVisArea.GetWidth(),
                maLastVisArea.GetHeight());
            break;

        case PROPERTY_SUB_CONTROLLER:
            rRet <<= mxSubController;
            break;

        default:
            if (mxSubController.is())
                rRet = mxSubController->getFastPropertyValue(nHandle);
            break;
    }
}




//-----------------------------------------------------------------------------

void DrawController::ProvideFrameworkControllers (void)
{
    SolarMutexGuard aGuard;
    try
    {
        Reference<XController> xController (this);
        const Reference<XComponentContext> xContext (
            ::comphelper::getProcessComponentContext() );
        mxConfigurationController = ConfigurationController::create(
            xContext,
            xController);
        mxModuleController = ModuleController::create(
            xContext,
            xController);
    }
    catch (const RuntimeException&)
    {
        mxConfigurationController = NULL;
        mxModuleController = NULL;
    }
}




void DrawController::DisposeFrameworkControllers (void)
{
    Reference<XComponent> xComponent (mxModuleController, UNO_QUERY);
    if (xComponent.is())
        xComponent->dispose();

    xComponent = Reference<XComponent>(mxConfigurationController, UNO_QUERY);
    if (xComponent.is())
        xComponent->dispose();
}




void DrawController::ThrowIfDisposed (void) const
    throw (::com::sun::star::lang::DisposedException)
{
    if (rBHelper.bDisposed || rBHelper.bInDispose || mbDisposing)
    {
        OSL_TRACE ("Calling disposed DrawController object. Throwing exception:");
        throw lang::DisposedException (
            "DrawController object has already been disposed",
            const_cast<uno::XWeak*>(static_cast<const uno::XWeak*>(this)));
    }
}





} // end of namespace sd


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
