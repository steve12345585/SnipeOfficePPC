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

#ifndef _XMLOFF_XMLEXP_HXX
#define _XMLOFF_XMLEXP_HXX

#include "sal/config.h"
#include "xmloff/dllapi.h"
#include "sal/types.h"

#include <com/sun/star/embed/XStorage.hpp>
#include <com/sun/star/xml/sax/SAXParseException.hpp>
#include <com/sun/star/xml/sax/XExtendedDocumentHandler.hpp>
#include <com/sun/star/xml/sax/SAXException.hpp>
#include <com/sun/star/xml/sax/XDocumentHandler.hpp>
#include <com/sun/star/xml/sax/XAttributeList.hpp>
#include <com/sun/star/xml/sax/XLocator.hpp>
#include <com/sun/star/util/XNumberFormatsSupplier.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <rtl/ustring.hxx>
#include <xmloff/txtparae.hxx>
#include <xmloff/formlayerexport.hxx>
#include <xmloff/xmlnumfe.hxx>
#include <xmloff/xmlaustp.hxx>
#include <xmloff/shapeexport.hxx>
#include <xmloff/xmltoken.hxx>
#include <xmloff/SchXMLExportHelper.hxx>
#include <xmloff/XMLFontAutoStylePool.hxx>
#include <com/sun/star/document/XFilter.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/document/XExporter.hpp>
#include <com/sun/star/document/XGraphicObjectResolver.hpp>
#include <com/sun/star/document/XEmbeddedObjectResolver.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XEventListener.hpp>
#include <com/sun/star/container/XNamed.hpp>

#include <unotools/saveopt.hxx>

#include <xmloff/XMLPageExport.hxx>
#include <xmloff/ProgressBarHelper.hxx>
#include <cppuhelper/implbase6.hxx>

#include <list>

class SvXMLNamespaceMap;
class SvXMLAttributeList;
class SvXMLExport_Impl;
class SvXMLUnitConverter;
class ProgressBarHelper;
class XMLEventExport;
class XMLSettingsExportHelper;
class XMLImageMapExport;
class XMLErrors;

// Shapes in Writer cannot be named via context menu (#i51726#)
#include <unotools/moduleoptions.hxx>

namespace rtl { class OUString; }
namespace com { namespace sun { namespace star {
    namespace uno { class XComponentContext; }
    namespace frame { class XModel; }
    namespace container { class XIndexContainer; }
} } }
namespace comphelper { class UnoInterfaceToUniqueIdentifierMapper; }

#define EXPORT_META                     0x0001
#define EXPORT_STYLES                   0x0002
#define EXPORT_MASTERSTYLES             0x0004
#define EXPORT_AUTOSTYLES               0x0008
#define EXPORT_CONTENT                  0x0010
#define EXPORT_SCRIPTS                  0x0020
#define EXPORT_SETTINGS                 0x0040
#define EXPORT_FONTDECLS                0x0080
#define EXPORT_EMBEDDED                 0x0100
#define EXPORT_NODOCTYPE                0x0200
#define EXPORT_PRETTY                   0x0400
#define EXPORT_SAVEBACKWARDCOMPATIBLE   0x0800
#define EXPORT_OASIS                    0x8000
#define EXPORT_ALL                      0x7fff

