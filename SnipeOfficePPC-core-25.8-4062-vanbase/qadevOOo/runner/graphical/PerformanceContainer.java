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

package graphical;

import java.io.File;
// import java.io.FileWriter;
// import java.io.OutputStream;
// import java.io.PrintStream;
import java.io.PrintStream;
import java.io.RandomAccessFile;
// import java.io.StringWriter;
// import java.lang.Double;


public class PerformanceContainer /* extends *//* implements */ {
    /*
      simple helper functions to start/stop a timer, to know how long a process need in milliseconds
     */
    public long getStartTime()
        {
            return System.currentTimeMillis();
        }
    public void setStartTime(long _nStartTime)
        {
        }

    /*
      return the time, which is done until last startTime()
     */
    private long meanTime(long _nCurrentTimer)
        {
            if (_nCurrentTimer == 0)
            {
                GlobalLogWriter.println("Forgotten to initialise a start timer.");
                return 0;
            }
            long nMeanTime = System.currentTimeMillis();
            return nMeanTime - _nCurrentTimer;
        }

    /*
      public long stopTimer()
        {
            if (m_nStartTime == 0)
            {
                System.out.println("Forgotten to initialise start timer.");
                return 0;
            }
            long nStopTime = System.currentTimeMillis();
            return nStopTime - m_nStartTime;
        }
    */

    final static int Load = 0;
    final static int Store = 1;
    final static int Print = 2;
    final static int OfficeStart = 3;
    final static int StoreAsPDF = 4;
    final static int OfficeStop = 5;
    final static int AllTime = 6;
    final static int LAST_VALUE = 7; // THIS MUST BE ALWAYS THE LAST AND THE BIGGEST VALUE!

    private long m_nTime[];
    private String m_sMSOfficeVersion;

    public PerformanceContainer()
        {
            m_nTime = new long[LAST_VALUE];
            // @todo: is this need?
            for (int i=0;i<LAST_VALUE;i++)
            {
                m_nTime[i] = 0;
            }
        }

    public void setTime(int _nIndex, long _nValue)
        {
            m_nTime[_nIndex] = _nValue;
        }
    public long getTime(int _nIndex)
        {
            return m_nTime[_nIndex];
        }

    public void startTime(int _nIndex)
        {
            m_nTime[_nIndex] = getStartTime();
        }

    public void stopTime(int _nIndex)
        {
            m_nTime[_nIndex] = meanTime(m_nTime[_nIndex]);
        }

    public String getMSOfficeVersion()
    {
        return m_sMSOfficeVersion;
    }

    public void print(PrintStream out)
        {
            // String ls = System.getProperty("line.separator");
            // out.
            out.println("loadtime="        + String.valueOf(m_nTime[ Load ]));
            out.println("storetime="       + String.valueOf(m_nTime[ Store ]));
            out.println("printtime="       + String.valueOf(m_nTime[ Print ]));
            out.println("officestarttime=" + String.valueOf(m_nTime[ OfficeStart ]));
            out.println("officestoptime="  + String.valueOf(m_nTime[ OfficeStop ]));
            out.println("storeaspdftime="  + String.valueOf(m_nTime[ StoreAsPDF ]));
            out.println("alltime="         + String.valueOf(m_nTime[ AllTime ]));
        }

    public void print(IniFile _aIniFile, String _sSection)
        {
            // String ls = System.getProperty("line.separator");
            // out.
            _aIniFile.insertValue(_sSection, "loadtime"        , String.valueOf(m_nTime[ Load ]));
            _aIniFile.insertValue(_sSection, "storetime"       , String.valueOf(m_nTime[ Store ]));
            _aIniFile.insertValue(_sSection, "printtime"       , String.valueOf(m_nTime[ Print ]));
            _aIniFile.insertValue(_sSection, "officestarttime" , String.valueOf(m_nTime[ OfficeStart ]));
            _aIniFile.insertValue(_sSection, "officestoptime"  , String.valueOf(m_nTime[ OfficeStop ]));
            _aIniFile.insertValue(_sSection, "storeaspdftime"  , String.valueOf(m_nTime[ StoreAsPDF ]));
            _aIniFile.insertValue(_sSection, "alltime"         , String.valueOf(m_nTime[ AllTime ]));
        }

