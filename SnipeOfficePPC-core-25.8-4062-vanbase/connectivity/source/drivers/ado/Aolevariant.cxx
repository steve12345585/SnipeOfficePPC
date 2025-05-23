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

#include "ado/Aolevariant.hxx"
#include "connectivity/dbconversion.hxx"
#include <com/sun/star/sdbc/SQLException.hpp>
#include <com/sun/star/util/Time.hpp>
#include <com/sun/star/util/Date.hpp>
#include <com/sun/star/util/DateTime.hpp>
#include "diagnose_ex.h"
#include "resource/sharedresources.hxx"
#include "resource/ado_res.hrc"
#include "com/sun/star/bridge/oleautomation/Date.hpp"
#include "com/sun/star/bridge/oleautomation/Currency.hpp"
#include "com/sun/star/bridge/oleautomation/SCode.hpp"
#include "com/sun/star/bridge/oleautomation/Decimal.hpp"

using namespace com::sun::star::beans;
using namespace com::sun::star::uno;
using namespace com::sun::star::bridge::oleautomation;
using namespace connectivity::ado;
using ::rtl::OUString;

OLEString::OLEString()
    :m_sStr(NULL)
{
}
OLEString::OLEString(const BSTR& _sBStr)
    :m_sStr(_sBStr)
{
}
OLEString::OLEString(const ::rtl::OUString& _sBStr)
{
    m_sStr = ::SysAllocString(reinterpret_cast<LPCOLESTR>(_sBStr.getStr()));
}
OLEString::~OLEString()
{
    if(m_sStr)
        ::SysFreeString(m_sStr);
}
OLEString& OLEString::operator=(const ::rtl::OUString& _rSrc)
{
    if(m_sStr)
        ::SysFreeString(m_sStr);
    m_sStr = ::SysAllocString(reinterpret_cast<LPCOLESTR>(_rSrc.getStr()));
    return *this;
}
OLEString& OLEString::operator=(const OLEString& _rSrc)
{
    if(this != &_rSrc)
    {
        if(m_sStr)
            ::SysFreeString(m_sStr);
        m_sStr = ::SysAllocString(_rSrc.m_sStr);
    }
    return *this;
}
OLEString& OLEString::operator=(const BSTR& _rSrc)
{
    if(m_sStr)
        ::SysFreeString(m_sStr);
    m_sStr = _rSrc;
    return *this;
}
OLEString::operator ::rtl::OUString() const
{
    return (m_sStr != NULL) ? ::rtl::OUString(reinterpret_cast<const sal_Unicode*>(LPCOLESTR(m_sStr)),::SysStringLen(m_sStr)) : ::rtl::OUString();
}
OLEString::operator BSTR() const
{
    return m_sStr;
}
BSTR* OLEString::operator &()
{
    return &m_sStr;
}
sal_Int32 OLEString::length() const
{
    return (m_sStr != NULL) ? ::SysStringLen(m_sStr) : 0;
}

OLEVariant::OLEVariant()
{
    VariantInit(this);
}
OLEVariant::OLEVariant(const VARIANT& varSrc)
{
    ::VariantInit(this);
    HRESULT eRet = ::VariantCopy(this, const_cast<VARIANT*>(&varSrc));
    OSL_ENSURE(eRet == S_OK,"Error while copying an ado variant!");
    OSL_UNUSED(eRet);
}
OLEVariant::OLEVariant(const OLEVariant& varSrc)
{
    ::VariantInit(this);
    HRESULT eRet = ::VariantCopy(this, const_cast<VARIANT*>(static_cast<const VARIANT*>(&varSrc)));
    OSL_ENSURE(eRet == S_OK,"Error while copying an ado variant!");
    OSL_UNUSED(eRet);
}

OLEVariant::OLEVariant(sal_Bool x)              {   VariantInit(this);  vt = VT_BOOL;   boolVal     = (x ? VARIANT_TRUE : VARIANT_FALSE);}
OLEVariant::OLEVariant(sal_Int8 n)              {   VariantInit(this);  vt = VT_I1;     bVal        = n;}
OLEVariant::OLEVariant(sal_Int16 n)             {   VariantInit(this);  vt = VT_I2;     intVal      = n;}
OLEVariant::OLEVariant(sal_Int32 n)             {   VariantInit(this);  vt = VT_I4;     lVal        = n;}
OLEVariant::OLEVariant(sal_Int64 x)             {   VariantInit(this);  vt = VT_I4;     lVal        = (LONG)x;}

