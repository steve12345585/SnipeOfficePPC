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

#ifndef OOX_DRAWINGML_DIAGRAM_DIAGRAM_HXX
#define OOX_DRAWINGML_DIAGRAM_DIAGRAM_HXX

#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <rtl/ustring.hxx>

#include "oox/drawingml/shape.hxx"
#include "oox/drawingml/fillproperties.hxx"

namespace com { namespace sun { namespace star {
    namespace xml { namespace dom { class XDocument; } }
} } }

namespace oox { namespace drawingml {

namespace dgm {

/** A Connection 
 */
struct Connection
{
    Connection() : 
        mnType( 0 ),
        mnSourceOrder( 0 ),
        mnDestOrder( 0 )
    {}

    void dump();

    sal_Int32 mnType;
    ::rtl::OUString msModelId;
    ::rtl::OUString msSourceId;
    ::rtl::OUString msDestId;
    ::rtl::OUString msParTransId;
    ::rtl::OUString msPresId;
    ::rtl::OUString msSibTransId;
    sal_Int32 mnSourceOrder;
    sal_Int32 mnDestOrder;
    
};

typedef std::vector< Connection > Connections;


/** A point
 */
struct Point
{
    Point() : 
        mnType(0),
        mnMaxChildren(-1),
        mnPreferredChildren(-1),
        mnDirection(XML_norm),
        mnHierarchyBranch(XML_std),
        mnResizeHandles(XML_rel),
        mnCustomAngle(-1),
        mnPercentageNeighbourWidth(-1),
        mnPercentageNeighbourHeight(-1),
        mnPercentageOwnWidth(-1),
        mnPercentageOwnHeight(-1),
        mnIncludeAngleScale(-1),
        mnRadiusScale(-1),
        mnWidthScale(-1),
        mnHeightScale(-1),
        mnWidthOverride(-1),
        mnHeightOverride(-1),
        mnLayoutStyleCount(-1),
        mnLayoutStyleIndex(-1),

        mbOrgChartEnabled(false),
        mbBulletEnabled(false),
        mbCoherent3DOffset(false),
        mbCustomHorizontalFlip(false),
        mbCustomVerticalFlip(false),
        mbCustomText(false),
        mbIsPlaceholder(false)
    {}
    void dump();

    ShapePtr      mpShape;

    rtl::OUString msCnxId;
    rtl::OUString msModelId;
    rtl::OUString msColorTransformCategoryId;
    rtl::OUString msColorTransformTypeId;
    rtl::OUString msLayoutCategoryId;
    rtl::OUString msLayoutTypeId;
    rtl::OUString msPlaceholderText;
    rtl::OUString msPresentationAssociationId;
    rtl::OUString msPresentationLayoutName;
    rtl::OUString msPresentationLayoutStyleLabel;
    rtl::OUString msQuickStyleCategoryId;
    rtl::OUString msQuickStyleTypeId; 

    sal_Int32     mnType;
    sal_Int32     mnMaxChildren;
    sal_Int32     mnPreferredChildren;
    sal_Int32     mnDirection;
    sal_Int32     mnHierarchyBranch;
    sal_Int32     mnResizeHandles;
    sal_Int32     mnCustomAngle;
    sal_Int32     mnPercentageNeighbourWidth;
    sal_Int32     mnPercentageNeighbourHeight;
    sal_Int32     mnPercentageOwnWidth;
    sal_Int32     mnPercentageOwnHeight;
    sal_Int32     mnIncludeAngleScale;
    sal_Int32     mnRadiusScale;
    sal_Int32     mnWidthScale;
    sal_Int32     mnHeightScale;
    sal_Int32     mnWidthOverride;
    sal_Int32     mnHeightOverride;
    sal_Int32     mnLayoutStyleCount;
    sal_Int32     mnLayoutStyleIndex;

    bool          mbOrgChartEnabled;
    bool          mbBulletEnabled;
    bool          mbCoherent3DOffset;
    bool          mbCustomHorizontalFlip;
    bool          mbCustomVerticalFlip;
    bool          mbCustomText;
    bool          mbIsPlaceholder;
};

typedef std::vector< Point >        Points;

}

class LayoutNode;
typedef boost::shared_ptr< LayoutNode > LayoutNodePtr;

////////////////////

class DiagramData
{
public:
    ::std::vector<rtl::OUString>  maExtDrawings;
    typedef std::map< rtl::OUString, dgm::Point* > PointNameMap;
    typedef std::map< rtl::OUString,
                      std::vector<dgm::Point*> >   PointsNameMap;
    typedef std::map< rtl::OUString, const dgm::Connection* > ConnectionNameMap;
    typedef std::map< rtl::OUString,
                      std::vector<std::pair<rtl::OUString,sal_Int32> > > StringMap;

