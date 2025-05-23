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

#ifndef __FRAMEWORK_MACROS_XINTERFACE_HXX_
#define __FRAMEWORK_MACROS_XINTERFACE_HXX_

#include <com/sun/star/uno/RuntimeException.hpp>

#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/uno/Type.hxx>
#include <cppuhelper/queryinterface.hxx>
#include <rtl/ustring.hxx>

namespace framework{

/*_________________________________________________________________________________________________________________

    macros for declaration and definition of XInterface
    Please use follow public macros only!

    1)  DEFINE_XINTERFACE                                                               => use it in header to declare XInterface and his methods
    2)  DIRECT_INTERFACE( INTERFACE )                                                   => use it as parameter INTERFACEx at 4) if interface not ambigous
    3)  DERIVED_INTERFACE( BASEINTERFACE, DERIVEDINTERFACE )                            => use it as parameter INTERFACEx at 4) if interface can be ambigous
    4)  DECLARE_XINTERFACE_0( CLASS, BASECLASS )                                        => use it to define implementation of XInterface for 0 additional interface to baseclass
        DECLARE_XINTERFACE_1( CLASS, BASECLASS, INTERFACE1 )                            => use it to define implementation of XInterface for 1 additional interface to baseclass
        ...
        DECLARE_XINTERFACE_16( CLASS, BASECLASS, INTERFACE1, ... , INTERFACE16 )

_________________________________________________________________________________________________________________*/

//*****************************************************************************************************************
//  private
//  implementation of   XInterface::aquire()
//                      XInterface::release()
//*****************************************************************************************************************
#define PRIVATE_DEFINE_XINTERFACE_AQUIRE_RELEASE( CLASS, BASECLASS )                                                                                        \
    void SAL_CALL CLASS::acquire() throw()                                                                          \
    {                                                                                                                                                       \
        /* Don't use mutex in methods of XInterface! */                                                                                                     \
        BASECLASS::acquire();                                                                                                                               \
    }                                                                                                                                                       \
                                                                                                                                                            \
    void SAL_CALL CLASS::release() throw()                                                                          \
    {                                                                                                                                                       \
        /* Don't use mutex in methods of XInterface! */                                                                                                     \
        BASECLASS::release();                                                                                                                               \
    }

//*****************************************************************************************************************
//  private
//  implementation of XInterface::queryInterface() without any other interfaces!
//*****************************************************************************************************************
#define PRIVATE_DEFINE_XINTERFACE_QUERYINTERFACE_PURE( CLASS, BASECLASS )                                                                                   \
    ::com::sun::star::uno::Any SAL_CALL CLASS::queryInterface( const ::com::sun::star::uno::Type& aType ) throw( ::com::sun::star::uno::RuntimeException )  \
    {                                                                                                                                                       \
        /* Attention: Don't use mutex or guard in this method!!! Is a method of XInterface. */                                                              \
        /* I have no own supported interfaces ...                                           */                                                              \
        /* ... ask baseclass for interfaces!                                                */                                                              \
        return BASECLASS::queryInterface( aType );                                                                                                          \
    }

//*****************************************************************************************************************
//  private
//  implementation of XInterface::queryInterface() with max. 12 other interfaces!
//*****************************************************************************************************************
#define PRIVATE_DEFINE_XINTERFACE_QUERYINTERFACE( CLASS, BASECLASS, INTERFACES )                                                                            \
    ::com::sun::star::uno::Any SAL_CALL CLASS::queryInterface( const ::com::sun::star::uno::Type& aType ) throw( ::com::sun::star::uno::RuntimeException )  \
    {                                                                                                                                                       \
        /* Attention: Don't use mutex or guard in this method!!! Is a method of XInterface. */                                                              \
        /* Ask for my own supported interfaces ...                                          */                                                              \
        ::com::sun::star::uno::Any aReturn  ( ::cppu::queryInterface INTERFACES                                                                             \
                                            );                                                                                                              \
        /* If searched interface not supported by this class ... */                                                                                         \
        if ( aReturn.hasValue() == sal_False )                                                                                                              \
        {                                                                                                                                                   \
            /* ... ask baseclass for interfaces! */                                                                                                         \
            aReturn = BASECLASS::queryInterface( aType );                                                                                                   \
        }                                                                                                                                                   \
        /* Return result of this search. */                                                                                                                 \
        return aReturn;                                                                                                                                     \
    }

//*****************************************************************************************************************
//  private
//  implementation of XInterface::queryInterface() with more then 12 other interfaces!
//*****************************************************************************************************************
#define PRIVATE_DEFINE_XINTERFACE_QUERYINTERFACE_LARGE( CLASS, BASECLASS, INTERFACES_FIRST, INTERFACES_SECOND )                                             \
    ::com::sun::star::uno::Any SAL_CALL CLASS::queryInterface( const ::com::sun::star::uno::Type& aType ) throw( ::com::sun::star::uno::RuntimeException )  \
    {                                                                                                                                                       \
        /* Attention: Don't use mutex or guard in this method!!! Is a method of XInterface. */                                                              \
        /* Ask for my own supported interfaces ...                                          */                                                              \
        ::com::sun::star::uno::Any aReturn  ( ::cppu::queryInterface INTERFACES_FIRST                                                                       \
                                            );                                                                                                              \
        /* If searched interface not supported by first group ... */                                                                                        \
        if ( aReturn.hasValue() == sal_False )                                                                                                              \
        {                                                                                                                                                   \
            /* ... search in second group. (cppuhelper support 12 items only!) */                                                                           \
            aReturn = ::cppu::queryInterface INTERFACES_SECOND ;                                                                                            \
            /* If searched interface not supported by this class ... */                                                                                     \
            if ( aReturn.hasValue() == sal_False )                                                                                                          \
            {                                                                                                                                               \
                /* ... ask baseclass for interfaces! */                                                                                                     \
                aReturn = BASECLASS::queryInterface( aType );                                                                                               \
            }                                                                                                                                               \
        }                                                                                                                                                   \
        /* Return result of this search. */                                                                                                                 \
        return aReturn;                                                                                                                                     \
    }

//*****************************************************************************************************************
//  private
//  complete implementation of XInterface for different use cases
//*****************************************************************************************************************
#define PRIVATE_DEFINE_XINTERFACE_PURE( CLASS, BASECLASS )                                                                                                  \
    PRIVATE_DEFINE_XINTERFACE_AQUIRE_RELEASE( CLASS, BASECLASS )                                                                                            \
    PRIVATE_DEFINE_XINTERFACE_QUERYINTERFACE_PURE( CLASS, BASECLASS )

#define PRIVATE_DEFINE_XINTERFACE( CLASS, BASECLASS, INTERFACES )                                                                                           \
    PRIVATE_DEFINE_XINTERFACE_AQUIRE_RELEASE( CLASS, BASECLASS )                                                                                            \
    PRIVATE_DEFINE_XINTERFACE_QUERYINTERFACE( CLASS, BASECLASS, INTERFACES )

#define PRIVATE_DEFINE_XINTERFACE_LARGE( CLASS, BASECLASS, INTERFACES_FIRST, INTERFACES_SECOND )                                                            \
    PRIVATE_DEFINE_XINTERFACE_AQUIRE_RELEASE( CLASS, BASECLASS )                                                                                            \
    PRIVATE_DEFINE_XINTERFACE_QUERYINTERFACE_LARGE( CLASS, BASECLASS, INTERFACES_FIRST, INTERFACES_SECOND )

//*****************************************************************************************************************
//  private
//  help macros to replace INTERFACES in queryInterface() [see before]
//*****************************************************************************************************************
/*
#ifdef ENABLE_SERVICEDEBUG
    #define PRIVATE_DEFINE_INTERFACE_1( INTERFACE1 )     \
        static_cast< XSPECIALDEBUGINTERFACE##* >( this ), \
        INTERFACE1
#else
*/
    #define PRIVATE_DEFINE_INTERFACE_1( INTERFACE1 )                                                                                                                                                    \
        INTERFACE1
//#endif // #ifdef ENABLE_SERVICEDEBUG

#define PRIVATE_DEFINE_INTERFACE_2( INTERFACE1, INTERFACE2 )                                                                                                                                            \
    PRIVATE_DEFINE_INTERFACE_1( INTERFACE1 ),                                                                                                                                                           \
    INTERFACE2

#define PRIVATE_DEFINE_INTERFACE_3( INTERFACE1, INTERFACE2, INTERFACE3 )                                                                                                                                \
    PRIVATE_DEFINE_INTERFACE_2( INTERFACE1, INTERFACE2 ),                                                                                                                                               \
    INTERFACE3

#define PRIVATE_DEFINE_INTERFACE_4( INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4 )                                                                                                                    \
    PRIVATE_DEFINE_INTERFACE_3( INTERFACE1, INTERFACE2, INTERFACE3 ),                                                                                                                                   \
    INTERFACE4

#define PRIVATE_DEFINE_INTERFACE_5( INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5 )                                                                                                        \
    PRIVATE_DEFINE_INTERFACE_4( INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4 ),                                                                                                                       \
    INTERFACE5

#define PRIVATE_DEFINE_INTERFACE_6( INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6 )                                                                                            \
    PRIVATE_DEFINE_INTERFACE_5( INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5 ),                                                                                                           \
    INTERFACE6

#define PRIVATE_DEFINE_INTERFACE_7( INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6, INTERFACE7 )                                                                                \
    PRIVATE_DEFINE_INTERFACE_6( INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6 ),                                                                                               \
    INTERFACE7

#define PRIVATE_DEFINE_INTERFACE_8( INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6, INTERFACE7, INTERFACE8 )                                                                    \
    PRIVATE_DEFINE_INTERFACE_7( INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6, INTERFACE7 ),                                                                                   \
    INTERFACE8

#define PRIVATE_DEFINE_INTERFACE_9( INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6, INTERFACE7, INTERFACE8, INTERFACE9 )                                                        \
    PRIVATE_DEFINE_INTERFACE_8( INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6, INTERFACE7, INTERFACE8 ),                                                                       \
    INTERFACE9

#define PRIVATE_DEFINE_INTERFACE_10( INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6, INTERFACE7, INTERFACE8, INTERFACE9, INTERFACE10 )                                          \
    PRIVATE_DEFINE_INTERFACE_9( INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6, INTERFACE7, INTERFACE8, INTERFACE9 ),                                                           \
    INTERFACE10

#define PRIVATE_DEFINE_INTERFACE_11( INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6, INTERFACE7, INTERFACE8, INTERFACE9, INTERFACE10, INTERFACE11 )                             \
    PRIVATE_DEFINE_INTERFACE_10( INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6, INTERFACE7, INTERFACE8, INTERFACE9, INTERFACE10 ),                                             \
    INTERFACE11

//*****************************************************************************************************************
//  public
//  help macros for follow XInterface definitions
//*****************************************************************************************************************

//  Use it as parameter for DEFINE_XINTERFACE_X(), if you CAN use an interface directly in queryInterface()!
#define DIRECT_INTERFACE( INTERFACE ) \
    static_cast< INTERFACE* >( this )

//  Use it as parameter for DEFINE_XINTERFACE_X(), if you CAN'T use an interface directly in queryInterface()!
//  (zB at ambigous errors!)
#define DERIVED_INTERFACE( BASEINTERFACE, DERIVEDINTERFACE ) \
    static_cast< BASEINTERFACE* >( static_cast< DERIVEDINTERFACE* >( this ) )

//*****************************************************************************************************************
//  public
//  declaration of XInterface
//*****************************************************************************************************************
#define FWK_DECLARE_XINTERFACE                                                                                                                                      \
    virtual ::com::sun::star::uno::Any  SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType   ) throw( ::com::sun::star::uno::RuntimeException ); \
    virtual void                        SAL_CALL acquire       (                                            ) throw();  \
    virtual void                        SAL_CALL release       (                                            ) throw();