OLEVariant::OLEVariant(const rtl::OUString& us)
{
    ::VariantInit(this);
    vt      = VT_BSTR;
    bstrVal = SysAllocString(reinterpret_cast<LPCOLESTR>(us.getStr()));
}
OLEVariant::~OLEVariant()
{
    HRESULT eRet = ::VariantClear(this);
    OSL_ENSURE(eRet == S_OK,"Error while clearing an ado variant!");
    OSL_UNUSED(eRet);
} // clears all the memory that was allocated before

OLEVariant::OLEVariant(const ::com::sun::star::util::Date& x )
{
    VariantInit(this);
    vt      = VT_DATE;
    dblVal  = ::dbtools::DBTypeConversion::toDouble(x,::com::sun::star::util::Date(30,12,1899));
}
OLEVariant::OLEVariant(const ::com::sun::star::util::Time& x )
{
    VariantInit(this);
    vt      = VT_DATE;
    dblVal  = ::dbtools::DBTypeConversion::toDouble(x);
}
OLEVariant::OLEVariant(const ::com::sun::star::util::DateTime& x )
{
    VariantInit(this);
    vt      = VT_DATE;
    dblVal  = ::dbtools::DBTypeConversion::toDouble(x,::com::sun::star::util::Date(30,12,1899));
}
OLEVariant::OLEVariant(const float &x)
{
    VariantInit(this);
    vt      = VT_R4;
    fltVal  = x;
}
OLEVariant::OLEVariant(const double &x)
{
    VariantInit(this);
    vt      = VT_R8;
    dblVal  = x;
}


OLEVariant::OLEVariant(IDispatch* pDispInterface)
{
    VariantInit(this);
    setIDispatch( pDispInterface );
}

OLEVariant::OLEVariant(const ::com::sun::star::uno::Sequence< sal_Int8 >& x)
{
    VariantInit(this);

    vt      = VT_ARRAY|VT_UI1;

    parray  = SafeArrayCreateVector(VT_UI1, 0, x.getLength());
    const sal_Int8* pBegin = x.getConstArray();
    const sal_Int8* pEnd = pBegin + x.getLength();

    for(sal_Int32 i=0;pBegin != pEnd;++i,++pBegin)
    {
        sal_Int32 nData = *pBegin;
        HRESULT rs = SafeArrayPutElement(parray,&i,&nData);
        OSL_ENSURE(S_OK == rs,"Error while copy byte data");
        OSL_UNUSED(rs);
    }
}
//
OLEVariant& OLEVariant::operator=(const OLEVariant& varSrc)
{
    HRESULT eRet = ::VariantCopy(this, const_cast<VARIANT*>(static_cast<const VARIANT*>(&varSrc)));
    OSL_ENSURE(eRet == S_OK,"Error while copying an ado variant!");
    OSL_UNUSED(eRet);
    return *this;
}
// Assign a const VARIANT& (::VariantCopy handles everything)
//
OLEVariant& OLEVariant::operator=(const tagVARIANT& varSrc)
{
    HRESULT eRet = ::VariantCopy(this, const_cast<VARIANT*>(&varSrc));
    OSL_ENSURE(eRet == S_OK,"Error while copying an ado variant!");
    OSL_UNUSED(eRet);

    return *this;
}

// Assign a const VARIANT* (::VariantCopy handles everything)
//
OLEVariant& OLEVariant::operator=(const VARIANT* pSrc)
{
    HRESULT eRet = ::VariantCopy(this, const_cast<VARIANT*>(pSrc));
    OSL_ENSURE(eRet == S_OK,"Error while copying an ado variant!");
    OSL_UNUSED(eRet);

    return *this;
}

