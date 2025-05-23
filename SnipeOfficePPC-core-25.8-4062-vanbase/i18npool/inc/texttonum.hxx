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
#ifndef _L10N_TRANSLITERATION_TEXTTONUM_HXX_
#define _L10N_TRANSLITERATION_TEXTTONUM_HXX_

#include <transliteration_Numeric.hxx>

namespace com { namespace sun { namespace star { namespace i18n {

#define TRANSLITERATION_TEXTTONUM( name ) \
class TextToNum##name : public transliteration_Numeric \
{ \
public: \
        TextToNum##name (); \
};

#ifdef TRANSLITERATION_ALL
TRANSLITERATION_TEXTTONUM(Lower_zh_CN)
TRANSLITERATION_TEXTTONUM(Upper_zh_CN)
TRANSLITERATION_TEXTTONUM(Lower_zh_TW)
TRANSLITERATION_TEXTTONUM(Upper_zh_TW)
TRANSLITERATION_TEXTTONUM(InformalLower_ko)
TRANSLITERATION_TEXTTONUM(InformalUpper_ko)
TRANSLITERATION_TEXTTONUM(InformalHangul_ko)
TRANSLITERATION_TEXTTONUM(FormalLower_ko)
TRANSLITERATION_TEXTTONUM(FormalUpper_ko)
TRANSLITERATION_TEXTTONUM(FormalHangul_ko)
TRANSLITERATION_TEXTTONUM(KanjiLongModern_ja_JP)
TRANSLITERATION_TEXTTONUM(KanjiLongTraditional_ja_JP)
#endif
#undef TRANSLITERATION_TEXTTONUM

} } } }

#endif // _L10N_TRANSLITERATION_TEXTTONUM_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
