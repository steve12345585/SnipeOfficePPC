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

#ifndef OOX_HELPER_MODELOBJECTHELPER_HXX
#define OOX_HELPER_MODELOBJECTHELPER_HXX

#include <com/sun/star/uno/Reference.hxx>

namespace com { namespace sun { namespace star {
    namespace awt { struct Gradient; }
    namespace container { class XNameContainer; }
    namespace drawing { struct LineDash; }
    namespace drawing { struct PolyPolygonBezierCoords; }
    namespace lang { class XMultiServiceFactory; }
} } }

namespace oox {

// ============================================================================

/** This helper manages named objects in a container, which is created on demand.
 */
class ObjectContainer
{
public:
    explicit            ObjectContainer(
                            const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& rxModelFactory,
                            const ::rtl::OUString& rServiceName );
                        ~ObjectContainer();

    /** Returns true, if the object with the passed name exists in the container. */
    bool                hasObject( const ::rtl::OUString& rObjName ) const;

    /** Inserts the passed object into the container, returns its final name. */
    ::rtl::OUString     insertObject(
                            const ::rtl::OUString& rObjName,
                            const ::com::sun::star::uno::Any& rObj,
                            bool bInsertByUnusedName );

private:
    void                createContainer() const;

private:
    mutable ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >
                        mxModelFactory;         ///< Factory to create the container.
    mutable ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameContainer >
                        mxContainer;            ///< Container for the objects.
    ::rtl::OUString     maServiceName;          ///< Service name to create the container.
    sal_Int32           mnIndex;                ///< Index to create unique identifiers.
};

// ============================================================================

/** Contains tables for named drawing objects for a document model.

    Contains tables for named line markers, line dashes, fill gradients, and
    fill bitmap URLs. The class is needed to handle different document models
    in the same filter (e.g. embedded charts) which carry their own drawing
    object tables.
 */
class ModelObjectHelper
{
public:
    explicit            ModelObjectHelper(
                            const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& rxModelFactory );

    /** Returns true, if the model contains a line marker with the passed name. */
    bool                hasLineMarker( const ::rtl::OUString& rMarkerName ) const;

    /** Inserts a new named line marker, overwrites an existing line marker
        with the same name. Returns true, if the marker could be inserted. */
    bool                insertLineMarker(
                            const ::rtl::OUString& rMarkerName,
                            const ::com::sun::star::drawing::PolyPolygonBezierCoords& rMarker );

    /** Inserts a new named line dash, returns the line dash name, based on an
        internal constant name with a new unused index appended. */
    ::rtl::OUString     insertLineDash( const ::com::sun::star::drawing::LineDash& rDash );

    /** Inserts a new named fill gradient, returns the gradient name, based on
        an internal constant name with a new unused index appended. */
    ::rtl::OUString     insertFillGradient( const ::com::sun::star::awt::Gradient& rGradient );

    ::rtl::OUString     insertTransGrandient( const ::com::sun::star::awt::Gradient& rGradient );

    /** Inserts a new named fill bitmap URL, returns the bitmap name, based on
        an internal constant name with a new unused index appended. */
    ::rtl::OUString     insertFillBitmapUrl( const ::rtl::OUString& rGraphicUrl );

private:
    ObjectContainer     maMarkerContainer;      ///< Contains all named line markers (line end polygons).
    ObjectContainer     maDashContainer;        ///< Contains all named line dsahes.
    ObjectContainer     maGradientContainer;    ///< Contains all named fill gradients.
    ObjectContainer     maTransGradContainer;   ///< Contains all named transparency Gradients.
    ObjectContainer     maBitmapUrlContainer;   ///< Contains all named fill bitmap URLs.
    const ::rtl::OUString maDashNameBase;       ///< Base name for all named line dashes.
    const ::rtl::OUString maGradientNameBase;   ///< Base name for all named fill gradients.
    const ::rtl::OUString maTransGradNameBase;   ///< Base name for all named fill gradients.
    const ::rtl::OUString maBitmapUrlNameBase;  ///< Base name for all named fill bitmap URLs.
};

// ============================================================================

} // namespace oox

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