void OLEVariant::setByte(sal_uInt8 n)
{
    HRESULT eRet = VariantClear(this);
    OSL_ENSURE(eRet == S_OK,"Error while clearing an ado variant!");
    OSL_UNUSED(eRet);
    vt = VT_UI1;
    bVal = n;
}
void OLEVariant::setInt16(sal_Int16 n)
{
    HRESULT eRet = VariantClear(this);
    OSL_ENSURE(eRet == S_OK,"Error while clearing an ado variant!");
    OSL_UNUSED(eRet);
    vt      = VT_I2;
    iVal    = n;
}
void OLEVariant::setInt32(sal_Int32 n)
{
    HRESULT eRet = VariantClear(this);
    OSL_ENSURE(eRet == S_OK,"Error while clearing an ado variant!");
    OSL_UNUSED(eRet);
    vt      = VT_I4;
    lVal    = n;
}
void OLEVariant::setFloat(float f)
{   HRESULT eRet = VariantClear(this);
    OSL_ENSURE(eRet == S_OK,"Error while clearing an ado variant!");
    OSL_UNUSED(eRet);
    vt      = VT_R4;
    fltVal  = f;
}
void OLEVariant::setDouble(double d)
{
    HRESULT eRet = VariantClear(this);
    OSL_ENSURE(eRet == S_OK,"Error while clearing an ado variant!");
    OSL_UNUSED(eRet);
    vt      = VT_R8;
    dblVal  = d;
}
void OLEVariant::setDate(DATE d)
{   HRESULT eRet = VariantClear(this);
    OSL_ENSURE(eRet == S_OK,"Error while clearing an ado variant!");
    OSL_UNUSED(eRet);
    vt      = VT_DATE;
    date    = d;
}
void OLEVariant::setChar(unsigned char a)
{
    HRESULT eRet = VariantClear(this);
    OSL_ENSURE(eRet == S_OK,"Error while clearing an ado variant!");
    OSL_UNUSED(eRet);
    vt = VT_UI1;
    bVal        = a;
}
void OLEVariant::setCurrency(double aCur)
{
    HRESULT eRet = VariantClear(this);
    OSL_ENSURE(eRet == S_OK,"Error while clearing an ado variant!");
    OSL_UNUSED(eRet);
    vt = VT_CY;
    set(aCur*10000);
}
void OLEVariant::setBool(sal_Bool b)
{
    HRESULT eRet = VariantClear(this);
    OSL_ENSURE(eRet == S_OK,"Error while clearing an ado variant!");
    OSL_UNUSED(eRet);
    vt = VT_BOOL;
    boolVal     = b ? VARIANT_TRUE : VARIANT_FALSE;
}
void OLEVariant::setString(const rtl::OUString& us)
{
    HRESULT eRet = VariantClear(this);
    OSL_ENSURE(eRet == S_OK,"Error while clearing an ado variant!");
    OSL_UNUSED(eRet);
    vt = VT_BSTR;
    bstrVal     = ::SysAllocString(reinterpret_cast<LPCOLESTR>(us.getStr()));
}
void OLEVariant::setNoArg()
{
    HRESULT eRet = VariantClear(this);
    OSL_ENSURE(eRet == S_OK,"Error while clearing an ado variant!");
    OSL_UNUSED(eRet);
    vt = VT_ERROR;
    scode       = DISP_E_PARAMNOTFOUND;
}

void OLEVariant::setNull()
{
    HRESULT eRet = VariantClear(this);
    OSL_ENSURE(eRet == S_OK,"Error while clearing an ado variant!");
    OSL_UNUSED(eRet);
    vt = VT_NULL;
}
void OLEVariant::setEmpty()
{
    HRESULT eRet = VariantClear(this);
    OSL_ENSURE(eRet == S_OK,"Error while clearing an ado variant!");
    OSL_UNUSED(eRet);
    vt = VT_EMPTY;
}

void OLEVariant::setUI1SAFEARRAYPtr(SAFEARRAY* pSafeAr)
{
    HRESULT eRet = VariantClear(this);
    OSL_ENSURE(eRet == S_OK,"Error while clearing an ado variant!");
    OSL_UNUSED(eRet);
    vt = VT_ARRAY|VT_UI1; parray = pSafeAr;
}

void OLEVariant::setArray(SAFEARRAY* pSafeArray, VARTYPE vtType)
{
    HRESULT eRet = VariantClear(this);
    OSL_ENSURE(eRet == S_OK,"Error while clearing an ado variant!");
    OSL_UNUSED(eRet);
    vt = (VARTYPE)(VT_ARRAY|vtType);
    parray = pSafeArray;
}

