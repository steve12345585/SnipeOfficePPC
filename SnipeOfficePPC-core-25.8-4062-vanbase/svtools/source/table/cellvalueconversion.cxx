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

#include "cellvalueconversion.hxx"

#include <com/sun/star/util/NumberFormatter.hpp>
#include <com/sun/star/util/XNumberFormatsSupplier.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/util/Date.hpp>
#include <com/sun/star/util/Time.hpp>
#include <com/sun/star/util/DateTime.hpp>
#include <com/sun/star/util/XNumberFormatTypes.hpp>
#include <com/sun/star/util/NumberFormat.hpp>
#include <comphelper/componentcontext.hxx>
#include <rtl/strbuf.hxx>
#include <rtl/math.hxx>
#include <tools/date.hxx>
#include <tools/time.hxx>
#include <tools/diagnose_ex.h>
#include <unotools/syslocale.hxx>

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

//......................................................................................................................
namespace svt
{
//......................................................................................................................

    /** === begin UNO using === **/
    using ::com::sun::star::uno::Any;
    using ::com::sun::star::util::XNumberFormatter;
    using ::com::sun::star::util::XNumberFormatter2;
    using ::com::sun::star::util::NumberFormatter;
    using ::com::sun::star::uno::UNO_QUERY_THROW;
    using ::com::sun::star::util::XNumberFormatsSupplier;
    using ::com::sun::star::beans::XPropertySet;
    using ::com::sun::star::uno::UNO_SET_THROW;
    using ::com::sun::star::uno::Exception;
    using ::com::sun::star::util::DateTime;
    using ::com::sun::star::uno::TypeClass;
    using ::com::sun::star::util::XNumberFormatTypes;
    using ::com::sun::star::uno::Reference;
    using ::com::sun::star::uno::Sequence;
    using ::com::sun::star::uno::makeAny;
    using ::com::sun::star::uno::Type;
    using ::com::sun::star::uno::TypeClass_BYTE;
    using ::com::sun::star::uno::TypeClass_SHORT;
    using ::com::sun::star::uno::TypeClass_UNSIGNED_SHORT;
    using ::com::sun::star::uno::TypeClass_LONG;
    using ::com::sun::star::uno::TypeClass_UNSIGNED_LONG;
    using ::com::sun::star::uno::TypeClass_HYPER;
    /** === end UNO using === **/
    namespace NumberFormat = ::com::sun::star::util::NumberFormat;

    typedef ::com::sun::star::util::Time UnoTime;
    typedef ::com::sun::star::util::Date UnoDate;

    //==================================================================================================================
    //= helper
    //==================================================================================================================
    namespace
    {
        //--------------------------------------------------------------------------------------------------------------
        double lcl_convertDateToDays( long const i_day, long const i_month, long const i_year )
        {
            long const nNullDateDays = ::Date::DateToDays( 1, 1, 1900 );
            long const nValueDateDays = ::Date::DateToDays( i_day, i_month, i_year );

            return nValueDateDays - nNullDateDays;
        }

        //--------------------------------------------------------------------------------------------------------------
        double lcl_convertTimeToDays( long const i_hours, long const i_minutes, long const i_seconds, long const i_100thSeconds )
        {
            return Time( i_hours, i_minutes, i_seconds, i_100thSeconds ).GetTimeInDays();
        }
    }

    //==================================================================================================================
    //= IValueNormalization
    //==================================================================================================================
    class SAL_NO_VTABLE IValueNormalization
    {
    public:
        virtual ~IValueNormalization() { }

        /** converts the given <code>Any</code> into a <code>double</code> value to be fed into a number formatter
        */
        virtual double convertToDouble( Any const & i_value ) const = 0;

        /** returns the format key to be used for formatting values
        */
        virtual ::sal_Int32 getFormatKey() const = 0;
    };

    typedef ::boost::shared_ptr< IValueNormalization > PValueNormalization;
    typedef ::boost::unordered_map< ::rtl::OUString, PValueNormalization, ::rtl::OUStringHash >    NormalizerCache;

