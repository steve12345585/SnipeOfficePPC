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


//------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------
#include <osl/diagnose.h>

#include "XTDataObject.hxx"

#include <windows.h>
#include <ole2.h>
#include <memory>

//------------------------------------------------------------------------
// namespace directives
//------------------------------------------------------------------------


//============================================================================
// OTWrapperDataObject
//============================================================================

//------------------------------------------------------------------------
// ctor
//------------------------------------------------------------------------

CXTDataObject::CXTDataObject( LONG nRefCntInitVal ) :
    m_nRefCnt( nRefCntInitVal )
{
}

//------------------------------------------------------------------------
// dtor
//------------------------------------------------------------------------

CXTDataObject::~CXTDataObject( )
{
}

//------------------------------------------------------------------------
// IUnknown->QueryInterface
//------------------------------------------------------------------------

STDMETHODIMP CXTDataObject::QueryInterface( REFIID iid, LPVOID* ppvObject )
{
    OSL_ASSERT( NULL != ppvObject );

    if ( NULL == ppvObject )
        return E_INVALIDARG;

    HRESULT hr = E_NOINTERFACE;

    *ppvObject = NULL;

    if ( ( __uuidof( IUnknown ) == iid ) || ( __uuidof( IDataObject ) == iid ) )
    {
        *ppvObject = static_cast< IUnknown* >( this );
        ( (LPUNKNOWN)*ppvObject )->AddRef( );
        hr = S_OK;
    }

    return hr;
}

//------------------------------------------------------------------------
// IUnknown->AddRef
//------------------------------------------------------------------------

STDMETHODIMP_(ULONG) CXTDataObject::AddRef( )
{
    return static_cast< ULONG >( InterlockedIncrement( &m_nRefCnt ) );
}

//------------------------------------------------------------------------
// IUnknown->Release
//------------------------------------------------------------------------

STDMETHODIMP_(ULONG) CXTDataObject::Release( )
{
    // we need a helper variable because it's
    // not allowed to access a member variable
    // after an object is destroyed
    ULONG nRefCnt = static_cast< ULONG >( InterlockedDecrement( &m_nRefCnt ) );

    if ( 0 == nRefCnt )
    {
        delete this;
    }

    return nRefCnt;
}

//------------------------------------------------------------------------
// IDataObject->GetData
// warning: 'goto' ahead (to easy error handling without using exceptions)
//------------------------------------------------------------------------

STDMETHODIMP CXTDataObject::GetData(LPFORMATETC pFormatetc, LPSTGMEDIUM pmedium )
{
    OSL_ASSERT( ( NULL != pFormatetc ) &&
                ( !IsBadReadPtr( (LPVOID)pFormatetc, sizeof( FORMATETC ) ) ) );
    OSL_ASSERT( ( NULL != pmedium ) &&
                ( !IsBadWritePtr( (LPVOID)pmedium, sizeof( STGMEDIUM ) ) ) );

    if ( ( NULL == pFormatetc ) || ( NULL == pmedium ) )
        return E_INVALIDARG;

    HRESULT hr = E_FAIL;

    if ( CF_TEXT == pFormatetc->cfFormat )
    {
        char     buff[] = "Hello World, How are you!";
        LPSTREAM lpStream;

        hr = CreateStreamOnHGlobal( NULL, FALSE, &lpStream );
        if ( SUCCEEDED( hr ) )
        {
            hr = lpStream->Write( buff, sizeof( buff ) * sizeof( char ), NULL );
            if ( SUCCEEDED( hr ) )
            {
                HGLOBAL hGlob;

                GetHGlobalFromStream( lpStream, &hGlob );

                pmedium->tymed          = TYMED_HGLOBAL;
                pmedium->hGlobal        = hGlob;
                pmedium->pUnkForRelease = NULL;
            }
            lpStream->Release( );
            hr = S_OK;
        }
        else
        {
            pmedium->tymed = TYMED_NULL;
        }
    }
    else if ( CF_UNICODETEXT == pFormatetc->cfFormat )
    {
        WCHAR     buff[] = L"Hello World, How are you!";
        LPSTREAM lpStream;

        hr = CreateStreamOnHGlobal( NULL, FALSE, &lpStream );
        if ( SUCCEEDED( hr ) )
        {
            hr = lpStream->Write( buff, sizeof( buff ) * sizeof( WCHAR ), NULL );
            if ( SUCCEEDED( hr ) )
            {
                HGLOBAL hGlob;

                GetHGlobalFromStream( lpStream, &hGlob );

                pmedium->tymed          = TYMED_HGLOBAL;
                pmedium->hGlobal        = hGlob;
                pmedium->pUnkForRelease = NULL;
            }
            lpStream->Release( );
            hr = S_OK;
        }
        else
        {
            pmedium->tymed = TYMED_NULL;
        }
    }

    return hr;
}