    DiagramData();
    FillPropertiesPtr & getFillProperties()
        { return mpFillProperties; }
    dgm::Connections & getConnections()
        { return maConnections; }
    dgm::Points & getPoints()
        { return maPoints; }
    ConnectionNameMap & getConnectionNameMap()
        { return maConnectionNameMap; }
    StringMap & getPresOfNameMap()
        { return maPresOfNameMap; }
    PointNameMap & getPointNameMap()
        { return maPointNameMap; }
    PointsNameMap & getPointsPresNameMap()
        { return maPointsPresNameMap; }
    ::std::vector<rtl::OUString> &getExtDrawings()
        { return maExtDrawings; }
    void dump();
private:
    FillPropertiesPtr mpFillProperties;
    dgm::Connections  maConnections;
    dgm::Points       maPoints;
    PointNameMap      maPointNameMap;
    PointsNameMap     maPointsPresNameMap;
    ConnectionNameMap maConnectionNameMap;
    StringMap         maPresOfNameMap;
};

typedef boost::shared_ptr< DiagramData > DiagramDataPtr;



////////////////////

class DiagramLayout
{
public:
    void setDefStyle( const ::rtl::OUString & sDefStyle )
        { msDefStyle = sDefStyle; }
    void setMinVer( const ::rtl::OUString & sMinVer )
        { msMinVer = sMinVer; }
    void setUniqueId( const ::rtl::OUString & sUniqueId )
        { msUniqueId = sUniqueId; }
    const ::rtl::OUString & getUniqueId()
        { return msUniqueId; }
    void setTitle( const ::rtl::OUString & sTitle )
        { msTitle = sTitle; }
    void setDesc( const ::rtl::OUString & sDesc )
        { msDesc = sDesc; }

    LayoutNodePtr & getNode()
        { return mpNode; }
    const LayoutNodePtr & getNode() const
        { return mpNode; }
    DiagramDataPtr & getSampData()
        { return mpSampData; }
    const DiagramDataPtr & getSampData() const
        { return mpSampData; }
    DiagramDataPtr & getStyleData()
        { return mpStyleData; }
    const DiagramDataPtr & getStyleData() const
        { return mpStyleData; }

private:
    ::rtl::OUString msDefStyle;
    ::rtl::OUString msMinVer;
    ::rtl::OUString msUniqueId;

    ::rtl::OUString msTitle;
    ::rtl::OUString msDesc;
    LayoutNodePtr  mpNode;
    DiagramDataPtr mpSampData;
    DiagramDataPtr mpStyleData;
    // TODO
    // catLst
    // clrData
};

typedef boost::shared_ptr< DiagramLayout > DiagramLayoutPtr;

///////////////////////

struct DiagramStyle
{
    ShapeStyleRef maFillStyle;
    ShapeStyleRef maLineStyle;
    ShapeStyleRef maEffectStyle;
    ShapeStyleRef maTextStyle;
};

typedef std::map<rtl::OUString,DiagramStyle> DiagramQStyleMap;

struct DiagramColor
{
    oox::drawingml::Color maFillColor;
    oox::drawingml::Color maLineColor;
    oox::drawingml::Color maEffectColor;
    oox::drawingml::Color maTextFillColor;
    oox::drawingml::Color maTextLineColor;
    oox::drawingml::Color maTextEffectColor;
};

typedef std::map<rtl::OUString,DiagramColor> DiagramColorMap;

///////////////////////

class Diagram
{
public:
    void setData( const DiagramDataPtr & );
    DiagramDataPtr getData() const
        {
            return mpData;
        }
    void setLayout( const DiagramLayoutPtr & );
    DiagramLayoutPtr getLayout() const
        {
            return mpLayout;
        }

    DiagramQStyleMap& getStyles() { return maStyles; }
    const DiagramQStyleMap& getStyles() const { return maStyles; }
    DiagramColorMap& getColors() { return maColors; }
    const DiagramColorMap& getColors() const { return maColors; }

    void addTo( const ShapePtr & pShape );
private:
    void build( );
    DiagramDataPtr                             mpData;
    DiagramLayoutPtr                           mpLayout;
    DiagramQStyleMap                           maStyles;
    DiagramColorMap                            maColors;
    std::map< ::rtl::OUString, ShapePtr >      maShapeMap;
};


typedef boost::shared_ptr< Diagram > DiagramPtr;

} }

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
