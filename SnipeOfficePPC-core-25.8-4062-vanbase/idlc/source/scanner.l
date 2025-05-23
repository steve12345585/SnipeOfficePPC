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

%option yylineno

%{
/*
 * scanner.ll - Lexical scanner for IDLC 1.0
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#ifndef _IDLC_IDLC_HXX_
#include <idlc/idlc.hxx>
#endif
#ifndef _IDLC_ERRORHANDLER_HXX_
#include <idlc/errorhandler.hxx>
#endif
#ifndef _IDLC_FEHELPER_HXX_
#include <idlc/fehelper.hxx>
#endif

#include "attributeexceptions.hxx"


class AstExpression;
class AstArray;
class AstMember;

#include <parser.hxx>

/* handle locations */
int yycolumn = 1;

#define YY_USER_ACTION idlc()->setOffset(yycolumn, yycolumn+yyleng-1); \
    yycolumn += yyleng;

sal_Int32		beginLine = 0;
::rtl::OString	docu;

static int asciiToInteger(char const * s, sal_Int64  * sval, sal_uInt64 * uval) {
    bool neg = false;
    if (*s == '-') {
        neg = true;
        ++s;
    }
    unsigned int base = 10;
    if (*s == '0') {
        base = 8;
        ++s;
        if (*s == 'X' || *s == 'x') {
            base = 16;
            ++s;
        }
    }
    sal_uInt64 val = 0;
    for (; *s != 0; ++s) {
        unsigned int n;
        if (*s >= '0' && *s <= '9') {
            n = *s - '0';
        } else {
            switch (*s) {
            case 'A':
            case 'a':
                n = 10;
                break;
            case 'B':
            case 'b':
                n = 11;
                break;
            case 'C':
            case 'c':
                n = 12;
                break;
            case 'D':
            case 'd':
                n = 13;
                break;
            case 'E':
            case 'e':
                n = 14;
                break;
            case 'F':
            case 'f':
                n = 15;
                break;
            default:
                goto done;
            }
        }
        // The following guarantees the invariant val <= SAL_MAX_UINT64 (because
        // base and n are sufficiently small), *if*
        // std::numeric_limits<sal_uInt64>::max() == SAL_MAX_UINT64:
        sal_uInt64 nval = val * base + n;
        if (nval < val) {
            idlc()->error()->syntaxError(
                PS_NoState, idlc()->getLineNumber(),
                "integral constant too large");
            val = 0;
            break;
        }
        val = nval;
    }
 done:
    if (neg) {
        if (val < SAL_CONST_UINT64(0x8000000000000000)) {
            *sval = -static_cast< sal_Int64 >(val);
        } else if (val == SAL_CONST_UINT64(0x8000000000000000)) {
            *sval = SAL_MIN_INT64;
        } else {
            idlc()->error()->syntaxError(
                PS_NoState, idlc()->getLineNumber(),
                "negative integral constant too large");
            *sval = 0;
        }
        return IDL_INTEGER_LITERAL;
    } else if (val <= static_cast< sal_uInt64 >(SAL_MAX_INT64)) {
        *sval = static_cast< sal_Int64 >(val);
        return IDL_INTEGER_LITERAL;
    } else {
        *uval = val;
        return IDL_INTEGER_ULITERAL;
    }
}

static double asciiToFloat(const sal_Char *s)
{
   	double  	d = 0.0;
   	double  	e, k;
   	sal_Int32  	neg = 0, negexp = 0;

   	if (*s == '-')
   	{
    	neg = 1;
       	s++;
   	}
   	while (*s >= '0' && *s <= '9')
   	{
    	d = (d * 10) + *s - '0';
       	s++;
   	}
   	if (*s == '.')
   	{
    	s++;
       	e = 10;
       	while (*s >= '0' && *s <= '9')
       	{
        	d += (*s - '0') / (e * 1.0);
           	e *= 10;
           	s++;
       	}
   	}
   	if (*s == 'e' || *s == 'E')
   	{
    	s++;
       	if (*s == '-')
        {
        	negexp = 1;
           	s++;
       	} else
       	{
        	if (*s == '+')
            	s++;
           	e = 0;
           	while (*s >= '0' && *s <= '9')
           	{
            	e = (e * 10) + *s - '0';
             	s++;
           	}
           	if (e > 0)
           	{
                for (k = 1; e > 0; k *= 10, e--)
                    ;
               	if (negexp)
                	d /= k;
               	else
                	d *= k;
           	}
		}
   	}
   	if (neg) d *= -1.0;
   	return d;
}

static void	idlParsePragma(sal_Char* pPragma)
{
	::rtl::OString pragma(pPragma);
	sal_Int32 index = pragma.indexOf("include");
	sal_Char* begin = pPragma + index + 8;
	sal_Char* offset = begin;
	while (*offset != ',') offset++;
	//::rtl::OString include = pragma.copy(index + 8, offset - begin);
	//unused// idlc()->insertInclude(pragma.copy(index + 8, (sal_Int32)(offset - begin)));
}	

static void parseLineAndFile(sal_Char* pBuf)
{
	sal_Char	*r = pBuf;
	sal_Char    *h;
	sal_Bool	bIsInMain = sal_False;

	/* Skip initial '#' */
	if (*r != '#')
		return;

	/* Find line number */
	for (r++; *r == ' ' || *r == '\t' || isalpha(*r); r++) ;
	h = r;
	for (; *r != '\0' && *r != ' ' && *r != '\t'; r++) ;
	*r++ = 0;
	idlc()->setLineNumber((sal_uInt32)atol(h));
    yylineno = atol(h);

	/* Find file name, if present */
	for (; *r != '"'; r++)
	{
		if (*r == '\n' || *r == '\0')
			return;
	}
	h = ++r;
	for (; *r != '"'; r++) ;
	*r = 0;
	if (*h == '\0')
		idlc()->setFileName(::rtl::OString("standard input"));
	else
		idlc()->setFileName(::rtl::OString(h));

	bIsInMain = (idlc()->getFileName() == idlc()->getRealFileName()) ? sal_True : sal_False;
	idlc()->setInMainfile(bIsInMain);		
}	

// Suppress any warnings from generated code:
#if defined __GNUC__
#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 2))
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-label"
#endif
#elif defined __SUNPRO_CC
#pragma disable_warn
#elif defined _MSC_VER
#pragma warning(push, 1)
/**/
#ifdef yywrap
#undef  yywrap
#define yywrap() 1
#endif
/**/
#endif
%}