//*****************************************************************************************************************
//  public
//  implementation of XInterface
//*****************************************************************************************************************

//  implementation of XInterface with 0 additional interface for queryInterface()
#define DEFINE_XINTERFACE_0( CLASS, BASECLASS )                                                                     \
    PRIVATE_DEFINE_XINTERFACE_PURE  (   CLASS,                                                                      \
                                          BASECLASS                                                                 \
                                    )

//  implementation of XInterface with 1 additional interface for queryInterface()
#define DEFINE_XINTERFACE_1( CLASS, BASECLASS, INTERFACE1 )                                                         \
    PRIVATE_DEFINE_XINTERFACE   (   CLASS,                                                                          \
                                     BASECLASS,                                                                     \
                                     ( aType, PRIVATE_DEFINE_INTERFACE_1    (   INTERFACE1                              \
                                                                        )                                           \
                                    )                                                                               \
                                )

//  implementation of XInterface with 2 additional interfaces for queryInterface()
#define DEFINE_XINTERFACE_2( CLASS, BASECLASS, INTERFACE1, INTERFACE2 )                                             \
    PRIVATE_DEFINE_XINTERFACE   (   CLASS,                                                                          \
                                    BASECLASS,                                                                      \
                                    ( aType, PRIVATE_DEFINE_INTERFACE_2 (   INTERFACE1  ,                           \
                                                                             INTERFACE2                             \
                                                                        )                                           \
                                    )                                                                               \
                                )

