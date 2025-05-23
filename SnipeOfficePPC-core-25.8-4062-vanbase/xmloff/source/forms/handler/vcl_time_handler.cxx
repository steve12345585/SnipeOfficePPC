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


#include "vcl_time_handler.hxx"

#include <rtl/ustrbuf.hxx>

#include <com/sun/star/util/Duration.hpp>

#include <sax/tools/converter.hxx>

#include <tools/diagnose_ex.h>
#include <tools/time.hxx>

//......................................................................................................................
namespace xmloff
{
//......................................................................................................................

    using ::com::sun::star::uno::Any;
    using ::com::sun::star::uno::makeAny;
    using ::com::sun::star::util::Duration;

    //==================================================================================================================
    //= VCLTimeHandler
    //==================================================================================================================
    //------------------------------------------------------------------------------------------------------------------
    VCLTimeHandler::VCLTimeHandler()
    {
    }

    //------------------------------------------------------------------------------------------------------------------
    ::rtl::OUString VCLTimeHandler::getAttributeValue( const PropertyValues& /*i_propertyValues*/ ) const
    {
        OSL_ENSURE( false, "VCLTimeHandler::getAttributeValue: unexpected call!" );
        return ::rtl::OUString();
    }

    //------------------------------------------------------------------------------------------------------------------
    ::rtl::OUString VCLTimeHandler::getAttributeValue( const Any& i_propertyValue ) const
    {
        sal_Int32 nVCLTime(0);
        OSL_VERIFY( i_propertyValue >>= nVCLTime );
        ::Time aVCLTime( nVCLTime );

        Duration aDuration; // default-inited to 0
        aDuration.Hours = aVCLTime.GetHour();
        aDuration.Minutes = aVCLTime.GetMin();
        aDuration.Seconds = aVCLTime.GetSec();
        aDuration.MilliSeconds = aVCLTime.Get100Sec() * 10;

        ::rtl::OUStringBuffer aBuffer;
        ::sax::Converter::convertDuration( aBuffer, aDuration );
        return aBuffer.makeStringAndClear();
    }

    //------------------------------------------------------------------------------------------------------------------
    bool VCLTimeHandler::getPropertyValues( const ::rtl::OUString i_attributeValue, PropertyValues& o_propertyValues ) const
    {
        sal_Int32 nVCLTime(0);

        Duration aDuration;
        if (::sax::Converter::convertDuration( aDuration, i_attributeValue ))
        {
            ::Time aVCLTime(aDuration.Hours, aDuration.Minutes,
                    aDuration.Seconds, aDuration.MilliSeconds / 10);
            nVCLTime = aVCLTime.GetTime();
        }
        else
        {
            // compatibility format, before we wrote those values in XML-schema compatible form
            if (!::sax::Converter::convertNumber(nVCLTime, i_attributeValue))
            {
                OSL_ENSURE( false, "VCLTimeHandler::getPropertyValues: unknown time format (no XML-schema time, no legacy integer)!" );
                return false;
            }
        }

        const Any aPropertyValue( makeAny( nVCLTime ) );

        OSL_ENSURE( o_propertyValues.size() == 1, "VCLTimeHandler::getPropertyValues: time strings represent exactly one property - not more, not less!" );
        for (   PropertyValues::iterator prop = o_propertyValues.begin();
                prop != o_propertyValues.end();
                ++prop
            )
        {
            prop->second = aPropertyValue;
        }
        return true;
    }

//......................................................................................................................
} // namespace xmloff
//......................................................................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