%option noyywrap
%option never-interactive

%x DOCU
%x COMMENT

DIGIT           [0-9]
OCT_DIGIT       [0-7]
HEX_DIGIT       [a-fA-F0-9]
CAPITAL         [A-Z]
ALPHA           [a-zA-Z]
INT_LITERAL     [1-9][0-9]*
OCT_LITERAL     0{OCT_DIGIT}*
HEX_LITERAL     (0x|0X){HEX_DIGIT}*

IDENTIFIER_NEW  ({ALPHA}({ALPHA}|{DIGIT})*)|({CAPITAL}("_"?({ALPHA}|{DIGIT})+)*)
IDENTIFIER      ("_"?({ALPHA}|{DIGIT})+)*

%%

[ \t\r]+	; /* eat up whitespace */
[\n]           {
       idlc()->incLineNumber();
       yycolumn = 1;
       yylineno++;
}

attribute       return IDL_ATTRIBUTE;
bound           return IDL_BOUND;
case            return IDL_CASE;
const           return IDL_CONST;
constants       return IDL_CONSTANTS;
constrained     return IDL_CONSTRAINED;
default         return IDL_DEFAULT;
enum            return IDL_ENUM;
exception       return IDL_EXCEPTION;
interface       return IDL_INTERFACE;
maybeambiguous  return IDL_MAYBEAMBIGUOUS;
maybedefault    return IDL_MAYBEDEFAULT;
maybevoid       return IDL_MAYBEVOID;
module          return IDL_MODULE;
needs           return IDL_NEEDS;
observes        return IDL_OBSERVES;
optional        return IDL_OPTIONAL;
property        return IDL_PROPERTY;
raises          return IDL_RAISES;
readonly        return IDL_READONLY;
removable       return IDL_REMOVEABLE;
service         return IDL_SERVICE;
sequence        return IDL_SEQUENCE;
singleton       return IDL_SINGLETON;
struct          return IDL_STRUCT;
switch          return IDL_SWITCH;
transient       return IDL_TRANSIENT;
typedef         return IDL_TYPEDEF;
union           return IDL_UNION;

