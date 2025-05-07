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

#ifndef _LEX_HXX
#define _LEX_HXX

#include <boost/ptr_container/ptr_vector.hpp>

#include <hash.hxx>
#include <tools/gen.hxx>
#include <tools/stream.hxx>

enum SVTOKEN_ENUM { SVTOKEN_EMPTY,      SVTOKEN_COMMENT,
                    SVTOKEN_INTEGER,    SVTOKEN_STRING,
                    SVTOKEN_BOOL,       SVTOKEN_IDENTIFIER,
                    SVTOKEN_CHAR,       SVTOKEN_RTTIBASE,
                    SVTOKEN_EOF,        SVTOKEN_HASHID };

class BigInt;
class SvToken
{
friend class SvTokenStream;
    sal_uLong               nLine, nColumn;
    SVTOKEN_ENUM            nType;
    rtl::OString            aString;
    union
    {
        sal_uLong           nLong;
        sal_Bool            bBool;
        char                cChar;
        SvStringHashEntry * pHash;
    };
public:
            SvToken();
            SvToken( const SvToken & rObj );
            SvToken( sal_uLong n );
            SvToken( SVTOKEN_ENUM nTypeP, sal_Bool b );
            SvToken( char c );
            SvToken( SVTOKEN_ENUM nTypeP, const rtl::OString& rStr );
            SvToken( SVTOKEN_ENUM nTypeP );

    SvToken & operator = ( const SvToken & rObj );

    rtl::OString GetTokenAsString() const;
    SVTOKEN_ENUM    GetType() const { return nType; }

    void        SetLine( sal_uLong nLineP )     { nLine = nLineP;       }
    sal_uLong       GetLine() const             { return nLine;         }

    void        SetColumn( sal_uLong nColumnP ) { nColumn = nColumnP;   }
    sal_uLong       GetColumn() const           { return nColumn;       }

    sal_Bool        IsEmpty() const     { return nType == SVTOKEN_EMPTY; }
    sal_Bool        IsComment() const   { return nType == SVTOKEN_COMMENT; }
    sal_Bool        IsInteger() const   { return nType == SVTOKEN_INTEGER; }
    sal_Bool        IsString() const    { return nType == SVTOKEN_STRING; }
    sal_Bool        IsBool() const      { return nType == SVTOKEN_BOOL; }
    sal_Bool        IsIdentifierHash() const
                { return nType == SVTOKEN_HASHID; }
    sal_Bool        IsIdentifier() const
                {
                    return nType == SVTOKEN_IDENTIFIER
                            || nType == SVTOKEN_HASHID;
                }
    sal_Bool        IsChar() const      { return nType == SVTOKEN_CHAR; }
    sal_Bool        IsRttiBase() const  { return nType == SVTOKEN_RTTIBASE; }
    sal_Bool        IsEof() const       { return nType == SVTOKEN_EOF; }

    const rtl::OString& GetString() const
                {
                    return IsIdentifierHash()
                        ? pHash->GetName()
                        : aString;
                }
    sal_uLong       GetNumber() const       { return nLong;         }
    sal_Bool        GetBool() const         { return bBool;         }
    char        GetChar() const         { return cChar;         }

    void        SetHash( SvStringHashEntry * pHashP )
                { pHash = pHashP; nType = SVTOKEN_HASHID; }
    sal_Bool        HasHash() const
                { return nType == SVTOKEN_HASHID; }
    SvStringHashEntry * GetHash() const { return pHash; }
    sal_Bool        Is( SvStringHashEntry * pEntry ) const
                { return IsIdentifierHash() && pHash == pEntry; }
};

inline SvToken::SvToken()
    : nType( SVTOKEN_EMPTY ) {}

inline SvToken::SvToken( sal_uLong n )
    : nType( SVTOKEN_INTEGER ), nLong( n ) {}

inline SvToken::SvToken( SVTOKEN_ENUM nTypeP, sal_Bool b )
    : nType( nTypeP ), bBool( b ) {}

