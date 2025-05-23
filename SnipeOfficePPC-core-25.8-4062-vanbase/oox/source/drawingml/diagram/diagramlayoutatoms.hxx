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

#ifndef OOX_DRAWINGML_DIAGRAMLAYOUTATOMS_HXX
#define OOX_DRAWINGML_DIAGRAMLAYOUTATOMS_HXX

#include <map>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/xml/sax/XFastAttributeList.hpp>

#include "oox/drawingml/shape.hxx"
#include "diagram.hxx"


namespace oox { namespace drawingml {

class DiagramLayout;
typedef boost::shared_ptr< DiagramLayout > DiagramLayoutPtr;

// AG_IteratorAttributes
struct IteratorAttr
{
    IteratorAttr();

    // not sure this belong here, but wth
    void loadFromXAttr( const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XFastAttributeList >& xAttributes );

    sal_Int32 mnAxis;
    sal_Int32 mnCnt;
    sal_Bool  mbHideLastTrans;
    sal_Int32 mnPtType;
    sal_Int32 mnSt;
    sal_Int32 mnStep;
};

struct ConditionAttr
{
    ConditionAttr();

    // not sure this belong here, but wth
    void loadFromXAttr( const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XFastAttributeList >& xAttributes );

    sal_Int32 mnFunc;
    sal_Int32 mnArg;
    sal_Int32 mnOp;
    ::rtl::OUString msVal;
};

struct LayoutAtomVisitor;
class LayoutAtom;

typedef boost::shared_ptr< LayoutAtom > LayoutAtomPtr;

/** abstract Atom for the layout */
class LayoutAtom
{
public:
    virtual ~LayoutAtom() { }

    /** visitor acceptance
     */
    virtual void accept( LayoutAtomVisitor& ) = 0;

    void setName( const ::rtl::OUString& sName )
        { msName = sName; }
    const ::rtl::OUString& getName() const
        { return msName; }

    virtual void addChild( const LayoutAtomPtr & pNode )
        { mpChildNodes.push_back( pNode ); }
    virtual const std::vector<LayoutAtomPtr>& getChildren() const
        { return mpChildNodes; }