any             return IDL_ANY;				
boolean         return IDL_BOOLEAN;
byte            return IDL_BYTE;
char            return IDL_CHAR;
double          return IDL_DOUBLE;
float           return IDL_FLOAT;
hyper           return IDL_HYPER;
long            return IDL_LONG;
short           return IDL_SHORT;
string          return IDL_STRING;
type            return IDL_TYPE;
unsigned        return IDL_UNSIGNED;
void            return IDL_VOID;

TRUE            return IDL_TRUE;
True            return IDL_TRUE;
FALSE           return IDL_FALSE;
False           return IDL_FALSE;

in              return IDL_IN;
out             return IDL_OUT;
inout           return IDL_INOUT;

get             return IDL_GET;
set             return IDL_SET;

published       return IDL_PUBLISHED;

"..."           return IDL_ELLIPSIS;

("-")?{INT_LITERAL}+(l|L|u|U)?    {
                return asciiToInteger(yytext, &yylval.ival, &yylval.uval);
            }

("-")?{OCT_LITERAL}+(l|L|u|U)?    {
                return asciiToInteger(yytext, &yylval.ival, &yylval.uval);
            }

("-")?{HEX_LITERAL}+(l|L|u|U)?    {
                return asciiToInteger(yytext, &yylval.ival, &yylval.uval);
            }

("-")?{DIGIT}+(e|E){1}(("+"|"-")?{DIGIT}+)+(f|F)?	|
("-")?"."{DIGIT}+((e|E)("+"|"-")?{DIGIT}+)?(f|F)?	|
("-")?{DIGIT}*"."{DIGIT}+((e|E)("+"|"-")?{DIGIT}+)?(f|F)?        {
                yylval.dval = asciiToFloat( yytext );
				return IDL_FLOATING_PT_LITERAL;
            }

{IDENTIFIER}	{
				yylval.sval = new ::rtl::OString(yytext);
				return IDL_IDENTIFIER;
			}

\<\<  	{
		yylval.strval = yytext;
		return IDL_LEFTSHIFT;
	}
\>\>	{
		yylval.strval = yytext;
		return IDL_RIGHTSHIFT;
	}
\:\:	{
		yylval.strval = yytext;
		return IDL_SCOPESEPARATOR;
	}

"/*"	{ 
			BEGIN( COMMENT );
			docu = ::rtl::OString();
			beginLine = idlc()->getLineNumber();
		}

"/***"	{ 
			BEGIN( COMMENT );
			docu = ::rtl::OString();
			beginLine = idlc()->getLineNumber();
		}

<COMMENT>[^*]+	{
				docu += ::rtl::OString(yytext);
			} 

<COMMENT>"*"[^*/]+ 	{
				docu += ::rtl::OString(yytext);
			}

<COMMENT>"**" 	{
				docu += ::rtl::OString(yytext);
			}

<COMMENT>[*]+"/"  {
				docu = docu.trim();
                sal_Int32 nIndex = 0;
                int count = 0;
                do { docu.getToken( 0, '\n', nIndex ); count++; } while( nIndex != -1 );
				idlc()->setLineNumber( beginLine + count - 1);
			  	BEGIN( INITIAL );
			}

