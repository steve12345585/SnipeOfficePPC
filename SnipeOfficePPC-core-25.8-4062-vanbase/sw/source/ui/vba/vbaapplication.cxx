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
#include <stdio.h>
#include "vbaapplication.hxx"
#include "vbadocument.hxx"
#include <osl/file.hxx>
#include <vbahelper/vbahelper.hxx>
#include "vbawindow.hxx"
#include "vbasystem.hxx"
#include "vbaoptions.hxx"
#include "vbaselection.hxx"
#include "vbadocuments.hxx"
#include "vbaaddins.hxx"
#include "vbadialogs.hxx"
#include <ooo/vba/word/WdEnableCancelKey.hpp>
#include <editeng/acorrcfg.hxx>
#include "wordvbahelper.hxx"
#include <docsh.hxx>
#include "vbalistgalleries.hxx"

using namespace ::ooo;
using namespace ::ooo::vba;
using namespace ::com::sun::star;

using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::UNO_QUERY_THROW;
using ::com::sun::star::uno::UNO_QUERY;
using ::rtl::OUString;

// #TODO is this defined somewhere else?
#if ( defined UNX )
#define FILE_PATH_SEPERATOR "/"
#else // windows
#define FILE_PATH_SEPERATOR "\\"
#endif

#define EXCELVERSION "11.0"

uno::Any sbxToUnoValue( SbxVariable* pVar );

SwVbaApplication::SwVbaApplication( uno::Reference<uno::XComponentContext >& xContext ): SwVbaApplication_BASE( xContext )
{
}

SwVbaApplication::~SwVbaApplication()
{
}

SfxObjectShell* SwVbaApplication::GetDocShell( const uno::Reference< frame::XModel >& xModel ) throw (uno::RuntimeException)
{
    return static_cast< SfxObjectShell* >( word::getDocShell( xModel ) );
}

rtl::OUString SAL_CALL
SwVbaApplication::getName() throw (uno::RuntimeException)
{
    static rtl::OUString appName( RTL_CONSTASCII_USTRINGPARAM("Microsoft Word" ) );
    return appName;
}

uno::Reference< word::XDocument > SAL_CALL
SwVbaApplication::getActiveDocument() throw (uno::RuntimeException)
{
    return new SwVbaDocument( this, mxContext, getCurrentDocument() );
}

uno::Reference< word::XWindow > SAL_CALL
SwVbaApplication::getActiveWindow() throw (uno::RuntimeException)
{
    // #FIXME sofar can't determine Parent
    uno::Reference< frame::XModel > xModel( getCurrentDocument(), uno::UNO_SET_THROW );
    uno::Reference< frame::XController > xController( xModel->getCurrentController(), uno::UNO_SET_THROW );
    return new SwVbaWindow( uno::Reference< XHelperInterface >(), mxContext, xModel, xController );
}

uno::Reference<word::XSystem > SAL_CALL
SwVbaApplication::getSystem() throw (uno::RuntimeException)
{
    return uno::Reference< word::XSystem >( new SwVbaSystem( mxContext ) );
}

uno::Reference<word::XOptions > SAL_CALL
SwVbaApplication::getOptions() throw (uno::RuntimeException)
{
    return uno::Reference< word::XOptions >( new SwVbaOptions( mxContext ) );
}

uno::Any SAL_CALL
SwVbaApplication::CommandBars( const uno::Any& aIndex ) throw (uno::RuntimeException)
{
    return VbaApplicationBase::CommandBars( aIndex );
}

uno::Reference< word::XSelection > SAL_CALL
SwVbaApplication::getSelection() throw (uno::RuntimeException)
{
    return new SwVbaSelection( this, mxContext, getCurrentDocument() );
}

uno::Any SAL_CALL
SwVbaApplication::Documents( const uno::Any& index ) throw (uno::RuntimeException)
{
    uno::Reference< XCollection > xCol( new SwVbaDocuments( this, mxContext ) );
    if ( index.hasValue() )
        return xCol->Item( index, uno::Any() );
    return uno::makeAny( xCol );
}

uno::Any SAL_CALL
SwVbaApplication::Addins( const uno::Any& index ) throw (uno::RuntimeException)
{
    static uno::Reference< XCollection > xCol( new SwVbaAddins( this, mxContext ) );
    if ( index.hasValue() )
        return xCol->Item( index, uno::Any() );
    return uno::makeAny( xCol );
}

uno::Any SAL_CALL
SwVbaApplication::Dialogs( const uno::Any& index ) throw (uno::RuntimeException)
{
    uno::Reference< word::XDialogs > xCol( new SwVbaDialogs( this, mxContext, getCurrentDocument() ));
    if ( index.hasValue() )
        return xCol->Item( index );
    return uno::makeAny( xCol );
}

uno::Any SAL_CALL
SwVbaApplication::ListGalleries( const uno::Any& index ) throw (uno::RuntimeException)
{
    uno::Reference< text::XTextDocument > xTextDoc( getCurrentDocument(), uno::UNO_QUERY_THROW );
    uno::Reference< XCollection > xCol( new SwVbaListGalleries( this, mxContext, xTextDoc ) );
    if ( index.hasValue() )
        return xCol->Item( index, uno::Any() );
    return uno::makeAny( xCol );
}

sal_Bool SAL_CALL SwVbaApplication::getDisplayAutoCompleteTips() throw (css::uno::RuntimeException)
{
    return SvxAutoCorrCfg::Get().IsAutoTextTip();
}

void SAL_CALL SwVbaApplication::setDisplayAutoCompleteTips( sal_Bool _displayAutoCompleteTips ) throw (css::uno::RuntimeException)
{
    SvxAutoCorrCfg::Get().SetAutoTextTip( _displayAutoCompleteTips );
}

sal_Int32 SAL_CALL SwVbaApplication::getEnableCancelKey() throw (css::uno::RuntimeException)
{
    // the default value is wdCancelInterrupt in Word
    return word::WdEnableCancelKey::wdCancelInterrupt;
}

void SAL_CALL SwVbaApplication::setEnableCancelKey( sal_Int32/* _enableCancelKey */) throw (css::uno::RuntimeException)
{
    // seems not supported in Writer
}

float SAL_CALL SwVbaApplication::CentimetersToPoints( float _Centimeters ) throw (uno::RuntimeException)
{
    return VbaApplicationBase::CentimetersToPoints( _Centimeters );
}

uno::Reference< frame::XModel >
SwVbaApplication::getCurrentDocument() throw (css::uno::RuntimeException)
{
    return getCurrentWordDoc( mxContext );
}

rtl::OUString
SwVbaApplication::getServiceImplName()
{
    return rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("SwVbaApplication"));
}

uno::Sequence< rtl::OUString >
SwVbaApplication::getServiceNames()
{
    static uno::Sequence< rtl::OUString > aServiceNames;
    if ( aServiceNames.getLength() == 0 )
    {
        aServiceNames.realloc( 1 );
        aServiceNames[ 0 ] = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("ooo.vba.word.Application" ) );
    }
    return aServiceNames;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