    //==================================================================================================================
    //= CellValueConversion_Data
    //==================================================================================================================
    struct CellValueConversion_Data
    {
        ::comphelper::ComponentContext const    aContext;
        Reference< XNumberFormatter >           xNumberFormatter;
        bool                                    bAttemptedFormatterCreation;
        NormalizerCache                         aNormalizers;

        CellValueConversion_Data( ::comphelper::ComponentContext const & i_context )
            :aContext( i_context )
            ,xNumberFormatter()
            ,bAttemptedFormatterCreation( false )
            ,aNormalizers()
        {
        }
    };

    //==================================================================================================================
    //= StandardFormatNormalizer
    //==================================================================================================================
    class StandardFormatNormalizer : public IValueNormalization
    {
    protected:
        StandardFormatNormalizer( Reference< XNumberFormatter > const & i_formatter, ::sal_Int32 const i_numberFormatType )
            :m_nFormatKey( 0 )
        {
            try
            {
                ENSURE_OR_THROW( i_formatter.is(), "StandardFormatNormalizer: no formatter!" );
                Reference< XNumberFormatsSupplier > const xSupplier( i_formatter->getNumberFormatsSupplier(), UNO_SET_THROW );
                Reference< XNumberFormatTypes > const xTypes( xSupplier->getNumberFormats(), UNO_QUERY_THROW );
                m_nFormatKey = xTypes->getStandardFormat( i_numberFormatType, SvtSysLocale().GetLanguageTag().getLocale() );
            }
            catch( const Exception& )
            {
                DBG_UNHANDLED_EXCEPTION();
            }
        }

        virtual ::sal_Int32 getFormatKey() const
        {
            return m_nFormatKey;
        }

    private:
        ::sal_Int32 m_nFormatKey;
    };

    //==================================================================================================================
    //= DoubleNormalization
    //==================================================================================================================
    class DoubleNormalization : public StandardFormatNormalizer
    {
    public:
        DoubleNormalization( Reference< XNumberFormatter > const & i_formatter )
            :StandardFormatNormalizer( i_formatter, NumberFormat::NUMBER )
        {
        }

        virtual double convertToDouble( Any const & i_value ) const
        {
            double returnValue(0);
            ::rtl::math::setNan( &returnValue );
            OSL_VERIFY( i_value >>= returnValue );
            return returnValue;
        }

        virtual ~DoubleNormalization() { }
    };

    //==================================================================================================================
    //= IntegerNormalization
    //==================================================================================================================
    class IntegerNormalization : public StandardFormatNormalizer
    {
    public:
        IntegerNormalization( Reference< XNumberFormatter > const & i_formatter )
            :StandardFormatNormalizer( i_formatter, NumberFormat::NUMBER )
        {
        }

        virtual ~IntegerNormalization() {}

        virtual double convertToDouble( Any const & i_value ) const
        {
            sal_Int64 value( 0 );
            OSL_VERIFY( i_value >>= value );
            return value;
        }
    };

    //==================================================================================================================
    //= BooleanNormalization
    //==================================================================================================================
    class BooleanNormalization : public StandardFormatNormalizer
    {
    public:
        BooleanNormalization( Reference< XNumberFormatter > const & i_formatter )
            :StandardFormatNormalizer( i_formatter, NumberFormat::LOGICAL )
        {
        }

        virtual ~BooleanNormalization() {}

        virtual double convertToDouble( Any const & i_value ) const
        {
            bool value( false );
            OSL_VERIFY( i_value >>= value );
            return value ? 1 : 0;
        }
    };

    //==================================================================================================================
    //= DateTimeNormalization
    //==================================================================================================================
    class DateTimeNormalization : public StandardFormatNormalizer
    {
    public:
        DateTimeNormalization( Reference< XNumberFormatter > const & i_formatter )
            :StandardFormatNormalizer( i_formatter, NumberFormat::DATETIME )
        {
        }

        virtual ~DateTimeNormalization() {}

