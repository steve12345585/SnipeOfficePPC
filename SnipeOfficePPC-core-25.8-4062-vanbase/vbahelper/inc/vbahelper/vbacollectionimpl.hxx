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

#ifndef OOVBAAPI_VBA_COLLECTION_IMPL_HXX
#define OOVBAAPI_VBA_COLLECTION_IMPL_HXX

#include <ooo/vba/XCollection.hpp>
#include <com/sun/star/container/XEnumerationAccess.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/script/XDefaultMethod.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/container/XNamed.hpp>

#include <cppuhelper/implbase3.hxx>
#include <cppuhelper/implbase2.hxx>
#include <cppuhelper/implbase1.hxx>

#include "vbahelper/vbahelper.hxx"
#include "vbahelper/vbahelperinterface.hxx"

#include <vector>

// ============================================================================

typedef ::cppu::WeakImplHelper1< css::container::XEnumeration > EnumerationHelper_BASE;

// ============================================================================

/** A wrapper that holds a com.sun.star.container.XIndexAccess and provides a
    com.sun.star.container.XEnumeration.

    Can be used to provide an enumeration from an index container that contains
    completely constructed/initialized VBA implementation objects. CANNOT be
    used to provide an enumeration from an index container with other objects
    (e.g. UNO objects) where construction of the VBA objects is needed first.
 */
class VBAHELPER_DLLPUBLIC SimpleIndexAccessToEnumeration : public EnumerationHelper_BASE
{
public:
    explicit SimpleIndexAccessToEnumeration(
            const css::uno::Reference< css::container::XIndexAccess >& rxIndexAccess ) throw (css::uno::RuntimeException) :
        mxIndexAccess( rxIndexAccess ), mnIndex( 0 ) {}

    virtual sal_Bool SAL_CALL hasMoreElements() throw (css::uno::RuntimeException)
    {
        return mnIndex < mxIndexAccess->getCount();
    }

    virtual css::uno::Any SAL_CALL nextElement() throw (css::container::NoSuchElementException, css::lang::WrappedTargetException, css::uno::RuntimeException)
    {
        if( !hasMoreElements() )
            throw css::container::NoSuchElementException();
        return mxIndexAccess->getByIndex( mnIndex++ );
    }

private:
    css::uno::Reference< css::container::XIndexAccess > mxIndexAccess;
    sal_Int32 mnIndex;
};

// ============================================================================

/** A wrapper that holds a com.sun.star.container.XEnumeration or a
    com.sun.star.container.XIndexAccess and provides an enumeration of VBA objects.

    The method createCollectionObject() needs to be implemented by the derived
    class. This class can be used to convert an enumeration or an index container
    containing UNO objects to an enumeration providing the related VBA objects.
 */
class VBAHELPER_DLLPUBLIC SimpleEnumerationBase : public EnumerationHelper_BASE
{
public:
    explicit SimpleEnumerationBase(
            const css::uno::Reference< ov::XHelperInterface >& rxParent,
            const css::uno::Reference< css::uno::XComponentContext >& rxContext,
            const css::uno::Reference< css::container::XEnumeration >& rxEnumeration ) throw (css::uno::RuntimeException) :
        mxParent( rxParent ), mxContext( rxContext ), mxEnumeration( rxEnumeration ) {}

    explicit SimpleEnumerationBase(
            const css::uno::Reference< ov::XHelperInterface >& rxParent,
            const css::uno::Reference< css::uno::XComponentContext >& rxContext,
            const css::uno::Reference< css::container::XIndexAccess >& rxIndexAccess ) throw (css::uno::RuntimeException) :
        mxParent( rxParent ), mxContext( rxContext ), mxEnumeration( new SimpleIndexAccessToEnumeration( rxIndexAccess ) ) {}

    virtual sal_Bool SAL_CALL hasMoreElements() throw (css::uno::RuntimeException)
    {
        return mxEnumeration->hasMoreElements();
    }

