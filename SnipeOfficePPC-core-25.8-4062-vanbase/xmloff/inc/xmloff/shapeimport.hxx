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

#ifndef _XMLOFF_SHAPEIMPORT_HXX_
#define _XMLOFF_SHAPEIMPORT_HXX_

#include "sal/config.h"
#include "xmloff/dllapi.h"
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/xml/sax/XAttributeList.hpp>
#include <com/sun/star/drawing/XShapes.hpp>
#include <com/sun/star/drawing/HomogenMatrix.hpp>
#include <com/sun/star/drawing/ProjectionMode.hpp>
#include <com/sun/star/drawing/ShadeMode.hpp>
#include <xmloff/uniref.hxx>
#include <xmloff/xmlictxt.hxx>

#include "xmloff/table/XMLTableImport.hxx"
#include <basegfx/vector/b3dvector.hxx>
#include <vector>

class SvXMLImport;
class SvXMLImportContext;
class SvXMLTokenMap;
class SvXMLStylesContext;
class XMLSdPropHdlFactory;
class XMLPropertySetMapper;
class SvXMLImportPropertyMapper;

//////////////////////////////////////////////////////////////////////////////

enum SdXMLGroupShapeElemTokenMap
{
    XML_TOK_GROUP_GROUP,
    XML_TOK_GROUP_RECT,
    XML_TOK_GROUP_LINE,
    XML_TOK_GROUP_CIRCLE,
    XML_TOK_GROUP_ELLIPSE,
    XML_TOK_GROUP_POLYGON,
    XML_TOK_GROUP_POLYLINE,
    XML_TOK_GROUP_PATH,

    XML_TOK_GROUP_CONTROL,
    XML_TOK_GROUP_CONNECTOR,
    XML_TOK_GROUP_MEASURE,
    XML_TOK_GROUP_PAGE,
    XML_TOK_GROUP_CAPTION,

    XML_TOK_GROUP_CHART,
    XML_TOK_GROUP_3DSCENE,

    XML_TOK_GROUP_FRAME,

    XML_TOK_GROUP_CUSTOM_SHAPE,

    XML_TOK_GROUP_ANNOTATION,

    XML_TOK_GROUP_A,

    XML_TOK_GROUP_LAST
};

enum SdXMLFrameShapeElemTokenMap
{
    XML_TOK_FRAME_TEXT_BOX,
    XML_TOK_FRAME_IMAGE,
    XML_TOK_FRAME_OBJECT,
    XML_TOK_FRAME_OBJECT_OLE,
    XML_TOK_FRAME_PLUGIN,
    XML_TOK_FRAME_FRAME,
    XML_TOK_FRAME_FLOATING_FRAME,
    XML_TOK_FRAME_APPLET,
    XML_TOK_FRAME_TABLE,

    XML_TOK_FRAME_LAST
};

enum SdXML3DSceneShapeElemTokenMap
{
    XML_TOK_3DSCENE_3DSCENE,
    XML_TOK_3DSCENE_3DCUBE,
    XML_TOK_3DSCENE_3DSPHERE,
    XML_TOK_3DSCENE_3DLATHE,
    XML_TOK_3DSCENE_3DEXTRUDE,

    XML_TOK_3DSCENE_LAST
};

enum SdXMLShapeAttrTokenMap
{
    XML_TOK_SHAPE_NAME,
    XML_TOK_SHAPE_DRAWSTYLE_NAME_GRAPHICS,
    XML_TOK_SHAPE_PRESENTATION_CLASS,
    XML_TOK_SHAPE_DRAWSTYLE_NAME_PRESENTATION,
    XML_TOK_SHAPE_TRANSFORM,
    XML_TOK_SHAPE_IS_PLACEHOLDER,
    XML_TOK_SHAPE_IS_USER_TRANSFORMED
};

enum SdXML3DObjectAttrTokenMap
{
    XML_TOK_3DOBJECT_DRAWSTYLE_NAME,
    XML_TOK_3DOBJECT_TRANSFORM
};

enum SdXML3DPolygonBasedAttrTokenMap
{
    XML_TOK_3DPOLYGONBASED_VIEWBOX,
    XML_TOK_3DPOLYGONBASED_D
};

enum SdXML3DCubeObjectAttrTokenMap
{
    XML_TOK_3DCUBEOBJ_MINEDGE,
    XML_TOK_3DCUBEOBJ_MAXEDGE
};

enum SdXML3DSphereObjectAttrTokenMap
{
    XML_TOK_3DSPHEREOBJ_CENTER,
    XML_TOK_3DSPHEREOBJ_SIZE
};

