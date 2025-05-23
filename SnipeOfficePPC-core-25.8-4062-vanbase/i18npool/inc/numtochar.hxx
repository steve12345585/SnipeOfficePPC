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
#ifndef _L10N_TRANSLITERATION_NUMTOCHAR_HXX_
#define _L10N_TRANSLITERATION_NUMTOCHAR_HXX_

#include <transliteration_Numeric.hxx>

namespace com { namespace sun { namespace star { namespace i18n {

#define TRANSLITERATION_NUMTOCHAR( name ) \
class NumToChar##name : public transliteration_Numeric \
{ \
public: \
        NumToChar##name (); \
};

#ifdef TRANSLITERATION_ALL
TRANSLITERATION_NUMTOCHAR(Lower_zh_CN)
TRANSLITERATION_NUMTOCHAR(Upper_zh_CN)
TRANSLITERATION_NUMTOCHAR(Lower_zh_TW)
TRANSLITERATION_NUMTOCHAR(Upper_zh_TW)
TRANSLITERATION_NUMTOCHAR(Upper_ko)
TRANSLITERATION_NUMTOCHAR(Hangul_ko)
TRANSLITERATION_NUMTOCHAR(Lower_ko)
TRANSLITERATION_NUMTOCHAR(KanjiShort_ja_JP)
TRANSLITERATION_NUMTOCHAR(KanjiTraditional_ja_JP)
TRANSLITERATION_NUMTOCHAR(Fullwidth)
TRANSLITERATION_NUMTOCHAR(Indic_ar)
TRANSLITERATION_NUMTOCHAR(EastIndic_ar)
TRANSLITERATION_NUMTOCHAR(Indic_hi)
TRANSLITERATION_NUMTOCHAR(_th)
TRANSLITERATION_NUMTOCHAR(Halfwidth)
#endif
#undef TRANSLITERATION_NUMTOCHAR

} } } }

#endif // _L10N_TRANSLITERATION_NUMTOCHAR_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
