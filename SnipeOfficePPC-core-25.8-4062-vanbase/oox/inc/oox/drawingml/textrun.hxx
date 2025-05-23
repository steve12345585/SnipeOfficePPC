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

#ifndef OOX_DRAWINGML_TEXTRUN_HXX
#define OOX_DRAWINGML_TEXTRUN_HXX

#include <com/sun/star/text/XTextCursor.hpp>
#include <com/sun/star/text/XText.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include "oox/drawingml/textcharacterproperties.hxx"

namespace oox { namespace drawingml {

class TextRun
{
public:
    TextRun();
    virtual ~TextRun();

    inline ::rtl::OUString&         getText() { return msText; }
    inline const ::rtl::OUString&   getText() const { return msText; }

    inline TextCharacterProperties&         getTextCharacterProperties() { return maTextCharacterProperties; }
    inline const TextCharacterProperties&   getTextCharacterProperties() const { return maTextCharacterProperties; }

    inline void                 setLineBreak() { mbIsLineBreak = true; }

    virtual sal_Int32               insertAt(
                                    const ::oox::core::XmlFilterBase& rFilterBase,
                                    const ::com::sun::star::uno::Reference < ::com::sun::star::text::XText >& xText,
                                    const ::com::sun::star::uno::Reference < ::com::sun::star::text::XTextCursor >& xAt,
                                    const TextCharacterProperties& rTextCharacterStyle ) const;

private:
    ::rtl::OUString             msText;
    TextCharacterProperties     maTextCharacterProperties;
    bool                        mbIsLineBreak;
};

typedef boost::shared_ptr< TextRun > TextRunPtr;

} }

#endif  //  OOX_DRAWINGML_TEXTRUN_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
