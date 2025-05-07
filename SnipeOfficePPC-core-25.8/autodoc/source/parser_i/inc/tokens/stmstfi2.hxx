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

#ifndef ADC_STMSTFI2_HXX
#define ADC_STMSTFI2_HXX

// USED SERVICES
    // BASE CLASSES
#include <tokens/stmstat2.hxx>
    // COMPONENTS
    // PARAMETERS


class TkpContext;
class StateMachineContext;

/**
**/
class StmBoundsStatu2 : public StmStatu2
{
  public:
    // LIFECYCLE
                        StmBoundsStatu2(
                            StateMachineContext &
                                                o_rOwner,
                            TkpContext &        i_rFollowUpContext,
                            uintt               i_nStatusFunctionNr,
                            bool                i_bIsDefault  );
    // INQUIRY
    TkpContext *        FollowUpContext();
    uintt               StatusFunctionNr() const;
    virtual bool        IsADefault() const;

    // ACCESS
    virtual StmBoundsStatu2 *
                        AsBounds();

  private:
    StateMachineContext *
                        pOwner;
    TkpContext *        pFollowUpContext;
    uintt               nStatusFunctionNr;
    bool                bIsDefault;
};

inline TkpContext *
StmBoundsStatu2::FollowUpContext()
    { return pFollowUpContext; }
inline uintt
StmBoundsStatu2::StatusFunctionNr() const
    { return nStatusFunctionNr; }


#endif


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