    virtual css::uno::Any SAL_CALL nextElement() throw (css::container::NoSuchElementException, css::lang::WrappedTargetException, css::uno::RuntimeException)
    {
        return createCollectionObject( mxEnumeration->nextElement() );
    }

    /** Derived classes implement creation of a VBA implementation object from
        the passed container element. */
    virtual css::uno::Any createCollectionObject( const css::uno::Any& rSource ) = 0;

protected:
    css::uno::Reference< ov::XHelperInterface > mxParent;
    css::uno::Reference< css::uno::XComponentContext > mxContext;
    css::uno::Reference< css::container::XEnumeration > mxEnumeration;
};

// ============================================================================

// deprecated, use SimpleEnumerationBase instead!
class VBAHELPER_DLLPUBLIC EnumerationHelperImpl : public EnumerationHelper_BASE
{
protected:
    css::uno::WeakReference< ov::XHelperInterface > m_xParent;
    css::uno::Reference< css::uno::XComponentContext > m_xContext;
    css::uno::Reference< css::container::XEnumeration > m_xEnumeration;
public:

    EnumerationHelperImpl( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::container::XEnumeration >& xEnumeration ) throw ( css::uno::RuntimeException ) : m_xParent( xParent ), m_xContext( xContext ),  m_xEnumeration( xEnumeration ) { }
    virtual ::sal_Bool SAL_CALL hasMoreElements(  ) throw (css::uno::RuntimeException) { return m_xEnumeration->hasMoreElements(); }
};

// a wrapper class for a providing a XIndexAccess, XNameAccess, XEnumerationAccess impl based on providing a vector of interfaces
// only requirement is the object needs to implement XName



typedef ::cppu::WeakImplHelper3< css::container::XNameAccess, css::container::XIndexAccess, css::container::XEnumerationAccess > XNamedCollectionHelper_BASE;

template< typename Ifc1 >
class XNamedObjectCollectionHelper : public XNamedCollectionHelper_BASE
{
public:
typedef std::vector< css::uno::Reference< Ifc1 > >  XNamedVec;
private:

    class XNamedEnumerationHelper : public EnumerationHelper_BASE
    {
        XNamedVec mXNamedVec;
        typename XNamedVec::iterator mIt;
    public:
            XNamedEnumerationHelper( const XNamedVec& sMap ) : mXNamedVec( sMap ), mIt( mXNamedVec.begin() ) {}

            virtual ::sal_Bool SAL_CALL hasMoreElements(  ) throw (css::uno::RuntimeException)
            {
            return ( mIt != mXNamedVec.end() );
            }

            virtual css::uno::Any SAL_CALL nextElement(  ) throw (css::container::NoSuchElementException, css::lang::WrappedTargetException, css::uno::RuntimeException)
            {
                    if ( hasMoreElements() )
                return css::uno::makeAny( *mIt++ );
                    throw css::container::NoSuchElementException();
            }
    };

protected:
    XNamedVec mXNamedVec;
    typename XNamedVec::iterator cachePos;
public:
    XNamedObjectCollectionHelper( const XNamedVec& sMap ) : mXNamedVec( sMap ), cachePos(mXNamedVec.begin()) {}
    // XElementAccess
    virtual css::uno::Type SAL_CALL getElementType(  ) throw (css::uno::RuntimeException) { return  Ifc1::static_type(0); }
    virtual ::sal_Bool SAL_CALL hasElements(  ) throw (css::uno::RuntimeException) { return ( mXNamedVec.size() > 0 ); }
    // XNameAcess
    virtual css::uno::Any SAL_CALL getByName( const ::rtl::OUString& aName ) throw (css::container::NoSuchElementException, css::lang::WrappedTargetException, css::uno::RuntimeException)
    {
        if ( !hasByName(aName) )
            throw css::container::NoSuchElementException();
        return css::uno::makeAny( *cachePos );
    }
    virtual css::uno::Sequence< ::rtl::OUString > SAL_CALL getElementNames(  ) throw (css::uno::RuntimeException)
    {
        css::uno::Sequence< rtl::OUString > sNames( mXNamedVec.size() );
        rtl::OUString* pString = sNames.getArray();
        typename XNamedVec::iterator it = mXNamedVec.begin();
        typename XNamedVec::iterator it_end = mXNamedVec.end();

        for ( ; it != it_end; ++it, ++pString )
        {
            css::uno::Reference< css::container::XNamed > xName( *it, css::uno::UNO_QUERY_THROW );
            *pString = xName->getName();
        }
        return sNames;
    }
    virtual ::sal_Bool SAL_CALL hasByName( const ::rtl::OUString& aName ) throw (css::uno::RuntimeException)
    {
        cachePos = mXNamedVec.begin();
        typename XNamedVec::iterator it_end = mXNamedVec.end();
        for ( ; cachePos != it_end; ++cachePos )
        {
            css::uno::Reference< css::container::XNamed > xName( *cachePos, css::uno::UNO_QUERY_THROW );
            if ( aName.equals( xName->getName() ) )
                break;
        }
        return ( cachePos != it_end );
    }

