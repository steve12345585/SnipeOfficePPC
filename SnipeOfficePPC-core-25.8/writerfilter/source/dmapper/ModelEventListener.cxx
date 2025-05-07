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
#include <ModelEventListener.hxx>
#include <PropertyIds.hxx>
#include <rtl/oustringostreaminserter.hxx>
#include <com/sun/star/document/XEventBroadcaster.hpp>
#include <com/sun/star/text/XDocumentIndex.hpp>
#include <com/sun/star/text/XDocumentIndexesSupplier.hpp>
#include <com/sun/star/text/XTextFieldsSupplier.hpp>
#include <com/sun/star/util/XRefreshable.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/text/ReferenceFieldPart.hpp>
#include <com/sun/star/text/ReferenceFieldSource.hpp>

namespace writerfilter {
namespace dmapper {

using namespace ::com::sun::star;




ModelEventListener::ModelEventListener()
{
}


ModelEventListener::~ModelEventListener()
{
}


void ModelEventListener::notifyEvent( const document::EventObject& rEvent ) throw (uno::RuntimeException)
{
    if ( rEvent.EventName == "OnFocus" )
    {
        try
        {
            PropertyNameSupplier& rPropNameSupplier = PropertyNameSupplier::GetPropertyNameSupplier();

            uno::Reference< text::XDocumentIndexesSupplier> xIndexesSupplier( rEvent.Source, uno::UNO_QUERY );
            //remove listener
            uno::Reference<document::XEventBroadcaster>(rEvent.Source, uno::UNO_QUERY )->removeEventListener(
            uno::Reference<document::XEventListener>(this));

            uno::Reference< container::XIndexAccess > xIndexes = xIndexesSupplier->getDocumentIndexes();

            sal_Int32 nIndexes = xIndexes->getCount();
            for( sal_Int32 nIndex = 0; nIndex < nIndexes; ++nIndex)
            {
                uno::Reference< text::XDocumentIndex> xIndex( xIndexes->getByIndex( nIndex ), uno::UNO_QUERY );
                xIndex->update();
            }

            // If we have PAGEREF fields, update fields as well.
            uno::Reference<text::XTextFieldsSupplier> xTextFieldsSupplier(rEvent.Source, uno::UNO_QUERY);
            uno::Reference<container::XEnumeration> xEnumeration(xTextFieldsSupplier->getTextFields()->createEnumeration(), uno::UNO_QUERY);
            sal_Int32 nIndex = 0;
            while(xEnumeration->hasMoreElements())
            {
                try
                {
                    uno::Reference<beans::XPropertySet> xPropertySet(xEnumeration->nextElement(), uno::UNO_QUERY);
                    sal_Int16 nSource = 0;
                    xPropertySet->getPropertyValue(rPropNameSupplier.GetName(PROP_REFERENCE_FIELD_SOURCE)) >>= nSource;
                    sal_Int16 nPart = 0;
                    xPropertySet->getPropertyValue(rPropNameSupplier.GetName(PROP_REFERENCE_FIELD_PART)) >>= nPart;
                    if (nSource == text::ReferenceFieldSource::BOOKMARK && nPart == text::ReferenceFieldPart::PAGE)
                        ++nIndex;
                }
                catch( const beans::UnknownPropertyException& )
                {
                    // doesn't even have such a property? ignore
                }
            }
            if (nIndex)
            {
                uno::Reference<util::XRefreshable> xRefreshable(xTextFieldsSupplier->getTextFields(), uno::UNO_QUERY);
                xRefreshable->refresh();
            }
        }
        catch( const uno::Exception& rEx )
        {
            SAL_WARN("writerfilter", "exception while updating indexes: " << rEx.Message);
        }
    }
}


void ModelEventListener::disposing( const lang::EventObject& rEvent ) throw (uno::RuntimeException)
{
    try
    {
        uno::Reference<document::XEventBroadcaster>(rEvent.Source, uno::UNO_QUERY )->removeEventListener(
            uno::Reference<document::XEventListener>(this));
    }
    catch( const uno::Exception& )
    {
    }
}

} //namespace dmapper
} //namespace writerfilter

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
