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

#include "oox/ppt/soundactioncontext.hxx"

#include "comphelper/anytostring.hxx"
#include "cppuhelper/exc_hlp.hxx"

#include "oox/helper/attributelist.hxx"
#include "oox/helper/propertymap.hxx"
#include "oox/drawingml/embeddedwavaudiofile.hxx"

using rtl::OUString;
using namespace ::oox::core;
using namespace ::com::sun::star::xml::sax;
using namespace ::com::sun::star::uno;


namespace oox { namespace ppt {


    SoundActionContext::SoundActionContext( FragmentHandler2& rParent, PropertyMap & aProperties ) throw()
    : FragmentHandler2( rParent )
    , maSlideProperties( aProperties )
    , mbHasStartSound( false )
    , mbLoopSound( false )
    , mbStopSound( false )
    {
    }


    SoundActionContext::~SoundActionContext() throw()
    {
    }


    void SoundActionContext::onEndElement()
    {
        if ( isCurrentElement( PPT_TOKEN( sndAc ) ) )
        {
            if( mbHasStartSound )
            {
                OUString url;
                // TODO this is very wrong
                if ( !msSndName.isEmpty() )
                {
                    // try the builtIn version
                    url = msSndName;
                }
#if 0 // OOo does not support embedded data yet
                else if ( msEmbedded.getLength() != 0 )
                {
                    RelationsRef xRel = getHandler()->getRelations();
                    url =   xRel->getRelationById( msEmbedded )->msTarget;
                }
                else if ( msLink.getLength() != 0 )
                {
                    url = msLink;
                }
#endif
                if ( !url.isEmpty() )
                {
                    maSlideProperties[ PROP_Sound ] <<= url;
                    maSlideProperties[ PROP_SoundOn ] <<= sal_True;
                }
            }
//          else if( mbStopSound )
//          {
//              maSlideProperties[ CREATE_OUSTRING( "" ) ] = Any( sal_True );
//          }
        }
    }


    ::oox::core::ContextHandlerRef SoundActionContext::onCreateContext( sal_Int32 aElementToken, const AttributeList& rAttribs )
    {
        switch( aElementToken )
        {
        case PPT_TOKEN( snd ):
            if( mbHasStartSound )
            {
                drawingml::EmbeddedWAVAudioFile aAudio;
                drawingml::getEmbeddedWAVAudioFile( getRelations(), rAttribs.getFastAttributeList(), aAudio);

                msSndName = ( aAudio.mbBuiltIn ? aAudio.msName : aAudio.msEmbed );
            }
            return this;
        case PPT_TOKEN( endSnd ):
            // CT_Empty
            mbStopSound = true;
            return this;
        case PPT_TOKEN( stSnd ):
            mbHasStartSound = true;
            mbLoopSound = rAttribs.getBool( XML_loop, false );
            return this;
        default:
            break;
        }

        return this;
    }



} }

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
