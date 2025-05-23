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
#include "cmd_run.hxx"


// NOT FULLY DEFINED SERVICES
#include <cosv/file.hxx>
#include <cosv/x.hxx>
#include <ary/ary.hxx>
#include <ary/idl/i_ce.hxx>
#include <ary/idl/i_gate.hxx>
#include <ary/idl/i_module.hxx>
#include <ary/idl/ip_ce.hxx>
#include <autodoc/filecoli.hxx>
#include <autodoc/parsing.hxx>
#include <autodoc/prs_docu.hxx>
#include <parser/unoidl.hxx>
#include <adc_cl.hxx>
#include "adc_cmd_parse.hxx"
#include "adc_cmds.hxx"

namespace autodoc
{
namespace command
{
namespace run
{

Parser::Parser( const Parse & i_command )
    :   rCommand(i_command),
        pCppDocuInterpreter(),
        pIdlParser()
{
}

Parser::~Parser()
{
}

bool
Parser::Perform()
{
    Cout() << "Parsing the repository "
              << rCommand.ReposyName()
              << " ..."
              << Endl();
  try
  {
    ::ary::Repository &
        rAry = CommandLine::Get_().TheRepository();
    rAry.Set_Title(rCommand.ReposyName());

    Dyn< FileCollector_Ifc >
        pFiles( ParseToolsFactory().Create_FileCollector(6000) );

    bool bIDL = false;

    command::Parse::ProjectIterator
        itEnd = rCommand.ProjectsEnd();
    for ( command::Parse::ProjectIterator it = rCommand.ProjectsBegin();
          it != itEnd;
          ++it )
    {
        uintt nCount = GatherFiles( *pFiles, *(*it) );
        Cout() << nCount
             << " files found to parse in project "
             << (*it)->Name()
             << "."
             << Endl();

        switch ( (*it)->Language().eLanguage )
        {
            case command::S_LanguageInfo::idl:
            {
                Get_IdlParser().Run(*pFiles);
                bIDL = true;
            }   break;
            default:
                Cerr() << "Project in yet unimplemented language skipped."
                       << Endl();
        }
    }   // end for

    if (bIDL)
    {
        rAry.Gate_Idl().Calculate_AllSecondaryInformation(
                            rCommand.DevelopersManual_RefFilePath() );

//        ::ary::idl::SecondariesPilot &
//            rIdl2sPilot = rAry.Gate_Idl().Secondaries();
//
//        rIdl2sPilot.CheckAllInterfaceBases( rAry.Gate_Idl() );
//        rIdl2sPilot.Connect_Types2Ces();
//        rIdl2sPilot.Gather_CrossReferences();
//
//        if (NOT rCommand.DevelopersManual_RefFilePath().empty())
//        {
//            csv::File
//                aFile(rCommand.DevelopersManual_RefFilePath(), csv::CFM_READ);
//            if ( aFile.open() )
//            {
//                rIdl2sPilot.Read_Links2DevManual(aFile);
//              aFile.close();
//            }
//        }
    }   // endif (bIDL)

    return true;

  }   // end try
  catch (csv::Exception & xx)
  {
    xx.GetInfo(Cerr());
    Cerr() << " program will exit." << Endl();

    return false;
  }
}

IdlParser &
Parser::Get_IdlParser()
{
    if ( NOT pIdlParser )
        Create_IdlParser();
    return *pIdlParser;
}

void
Parser::Create_IdlParser()
{
    pIdlParser = new IdlParser(CommandLine::Get_().TheRepository());
}

const ParseToolsFactory_Ifc &
Parser::ParseToolsFactory()
{
    return ParseToolsFactory_Ifc::GetIt_();
}

uintt
Parser::GatherFiles( FileCollector_Ifc &    o_rFiles,
                     const S_ProjectData &  i_rProject )
{
    uintt ret = 0;
    o_rFiles.EraseAll();

    typedef StringVector                StrVector;
    typedef StrVector::const_iterator   StrIterator;
    const S_Sources &
        rSources = i_rProject.Sources();
    const StrVector &
        rExtensions = i_rProject.Language().aExtensions;

    StrIterator     it;
    StrIterator     itTreesEnd  = rSources.aTrees.end();
    StrIterator     itDirsEnd   = rSources.aDirectories.end();
    StrIterator     itFilesEnd  = rSources.aFiles.end();
    StrIterator     itExt;
    StrIterator     itExtEnd    = rExtensions.end();

    csv::StreamStr aDir(500);
    i_rProject.RootDirectory().Get( aDir );

    uintt nProjectDir_AddPosition =
            ( strcmp(aDir.c_str(),".\\") == 0 OR strcmp(aDir.c_str(),"./") == 0 )
                ?   0
                :   uintt( aDir.tellp() );

    for ( it = rSources.aDirectories.begin();
          it != itDirsEnd;
          ++it )
    {
        aDir.seekp( nProjectDir_AddPosition );
        aDir << *it;

        for ( itExt = rExtensions.begin();
              itExt != itExtEnd;
              ++itExt )
        {
            ret += o_rFiles.AddFilesFrom( aDir.c_str(),
                                          *itExt,
                                          FileCollector_Ifc::flat );
        }   // end for itExt
    }   // end for it
    for ( it = rSources.aTrees.begin();
          it != itTreesEnd;
          ++it )
    {
        aDir.seekp( nProjectDir_AddPosition );
        aDir << *it;

        for ( itExt = rExtensions.begin();
              itExt != itExtEnd;
              ++itExt )
        {
            ret += o_rFiles.AddFilesFrom( aDir.c_str(),
                                          *itExt,
                                          FileCollector_Ifc::recursive );
        }   // end for itExt
    }   // end for it
    for ( it = rSources.aFiles.begin();
          it != itFilesEnd;
          ++it )
    {
        aDir.seekp( nProjectDir_AddPosition );
        aDir << *it;

        o_rFiles.AddFile( aDir.c_str() );
    }   // end for it
    ret += rSources.aFiles.size();

    return ret;
}


}   // namespace run
}   // namespace command
}   // namespace autodoc




/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
