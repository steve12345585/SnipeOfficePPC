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


#ifndef OOX_DRAWINGML_TIMENODE_HXX
#define OOX_DRAWINGML_TIMENODE_HXX

#include <boost/shared_ptr.hpp>
#include <vector>
#include <list>
#include <rtl/ustring.hxx>

#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/animations/XAnimationNode.hpp>
#include "oox/helper/propertymap.hxx"
#include "oox/ppt/slidetransition.hxx"
#include "oox/ppt/slidepersist.hxx"
#include "oox/ppt/animationspersist.hxx"
#include "oox/ppt/timenode.hxx"

namespace oox { namespace ppt {

    class TimeNode;
    class SlideTransition;

    typedef boost::shared_ptr< TimeNode > TimeNodePtr;
    typedef ::std::list< TimeNodePtr > TimeNodePtrList;

    class TimeNode
    {
    public:
        typedef ::std::map< ::rtl::OUString, ::com::sun::star::uno::Any > UserDataMap;

        TimeNode( sal_Int16 nNodeType );
        virtual ~TimeNode();

        NodePropertyMap & getNodeProperties() { return maNodeProperties; }
        UserDataMap & getUserData() { return maUserData; }
        void addChild( const TimeNodePtr & pChildPtr )
            { maChildren.push_back( pChildPtr ); }

        TimeNodePtrList & getChildren()
            { return maChildren; }

        void setId( sal_Int32 nId );
        const ::rtl::OUString & getId() const { return msId; }

        void addNode(
            const ::oox::core::XmlFilterBase& rFilter,
            const ::com::sun::star::uno::Reference< ::com::sun::star::animations::XAnimationNode >& rxNode,
            const SlidePersistPtr & slide);
        // data setters
        void setTo( const ::com::sun::star::uno::Any & aTo );
        void setFrom( const ::com::sun::star::uno::Any & aFrom );
        void setBy( const ::com::sun::star::uno::Any & aBy );
        void setTransitionFilter( const SlideTransition & aTransition)
            { maTransitionFilter = aTransition; }

        void setNode(
            const ::oox::core::XmlFilterBase& rFilter,
            const ::com::sun::star::uno::Reference< ::com::sun::star::animations::XAnimationNode >& xNode,
            const SlidePersistPtr & pSlide );

        AnimTargetElementPtr getTarget()
            {
                if( !mpTarget )
                    mpTarget.reset( new AnimTargetElement );
                return mpTarget;
            }

        AnimationConditionList &getStartCondition()
            { return maStCondList; }
        AnimationConditionList &getEndCondition()
            { return maEndCondList; }
        AnimationConditionList &getNextCondition()
            { return maNextCondList; }
        AnimationConditionList &getPrevCondition()
            { return maPrevCondList; }
        AnimationCondition & getEndSyncValue()
            { mbHasEndSyncValue = true; return maEndSyncValue; }
    protected:

        static rtl::OUString getServiceName( sal_Int16 nNodeType );

        ::com::sun::star::uno::Reference< ::com::sun::star::animations::XAnimationNode >
        createAndInsert(
            const ::oox::core::XmlFilterBase& rFilter,
            const rtl::OUString& rServiceName,
            const ::com::sun::star::uno::Reference< ::com::sun::star::animations::XAnimationNode >& rxNode );

    private:
        const sal_Int16 mnNodeType;

        TimeNodePtrList maChildren;

        rtl::OUString   msId;
        NodePropertyMap            maNodeProperties;
        UserDataMap                maUserData; // a sequence to be stored as "UserData" property
        SlideTransition            maTransitionFilter;
        AnimTargetElementPtr       mpTarget;
        bool                       mbHasEndSyncValue; // set to true if we try to get the endSync.
        AnimationCondition         maEndSyncValue;
        AnimationConditionList     maStCondList, maEndCondList;
        AnimationConditionList     maPrevCondList, maNextCondList;
    };

} }


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