void OLEVariant::setIDispatch(IDispatch* pDispInterface)
{
    HRESULT eRet = VariantClear(this);
    OSL_ENSURE(eRet == S_OK,"Error while clearing an ado variant!");
    OSL_UNUSED(eRet);

    vt = VT_DISPATCH;
    pdispVal = pDispInterface;

    if ( pDispInterface )
        pDispInterface->AddRef();
}


sal_Bool OLEVariant::isNull() const  {  return (vt == VT_NULL);     }
sal_Bool OLEVariant::isEmpty() const {  return (vt == VT_EMPTY);    }

VARTYPE OLEVariant::getType() const { return vt; }

OLEVariant::operator ::com::sun::star::util::Date() const
{
    return isNull() ? ::com::sun::star::util::Date(30,12,1899) : ::dbtools::DBTypeConversion::toDate(getDate(),::com::sun::star::util::Date(30,12,1899));
}
OLEVariant::operator ::com::sun::star::util::Time() const
{
    return isNull() ? ::com::sun::star::util::Time() : ::dbtools::DBTypeConversion::toTime(getDate());
}
OLEVariant::operator ::com::sun::star::util::DateTime()const
{
    return isNull() ? ::com::sun::star::util::DateTime() : ::dbtools::DBTypeConversion::toDateTime(getDate(),::com::sun::star::util::Date(30,12,1899));
}

VARIANT_BOOL OLEVariant::VariantBool(sal_Bool bEinBoolean)
{
    return (bEinBoolean ? VARIANT_TRUE : VARIANT_FALSE);
}

void OLEVariant::CHS()
{
    cyVal.Lo  ^= (sal_uInt32)-1;
    cyVal.Hi ^= -1;
    cyVal.Lo++;
    if( !cyVal.Lo )
        cyVal.Hi++;
}

void OLEVariant::set(double n)
{
    if( n >= 0 )
    {
        cyVal.Hi = (sal_Int32)(n / (double)4294967296.0);
        cyVal.Lo  = (sal_uInt32)(n - ((double)cyVal.Hi * (double)4294967296.0));
    }
    else {
        cyVal.Hi = (sal_Int32)(-n / (double)4294967296.0);
        cyVal.Lo  = (sal_uInt32)(-n - ((double)cyVal.Hi * (double)4294967296.0));
        CHS();
    }
}

OLEVariant::operator rtl::OUString() const
{
    if (V_VT(this) == VT_BSTR)
        return reinterpret_cast<const sal_Unicode*>(LPCOLESTR(V_BSTR(this)));

    if(isNull())
        return ::rtl::OUString();

    OLEVariant varDest;

    varDest.ChangeType(VT_BSTR, this);

    return reinterpret_cast<const sal_Unicode*>(LPCOLESTR(V_BSTR(&varDest)));
}

// -----------------------------------------------------------------------------
void OLEVariant::ChangeType(VARTYPE vartype, const OLEVariant* pSrc)
{
    //
    // If pDest is NULL, convert type in place
    //
    if (pSrc == NULL)
        pSrc = this;

    if  (   ( this != pSrc )
        ||  ( vartype != V_VT( this ) )
        )
    {
        if ( FAILED( ::VariantChangeType(   static_cast< VARIANT* >( this ),
                                            const_cast< VARIANT* >( static_cast< const VARIANT* >( pSrc ) ),
                                            0,
                                            vartype ) ) )
        {
            ::connectivity::SharedResources aResources;
            const ::rtl::OUString sError( aResources.getResourceString(STR_TYPE_NOT_CONVERT));
            throw ::com::sun::star::sdbc::SQLException(
                sError,
                NULL,
                ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM( "S1000" )),
                1000,
                ::com::sun::star::uno::Any()
            );
        }
    }
}

