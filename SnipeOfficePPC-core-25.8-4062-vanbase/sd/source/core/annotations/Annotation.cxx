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


#include "osl/time.h"
#include "sal/config.h"

#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/office/XAnnotation.hpp>
#include <com/sun/star/drawing/XDrawPage.hpp>

#include <comphelper/processfactory.hxx>
#include <cppuhelper/propertysetmixin.hxx>
#include <cppuhelper/compbase1.hxx>
#include <cppuhelper/basemutex.hxx>

#include "drawdoc.hxx"
#include "sdpage.hxx"
#include "textapi.hxx"

using ::rtl::OUString;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::office;
using namespace ::com::sun::star::drawing;
using namespace ::com::sun::star::geometry;
using namespace ::com::sun::star::text;
using namespace ::com::sun::star::util;
using namespace ::com::sun::star;

extern void NotifyDocumentEvent( SdDrawDocument* pDocument, const rtl::OUString& rEventName, const Reference< XInterface >& xSource );

namespace sd {

class Annotation : private ::cppu::BaseMutex,
                   public ::cppu::WeakComponentImplHelper1< XAnnotation>,
                   public ::cppu::PropertySetMixin< XAnnotation >
{
public:
    explicit Annotation( const Reference< XComponentContext >& context, SdPage* pPage );

    SdPage* GetPage() const { return mpPage; }
    SdrModel* GetModel() { return (mpPage != 0) ? mpPage->GetModel() : 0; }

    // XInterface:
    virtual Any SAL_CALL queryInterface(Type const & type) throw (RuntimeException);
    virtual void SAL_CALL acquire() throw () { ::cppu::WeakComponentImplHelper1< XAnnotation >::acquire(); }
    virtual void SAL_CALL release() throw () { ::cppu::WeakComponentImplHelper1< XAnnotation >::release(); }

    // ::com::sun::star::beans::XPropertySet:
    virtual Reference< XPropertySetInfo > SAL_CALL getPropertySetInfo() throw (RuntimeException);
    virtual void SAL_CALL setPropertyValue(const OUString & aPropertyName, const Any & aValue) throw (RuntimeException, UnknownPropertyException, PropertyVetoException, IllegalArgumentException, WrappedTargetException);
    virtual Any SAL_CALL getPropertyValue(const OUString & PropertyName) throw (RuntimeException, UnknownPropertyException, WrappedTargetException);
    virtual void SAL_CALL addPropertyChangeListener(const OUString & aPropertyName, const Reference< XPropertyChangeListener > & xListener) throw (RuntimeException, UnknownPropertyException, WrappedTargetException);
    virtual void SAL_CALL removePropertyChangeListener(const OUString & aPropertyName, const Reference< XPropertyChangeListener > & aListener) throw (RuntimeException, UnknownPropertyException, WrappedTargetException);
    virtual void SAL_CALL addVetoableChangeListener(const OUString & PropertyName, const Reference< XVetoableChangeListener > & aListener) throw (RuntimeException, UnknownPropertyException, WrappedTargetException);
    virtual void SAL_CALL removeVetoableChangeListener(const OUString & PropertyName, const Reference< XVetoableChangeListener > & aListener) throw (RuntimeException, UnknownPropertyException, WrappedTargetException);

    // ::com::sun::star::office::XAnnotation:
    virtual ::com::sun::star::uno::Any SAL_CALL getAnchor() throw (::com::sun::star::uno::RuntimeException);
    virtual RealPoint2D SAL_CALL getPosition() throw (RuntimeException);
    virtual void SAL_CALL setPosition(const RealPoint2D & the_value) throw (RuntimeException);
    virtual ::com::sun::star::geometry::RealSize2D SAL_CALL getSize() throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setSize( const ::com::sun::star::geometry::RealSize2D& _size ) throw (::com::sun::star::uno::RuntimeException);
    virtual OUString SAL_CALL getAuthor() throw (RuntimeException);
    virtual void SAL_CALL setAuthor(const OUString & the_value) throw (RuntimeException);
    virtual util::DateTime SAL_CALL getDateTime() throw (RuntimeException);
    virtual void SAL_CALL setDateTime(const util::DateTime & the_value) throw (RuntimeException);
    virtual Reference< XText > SAL_CALL getTextRange() throw (RuntimeException);

private:
    Annotation(const Annotation &); // not defined
    Annotation& operator=(const Annotation &); // not defined

    // destructor is private and will be called indirectly by the release call    virtual ~Annotation() {}

    void createChangeUndo();

    // overload WeakComponentImplHelperBase::disposing()
    // This function is called upon disposing the component,
    // if your component needs special work when it becomes
    // disposed, do it here.
    virtual void SAL_CALL disposing();

    SdPage* mpPage;
    Reference< XComponentContext > m_xContext;
    mutable ::osl::Mutex m_aMutex;
    RealPoint2D m_Position;
    RealSize2D m_Size;
    OUString m_Author;
    util::DateTime m_DateTime;
    rtl::Reference< TextApiObject > m_TextRange;
};

class UndoInsertOrRemoveAnnotation : public SdrUndoAction
{
public:
    UndoInsertOrRemoveAnnotation( Annotation& rAnnotation, bool bInsert );