enum SdXMLPolygonShapeAttrTokenMap
{
    XML_TOK_POLYGONSHAPE_VIEWBOX,
    XML_TOK_POLYGONSHAPE_POINTS
};

enum SdXMLPathShapeAttrTokenMap
{
    XML_TOK_PATHSHAPE_VIEWBOX,
    XML_TOK_PATHSHAPE_D
};

enum SdXML3DSceneShapeAttrTokenMap
{
    XML_TOK_3DSCENESHAPE_TRANSFORM,
    XML_TOK_3DSCENESHAPE_VRP,
    XML_TOK_3DSCENESHAPE_VPN,
    XML_TOK_3DSCENESHAPE_VUP,
    XML_TOK_3DSCENESHAPE_PROJECTION,
    XML_TOK_3DSCENESHAPE_DISTANCE,
    XML_TOK_3DSCENESHAPE_FOCAL_LENGTH,
    XML_TOK_3DSCENESHAPE_SHADOW_SLANT,
    XML_TOK_3DSCENESHAPE_SHADE_MODE,
    XML_TOK_3DSCENESHAPE_AMBIENT_COLOR,
    XML_TOK_3DSCENESHAPE_LIGHTING_MODE
};

enum SdXML3DLightAttrTokenMap
{
    XML_TOK_3DLIGHT_DIFFUSE_COLOR,
    XML_TOK_3DLIGHT_DIRECTION,
    XML_TOK_3DLIGHT_ENABLED,
    XML_TOK_3DLIGHT_SPECULAR
};

//////////////////////////////////////////////////////////////////////////////
// dr3d:3dlight context

class SdXML3DLightContext: public SvXMLImportContext
{
    // local parameters which need to be read
    sal_Int32                   maDiffuseColor;
    ::basegfx::B3DVector        maDirection;
    bool                        mbEnabled;
    bool                        mbSpecular;

public:
    SdXML3DLightContext(
        SvXMLImport& rImport,
        sal_uInt16 nPrfx,
        const rtl::OUString& rLName,
        const com::sun::star::uno::Reference< com::sun::star::xml::sax::XAttributeList >& xAttrList);
    virtual ~SdXML3DLightContext();

    sal_Int32 GetDiffuseColor() { return maDiffuseColor; }
    const ::basegfx::B3DVector& GetDirection() { return maDirection; }
    sal_Bool GetEnabled() { return mbEnabled; }
    sal_Bool GetSpecular() { return mbSpecular; }
};

//////////////////////////////////////////////////////////////////////////////

typedef ::std::vector< SdXML3DLightContext* > Imp3DLightList;

class SdXML3DSceneAttributesHelper
{
protected:
    SvXMLImport& mrImport;

    // list for local light contexts
    Imp3DLightList              maList;

    // local parameters which need to be read
    com::sun::star::drawing::HomogenMatrix mxHomMat;
    sal_Bool                        mbSetTransform;

    com::sun::star::drawing::ProjectionMode mxPrjMode;
    sal_Int32                   mnDistance;
    sal_Int32                   mnFocalLength;
    sal_Int32                   mnShadowSlant;
    com::sun::star::drawing::ShadeMode mxShadeMode;
    sal_Int32                   maAmbientColor;
    bool                        mbLightingMode;

    ::basegfx::B3DVector        maVRP;
    ::basegfx::B3DVector        maVPN;
    ::basegfx::B3DVector        maVUP;
    sal_Bool                        mbVRPUsed;
    sal_Bool                        mbVPNUsed;
    sal_Bool                        mbVUPUsed;

public:
    SdXML3DSceneAttributesHelper( SvXMLImport& rImporter );
    ~SdXML3DSceneAttributesHelper();

    /** creates a 3d ligth context and adds it to the internal list for later processing */
    SvXMLImportContext * create3DLightContext( sal_uInt16 nPrfx, const rtl::OUString& rLName, const com::sun::star::uno::Reference< com::sun::star::xml::sax::XAttributeList >& xAttrList);

    /** this should be called for each scene attribute */
    void processSceneAttribute( sal_uInt16 nPrefix, const ::rtl::OUString& rLocalName, const ::rtl::OUString& rValue );

    /** this sets the scene attributes at this propertyset */
    void setSceneAttributes( const com::sun::star::uno::Reference< com::sun::star::beans::XPropertySet >& xPropSet );
};

//////////////////////////////////////////////////////////////////////////////

class SvXMLShapeContext : public SvXMLImportContext
{
protected:
    com::sun::star::uno::Reference< com::sun::star::drawing::XShape >   mxShape;
    sal_Bool                                                            mbTemporaryShape;
    rtl::OUString                                                       msHyperlink;

public:
    SvXMLShapeContext( SvXMLImport& rImp, sal_uInt16 nPrfx,
        const ::rtl::OUString& rLName, sal_Bool bTemporaryShape ) : SvXMLImportContext( rImp, nPrfx, rLName ), mbTemporaryShape(bTemporaryShape) {}

