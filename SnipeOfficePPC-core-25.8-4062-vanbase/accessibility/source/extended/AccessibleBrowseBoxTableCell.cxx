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


#include "accessibility/extended/AccessibleBrowseBoxTableCell.hxx"
#include <svtools/accessibletableprovider.hxx>
#include "accessibility/extended/AccessibleBrowseBox.hxx"
#include <tools/gen.hxx>
#include <toolkit/helper/vclunohelper.hxx>
#include <com/sun/star/accessibility/AccessibleEventId.hpp>

namespace accessibility
{
    namespace
    {
        static void checkIndex_Impl( sal_Int32 _nIndex, const OUString& _sText ) throw (::com::sun::star::lang::IndexOutOfBoundsException)
        {
            if ( _nIndex >= _sText.getLength() )
                throw ::com::sun::star::lang::IndexOutOfBoundsException();
        }

        static sal_Int32 getIndex_Impl( sal_Int32 _nRow, sal_uInt16 _nColumn, sal_uInt16 _nColumnCount )
        {
            return _nRow * _nColumnCount + _nColumn;
        }
    }
    using namespace ::com::sun::star::lang;
    using namespace utl;
    using namespace comphelper;
    using ::accessibility::AccessibleBrowseBox;
    using namespace ::com::sun::star::uno;
    using ::com::sun::star::accessibility::XAccessible;
    using namespace ::com::sun::star::accessibility;
    using namespace ::svt;


    // implementation of a table cell
    OUString AccessibleBrowseBoxTableCell::implGetText()
    {
        ensureIsAlive();
        return mpBrowseBox->GetAccessibleCellText( getRowPos(), static_cast< sal_uInt16 >( getColumnPos() ) );
    }

    ::com::sun::star::lang::Locale AccessibleBrowseBoxTableCell::implGetLocale()
    {
        ensureIsAlive();
        return mpBrowseBox->GetAccessible()->getAccessibleContext()->getLocale();
    }

    void AccessibleBrowseBoxTableCell::implGetSelection( sal_Int32& nStartIndex, sal_Int32& nEndIndex )
    {
        nStartIndex = 0;
        nEndIndex = 0;
    }

    AccessibleBrowseBoxTableCell::AccessibleBrowseBoxTableCell(const Reference<XAccessible >& _rxParent,
                                IAccessibleTableProvider& _rBrowseBox,
                                const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow >& _xFocusWindow,
                                sal_Int32 _nRowPos,
                                sal_uInt16 _nColPos,
                                sal_Int32 _nOffset )
        :AccessibleBrowseBoxCell( _rxParent, _rBrowseBox, _xFocusWindow, _nRowPos, _nColPos )
    {
        m_nOffset = ( OFFSET_DEFAULT == _nOffset ) ? (sal_Int32)BBINDEX_FIRSTCONTROL : _nOffset;
        sal_Int32 nIndex = getIndex_Impl( _nRowPos, _nColPos, _rBrowseBox.GetColumnCount() );
       setAccessibleName( _rBrowseBox.GetAccessibleObjectName( BBTYPE_TABLECELL, nIndex ) );
       setAccessibleDescription( _rBrowseBox.GetAccessibleObjectDescription( BBTYPE_TABLECELL, nIndex ) );
        // Need to register as event listener
        Reference< XComponent > xComponent(_rxParent, UNO_QUERY);
        if( xComponent.is() )
            xComponent->addEventListener(static_cast< XEventListener *> (this));
    }

    void AccessibleBrowseBoxTableCell::nameChanged( const OUString& rNewName, const OUString& rOldName )
    {
        implSetName( rNewName );
        Any aOldValue, aNewValue;
        aOldValue <<= rOldName;
        aNewValue <<= rNewName;
        commitEvent( AccessibleEventId::NAME_CHANGED, aOldValue, aNewValue );
    }

    // XInterface -------------------------------------------------------------

    /** Queries for a new interface. */
    ::com::sun::star::uno::Any SAL_CALL AccessibleBrowseBoxTableCell::queryInterface(
            const ::com::sun::star::uno::Type& rType )
        throw ( ::com::sun::star::uno::RuntimeException )
    {
        Any aRet = AccessibleBrowseBoxCell::queryInterface(rType);
        if ( !aRet.hasValue() )
            aRet = AccessibleTextHelper_BASE::queryInterface(rType);
        return aRet;
    }

    /** Aquires the object (calls acquire() on base class). */
    void SAL_CALL AccessibleBrowseBoxTableCell::acquire() throw ()
    {
        AccessibleBrowseBoxCell::acquire();
    }

    /** Releases the object (calls release() on base class). */
    void SAL_CALL AccessibleBrowseBoxTableCell::release() throw ()
    {
        AccessibleBrowseBoxCell::release();
    }

