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

#include "comphelper/anytostring.hxx"
#include "cppuhelper/exc_hlp.hxx"
#include <osl/diagnose.h>
#include <rtl/ustrbuf.hxx>

#include <com/sun/star/animations/XTimeContainer.hpp>
#include <com/sun/star/animations/XAnimationNode.hpp>
#include <com/sun/star/animations/XAnimate.hpp>

#include "oox/core/fragmenthandler.hxx"

#include "commonbehaviorcontext.hxx"
#include "commontimenodecontext.hxx"
#include "timetargetelementcontext.hxx"
#include "pptfilterhelpers.hxx"

#include <string.h>

using ::rtl::OUString;
using ::rtl::OUStringBuffer;
using namespace ::oox::core;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::xml::sax;
using namespace ::com::sun::star::animations;

namespace oox { namespace ppt {

    CommonBehaviorContext::CommonBehaviorContext( FragmentHandler2& rParent,
            const Reference< XFastAttributeList >& xAttribs,
            const TimeNodePtr & pNode )
        : TimeNodeContext( rParent, PPT_TOKEN( cBhvr ), xAttribs, pNode )
            , mbInAttrList( false )
            , mbIsInAttrName( false )
    {
    }


    CommonBehaviorContext::~CommonBehaviorContext( ) throw( )
    {
    }



    void CommonBehaviorContext::onEndElement()
    {
        switch( getCurrentElement() )
        {
        case PPT_TOKEN( cBhvr ):
        {
            if( !maAttributes.empty() )
            {
                OUStringBuffer sAttributes;
                std::list< Attribute >::const_iterator iter;
                for(iter = maAttributes.begin(); iter != maAttributes.end(); ++iter)
                {
                    if( sAttributes.getLength() )
                    {
                        sAttributes.appendAscii( ";" );
                    }
                    sAttributes.append( iter->name );
                }
                OUString sTmp( sAttributes.makeStringAndClear() );
                mpNode->getNodeProperties()[ NP_ATTRIBUTENAME ] = makeAny( sTmp );
            }
            break;
        }
        case PPT_TOKEN( attrNameLst ):
            mbInAttrList = false;
            break;
        case PPT_TOKEN( attrName ):
            if( mbIsInAttrName )
            {
                const ImplAttributeNameConversion *attrConv = gImplConversionList;
                while( attrConv->mpMSName != NULL )
                {
                    if(msCurrentAttribute.compareToAscii( attrConv->mpMSName ) == 0 )
                    {
                        Attribute attr;
                        attr.name = ::rtl::OUString::intern( attrConv->mpAPIName,
                                                             strlen(attrConv->mpAPIName),
                                                             RTL_TEXTENCODING_ASCII_US );
                        attr.type = attrConv->meAttribute;
                        maAttributes.push_back( attr );
                        OSL_TRACE( "OOX: attrName is %s -> %s",
                                   OUSTRING_TO_CSTR( msCurrentAttribute ),
                                   attrConv->mpAPIName );
                        break;
                    }
                    attrConv++;
                }
                mbIsInAttrName = false;
            }
            break;
        default:
            break;
        }
    }


    void CommonBehaviorContext::onCharacters( const OUString& aChars )
    {
        if( mbIsInAttrName )
        {
            msCurrentAttribute += aChars;
        }
    }


    ::oox::core::ContextHandlerRef CommonBehaviorContext::onCreateContext( sal_Int32 aElementToken, const AttributeList& rAttribs )
    {
        switch ( aElementToken )
        {
        case PPT_TOKEN( cTn ):
            return new CommonTimeNodeContext( *this, aElementToken, rAttribs.getFastAttributeList(), mpNode );
        case PPT_TOKEN( tgtEl ):
            return new TimeTargetElementContext( *this, mpNode->getTarget() );
        case PPT_TOKEN( attrNameLst ):
            mbInAttrList = true;
            return this;
        case PPT_TOKEN( attrName ):
        {
            if( mbInAttrList )
            {
                mbIsInAttrName = true;
                msCurrentAttribute = OUString();
            }
            else
            {
                OSL_TRACE( "OOX: Attribute Name outside an Attribute List" );
            }
            return this;
        }
        default:
            break;
        }

        return this;
    }

} }

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