//  implementation of XInterface with 3 additional interfaces for queryInterface()
#define DEFINE_XINTERFACE_3( CLASS, BASECLASS, INTERFACE1, INTERFACE2, INTERFACE3 )                                 \
    PRIVATE_DEFINE_XINTERFACE   (   CLASS,                                                                          \
                                    BASECLASS,                                                                      \
                                    ( aType, PRIVATE_DEFINE_INTERFACE_3 (   INTERFACE1  ,                           \
                                                                             INTERFACE2 ,                           \
                                                                            INTERFACE3                              \
                                                                        )                                           \
                                    )                                                                               \
                                )

//  implementation of XInterface with 4 additional interfaces for queryInterface()
#define DEFINE_XINTERFACE_4( CLASS, BASECLASS, INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4 )                     \
    PRIVATE_DEFINE_XINTERFACE   (   CLASS,                                                                          \
                                    BASECLASS,                                                                      \
                                    ( aType, PRIVATE_DEFINE_INTERFACE_4 (   INTERFACE1  ,                           \
                                                                            INTERFACE2  ,                           \
                                                                            INTERFACE3  ,                           \
                                                                            INTERFACE4                              \
                                                                        )                                           \
                                    )                                                                               \
                                )

//  implementation of XInterface with 5 additional interfaces for queryInterface()
#define DEFINE_XINTERFACE_5( CLASS, BASECLASS, INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5 )         \
    PRIVATE_DEFINE_XINTERFACE   (   CLASS,                                                                          \
                                    BASECLASS,                                                                      \
                                    ( aType, PRIVATE_DEFINE_INTERFACE_5 (   INTERFACE1  ,                           \
                                                                            INTERFACE2  ,                           \
                                                                            INTERFACE3  ,                           \
                                                                            INTERFACE4  ,                           \
                                                                            INTERFACE5                              \
                                                                        )                                           \
                                    )                                                                               \
                                )