class XMLOFF_DLLPUBLIC SvXMLExport : public ::cppu::WeakImplHelper6<
             ::com::sun::star::document::XFilter,
             ::com::sun::star::lang::XServiceInfo,
             ::com::sun::star::document::XExporter,
              ::com::sun::star::lang::XInitialization,
             ::com::sun::star::container::XNamed,
             ::com::sun::star::lang::XUnoTunnel>
{
    SvXMLExport_Impl            *mpImpl;            // dummy

    // #110680#
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > mxServiceFactory;

    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel > mxModel;
    ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler >            mxHandler;      // the handlers
    ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XExtendedDocumentHandler >    mxExtHandler;
    ::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormatsSupplier > mxNumberFormatsSupplier;
    ::com::sun::star::uno::Reference< ::com::sun::star::document::XGraphicObjectResolver > mxGraphicResolver;
    ::com::sun::star::uno::Reference< ::com::sun::star::document::XEmbeddedObjectResolver > mxEmbeddedResolver;
    ::com::sun::star::uno::Reference< ::com::sun::star::task::XStatusIndicator > mxStatusIndicator;
    ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > mxExportInfo;
     ::com::sun::star::uno::Reference< ::com::sun::star::lang::XEventListener > mxEventListener;

    SvXMLAttributeList          *mpAttrList;        // a common attribute list
    ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >          mxAttrList;     // and an interface of it

    ::rtl::OUString     msOrigFileName; // the original URL
    ::rtl::OUString     msGraphicObjectProtocol;
    ::rtl::OUString     msEmbeddedObjectProtocol;
    ::rtl::OUString     msFilterName;
    SvXMLNamespaceMap           *mpNamespaceMap;    // the namepspace map
    SvXMLUnitConverter          *mpUnitConv;        // the unit converter
    SvXMLNumFmtExport           *mpNumExport;
    ProgressBarHelper           *mpProgressBarHelper;

    UniReference< XMLTextParagraphExport > mxTextParagraphExport;
    UniReference< XMLShapeExport > mxShapeExport;
    UniReference< SvXMLAutoStylePoolP > mxAutoStylePool;
    UniReference< SchXMLExportHelper > mxChartExport;
    UniReference< XMLPageExport > mxPageExport;
    UniReference< XMLFontAutoStylePool > mxFontAutoStylePool;
    UniReference< xmloff::OFormLayerXMLExport > mxFormExport;
    XMLEventExport* mpEventExport;
    XMLImageMapExport* mpImageMapExport;
    XMLErrors*  mpXMLErrors;

    sal_Bool                        mbExtended;     // Does document contain extens.

    const enum ::xmloff::token::XMLTokenEnum meClass;
    SAL_DLLPRIVATE void _InitCtor();

    sal_uInt16  mnExportFlags;
    sal_uInt16  mnErrorFlags;
    bool  mbEnableExperimentalOdfExport;

public:

    const ::rtl::OUString               msWS;           // " "

private:

    // Shapes in Writer cannot be named via context menu (#i51726#)
    SvtModuleOptions::EFactory meModelType;
    SAL_DLLPRIVATE void _DetermineModelType();

    SAL_DLLPRIVATE void ImplExportMeta(); // <office:meta>
    SAL_DLLPRIVATE void ImplExportSettings(); // <office:settings>
    SAL_DLLPRIVATE void ImplExportStyles( sal_Bool bUsed ); // <office:styles>
    SAL_DLLPRIVATE void ImplExportAutoStyles( sal_Bool bUsed );
        // <office:automatic-styles>
    SAL_DLLPRIVATE void ImplExportMasterStyles( sal_Bool bUsed );
        // <office:master-styles>
    SAL_DLLPRIVATE void ImplExportContent(); // <office:body>
    virtual void SetBodyAttributes();
    void GetViewSettingsAndViews(com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue>& rProps);

protected:
    void setExportFlags( sal_uInt16 nExportFlags ) { mnExportFlags = nExportFlags; }

    // Get (modifyable) namespace map
    SvXMLNamespaceMap& _GetNamespaceMap() { return *mpNamespaceMap; }

    // get a new namespave map (used in starmath to have a default namespace)
    void ResetNamespaceMap();

    // This method can be overloaded to export the content of <office:meta>.
    // There is a default implementation.
    virtual void _ExportMeta();

    // This method can be overloaded to export the content of <office:scripts>.
    // There is a default implementation.
    virtual void _ExportScripts();

    // This method can be overloaded to export the font declarations
    // The default implementation will export the contents of the
    // XMLFontAutoStylePool if it has been created.
    virtual void _ExportFontDecls();

    // This method should be overloaded to export the content of <style:styles>.
    // If bUsed is set, used styles should be exported only.
    // Overloaded Methods must call this method !
    virtual void _ExportStyles( sal_Bool bUsed ) ;

    // This method must be overloaded to export the contents of
    // <style:auto-styles>
    virtual void _ExportAutoStyles() = 0;

    // This method must be overloaded to export the contents of
    // <style:master-styles>
    virtual void _ExportMasterStyles() = 0;

    // This method must be overloaded to export the content of <office:body>.
    virtual void _ExportContent() = 0;

    void SetExtended( sal_Bool bSet=sal_True ) { mbExtended = bSet; }

    // save linked sections? (may be false in global documents)
    sal_Bool mbSaveLinkedSections;

    virtual XMLTextParagraphExport* CreateTextParagraphExport();
    virtual XMLShapeExport* CreateShapeExport();
    virtual SvXMLAutoStylePoolP* CreateAutoStylePool();
    virtual SchXMLExportHelper* CreateChartExport();
    virtual XMLPageExport* CreatePageExport();
    virtual XMLFontAutoStylePool* CreateFontAutoStylePool();
    virtual xmloff::OFormLayerXMLExport* CreateFormExport();
    virtual void GetViewSettings(com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue>& aProps);
    virtual void GetConfigurationSettings(com::sun::star::uno::Sequence<com::sun::star::beans::PropertyValue>& aProps);

    struct SettingsGroup
    {
        ::xmloff::token::XMLTokenEnum                                               eGroupName;
        ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >   aSettings;

        SettingsGroup()
            :eGroupName( ::xmloff::token::XML_TOKEN_INVALID )
            ,aSettings()
        {
        }

        SettingsGroup(
                const ::xmloff::token::XMLTokenEnum _eGroupName,
                const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& _rSettings )
            :eGroupName( _eGroupName )
            ,aSettings( _rSettings )
        {
        }
    };
    /** returns the current document settings

        The default implementation will obtain the view settings by calling GetViewSettingsAndViews, and the
        configuration settings by calling GetConfigurationSettings, and return them together with the proper XML token.

        @return
            the accumulated count of all settings in all groups
    */
    virtual sal_Int32 GetDocumentSpecificSettings( ::std::list< SettingsGroup >& _out_rSettings );

    const ::com::sun::star::uno::Reference< ::com::sun::star::document::XEmbeddedObjectResolver >& GetEmbeddedResolver() const { return mxEmbeddedResolver; }
    inline void SetEmbeddedResolver( com::sun::star::uno::Reference< com::sun::star::document::XEmbeddedObjectResolver >& _xEmbeddedResolver );

    const ::com::sun::star::uno::Reference< ::com::sun::star::document::XGraphicObjectResolver >& GetGraphicResolver() const { return mxGraphicResolver; }
    void SetGraphicResolver( com::sun::star::uno::Reference< com::sun::star::document::XGraphicObjectResolver >& _xGraphicResolver );

    void SetDocHandler( const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler > &rHandler );

public:

    // #110680#
    //SvXMLExport( MapUnit eDfltUnit,
    //             const enum ::xmloff::token::XMLTokenEnum eClass = xmloff::token::XML_TOKEN_INVALID,
    //             sal_uInt16 nExportFlag = EXPORT_ALL );
    SvXMLExport(
        sal_Int16 const eDefaultMeasureUnit /*css::util::MeasureUnit*/,
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory,
        const enum ::xmloff::token::XMLTokenEnum eClass = xmloff::token::XML_TOKEN_INVALID,
        sal_uInt16 nExportFlag = EXPORT_ALL );

    // #110680#
    //SvXMLExport( const ::rtl::OUString& rFileName,
    //           const ::com::sun::star::uno::Reference<
    //              ::com::sun::star::xml::sax::XDocumentHandler > & rHandler,
    //           MapUnit eDfltUnit = MAP_INCH );
    SvXMLExport(
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory,
        const ::rtl::OUString& rFileName,
        sal_Int16 const eDefaultMeasureUnit /*css::util::MeasureUnit*/,
		const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler > & rHandler);

    // #110680#
    //SvXMLExport( const ::rtl::OUString& rFileName,
    //           const ::com::sun::star::uno::Reference<
    //              ::com::sun::star::xml::sax::XDocumentHandler > & rHandler,
    //           const ::com::sun::star::uno::Reference<
    //              ::com::sun::star::frame::XModel > &,
    //           sal_Int16 eDfltUnit );
    SvXMLExport(
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory,
        const ::rtl::OUString& rFileName,
        const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler > & rHandler,
        const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel > &,
        sal_Int16 const eDefaultFieldUnit );

    virtual ~SvXMLExport();

    static const ::com::sun::star::uno::Sequence< sal_Int8 > & getUnoTunnelId() throw();
    static SvXMLExport* getImplementation( ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > ) throw();

    // XExporter
    virtual void SAL_CALL setSourceDocument( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent >& xDoc ) throw(::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);

    // XFilter
    virtual sal_Bool SAL_CALL filter( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& aDescriptor ) throw(::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL cancel() throw(::com::sun::star::uno::RuntimeException);

    // XInitialization
    virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments ) throw(::com::sun::star::uno::Exception, ::com::sun::star::uno::RuntimeException);

    // XNamed
    virtual ::rtl::OUString SAL_CALL getName(  ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL setName( const ::rtl::OUString& aName ) throw (::com::sun::star::uno::RuntimeException);

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  ) throw(::com::sun::star::uno::RuntimeException);
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName ) throw(::com::sun::star::uno::RuntimeException);
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  ) throw(::com::sun::star::uno::RuntimeException);

    // XUnoTunnel
    virtual sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence< sal_Int8 >& aIdentifier ) throw(::com::sun::star::uno::RuntimeException);

    /** ensures that the given namespace is in scope at the next started
        element.

        <p>If the namespace is not yet declared, the necessary attribute will
        be added, as well.</p>

        @param i_rNamespace         the namespace to be declared
        @param i_rPreferredPrefix   (opt.) preferred prefix for the namespace

        @returns the actual prefix that the namespace is associated with
      */
    ::rtl::OUString
    EnsureNamespace(::rtl::OUString const & i_rNamespace,
        ::rtl::OUString const & i_rPreferredPrefix
        = ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("gen")) );

    // Check if common attribute list is empty.
