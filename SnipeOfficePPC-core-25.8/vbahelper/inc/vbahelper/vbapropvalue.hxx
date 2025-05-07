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
#ifndef SC_VBA_PROPVALULE_HXX
#define SC_VBA_PROPVALULE_HXX
#include <ooo/vba/XPropValue.hpp>
#include <cppuhelper/implbase1.hxx>

#include <vbahelper/vbahelper.hxx>

typedef ::cppu::WeakImplHelper1< ov::XPropValue > PropValueImpl_BASE;

class VBAHELPER_DLLPUBLIC PropListener
{
public:
    virtual void setValueEvent( const css::uno::Any& value ) = 0;
    virtual css::uno::Any getValueEvent() = 0;

protected:
    ~PropListener() {}
};


class VBAHELPER_DLLPUBLIC ScVbaPropValue : public PropValueImpl_BASE
{
    PropListener* m_pListener;
public:
    ScVbaPropValue( PropListener* pListener );

    // Attributes
    virtual css::uno::Any SAL_CALL getValue() throw (css::uno::RuntimeException);
    virtual void SAL_CALL setValue( const css::uno::Any& _value ) throw (css::uno::RuntimeException);

    rtl::OUString SAL_CALL getDefaultPropertyName() throw (css::uno::RuntimeException) { return ::rtl::OUString("Value"); }

};
#endif //SC_VBA_PROPVALULE_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