inline SvToken::SvToken( char c )
    : nType( SVTOKEN_CHAR ), cChar( c ) {}

inline SvToken::SvToken( SVTOKEN_ENUM nTypeP, const rtl::OString& rStr )
    : nType( nTypeP ), aString( rStr ) {}

inline SvToken::SvToken( SVTOKEN_ENUM nTypeP )
: nType( nTypeP ) {}

class SvTokenStream
{
    sal_uLong       nLine, nColumn;
    int         nBufPos;
    int         c;          // next character
    sal_uInt16      nTabSize;   // length of tabulator
    rtl::OString    aStrTrue;
    rtl::OString    aStrFalse;
    sal_uLong       nMaxPos;

    SvFileStream *  pInStream;
    SvStream &      rInStream;
    String          aFileName;
    boost::ptr_vector<SvToken> aTokList;
    boost::ptr_vector<SvToken>::iterator pCurToken;

    void        InitCtor();

    rtl::OString aBufStr;
    int             GetNextChar();
    int             GetFastNextChar()
                    {
                        return aBufStr[nBufPos++];
                    }

    void            FillTokenList();
    sal_uLong           GetNumber();
    sal_Bool            MakeToken( SvToken & );
    sal_Bool            IsEof() const { return rInStream.IsEof(); }
    void            SetMax()
                    {
                        sal_uLong n = Tell();
                        if( n > nMaxPos )
                            nMaxPos = n;
                    }
    void            CalcColumn()
                    {
                        // if end of line spare calculation
                        if( 0 != c )
                        {
                            sal_uInt16 n = 0;
                            nColumn = 0;
                            while( n < nBufPos )
                                nColumn += aBufStr[n++] == '\t' ? nTabSize : 1;
                        }
                    }
public:
                    SvTokenStream( const String & rFileName );
                    SvTokenStream( SvStream & rInStream, const String & rFileName );
                    ~SvTokenStream();

    const String &  GetFileName() const { return aFileName; }
    SvStream &      GetStream() { return rInStream; }

    void            SetTabSize( sal_uInt16 nTabSizeP )
                    { nTabSize = nTabSizeP; }
    sal_uInt16          GetTabSize() const { return nTabSize; }

    SvToken* GetToken_PrevAll()
    {
        boost::ptr_vector<SvToken>::iterator pRetToken = pCurToken;

        // current iterator always valid
        if(pCurToken != aTokList.begin())
            --pCurToken;

        return &(*pRetToken);
    }

    SvToken* GetToken_NextAll()
    {
        boost::ptr_vector<SvToken>::iterator pRetToken = pCurToken++;

        if (pCurToken == aTokList.end())
            pCurToken = pRetToken;

        SetMax();

        return &(*pRetToken);
    }

    SvToken* GetToken_Next()
    {
        // comments get removed initially
        return GetToken_NextAll();
    }

    SvToken* GetToken() const { return &(*pCurToken); }

    sal_Bool            Read( char cChar )
                    {
                        if( pCurToken->IsChar()
                          && cChar == pCurToken->GetChar() )
                        {
                            GetToken_Next();
                            return sal_True;
                        }
                        else
                            return sal_False;
                    }

    void            ReadDelemiter()
                    {
                        if( pCurToken->IsChar()
                          && (';' == pCurToken->GetChar()
                                || ',' == pCurToken->GetChar()) )
                        {
                            GetToken_Next();
                        }
                    }

    sal_uInt32 Tell() const { return pCurToken-aTokList.begin(); }

    void Seek( sal_uInt32 nPos )
    {
        pCurToken = aTokList.begin() + nPos;
        SetMax();
    }

    void SeekRel( sal_uInt32 nRelPos )
    {
        sal_uInt32 relIdx = Tell() + nRelPos;

        if ( relIdx < aTokList.size())
        {
            pCurToken = aTokList.begin()+ (Tell() + nRelPos );
            SetMax();
        }
    }

    void SeekEnd()
    {
        pCurToken = aTokList.begin()+nMaxPos;
    }
};



#endif // _LEX_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