#ifndef DBG_UTIL
    void CheckAttrList() {}
#else
    void CheckAttrList();
#endif

    // Clear common attribute list.
    void ClearAttrList();

    // Add an attribute to the common attribute list.
    void AddAttributeASCII( sal_uInt16 nPrefix, const sal_Char *pName,
                            const sal_Char *pValue );
    void AddAttribute( sal_uInt16 nPrefix, const sal_Char *pName,
                       const ::rtl::OUString& rValue );
    void AddAttribute( sal_uInt16 nPrefix, const ::rtl::OUString& rName,
                       const ::rtl::OUString& rValue );
    void AddAttribute( sal_uInt16 nPrefix,
                       enum ::xmloff::token::XMLTokenEnum eName,
                       const ::rtl::OUString& rValue );
    void AddAttribute( sal_uInt16 nPrefix,
                       enum ::xmloff::token::XMLTokenEnum eName,
                       enum ::xmloff::token::XMLTokenEnum eValue );
    void AddAttribute( const ::rtl::OUString& rQName,
                       const ::rtl::OUString& rValue );
    void AddAttribute( const ::rtl::OUString& rQName,
                       enum ::xmloff::token::XMLTokenEnum eValue );
    // add several attributes to the common attribute list
    void AddAttributeList( const ::com::sun::star::uno::Reference<
                                  ::com::sun::star::xml::sax::XAttributeList >& xAttrList );

    // Get common attribute list as implementation or interface.
    SvXMLAttributeList &GetAttrList() { return *mpAttrList; }
    const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > & GetXAttrList() { return mxAttrList; }

    // Get document handler. This methods are not const, because the
    // reference allowes modifications through the handler.
    const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XDocumentHandler > & GetDocHandler() { return mxHandler; }
    const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XExtendedDocumentHandler > & GetExtDocHandler()
    {
        return mxExtHandler;
    }

    // Get original ::com::sun::star::util::URL.
    const ::rtl::OUString& GetOrigFileName() const { return msOrigFileName; }

    // Get (const) namespace map.
    const SvXMLNamespaceMap& GetNamespaceMap() const { return *mpNamespaceMap; }

    // Get unit converter
    const SvXMLUnitConverter& GetMM100UnitConverter() const { return *mpUnitConv; }

    SvXMLUnitConverter& GetMM100UnitConverter() { return *mpUnitConv; }

    void addChaffWhenEncryptedStorage();

    // Export the document.
    virtual sal_uInt32 exportDoc( enum ::xmloff::token::XMLTokenEnum eClass = ::xmloff::token::XML_TOKEN_INVALID );

    virtual void addDataStyle(const sal_Int32 nNumberFormat, sal_Bool bTimeFormat = sal_False );
    virtual void exportDataStyles();
    virtual void exportAutoDataStyles();
    virtual rtl::OUString getDataStyleName(const sal_Int32 nNumberFormat, sal_Bool bTimeFormat = sal_False ) const;
    sal_Int32 dataStyleForceSystemLanguage(sal_Int32 nFormat) const;

    virtual void exportAnnotationMeta( const com::sun::star::uno::Reference < com::sun::star::drawing::XShape >& xShape);

    // Get XModel
    const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel > &
               GetModel() const { return mxModel; }
    // Get XNumberFormatsSupplier
    ::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormatsSupplier > & GetNumberFormatsSupplier() { return mxNumberFormatsSupplier; }
    inline void SetNumberFormatsSupplier(const ::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormatsSupplier >& _xNumberFormatSupplier)
    {
        mxNumberFormatsSupplier = _xNumberFormatSupplier;
        if ( mxNumberFormatsSupplier.is() && mxHandler.is() )
            mpNumExport = new SvXMLNumFmtExport(*this, mxNumberFormatsSupplier);
    }

    // get export helper for text
    inline UniReference< XMLTextParagraphExport > GetTextParagraphExport();

    // get export helper for shapes
    inline UniReference< XMLShapeExport > GetShapeExport();

    // get auto style pool
    inline UniReference< SvXMLAutoStylePoolP > GetAutoStylePool();

    // get Page Export
    inline UniReference< XMLPageExport > GetPageExport();

    // get chart export helper
    inline UniReference< SchXMLExportHelper > GetChartExport();

    // get font auto style pool
    inline UniReference< XMLFontAutoStylePool > GetFontAutoStylePool();

    ProgressBarHelper*  GetProgressBarHelper();

    // get Formlayer Export
    inline UniReference< xmloff::OFormLayerXMLExport > GetFormExport();
    inline bool HasFormExport();

    // get XPropertySet with export information
    inline ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > getExportInfo() const { return mxExportInfo; }

    com::sun::star::uno::Reference< com::sun::star::task::XStatusIndicator > GetStatusIndicator() { return mxStatusIndicator; }

    /// get Event export, with handlers for script types "None" and
    /// "StarBasic" already registered; other handlers may be registered, too.
    XMLEventExport& GetEventExport();

    /// get the export for image maps
    XMLImageMapExport& GetImageMapExport();

    ::rtl::OUString AddEmbeddedGraphicObject(
                            const ::rtl::OUString& rGraphicObjectURL );
    sal_Bool AddEmbeddedGraphicObjectAsBase64(
                            const ::rtl::OUString& rGraphicObjectURL );

    ::rtl::OUString AddEmbeddedObject(
                            const ::rtl::OUString& rEmbeddedObjectURL );
    sal_Bool AddEmbeddedObjectAsBase64(
                            const ::rtl::OUString& rEmbeddedObjectURL );

    ::rtl::OUString EncodeStyleName( const ::rtl::OUString& rName,
                                     sal_Bool *pEncoded=0 ) const;

    // save linked sections?
    inline sal_Bool IsSaveLinkedSections() { return mbSaveLinkedSections; }

    // get export flags
    sal_uInt16 getExportFlags() const { return mnExportFlags; }

    sal_Bool ExportEmbeddedOwnObject(
        ::com::sun::star::uno::Reference<
            ::com::sun::star::lang::XComponent >& rComp );

    rtl::OUString GetRelativeReference(const rtl::OUString& rValue);

    // methods for accessing the document handler and handling SAX errors
    void StartElement(sal_uInt16 nPrefix,
                        enum ::xmloff::token::XMLTokenEnum eName,
                        sal_Bool bIgnWSOutside );
    void StartElement(const ::rtl::OUString& rName,
                        sal_Bool bIgnWSOutside );
    void Characters(const ::rtl::OUString& rChars);
    void EndElement(sal_uInt16 nPrefix,
                        enum ::xmloff::token::XMLTokenEnum eName,
                        sal_Bool bIgnWSInside );
    void EndElement(const ::rtl::OUString& rName,
                        sal_Bool bIgnWSInside );
    void IgnorableWhitespace();

    /**
     * Record an error condition that occurred during export. The
     * behavior of SetError can be modified using the error flag
     * constants.
     */
    void SetError(
        /// error ID, may contain an error flag
        sal_Int32 nId,
        /// string parameters for the error message
        const ::com::sun::star::uno::Sequence< ::rtl::OUString> & rMsgParams,
        /// original exception message (if applicable)
        const ::rtl::OUString& rExceptionMessage,
        /// error location (if applicable)
        const ::com::sun::star::uno::Reference<
            ::com::sun::star::xml::sax::XLocator> & rLocator );

    void SetError(
        sal_Int32 nId,
        const ::com::sun::star::uno::Sequence< ::rtl::OUString> & rMsgParams);

    /** return current error flags (logical 'or' of all error flags so far) */
    sal_uInt16 GetErrorFlags()  { return mnErrorFlags; }

    virtual void DisposingModel();

    ::comphelper::UnoInterfaceToUniqueIdentifierMapper& getInterfaceToIdentifierMapper();

    // #110680#
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > getServiceFactory();

    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >
    GetComponentContext() const;

    // Shapes in Writer cannot be named via context menu (#i51726#)
    SvtModuleOptions::EFactory GetModelType() const
    {
        return meModelType;
    }

    // Written OpenDocument file format doesn't fit to the created text document (#i69627#)
    sal_Bool writeOutlineStyleAsNormalListStyle() const;
       bool isExperimentalOdfExportEnabled() const { return mbEnableExperimentalOdfExport; }

    ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage > GetTargetStorage();

    /// returns the currently configured default version for odf export
    SvtSaveOptions::ODFDefaultVersion getDefaultVersion() const;

    /// name of stream in package, e.g., "content.xml"
    ::rtl::OUString GetStreamName() const;

    // FIXME: this is only for legacy stuff that has not yet been adapted
    //        to implement XMetadatable; this can write duplicate IDs!
    /// add xml:id and legacy namespace id
    void SAL_DLLPRIVATE AddAttributeIdLegacy(
            sal_uInt16 const nLegacyPrefix, ::rtl::OUString const& rValue);

    /// add xml:id attribute (for RDF metadata)
    void AddAttributeXmlId(::com::sun::star::uno::Reference<
            ::com::sun::star::uno::XInterface> const & i_xIfc);

    /// add RDFa attributes for a metadatable text content
    void AddAttributesRDFa( ::com::sun::star::uno::Reference<
        ::com::sun::star::text::XTextContent> const & i_xTextContent);

    sal_Bool exportTextNumberElement() const;

    /// set null date from model to unit converter, if not already done
    sal_Bool SetNullDateOnUnitConverter();
};