//  implementation of XInterface with 6 additional interfaces for queryInterface()
#define DEFINE_XINTERFACE_6( CLASS, BASECLASS, INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6 ) \
    PRIVATE_DEFINE_XINTERFACE   (   CLASS,                                                                          \
                                    BASECLASS,                                                                      \
                                    ( aType, PRIVATE_DEFINE_INTERFACE_6 (   INTERFACE1  ,                           \
                                                                            INTERFACE2  ,                           \
                                                                            INTERFACE3  ,                           \
                                                                            INTERFACE4  ,                           \
                                                                            INTERFACE5  ,                           \
                                                                            INTERFACE6                              \
                                                                        )                                           \
                                    )                                                                               \
                                )

//  implementation of XInterface with 7 additional interfaces for queryInterface()
#define DEFINE_XINTERFACE_7( CLASS, BASECLASS, INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6, INTERFACE7 ) \
    PRIVATE_DEFINE_XINTERFACE   (   CLASS,                                                                          \
                                    BASECLASS,                                                                      \
                                    ( aType, PRIVATE_DEFINE_INTERFACE_7 (   INTERFACE1  ,                           \
                                                                            INTERFACE2  ,                           \
                                                                            INTERFACE3  ,                           \
                                                                            INTERFACE4  ,                           \
                                                                            INTERFACE5  ,                           \
                                                                            INTERFACE6  ,                           \
                                                                            INTERFACE7                              \
                                                                        )                                           \
                                    )                                                                               \
                                )

//  implementation of XInterface with 8 additional interfaces for queryInterface()
#define DEFINE_XINTERFACE_8( CLASS, BASECLASS, INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6, INTERFACE7, INTERFACE8 ) \
    PRIVATE_DEFINE_XINTERFACE   (   CLASS,                                                                          \
                                    BASECLASS,                                                                      \
                                    ( aType, PRIVATE_DEFINE_INTERFACE_8 (   INTERFACE1  ,                           \
                                                                            INTERFACE2  ,                           \
                                                                            INTERFACE3  ,                           \
                                                                            INTERFACE4  ,                           \
                                                                            INTERFACE5  ,                           \
                                                                            INTERFACE6  ,                           \
                                                                            INTERFACE7  ,                           \
                                                                            INTERFACE8                              \
                                                                        )                                           \
                                    )                                                                               \
                                )

//  implementation of XInterface with 9 additional interfaces for queryInterface()
#define DEFINE_XINTERFACE_9( CLASS, BASECLASS, INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6, INTERFACE7, INTERFACE8, INTERFACE9 ) \
    PRIVATE_DEFINE_XINTERFACE   (   CLASS,                                                                          \
                                    BASECLASS,                                                                      \
                                    ( aType, PRIVATE_DEFINE_INTERFACE_9 (   INTERFACE1  ,                           \
                                                                            INTERFACE2  ,                           \
                                                                            INTERFACE3  ,                           \
                                                                            INTERFACE4  ,                           \
                                                                            INTERFACE5  ,                           \
                                                                            INTERFACE6  ,                           \
                                                                            INTERFACE7  ,                           \
                                                                            INTERFACE8  ,                           \
                                                                            INTERFACE9                              \
                                                                        )                                           \
                                    )                                                                               \
                                )

//  implementation of XInterface with 10 additional interfaces for queryInterface()
#define DEFINE_XINTERFACE_10( CLASS, BASECLASS, INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6, INTERFACE7, INTERFACE8, INTERFACE9, INTERFACE10 )   \
    PRIVATE_DEFINE_XINTERFACE   (   CLASS,                                                                          \
                                    BASECLASS,                                                                      \
                                    ( aType, PRIVATE_DEFINE_INTERFACE_10    (   INTERFACE1  ,                       \
                                                                                INTERFACE2  ,                       \
                                                                                INTERFACE3  ,                       \
                                                                                INTERFACE4  ,                       \
                                                                                INTERFACE5  ,                       \
                                                                                INTERFACE6  ,                       \
                                                                                INTERFACE7  ,                       \
                                                                                INTERFACE8  ,                       \
                                                                                INTERFACE9  ,                       \
                                                                                INTERFACE10                         \
                                                                            )                                       \
                                    )                                                                               \
                                )

