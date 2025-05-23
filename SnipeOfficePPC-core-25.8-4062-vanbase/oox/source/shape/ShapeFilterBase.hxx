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

#ifndef OOX_SHAPE_SHAPEFILTERBASE_HXX
#define OOX_SHAPE_SHAPEFILTERBASE_HXX

#include <boost/shared_ptr.hpp>
#include <rtl/ref.hxx>
#include "oox/vml/vmldrawing.hxx"
#include "oox/drawingml/table/tablestylelist.hxx"
#include "oox/core/xmlfilterbase.hxx"
#include "oox/drawingml/drawingmltypes.hxx"

namespace oox {
namespace shape {

// ============================================================================

class ShapeFilterBase : public core::XmlFilterBase
{
public:
    typedef boost::shared_ptr<ShapeFilterBase> Pointer_t;

    explicit            ShapeFilterBase(
                            const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& rxContext )
                            throw( ::com::sun::star::uno::RuntimeException );

    virtual             ~ShapeFilterBase();

    /** Has to be implemented by each filter, returns the current theme. */
    virtual const ::oox::drawingml::Theme* getCurrentTheme() const;

    void setCurrentTheme(::oox::drawingml::ThemePtr pTheme);

    /** Has to be implemented by each filter to return the collection of VML shapes. */
    virtual ::oox::vml::Drawing* getVmlDrawing();

    /** Has to be implemented by each filter to return TableStyles. */
    virtual const ::oox::drawingml::table::TableStyleListPtr getTableStyles();

    virtual ::oox::drawingml::chart::ChartConverter* getChartConverter();

    virtual bool importDocument() { return true; }
    virtual bool exportDocument() { return true; }

    sal_Int32 getSchemeColor( sal_Int32 nToken ) const;

private:
    virtual ::oox::ole::VbaProject* implCreateVbaProject() const;
    virtual rtl::OUString implGetImplementationName() const;
    virtual GraphicHelper* implCreateGraphicHelper() const;

    ::boost::shared_ptr< ::oox::drawingml::chart::ChartConverter > mxChartConv;
    ::oox::drawingml::ThemePtr mpTheme;
};

// ============================================================================

} // namespace shape
} // namespace oox

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
