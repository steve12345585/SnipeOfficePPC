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


#ifndef OOX_PPT_ANIMATIONPERSIST
#define OOX_PPT_ANIMATIONPERSIST

#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

#include <rtl/ustring.hxx>

#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/drawing/XShape.hpp>

#include "oox/drawingml/drawingmltypes.hxx"
#include "oox/ppt/slidepersist.hxx"

namespace oox { namespace ppt {

    enum {
        NP_TO = 0,
        NP_FROM, NP_BY, NP_USERDATA, NP_ATTRIBUTENAME,
        NP_ACCELERATION, NP_AUTOREVERSE, NP_DECELERATE, NP_DURATION, NP_FILL,
        NP_REPEATCOUNT, NP_REPEATDURATION, NP_RESTART,
        NP_DIRECTION, NP_COLORINTERPOLATION, NP_CALCMODE, NP_TRANSFORMTYPE,
        NP_PATH,
        NP_ENDSYNC, NP_ITERATETYPE, NP_ITERATEINTERVAL,
        NP_SUBITEM, NP_TARGET, NP_COMMAND, NP_PARAMETER,
        NP_VALUES, NP_FORMULA, NP_KEYTIMES, NP_DISPLAY,
        _NP_SIZE
    };

    typedef boost::array< ::com::sun::star::uno::Any, _NP_SIZE > NodePropertyMap;


    /** data for CT_TLShapeTargetElement */
    struct ShapeTargetElement
    {
        ShapeTargetElement()
            : mnType( 0 )
            {}
        void convert( ::com::sun::star::uno::Any & aAny, sal_Int16 & rSubType ) const;

        sal_Int32               mnType;
        sal_Int32               mnRangeType;
        drawingml::IndexRange   maRange;
        ::rtl::OUString msSubShapeId;
    };


    /** data for CT_TLTimeTargetElement */
    struct AnimTargetElement
    {
        AnimTargetElement()
            : mnType( 0 )
            {}
        /** convert to a set of properties */
        ::com::sun::star::uno::Any convert(const SlidePersistPtr & pSlide, sal_Int16 & nSubType) const;

        sal_Int32                  mnType;
        ::rtl::OUString            msValue;

      ShapeTargetElement         maShapeTarget;
    };

    typedef boost::shared_ptr< AnimTargetElement > AnimTargetElementPtr;

    struct AnimationCondition;

    typedef ::std::list< AnimationCondition > AnimationConditionList;

    /** data for CT_TLTimeCondition */
    struct AnimationCondition
    {
        AnimationCondition()
            : mnType( 0 )
            {}

        ::com::sun::star::uno::Any convert(const SlidePersistPtr & pSlide) const;
        static ::com::sun::star::uno::Any convertList(const SlidePersistPtr & pSlide, const AnimationConditionList & l);

        AnimTargetElementPtr &     getTarget()
            { if(!mpTarget) mpTarget.reset( new AnimTargetElement ); return mpTarget; }
        ::com::sun::star::uno::Any maValue;
        sal_Int32                  mnType;
    private:
        AnimTargetElementPtr       mpTarget;
    };


    struct TimeAnimationValue
    {
        ::rtl::OUString            msFormula;
        ::rtl::OUString            msTime;
        ::com::sun::star::uno::Any maValue;
    };

    typedef ::std::list< TimeAnimationValue > TimeAnimationValueList;

} }





#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
