/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Version: MPL 1.1 / GPLv3+ / LGPLv3+
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Initial Developer of the Original Code is
 *       Miklos Vajna <vmiklos@suse.cz> (SUSE, Inc.)
 * Portions created by the Initial Developer are Copyright (C) 2012 the
 * Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPLv3+"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
 * in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
 * instead of those above.
 */

#ifndef INCLUDED_MSFILTER_UTIL_HXX
#define INCLUDED_MSFILTER_UTIL_HXX

#include <rtl/textenc.h>
#include <com/sun/star/lang/Locale.hpp>
#include "filter/msfilter/msfilterdllapi.h"

namespace msfilter {
namespace util {

/// Returns the best-fit default 8bit encoding for a given locale
/// i.e. useful when dealing with legacy formats which use legacy text encodings without recording
/// what the encoding is, but you know or can guess the language
MSFILTER_DLLPUBLIC rtl_TextEncoding getBestTextEncodingFromLocale(const ::com::sun::star::lang::Locale &rLocale);

/// Convert a color in BGR format to RGB.
MSFILTER_DLLPUBLIC sal_uInt32 BGRToRGB(sal_uInt32 nColour);
}
}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
