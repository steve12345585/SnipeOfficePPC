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

#ifndef INCLUDED_SVGIO_SVGREADER_SVGDOCUMENTHANDLER_HXX
#define INCLUDED_SVGIO_SVGREADER_SVGDOCUMENTHANDLER_HXX

#include <com/sun/star/xml/sax/XDocumentHandler.hpp>
#include <svgio/svgreader/svgdocument.hxx>

//////////////////////////////////////////////////////////////////////////////
// predefines

namespace svgio { namespace svgreader { class SvgCharacterNode; }}

//////////////////////////////////////////////////////////////////////////////

namespace svgio
{
    namespace svgreader
    {
        class SvgDocHdl : public cppu::WeakImplHelper1< com::sun::star::xml::sax::XDocumentHandler >
        {
        private:
            // the complete SVG Document
            SvgDocument                     maDocument;

            // current node for parsing
            SvgNode*                        mpTarget;

            // text collector string stack for css styles
            std::vector< rtl::OUString >    maCssContents;

        public:
            SvgDocHdl(const rtl::OUString& rAbsolutePath);
            ~SvgDocHdl();

            // Methods XDocumentHandler
            virtual void SAL_CALL startDocument(  ) throw (com::sun::star::xml::sax::SAXException, com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL endDocument(  ) throw (com::sun::star::xml::sax::SAXException, com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL startElement( const ::rtl::OUString& aName, const com::sun::star::uno::Reference< com::sun::star::xml::sax::XAttributeList >& xAttribs ) throw (com::sun::star::xml::sax::SAXException, com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL endElement( const ::rtl::OUString& aName ) throw (com::sun::star::xml::sax::SAXException, com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL characters( const ::rtl::OUString& aChars ) throw (com::sun::star::xml::sax::SAXException, com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL ignorableWhitespace( const ::rtl::OUString& aWhitespaces ) throw (com::sun::star::xml::sax::SAXException, com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL processingInstruction( const ::rtl::OUString& aTarget, const ::rtl::OUString& aData ) throw (com::sun::star::xml::sax::SAXException, com::sun::star::uno::RuntimeException);
            virtual void SAL_CALL setDocumentLocator( const com::sun::star::uno::Reference< com::sun::star::xml::sax::XLocator >& xLocator ) throw (com::sun::star::xml::sax::SAXException, com::sun::star::uno::RuntimeException);

            const SvgDocument& getSvgDocument() const { return maDocument; }
        };
    } // end of namespace svgreader
} // end of namespace svgio

//////////////////////////////////////////////////////////////////////////////

#endif //INCLUDED_SVGIO_SVGREADER_SVGDOCUMENTHANDLER_HXX

// eof

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