    TYPEINFO();

    const com::sun::star::uno::Reference< com::sun::star::drawing::XShape >& getShape() const { return mxShape; }

    void setHyperlink( const ::rtl::OUString& rHyperlink );
};

//////////////////////////////////////////////////////////////////////////////

class ShapeSortContext;
struct XMLShapeImportHelperImpl;
struct XMLShapeImportPageContextImpl;

class XMLOFF_DLLPUBLIC XMLShapeImportHelper : public UniRefBase
{
    XMLShapeImportHelperImpl*   mpImpl;

    XMLShapeImportPageContextImpl*  mpPageContext;

    com::sun::star::uno::Reference< com::sun::star::frame::XModel > mxModel;

    // PropertySetMappers and factory
    XMLSdPropHdlFactory*        mpSdPropHdlFactory;
    SvXMLImportPropertyMapper*      mpPropertySetMapper;
    SvXMLImportPropertyMapper*      mpPresPagePropsMapper;

    // contexts for Style and AutoStyle import
    SvXMLStylesContext*         mpStylesContext;
    SvXMLStylesContext*         mpAutoStylesContext;

    // contexts for xShape contents TokenMaps
    SvXMLTokenMap*              mpGroupShapeElemTokenMap;
    SvXMLTokenMap*              mpFrameShapeElemTokenMap;
    SvXMLTokenMap*              mp3DSceneShapeElemTokenMap;
    SvXMLTokenMap*              mp3DObjectAttrTokenMap;
    SvXMLTokenMap*              mp3DPolygonBasedAttrTokenMap;
    SvXMLTokenMap*              mp3DCubeObjectAttrTokenMap;
    SvXMLTokenMap*              mp3DSphereObjectAttrTokenMap;
    SvXMLTokenMap*              mp3DSceneShapeAttrTokenMap;
    SvXMLTokenMap*              mp3DLightAttrTokenMap;
    SvXMLTokenMap*              mpPathShapeAttrTokenMap;
    SvXMLTokenMap*              mpPolygonShapeAttrTokenMap;

    const ::rtl::OUString       msStartShape;
    const ::rtl::OUString       msEndShape;
    const ::rtl::OUString       msStartGluePointIndex;
    const ::rtl::OUString       msEndGluePointIndex;

    rtl::Reference< XMLTableImport > mxShapeTableImport;

protected:
    SvXMLImport& mrImporter;

public:
    XMLShapeImportHelper( SvXMLImport& rImporter,
        const com::sun::star::uno::Reference< com::sun::star::frame::XModel>& rModel,
    SvXMLImportPropertyMapper *pExtMapper=0 );

    ~XMLShapeImportHelper();

    SvXMLShapeContext* CreateGroupChildContext(
        SvXMLImport& rImport, sal_uInt16 nPrefix, const rtl::OUString& rLocalName,
        const com::sun::star::uno::Reference< com::sun::star::xml::sax::XAttributeList >& xAttrList,
        com::sun::star::uno::Reference< com::sun::star::drawing::XShapes >& rShapes,
        sal_Bool bTemporaryShape = sal_False);

    SvXMLShapeContext* CreateFrameChildContext(
        SvXMLImport& rImport, sal_uInt16 nPrefix, const rtl::OUString& rLocalName,
        const com::sun::star::uno::Reference< com::sun::star::xml::sax::XAttributeList >& xAttrList,
        com::sun::star::uno::Reference< com::sun::star::drawing::XShapes >& rShapes,
        const com::sun::star::uno::Reference< com::sun::star::xml::sax::XAttributeList >& xFrameAttrList);
    SvXMLImportContext* CreateFrameChildContext(
        SvXMLImportContext *pThisContext, sal_uInt16 nPrefix, const rtl::OUString& rLocalName,
        const com::sun::star::uno::Reference< com::sun::star::xml::sax::XAttributeList >& xAttrList );

    SvXMLShapeContext* Create3DSceneChildContext(
        SvXMLImport& rImport, sal_uInt16 nPrefix, const rtl::OUString& rLocalName,
        const com::sun::star::uno::Reference< com::sun::star::xml::sax::XAttributeList >& xAttrList,
        com::sun::star::uno::Reference< com::sun::star::drawing::XShapes >& rShapes);

