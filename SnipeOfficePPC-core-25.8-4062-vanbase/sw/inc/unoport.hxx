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
#ifndef _UNOPORT_HXX
#define _UNOPORT_HXX

#include <unocrsr.hxx>
#include <unoevtlstnr.hxx>
#include <calbck.hxx>

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/text/XTextRange.hpp>
#include <com/sun/star/container/XEnumeration.hpp>
#include <com/sun/star/container/XContentEnumerationAccess.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/beans/XPropertyState.hpp>
#include <com/sun/star/beans/XMultiPropertySet.hpp>
#include <com/sun/star/beans/XTolerantMultiPropertySet.hpp>
#include <com/sun/star/text/XTextField.hpp>
#include <com/sun/star/text/XFootnote.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <cppuhelper/implbase8.hxx>
#include <cppuhelper/implbase3.hxx>
#include <svl/itemprop.hxx>

#include <memory>
#include <deque>


class SwFmtFld;
class SwFrmFmt;
class SwTxtRuby;


typedef ::std::deque<
    ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > >
    TextRangeList_t;

enum SwTextPortionType
{
    PORTION_TEXT,
    PORTION_FIELD,
    PORTION_FRAME,
    PORTION_FOOTNOTE,
    PORTION_REFMARK_START,
    PORTION_REFMARK_END,
    PORTION_TOXMARK_START,
    PORTION_TOXMARK_END,
    PORTION_BOOKMARK_START,
    PORTION_BOOKMARK_END,
    PORTION_REDLINE_START,
    PORTION_REDLINE_END,
    PORTION_RUBY_START,
    PORTION_RUBY_END,
    PORTION_SOFT_PAGEBREAK,
    PORTION_META,
    PORTION_FIELD_START,
    PORTION_FIELD_END,
    PORTION_FIELD_START_END
};


class SwXTextPortion : public cppu::WeakImplHelper8
<
    ::com::sun::star::beans::XTolerantMultiPropertySet,
    ::com::sun::star::beans::XMultiPropertySet,
    ::com::sun::star::beans::XPropertySet,
    ::com::sun::star::text::XTextRange,
    ::com::sun::star::beans::XPropertyState,
    ::com::sun::star::container::XContentEnumerationAccess,
    ::com::sun::star::lang::XUnoTunnel,
    ::com::sun::star::lang::XServiceInfo
>,
    public SwClient
{
private:

    SwEventListenerContainer    m_ListenerContainer;
    const SfxItemPropertySet *  m_pPropSet;
    const ::com::sun::star::uno::Reference< ::com::sun::star::text::XText >
        m_xParentText;
    ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextContent >
        m_xRefMark;
    ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextContent >
        m_xTOXMark;
    ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextContent >
        m_xBookmark;
    ::com::sun::star::uno::Reference< ::com::sun::star::text::XFootnote >
        m_xFootnote;
    ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextField >
        m_xTextField;
    ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextContent >
        m_xMeta;
    SAL_WNODEPRECATED_DECLARATIONS_PUSH
    ::std::auto_ptr< ::com::sun::star::uno::Any > m_pRubyText;
    ::std::auto_ptr< ::com::sun::star::uno::Any > m_pRubyStyle;
    ::std::auto_ptr< ::com::sun::star::uno::Any > m_pRubyAdjust;
    ::std::auto_ptr< ::com::sun::star::uno::Any > m_pRubyIsAbove;
    SAL_WNODEPRECATED_DECLARATIONS_POP

    const SwDepend              m_FrameDepend;
    SwFrmFmt *                  m_pFrameFmt;
    const SwTextPortionType     m_ePortionType;

    bool                        m_bIsCollapsed;

    SwFmtFld * GetFldFmt(bool bInit = false);

    void init(const SwUnoCrsr* pPortionCursor);

protected:

    void SAL_CALL SetPropertyValues_Impl(
        const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames,
        const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aValues )
            throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > SAL_CALL GetPropertyValues_Impl(
        const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames )
            throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

    void        GetPropertyValues( const ::rtl::OUString *pPropertyNames,
                                    ::com::sun::star::uno::Any *pValues,
                                    sal_Int32 nLength );

    void GetPropertyValue( ::com::sun::star::uno::Any &rVal,
                const SfxItemPropertySimpleEntry& rEntry, SwUnoCrsr *pUnoCrsr, SfxItemSet *&pSet );

    ::com::sun::star::uno::Sequence< ::com::sun::star::beans::GetDirectPropertyTolerantResult > SAL_CALL GetPropertyValuesTolerant_Impl(
        const ::com::sun::star::uno::Sequence< rtl::OUString >& rPropertyNames,
        sal_Bool bDirectValuesOnly ) throw (::com::sun::star::uno::RuntimeException);

    virtual ~SwXTextPortion();

    //SwClient
   virtual void Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew);