inline UniReference< XMLTextParagraphExport > SvXMLExport::GetTextParagraphExport()
{
    if( !mxTextParagraphExport.is() )
        mxTextParagraphExport = CreateTextParagraphExport();

    return mxTextParagraphExport;
}

inline UniReference< XMLShapeExport > SvXMLExport::GetShapeExport()
{
    if( !mxShapeExport.is() )
        mxShapeExport = CreateShapeExport();

    return mxShapeExport;
}

inline UniReference< SvXMLAutoStylePoolP > SvXMLExport::GetAutoStylePool()
{
    if( !mxAutoStylePool.is() )
        mxAutoStylePool = CreateAutoStylePool();

    return mxAutoStylePool;
}

inline UniReference< SchXMLExportHelper > SvXMLExport::GetChartExport()
{
    if( !mxChartExport.is() )
        mxChartExport = CreateChartExport();

    return mxChartExport;
}

inline UniReference< XMLPageExport > SvXMLExport::GetPageExport()
{
    if( !mxPageExport.is() )
        mxPageExport = CreatePageExport();

    return mxPageExport;
}

inline UniReference< XMLFontAutoStylePool > SvXMLExport::GetFontAutoStylePool()
{
    if( !mxFontAutoStylePool.is() )
        mxFontAutoStylePool = CreateFontAutoStylePool();

    return mxFontAutoStylePool;
}

