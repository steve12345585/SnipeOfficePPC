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

package com.sun.star.filter.config.tools.utils;

//_______________________________________________
// imports
import java.util.*;
import java.lang.*;

//_______________________________________________
// implementation
public class AnalyzeStartupLog
{
    private class OperationTime
    {
        /** name the measured operation. */
        public java.lang.String sOperation;

        /** contains the time value, when this operation was started. */
        public long nStartTime;

        /** contains the time value, when this operation was finished. */
        public long nEndTime;

        /** text inside log file, which identifies the start time value. */
        public java.lang.String sStartMsg;

        /** text inside log file, which identifies the end time value. */
        public java.lang.String sEndMsg;
    }

    //_________________________________
    // main

    public static void main(java.lang.String[] lCmdLine)
    {
        int nExit = 0;
        try
        {
            // analyze command line
            ConfigHelper aCmdLine  = new ConfigHelper("", lCmdLine);
            java.lang.String        sLogDir   = aCmdLine.getString("logdir"  );
            java.lang.String        sDataFile = aCmdLine.getString("datafile");

            if (sLogDir == null || sDataFile == null)
            {
                System.err.println("AnalyzeStartupLog lodir=<dir> datafile=<file>");
                System.err.println("E.g.: AnalyzeStartupLog lodir=c:\\temp\\logs datafile=c:\\temp\\data.csv");
                System.exit(--nExit);
            }

            // get list of all log files
            boolean bRecursive = true;
            java.util.Vector lLogs = FileHelper.getSystemFilesFromDir(new java.io.File(sLogDir), bRecursive);
            if (lLogs == null || lLogs.isEmpty())
            {
                System.err.println("log dir is empty");
                System.exit(--nExit);
            }

            // analyze it
            java.lang.StringBuffer sOut = new java.lang.StringBuffer(1000);
            sOut.append("log;t_cfg_start;t_cfg_end;t_fwk_start;t_fwk_end;t_sfx_start;t_sfx_end;t_types_start;t_types_end;t_filters_start;t_filters_end;");
            sOut.append("t_filters_swriter_start;t_filters_swriter_end;t_filters_sweb_start;t_filters_sweb_end;t_filters_sglobal_start;t_filters_sglobal_end;t_filters_scalc_start;t_filters_scalc_end;t_filters_sdraw_start;t_filters_sdraw_end;t_filters_simpress_start;t_filters_simpress_end;t_filters_schart_start;t_filters_schart_end;t_filters_smath_start;t_filters_smath_end;");
            sOut.append("t_others_start;t_others_end;d_cfg;d_fwk;d_sfx;d_types;d_filters;d_others;d_complete\n");

            java.util.Enumeration aIt = lLogs.elements();
            while (aIt.hasMoreElements())
            {
                java.io.File           aLog    = (java.io.File)aIt.nextElement();
                java.io.FileReader     aReader = new java.io.FileReader(aLog);
                java.io.BufferedReader aBuffer = new java.io.BufferedReader(aReader);

                long t_cfg_start                = 0;
                long t_cfg_end                  = 0;

                long t_fwk_start                = 0;
                long t_fwk_end                  = 0;

                long t_sfx_start                = 0;
                long t_sfx_end                  = 0;

                long t_types_start              = 0;
                long t_types_end                = 0;

                long t_filters_start            = 0;
                long t_filters_end              = 0;

                long t_filters_swriter_start    = 0;
                long t_filters_swriter_end      = 0;

                long t_filters_sweb_start       = 0;
                long t_filters_sweb_end         = 0;

                long t_filters_sglobal_start    = 0;
                long t_filters_sglobal_end      = 0;

                long t_filters_scalc_start      = 0;
                long t_filters_scalc_end        = 0;

                long t_filters_sdraw_start      = 0;
                long t_filters_sdraw_end        = 0;

                long t_filters_simpress_start   = 0;
                long t_filters_simpress_end     = 0;

                long t_filters_schart_start     = 0;
                long t_filters_schart_end       = 0;

                long t_filters_smath_start      = 0;
                long t_filters_smath_end        = 0;

                long t_others_start             = 0;
                long t_others_end               = 0;

                while (true)
                {
                    java.lang.String sLine = aBuffer.readLine();
                    if (sLine == null)
                        break;

                    if (sLine.endsWith("| framework (as96863) ::FilterCache::FilterCache : { creation ConfigItem [file=standard, version=6, mode=3]"))
                        t_cfg_start = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("| framework (as96863) ::FilterCache::FilterCache : } creation ConfigItem"))
                        t_cfg_end = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("| framework (as96863) ::FilterCache::FilterCache : { reading TypeDetection.xml"))
                        t_fwk_start = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("| framework (as96863) ::FilterCache::FilterCache : } reading TypeDetection.xml"))
                        t_fwk_end = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("{ framework (as96863) ::FilterCFGAccess::impl_loadTypes"))
                        t_types_start = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("} framework (as96863) ::FilterCFGAccess::impl_loadTypes"))
                        t_types_end = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("{ framework (as96863) ::FilterCFGAccess::impl_loadFilters"))
                        t_filters_start = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("} framework (as96863) ::FilterCFGAccess::impl_loadFilters"))
                        t_filters_end = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("{ reading FilterGroup [swriter]"))
                        t_filters_swriter_start = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("} reading FilterGroup [swriter]"))
                        t_filters_swriter_end = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("{ reading FilterGroup [sweb]"))
                        t_filters_sweb_start = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("} reading FilterGroup [sweb]"))
                        t_filters_sweb_end = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("{ reading FilterGroup [sglobal]"))
                        t_filters_sglobal_start = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("} reading FilterGroup [sglobal]"))
                        t_filters_sglobal_end = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("{ reading FilterGroup [scalc]"))
                        t_filters_scalc_start = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("} reading FilterGroup [scalc]"))
                        t_filters_scalc_end = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("{ reading FilterGroup [sdraw]"))
                        t_filters_sdraw_start = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("} reading FilterGroup [sdraw]"))
                        t_filters_sdraw_end = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("{ reading FilterGroup [simpress]"))
                        t_filters_simpress_start = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("} reading FilterGroup [simpress]"))
                        t_filters_simpress_end = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("{ reading FilterGroup [schart]"))
                        t_filters_schart_start = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("} reading FilterGroup [schart]"))
                        t_filters_schart_end = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("{ reading FilterGroup [smath]"))
                        t_filters_smath_start = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("} reading FilterGroup [smath]"))
                        t_filters_smath_end = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("{ framework (as96863) ::FilterCFGAccess::impl_loadDetectors"))
                        t_others_start = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("} framework (as96863) ::FilterCFGAccess::impl_loadContentHandlers"))
                        t_others_end = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("} framework (as96863) ::FilterCache::FilterCache"))
                        t_sfx_start = new java.lang.Long(sLine.substring(0, 6)).longValue();
                    else
                    if (sLine.endsWith("} desktop (lo119109) OfficeWrapper::OfficeWrapper"))
                        t_sfx_end = new java.lang.Long(sLine.substring(0, 6)).longValue();
                }

                sOut.append(aLog.getName()               );
                sOut.append(";"                          );
                sOut.append(t_cfg_start                  );
                sOut.append(";"                          );
                sOut.append(t_cfg_end                    );
                sOut.append(";"                          );
                sOut.append(t_fwk_start                  );
                sOut.append(";"                          );
                sOut.append(t_fwk_end                    );
                sOut.append(";"                          );
                sOut.append(t_sfx_start                  );
                sOut.append(";"                          );
                sOut.append(t_sfx_end                    );
                sOut.append(";"                          );
                sOut.append(t_types_start                );
                sOut.append(";"                          );
                sOut.append(t_types_end                  );
                sOut.append(";"                          );
                sOut.append(t_filters_start              );
                sOut.append(";"                          );
                sOut.append(t_filters_end                );
                sOut.append(";"                          );

                sOut.append(t_filters_swriter_start      );
                sOut.append(";"                          );
                sOut.append(t_filters_swriter_end        );
                sOut.append(";"                          );
                sOut.append(t_filters_sweb_start         );
                sOut.append(";"                          );
                sOut.append(t_filters_sweb_end           );
                sOut.append(";"                          );
                sOut.append(t_filters_sglobal_start      );
                sOut.append(";"                          );
                sOut.append(t_filters_sglobal_end        );
                sOut.append(";"                          );
                sOut.append(t_filters_scalc_start        );
                sOut.append(";"                          );
                sOut.append(t_filters_scalc_end          );
                sOut.append(";"                          );
                sOut.append(t_filters_sdraw_start        );
                sOut.append(";"                          );
                sOut.append(t_filters_sdraw_end          );
                sOut.append(";"                          );
                sOut.append(t_filters_simpress_start     );
                sOut.append(";"                          );
                sOut.append(t_filters_simpress_end       );
                sOut.append(";"                          );
                sOut.append(t_filters_schart_start       );
                sOut.append(";"                          );
                sOut.append(t_filters_schart_end         );
                sOut.append(";"                          );
                sOut.append(t_filters_smath_start        );
                sOut.append(";"                          );
                sOut.append(t_filters_smath_end          );
                sOut.append(";"                          );

                sOut.append(t_others_start               );
                sOut.append(";"                          );
                sOut.append(t_others_end                 );
                sOut.append(";"                          );
                sOut.append(t_cfg_end    -t_cfg_start    );
                sOut.append(";"                          );
                sOut.append(t_fwk_end    -t_fwk_start    );
                sOut.append(";"                          );
                sOut.append(t_sfx_end    -t_sfx_start    );
                sOut.append(";"                          );
                sOut.append(t_types_end  -t_types_start  );
                sOut.append(";"                          );
                sOut.append(t_filters_end-t_filters_start);
                sOut.append(";"                          );
                sOut.append(t_others_end -t_others_start );
                sOut.append(";"                          );
                sOut.append(t_others_end -t_cfg_start    );
                sOut.append("\n"                         );

                aBuffer.close();
            }

            java.io.FileWriter aCSV  = new java.io.FileWriter(sDataFile);
            java.lang.String   sData = sOut.toString();
            aCSV.write(sData, 0, sData.length());
            aCSV.flush();
            aCSV.close();
        }
        catch(java.lang.Throwable exAny)
        {
            System.err.println(exAny.getMessage());
            exAny.printStackTrace();
            System.exit(--nExit);
        }

        System.exit(0);
    }
}
