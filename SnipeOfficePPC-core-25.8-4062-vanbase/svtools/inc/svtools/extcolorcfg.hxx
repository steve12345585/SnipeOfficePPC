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
#ifndef INCLUDED_SVTOOLS_EXTCOLORCFG_HXX
#define INCLUDED_SVTOOLS_EXTCOLORCFG_HXX

#include "svtools/svtdllapi.h"
#include <rtl/ustring.hxx>
#include <tools/color.hxx>
#include <com/sun/star/uno/Sequence.h>
#include <svl/brdcst.hxx>
#include <svl/lstner.hxx>

//-----------------------------------------------------------------------------
namespace svtools{

class ExtendedColorConfig_Impl;
class ExtendedColorConfigValue
{
    ::rtl::OUString m_sName;
    ::rtl::OUString m_sDisplayName;
    sal_Int32       m_nColor;
    sal_Int32       m_nDefaultColor;
public:
    ExtendedColorConfigValue() : m_nColor(0),m_nDefaultColor(0){}
    ExtendedColorConfigValue(const ::rtl::OUString& _sName
                            ,const ::rtl::OUString& _sDisplayName
                            ,sal_Int32      _nColor
                            ,sal_Int32      _nDefaultColor)
    : m_sName(_sName)
    ,m_sDisplayName(_sDisplayName)
    ,m_nColor(_nColor)
    ,m_nDefaultColor(_nDefaultColor)
    {}

    inline ::rtl::OUString getName()         const { return m_sName; }
    inline ::rtl::OUString getDisplayName()  const { return m_sDisplayName; }
    inline sal_Int32       getColor()        const { return m_nColor; }
    inline sal_Int32       getDefaultColor() const { return m_nDefaultColor; }

    inline void setColor(sal_Int32 _nColor) { m_nColor = _nColor; }

    sal_Bool operator !=(const ExtendedColorConfigValue& rCmp) const
        { return m_nColor != rCmp.m_nColor;}
};

class SVT_DLLPUBLIC ExtendedColorConfig : public SfxBroadcaster, public SfxListener
{
    friend class ExtendedColorConfig_Impl;
private:
    static ExtendedColorConfig_Impl* m_pImpl;
public:
    ExtendedColorConfig();
    ~ExtendedColorConfig();

    virtual void        Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

    // get the configured value
    ExtendedColorConfigValue        GetColorValue(const ::rtl::OUString& _sComponentName,const ::rtl::OUString& _sName)const;
    sal_Int32                       GetComponentCount() const;
    ::rtl::OUString                 GetComponentName(sal_uInt32 _nPos) const;
    ::rtl::OUString                 GetComponentDisplayName(const ::rtl::OUString& _sComponentName) const;
    sal_Int32                       GetComponentColorCount(const ::rtl::OUString& _sName) const;
    ExtendedColorConfigValue        GetComponentColorConfigValue(const ::rtl::OUString& _sComponentName,sal_uInt32 _nPos) const;
};

class SVT_DLLPUBLIC EditableExtendedColorConfig
{
    ExtendedColorConfig_Impl*   m_pImpl;
    sal_Bool            m_bModified;
public:
    EditableExtendedColorConfig();
    ~EditableExtendedColorConfig();

    void                                                DeleteScheme(const ::rtl::OUString& rScheme );
    void                                                AddScheme(const ::rtl::OUString& rScheme );
    sal_Bool                                            LoadScheme(const ::rtl::OUString& rScheme );
    void                        SetCurrentSchemeName(const ::rtl::OUString& rScheme);

    sal_Int32                   GetComponentCount() const;
    ::rtl::OUString             GetComponentName(sal_uInt32 _nPos) const;
    sal_Int32                   GetComponentColorCount(const ::rtl::OUString& _sName) const;
    ExtendedColorConfigValue    GetComponentColorConfigValue(const ::rtl::OUString& _sName,sal_uInt32 _nPos) const;
    void                        SetColorValue(const ::rtl::OUString& _sComponentName, const ExtendedColorConfigValue& rValue);
    void                        SetModified();
    void                        ClearModified(){m_bModified = sal_False;}
    sal_Bool                    IsModified()const{return m_bModified;}
    void                        Commit();

    void                        DisableBroadcast();
    void                        EnableBroadcast();
};
}//namespace svtools
#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