inline UniReference< xmloff::OFormLayerXMLExport > SvXMLExport::GetFormExport()
{
    if( !mxFormExport.is() )
        mxFormExport = CreateFormExport();

    return mxFormExport;
}

inline bool SvXMLExport::HasFormExport()
{
    return mxFormExport.is();
}

inline void SvXMLExport::SetEmbeddedResolver(
    com::sun::star::uno::Reference< com::sun::star::document::XEmbeddedObjectResolver >& _xEmbeddedResolver )
{
    mxEmbeddedResolver = _xEmbeddedResolver;
}

inline void SvXMLExport::SetGraphicResolver(
    com::sun::star::uno::Reference< com::sun::star::document::XGraphicObjectResolver >& _xGraphicResolver )
{
    mxGraphicResolver = _xGraphicResolver;
}

// Helper class to export an element.
class XMLOFF_DLLPUBLIC SvXMLElementExport
{
    SvXMLExport& rExport;
    ::rtl::OUString aName;
    sal_Bool bIgnWS : 1;
    sal_Bool bDoSomething : 1;

    SAL_DLLPRIVATE void StartElement( SvXMLExport& rExp, sal_uInt16 nPrefix,
                       const ::rtl::OUString& rName,
                       sal_Bool bIgnWSOutside );

public:

