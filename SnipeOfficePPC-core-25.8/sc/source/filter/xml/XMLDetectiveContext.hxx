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

#ifndef SC_XMLDETECTIVECONTEXT_HXX
#define SC_XMLDETECTIVECONTEXT_HXX

#include <xmloff/xmlimp.hxx>
#include <com/sun/star/table/CellRangeAddress.hpp>
#include "detfunc.hxx"
#include "detdata.hxx"

#include <list>

class ScXMLImport;


//___________________________________________________________________

struct ScMyImpDetectiveObj
{
    ScRange                     aSourceRange;
    ScDetectiveObjType          eObjType;
    bool                        bHasError;

                                ScMyImpDetectiveObj();
};

typedef ::std::vector< ScMyImpDetectiveObj > ScMyImpDetectiveObjVec;


//___________________________________________________________________

struct ScMyImpDetectiveOp
{
    ScAddress                   aPosition;
    ScDetOpType                 eOpType;
    sal_Int32                   nIndex;

    inline                      ScMyImpDetectiveOp() : nIndex( -1 ) {}
    bool                        operator<(const ScMyImpDetectiveOp& rDetOp) const;
};

typedef ::std::list< ScMyImpDetectiveOp > ScMyImpDetectiveOpList;

class ScMyImpDetectiveOpArray
{
private:
    ScMyImpDetectiveOpList      aDetectiveOpList;

public:
    inline                      ScMyImpDetectiveOpArray() :
                                    aDetectiveOpList()  {}

    inline void                 AddDetectiveOp( const ScMyImpDetectiveOp& rDetOp )
                                    { aDetectiveOpList.push_back( rDetOp ); }

    void                        Sort();
    bool                        GetFirstOp( ScMyImpDetectiveOp& rDetOp );
};


//___________________________________________________________________

class ScXMLDetectiveContext : public SvXMLImportContext
{
private:
    ScMyImpDetectiveObjVec*     pDetectiveObjVec;

    const ScXMLImport&          GetScImport() const { return (const ScXMLImport&)GetImport(); }
    ScXMLImport&                GetScImport()       { return (ScXMLImport&)GetImport(); }

public:
                                ScXMLDetectiveContext(
                                    ScXMLImport& rImport,
                                    sal_uInt16 nPrfx,
                                    const ::rtl::OUString& rLName,
                                    ScMyImpDetectiveObjVec* pNewDetectiveObjVec
                                    );
    virtual                     ~ScXMLDetectiveContext();

    virtual SvXMLImportContext* CreateChildContext(
                                    sal_uInt16 nPrefix,
                                    const ::rtl::OUString& rLocalName,
                                    const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList
                                    );
    virtual void                EndElement();
};


//___________________________________________________________________

class ScXMLDetectiveHighlightedContext : public SvXMLImportContext
{
private:
    ScMyImpDetectiveObjVec*     pDetectiveObjVec;
    ScMyImpDetectiveObj         aDetectiveObj;
    bool                        bValid;

    const ScXMLImport&          GetScImport() const { return (const ScXMLImport&)GetImport(); }
    ScXMLImport&                GetScImport()       { return (ScXMLImport&)GetImport(); }

public:
                                ScXMLDetectiveHighlightedContext(
                                    ScXMLImport& rImport,
                                    sal_uInt16 nPrfx,
                                    const ::rtl::OUString& rLName,
                                    const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList,
                                    ScMyImpDetectiveObjVec* pNewDetectiveObjVec
                                    );
    virtual                     ~ScXMLDetectiveHighlightedContext();

    virtual SvXMLImportContext* CreateChildContext(
                                    sal_uInt16 nPrefix,
                                    const ::rtl::OUString& rLocalName,
                                    const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList
                                    );
    virtual void                EndElement();
};


//___________________________________________________________________

class ScXMLDetectiveOperationContext : public SvXMLImportContext
{
private:
    ScMyImpDetectiveOp          aDetectiveOp;
    bool                        bHasType;

    const ScXMLImport&          GetScImport() const { return (const ScXMLImport&)GetImport(); }
    ScXMLImport&                GetScImport()       { return (ScXMLImport&)GetImport(); }

public:
                                ScXMLDetectiveOperationContext(
                                    ScXMLImport& rImport,
                                    sal_uInt16 nPrfx,
                                    const ::rtl::OUString& rLName,
                                    const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList
                                    );
    virtual                     ~ScXMLDetectiveOperationContext();

    virtual SvXMLImportContext* CreateChildContext(
                                    sal_uInt16 nPrefix,
                                    const ::rtl::OUString& rLocalName,
                                    const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList
                                    );
    virtual void                EndElement();
};


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
