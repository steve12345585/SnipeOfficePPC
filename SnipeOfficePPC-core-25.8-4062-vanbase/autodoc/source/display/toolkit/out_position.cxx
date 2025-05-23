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

#include <precomp.h>
#include <toolkit/out_position.hxx>


// NOT FULLY DEFINED SERVICES



namespace output
{



namespace
{

const int       C_nAssumedMaxLinkLength = 500;

void                move_ToParent(
                        Node * &            io_node,
                        intt                i_levels = 1 );

void
move_ToParent( Node * &   io_node,
               intt       i_levels )
{
    for ( intt n = 0; n < i_levels; ++n )
    {
        csv_assert(io_node != 0);
        io_node = io_node->Parent();
    }
}



}   // namepace anonymous



Position::Position()
    :   sFile(),
        pDirectory(&Node::Null_())
{
}


Position::Position( Node &              i_directory,
                    const String &      i_file )
    :   sFile(i_file),
        pDirectory(&i_directory)
{
}

Position::Position( const Position &    i_directory,
                    const String &      i_sDifferentFile )
    :   sFile(i_sDifferentFile),
        pDirectory(i_directory.pDirectory)
{
}


Position::~Position()
{
}


Position &
Position::operator=( Node & i_node )
{
    pDirectory = &i_node;
    sFile.clear();
    return *this;
}

Position &
Position::operator+=( const String & i_nodeName )
{
    csv_assert(pDirectory != 0);

    pDirectory = &pDirectory->Provide_Child(i_nodeName);
    sFile.clear();

    return *this;
}

Position &
Position::operator-=( intt i_levels )
{
    csv_assert(pDirectory != 0);

    for ( intt i = i_levels; i > 0; --i )
    {
        pDirectory = pDirectory->Parent();
        if (pDirectory == 0)
        {
            pDirectory = &Node::Null_();
            i = 0;
        }
    }
    sFile.clear();

    return *this;
}

String
Position::LinkToRoot() const
{
    StreamLock sl(C_nAssumedMaxLinkLength);
    return sl() << get_UpLink(Depth()) << c_str;
}

void
Position::Get_LinkTo( StreamStr &         o_result,
                      const Position &    i_destination,
                      const String &      i_localLabel ) const
{
    Node * p1 = pDirectory;
    Node * p2 = i_destination.pDirectory;

    intt diff = Depth() - i_destination.Depth();
    intt pathLength1 = 0;
    intt pathLength2 = 0;

    if ( diff > 0 )
    {
        pathLength1 = diff;
        move_ToParent(p1,pathLength1);
    }
    else if ( diff < 0 )
    {
        pathLength2 = -diff;
        move_ToParent(p2,pathLength2);
    }

    while ( p1 != p2 )
    {
        move_ToParent(p1);
        move_ToParent(p2);
        ++pathLength1;
        ++pathLength2;
    }

    o_result << get_UpLink(pathLength1);
    i_destination.pDirectory->Get_Path(o_result, pathLength2);
    o_result << i_destination.sFile;
    if (i_localLabel.length())
        o_result << "#" << i_localLabel;
}

void
Position::Get_LinkToRoot( StreamStr & o_result ) const
{
    o_result << get_UpLink(Depth());
}

void
Position::Set( Node &           i_node,
               const String &   i_file )
{
    sFile = i_file;
    pDirectory = &i_node;
}




const char *
get_UpLink(uintt i_depth)
{
    static const uintt
        C_nMaxDepth = 30;
    static const char
        C_sUpLinkArray[3*C_nMaxDepth+1] =
                        "../../../../../../../../../../"
                        "../../../../../../../../../../"
                        "../../../../../../../../../../";
    static const char *
        C_sUpLink = &C_sUpLinkArray[0];

    if ( i_depth <= C_nMaxDepth )
    {
        return C_sUpLink + 3*(C_nMaxDepth - i_depth);
    }
    else
    {   // not THREAD fast
        static std::vector<char>
            aRet;
        uintt nNeededSize = i_depth * 3 + 1;

        if (aRet.size() < nNeededSize)
        {
            aRet.resize(nNeededSize);
            char * pEnd = &aRet[nNeededSize-1];
            *pEnd = '\0';

            for ( char * pFill = &(*aRet.begin());
                  pFill != pEnd;
                  pFill += 3 )
            {
                memcpy(pFill, C_sUpLink, 3);
            }
        }   // end if

        return &aRet[aRet.size() - 1 - 3*i_depth];
    }
}




}   // namespace output

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