// -----------------------------------------------------------------------------
OLEVariant::operator ::com::sun::star::uno::Sequence< sal_Int8 >() const
{
    ::com::sun::star::uno::Sequence< sal_Int8 > aRet;
    if(V_VT(this) == VT_BSTR)
    {
        OLEString sStr(V_BSTR(this));
        aRet = ::com::sun::star::uno::Sequence<sal_Int8>(reinterpret_cast<const sal_Int8*>((const wchar_t*)sStr),sizeof(sal_Unicode)*sStr.length());
    }
    else if(!isNull())
    {
        SAFEARRAY* pArray = getUI1SAFEARRAYPtr();

        if(pArray)
        {
            HRESULT hresult1,hresult2;
            long lBound,uBound;
            // Verify that the SafeArray is the proper shape.
            hresult1 = ::SafeArrayGetLBound(pArray, 1, &lBound);
            hresult2 = ::SafeArrayGetUBound(pArray, 1, &uBound);
            if ( SUCCEEDED(hresult1) && SUCCEEDED(hresult2) )
            {
                long nCount = uBound-lBound+1;
                aRet.realloc(nCount);
                sal_Int8* pData = aRet.getArray();
                for(long i=0; SUCCEEDED(hresult1) && lBound <= uBound ;++i,++lBound)
                {
                    sal_Int32 nData = 0;
                    hresult1 = ::SafeArrayGetElement(pArray,&lBound,&nData);
                    if ( SUCCEEDED(hresult1) )
                    {
                        *pData = static_cast<sal_Int8>(nData);
                        ++pData;
                    }
                }
            }
        }
    }

    return aRet;
}
// -----------------------------------------------------------------------------
::rtl::OUString OLEVariant::getString() const
{
    if(isNull())
        return ::rtl::OUString();
    else
        return *this;
}
// -----------------------------------------------------------------------------
sal_Bool OLEVariant::getBool() const
{
    if (V_VT(this) == VT_BOOL)
        return V_BOOL(this) == VARIANT_TRUE ? sal_True : sal_False;
    if(isNull())
        return sal_False;

    OLEVariant varDest;

    varDest.ChangeType(VT_BOOL, this);

    return V_BOOL(&varDest) == VARIANT_TRUE ? sal_True : sal_False;
}
// -----------------------------------------------------------------------------
IUnknown* OLEVariant::getIUnknown() const
{
    if (V_VT(this) == VT_UNKNOWN)
    {
        return V_UNKNOWN(this);
    }
    if(isNull())
        return NULL;

    OLEVariant varDest;

    varDest.ChangeType(VT_UNKNOWN, this);

    V_UNKNOWN(&varDest)->AddRef();
    return V_UNKNOWN(&varDest);
}
// -----------------------------------------------------------------------------
IDispatch* OLEVariant::getIDispatch() const
{
    if (V_VT(this) == VT_DISPATCH)
    {
        return V_DISPATCH(this);
    }

    if(isNull())
        return NULL;

    OLEVariant varDest;

    varDest.ChangeType(VT_DISPATCH, this);

    V_DISPATCH(&varDest)->AddRef();
    return V_DISPATCH(&varDest);
}
// -----------------------------------------------------------------------------
sal_uInt8 OLEVariant::getByte() const
{
    if (V_VT(this) == VT_UI1)
        return V_UI1(this);

    if(isNull())
        return sal_Int8(0);
    OLEVariant varDest;

    varDest.ChangeType(VT_UI1, this);

    return V_UI1(&varDest);
}
// -----------------------------------------------------------------------------
sal_Int16 OLEVariant::getInt16() const
{
    if (V_VT(this) == VT_I2)
        return V_I2(this);

    if(isNull())
        return sal_Int16(0);
    OLEVariant varDest;

    varDest.ChangeType(VT_I2, this);

    return V_I2(&varDest);
}
// -----------------------------------------------------------------------------
sal_Int8 OLEVariant::getInt8() const
{
    if (V_VT(this) == VT_I1)
        return V_I1(this);

    if(isNull())
        return sal_Int8(0);

    OLEVariant varDest;

    varDest.ChangeType(VT_I1, this);

    return V_I1(&varDest);
}
// -----------------------------------------------------------------------------
sal_Int32 OLEVariant::getInt32() const
{
    if (V_VT(this) == VT_I4)
        return V_I4(this);

    if(isNull())
        return sal_Int32(0);

    OLEVariant varDest;

    varDest.ChangeType(VT_I4, this);

    return V_I4(&varDest);
}
// -----------------------------------------------------------------------------
sal_uInt32 OLEVariant::getUInt32() const
{
    if (V_VT(this) == VT_UI4)
        return V_UI4(this);

    if(isNull())
        return sal_uInt32(0);

    OLEVariant varDest;

    varDest.ChangeType(VT_UI4, this);

    return V_UI4(&varDest);
}
// -----------------------------------------------------------------------------
float OLEVariant::getFloat() const
{
    if (V_VT(this) == VT_R4)
        return V_R4(this);

    if(isNull())
        return float(0);
    OLEVariant varDest;

    varDest.ChangeType(VT_R4, this);

    return V_R4(&varDest);
}
// -----------------------------------------------------------------------------
double OLEVariant::getDouble() const
{
    if (V_VT(this) == VT_R8)
        return V_R8(this);

    if(isNull())
        return double(0);
    OLEVariant varDest;

    varDest.ChangeType(VT_R8, this);

    return V_R8(&varDest);
}
// -----------------------------------------------------------------------------
double OLEVariant::getDate() const
{
    if (V_VT(this) == VT_DATE)
        return V_DATE(this);

    if(isNull())
        return double(0);
    OLEVariant varDest;

    varDest.ChangeType(VT_DATE, this);

    return V_DATE(&varDest);
}
// -----------------------------------------------------------------------------
CY OLEVariant::getCurrency() const
{
    if (V_VT(this) == VT_CY)
        return V_CY(this);

    if(isNull())
    {
        CY aVar;
        aVar.int64 = sal_Int64(0);
        return aVar;
    }
    OLEVariant varDest;

    varDest.ChangeType(VT_CY, this);

    return V_CY(&varDest);
}
// -----------------------------------------------------------------------------
SAFEARRAY* OLEVariant::getUI1SAFEARRAYPtr() const
{
    if (V_VT(this) == (VT_ARRAY|VT_UI1))
        return V_ARRAY(this);

    if(isNull())
        return (0);
    OLEVariant varDest;

    varDest.ChangeType((VT_ARRAY|VT_UI1), this);

    return V_ARRAY(&varDest);
}
// -----------------------------------------------------------------------------
::com::sun::star::uno::Any OLEVariant::makeAny() const
{
    ::com::sun::star::uno::Any aValue;
    switch (V_VT(this))
    {
        case VT_EMPTY:
        case VT_NULL:
            aValue.setValue(NULL, Type());
            break;
        case VT_I2:
            aValue.setValue( & iVal, getCppuType( (sal_Int16*)0));
            break;
        case VT_I4:
            aValue.setValue( & lVal, getCppuType( (sal_Int32*)0));
            break;
        case VT_R4:
            aValue.setValue( & fltVal, getCppuType( (float*)0));
            break;
        case VT_R8:
            aValue.setValue(& dblVal, getCppuType( (double*)0));
            break;
        case VT_CY:
         {
             Currency cy(cyVal.int64);
             aValue <<= cy;
            break;
         }
        case VT_DATE:
         {
             aValue <<= (::com::sun::star::util::Date)*this;
            break;
         }
        case VT_BSTR:
        {
            OUString b(reinterpret_cast<const sal_Unicode*>(bstrVal));
            aValue.setValue( &b, getCppuType( &b));
            break;
        }
        case VT_BOOL:
        {
            sal_Bool b= boolVal == VARIANT_TRUE;
            aValue.setValue( &b, getCppuType( &b));
            break;
        }
        case VT_I1:
            aValue.setValue( & cVal, getCppuType((sal_Int8*)0));
            break;
        case VT_UI1: // there is no unsigned char in UNO
            aValue.setValue( & bVal, getCppuType( (sal_Int8*)0));
            break;
        case VT_UI2:
            aValue.setValue( & uiVal, getCppuType( (sal_uInt16*)0));
            break;
        case VT_UI4:
            aValue.setValue( & ulVal, getCppuType( (sal_uInt32*)0));
            break;
        case VT_INT:
            aValue.setValue( & intVal, getCppuType( (sal_Int32*)0));
            break;
        case VT_UINT:
            aValue.setValue( & uintVal, getCppuType( (sal_uInt32*)0));
            break;
        case VT_VOID:
            aValue.setValue( NULL, Type());
            break;
         case VT_DECIMAL:
         {
             Decimal dec;
             dec.Scale = decVal.scale;
             dec.Sign = decVal.sign;
             dec.LowValue = decVal.Lo32;
             dec.MiddleValue = decVal.Mid32;
             dec.HighValue = decVal.Hi32;
             aValue <<= dec;
             break;
         }

        default:
            break;
    }
    return aValue;
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