    public static double stringToDouble(String _sStr)
        {
            double nValue = 0;
            try
            {
                nValue = Double.parseDouble( _sStr );
            }
            catch (NumberFormatException e)
            {
                GlobalLogWriter.println("Can't convert string to double " + _sStr);
            }
            return nValue;
        }

    public static long secondsToMilliSeconds(double _nSeconds)
        {
            return (long)(_nSeconds * 1000.0);
        }

    /*
      Helper function, which read some values from a given file

      sample of wordinfofile
    name=c:\doc-pool\wntmsci\samples\msoffice\word\LineSpacing.doc
    WordVersion=11.0
    WordStartTime=0.340490102767944
    WordLoadTime=0.650935888290405
    WordPrintTime=0.580835103988647
     */
    public void readWordValuesFromFile(String sFilename)
        {
            File aFile = new File(sFilename);
            if (! aFile.exists())
            {
                GlobalLogWriter.println("couldn't find file " + sFilename);
                return;
            }

            RandomAccessFile aRandomAccessFile = null;
            try
            {
                aRandomAccessFile = new RandomAccessFile(aFile,"r");
                String sLine = "";
                while (sLine != null)
                {
                    sLine = aRandomAccessFile.readLine();
                    if ( (sLine != null) &&
                         (! (sLine.length() < 2) ) &&
                         (! sLine.startsWith("#")))
                    {
                        if (sLine.startsWith("WordStartTime="))
                        {
                            String sTime = sLine.substring(14);
                            m_nTime[OfficeStart] = secondsToMilliSeconds(stringToDouble(sTime));
                        }
                        else if (sLine.startsWith("WordLoadTime="))
                        {
                            String sTime = sLine.substring(13);
                            m_nTime[Load] = secondsToMilliSeconds(stringToDouble(sTime));
                        }
                        else if (sLine.startsWith("WordPrintTime="))
                        {
                            String sTime = sLine.substring(14);
                            m_nTime[Print] = secondsToMilliSeconds(stringToDouble(sTime));
                        }
                        else if (sLine.startsWith("WordVersion="))
                        {
                            String sMSOfficeVersion = sLine.substring(12);
                            m_sMSOfficeVersion = "Word:" + sMSOfficeVersion;
                        }
                        else if (sLine.startsWith("ExcelVersion="))
                        {
                            String sMSOfficeVersion = sLine.substring(13);
                            m_sMSOfficeVersion = "Excel:" + sMSOfficeVersion;
                        }
                        else if (sLine.startsWith("PowerPointVersion="))
                        {
                            String sMSOfficeVersion = sLine.substring(18);
                            m_sMSOfficeVersion = "PowerPoint:" + sMSOfficeVersion;
                        }
                    }
                }
            }
            catch (java.io.FileNotFoundException fne)
            {
                GlobalLogWriter.println("couldn't open file " + sFilename);
                GlobalLogWriter.println("Message: " + fne.getMessage());
            }
            catch (java.io.IOException ie)
            {
                GlobalLogWriter.println("Exception while reading file " + sFilename);
                GlobalLogWriter.println("Message: " + ie.getMessage());
            }
            try
            {
                aRandomAccessFile.close();
            }
            catch (java.io.IOException ie)
            {
                GlobalLogWriter.println("Couldn't close file " + sFilename);
                GlobalLogWriter.println("Message: " + ie.getMessage());
            }
        }

//    public static void main(String[] args)
//    {
//        BorderRemover a = new BorderRemover();
//        try
//        {
//            a.createNewImageWithoutBorder(args[0], args[1]);
//        }
//        catch(java.io.IOException e)
//        {
//            System.out.println("Exception caught.");
//        }
//
//    }
}
