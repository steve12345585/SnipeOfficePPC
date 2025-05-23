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


// must be first
#include <canvas/debug.hxx>
#include <canvas/verbosetrace.hxx>
#include <com/sun/star/animations/AnimationTransformType.hpp>

#include "animationtransformnode.hxx"
#include "animationfactory.hxx"
#include "activitiesfactory.hxx"

using namespace com::sun::star;

namespace slideshow {
namespace internal {

void AnimationTransformNode::dispose()
{
    mxTransformNode.clear();
    AnimationBaseNode::dispose();
}

AnimationActivitySharedPtr AnimationTransformNode::createActivity() const
{
    ActivitiesFactory::CommonParameters aParms( fillCommonParameters() );

    const sal_Int16 nTransformType( mxTransformNode->getTransformType() );

    const AttributableShapeSharedPtr& rShape( getShape() );

    switch( nTransformType )
    {
    default:
        ENSURE_OR_THROW(
            false, "AnimationTransformNode::createTransformActivity(): "
            "Unknown transform type" );

    case animations::AnimationTransformType::TRANSLATE:
        // FALLTHROUGH intended
    case animations::AnimationTransformType::SCALE:
        return ActivitiesFactory::createAnimateActivity(
            aParms,
            AnimationFactory::createPairPropertyAnimation(
                rShape,
                getContext().mpSubsettableShapeManager,
                getSlideSize(),
                nTransformType ),
            getXAnimateNode() );

    case animations::AnimationTransformType::ROTATE:
        return ActivitiesFactory::createAnimateActivity(
            aParms,
            AnimationFactory::createNumberPropertyAnimation(
                ::rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM("Rotate") ),
                rShape,
                getContext().mpSubsettableShapeManager,
                getSlideSize() ),
            getXAnimateNode() );

    case animations::AnimationTransformType::SKEWX:
        return ActivitiesFactory::createAnimateActivity(
            aParms,
            AnimationFactory::createNumberPropertyAnimation(
                ::rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM("SkewX") ),
                rShape,
                getContext().mpSubsettableShapeManager,
                getSlideSize() ),
            getXAnimateNode() );

    case animations::AnimationTransformType::SKEWY:
        return ActivitiesFactory::createAnimateActivity(
            aParms,
            AnimationFactory::createNumberPropertyAnimation(
                ::rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM("SkewY") ),
                rShape,
                getContext().mpSubsettableShapeManager,
                getSlideSize() ),
            getXAnimateNode() );
    }
}

} // namespace internal
} // namespace slideshow

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
