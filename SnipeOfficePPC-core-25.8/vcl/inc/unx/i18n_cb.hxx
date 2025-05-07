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
#ifndef _SAL_I18N_CALLBACK_HXX
#define _SAL_I18N_CALLBACK_HXX

#include <salwtype.hxx>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

// xim callbacks
void PreeditDoneCallback ( XIC ic, XPointer client_data, XPointer call_data);
int  PreeditStartCallback( XIC ic, XPointer client_data, XPointer call_data);
void PreeditDoneCallback ( XIC ic, XPointer client_data, XPointer call_data);
void PreeditDrawCallback ( XIC ic, XPointer client_data,
                           XIMPreeditDrawCallbackStruct *call_data );
void PreeditCaretCallback( XIC ic, XPointer client_data,
                           XIMPreeditCaretCallbackStruct *call_data );
void GetPreeditSpotLocation(XIC ic, XPointer client_data);

// private hook to prevent from sending further edit events
void PreeditCancelCallback( XPointer client_data );

void StatusStartCallback (XIC ic, XPointer client_data, XPointer call_data);
void StatusDoneCallback  (XIC ic, XPointer client_data, XPointer call_data);
void StatusDrawCallback  (XIC ic, XPointer client_data,
            XIMStatusDrawCallbackStruct *call_data);
void SwitchIMCallback (XIC ix, XPointer client_data, XPointer call_data );

// keep informed if kinput2 crashed again
void IC_IMDestroyCallback (XIM im, XPointer client_data, XPointer call_data);
void IM_IMDestroyCallback (XIM im, XPointer client_data, XPointer call_data);

Bool IsControlCode(sal_Unicode nChar);

#ifdef __cplusplus
} /* extern "C" */
#endif

typedef struct {
  sal_Unicode   *pUnicodeBuffer;
  XIMFeedback   *pCharStyle;
  unsigned int   nCursorPos;
  unsigned int   nLength;
  unsigned int   nSize;
} preedit_text_t;

class SalFrame;

typedef enum {
    ePreeditStatusDontKnow = 0,
    ePreeditStatusActive,
    ePreeditStatusActivationRequired,
    ePreeditStatusStartPending
} preedit_status_t;

typedef struct {
    SalFrame*               pFrame;
    preedit_status_t        eState;
    preedit_text_t          aText;
    SalExtTextInputEvent    aInputEv;
    std::vector< sal_uInt16 >   aInputFlags;
} preedit_data_t;

#endif /* _SAL_I18N_CALLBACK_HXX */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
