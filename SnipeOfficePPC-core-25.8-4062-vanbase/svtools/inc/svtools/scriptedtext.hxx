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

#ifndef _SVTOOLS_SCRIPTEDTEXT_HXX
#define _SVTOOLS_SCRIPTEDTEXT_HXX

#include "svtools/svtdllapi.h"
#include <tools/gen.hxx>
#include <com/sun/star/i18n/XBreakIterator.hpp>


namespace rtl { class OUString; }
class OutputDevice;
class Font;
class SvtScriptedTextHelper_Impl;


//_____________________________________________________________________________

/**
This class provides drawing text with different script types on any output devices.
*/
class SVT_DLLPUBLIC SvtScriptedTextHelper
{
private:
    SvtScriptedTextHelper_Impl* mpImpl;             /// Implementation of class functionality.

                                /** Assignment operator not implemented to prevent usage. */
    SvtScriptedTextHelper&      operator=( const SvtScriptedTextHelper& );

public:
                                /** Constructor sets an output device and no fonts.
                                    @param  _rOutDevice
                                    A reference to an output device. */
                                SvtScriptedTextHelper( OutputDevice& _rOutDevice );

                                /** Copy constructor. */
                                SvtScriptedTextHelper(
                                    const SvtScriptedTextHelper& _rCopy );

                                /** Destructor. */
    virtual                     ~SvtScriptedTextHelper();

                                /** Sets new fonts and recalculates the text width.
                                    @param  _pLatinFont
                                    The font for latin characters.
                                    @param  _pAsianFont
                                    The font for asian characters.
                                    @param  _pCmplxFont
                                    The font for complex text layout. */
    void                        SetFonts( Font* _pLatinFont, Font* _pAsianFont, Font* _pCmplxFont );

                                /** Sets the default font of the current output device to all script types. */
    void                        SetDefaultFont();

                                /** Sets a new text and calculates all script breaks and the text width.
                                    @param  _rText
                                    The new text.
                                    @param  _xBreakIter
                                    The break iterator for iterating through the script portions. */
    void                        SetText(
                                    const ::rtl::OUString& _rText,
                                    const ::com::sun::star::uno::Reference< ::com::sun::star::i18n::XBreakIterator >& _xBreakIter );

                                /** Returns a size struct containing the width and height of the text in the current output device.
                                    @return  A size struct with the text dimensions. */
    const Size&                 GetTextSize() const;

                                /** Draws the text in the current output device.
                                    @param _rPos
                                    The position of the top left edge of the text. */
    void                        DrawText( const Point& _rPos );
};

//_____________________________________________________________________________

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