public:
    SwXTextPortion(const SwUnoCrsr* pPortionCrsr, ::com::sun::star::uno::Reference< ::com::sun::star::text::XText > const& rParent, SwTextPortionType   eType   );
    SwXTextPortion(const SwUnoCrsr* pPortionCrsr, ::com::sun::star::uno::Reference< ::com::sun::star::text::XText > const& rParent, SwFrmFmt& rFmt );

    // for Ruby
    SwXTextPortion(const SwUnoCrsr* pPortionCrsr,
        SwTxtRuby const& rAttr,
        ::com::sun::star::uno::Reference< ::com::sun::star::text::XText >
            const& xParent,
        sal_Bool bIsEnd );

    //XTextRange
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XText >  SAL_CALL getText() throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > SAL_CALL  getStart() throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > SAL_CALL   getEnd() throw( ::com::sun::star::uno::RuntimeException );
    virtual rtl::OUString SAL_CALL  getString() throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL  setString(const rtl::OUString& aString) throw( ::com::sun::star::uno::RuntimeException );

    //XTolerantMultiPropertySet
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::SetPropertyTolerantFailed > SAL_CALL setPropertyValuesTolerant( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aValues ) throw (::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::GetPropertyTolerantResult > SAL_CALL getPropertyValuesTolerant( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames ) throw (::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::GetDirectPropertyTolerantResult > SAL_CALL getDirectPropertyValuesTolerant( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames ) throw (::com::sun::star::uno::RuntimeException);

    //XMultiPropertySet
    virtual void SAL_CALL setPropertyValues( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aValues ) throw(::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any > SAL_CALL getPropertyValues( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addPropertiesChangeListener( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removePropertiesChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL firePropertiesChangeEvent( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyNames, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertiesChangeListener >& xListener ) throw(::com::sun::star::uno::RuntimeException);

    //XPropertySet
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setPropertyValue( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Any& aValue ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::beans::PropertyVetoException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyValue( const ::rtl::OUString& PropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addPropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& xListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removePropertyChangeListener( const ::rtl::OUString& aPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& aListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL addVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL removeVetoableChangeListener( const ::rtl::OUString& PropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener >& aListener ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

    //XPropertyState
    virtual ::com::sun::star::beans::PropertyState SAL_CALL getPropertyState( const ::rtl::OUString& PropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyState > SAL_CALL getPropertyStates( const ::com::sun::star::uno::Sequence< ::rtl::OUString >& aPropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setPropertyToDefault( const ::rtl::OUString& PropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Any SAL_CALL getPropertyDefault( const ::rtl::OUString& aPropertyName ) throw(::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::WrappedTargetException, ::com::sun::star::uno::RuntimeException);

    //XTextContent
    virtual void SAL_CALL attach(const ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > & xTextRange) throw( ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > SAL_CALL getAnchor(  ) throw(::com::sun::star::uno::RuntimeException);

    //XComponent
    virtual void SAL_CALL dispose() throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL addEventListener(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & aListener) throw( ::com::sun::star::uno::RuntimeException );
    virtual void SAL_CALL removeEventListener(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > & aListener) throw( ::com::sun::star::uno::RuntimeException );

    //XUnoTunnel
    static const ::com::sun::star::uno::Sequence< sal_Int8 > & getUnoTunnelId();
    virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier ) throw(::com::sun::star::uno::RuntimeException);

    //XServiceInfo
    virtual rtl::OUString SAL_CALL getImplementationName() throw( ::com::sun::star::uno::RuntimeException );
    virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName) throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getSupportedServiceNames() throw( ::com::sun::star::uno::RuntimeException );

    //XContentEnumerationAccess
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::container::XEnumeration >  SAL_CALL createContentEnumeration(const rtl::OUString& aServiceName) throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL getAvailableServiceNames() throw( ::com::sun::star::uno::RuntimeException );

    void SetRefMark( ::com::sun::star::uno::Reference<
                        ::com::sun::star::text::XTextContent >  xMark)
    { m_xRefMark = xMark; }

    void SetTOXMark( ::com::sun::star::uno::Reference<
                        ::com::sun::star::text::XTextContent >  xMark)
    { m_xTOXMark = xMark; }

    void SetBookmark( ::com::sun::star::uno::Reference<
                        ::com::sun::star::text::XTextContent >  xMark)
    { m_xBookmark = xMark; }

    void SetFootnote( ::com::sun::star::uno::Reference<
                        ::com::sun::star::text::XFootnote > xNote)
    { m_xFootnote = xNote; }

    void SetTextField( ::com::sun::star::uno::Reference<
                        ::com::sun::star::text::XTextField> xField)
    { m_xTextField = xField; }

    void SetMeta( ::com::sun::star::uno::Reference<
                        ::com::sun::star::text::XTextContent >  xMeta)
    { m_xMeta = xMeta; }

    bool IsCollapsed() const { return m_bIsCollapsed; }
    void SetCollapsed(bool bSet)        { m_bIsCollapsed = bSet;}

    SwTextPortionType GetTextPortionType() const { return m_ePortionType; }

    SwUnoCrsr* GetCursor() const
    {return static_cast<SwUnoCrsr*>(const_cast<SwModify*>(GetRegisteredIn()));}
};

class SwXTextPortionEnumeration
    : public ::cppu::WeakImplHelper3
        < ::com::sun::star::container::XEnumeration
        , ::com::sun::star::lang::XServiceInfo
        , ::com::sun::star::lang::XUnoTunnel
        >
   , public SwClient
{
    TextRangeList_t m_Portions; // contains all portions, filled by ctor

    SwUnoCrsr*          GetCursor() const
    {return static_cast<SwUnoCrsr*>(const_cast<SwModify*>(GetRegisteredIn()));}

protected:
    virtual ~SwXTextPortionEnumeration();

public:
    SwXTextPortionEnumeration(SwPaM& rParaCrsr,
            ::com::sun::star::uno::Reference< ::com::sun::star::text::XText >
                const & xParent,
            const sal_Int32 nStart, const sal_Int32 nEnd );

    SwXTextPortionEnumeration(SwPaM& rParaCrsr,
        TextRangeList_t const & rPortions );


    static const ::com::sun::star::uno::Sequence< sal_Int8 > & getUnoTunnelId();

    //XUnoTunnel
    virtual sal_Int64 SAL_CALL getSomething(
            const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier )
        throw(::com::sun::star::uno::RuntimeException);

    //XEnumeration
    virtual sal_Bool SAL_CALL hasMoreElements()
        throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Any SAL_CALL nextElement()
        throw( ::com::sun::star::container::NoSuchElementException,
               ::com::sun::star::lang::WrappedTargetException,
               ::com::sun::star::uno::RuntimeException );

    //XServiceInfo
    virtual rtl::OUString SAL_CALL getImplementationName()
        throw( ::com::sun::star::uno::RuntimeException );
    virtual sal_Bool SAL_CALL supportsService(const rtl::OUString& ServiceName)
        throw( ::com::sun::star::uno::RuntimeException );
    virtual ::com::sun::star::uno::Sequence< rtl::OUString > SAL_CALL
        getSupportedServiceNames()
        throw( ::com::sun::star::uno::RuntimeException );
protected:
    //SwClient
    virtual void Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew);
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