//------------------------------------------------------------------------
// IDataObject->EnumFormatEtc
//------------------------------------------------------------------------

STDMETHODIMP CXTDataObject::EnumFormatEtc( DWORD dwDirection, IEnumFORMATETC** ppenumFormatetc )
{
    if ( ( NULL == ppenumFormatetc ) || ( DATADIR_SET == dwDirection ) )
        return E_INVALIDARG;

    *ppenumFormatetc = NULL;

    HRESULT hr = E_FAIL;

    if ( DATADIR_GET == dwDirection )
    {
        *ppenumFormatetc = new CEnumFormatEtc( this );
        static_cast< LPUNKNOWN >( *ppenumFormatetc )->AddRef( );
        hr = S_OK;
    }

    return hr;
}

//------------------------------------------------------------------------
// IDataObject->QueryGetData
//------------------------------------------------------------------------

STDMETHODIMP CXTDataObject::QueryGetData( LPFORMATETC pFormatetc )
{
    return E_NOTIMPL;
}

//------------------------------------------------------------------------
// IDataObject->GetDataHere
//------------------------------------------------------------------------

STDMETHODIMP CXTDataObject::GetDataHere( LPFORMATETC, LPSTGMEDIUM )
{
    return E_NOTIMPL;
}

//------------------------------------------------------------------------
// IDataObject->GetCanonicalFormatEtc
//------------------------------------------------------------------------

STDMETHODIMP CXTDataObject::GetCanonicalFormatEtc( LPFORMATETC, LPFORMATETC )
{
    return E_NOTIMPL;
}

//------------------------------------------------------------------------
// IDataObject->SetData
//------------------------------------------------------------------------

STDMETHODIMP CXTDataObject::SetData( LPFORMATETC, LPSTGMEDIUM, BOOL )
{
    return E_NOTIMPL;
}

//------------------------------------------------------------------------
// IDataObject->DAdvise
//------------------------------------------------------------------------

STDMETHODIMP CXTDataObject::DAdvise( LPFORMATETC, DWORD, LPADVISESINK, DWORD * )
{
    return E_NOTIMPL;
}

//------------------------------------------------------------------------
// IDataObject->DUnadvise
//------------------------------------------------------------------------

STDMETHODIMP CXTDataObject::DUnadvise( DWORD )
{
    return E_NOTIMPL;
}

//------------------------------------------------------------------------
// IDataObject->EnumDAdvise
//------------------------------------------------------------------------

STDMETHODIMP CXTDataObject::EnumDAdvise( LPENUMSTATDATA * )
{
    return E_NOTIMPL;
}

//------------------------------------------------------------------------
// for our convenience
//------------------------------------------------------------------------

CXTDataObject::operator IDataObject*( )
{
    return static_cast< IDataObject* >( this );
}


//============================================================================
// CEnumFormatEtc
//============================================================================


//----------------------------------------------------------------------------
// ctor
//----------------------------------------------------------------------------

CEnumFormatEtc::CEnumFormatEtc( LPUNKNOWN pUnkDataObj ) :
    m_nRefCnt( 0 ),
    m_pUnkDataObj( pUnkDataObj ),
    m_nCurrentPos( 0 )
{
    m_cfFormats[0] = CF_UNICODETEXT;
    m_cfFormats[1] = CF_TEXT;
}

//----------------------------------------------------------------------------
// dtor
//----------------------------------------------------------------------------

CEnumFormatEtc::~CEnumFormatEtc( )
{
}

//----------------------------------------------------------------------------
// IUnknown->QueryInterface
//----------------------------------------------------------------------------

STDMETHODIMP CEnumFormatEtc::QueryInterface( REFIID iid, LPVOID* ppvObject )
{
    if ( NULL == ppvObject )
        return E_INVALIDARG;

    HRESULT hr = E_NOINTERFACE;

    *ppvObject = NULL;

    if ( ( __uuidof( IUnknown ) == iid ) || ( __uuidof( IEnumFORMATETC ) == iid ) )
    {
        *ppvObject = static_cast< IUnknown* >( this );
        static_cast< LPUNKNOWN >( *ppvObject )->AddRef( );
        hr = S_OK;
    }

    return hr;
}

