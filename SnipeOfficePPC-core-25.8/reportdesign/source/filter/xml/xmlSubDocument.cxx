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
#include "xmlSubDocument.hxx"
#include "xmlCell.hxx"
#include "xmlfilter.hxx"
#include <xmloff/xmltoken.hxx>
#include <xmloff/xmlnmspe.hxx>
#include <xmloff/nmspmap.hxx>
#include "xmlEnums.hxx"
#include "xmlMasterFields.hxx"
#include "xmlTable.hxx"
#include <tools/debug.hxx>
#include <comphelper/property.hxx>
#include <com/sun/star/report/XReportControlModel.hpp>

namespace rptxml
{
    using namespace ::com::sun::star;
    using namespace ::com::sun::star::report;
    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::xml::sax;
DBG_NAME( rpt_OXMLSubDocument )

OXMLSubDocument::OXMLSubDocument( ORptFilter& rImport,
                sal_uInt16 nPrfx
                ,const ::rtl::OUString& rLName
                ,const Reference< XReportComponent > & _xComponent
                ,OXMLTable* _pContainer
                ,OXMLCell* _pCellParent) :
    OXMLReportElementBase( rImport, nPrfx, rLName,_xComponent.get(),_pContainer)
,m_xFake(_xComponent)
,m_pCellParent(_pCellParent)
,m_nCurrentCount(0)
,m_bContainsShape(false)
{
    DBG_CTOR( rpt_OXMLSubDocument,NULL);

}
// -----------------------------------------------------------------------------

OXMLSubDocument::~OXMLSubDocument()
{
    DBG_DTOR( rpt_OXMLSubDocument,NULL);
}

// -----------------------------------------------------------------------------
SvXMLImportContext* OXMLSubDocument::_CreateChildContext(
        sal_uInt16 _nPrefix,
        const ::rtl::OUString& _rLocalName,
        const Reference< XAttributeList > & xAttrList )
{
    SvXMLImportContext *pContext = OXMLReportElementBase::_CreateChildContext(_nPrefix,_rLocalName,xAttrList);
    if ( pContext )
        return pContext;
    const SvXMLTokenMap&    rTokenMap   = static_cast<ORptFilter&>(GetImport()).GetReportElemTokenMap();

    switch( rTokenMap.Get( _nPrefix, _rLocalName ) )
    {
        case XML_TOK_MASTER_DETAIL_FIELDS:
            {
                GetImport().GetProgressBarHelper()->Increment( PROGRESS_BAR_STEP );
                pContext = new OXMLMasterFields(static_cast<ORptFilter&>(GetImport()), _nPrefix, _rLocalName,xAttrList ,this);
            }
            break;
        case XML_TOK_SUB_FRAME:
            {
                if ( !m_bContainsShape )
                    m_nCurrentCount = m_pContainer->getSection()->getCount();
                UniReference< XMLShapeImportHelper > xShapeImportHelper = GetImport().GetShapeImport();
                uno::Reference< drawing::XShapes > xShapes = m_pContainer->getSection().get();
                pContext = xShapeImportHelper->CreateGroupChildContext(GetImport(),_nPrefix,_rLocalName,xAttrList,xShapes);
                m_bContainsShape = true;
                if (m_pCellParent)
                {
                    // #i94115 say to the parent Cell it contains shapes
                    m_pCellParent->setContainsShape(true);
                }
            }
            break;
        default:
            break;
    }

    if( !pContext )
        pContext = new SvXMLImportContext( GetImport(), _nPrefix, _rLocalName );

    return pContext;
}
// -----------------------------------------------------------------------------
void OXMLSubDocument::EndElement()
{
    if ( m_bContainsShape )
    {
        m_xComponent.set(m_pContainer->getSection()->getByIndex(m_nCurrentCount),uno::UNO_QUERY);
        if ( m_xComponent.is() )
        {
            if ( !m_aMasterFields.empty() )
                m_xComponent->setMasterFields(Sequence< ::rtl::OUString>(&*m_aMasterFields.begin(),m_aMasterFields.size()));
            if ( !m_aDetailFields.empty() )
                m_xComponent->setDetailFields(Sequence< ::rtl::OUString>(&*m_aDetailFields.begin(),m_aDetailFields.size()));

            m_xComponent->setName(m_xFake->getName());
            m_xComponent->setPrintRepeatedValues(m_xFake->getPrintRepeatedValues());
            uno::Reference< report::XReportControlModel >   xFakeModel(m_xFake,uno::UNO_QUERY);
            uno::Reference< report::XReportControlModel >   xComponentModel(m_xComponent,uno::UNO_QUERY);
            if ( xComponentModel.is() && xFakeModel.is() )
            {
                const sal_Int32 nCount = xFakeModel->getCount();
                try
                {
                    for (sal_Int32 i = 0; i < nCount ; ++i)
                    {
                        uno::Reference< report::XFormatCondition > xCond(xFakeModel->getByIndex(i),uno::UNO_QUERY);
                        uno::Reference< report::XFormatCondition > xNewCond = xComponentModel->createFormatCondition();
                        ::comphelper::copyProperties(xCond.get(),xNewCond.get());
                        xComponentModel->insertByIndex(xComponentModel->getCount(),uno::makeAny(xNewCond));
                    }
                }
                catch(uno::Exception&)
                {
                    OSL_FAIL("Can not access format condition!");
                }

            }
        }
    }
}
// -----------------------------------------------------------------------------
void OXMLSubDocument::addMasterDetailPair(const ::std::pair< ::rtl::OUString,::rtl::OUString >& _aPair)
{
    m_aMasterFields.push_back(_aPair.first);
    m_aDetailFields.push_back(_aPair.second);
}
//----------------------------------------------------------------------------
} // namespace rptxml
// -----------------------------------------------------------------------------

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