    // XElementAccess
    virtual ::sal_Int32 SAL_CALL getCount(  ) throw (css::uno::RuntimeException) { return mXNamedVec.size(); }
    virtual css::uno::Any SAL_CALL getByIndex( ::sal_Int32 Index ) throw (css::lang::IndexOutOfBoundsException, css::lang::WrappedTargetException, css::uno::RuntimeException )
    {
        if ( Index < 0 || Index >= getCount() )
            throw css::lang::IndexOutOfBoundsException();

        return css::uno::makeAny( mXNamedVec[ Index ] );

    }
    // XEnumerationAccess
    virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration(  ) throw (css::uno::RuntimeException)
    {
        return new XNamedEnumerationHelper( mXNamedVec );
    }
};

// including a HelperInterface implementation
template< typename Ifc1 >
class ScVbaCollectionBase : public InheritedHelperInterfaceImpl< Ifc1 >
{
typedef InheritedHelperInterfaceImpl< Ifc1 > BaseColBase;
protected:
    css::uno::Reference< css::container::XIndexAccess > m_xIndexAccess;
    css::uno::Reference< css::container::XNameAccess > m_xNameAccess;
    sal_Bool mbIgnoreCase;

    virtual css::uno::Any getItemByStringIndex( const rtl::OUString& sIndex ) throw (css::uno::RuntimeException)
    {
        if ( !m_xNameAccess.is() )
            throw css::uno::RuntimeException( rtl::OUString( "ScVbaCollectionBase string index access not supported by this object" ), css::uno::Reference< css::uno::XInterface >() );

        if( mbIgnoreCase )
        {
            css::uno::Sequence< rtl::OUString > sElementNames = m_xNameAccess->getElementNames();
            for( sal_Int32 i = 0; i < sElementNames.getLength(); i++ )
            {
                rtl::OUString aName = sElementNames[i];
                if( aName.equalsIgnoreAsciiCase( sIndex ) )
                {
                    return createCollectionObject( m_xNameAccess->getByName( aName ) );
                }
            }
        }
        return createCollectionObject( m_xNameAccess->getByName( sIndex ) );
    }

    virtual css::uno::Any getItemByIntIndex( const sal_Int32 nIndex ) throw (css::uno::RuntimeException)
    {
        if ( !m_xIndexAccess.is() )
            throw css::uno::RuntimeException( rtl::OUString( "ScVbaCollectionBase numeric index access not supported by this object" ), css::uno::Reference< css::uno::XInterface >() );
        if ( nIndex <= 0 )
        {
            throw  css::lang::IndexOutOfBoundsException(
                ::rtl::OUString( "index is 0 or negative"  ),
                css::uno::Reference< css::uno::XInterface >() );
        }
        // need to adjust for vba index ( for which first element is 1 )
        return createCollectionObject( m_xIndexAccess->getByIndex( nIndex - 1 ) );
    }

