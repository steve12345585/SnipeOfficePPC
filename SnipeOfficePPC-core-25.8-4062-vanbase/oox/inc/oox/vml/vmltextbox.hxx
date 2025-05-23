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

#ifndef OOX_VML_VMLTEXTBOX_HXX
#define OOX_VML_VMLTEXTBOX_HXX

#include <vector>
#include <rtl/ustring.hxx>
#include "oox/helper/helper.hxx"
#include "oox/dllapi.h"
#include <com/sun/star/uno/Reference.h>

namespace com { namespace sun { namespace star {
    namespace drawing { class XShape; }
} } }

namespace oox {
namespace vml {

struct ShapeTypeModel;

// ============================================================================

/** Font settings for a text portion in a textbox. */
struct OOX_DLLPUBLIC TextFontModel
{
    OptValue< ::rtl::OUString > moName;     ///< Font name.
    OptValue< ::rtl::OUString > moColor;    ///< Font color, HTML encoded, sort of.
    OptValue< sal_Int32 > monSize;          ///< Font size in twips.
    OptValue< sal_Int32 > monUnderline;     ///< Single or double underline.
    OptValue< sal_Int32 > monEscapement;    ///< Subscript or superscript.
    OptValue< bool >    mobBold;
    OptValue< bool >    mobItalic;
    OptValue< bool >    mobStrikeout;

    explicit            TextFontModel();
};

// ============================================================================

/** A text portion in a textbox with the same formatting for all characters. */
struct TextPortionModel
{
    TextFontModel       maFont;
    ::rtl::OUString     maText;

    explicit            TextPortionModel( const TextFontModel& rFont, const ::rtl::OUString& rText );
};

// ============================================================================

/** The textbox contains all text contents and properties. */
class OOX_DLLPUBLIC TextBox
{
public:
    explicit            TextBox(ShapeTypeModel& rTypeModel);

    /** Appends a new text portion to the textbox. */
    void                appendPortion( const TextFontModel& rFont, const ::rtl::OUString& rText );

    /** Returns the current number of text portions. */
    inline size_t       getPortionCount() const { return maPortions.size(); }
    /** Returns the font settings of the first text portion. */
    const TextFontModel* getFirstFont() const;
    /** Returns the entire text of all text portions. */
    ::rtl::OUString     getText() const;
    void convert(com::sun::star::uno::Reference<com::sun::star::drawing::XShape> xShape) const;

    ShapeTypeModel&     mrTypeModel;
    /// Text distance from the border (inset attribute of v:textbox), valid only if set.
    bool borderDistanceSet;
    int borderDistanceLeft, borderDistanceTop, borderDistanceRight, borderDistanceBottom;

private:
    typedef ::std::vector< TextPortionModel > PortionVector;

    PortionVector       maPortions;
};

// ============================================================================

} // namespace vml
} // namespace oox

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