    // dump for debug
    void dump(int level = 0);
protected:
    std::vector< LayoutAtomPtr > mpChildNodes;
    ::rtl::OUString              msName;
};

class ConstraintAtom
    : public LayoutAtom
{
public:
    ConstraintAtom() :
        mnFor(-1), msForName(), mnPointType(-1), mnType(-1), mnRefFor(-1), msRefForName(),
        mnRefType(-1), mnRefPointType(-1), mfFactor(1.0), mfValue(0.0), mnOperator(0)
    {}

    virtual ~ConstraintAtom() { }

    virtual void accept( LayoutAtomVisitor& );

    void setFor( sal_Int32 nToken )
        { mnFor = nToken; }
    void setForName( const ::rtl::OUString & sName )
        { msForName = sName; }
    void setPointType( sal_Int32 nToken )
        { mnPointType = nToken; }
    void setType( sal_Int32 nToken )
        { mnType = nToken; }
    void setRefFor( sal_Int32 nToken )
        { mnRefFor = nToken; }
    void setRefForName( const ::rtl::OUString & sName )
        { msRefForName = sName; }
    void setRefType( sal_Int32 nToken )
        { mnRefType = nToken; }
    void setRefPointType( sal_Int32 nToken )
        { mnRefPointType = nToken; }
    void setFactor( const double& fVal )
        { mfFactor = fVal; }
    void setValue( const double& fVal )
        { mfValue = fVal; }
    void setOperator( sal_Int32 nToken )
        { mnOperator = nToken; }
private:
    sal_Int32 mnFor;
    ::rtl::OUString msForName;
    sal_Int32 mnPointType;
    sal_Int32 mnType;
    sal_Int32 mnRefFor;
    ::rtl::OUString msRefForName;
    sal_Int32 mnRefType;
    sal_Int32 mnRefPointType;
    double mfFactor;
    double mfValue;
    sal_Int32 mnOperator;
};

typedef boost::shared_ptr< ConstraintAtom > ConstraintAtomPtr;

class AlgAtom
    : public LayoutAtom
{
public:
    AlgAtom() : mnType(0), maMap() {}

    virtual ~AlgAtom() { }

    typedef std::map<sal_Int32,sal_Int32> ParamMap;

    virtual void accept( LayoutAtomVisitor& );

    void setType( sal_Int32 nToken )
        { mnType = nToken; }
    void addParam( sal_Int32 nType, sal_Int32 nVal )
        { maMap[nType]=nVal; }
    void layoutShape( const ShapePtr& rShape,
                      const Diagram& rDgm,
                      const rtl::OUString& rName ) const;
private:
    sal_Int32 mnType;
    ParamMap  maMap;
};

typedef boost::shared_ptr< AlgAtom > AlgAtomPtr;

class ForEachAtom
    : public LayoutAtom
{
public:
    explicit ForEachAtom(const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XFastAttributeList >& xAttributes);

    virtual ~ForEachAtom() { }

    IteratorAttr & iterator()
        { return maIter; }
    virtual void accept( LayoutAtomVisitor& );

private:
    IteratorAttr maIter;
};

typedef boost::shared_ptr< ForEachAtom > ForEachAtomPtr;


class ConditionAtom
    : public LayoutAtom
{
public:
    explicit ConditionAtom(const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XFastAttributeList >& xAttributes);
    virtual ~ConditionAtom()
        { }
    bool test();
    virtual void accept( LayoutAtomVisitor& );
    IteratorAttr & iterator()
        { return maIter; }
    ConditionAttr & cond()
        { return maCond; }
    void readElseBranch()
        { mbElse=true; }
    virtual void addChild( const LayoutAtomPtr & pNode );
    virtual const std::vector<LayoutAtomPtr>& getChildren() const;
private:
    bool          mbElse;
    IteratorAttr  maIter;
    ConditionAttr maCond;
    std::vector< LayoutAtomPtr > mpElseChildNodes;
};

typedef boost::shared_ptr< ConditionAtom > ConditionAtomPtr;


/** "choose" statements. Atoms will be tested in order. */
class ChooseAtom
    : public LayoutAtom
{
public:
    virtual ~ChooseAtom()
        { }
    virtual void accept( LayoutAtomVisitor& );
};

class LayoutNode
    : public LayoutAtom
{
public:
    enum {
        VAR_animLvl = 0,
        VAR_animOne,
        VAR_bulletEnabled,
        VAR_chMax,
        VAR_chPref,
        VAR_dir,
        VAR_hierBranch,
        VAR_orgChart,
        VAR_resizeHandles
    };
    // we know that the array is of fixed size
    // the use of Any allow having empty values
    typedef boost::array< ::com::sun::star::uno::Any, 9 > VarMap;

    LayoutNode() : mnChildOrder(0) {}
    virtual ~LayoutNode() { }
    virtual void accept( LayoutAtomVisitor& );
    VarMap & variables()
        { return mVariables; }
    void setMoveWith( const ::rtl::OUString & sName )
        { msMoveWith = sName; }
    void setStyleLabel( const ::rtl::OUString & sLabel )
        { msStyleLabel = sLabel; }
    void setChildOrder( sal_Int32 nOrder )
        { mnChildOrder = nOrder; }
    void setShape( const ShapePtr& pShape )
        { mpShape = pShape; }
    const ShapePtr& getShape() const
        { return mpShape; }

    bool setupShape( const ShapePtr& rShape,
                     const Diagram& rDgm,
                     sal_uInt32 nIdx ) const;

private:
    VarMap                       mVariables;
    ::rtl::OUString              msMoveWith;
    ::rtl::OUString              msStyleLabel;
    ShapePtr                     mpShape;
    sal_Int32                    mnChildOrder;
};

typedef boost::shared_ptr< LayoutNode > LayoutNodePtr;

struct LayoutAtomVisitor
{
    virtual ~LayoutAtomVisitor() {}
    virtual void visit(ConstraintAtom& rAtom) = 0;
    virtual void visit(AlgAtom& rAtom) = 0;
    virtual void visit(ForEachAtom& rAtom) = 0;
    virtual void visit(ConditionAtom& rAtom) = 0;
    virtual void visit(ChooseAtom& rAtom) = 0;
    virtual void visit(LayoutNode& rAtom) = 0;
};

class ShapeCreationVisitor : public LayoutAtomVisitor
{
    ShapePtr mpParentShape;
    const Diagram& mrDgm;
    sal_Int32 mnCurrIdx;

    void defaultVisit(LayoutAtom& rAtom);
    virtual void visit(ConstraintAtom& rAtom);
    virtual void visit(AlgAtom& rAtom);
    virtual void visit(ForEachAtom& rAtom);
    virtual void visit(ConditionAtom& rAtom);
    virtual void visit(ChooseAtom& rAtom);
    virtual void visit(LayoutNode& rAtom);

public:
    ShapeCreationVisitor(const ShapePtr& rParentShape,
                         const Diagram& rDgm) :
        mpParentShape(rParentShape),
        mrDgm(rDgm),
        mnCurrIdx(0)
    {}
};

} }

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