    ::com::sun::star::awt::Rectangle SAL_CALL AccessibleBrowseBoxTableCell::getCharacterBounds( sal_Int32 nIndex ) throw (IndexOutOfBoundsException, RuntimeException)
    {
        SolarMutexGuard aSolarGuard;
        ::osl::MutexGuard aGuard( getOslMutex() );

        ensureIsAlive();
        if ( !implIsValidIndex( nIndex, implGetText().getLength() ) )
            throw IndexOutOfBoundsException();

        ::com::sun::star::awt::Rectangle aRect;

        if ( mpBrowseBox )
        {
            aRect = AWTRectangle( mpBrowseBox->GetFieldCharacterBounds( getRowPos(), getColumnPos(), nIndex ) );
        }

        return aRect;
    }

    sal_Int32 SAL_CALL AccessibleBrowseBoxTableCell::getIndexAtPoint( const ::com::sun::star::awt::Point& _aPoint ) throw (RuntimeException)
    {
        //! TODO CTL bidi
        // OSL_FAIL("Need to be done by base class!");
        SolarMutexGuard aSolarGuard;
        ::osl::MutexGuard aGuard( getOslMutex() );
        ensureIsAlive();

        return mpBrowseBox->GetFieldIndexAtPoint( getRowPos(), getColumnPos(), VCLPoint( _aPoint ) );
    }

    /** @return
            The name of this class.
    */
    OUString SAL_CALL AccessibleBrowseBoxTableCell::getImplementationName()
        throw ( ::com::sun::star::uno::RuntimeException )
    {
        return OUString( "com.sun.star.comp.svtools.AccessibleBrowseBoxTableCell" );
    }

    /** @return  The count of visible children. */
    sal_Int32 SAL_CALL AccessibleBrowseBoxTableCell::getAccessibleChildCount()
        throw ( ::com::sun::star::uno::RuntimeException )
    {
        return 0;
    }

    /** @return  The XAccessible interface of the specified child. */
    ::com::sun::star::uno::Reference<
        ::com::sun::star::accessibility::XAccessible > SAL_CALL
        AccessibleBrowseBoxTableCell::getAccessibleChild( sal_Int32 )
            throw ( ::com::sun::star::lang::IndexOutOfBoundsException,
                    ::com::sun::star::uno::RuntimeException )
    {
        throw ::com::sun::star::lang::IndexOutOfBoundsException();
    }

    /** Creates a new AccessibleStateSetHelper and fills it with states of the
        current object.
        @return
            A filled AccessibleStateSetHelper.
    */
    ::utl::AccessibleStateSetHelper* AccessibleBrowseBoxTableCell::implCreateStateSetHelper()
    {
        SolarMutexGuard aSolarGuard;
        ::osl::MutexGuard aGuard( getOslMutex() );

        ::utl::AccessibleStateSetHelper* pStateSetHelper = new ::utl::AccessibleStateSetHelper;

        if( isAlive() )
        {
            // SHOWING done with mxParent
            if( implIsShowing() )
                pStateSetHelper->AddState( AccessibleStateType::SHOWING );

            mpBrowseBox->FillAccessibleStateSetForCell( *pStateSetHelper, getRowPos(), static_cast< sal_uInt16 >( getColumnPos() ) );
        }
        else
            pStateSetHelper->AddState( AccessibleStateType::DEFUNC );

        return pStateSetHelper;
    }


    // XAccessible ------------------------------------------------------------

    /** @return  The XAccessibleContext interface of this object. */
    Reference< XAccessibleContext > SAL_CALL AccessibleBrowseBoxTableCell::getAccessibleContext() throw ( RuntimeException )
    {
        ensureIsAlive();
        return this;
    }

    // XAccessibleContext -----------------------------------------------------

    sal_Int32 SAL_CALL AccessibleBrowseBoxTableCell::getAccessibleIndexInParent()
            throw ( ::com::sun::star::uno::RuntimeException )
    {
        SolarMutexGuard aSolarGuard;
        ::osl::MutexGuard aGuard( getOslMutex() );
        ensureIsAlive();

        return /*BBINDEX_FIRSTCONTROL*/ m_nOffset + ( getRowPos() * mpBrowseBox->GetColumnCount() ) + getColumnPos();
    }

    sal_Int32 SAL_CALL AccessibleBrowseBoxTableCell::getCaretPosition(  ) throw (::com::sun::star::uno::RuntimeException)
    {
        return -1;
    }
    sal_Bool SAL_CALL AccessibleBrowseBoxTableCell::setCaretPosition ( sal_Int32 nIndex ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException)
    {
        SolarMutexGuard aSolarGuard;
        ::osl::MutexGuard aGuard( getOslMutex() );

        if ( !implIsValidRange( nIndex, nIndex, implGetText().getLength() ) )
            throw IndexOutOfBoundsException();

        return sal_False;
    }
    sal_Unicode SAL_CALL AccessibleBrowseBoxTableCell::getCharacter( sal_Int32 nIndex ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException)
    {
        SolarMutexGuard aSolarGuard;
        ::osl::MutexGuard aGuard( getOslMutex() );
        return OCommonAccessibleText::getCharacter( nIndex );
    }
    ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue > SAL_CALL AccessibleBrowseBoxTableCell::getCharacterAttributes( sal_Int32 nIndex, const ::com::sun::star::uno::Sequence< OUString >& ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException)
    {
        SolarMutexGuard aSolarGuard;
        ::osl::MutexGuard aGuard( getOslMutex() );

        OUString sText( implGetText() );

        if ( !implIsValidIndex( nIndex, sText.getLength() ) )
            throw IndexOutOfBoundsException();

        return ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >();
    }
    sal_Int32 SAL_CALL AccessibleBrowseBoxTableCell::getCharacterCount(  ) throw (::com::sun::star::uno::RuntimeException)
    {
        SolarMutexGuard aSolarGuard;
        ::osl::MutexGuard aGuard( getOslMutex() );
        return OCommonAccessibleText::getCharacterCount(  );
    }

