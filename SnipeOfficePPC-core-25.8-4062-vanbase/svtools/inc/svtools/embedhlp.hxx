/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
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

#ifndef _SVTOOLS_EMBEDHLP_HXX
#define _SVTOOLS_EMBEDHLP_HXX

#include "svtools/svtdllapi.h"
#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/embed/XEmbeddedObject.hpp>
#include <com/sun/star/embed/Aspects.hpp>
#include <com/sun/star/io/XInputStream.hpp>
#include <vcl/graph.hxx>
#include <tools/mapunit.hxx>
#include <rtl/ustring.hxx>

namespace comphelper
{
    class EmbeddedObjectContainer;
}

class Rectangle;
class OutputDevice;
class String;
namespace svt
{
    struct EmbeddedObjectRef_Impl;
    class SVT_DLLPUBLIC EmbeddedObjectRef
    {
        EmbeddedObjectRef_Impl*  mpImp;
        ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject > mxObj;

        SVT_DLLPRIVATE SvStream*   GetGraphicStream( sal_Bool bUpdate ) const;
        /* SVT_DLLPRIVATE */ void        GetReplacement( sal_Bool bUpdate );
        SVT_DLLPRIVATE void        Construct_Impl();

        EmbeddedObjectRef& operator = ( const EmbeddedObjectRef& );

    public:
        const ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject >& operator ->() const { return mxObj; }
        const ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject >& GetObject() const { return mxObj; }

        static void DrawPaintReplacement( const Rectangle &rRect, const String &rText, OutputDevice *pOut );
        static void DrawShading( const Rectangle &rRect, OutputDevice *pOut );
        static sal_Bool TryRunningState( const ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject >& );
        static void SetGraphicToContainer( const Graphic& rGraphic,
                                            comphelper::EmbeddedObjectContainer& aContainer,
                                            const ::rtl::OUString& aName,
                                            const ::rtl::OUString& aMediaType );

        static ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > GetGraphicReplacementStream(
                                            sal_Int64 nViewAspect,
                                            const ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject >&,
                                            ::rtl::OUString* pMediaType )
                                throw();

        // default constructed object; needs further assignment before it can be used
        EmbeddedObjectRef();

        // assign a previously default constructed object
        void Assign( const ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject >& xObj, sal_Int64 nAspect );

        // create object for a certain view aspect
        EmbeddedObjectRef( const ::com::sun::star::uno::Reference < ::com::sun::star::embed::XEmbeddedObject >& xObj, sal_Int64 nAspect );

        ~EmbeddedObjectRef();
        EmbeddedObjectRef( const EmbeddedObjectRef& );

        // assigning to a container enables the object to exchange graphical representations with a storage
        void            AssignToContainer( comphelper::EmbeddedObjectContainer* pContainer, const ::rtl::OUString& rPersistName );
        comphelper::EmbeddedObjectContainer* GetContainer() const;

        sal_Int64       GetViewAspect() const;
        void            SetViewAspect( sal_Int64 nAspect );
        Graphic*        GetGraphic( ::rtl::OUString* pMediaType=0 ) const;

        // the original size of the object ( size of the icon for iconified object )
        // no conversion is done if no target mode is provided
        Size            GetSize( MapMode* pTargetMapMode = NULL ) const;

        void            SetGraphic( const Graphic& rGraphic, const ::rtl::OUString& rMediaType );
        void            SetGraphicStream(
                            const ::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream >& xInGrStream,
                            const ::rtl::OUString& rMediaType );

        void            UpdateReplacement() { GetReplacement( sal_True ); }
        void            UpdateReplacementOnDemand();
        void            Lock( sal_Bool bLock = sal_True );
        sal_Bool            IsLocked() const;
        void            Clear();
        sal_Bool            is() const { return mxObj.is(); }

        sal_Bool            IsChart() const;

        // #i104867#
        // Provides a graphic version number for the fetchable Graphic during this object's lifetime. Internally,
        // that number is incremented at each change of the Graphic. This mechanism is needed to identify if a
        // remembered Graphic (e.g. primitives) has changed compared to the current one, but without actively
        // fetching the Graphic what would be too expensive e.g. for charts
        sal_uInt32 getGraphicVersion() const;
        void            SetDefaultSizeForChart( const Size& rSizeIn_100TH_MM );//#i103460# charts do not necessaryly have an own size within ODF files, in this case they need to use the size settings from the surrounding frame, which is made available with this method
    };
}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
