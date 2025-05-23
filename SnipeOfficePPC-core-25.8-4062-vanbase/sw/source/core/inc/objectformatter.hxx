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
#ifndef _OBJECTFORMATTER_HXX
#define _OBJECTFORMATTER_HXX

#include <sal/types.h>

class SwFrm;
// #i26945#
class SwTxtFrm;
class SwLayoutFrm;
class SwPageFrm;
class SwAnchoredObject;
class SwLayAction;
// OD 2004-10-04 #i26945#
class SwPageNumAndTypeOfAnchors;

// -----------------------------------------------------------------------------
// #i28701#
// Format floating screen objects, which are anchored at the given anchor frame
// and registered at the given page frame.
// -----------------------------------------------------------------------------
class SwObjectFormatter
{
    private:
        // page frame, at which the floating screen objects are registered.
        const SwPageFrm& mrPageFrm;

        // boolean, indicating that only as-character anchored objects have to
        // be formatted.
        bool mbFormatOnlyAsCharAnchored;

        // value of document compatibility option 'Consider wrapping style on
        // object positioning'
        const bool mbConsiderWrapOnObjPos;

        // layout action calling the format of the floating screen objects
        SwLayAction* mpLayAction;

        // data structure to collect page number of object's 'anchor'
        // #i26945#
        SwPageNumAndTypeOfAnchors* mpPgNumAndTypeOfAnchors;

        /** helper method for method <_FormatObj(..)> - performs the intrinsic
            format of the layout of the given layout frame and all its lower
            layout frames.

            #i28701#
            IMPORTANT NOTE:
            Method corresponds to methods <SwLayAction::FormatLayoutFly(..)> and
            <SwLayAction::FormatLayout(..)>. Thus, its code for the formatting have
            to be synchronised.
        */
        void _FormatLayout( SwLayoutFrm& _rLayoutFrm );

        /** helper method for method <_FormatObj(..)> - performs the intrinsic
            format of the content of the given floating screen object.

            #i28701#
        */
        void _FormatObjCntnt( SwAnchoredObject& _rAnchoredObj );

    protected:
        SwObjectFormatter( const SwPageFrm& _rPageFrm,
                           SwLayAction* _pLayAction = 0L,
                           const bool _bCollectPgNumOfAnchors = false );

        static SwObjectFormatter* CreateObjFormatter( SwFrm& _rAnchorFrm,
                                                      const SwPageFrm& _rPageFrm,
                                                      SwLayAction* _pLayAction );

        virtual SwFrm& GetAnchorFrm() = 0;

        inline const SwPageFrm& GetPageFrm() const
        {
            return mrPageFrm;
        }

        inline bool ConsiderWrapOnObjPos() const
        {
            return mbConsiderWrapOnObjPos;
        }

        inline SwLayAction* GetLayAction()
        {
            return mpLayAction;
        }

        /** method to restrict the format of floating screen objects to
            as-character anchored ones
        */
        inline void SetFormatOnlyAsCharAnchored()
        {
            mbFormatOnlyAsCharAnchored = true;
        }

        inline bool FormatOnlyAsCharAnchored() const
        {
            return mbFormatOnlyAsCharAnchored;
        }

        /** performs the intrinsic format of a given floating screen object and its content.

            #i28701#
        */
        void _FormatObj( SwAnchoredObject& _rAnchoredObj );

        /** invokes the intrinsic format method for all floating screen objects,
            anchored at anchor frame on the given page frame

            #i28701#
            #i26945# - for format of floating screen objects for
            follow text frames, the 'master' text frame is passed to the method.
            Thus, the objects, whose anchor character is inside the follow text
            frame can be formatted.

            @param _pMasterTxtFrm
            input parameter - pointer to 'master' text frame. default value: NULL
        */
        bool _FormatObjsAtFrm( SwTxtFrm* _pMasterTxtFrm = 0L );

        /** accessor to collected anchored object

            #i28701#
        */
        SwAnchoredObject* GetCollectedObj( const sal_uInt32 _nIndex );

        /** accessor to 'anchor' page number of collected anchored object

            #i28701#
        */
        sal_uInt32 GetPgNumOfCollected( const sal_uInt32 _nIndex );

        /** accessor to 'anchor' type of collected anchored object

            #i26945#
        */
        bool IsCollectedAnchoredAtMaster( const sal_uInt32 _nIndex );

        /** accessor to total number of collected anchored objects

            #i28701#
        */
        sal_uInt32 CountOfCollected();

    public:
        virtual ~SwObjectFormatter();

        /** intrinsic method to format a certain floating screen object

            #i40147# - add parameter <_bCheckForMovedFwd>

            @param _rAnchoredObj
            input parameter - anchored object, which have to be formatted.

            @param _bCheckForMovedFwd
            input parameter - boolean indicating, that after a successful
            format of the anchored object the anchor frame has to be checked,
            if it would moved forward due to the positioning of the anchored object.
            default value: false
            value only considered, if wrapping style influence has to be
            considered for the positioning of the anchored object.
        */
        virtual bool DoFormatObj( SwAnchoredObject& _rAnchoredObj,
                                  const bool _bCheckForMovedFwd = false ) = 0;

        /** intrinsic method to format all floating screen objects
        */
        virtual bool DoFormatObjs() = 0;

        /** method to format all floating screen objects at the given anchor frame
        */
        static bool FormatObjsAtFrm( SwFrm& _rAnchorFrm,
                                     const SwPageFrm& _rPageFrm,
                                     SwLayAction* _pLayAction = 0L );

        /** method to format a given floating screen object
        */
        static bool FormatObj( SwAnchoredObject& _rAnchoredObj,
                               SwFrm* _pAnchorFrm = 0L,
                               const SwPageFrm* _pPageFrm = 0L,
                               SwLayAction* _pLayAction = 0L );
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
