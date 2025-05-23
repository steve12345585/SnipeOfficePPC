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

#ifndef OOX_POWERPOINT_QUICKDIAGRAMMINGLAYOUT_HXX
#define OOX_POWERPOINT_QUICKDIAGRAMMINGLAYOUT_HXX

#include "oox/core/xmlfilterbase.hxx"

#include <com/sun/star/animations/XAnimationNode.hpp>
#include <oox/drawingml/theme.hxx>
#include "oox/ppt/presentationfragmenthandler.hxx"
#include "oox/ppt/slidepersist.hxx"
#include <vector>
#include <map>

namespace oox { namespace ppt {

// ---------------------------------------------------------------------

class QuickDiagrammingLayout : public oox::core::XmlFilterBase
{
public:

    QuickDiagrammingLayout( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& rxContext );

    // from FilterBase
    virtual bool importDocument() throw();
    virtual bool exportDocument() throw();

    virtual const ::oox::drawingml::Theme* getCurrentTheme() const;
    virtual sal_Int32 getSchemeClr( sal_Int32 nColorSchemeToken ) const;
    virtual const oox::drawingml::table::TableStyleListPtr getTableStyles();

    virtual ::oox::vml::Drawing* getVmlDrawing();
    virtual ::oox::drawingml::chart::ChartConverter* getChartConverter();

private:
    virtual ::rtl::OUString implGetImplementationName() const;
    virtual ::oox::ole::VbaProject* implCreateVbaProject() const;
    drawingml::ThemePtr mpThemePtr;
};

} }

#endif // OOX_POWERPOINT_QUICKDIAGRAMMINGLAYOUT_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