//  implementation of XInterface with 11 additional interfaces for queryInterface()
#define DEFINE_XINTERFACE_11( CLASS, BASECLASS, INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6, INTERFACE7, INTERFACE8, INTERFACE9, INTERFACE10, INTERFACE11 )  \
    PRIVATE_DEFINE_XINTERFACE   (   CLASS,                                                                          \
                                    BASECLASS,                                                                      \
                                    ( aType, PRIVATE_DEFINE_INTERFACE_11    (   INTERFACE1  ,                       \
                                                                                INTERFACE2  ,                       \
                                                                                INTERFACE3  ,                       \
                                                                                INTERFACE4  ,                       \
                                                                                INTERFACE5  ,                       \
                                                                                INTERFACE6  ,                       \
                                                                                INTERFACE7  ,                       \
                                                                                INTERFACE8  ,                       \
                                                                                INTERFACE9  ,                       \
                                                                                INTERFACE10 ,                       \
                                                                                INTERFACE11                         \
                                                                            )                                       \
                                    )                                                                               \
                                )

//  implementation of XInterface with 12 additional interfaces for queryInterface()
#define DEFINE_XINTERFACE_12( CLASS, BASECLASS, INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6, INTERFACE7, INTERFACE8, INTERFACE9, INTERFACE10, INTERFACE11, INTERFACE12 ) \
    PRIVATE_DEFINE_XINTERFACE_LARGE (   CLASS,                                                                      \
                                        BASECLASS,                                                                  \
                                        ( aType, PRIVATE_DEFINE_INTERFACE_11    (   INTERFACE1  ,                   \
                                                                                    INTERFACE2  ,                   \
                                                                                    INTERFACE3  ,                   \
                                                                                    INTERFACE4  ,                   \
                                                                                    INTERFACE5  ,                   \
                                                                                    INTERFACE6  ,                   \
                                                                                    INTERFACE7  ,                   \
                                                                                    INTERFACE8  ,                   \
                                                                                    INTERFACE9  ,                   \
                                                                                    INTERFACE10 ,                   \
                                                                                    INTERFACE11                     \
                                                                                )                                   \
                                        ),                                                                          \
                                        ( aType, PRIVATE_DEFINE_INTERFACE_1     (   INTERFACE12                     \
                                                                                )                                   \
                                        )                                                                           \
                                    )

//  implementation of XInterface with 13 additional interfaces for queryInterface()
#define DEFINE_XINTERFACE_13( CLASS, BASECLASS, INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6, INTERFACE7, INTERFACE8, INTERFACE9, INTERFACE10, INTERFACE11, INTERFACE12, INTERFACE13 )    \
    PRIVATE_DEFINE_XINTERFACE_LARGE (   CLASS,                                                                      \
                                        BASECLASS,                                                                  \
                                        ( aType, PRIVATE_DEFINE_INTERFACE_11    (   INTERFACE1  ,                   \
                                                                                    INTERFACE2  ,                   \
                                                                                    INTERFACE3  ,                   \
                                                                                    INTERFACE4  ,                   \
                                                                                    INTERFACE5  ,                   \
                                                                                    INTERFACE6  ,                   \
                                                                                    INTERFACE7  ,                   \
                                                                                    INTERFACE8  ,                   \
                                                                                    INTERFACE9  ,                   \
                                                                                    INTERFACE10 ,                   \
                                                                                    INTERFACE11                     \
                                                                                )                                   \
                                        ),                                                                          \
                                        ( aType, PRIVATE_DEFINE_INTERFACE_2     (   INTERFACE12 ,                   \
                                                                                    INTERFACE13                     \
                                                                                )                                   \
                                        )                                                                           \
                                    )

//  implementation of XInterface with 14 additional interfaces for queryInterface()
#define DEFINE_XINTERFACE_14( CLASS, BASECLASS, INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6, INTERFACE7, INTERFACE8, INTERFACE9, INTERFACE10, INTERFACE11, INTERFACE12, INTERFACE13, INTERFACE14 )   \
    PRIVATE_DEFINE_XINTERFACE_LARGE (   CLASS,                                                                      \
                                        BASECLASS,                                                                  \
                                        ( aType, PRIVATE_DEFINE_INTERFACE_11    (   INTERFACE1  ,                   \
                                                                                    INTERFACE2  ,                   \
                                                                                    INTERFACE3  ,                   \
                                                                                    INTERFACE4  ,                   \
                                                                                    INTERFACE5  ,                   \
                                                                                    INTERFACE6  ,                   \
                                                                                    INTERFACE7  ,                   \
                                                                                    INTERFACE8  ,                   \
                                                                                    INTERFACE9  ,                   \
                                                                                    INTERFACE10 ,                   \
                                                                                    INTERFACE11                     \
                                                                                )                                   \
                                        ),                                                                          \
                                        ( aType, PRIVATE_DEFINE_INTERFACE_3     (   INTERFACE12 ,                   \
                                                                                    INTERFACE13 ,                   \
                                                                                    INTERFACE14                     \
                                                                                )                                   \
                                        )                                                                           \
                                    )