//----------------------------------------------------------------------------
// IUnknown->AddRef
//----------------------------------------------------------------------------

STDMETHODIMP_(ULONG) CEnumFormatEtc::AddRef( )
{
    // keep the dataobject alive
    m_pUnkDataObj->AddRef( );
    return InterlockedIncrement( &m_nRefCnt );
}

//----------------------------------------------------------------------------
// IUnknown->Release
//----------------------------------------------------------------------------

STDMETHODIMP_(ULONG) CEnumFormatEtc::Release( )
{
    // release the outer dataobject
    m_pUnkDataObj->Release( );

    // we need a helper variable because it's
    // not allowed to access a member variable
    // after an object is destroyed
    ULONG nRefCnt = InterlockedDecrement( &m_nRefCnt );
    if ( 0 == nRefCnt )
        delete this;

    return nRefCnt;
}

//----------------------------------------------------------------------------
// IEnumFORMATETC->Next
//----------------------------------------------------------------------------

STDMETHODIMP CEnumFormatEtc::Next( ULONG celt, LPFORMATETC rgelt, ULONG* pceltFetched )
{
    OSL_ASSERT( ( ( celt > 0 ) && ( NULL != rgelt ) ) ||
                ( ( 0 == celt ) && ( NULL == rgelt ) ) );

    if ( ( 0 != celt ) && ( NULL == rgelt ) )
        return E_INVALIDARG;

    ULONG   ulFetched = 0;
    ULONG   ulToFetch = celt;
    HRESULT hr        = S_FALSE;

    while( ( m_nCurrentPos < sizeof( m_cfFormats ) ) && ( ulToFetch > 0 ) )
    {
        OSL_ASSERT( !IsBadWritePtr( (LPVOID)rgelt, sizeof( FORMATETC ) ) );

        rgelt->cfFormat = m_cfFormats[m_nCurrentPos];
        rgelt->ptd      = NULL;
        rgelt->dwAspect = DVASPECT_CONTENT;
        rgelt->lindex   = -1;
        rgelt->tymed    = TYMED_HGLOBAL;

        ++m_nCurrentPos;
        ++rgelt;
        --ulToFetch;
        ++ulFetched;
    }

    if ( ulFetched == celt )
        hr = S_OK;

    if ( NULL != pceltFetched )
    {
        OSL_ASSERT( !IsBadWritePtr( (LPVOID)pceltFetched, sizeof( ULONG ) ) );
        *pceltFetched = ulFetched;
    }

    return hr;
}

//----------------------------------------------------------------------------
// IEnumFORMATETC->Skip
//----------------------------------------------------------------------------

STDMETHODIMP CEnumFormatEtc::Skip( ULONG celt )
{
    HRESULT hr = S_FALSE;

    if ( ( m_nCurrentPos + celt ) < sizeof( m_cfFormats ) )
    {
        m_nCurrentPos += celt;
        hr = S_OK;
    }

    return hr;
}

//----------------------------------------------------------------------------
// IEnumFORMATETC->Reset
//----------------------------------------------------------------------------

STDMETHODIMP CEnumFormatEtc::Reset( )
{
    m_nCurrentPos = 0;
    return S_OK;
}

//----------------------------------------------------------------------------
// IEnumFORMATETC->Clone
//----------------------------------------------------------------------------

STDMETHODIMP CEnumFormatEtc::Clone( IEnumFORMATETC** ppenum )
{
    OSL_ASSERT( NULL != ppenum );

    if ( NULL == ppenum )
        return E_INVALIDARG;

    HRESULT hr = E_FAIL;

    *ppenum = NULL;

    CEnumFormatEtc* pCEnumFEtc = new CEnumFormatEtc( m_pUnkDataObj );
    if ( NULL != pCEnumFEtc )
    {
        pCEnumFEtc->m_nCurrentPos = m_nCurrentPos;
        *ppenum = static_cast< IEnumFORMATETC* >( pCEnumFEtc );
        static_cast< LPUNKNOWN >( *ppenum )->AddRef( );
        hr = NOERROR;
    }

    return hr;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
