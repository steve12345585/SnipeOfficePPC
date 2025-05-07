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

#include "swdetect.hxx"

#include <framework/interaction.hxx>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <comphelper/processfactory.hxx>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/task/XInteractionHandler.hpp>
#include <com/sun/star/lang/WrappedTargetRuntimeException.hpp>
#include <com/sun/star/ucb/CommandAbortedException.hpp>
#include <com/sun/star/ucb/InteractiveAppException.hpp>
#include <com/sun/star/ucb/XContent.hpp>
#include <com/sun/star/packages/zip/ZipIOException.hpp>
#include <toolkit/helper/vclunohelper.hxx>
#include <ucbhelper/simpleinteractionrequest.hxx>
#include <rtl/ustring.h>
#include <rtl/logfile.hxx>
#include <svl/itemset.hxx>
#include <vcl/window.hxx>
#include <svl/eitem.hxx>
#include <svl/stritem.hxx>
#include <tools/urlobj.hxx>
#include <osl/mutex.hxx>
#include <svtools/sfxecode.hxx>
#include <svtools/ehdl.hxx>
#include <sot/storinfo.hxx>
#include <vcl/svapp.hxx>
#include <sfx2/app.hxx>
#include <sfx2/sfxsids.hrc>
#include <sfx2/request.hxx>
#include <sfx2/docfile.hxx>
#include <sfx2/docfilt.hxx>
#include <sfx2/fcontnr.hxx>
#include <sfx2/brokenpackageint.hxx>
#include <svtools/FilterConfigItem.hxx>
#include <unotools/moduleoptions.hxx>
#include <com/sun/star/util/XArchiver.hpp>
#include <comphelper/ihwrapnofilter.hxx>

#include <swdll.hxx>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::io;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::task;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::ucb;
using ::rtl::OUString;

SwFilterDetect::SwFilterDetect( const REFERENCE < lang::XMultiServiceFactory >& /*xFactory*/ )
{
}

SwFilterDetect::~SwFilterDetect()
{
}

