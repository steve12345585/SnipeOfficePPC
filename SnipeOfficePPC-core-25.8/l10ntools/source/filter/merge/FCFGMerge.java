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

package com.sun.star.filter.config.tools.merge;

//_______________________________________________

import java.lang.*;
import java.util.*;
import java.io.*;
import com.sun.star.filter.config.tools.utils.*;

//_______________________________________________

/**
 *  Its a simple command line tool, which can merge different XML fragments
 *  together. Such fragments must exist as files on disk, will be moved into
 *  one file together on disk.
 *
 *
 */
public class FCFGMerge
{
    //___________________________________________
    // const

    private static final java.lang.String CFGFILE = "com/sun/star/filter/config/tools/merge/FCFGMerge.cfg";
    private static final java.lang.String PROP_LOGLEVEL = "loglevel";

    //___________________________________________
    // main

    public static void main(java.lang.String[] sCmdLine)
    {
        // create log object in default mode "WARNINGS"
        // If a command line parameter indicates another
        // level - change it!
        Logger aLog = new Logger();

        try
        {
            // merge config file and overwrite properties
            // via command line
            ConfigHelper aCfg = null;
            aCfg = new ConfigHelper(CFGFILE, sCmdLine);

            // update log level
            int nLogLevel = aCfg.getInt(PROP_LOGLEVEL, Logger.LEVEL_WARNINGS);
            aLog = new Logger(nLogLevel);

            // help requested?
            if (aCfg.isHelp())
            {
                FCFGMerge.printCopyright();
                FCFGMerge.printHelp();
                System.exit(-1);
            }

            // create new merge object and start operation
            Merger aMerger = new Merger(aCfg, aLog);
            aMerger.merge();
        }
        catch(java.lang.Throwable ex)
        {
            aLog.setException(ex);
            System.exit(-1);
        }

        System.exit(0);
    }

    //___________________________________________

    /** prints out a copyright message on stdout.
     */
    private static void printCopyright()
    {
        java.lang.StringBuffer sOut = new java.lang.StringBuffer(256);
        sOut.append("FCFGMerge (LibreOffice)\n\n");
        sOut.append("Copyright (C) 2003 Sun Microsystems, Inc.\n");
        sOut.append("Copyright (C) 2012 The Document Foundation\n");
        sOut.append("License LGPLv3: GNU LGPL version 3 <http://www.gnu.org/licenses/lgpl.html>.\n");
        sOut.append("This is free software: you are free to change and redistribute it.\n");
        sOut.append("There is NO WARRANTY, to the extent permitted by law.\n");
        System.out.println(sOut.toString());
    }

    //___________________________________________

    /** prints out a help message on stdout.
     */
    private static void printHelp()
    {
        java.lang.StringBuffer sOut = new java.lang.StringBuffer(256);
        sOut.append("____________________________________________________________\n");
        sOut.append("usage: FCFGMerge cfg=<file name>\n"                            );
        sOut.append("parameters:\n"                                                 );
        sOut.append("\tcfg=<file name>\n"                                           );
        sOut.append("\t\tmust point to a system file, which contains\n"             );
        sOut.append("\t\tall neccessary configuration data for the merge process.\n");
        sOut.append("\n\tFurther cou can specify every parameter allowed in the\n"  );
        sOut.append("\n\tconfig file as command line parameter too, to overwrite\n" );
        sOut.append("\n\tthe value from the file.\n"                                );
        System.out.println(sOut.toString());
    }
}
