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


#include "officeforms.hxx"

#include <sax/tools/converter.hxx>

#include <xmloff/xmltoken.hxx>
#include "xmloff/xmlnmspe.hxx"
#include <xmloff/xmlexp.hxx>
#include <xmloff/xmlimp.hxx>
#include <xmloff/nmspmap.hxx>
#include <comphelper/extract.hxx>
#include "strings.hxx"
#include <rtl/logfile.hxx>

//.........................................................................
namespace xmloff
{
//.........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::beans;
    using namespace ::com::sun::star::frame;
    using namespace ::com::sun::star::xml;
    using ::xmloff::token::XML_FORMS;
    using ::com::sun::star::xml::sax::XAttributeList;

    //=========================================================================
    //= OFormsRootImport
    //=========================================================================
    TYPEINIT1(OFormsRootImport, SvXMLImportContext);
    //-------------------------------------------------------------------------
    OFormsRootImport::OFormsRootImport( SvXMLImport& rImport, sal_uInt16 nPrfx, const rtl::OUString& rLocalName )
        :SvXMLImportContext(rImport, nPrfx, rLocalName)
    {
    }

    //-------------------------------------------------------------------------
    OFormsRootImport::~OFormsRootImport()
    {
    }

    //-------------------------------------------------------------------------
    SvXMLImportContext* OFormsRootImport::CreateChildContext( sal_uInt16 _nPrefix, const ::rtl::OUString& _rLocalName,
            const Reference< XAttributeList>& xAttrList )
    {
        return GetImport().GetFormImport()->createContext( _nPrefix, _rLocalName, xAttrList );
    }

    //-------------------------------------------------------------------------
    void OFormsRootImport::implImportBool(const Reference< XAttributeList >& _rxAttributes, OfficeFormsAttributes _eAttribute,
            const Reference< XPropertySet >& _rxProps, const Reference< XPropertySetInfo >& _rxPropInfo,
            const ::rtl::OUString& _rPropName, sal_Bool _bDefault)
    {
        // the complete attribute name to look for
        ::rtl::OUString sCompleteAttributeName = GetImport().GetNamespaceMap().GetQNameByIndex(
            OAttributeMetaData::getOfficeFormsAttributeNamespace(_eAttribute),
            ::rtl::OUString::createFromAscii(OAttributeMetaData::getOfficeFormsAttributeName(_eAttribute)));

        // get and convert the value
        ::rtl::OUString sAttributeValue = _rxAttributes->getValueByName(sCompleteAttributeName);
        bool bValue = _bDefault;
        ::sax::Converter::convertBool(bValue, sAttributeValue);

        // set the property
        if (_rxPropInfo->hasPropertyByName(_rPropName))
        {
            _rxProps->setPropertyValue(_rPropName, makeAny(bValue));
        }
    }

    //-------------------------------------------------------------------------
    void OFormsRootImport::StartElement( const Reference< XAttributeList >& _rxAttrList )
    {
        ENTER_LOG_CONTEXT( "xmloff::OFormsRootImport - importing the complete tree" );
        SvXMLImportContext::StartElement( _rxAttrList );

        try
        {
            Reference< XPropertySet > xDocProperties(GetImport().GetModel(), UNO_QUERY);
            if ( xDocProperties.is() )
            {   // an empty model is allowed: when doing a copy'n'paste from e.g. Writer to Calc,
                // this is done via streaming the controls as XML.
                Reference< XPropertySetInfo > xDocPropInfo;
                if (xDocProperties.is())
                    xDocPropInfo = xDocProperties->getPropertySetInfo();

                implImportBool(_rxAttrList, ofaAutomaticFocus, xDocProperties, xDocPropInfo, PROPERTY_AUTOCONTROLFOCUS, sal_False);
                implImportBool(_rxAttrList, ofaApplyDesignMode, xDocProperties, xDocPropInfo, PROPERTY_APPLYDESIGNMODE, sal_True);
            }
        }
        catch(Exception&)
        {
            OSL_FAIL("OFormsRootImport::StartElement: caught an exception while setting the document properties!");
        }
    }

    //-------------------------------------------------------------------------
    void OFormsRootImport::EndElement()
    {
        SvXMLImportContext::EndElement();
        LEAVE_LOG_CONTEXT( );
    }

    //=====================================================================
    //= OFormsRootExport
    //=====================================================================
    //---------------------------------------------------------------------
    OFormsRootExport::OFormsRootExport( SvXMLExport& _rExp )
        :m_pImplElement(NULL)
    {
        addModelAttributes(_rExp);

        m_pImplElement = new SvXMLElementExport(_rExp, XML_NAMESPACE_OFFICE, XML_FORMS, sal_True, sal_True);
    }

    //---------------------------------------------------------------------
    OFormsRootExport::~OFormsRootExport( )
    {
        delete m_pImplElement;
    }

    //-------------------------------------------------------------------------
    void OFormsRootExport::implExportBool(SvXMLExport& _rExp, OfficeFormsAttributes _eAttribute,
        const Reference< XPropertySet >& _rxProps, const Reference< XPropertySetInfo >& _rxPropInfo,
        const ::rtl::OUString& _rPropName, sal_Bool _bDefault)
    {
        // retrieve the property value
        sal_Bool bValue = _bDefault;
        if (_rxPropInfo->hasPropertyByName(_rPropName))
            bValue = ::cppu::any2bool(_rxProps->getPropertyValue(_rPropName));

        // convert into a string
        ::rtl::OUStringBuffer aValue;
        ::sax::Converter::convertBool(aValue, bValue);

        // add the attribute
        _rExp.AddAttribute(
            OAttributeMetaData::getOfficeFormsAttributeNamespace(_eAttribute),
            OAttributeMetaData::getOfficeFormsAttributeName(_eAttribute),
            aValue.makeStringAndClear());
    }

    //-------------------------------------------------------------------------
    void OFormsRootExport::addModelAttributes(SvXMLExport& _rExp) SAL_THROW(())
    {
        try
        {
            Reference< XPropertySet > xDocProperties(_rExp.GetModel(), UNO_QUERY);
            if ( xDocProperties.is() )
            {   // an empty model is allowed: when doing a copy'n'paste from e.g. Writer to Calc,
                // this is done via streaming the controls as XML.
                Reference< XPropertySetInfo > xDocPropInfo;
                if (xDocProperties.is())
                    xDocPropInfo = xDocProperties->getPropertySetInfo();

                implExportBool(_rExp, ofaAutomaticFocus, xDocProperties, xDocPropInfo, PROPERTY_AUTOCONTROLFOCUS, sal_False);
                implExportBool(_rExp, ofaApplyDesignMode, xDocProperties, xDocPropInfo, PROPERTY_APPLYDESIGNMODE, sal_True);
            }
        }
        catch(Exception&)
        {
            OSL_FAIL("OFormsRootExport::addModelAttributes: caught an exception while retrieving the document properties!");
        }
    }

//.........................................................................
}   // namespace xmloff
//.........................................................................


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