        virtual double convertToDouble( Any const & i_value ) const
        {
            double returnValue(0);
            ::rtl::math::setNan( &returnValue );

            // extract actual UNO value
            DateTime aDateTimeValue;
            ENSURE_OR_RETURN( i_value >>= aDateTimeValue, "allowed for DateTime values only", returnValue );

            // date part
            returnValue = lcl_convertDateToDays( aDateTimeValue.Day, aDateTimeValue.Month, aDateTimeValue.Year );

            // time part
            returnValue += lcl_convertTimeToDays(
                aDateTimeValue.Hours, aDateTimeValue.Minutes, aDateTimeValue.Seconds, aDateTimeValue.HundredthSeconds );

            // done
            return returnValue;
        }
    };

    //==================================================================================================================
    //= DateNormalization
    //==================================================================================================================
    class DateNormalization : public StandardFormatNormalizer
    {
    public:
        DateNormalization( Reference< XNumberFormatter > const & i_formatter )
            :StandardFormatNormalizer( i_formatter, NumberFormat::DATE )
        {
        }

        virtual ~DateNormalization() {}

        virtual double convertToDouble( Any const & i_value ) const
        {
            double returnValue(0);
            ::rtl::math::setNan( &returnValue );

            // extract
            UnoDate aDateValue;
            ENSURE_OR_RETURN( i_value >>= aDateValue, "allowed for Date values only", returnValue );

            // convert
            returnValue = lcl_convertDateToDays( aDateValue.Day, aDateValue.Month, aDateValue.Year );

            // done
            return returnValue;
        }
    };

    //==================================================================================================================
    //= TimeNormalization
    //==================================================================================================================
    class TimeNormalization : public StandardFormatNormalizer
    {
    public:
        TimeNormalization( Reference< XNumberFormatter > const & i_formatter )
            :StandardFormatNormalizer( i_formatter, NumberFormat::TIME )
        {
        }

        virtual ~TimeNormalization() {}

        virtual double convertToDouble( Any const & i_value ) const
        {
            double returnValue(0);
            ::rtl::math::setNan( &returnValue );

            // extract
            UnoTime aTimeValue;
            ENSURE_OR_RETURN( i_value >>= aTimeValue, "allowed for Time values only", returnValue );

            // convert
            returnValue += lcl_convertTimeToDays(
                aTimeValue.Hours, aTimeValue.Minutes, aTimeValue.Seconds, aTimeValue.HundredthSeconds );

            // done
            return returnValue;
        }
    };

    //==================================================================================================================
    //= operations
    //==================================================================================================================
    namespace
    {
        //--------------------------------------------------------------------------------------------------------------
        bool lcl_ensureNumberFormatter( CellValueConversion_Data & io_data )
        {
            if ( io_data.bAttemptedFormatterCreation )
                return io_data.xNumberFormatter.is();
            io_data.bAttemptedFormatterCreation = true;

            try
            {
                // a number formatter
                Reference< XNumberFormatter > const xFormatter( NumberFormatter::create( io_data.aContext.getUNOContext() ), UNO_QUERY_THROW );

                // a supplier of number formats
                Sequence< Any > aInitArgs(1);
                aInitArgs[0] <<= SvtSysLocale().GetLanguageTag().getLocale();

                Reference< XNumberFormatsSupplier > const xSupplier(
                    io_data.aContext.createComponentWithArguments( "com.sun.star.util.NumberFormatsSupplier", aInitArgs ),
                    UNO_QUERY_THROW
                );

                // ensure a NullDate we will assume later on
                UnoDate const aNullDate( 1, 1, 1900 );
                Reference< XPropertySet > const xFormatSettings( xSupplier->getNumberFormatSettings(), UNO_SET_THROW );
                xFormatSettings->setPropertyValue( ::rtl::OUString::createFromAscii( "NullDate" ), makeAny( aNullDate ) );

                // knit
                xFormatter->attachNumberFormatsSupplier( xSupplier );

                // done
                io_data.xNumberFormatter = xFormatter;
            }
            catch( const Exception& )
            {
                DBG_UNHANDLED_EXCEPTION();
            }

            return io_data.xNumberFormatter.is();
        }