::rtl::OUString SAL_CALL SwFilterDetect::detect( uno::Sequence< beans::PropertyValue >& lDescriptor ) throw( uno::RuntimeException )
{
    REFERENCE< XInputStream > xStream;
    REFERENCE< XContent > xContent;
    REFERENCE< XInteractionHandler > xInteraction;
    String aURL;
    ::rtl::OUString sTemp;
    String aTypeName;            // a name describing the type (from MediaDescriptor, usually from flat detection)
    String aPreselectedFilterName;      // a name describing the filter to use (from MediaDescriptor, usually from UI action)

    ::rtl::OUString aDocumentTitle; // interesting only if set in this method

    // opening as template is done when a parameter tells to do so and a template filter can be detected
    // (otherwise no valid filter would be found) or if the detected filter is a template filter and
    // there is no parameter that forbids to open as template
    sal_Bool bOpenAsTemplate = sal_False;
    sal_Bool bWasReadOnly = sal_False, bReadOnly = sal_False;

    sal_Bool bRepairPackage = sal_False;
    sal_Bool bRepairAllowed = sal_False;

    // now some parameters that can already be in the array, but may be overwritten or new inserted here
    // remember their indices in the case new values must be added to the array
    sal_Int32 nPropertyCount = lDescriptor.getLength();
    sal_Int32 nIndexOfInputStream = -1;
    sal_Int32 nIndexOfContent = -1;
    sal_Int32 nIndexOfReadOnlyFlag = -1;
    sal_Int32 nIndexOfTemplateFlag = -1;
    sal_Int32 nIndexOfDocumentTitle = -1;
    sal_Int32 nIndexOfInteractionHandler = -1;

    for( sal_Int32 nProperty=0; nProperty<nPropertyCount; ++nProperty )
    {
        // extract properties
        if ( lDescriptor[nProperty].Name == "URL" )
        {
            lDescriptor[nProperty].Value >>= sTemp;
            aURL = sTemp;
        }
        else if( !aURL.Len() && lDescriptor[nProperty].Name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM("FileName")) )
        {
            lDescriptor[nProperty].Value >>= sTemp;
            aURL = sTemp;
        }
        else if ( lDescriptor[nProperty].Name == "TypeName" )
        {
            lDescriptor[nProperty].Value >>= sTemp;
            aTypeName = sTemp;
        }
        else if ( lDescriptor[nProperty].Name == "FilterName" )
        {
            lDescriptor[nProperty].Value >>= sTemp;
            aPreselectedFilterName = sTemp;
        }
        else if ( lDescriptor[nProperty].Name == "InputStream" )
            nIndexOfInputStream = nProperty;
        else if ( lDescriptor[nProperty].Name == "ReadOnly" )
            nIndexOfReadOnlyFlag = nProperty;
        else if ( lDescriptor[nProperty].Name == "UCBContent" )
            nIndexOfContent = nProperty;
        else if ( lDescriptor[nProperty].Name == "AsTemplate" )
        {
            lDescriptor[nProperty].Value >>= bOpenAsTemplate;
            nIndexOfTemplateFlag = nProperty;
        }
        else if ( lDescriptor[nProperty].Name == "InteractionHandler" )
        {
            lDescriptor[nProperty].Value >>= xInteraction;
            nIndexOfInteractionHandler = nProperty;
        }
        else if ( lDescriptor[nProperty].Name == "RepairPackage" )
            lDescriptor[nProperty].Value >>= bRepairPackage;
        else if ( lDescriptor[nProperty].Name == "DocumentTitle" )
            nIndexOfDocumentTitle = nProperty;
    }

    SolarMutexGuard aGuard;

    SfxApplication* pApp = SFX_APP();
    SfxAllItemSet *pSet = new SfxAllItemSet( pApp->GetPool() );
    TransformParameters( SID_OPENDOC, lDescriptor, *pSet );
    SFX_ITEMSET_ARG( pSet, pItem, SfxBoolItem, SID_DOC_READONLY, sal_False );

    bWasReadOnly = pItem && pItem->GetValue();

    const SfxFilter* pFilter = 0;
    String aPrefix = String::CreateFromAscii( "private:factory/" );
    if( aURL.Match( aPrefix ) == aPrefix.Len() )
    {
        if( SvtModuleOptions().IsWriter() )
        {
            String aPattern( aPrefix );
            aPattern += String::CreateFromAscii("swriter");
            if ( aURL.Match( aPattern ) >= aPattern.Len() )
                return aTypeName;
        }
    }
    else
    {
        // ctor of SfxMedium uses owner transition of ItemSet
        SfxMedium aMedium( aURL, bWasReadOnly ? STREAM_STD_READ : STREAM_STD_READWRITE, NULL, pSet );
        aMedium.UseInteractionHandler( sal_True );
        if ( aMedium.GetErrorCode() == ERRCODE_NONE )
        {
            // remember input stream and content and put them into the descriptor later
            // should be done here since later the medium can switch to a version
            xStream = aMedium.GetInputStream();
            xContent = aMedium.GetContent();
            bReadOnly = aMedium.IsReadOnly();

            sal_Bool bIsStorage = aMedium.IsStorage();
            if ( bIsStorage )
            {
                uno::Reference< embed::XStorage > xStorage = aMedium.GetStorage( sal_False );
                if ( aMedium.GetLastStorageCreationState() != ERRCODE_NONE )
                {
                    // error during storage creation means _here_ that the medium
                    // is broken, but we can not handle it in medium since impossibility
                    // to create a storage does not _always_ means that the medium is broken
                    aMedium.SetError( aMedium.GetLastStorageCreationState(), ::rtl::OUString( OSL_LOG_PREFIX  ) );
                    if ( xInteraction.is() )
                    {
                        OUString empty;
                        try
                        {
                            InteractiveAppException xException( empty,
                                                            REFERENCE< XInterface >(),
                                                            InteractionClassification_ERROR,
                                                            aMedium.GetError() );

                            REFERENCE< XInteractionRequest > xRequest(
                                new ucbhelper::SimpleInteractionRequest( makeAny( xException ),
                                                                      ucbhelper::CONTINUATION_APPROVE ) );
                            xInteraction->handle( xRequest );
                        }
                        catch (const Exception&)
                        {
                        }
                    }
                }
                else
                {
                    OSL_ENSURE( xStorage.is(), "At this point storage must exist!" );

                    try
                    {
                        const SfxFilter* pPreFilter = aPreselectedFilterName.Len() ?
                                SfxFilterMatcher().GetFilter4FilterName( aPreselectedFilterName ) : aTypeName.Len() ?
                                SfxFilterMatcher(String::CreateFromAscii("swriter")).GetFilter4EA( aTypeName ) : 0;
                        if (!pPreFilter)
                            pPreFilter = SfxFilterMatcher(String::CreateFromAscii("sweb")).GetFilter4EA( aTypeName );
                        String aFilterName;
                        if ( pPreFilter )
                        {
                            aFilterName = pPreFilter->GetName();
                            aTypeName = pPreFilter->GetTypeName();
                        }

                        aTypeName = SfxFilter::GetTypeFromStorage( xStorage, pPreFilter ? pPreFilter->IsOwnTemplateFormat() : sal_False, &aFilterName );
                    }
                    catch (const lang::WrappedTargetException& aWrap)
                    {
                        packages::zip::ZipIOException aZipException;

                        // repairing is done only if this type is requested from outside
                        // we don't do any type detection on broken packages (f.e. because it might be impossible), so any requested
                        // type will be accepted if the user allows to repair the file
                        if ( ( aWrap.TargetException >>= aZipException ) && ( aTypeName.Len() || aPreselectedFilterName.Len() ) )
                        {
                            if ( xInteraction.is() )
                            {
                                // the package is a broken one
                                   aDocumentTitle = aMedium.GetURLObject().getName(
                                                            INetURLObject::LAST_SEGMENT,
                                                            true,
                                                            INetURLObject::DECODE_WITH_CHARSET );

                                if ( !bRepairPackage )
                                {
                                    // ask the user whether he wants to try to repair
                                    RequestPackageReparation aRequest( aDocumentTitle );
                                    xInteraction->handle( aRequest.GetRequest() );
                                    bRepairAllowed = aRequest.isApproved();
                                }

                                if ( !bRepairAllowed )
                                {
                                    // repair either not allowed or not successful
                                    // repair either not allowed or not successful
                                    NotifyBrokenPackage aNotifyRequest( aDocumentTitle );
                                    xInteraction->handle( aNotifyRequest.GetRequest() );

                                    rtl::Reference< ::comphelper::OIHWrapNoFilterDialog > xHandler = new ::comphelper::OIHWrapNoFilterDialog( xInteraction );
                                    if ( nIndexOfInteractionHandler != -1 )
                                        lDescriptor[nIndexOfInteractionHandler].Value <<= uno::Reference< XInteractionHandler >( static_cast< task::XInteractionHandler* >( xHandler.get() ) );

                                    aMedium.SetError( ERRCODE_ABORT, ::rtl::OUString( OSL_LOG_PREFIX  ) );
                                }
                            }
                            else
                                // no interaction, error handling as usual
                                aMedium.SetError( ERRCODE_IO_BROKENPACKAGE, ::rtl::OUString( OSL_LOG_PREFIX  ) );

                            if ( !bRepairAllowed )
                            {
                                aTypeName.Erase();
                                aPreselectedFilterName.Erase();
                            }
                        }
                    }
                    catch (const uno::RuntimeException&)
                    {
                        throw;
                    }
                    catch (const uno::Exception&)
                    {
                        aTypeName.Erase();
                        aPreselectedFilterName.Erase();
                    }
                }
            }
            else
            {
                aMedium.GetInStream();
                if ( aMedium.GetErrorCode() == ERRCODE_NONE )
                {
                    if ( aPreselectedFilterName.Len() )
                        pFilter = SfxFilter::GetFilterByName( aPreselectedFilterName );
                    else
                        pFilter = SfxFilterMatcher().GetFilter4EA( aTypeName );

                    sal_Bool bTestWriter = !pFilter || pFilter->GetServiceName().EqualsAscii("com.sun.star.text.TextDocument") ||
                        pFilter->GetServiceName().EqualsAscii("com.sun.star.text.WebDocument");
                    sal_Bool bTestGlobal = !pFilter || pFilter->GetServiceName().EqualsAscii("com.sun.star.text.GlobalDocument");

                    const SfxFilter* pOrigFilter = NULL;
                    if ( !bTestWriter && !bTestGlobal && pFilter )
                    {
                        // cross filter; now this should be a type detection only, not a filter detection
                        // we can simulate it by preserving the preselected filter if the type matches
                        // example: HTML filter for Calc
                        pOrigFilter = pFilter;
                        pFilter = SfxFilterMatcher().GetFilter4EA( pFilter->GetTypeName() );
                        bTestWriter = sal_True;
                    }

                    sal_uLong nErr = ERRCODE_NONE;
                    if ( pFilter || bTestWriter )
                        nErr = DetectFilter( aMedium, &pFilter );
                    if ( nErr != ERRCODE_NONE )
                        pFilter = NULL;
                    else if ( pOrigFilter && pFilter && pFilter->GetTypeName() == pOrigFilter->GetTypeName() )
                        // cross filter, see above
                        pFilter = pOrigFilter;
                }

                if ( pFilter )
                    aTypeName = pFilter->GetTypeName();
                else
                    aTypeName.Erase();
            }
        }
    }

    if ( nIndexOfInputStream == -1 && xStream.is() )
    {
        // if input stream wasn't part of the descriptor, now it should be, otherwise the content would be opend twice
        lDescriptor.realloc( nPropertyCount + 1 );
        lDescriptor[nPropertyCount].Name = ::rtl::OUString("InputStream");
        lDescriptor[nPropertyCount].Value <<= xStream;
        nPropertyCount++;
    }

    if ( nIndexOfContent == -1 && xContent.is() )
    {
        // if input stream wasn't part of the descriptor, now it should be, otherwise the content would be opend twice
        lDescriptor.realloc( nPropertyCount + 1 );
        lDescriptor[nPropertyCount].Name = ::rtl::OUString("UCBContent");
        lDescriptor[nPropertyCount].Value <<= xContent;
        nPropertyCount++;
    }

    if ( bReadOnly != bWasReadOnly )
    {
        if ( nIndexOfReadOnlyFlag == -1 )
        {
            lDescriptor.realloc( nPropertyCount + 1 );
            lDescriptor[nPropertyCount].Name = ::rtl::OUString("ReadOnly");
            lDescriptor[nPropertyCount].Value <<= bReadOnly;
            nPropertyCount++;
        }
        else
            lDescriptor[nIndexOfReadOnlyFlag].Value <<= bReadOnly;
    }

    if ( !bRepairPackage && bRepairAllowed )
    {
        lDescriptor.realloc( nPropertyCount + 1 );
        lDescriptor[nPropertyCount].Name = ::rtl::OUString("RepairPackage");
        lDescriptor[nPropertyCount].Value <<= bRepairAllowed;
        nPropertyCount++;
        bOpenAsTemplate = sal_True;
        // TODO/LATER: set progress bar that should be used
    }

    if ( bOpenAsTemplate )
    {
        if ( nIndexOfTemplateFlag == -1 )
        {
            lDescriptor.realloc( nPropertyCount + 1 );
            lDescriptor[nPropertyCount].Name = ::rtl::OUString("AsTemplate");
            lDescriptor[nPropertyCount].Value <<= bOpenAsTemplate;
            nPropertyCount++;
        }
        else
            lDescriptor[nIndexOfTemplateFlag].Value <<= bOpenAsTemplate;
    }

    if ( !aDocumentTitle.isEmpty() )
    {
        // the title was set here
        if ( nIndexOfDocumentTitle == -1 )
        {
            lDescriptor.realloc( nPropertyCount + 1 );
            lDescriptor[nPropertyCount].Name = ::rtl::OUString("DocumentTitle");
            lDescriptor[nPropertyCount].Value <<= aDocumentTitle;
            nPropertyCount++;
        }
        else
            lDescriptor[nIndexOfDocumentTitle].Value <<= aDocumentTitle;
    }


    return aTypeName;
}