    OUString SAL_CALL AccessibleBrowseBoxTableCell::getSelectedText(  ) throw (::com::sun::star::uno::RuntimeException)
    {
        SolarMutexGuard aSolarGuard;
        ::osl::MutexGuard aGuard( getOslMutex() );
        return OCommonAccessibleText::getSelectedText(  );
    }
    sal_Int32 SAL_CALL AccessibleBrowseBoxTableCell::getSelectionStart(  ) throw (::com::sun::star::uno::RuntimeException)
    {
        SolarMutexGuard aSolarGuard;
        ::osl::MutexGuard aGuard( getOslMutex() );
        return OCommonAccessibleText::getSelectionStart(  );
    }
    sal_Int32 SAL_CALL AccessibleBrowseBoxTableCell::getSelectionEnd(  ) throw (::com::sun::star::uno::RuntimeException)
    {
        SolarMutexGuard aSolarGuard;
        ::osl::MutexGuard aGuard( getOslMutex() );
        return OCommonAccessibleText::getSelectionEnd(  );
    }
    sal_Bool SAL_CALL AccessibleBrowseBoxTableCell::setSelection( sal_Int32 nStartIndex, sal_Int32 nEndIndex ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException)
    {
        SolarMutexGuard aSolarGuard;
        ::osl::MutexGuard aGuard( getOslMutex() );
        if ( !implIsValidRange( nStartIndex, nEndIndex, implGetText().getLength() ) )
            throw IndexOutOfBoundsException();

        return sal_False;
    }
    OUString SAL_CALL AccessibleBrowseBoxTableCell::getText(  ) throw (::com::sun::star::uno::RuntimeException)
    {
        SolarMutexGuard aSolarGuard;
        ::osl::MutexGuard aGuard( getOslMutex() );
        return OCommonAccessibleText::getText(  );
    }
    OUString SAL_CALL AccessibleBrowseBoxTableCell::getTextRange( sal_Int32 nStartIndex, sal_Int32 nEndIndex ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException)
    {
        SolarMutexGuard aSolarGuard;
        ::osl::MutexGuard aGuard( getOslMutex() );
        return OCommonAccessibleText::getTextRange( nStartIndex, nEndIndex );
    }
    ::com::sun::star::accessibility::TextSegment SAL_CALL AccessibleBrowseBoxTableCell::getTextAtIndex( sal_Int32 nIndex, sal_Int16 aTextType ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException)
    {
        SolarMutexGuard aSolarGuard;
        ::osl::MutexGuard aGuard( getOslMutex() );
        return OCommonAccessibleText::getTextAtIndex( nIndex ,aTextType);
    }
    ::com::sun::star::accessibility::TextSegment SAL_CALL AccessibleBrowseBoxTableCell::getTextBeforeIndex( sal_Int32 nIndex, sal_Int16 aTextType ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException)
    {
        SolarMutexGuard aSolarGuard;
        ::osl::MutexGuard aGuard( getOslMutex() );
        return OCommonAccessibleText::getTextBeforeIndex( nIndex ,aTextType);
    }
    ::com::sun::star::accessibility::TextSegment SAL_CALL AccessibleBrowseBoxTableCell::getTextBehindIndex( sal_Int32 nIndex, sal_Int16 aTextType ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException)
    {
        SolarMutexGuard aSolarGuard;
        ::osl::MutexGuard aGuard( getOslMutex() );
        return OCommonAccessibleText::getTextBehindIndex( nIndex ,aTextType);
    }
    sal_Bool SAL_CALL AccessibleBrowseBoxTableCell::copyText( sal_Int32 nStartIndex, sal_Int32 nEndIndex ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException)
    {
        SolarMutexGuard aSolarGuard;
        ::osl::MutexGuard aGuard( getOslMutex() );
        OUString sText = implGetText();
        checkIndex_Impl( nStartIndex, sText );
        checkIndex_Impl( nEndIndex, sText );

        //!!! don't know how to put a string into the clipboard
        return sal_False;
    }
    void AccessibleBrowseBoxTableCell::disposing( const EventObject& _rSource ) throw (RuntimeException)
    {
            if ( _rSource.Source == mxParent )
            {
                    dispose();
            }
    }

}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