    virtual void Undo();
    virtual void Redo();

protected:
    rtl::Reference< Annotation > mxAnnotation;
    bool mbInsert;
    int mnIndex;
};

struct AnnotationData
{
    RealPoint2D m_Position;
    RealSize2D m_Size;
    OUString m_Author;
    util::DateTime m_DateTime;

    void get( const rtl::Reference< Annotation >& xAnnotation )
    {
        m_Position = xAnnotation->getPosition();
        m_Size = xAnnotation->getSize();
        m_Author = xAnnotation->getAuthor();
        m_DateTime = xAnnotation->getDateTime();
    }

    void set( const rtl::Reference< Annotation >& xAnnotation )
    {
        xAnnotation->setPosition(m_Position);
        xAnnotation->setSize(m_Size);
        xAnnotation->setAuthor(m_Author);
        xAnnotation->setDateTime(m_DateTime);
    }
};

class UndoAnnotation : public SdrUndoAction
{
public:
    UndoAnnotation( Annotation& rAnnotation );

    virtual void Undo();
    virtual void Redo();

protected:
    rtl::Reference< Annotation > mxAnnotation;
    AnnotationData maUndoData;
    AnnotationData maRedoData;
};

void createAnnotation( Reference< XAnnotation >& xAnnotation, SdPage* pPage )
{
    Reference<XComponentContext> xContext (comphelper_getProcessComponentContext());
    xAnnotation.set( new Annotation(xContext, pPage) );
    pPage->addAnnotation(xAnnotation);
}

Annotation::Annotation( const Reference< XComponentContext >& context, SdPage* pPage )
: ::cppu::WeakComponentImplHelper1< XAnnotation >(m_aMutex)
, ::cppu::PropertySetMixin< XAnnotation >(context, static_cast< Implements >(IMPLEMENTS_PROPERTY_SET), Sequence< ::rtl::OUString >())
, mpPage( pPage )
{
}

// overload WeakComponentImplHelperBase::disposing()
// This function is called upon disposing the component,
// if your component needs special work when it becomes
// disposed, do it here.
void SAL_CALL Annotation::disposing()
{
    mpPage = 0;
    if( m_TextRange.is() )
    {
        m_TextRange->dispose();
        m_TextRange.clear();
    }
}

Any Annotation::queryInterface(Type const & type) throw (RuntimeException)
{
    return ::cppu::WeakComponentImplHelper1< XAnnotation>::queryInterface(type);
}

// com.sun.star.beans.XPropertySet:
Reference< XPropertySetInfo > SAL_CALL Annotation::getPropertySetInfo() throw (RuntimeException)
{
    return ::cppu::PropertySetMixin< XAnnotation >::getPropertySetInfo();
}

void SAL_CALL Annotation::setPropertyValue(const OUString & aPropertyName, const Any & aValue) throw (RuntimeException, UnknownPropertyException, PropertyVetoException, IllegalArgumentException, WrappedTargetException)
{
    ::cppu::PropertySetMixin< XAnnotation >::setPropertyValue(aPropertyName, aValue);
}

Any SAL_CALL Annotation::getPropertyValue(const OUString & aPropertyName) throw (RuntimeException, UnknownPropertyException, WrappedTargetException)
{
    return ::cppu::PropertySetMixin< XAnnotation >::getPropertyValue(aPropertyName);
}

void SAL_CALL Annotation::addPropertyChangeListener(const OUString & aPropertyName, const Reference< XPropertyChangeListener > & xListener) throw (RuntimeException, UnknownPropertyException, WrappedTargetException)
{
    ::cppu::PropertySetMixin< XAnnotation >::addPropertyChangeListener(aPropertyName, xListener);
}

void SAL_CALL Annotation::removePropertyChangeListener(const OUString & aPropertyName, const Reference< XPropertyChangeListener > & xListener) throw (RuntimeException, UnknownPropertyException, WrappedTargetException)
{
    ::cppu::PropertySetMixin< XAnnotation >::removePropertyChangeListener(aPropertyName, xListener);
}

void SAL_CALL Annotation::addVetoableChangeListener(const OUString & aPropertyName, const Reference< XVetoableChangeListener > & xListener) throw (RuntimeException, UnknownPropertyException, WrappedTargetException)
{
    ::cppu::PropertySetMixin< XAnnotation >::addVetoableChangeListener(aPropertyName, xListener);
}

void SAL_CALL Annotation::removeVetoableChangeListener(const OUString & aPropertyName, const Reference< XVetoableChangeListener > & xListener) throw (RuntimeException, UnknownPropertyException, WrappedTargetException)
{
    ::cppu::PropertySetMixin< XAnnotation >::removeVetoableChangeListener(aPropertyName, xListener);
}

Any SAL_CALL Annotation::getAnchor() throw (RuntimeException)
{
    osl::MutexGuard g(m_aMutex);
    Any aRet;
    if( mpPage )
    {
        Reference< XDrawPage > xPage( mpPage->getUnoPage(), UNO_QUERY );
        aRet <<= xPage;
    }
    return aRet;
}

// ::com::sun::star::office::XAnnotation:
RealPoint2D SAL_CALL Annotation::getPosition() throw (RuntimeException)
{
    osl::MutexGuard g(m_aMutex);
    return m_Position;
}

void SAL_CALL Annotation::setPosition(const RealPoint2D & the_value) throw (RuntimeException)
{
    prepareSet("Position", Any(), Any(), 0);
    {
        osl::MutexGuard g(m_aMutex);
        createChangeUndo();
        m_Position = the_value;
    }
}

// ::com::sun::star::office::XAnnotation:
RealSize2D SAL_CALL Annotation::getSize() throw (RuntimeException)
{
    osl::MutexGuard g(m_aMutex);
    return m_Size;
}

void SAL_CALL Annotation::setSize(const RealSize2D & the_value) throw (RuntimeException)
{
    prepareSet("Size", Any(), Any(), 0);
    {
        osl::MutexGuard g(m_aMutex);
        createChangeUndo();
        m_Size = the_value;
    }
}

OUString SAL_CALL Annotation::getAuthor() throw (RuntimeException)
{
    osl::MutexGuard g(m_aMutex);
    return m_Author;
}

void SAL_CALL Annotation::setAuthor(const OUString & the_value) throw (RuntimeException)
{
    prepareSet("Author", Any(), Any(), 0);
    {
        osl::MutexGuard g(m_aMutex);
        createChangeUndo();
        m_Author = the_value;
    }
}

util::DateTime SAL_CALL Annotation::getDateTime() throw (RuntimeException)
{
    osl::MutexGuard g(m_aMutex);
    return m_DateTime;
}

void SAL_CALL Annotation::setDateTime(const util::DateTime & the_value) throw (RuntimeException)
{
    prepareSet("DateTime", Any(), Any(), 0);
    {
        osl::MutexGuard g(m_aMutex);
        createChangeUndo();
        m_DateTime = the_value;
    }
}

void Annotation::createChangeUndo()
{
    SdrModel* pModel = GetModel();
    if( pModel && pModel->IsUndoEnabled() )
        pModel->AddUndo( new UndoAnnotation( *this ) );

    if( pModel )
    {
        pModel->SetChanged();
        Reference< XInterface > xSource( static_cast<uno::XWeak*>( this ) );
        NotifyDocumentEvent( static_cast< SdDrawDocument* >( pModel ), "OnAnnotationChanged" , xSource );
    }
}

Reference< XText > SAL_CALL Annotation::getTextRange() throw (RuntimeException)
{
    osl::MutexGuard g(m_aMutex);
    if( !m_TextRange.is() && (mpPage != 0) )
    {
        m_TextRange = TextApiObject::create( static_cast< SdDrawDocument* >( mpPage->GetModel() ) );
    }
    return Reference< XText >( m_TextRange.get() );
}

SdrUndoAction* CreateUndoInsertOrRemoveAnnotation( const Reference< XAnnotation >& xAnnotation, bool bInsert )
{
    Annotation* pAnnotation = dynamic_cast< Annotation* >( xAnnotation.get() );
    if( pAnnotation )
    {
        return new UndoInsertOrRemoveAnnotation( *pAnnotation, bInsert );
    }
    else
    {
        return 0;
    }
}

UndoInsertOrRemoveAnnotation::UndoInsertOrRemoveAnnotation( Annotation& rAnnotation, bool bInsert )
: SdrUndoAction( *rAnnotation.GetModel() )
, mxAnnotation( &rAnnotation )
, mbInsert( bInsert )
, mnIndex( 0 )
{
    SdPage* pPage = rAnnotation.GetPage();
    if( pPage )
    {
        Reference< XAnnotation > xAnnotation( &rAnnotation );

        const AnnotationVector& rVec = pPage->getAnnotations();
        for( AnnotationVector::const_iterator iter = rVec.begin(); iter != rVec.end(); ++iter )
        {
            if( (*iter) == xAnnotation )
                break;

            mnIndex++;
        }
    }
}

void UndoInsertOrRemoveAnnotation::Undo()
{
    SdPage* pPage = mxAnnotation->GetPage();
    SdrModel* pModel = mxAnnotation->GetModel();
    if( pPage && pModel )
    {
        Reference< XAnnotation > xAnnotation( mxAnnotation.get() );
        if( mbInsert )
        {
            pPage->removeAnnotation( xAnnotation );
        }
        else
        {
            pPage->addAnnotation( xAnnotation, mnIndex );
        }
    }
}

void UndoInsertOrRemoveAnnotation::Redo()
{
    SdPage* pPage = mxAnnotation->GetPage();
    SdrModel* pModel = mxAnnotation->GetModel();
    if( pPage && pModel )
    {
        Reference< XAnnotation > xAnnotation( mxAnnotation.get() );

        if( mbInsert )
        {
            pPage->addAnnotation( xAnnotation, mnIndex );
        }
        else
        {
            pPage->removeAnnotation( xAnnotation );
        }
    }
}

UndoAnnotation::UndoAnnotation( Annotation& rAnnotation )
: SdrUndoAction( *rAnnotation.GetModel() )
, mxAnnotation( &rAnnotation )
{
    maUndoData.get( mxAnnotation );
}

void UndoAnnotation::Undo()
{
    maRedoData.get( mxAnnotation );
    maUndoData.set( mxAnnotation );
}

void UndoAnnotation::Redo()
{
    maUndoData.get( mxAnnotation );
    maRedoData.set( mxAnnotation );
}

} // namespace sd

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