    virtual void UpdateCollectionIndex( const css::uno::Reference< css::container::XIndexAccess >& xIndexAccess )
    {
        css::uno::Reference< css::container::XNameAccess > xNameAccess( xIndexAccess, css::uno::UNO_QUERY_THROW );
        m_xIndexAccess = xIndexAccess;
        m_xNameAccess = xNameAccess;
    }

public:
    ScVbaCollectionBase( const css::uno::Reference< ov::XHelperInterface >& xParent,   const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::container::XIndexAccess >& xIndexAccess, sal_Bool bIgnoreCase = sal_False ) : BaseColBase( xParent, xContext ), m_xIndexAccess( xIndexAccess ), mbIgnoreCase( bIgnoreCase ) { m_xNameAccess.set(m_xIndexAccess, css::uno::UNO_QUERY); }
    //XCollection
    virtual ::sal_Int32 SAL_CALL getCount() throw (css::uno::RuntimeException)
    {
        return m_xIndexAccess->getCount();
    }

    virtual css::uno::Any SAL_CALL Item( const css::uno::Any& Index1, const css::uno::Any& /*not processed in this base class*/ ) throw (css::uno::RuntimeException)
    {
        if ( Index1.getValueTypeClass() != css::uno::TypeClass_STRING )
        {
            sal_Int32 nIndex = 0;

            if ( ( Index1 >>= nIndex ) != sal_True )
            {
                rtl::OUString message;
                message = rtl::OUString( "Couldn't convert index to Int32");
                throw  css::lang::IndexOutOfBoundsException( message,
                    css::uno::Reference< css::uno::XInterface >() );
            }
            return  getItemByIntIndex( nIndex );
        }
        rtl::OUString aStringSheet;

        Index1 >>= aStringSheet;
        return getItemByStringIndex( aStringSheet );
    }
    // XDefaultMethod
    ::rtl::OUString SAL_CALL getDefaultMethodName(  ) throw (css::uno::RuntimeException)
    {
        const static rtl::OUString sName( "Item" );
        return sName;
    }
    // XEnumerationAccess
    virtual css::uno::Reference< css::container::XEnumeration > SAL_CALL createEnumeration() throw (css::uno::RuntimeException) = 0;

    // XElementAccess
    virtual css::uno::Type SAL_CALL getElementType() throw (css::uno::RuntimeException) = 0;
    // XElementAccess
    virtual ::sal_Bool SAL_CALL hasElements() throw (css::uno::RuntimeException)
    {
        return ( m_xIndexAccess->getCount() > 0 );
    }
    virtual css::uno::Any createCollectionObject( const css::uno::Any& aSource ) = 0;

};

typedef ::cppu::WeakImplHelper1<ov::XCollection> XCollection_InterfacesBASE;

typedef ScVbaCollectionBase< XCollection_InterfacesBASE > CollImplBase1;
// compatible with the old collections ( pre XHelperInterface base class ) ( some internal objects still use this )
class VBAHELPER_DLLPUBLIC ScVbaCollectionBaseImpl : public CollImplBase1
{
public:
    ScVbaCollectionBaseImpl( const css::uno::Reference< ov::XHelperInterface > xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext, const css::uno::Reference< css::container::XIndexAccess >& xIndexAccess ) throw( css::uno::RuntimeException ) : CollImplBase1( xParent, xContext, xIndexAccess){}

};

template <typename Ifc> // where Ifc must implement XCollectionTest
class CollTestImplHelper :  public ScVbaCollectionBase< ::cppu::WeakImplHelper1< Ifc > >
{
typedef ScVbaCollectionBase< ::cppu::WeakImplHelper1< Ifc >  > ImplBase1;

public:
    CollTestImplHelper( const css::uno::Reference< ov::XHelperInterface >& xParent, const css::uno::Reference< css::uno::XComponentContext >& xContext,  const css::uno::Reference< css::container::XIndexAccess >& xIndexAccess, sal_Bool bIgnoreCase = sal_False ) throw( css::uno::RuntimeException ) : ImplBase1( xParent, xContext, xIndexAccess, bIgnoreCase ) {}
};


#endif //SC_VBA_COLLECTION_IMPL_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