    // The constructor prints a start tag that has the common attributes
    // of the XMLExport instance attached.
    SvXMLElementExport( SvXMLExport& rExp, sal_uInt16 nPrefix,
                        const sal_Char *pName,
                        sal_Bool bIgnWSOutside, sal_Bool bIgnWSInside );
    SvXMLElementExport( SvXMLExport& rExp, sal_uInt16 nPrefix,
                        const ::rtl::OUString& rName,
                        sal_Bool bIgnWSOutside, sal_Bool bIgnWSInside );
    SvXMLElementExport( SvXMLExport& rExp, sal_uInt16 nPrefix,
                        enum ::xmloff::token::XMLTokenEnum eName,
                        sal_Bool bIgnWSOutside, sal_Bool bIgnWSInside );
    SvXMLElementExport( SvXMLExport& rExp, const ::rtl::OUString& rQName,
                        sal_Bool bIgnWSOutside, sal_Bool bIgnWSInside );

    // Thes constructors do nothing if bDoSomething is not set
    SvXMLElementExport( SvXMLExport& rExp, sal_Bool bDoSomething,
                        sal_uInt16 nPrefix,
                        enum ::xmloff::token::XMLTokenEnum eName,
                        sal_Bool bIgnWSOutside, sal_Bool bIgnWSInside );

    // The destructor prints an end tag.
    ~SvXMLElementExport();
};

#endif  //  _XMLOFF_SVXMLEXP_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