"/**"	{
			BEGIN( DOCU );
			docu = ::rtl::OString();
			beginLine = idlc()->getLineNumber();
		}

<DOCU>[^*\n]+	{
				docu += ::rtl::OString(yytext);
			}

<DOCU>"\n"[ \t]*"*"{1} 	{
				idlc()->setLineNumber( idlc()->getLineNumber()  + 1);
				docu += ::rtl::OString("\n");
			}

<DOCU>"\n"	{
				idlc()->setLineNumber( idlc()->getLineNumber()  + 1);
				docu += ::rtl::OString(yytext);
			}

<DOCU>"*"[^*^/\n]* 	{
				docu += ::rtl::OString(yytext);
			}

<DOCU>"\n"[ \t]*"*/" 	{
				docu = docu.trim();
				sal_Int32 nIndex = 0;
				int count = 0;
				do { docu.getToken( 0, '\n', nIndex ); count++; } while( nIndex != -1 );
				idlc()->setLineNumber( beginLine + count - 1);                
				if ( (nIndex = docu.indexOf("/*")) >= 0 || (nIndex = docu.indexOf("///")) >= 0 )
				{
                    if ( 0 != nIndex &&
                         (docu.getStr()[nIndex - 1] != '"' && docu.getStr()[nIndex - 1] != ':') )
                        idlc()->error()->syntaxError(PS_NoState, idlc()->getLineNumber(),
                                                     "nested documentation strings are not allowed!");
				}
				idlc()->setDocumentation(docu);
			  	BEGIN( INITIAL );
			}

<DOCU>"*/"	{
				docu = docu.trim();
				sal_Int32 nIndex = 0;
				int count = 0;
				do { docu.getToken( 0, '\n', nIndex ); count++; } while( nIndex != -1 );
				idlc()->setLineNumber( beginLine + count - 1);
				if ( docu.indexOf("/*") >= 0 || docu.indexOf("//") >= 0 )
				{
                    if ( 0 != nIndex &&
                         (docu.getStr()[nIndex - 1] != '"' && docu.getStr()[nIndex - 1] != ':') )
                        idlc()->error()->syntaxError(PS_NoState, idlc()->getLineNumber(),
                                                     "nested documentation strings are not allowed!");
				}
				idlc()->setDocumentation(docu);
			  	BEGIN( INITIAL );
			}

"//"[^/]{1}.*"\n" {
				/* only a comment */
				::rtl::OString docStr(yytext);
				docStr = docStr.copy( 0, docStr.lastIndexOf('\n') );
				docStr = docStr.copy( docStr.lastIndexOf('/')+1 );
				docStr = docStr.trim();
				idlc()->incLineNumber();
			}

"///".*"\n"  {
				::rtl::OString docStr(yytext);
				docStr = docStr.copy( 0, docStr.lastIndexOf('\n') );
				docStr = docStr.copy( docStr.lastIndexOf('/')+1 );
				docStr = docStr.trim();
				idlc()->incLineNumber();
				idlc()->setDocumentation(docStr);
			}

.	return yytext[0];

^#[ \t]*line[ \t]*[0-9]*" ""\""[^\"]*"\""\n    {
	parseLineAndFile(yytext);
}

^#[ \t]*[0-9]*" ""\""[^\"]*"\""" "[0-9]*\n {
	parseLineAndFile(yytext);
}

^#[ \t]*[0-9]*" ""\""[^\"]*"\""\n {
	parseLineAndFile(yytext);
}

^#[ \t]*[0-9]*\n {
	parseLineAndFile(yytext);
}

^#[ \t]*ident.*\n {
	/* ignore cpp ident */
	idlc()->incLineNumber();
}

^#[ \t]*pragma[ \t].*\n        {       /* remember pragma */
	idlParsePragma(yytext);
	idlc()->incLineNumber();
}

%%