//  implementation of XInterface with 15 additional interfaces for queryInterface()
#define DEFINE_XINTERFACE_15( CLASS, BASECLASS, INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6, INTERFACE7, INTERFACE8, INTERFACE9, INTERFACE10, INTERFACE11, INTERFACE12, INTERFACE13, INTERFACE14, INTERFACE15 )  \
    PRIVATE_DEFINE_XINTERFACE_LARGE (   CLASS,                                                                      \
                                        BASECLASS,                                                                  \
                                        ( aType, PRIVATE_DEFINE_INTERFACE_11    (   INTERFACE1  ,                   \
                                                                                    INTERFACE2  ,                   \
                                                                                    INTERFACE3  ,                   \
                                                                                    INTERFACE4  ,                   \
                                                                                    INTERFACE5  ,                   \
                                                                                    INTERFACE6  ,                   \
                                                                                    INTERFACE7  ,                   \
                                                                                    INTERFACE8  ,                   \
                                                                                    INTERFACE9  ,                   \
                                                                                    INTERFACE10 ,                   \
                                                                                    INTERFACE11                     \
                                                                                )                                   \
                                        ),                                                                          \
                                        ( aType, PRIVATE_DEFINE_INTERFACE_4     (   INTERFACE12 ,                   \
                                                                                    INTERFACE13 ,                   \
                                                                                    INTERFACE14 ,                   \
                                                                                    INTERFACE15                     \
                                                                                )                                   \
                                        )                                                                           \
                                    )

//  implementation of XInterface with 16 additional interfaces for queryInterface()
#define DEFINE_XINTERFACE_16( CLASS, BASECLASS, INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6, INTERFACE7, INTERFACE8, INTERFACE9, INTERFACE10, INTERFACE11, INTERFACE12, INTERFACE13, INTERFACE14, INTERFACE15, INTERFACE16 ) \
    PRIVATE_DEFINE_XINTERFACE_LARGE (   CLASS,                                                                      \
                                        BASECLASS,                                                                  \
                                        ( aType, PRIVATE_DEFINE_INTERFACE_11    (   INTERFACE1  ,                   \
                                                                                    INTERFACE2  ,                   \
                                                                                    INTERFACE3  ,                   \
                                                                                    INTERFACE4  ,                   \
                                                                                    INTERFACE5  ,                   \
                                                                                    INTERFACE6  ,                   \
                                                                                    INTERFACE7  ,                   \
                                                                                    INTERFACE8  ,                   \
                                                                                    INTERFACE9  ,                   \
                                                                                    INTERFACE10 ,                   \
                                                                                    INTERFACE11                     \
                                                                                )                                   \
                                        ),                                                                          \
                                        ( aType, PRIVATE_DEFINE_INTERFACE_5     (   INTERFACE12 ,                   \
                                                                                    INTERFACE13 ,                   \
                                                                                    INTERFACE14 ,                   \
                                                                                    INTERFACE15 ,                   \
                                                                                    INTERFACE16                     \
                                                                                )                                   \
                                        )                                                                           \
                                    )

//  implementation of XInterface with 17 additional interfaces for queryInterface()
#define DEFINE_XINTERFACE_17( CLASS, BASECLASS, INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6, INTERFACE7, INTERFACE8, INTERFACE9, INTERFACE10, INTERFACE11, INTERFACE12, INTERFACE13, INTERFACE14, INTERFACE15, INTERFACE16, INTERFACE17 ) \
    PRIVATE_DEFINE_XINTERFACE_LARGE (   CLASS,                                                                      \
                                        BASECLASS,                                                                  \
                                        ( aType, PRIVATE_DEFINE_INTERFACE_11    (   INTERFACE1  ,                   \
                                                                                    INTERFACE2  ,                   \
                                                                                    INTERFACE3  ,                   \
                                                                                    INTERFACE4  ,                   \
                                                                                    INTERFACE5  ,                   \
                                                                                    INTERFACE6  ,                   \
                                                                                    INTERFACE7  ,                   \
                                                                                    INTERFACE8  ,                   \
                                                                                    INTERFACE9  ,                   \
                                                                                    INTERFACE10 ,                   \
                                                                                    INTERFACE11                     \
                                                                                )                                   \
                                        ),                                                                          \
                                        ( aType, PRIVATE_DEFINE_INTERFACE_6     (   INTERFACE12 ,                   \
                                                                                    INTERFACE13 ,                   \
                                                                                    INTERFACE14 ,                   \
                                                                                    INTERFACE15 ,                   \
                                                                                    INTERFACE16 ,                   \
                                                                                    INTERFACE17                     \
                                                                                )                                   \
                                        )                                                                           \
                                    )