    const SvXMLTokenMap& GetGroupShapeElemTokenMap();
    const SvXMLTokenMap& GetFrameShapeElemTokenMap();
    const SvXMLTokenMap& Get3DSceneShapeElemTokenMap();
    const SvXMLTokenMap& Get3DObjectAttrTokenMap();
    const SvXMLTokenMap& Get3DPolygonBasedAttrTokenMap();
    const SvXMLTokenMap& Get3DCubeObjectAttrTokenMap();
    const SvXMLTokenMap& Get3DSphereObjectAttrTokenMap();
    const SvXMLTokenMap& Get3DLightAttrTokenMap();

    // Styles and AutoStyles contexts
    SvXMLStylesContext* GetStylesContext() const { return mpStylesContext; }
    void SetStylesContext(SvXMLStylesContext* pNew);
    SvXMLStylesContext* GetAutoStylesContext() const { return mpAutoStylesContext; }
    void SetAutoStylesContext(SvXMLStylesContext* pNew);

    // get factories and mappers
    SvXMLImportPropertyMapper* GetPropertySetMapper() const { return mpPropertySetMapper; }
    SvXMLImportPropertyMapper* GetPresPagePropsMapper() const { return mpPresPagePropsMapper; }

    // this function is called whenever the implementation classes like to add this new
    // shape to the given XShapes.
    virtual void addShape(
        com::sun::star::uno::Reference< com::sun::star::drawing::XShape >& rShape,
        const com::sun::star::uno::Reference< com::sun::star::xml::sax::XAttributeList >& xAttrList,
        com::sun::star::uno::Reference< com::sun::star::drawing::XShapes >& rShapes);

    // this function is called whenever the implementation classes have finished importing
    // a shape to the given XShapes. The shape is already inserted into its XShapes and
    // all properties and styles are set.
    virtual void finishShape(
        com::sun::star::uno::Reference< com::sun::star::drawing::XShape >& rShape,
        const com::sun::star::uno::Reference< com::sun::star::xml::sax::XAttributeList >& xAttrList,
        com::sun::star::uno::Reference< com::sun::star::drawing::XShapes >& rShapes);

    // helper functions for z-order sorting
    void pushGroupForSorting( com::sun::star::uno::Reference< com::sun::star::drawing::XShapes >& rShapes );
    void popGroupAndSort();

    void shapeWithZIndexAdded( com::sun::star::uno::Reference< com::sun::star::drawing::XShape >& rShape,
                               sal_Int32 nZIndex );

    void addShapeConnection( com::sun::star::uno::Reference< com::sun::star::drawing::XShape >& rConnectorShape,
                             sal_Bool bStart,
                             const rtl::OUString& rDestShapeId,
                             sal_Int32 nDestGlueId );

    /** adds a mapping for a glue point identifier from an xml file to the identifier created after inserting
        the new glue point into the core. The saved mappings can be retrieved by getGluePointId() */
    void addGluePointMapping( com::sun::star::uno::Reference< com::sun::star::drawing::XShape >& xShape,
                              sal_Int32 nSourceId, sal_Int32 nDestinnationId );

    /** moves all current DestinationId's for rXShape by n */
    void moveGluePointMapping( const com::sun::star::uno::Reference< com::sun::star::drawing::XShape >& xShape, const sal_Int32 n );

    /** retrieves a mapping for a glue point identifier from the current xml file to the identifier created after
        inserting the new glue point into the core. The mapping must be initialized first with addGluePointMapping() */
    sal_Int32 getGluePointId( com::sun::star::uno::Reference< com::sun::star::drawing::XShape >& xShape, sal_Int32 nSourceId );

    /** this method must be calling before the first shape is imported for the given page.
        Calls to this method can be nested */
    void startPage( com::sun::star::uno::Reference< com::sun::star::drawing::XShapes >& rShapes );

    /** this method must be calling after the last shape is imported for the given page
        Calls to this method can be nested */
    void endPage( com::sun::star::uno::Reference< com::sun::star::drawing::XShapes >& rShapes );

    void restoreConnections();

    /** creates a property mapper for external chaining */
    static SvXMLImportPropertyMapper* CreateShapePropMapper(
        const com::sun::star::uno::Reference< com::sun::star::frame::XModel>& rModel, SvXMLImport& rImport );

    // #88546#
    /** defines if the import should increment the progress bar or not */
    void enableHandleProgressBar( sal_Bool bEnable = sal_True );
    sal_Bool IsHandleProgressBarEnabled() const;

    /** queries the capability of the current model to create presentation shapes */
    sal_Bool IsPresentationShapesSupported();

    XMLSdPropHdlFactory* GetSdPropHdlFactory() const { return mpSdPropHdlFactory; }

    const rtl::Reference< XMLTableImport >&     GetShapeTableImport();
};

#endif // _XMLOFF_SHAPEIMPORT_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
