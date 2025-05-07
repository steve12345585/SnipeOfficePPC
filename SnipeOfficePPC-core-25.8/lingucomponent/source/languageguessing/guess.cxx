/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/***************************************************************************
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


#include <iostream>
#include <string.h>

#include <libexttextcat/textcat.h>
#include <altstrfunc.hxx>
#include <guess.hxx>

using namespace std;

Guess::Guess()
{
    language_str = DEFAULT_LANGUAGE;
    country_str = DEFAULT_COUNTRY;
    encoding_str = DEFAULT_ENCODING;
}

/*
* this use a char * string to build the guess object
* a string like those is made as : [language-country-encoding]...
*
*/

Guess::Guess(const char * guess_str)
{
    Guess();

    string lang;
    string country;
    string enc;

    //if the guess is not like "UNKNOWN" or "SHORT", go into the brackets
//     if(strncmp((const char*)(guess_str + 1), _TEXTCAT_RESULT_UNKOWN, strlen(_TEXTCAT_RESULT_UNKOWN)) != 0
//        &&
//        strncmp((const char*)(guess_str + 1), _TEXTCAT_RESULT_SHORT, strlen(_TEXTCAT_RESULT_SHORT)) != 0)
//     {
        if(strcmp((const char*)(guess_str + 1), _TEXTCAT_RESULT_UNKOWN) != 0
           &&
           strcmp((const char*)(guess_str + 1), _TEXTCAT_RESULT_SHORT) != 0)
        {

        int current_pointer = 0;

        //this is to go to the first char of the guess string ( the '[' of "[en-US-utf8]" )
        while(!isSeparator(guess_str[current_pointer])){
            current_pointer++;
        }
        current_pointer++;

        //this is to pick up the language ( the "en" from "[en-US-utf8]" )
        while(!isSeparator(guess_str[current_pointer])){
            lang+=guess_str[current_pointer];
            current_pointer++;
        }
        current_pointer++;

        //this is to pick up the country ( the "US" from "[en-US-utf8]" )
        while(!isSeparator(guess_str[current_pointer])){
            country+=guess_str[current_pointer];
            current_pointer++;
        }
        current_pointer++;

        //this is to pick up the encoding ( the "utf8" from "[en-US-utf8]" )
        while(!isSeparator(guess_str[current_pointer])){
            enc+=guess_str[current_pointer];
            current_pointer++;
        }

        if(lang!=""){//if not we use the default value
            language_str=lang;
        }
        country_str=country;

        if(enc!=""){//if not we use the default value
            encoding_str=enc;
        }
    }
}

Guess::~Guess()
{
}

string Guess::GetLanguage()
{
    return language_str;
}

string Guess::GetCountry()
{
    return country_str;
}

string Guess::GetEncoding()
{
    return encoding_str;
}

bool Guess::operator==(string lang)
{
    string toString;
    toString += GetLanguage();
    toString += "-";
    toString += GetCountry();
    toString += "-";
    toString += GetEncoding();
    return start(toString, lang);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