//  implementation of XInterface with 18 additional interfaces for queryInterface()
#define DEFINE_XINTERFACE_18( CLASS, BASECLASS, INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6, INTERFACE7, INTERFACE8, INTERFACE9, INTERFACE10, INTERFACE11, INTERFACE12, INTERFACE13, INTERFACE14, INTERFACE15, INTERFACE16, INTERFACE17, INTERFACE18 ) \
    PRIVATE_DEFINE_XINTERFACE_LARGE (   CLASS,                                                                      \
                                        BASECLASS,                                                                  \
                                        ( aType, PRIVATE_DEFINE_INTERFACE_11    (   INTERFACE1  ,                   \
                                                                                    INTERFACE2  ,                   \
                                                                                    INTERFACE3  ,                   \
                                                                                    INTERFACE4  ,                   \
                                                                                    INTERFACE5  ,                   \
                                                                                    INTERFACE6  ,                   \
                                                                                    INTERFACE7  ,                   \
                                                                                    INTERFACE8  ,                   \
                                                                                    INTERFACE9  ,                   \
                                                                                    INTERFACE10 ,                   \
                                                                                    INTERFACE11                     \
                                                                                )                                   \
                                        ),                                                                          \
                                        ( aType, PRIVATE_DEFINE_INTERFACE_7     (   INTERFACE12 ,                   \
                                                                                    INTERFACE13 ,                   \
                                                                                    INTERFACE14 ,                   \
                                                                                    INTERFACE15 ,                   \
                                                                                    INTERFACE16 ,                   \
                                                                                    INTERFACE17 ,                   \
                                                                                    INTERFACE18                     \
                                                                                )                                   \
                                        )                                                                           \
                                    )

//  implementation of XInterface with 19 additional interfaces for queryInterface()
#define DEFINE_XINTERFACE_19( CLASS, BASECLASS, INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6, INTERFACE7, INTERFACE8, INTERFACE9, INTERFACE10, INTERFACE11, INTERFACE12, INTERFACE13, INTERFACE14, INTERFACE15, INTERFACE16, INTERFACE17, INTERFACE18, INTERFACE19 ) \
    PRIVATE_DEFINE_XINTERFACE_LARGE (   CLASS,                                                                      \
                                        BASECLASS,                                                                  \
                                        ( aType, PRIVATE_DEFINE_INTERFACE_11    (   INTERFACE1  ,                   \
                                                                                    INTERFACE2  ,                   \
                                                                                    INTERFACE3  ,                   \
                                                                                    INTERFACE4  ,                   \
                                                                                    INTERFACE5  ,                   \
                                                                                    INTERFACE6  ,                   \
                                                                                    INTERFACE7  ,                   \
                                                                                    INTERFACE8  ,                   \
                                                                                    INTERFACE9  ,                   \
                                                                                    INTERFACE10 ,                   \
                                                                                    INTERFACE11                     \
                                                                                )                                   \
                                        ),                                                                          \
                                        ( aType, PRIVATE_DEFINE_INTERFACE_8     (   INTERFACE12 ,                   \
                                                                                    INTERFACE13 ,                   \
                                                                                    INTERFACE14 ,                   \
                                                                                    INTERFACE15 ,                   \
                                                                                    INTERFACE16 ,                   \
                                                                                    INTERFACE17 ,                   \
                                                                                    INTERFACE18 ,                   \
                                                                                    INTERFACE19                     \
                                                                                )                                   \
                                        )                                                                           \
                                    )

//  implementation of XInterface with 20 additional interfaces for queryInterface()
#define DEFINE_XINTERFACE_20( CLASS, BASECLASS, INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6, INTERFACE7, INTERFACE8, INTERFACE9, INTERFACE10, INTERFACE11, INTERFACE12, INTERFACE13, INTERFACE14, INTERFACE15, INTERFACE16, INTERFACE17, INTERFACE18, INTERFACE19, INTERFACE20 ) \
    PRIVATE_DEFINE_XINTERFACE_LARGE (   CLASS,                                                                      \
                                        BASECLASS,                                                                  \
                                        ( aType, PRIVATE_DEFINE_INTERFACE_11    (   INTERFACE1  ,                   \
                                                                                    INTERFACE2  ,                   \
                                                                                    INTERFACE3  ,                   \
                                                                                    INTERFACE4  ,                   \
                                                                                    INTERFACE5  ,                   \
                                                                                    INTERFACE6  ,                   \
                                                                                    INTERFACE7  ,                   \
                                                                                    INTERFACE8  ,                   \
                                                                                    INTERFACE9  ,                   \
                                                                                    INTERFACE10 ,                   \
                                                                                    INTERFACE11                     \
                                                                                )                                   \
                                        ),                                                                          \
                                        ( aType, PRIVATE_DEFINE_INTERFACE_9     (   INTERFACE12 ,                   \
                                                                                    INTERFACE13 ,                   \
                                                                                    INTERFACE14 ,                   \
                                                                                    INTERFACE15 ,                   \
                                                                                    INTERFACE16 ,                   \
                                                                                    INTERFACE17 ,                   \
                                                                                    INTERFACE18 ,                   \
                                                                                    INTERFACE19 ,                   \
                                                                                    INTERFACE20                     \
                                                                                )                                   \
                                        )                                                                           \
                                    )

