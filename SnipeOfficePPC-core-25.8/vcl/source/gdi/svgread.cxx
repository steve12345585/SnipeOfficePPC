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

#include <tools/stream.hxx>
#include <vcl/svgread.hxx>
#include <vcl/gdimtf.hxx>
#include <vcl/metaact.hxx>
#include <vcl/rendergraphicrasterizer.hxx>

// -----------
// - Defines -
// -----------

namespace vcl
{
    // -----------------
    // - SVGReaderImpl -
    // -----------------

    class SVGReaderImpl
    {
    public:

        SVGReaderImpl( SvStream& rStm );
        ~SVGReaderImpl();

        GDIMetaFile& ImplRead( GDIMetaFile& rSVGMtf );
        vcl::RenderGraphic ImplGetRenderGraphic();

    private:

        SvStream&           mrStm;
    };

    // ------------------------------------------------------------------------------

    SVGReaderImpl::SVGReaderImpl( SvStream& rStm ) :
        mrStm( rStm )
    {
    }

    // ------------------------------------------------------------------------

    SVGReaderImpl::~SVGReaderImpl()
    {
    }

    // ------------------------------------------------------------------------

    GDIMetaFile& SVGReaderImpl::ImplRead( GDIMetaFile& rSVGMtf )
    {
        vcl::RenderGraphic aSVGGraphic = ImplGetRenderGraphic();

        if( !mrStm.GetError() )
        {
            const vcl::RenderGraphicRasterizer  aRasterizer( aSVGGraphic );
            const Size                          aDefaultSizePixel( aRasterizer.GetDefaultSizePixel() );

            if( aDefaultSizePixel.Width() && aDefaultSizePixel.Height() )
            {
                const Point aPos;
                const Size  aPrefSize( aRasterizer.GetPrefSize() );

                rSVGMtf.SetPrefMapMode( aRasterizer.GetPrefMapMode() );
                rSVGMtf.SetPrefSize( aPrefSize );
                rSVGMtf.AddAction( new MetaRenderGraphicAction( aPos, aPrefSize, aSVGGraphic ) );
                rSVGMtf.WindStart();
            }
        }

        return( rSVGMtf );
    }

    vcl::RenderGraphic SVGReaderImpl::ImplGetRenderGraphic()
    {
        const sal_uInt32 nStmPos = mrStm.Tell();
        const sal_uInt32 nStmLen = mrStm.Seek( STREAM_SEEK_TO_END ) - nStmPos;

        vcl::RenderGraphic aSVGGraphic( ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("image/svg+xml")), nStmLen );
        mrStm.Seek( nStmPos );
        mrStm.Read( aSVGGraphic.GetGraphicData().get(), nStmLen );

        return( aSVGGraphic );
    }


    // -------------
    // - SVGReader -
    // -------------

    SVGReader::SVGReader( SvStream& rIStm ) :
        mapImpl( new ::vcl::SVGReaderImpl( rIStm ) )
    {
    }

    // ------------------------------------------------------------------------

    SVGReader::~SVGReader()
    {
    }

    // ------------------------------------------------------------------------

    GDIMetaFile& SVGReader::Read( GDIMetaFile& rSVGMtf )
    {
        rSVGMtf = GDIMetaFile();

        return( mapImpl.get() ? mapImpl->ImplRead( rSVGMtf ) : rSVGMtf );
    }

    vcl::RenderGraphic SVGReader::GetRenderGraphic()
    {
        return( mapImpl->ImplGetRenderGraphic() );
    }

} // namespace vcl

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
