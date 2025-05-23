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

#ifndef _PAD_TITLECTRL_HXX_
#define _PAD_TITLECTRL_HXX_

#include <vcl/ctrl.hxx>
#include <vcl/image.hxx>

namespace padmin
{

class TitleImage : public Control
{
    Image               m_aImage;
    String              m_aText;
    Color               m_aBGColor;
    Point               m_aImagePos;
    Point               m_aTextPos;

    bool                m_bArranged;

    void arrange();
public:
    TitleImage( Window* pParent, const ResId& rResId );
    ~TitleImage();

    virtual void Paint( const Rectangle& rRect );

    void SetImage( const Image& rImage );
    const Image& GetImage() const { return m_aImage; }

    virtual void SetText( const String& rText );
    virtual String GetText() const { return m_aText; }

    void SetBackgroundColor( const Color& rColor );
    const Color& GetBackgroundColor() const { return m_aBGColor; }
};

}

#endif // _PAD_TITLECTRL_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
