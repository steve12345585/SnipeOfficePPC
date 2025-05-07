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

#ifndef _FRM_IMAGE_CONTROL_HXX_
#define _FRM_IMAGE_CONTROL_HXX_

#include "FormComponent.hxx"
#include "imgprod.hxx"
#include <com/sun/star/form/XImageProducerSupplier.hpp>
#include <com/sun/star/awt/XMouseListener.hpp>
#include <com/sun/star/util/XModifyBroadcaster.hpp>
#include <com/sun/star/graphic/XGraphicObject.hpp>
#include <comphelper/propmultiplex.hxx>
#include <comphelper/implementationreference.hxx>
#include <cppuhelper/implbase2.hxx>

using namespace comphelper;

//.........................................................................
namespace frm
{
//.........................................................................

//==================================================================
// OImageControlModel
//==================================================================
typedef ::cppu::ImplHelper2 <   ::com::sun::star::form::XImageProducerSupplier
                            ,   ::com::sun::star::awt::XImageProducer
                            >   OImageControlModel_Base;

class OImageControlModel
                :public OImageControlModel_Base
                ,public OBoundControlModel
{
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XImageProducer>    m_xImageProducer;
    ImageProducer*                                  m_pImageProducer;
    bool                                            m_bExternalGraphic;
    sal_Bool                                        m_bReadOnly;
    ::rtl::OUString                                 m_sImageURL;
    ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphicObject >
                                                    m_xGraphicObject;
    ::rtl::OUString                                 m_sDocumentURL;

protected:
    // UNO Anbindung
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type> _getTypes();

    inline ImageProducer* GetImageProducer() { return m_pImageProducer; }

public:
    DECLARE_DEFAULT_LEAF_XTOR( OImageControlModel );

    virtual void SAL_CALL getFastPropertyValue(::com::sun::star::uno::Any& rValue, sal_Int32 nHandle ) const;
    virtual void SAL_CALL setFastPropertyValue_NoBroadcast(sal_Int32 nHandle, const ::com::sun::star::uno::Any& rValue) throw ( ::com::sun::star::uno::Exception);

    virtual sal_Bool SAL_CALL convertFastPropertyValue(::com::sun::star::uno::Any& rConvertedValue, ::com::sun::star::uno::Any& rOldValue, sal_Int32 nHandle, const ::com::sun::star::uno::Any& rValue )
        throw(::com::sun::star::lang::IllegalArgumentException);

    // UNO Anbindung
    DECLARE_UNO3_AGG_DEFAULTS(OImageControlModel, OBoundControlModel);
    virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation(const ::com::sun::star::uno::Type& _rType) throw(::com::sun::star::uno::RuntimeException);

    // XServiceInfo
    IMPLEMENTATION_NAME(OImageControlModel);
    virtual StringSequence SAL_CALL getSupportedServiceNames() throw();

    // OComponentHelper
    virtual void SAL_CALL disposing();

    // XPersistObject
    virtual ::rtl::OUString SAL_CALL getServiceName() throw ( ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL write(const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectOutputStream>& _rxOutStream) throw ( ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL read(const ::com::sun::star::uno::Reference< ::com::sun::star::io::XObjectInputStream>& _rxInStream) throw ( ::com::sun::star::io::IOException, ::com::sun::star::uno::RuntimeException);

    // XImageProducerSupplier
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::awt::XImageProducer> SAL_CALL getImageProducer() throw ( ::com::sun::star::uno::RuntimeException);

    // XImageProducer
    virtual void SAL_CALL addConsumer( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XImageConsumer >& xConsumer ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeConsumer( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XImageConsumer >& xConsumer ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL startProduction(  ) throw (::com::sun::star::uno::RuntimeException);

    // OControlModel's property handling
    virtual void describeAggregateProperties(
        ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& /* [out] */ _rAggregateProps
    ) const;
    virtual void describeFixedProperties(
        ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >& /* [out] */ _rProps
    ) const;

    // prevent method hiding
    using OBoundControlModel::disposing;
    using OBoundControlModel::getFastPropertyValue;

protected:
    // OBoundControlModel overridables
    virtual void            onConnectedDbColumn( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& _rxForm );
    virtual void            onDisconnectedDbColumn();
    virtual ::com::sun::star::uno::Any
                            translateDbColumnToControlValue( );
    virtual sal_Bool        commitControlValueToDbColumn( bool _bPostReset );

    virtual ::com::sun::star::uno::Any
                            getControlValue( ) const;
    virtual void            doSetControlValue( const ::com::sun::star::uno::Any& _rValue );

    virtual sal_Bool        approveDbColumnType(sal_Int32 _nColumnType);

    virtual void            resetNoBroadcast();

protected:
    DECLARE_XCLONEABLE();

    void implConstruct();

    /** displays the image described by the given URL
        @precond
            our own mutex is locked
    */
    sal_Bool    impl_handleNewImageURL_lck( ValueChangeInstigator _eInstigator );

    /** updates the binary stream, created from loading the file which the given URL points to, into our
        bound field, or the control itself if there is no bound field
    */
    sal_Bool    impl_updateStreamForURL_lck( const ::rtl::OUString& _rURL, ValueChangeInstigator _eInstigator );

    DECL_LINK( OnImageImportDone, ::Graphic* );
};

//==================================================================
//= OImageControlControl
//==================================================================
typedef ::cppu::ImplHelper2 <   ::com::sun::star::awt::XMouseListener
                            ,   ::com::sun::star::util::XModifyBroadcaster
                            >   OImageControlControl_Base;
class OImageControlControl  : public OBoundControl
                            , public OImageControlControl_Base
{
private:
    ::cppu::OInterfaceContainerHelper   m_aModifyListeners;

    // XTypeProvider
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type> _getTypes();

public:
    OImageControlControl(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory>& _rxFactory);

    // UNO
    DECLARE_UNO3_AGG_DEFAULTS( OImageControlControl, OBoundControl );
    virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type& _rType ) throw(::com::sun::star::uno::RuntimeException);

    // XEventListener
    virtual void SAL_CALL disposing(const ::com::sun::star::lang::EventObject& _rSource) throw(::com::sun::star::uno::RuntimeException);

    // XServiceInfo
    IMPLEMENTATION_NAME(OImageControlControl);
    virtual StringSequence SAL_CALL getSupportedServiceNames() throw();

    // XMouseListener
    virtual void SAL_CALL mousePressed(const ::com::sun::star::awt::MouseEvent& e) throw ( ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL mouseReleased(const ::com::sun::star::awt::MouseEvent& e) throw ( ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL mouseEntered(const ::com::sun::star::awt::MouseEvent& e) throw ( ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL mouseExited(const ::com::sun::star::awt::MouseEvent& e) throw ( ::com::sun::star::uno::RuntimeException);

    // XModifyBroadcaster
    virtual void SAL_CALL addModifyListener( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener >& aListener ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeModifyListener( const ::com::sun::star::uno::Reference< ::com::sun::star::util::XModifyListener >& aListener ) throw (::com::sun::star::uno::RuntimeException);

    // OComponentHelper
    virtual void SAL_CALL disposing();

private:
    void    implClearGraphics( sal_Bool _bForce );
    bool    implInsertGraphics();

    /** determines whether the control does currently have an empty grahic set
    */
    bool    impl_isEmptyGraphics_nothrow() const;
};

//.........................................................................
}   // namespace frm
//.........................................................................

#endif // _FRM_IMAGE_CONTROL_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
