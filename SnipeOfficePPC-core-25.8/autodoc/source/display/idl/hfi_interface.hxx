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

#ifndef ADC_DISPLAY_HFI_INTERFACE_HXX
#define ADC_DISPLAY_HFI_INTERFACE_HXX


// USED SERVICES
    // BASE CLASSES
#include "hi_factory.hxx"
    // COMPONENTS
    // PARAMETERS
#include <ary/idl/i_types4idl.hxx>
#include <ary/doc/d_oldidldocu.hxx>


class HF_NaviSubRow;
class HF_SubTitleTable;
class HF_IdlBaseNode;

class HF_IdlInterface : public HtmlFactory_Idl
{
  public:
                        HF_IdlInterface(
                            Environment &       io_rEnv,
                            Xml::Element &      o_rOut );
    virtual             ~HF_IdlInterface();

    void                Produce_byData(
                            const client &      i_ce ) const;
  private:
    // Locals
    DYN HF_NaviSubRow & make_Navibar(
                            const client &      i_ce ) const;

    virtual void        produce_MemberDetails(
                            HF_SubTitleTable &  o_table,
                            const client &      ce ) const;
    void                produce_BaseHierarchy(
                            Xml::Element &      o_screen,
                            const client &      i_ce,
                            const String &      i_sLabel ) const;

    // Locals
    enum E_CurProducedMembers
    {
        mem_none,
        mem_Functions,
        mem_Attributes
    };

    // DATA
    mutable E_CurProducedMembers
                        eCurProducedMembers;
};



// IMPLEMENTATION

extern const String
    C_sCePrefix_Interface;



#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
