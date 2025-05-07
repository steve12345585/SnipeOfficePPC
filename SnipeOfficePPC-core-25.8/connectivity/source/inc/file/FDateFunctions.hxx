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

#ifndef _CONNECTIVITY_FILE_FDATEFUNCTIONS_HXX_
#define _CONNECTIVITY_FILE_FDATEFUNCTIONS_HXX_

#include "file/fcode.hxx"
#include "file/filedllapi.hxx"

namespace connectivity
{
    class OSQLParseNode;
    namespace file
    {
        /** DAYOFWEEK(date)
            Returns the weekday index for date (1 = Sunday, 2 = Monday, ... 7 = Saturday). These index values correspond to the ODBC standard.

        > SELECT DAYOFWEEK('1998-02-03');
                -> 3
        */
        class OOp_DayOfWeek : public OUnaryOperator
        {
        protected:
            virtual ORowSetValue operate(const ORowSetValue& lhs) const;
        };

        /** DAYOFMONTH(date)
            Returns the day of the month for date, in the range 1 to 31:

        > SELECT DAYOFMONTH('1998-02-03');
                -> 3
        */
        class OOp_DayOfMonth : public OUnaryOperator
        {
        protected:
            virtual ORowSetValue operate(const ORowSetValue& lhs) const;
        };

        /** DAYOFYEAR(date)
            Returns the day of the year for date, in the range 1 to 366:

        > SELECT DAYOFYEAR('1998-02-03');
                -> 34

        */
        class OOp_DayOfYear : public OUnaryOperator
        {
        protected:
            virtual ORowSetValue operate(const ORowSetValue& lhs) const;
        };

        /** MONTH(date)
            Returns the month for date, in the range 1 to 12:

        > SELECT MONTH('1998-02-03');
                -> 2
        */
        class OOp_Month : public OUnaryOperator
        {
        protected:
            virtual ORowSetValue operate(const ORowSetValue& lhs) const;
        };

        /** DAYNAME(date)
            Returns the name of the weekday for date:

        > SELECT DAYNAME('1998-02-05');
                -> 'Thursday'

        */
        class OOp_DayName : public OUnaryOperator
        {
        protected:
            virtual ORowSetValue operate(const ORowSetValue& lhs) const;
        };

        /** MONTHNAME(date)
            Returns the name of the month for date:

        > SELECT MONTHNAME('1998-02-05');
                -> 'February'

        */
        class OOp_MonthName : public OUnaryOperator
        {
        protected:
            virtual ORowSetValue operate(const ORowSetValue& lhs) const;
        };

        /** QUARTER(date)
            Returns the quarter of the year for date, in the range 1 to 4:

        > SELECT QUARTER('98-04-01');
                -> 2

        */
        class OOp_Quarter : public OUnaryOperator
        {
        protected:
            virtual ORowSetValue operate(const ORowSetValue& lhs) const;
        };

        /** WEEK(date)
            WEEK(date,first)
                With a single argument, returns the week for date, in the range 0 to 53 (yes, there may be the beginnings of a week 53), for locations where Sunday is the first day of the week. The two-argument form of WEEK() allows you to specify whether the week starts on Sunday or Monday and whether the return value should be in the range 0-53 or 1-52. Here is a table for how the second argument works:
                Value   Meaning
                0   Week starts on Sunday and return value is in range 0-53
                1   Week starts on Monday and return value is in range 0-53
                2   Week starts on Sunday and return value is in range 1-53
                3   Week starts on Monday and return value is in range 1-53 (ISO 8601)

            > SELECT WEEK('1998-02-20');
                    -> 7
            > SELECT WEEK('1998-02-20',0);
                    -> 7
            > SELECT WEEK('1998-02-20',1);
                    -> 8
            > SELECT WEEK('1998-12-31',1);
                    -> 53

        */
        class OOp_Week : public ONthOperator
        {
        protected:
            virtual ORowSetValue operate(const ::std::vector<ORowSetValue>& lhs) const;
        };

        /** YEAR(date)
            Returns the year for date, in the range 1000 to 9999:

        > SELECT YEAR('98-02-03');
                -> 1998
        */
        class OOp_Year : public OUnaryOperator
        {
        protected:
            virtual ORowSetValue operate(const ORowSetValue& lhs) const;
        };

        /** HOUR(time)
            Returns the hour for time, in the range 0 to 23:

        > SELECT HOUR('10:05:03');
                -> 10
        */
        class OOp_Hour : public OUnaryOperator
        {
        protected:
            virtual ORowSetValue operate(const ORowSetValue& lhs) const;
        };

        /** MINUTE(time)
            Returns the minute for time, in the range 0 to 59:

        > SELECT MINUTE('98-02-03 10:05:03');
                -> 5

        */
        class OOp_Minute : public OUnaryOperator
        {
        protected:
            virtual ORowSetValue operate(const ORowSetValue& lhs) const;
        };

        /** SECOND(time)
            Returns the second for time, in the range 0 to 59:

        > SELECT SECOND('10:05:03');
                -> 3
        */
        class OOp_Second : public OUnaryOperator
        {
        protected:
            virtual ORowSetValue operate(const ORowSetValue& lhs) const;
        };

        /** CURDATE()
            CURRENT_DATE
                Returns today's date as a value in 'YYYY-MM-DD' or YYYYMMDD format, depending on whether the function is used in a string or numeric context:

            > SELECT CURDATE();
                    -> '1997-12-15'
        */
        class OOp_CurDate : public ONthOperator
        {
        protected:
            virtual ORowSetValue operate(const ::std::vector<ORowSetValue>& lhs) const;
        };

        /** CURTIME()
            CURRENT_TIME
                Returns the current time as a value in 'HH:MM:SS' or HHMMSS format, depending on whether the function is used in a string or numeric context:

            > SELECT CURTIME();
                    -> '23:50:26'
        */
        class OOp_CurTime : public ONthOperator
        {
        protected:
            virtual ORowSetValue operate(const ::std::vector<ORowSetValue>& lhs) const;
        };

        /** NOW()
            Returns the current date and time as a value in 'YYYY-MM-DD HH:MM:SS' or YYYYMMDDHHMMSS format, depending on whether the function is used in a string or numeric context:

            > SELECT NOW();
                    -> '1997-12-15 23:50:26'
        */
        class OOp_Now : public ONthOperator
        {
        protected:
            virtual ORowSetValue operate(const ::std::vector<ORowSetValue>& lhs) const;
        };
    }
}

#endif // _CONNECTIVITY_FILE_FCODE_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
