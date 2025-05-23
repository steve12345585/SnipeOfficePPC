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

#ifndef OOX_HELPER_GRAPHICHELPER_HXX
#define OOX_HELPER_GRAPHICHELPER_HXX

#include <deque>
#include <map>
#include <rtl/ustring.hxx>
#include <com/sun/star/awt/DeviceInfo.hpp>
#include <com/sun/star/uno/Reference.hxx>
#include "oox/helper/binarystreambase.hxx"
#include "oox/helper/storagebase.hxx"

struct WMF_EXTERNALHEADER;

namespace com { namespace sun { namespace star {
    namespace awt { struct Point; }
    namespace awt { struct Size; }
    namespace awt { class XUnitConversion; }
    namespace io { class XInputStream; }
    namespace frame { class XFrame; }
    namespace graphic { class XGraphic; }
    namespace graphic { class XGraphicObject; }
    namespace graphic { class XGraphicProvider; }
    namespace uno { class XComponentContext; }
} } }

namespace oox {

// ============================================================================

/** Provides helper functions for colors, device measurement conversion,
    graphics, and graphic objects handling.

    All createGraphicObject() and importGraphicObject() functions create
    persistent graphic objects internally and store them in an internal
    container to prevent their early destruction. This makes it possible to use
    the returned URL of the graphic object in any way (e.g. insert it into a
    property map) without needing to store it immediately at an object that
    resolves the graphic object from the passed URL and thus prevents it from
    being destroyed.
 */
class OOX_DLLPUBLIC GraphicHelper
{
public:
    explicit            GraphicHelper(
                            const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& rxContext,
                            const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& rxTargetFrame,
                            const StorageRef& rxStorage );
    virtual             ~GraphicHelper();

    // System colors and predefined colors ------------------------------------

    /** Returns a system color specified by the passed XML token identifier. */
    sal_Int32           getSystemColor( sal_Int32 nToken, sal_Int32 nDefaultRgb = API_RGB_TRANSPARENT ) const;
    /** Derived classes may implement to resolve a scheme color from the passed XML token identifier. */
    virtual sal_Int32   getSchemeColor( sal_Int32 nToken ) const;
    /** Derived classes may implement to resolve a palette index to an RGB color. */
    virtual sal_Int32   getPaletteColor( sal_Int32 nPaletteIdx ) const;

    // Device info and device dependent unit conversion -----------------------

    /** Returns information about the output device. */
    const ::com::sun::star::awt::DeviceInfo& getDeviceInfo() const;

    /** Converts the passed value from horizontal screen pixels to 1/100 mm. */
    sal_Int32           convertScreenPixelXToHmm( double fPixelX ) const;
    /** Converts the passed value from vertical screen pixels to 1/100 mm. */
    sal_Int32           convertScreenPixelYToHmm( double fPixelY ) const;
    /** Converts the passed size from screen pixels to 1/100 mm. */
    ::com::sun::star::awt::Size convertScreenPixelToHmm( const ::com::sun::star::awt::Size& rPixel ) const;

    /** Converts the passed value from 1/100 mm to horizontal screen pixels. */
    double              convertHmmToScreenPixelX( sal_Int32 nHmmX ) const;
    /** Converts the passed value from 1/100 mm to vertical screen pixels. */
    double              convertHmmToScreenPixelY( sal_Int32 nHmmY ) const;
    /** Converts the passed point from 1/100 mm to screen pixels. */
    ::com::sun::star::awt::Point convertHmmToScreenPixel( const ::com::sun::star::awt::Point& rHmm ) const;
    /** Converts the passed size from 1/100 mm to screen pixels. */
    ::com::sun::star::awt::Size convertHmmToScreenPixel( const ::com::sun::star::awt::Size& rHmm ) const;

    /** Converts the passed point from 1/100 mm to AppFont units. */
    ::com::sun::star::awt::Point convertHmmToAppFont( const ::com::sun::star::awt::Point& rHmm ) const;
    /** Converts the passed size from 1/100 mm to AppFont units. */
    ::com::sun::star::awt::Size convertHmmToAppFont( const ::com::sun::star::awt::Size& rHmm ) const;

    // Graphics and graphic objects  ------------------------------------------

    /** Imports a graphic from the passed input stream. */
    ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic >
                        importGraphic(
                            const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& rxInStrm,
                            const WMF_EXTERNALHEADER* pExtHeader = NULL ) const;

    /** Imports a graphic from the passed binary memory block. */
    ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic >
                        importGraphic( const StreamDataSequence& rGraphicData ) const;

    /** Imports a graphic from the storage stream with the passed path and name. */
    ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic >
                        importEmbeddedGraphic( const ::rtl::OUString& rStreamName ) const;

    /** Creates a persistent graphic object from the passed graphic.
        @return  The URL of the created and internally cached graphic object. */
    ::rtl::OUString     createGraphicObject(
                            const ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic >& rxGraphic ) const;

    /** Creates a persistent graphic object from the passed input stream.
        @return  The URL of the created and internally cached graphic object. */
    ::rtl::OUString     importGraphicObject(
                            const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& rxInStrm,
                            const WMF_EXTERNALHEADER* pExtHeader = NULL ) const;

    /** Creates a persistent graphic object from the passed binary memory block.
        @return  The URL of the created and internally cached graphic object. */
    ::rtl::OUString     importGraphicObject( const StreamDataSequence& rGraphicData ) const;

    /** Imports a graphic object from the storage stream with the passed path and name.
        @return  The URL of the created and internally cached graphic object. */
    ::rtl::OUString     importEmbeddedGraphicObject( const ::rtl::OUString& rStreamName ) const;

    /** calculates the orignal size of a graphic which is necessary to be able to calculate cropping values
        @return The original Graphic size in 100thmm */
    ::com::sun::star::awt::Size getOriginalSize( const ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic >& rxGraphic ) const;

    // ------------------------------------------------------------------------
private:
    typedef ::std::map< sal_Int32, sal_Int32 > SystemPalette;
    typedef ::std::deque< ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphicObject > > GraphicObjectDeque;
    typedef ::std::map< ::rtl::OUString, ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphic > > EmbeddedGraphicMap;

    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > mxContext;
    ::com::sun::star::uno::Reference< ::com::sun::star::graphic::XGraphicProvider > mxGraphicProvider;
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XUnitConversion > mxUnitConversion;
    ::com::sun::star::awt::DeviceInfo maDeviceInfo; ///< Current output device info.
    SystemPalette       maSystemPalette;            ///< Maps system colors (XML tokens) to RGB color values.
    StorageRef          mxStorage;                  ///< Storage containing embedded graphics.
    mutable GraphicObjectDeque maGraphicObjects;    ///< Caches all created graphic objects to keep them alive.
    mutable EmbeddedGraphicMap maEmbeddedGraphics;  ///< Maps all embedded graphics by their storage path.
    const ::rtl::OUString maGraphicObjScheme;       ///< The URL scheme name for graphic objects.
    double              mfPixelPerHmmX;             ///< Number of screen pixels per 1/100 mm in X direction.
    double              mfPixelPerHmmY;             ///< Number of screen pixels per 1/100 mm in Y direction.
};

// ============================================================================

} // namespace oox

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