/* XServiceInfo */
rtl::OUString SAL_CALL SwFilterDetect::getImplementationName() throw( UNORUNTIMEEXCEPTION )
{
    return impl_getStaticImplementationName();
}
                                                                                                                                \
/* XServiceInfo */
sal_Bool SAL_CALL SwFilterDetect::supportsService( const rtl::OUString& sServiceName ) throw( UNORUNTIMEEXCEPTION )
{
    UNOSEQUENCE< rtl::OUString > seqServiceNames = getSupportedServiceNames();
    const rtl::OUString*         pArray          = seqServiceNames.getConstArray();
    for ( sal_Int32 nCounter=0; nCounter<seqServiceNames.getLength(); nCounter++ )
    {
        if ( pArray[nCounter] == sServiceName )
        {
            return sal_True ;
        }
    }
    return sal_False ;
}

/* XServiceInfo */
UNOSEQUENCE< rtl::OUString > SAL_CALL SwFilterDetect::getSupportedServiceNames() throw( UNORUNTIMEEXCEPTION )
{
    return impl_getStaticSupportedServiceNames();
}

/* Helper for XServiceInfo */
UNOSEQUENCE< rtl::OUString > SwFilterDetect::impl_getStaticSupportedServiceNames()
{
    UNOSEQUENCE< rtl::OUString > seqServiceNames( 3 );
    seqServiceNames.getArray() [0] = ::rtl::OUString("com.sun.star.frame.ExtendedTypeDetection"  );
    seqServiceNames.getArray() [1] = ::rtl::OUString("com.sun.star.text.FormatDetector"  );
    seqServiceNames.getArray() [2] = ::rtl::OUString("com.sun.star.text.W4WFormatDetector"  );
    return seqServiceNames ;
}

/* Helper for XServiceInfo */
rtl::OUString SwFilterDetect::impl_getStaticImplementationName()
{
    return ::rtl::OUString("com.sun.star.comp.writer.FormatDetector" );
}

/* Helper for registry */
UNOREFERENCE< UNOXINTERFACE > SAL_CALL SwFilterDetect::impl_createInstance( const UNOREFERENCE< UNOXMULTISERVICEFACTORY >& xServiceManager ) throw( UNOEXCEPTION )
{
    return UNOREFERENCE< UNOXINTERFACE >( *new SwFilterDetect( xServiceManager ) );
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