//  implementation of XInterface with 21 additional interfaces for queryInterface()
#define DEFINE_XINTERFACE_21( CLASS, BASECLASS, INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6, INTERFACE7, INTERFACE8, INTERFACE9, INTERFACE10, INTERFACE11, INTERFACE12, INTERFACE13, INTERFACE14, INTERFACE15, INTERFACE16, INTERFACE17, INTERFACE18, INTERFACE19, INTERFACE20, INTERFACE21 ) \
    PRIVATE_DEFINE_XINTERFACE_LARGE (   CLASS,                                                                      \
                                        BASECLASS,                                                                  \
                                        ( aType, PRIVATE_DEFINE_INTERFACE_11    (   INTERFACE1  ,                   \
                                                                                    INTERFACE2  ,                   \
                                                                                    INTERFACE3  ,                   \
                                                                                    INTERFACE4  ,                   \
                                                                                    INTERFACE5  ,                   \
                                                                                    INTERFACE6  ,                   \
                                                                                    INTERFACE7  ,                   \
                                                                                    INTERFACE8  ,                   \
                                                                                    INTERFACE9  ,                   \
                                                                                    INTERFACE10 ,                   \
                                                                                    INTERFACE11                     \
                                                                                )                                   \
                                        ),                                                                          \
                                        ( aType, PRIVATE_DEFINE_INTERFACE_10    (   INTERFACE12 ,                   \
                                                                                    INTERFACE13 ,                   \
                                                                                    INTERFACE14 ,                   \
                                                                                    INTERFACE15 ,                   \
                                                                                    INTERFACE16 ,                   \
                                                                                    INTERFACE17 ,                   \
                                                                                    INTERFACE18 ,                   \
                                                                                    INTERFACE19 ,                   \
                                                                                    INTERFACE20 ,                   \
                                                                                    INTERFACE21                     \
                                                                                )                                   \
                                        )                                                                           \
                                    )

//  implementation of XInterface with 22 additional interfaces for queryInterface()
#define DEFINE_XINTERFACE_22( CLASS, BASECLASS, INTERFACE1, INTERFACE2, INTERFACE3, INTERFACE4, INTERFACE5, INTERFACE6, INTERFACE7, INTERFACE8, INTERFACE9, INTERFACE10, INTERFACE11, INTERFACE12, INTERFACE13, INTERFACE14, INTERFACE15, INTERFACE16, INTERFACE17, INTERFACE18, INTERFACE19, INTERFACE20, INTERFACE21, INTERFACE22 ) \
    PRIVATE_DEFINE_XINTERFACE_LARGE (   CLASS,                                                                      \
                                        BASECLASS,                                                                  \
                                        ( aType, PRIVATE_DEFINE_INTERFACE_11    (   INTERFACE1  ,                   \
                                                                                    INTERFACE2  ,                   \
                                                                                    INTERFACE3  ,                   \
                                                                                    INTERFACE4  ,                   \
                                                                                    INTERFACE5  ,                   \
                                                                                    INTERFACE6  ,                   \
                                                                                    INTERFACE7  ,                   \
                                                                                    INTERFACE8  ,                   \
                                                                                    INTERFACE9  ,                   \
                                                                                    INTERFACE10 ,                   \
                                                                                    INTERFACE11                     \
                                                                                )                                   \
                                        ),                                                                          \
                                        ( aType, PRIVATE_DEFINE_INTERFACE_11    (   INTERFACE12 ,                   \
                                                                                    INTERFACE13 ,                   \
                                                                                    INTERFACE14 ,                   \
                                                                                    INTERFACE15 ,                   \
                                                                                    INTERFACE16 ,                   \
                                                                                    INTERFACE17 ,                   \
                                                                                    INTERFACE18 ,                   \
                                                                                    INTERFACE19 ,                   \
                                                                                    INTERFACE20 ,                   \
                                                                                    INTERFACE22 ,                   \
                                                                                    INTERFACE21                     \
                                                                                )                                   \
                                        )                                                                           \
                                    )

}       //  namespace framework

#endif  //  #ifndef __FRAMEWORK_MACROS_XINTERFACE_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