        //--------------------------------------------------------------------------------------------------------------
        bool lcl_getValueNormalizer( CellValueConversion_Data & io_data, Type const & i_valueType,
            PValueNormalization & o_formatter )
        {
            NormalizerCache::const_iterator pos = io_data.aNormalizers.find( i_valueType.getTypeName() );
            if ( pos == io_data.aNormalizers.end() )
            {
                // never encountered this type before
                o_formatter.reset();

                ::rtl::OUString const sTypeName( i_valueType.getTypeName() );
                TypeClass const eTypeClass = i_valueType.getTypeClass();

                if ( sTypeName.equals( ::cppu::UnoType< DateTime >::get().getTypeName() ) )
                {
                    o_formatter.reset( new DateTimeNormalization( io_data.xNumberFormatter ) );
                }
                else if ( sTypeName.equals( ::cppu::UnoType< UnoDate >::get().getTypeName() ) )
                {
                    o_formatter.reset( new DateNormalization( io_data.xNumberFormatter ) );
                }
                else if ( sTypeName.equals( ::cppu::UnoType< UnoTime >::get().getTypeName() ) )
                {
                    o_formatter.reset( new TimeNormalization( io_data.xNumberFormatter ) );
                }
                else if ( sTypeName.equals( ::cppu::UnoType< ::sal_Bool >::get().getTypeName() ) )
                {
                    o_formatter.reset( new BooleanNormalization( io_data.xNumberFormatter ) );
                }
                else if (   sTypeName.equals( ::cppu::UnoType< double >::get().getTypeName() )
                        ||  sTypeName.equals( ::cppu::UnoType< float >::get().getTypeName() )
                        )
                {
                    o_formatter.reset( new DoubleNormalization( io_data.xNumberFormatter ) );
                }
                else if (   ( eTypeClass == TypeClass_BYTE )
                        ||  ( eTypeClass == TypeClass_SHORT )
                        ||  ( eTypeClass == TypeClass_UNSIGNED_SHORT )
                        ||  ( eTypeClass == TypeClass_LONG )
                        ||  ( eTypeClass == TypeClass_UNSIGNED_LONG )
                        ||  ( eTypeClass == TypeClass_HYPER )
                        )
                {
                    o_formatter.reset( new IntegerNormalization( io_data.xNumberFormatter ) );
                }
                else
                {
                    SAL_WARN( "svtools.table", "unsupported type '" << sTypeName << "'!" );
                }
                io_data.aNormalizers[ sTypeName ] = o_formatter;
            }
            else
                o_formatter = pos->second;

            return !!o_formatter;
        }
    }

    //==================================================================================================================
    //= CellValueConversion
    //==================================================================================================================
    //------------------------------------------------------------------------------------------------------------------
    CellValueConversion::CellValueConversion( ::comphelper::ComponentContext const & i_context )
        :m_pData( new CellValueConversion_Data( i_context ) )
    {
    }

    //------------------------------------------------------------------------------------------------------------------
    CellValueConversion::~CellValueConversion()
    {
    }

    //------------------------------------------------------------------------------------------------------------------
    ::rtl::OUString CellValueConversion::convertToString( const Any& i_value )
    {
        ::rtl::OUString sStringValue;
        if ( !i_value.hasValue() )
            return sStringValue;

        if ( ! ( i_value >>= sStringValue ) )
        {
            if ( lcl_ensureNumberFormatter( *m_pData ) )
            {
                PValueNormalization pNormalizer;
                if ( lcl_getValueNormalizer( *m_pData, i_value.getValueType(), pNormalizer ) )
                {
                    try
                    {
                        double const formatterCompliantValue = pNormalizer->convertToDouble( i_value );
                        sal_Int32 const formatKey = pNormalizer->getFormatKey();
                        sStringValue = m_pData->xNumberFormatter->convertNumberToString(
                            formatKey, formatterCompliantValue );
                    }
                    catch( const Exception& )
                    {
                        DBG_UNHANDLED_EXCEPTION();
                    }
                }
            }
        }

        return sStringValue;
    }

//......................................................................................................................
} // namespace svt
//......................................................................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
