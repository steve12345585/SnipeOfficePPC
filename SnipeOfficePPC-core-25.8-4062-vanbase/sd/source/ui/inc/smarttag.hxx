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

#ifndef _SD_SMARTTAG_HXX_
#define _SD_SMARTTAG_HXX_

#include "helper/simplereferencecomponent.hxx"
#include <rtl/ref.hxx>
#include <set>
#include <svx/svdhdl.hxx>
#include <svx/svdview.hxx>

class KeyEvent;
class MouseEvent;
class SdrHdlList;

namespace sd {

class View;
class SmartHdl;

/** a smart tag represents a visual user interface element on the documents edit view
    that is not part of the document. It uses derivations from SmartHdl for its visuals.
    A SmartTag adds himself to the given view if created. It removes himself if it
    is disposed before the view is disposed.

    Derive from this class to implement your own smart tag.
*/
class SmartTag : public SimpleReferenceComponent
{
    friend class SmartTagSet;

public:
    explicit SmartTag( ::sd::View& rView );
    virtual ~SmartTag();

    /** returns true if the SmartTag consumes this event. */
    virtual bool MouseButtonDown( const MouseEvent&, SmartHdl& );

    /** returns true if the SmartTag consumes this event. */
    virtual bool KeyInput( const KeyEvent& rKEvt );

    /** returns true if the SmartTag consumes this event. */
    virtual bool RequestHelp( const HelpEvent& rHEvt );

    /** returns true if the SmartTag consumes this event. */
    virtual bool Command( const CommandEvent& rCEvt );

    /** returns true if this smart tag is currently selected */
    bool isSelected() const;

    ::sd::View& getView() const { return mrView; }

protected:
    virtual sal_uLong GetMarkablePointCount() const;
    virtual sal_uLong GetMarkedPointCount() const;
    virtual sal_Bool MarkPoint(SdrHdl& rHdl, sal_Bool bUnmark=sal_False);
    virtual void CheckPossibilities();
    virtual sal_Bool MarkPoints(const Rectangle* pRect, sal_Bool bUnmark);

    virtual void addCustomHandles( SdrHdlList& rHandlerList );
    virtual void select();
    virtual void deselect();
    virtual bool getContext( SdrViewContext& rContext );

    virtual void disposing();

    ::sd::View& mrView;
    bool mbSelected;

private:
    SmartTag( const SmartTag& );                // not implemented
    SmartTag& operator=( const SmartTag& ); // not implemented
};

typedef rtl::Reference< SmartTag > SmartTagReference;

/** class to administrate the available smart tags for a single view. */
class SmartTagSet
{
    friend class SmartTag;
public:
    explicit SmartTagSet( ::sd::View& rView );
    ~SmartTagSet();

    /** selects the given smart tag and updates all handles */
    void select( const SmartTagReference& xTag );

    /** deselects the current selected smart tag and updates all handles */
    void deselect();

    /** returns the currently selected tag or an empty reference. */
    const SmartTagReference& getSelected() const { return mxSelectedTag; }

    /** returns true if a SmartTag consumes this event. */
    bool MouseButtonDown( const MouseEvent& );

    /** returns true if a SmartTag consumes this event. */
    bool KeyInput( const KeyEvent& rKEvt );

    /** returns true if a SmartTag consumes this event. */
    bool RequestHelp( const HelpEvent& rHEvt );

    /** returns true if a SmartTag consumes this event. */
    bool Command( const CommandEvent& rCEvt );

    /** disposes all smart tags and clears the set */
    void Dispose();

    /** addes the handles from all smart tags to the given list */
    void addCustomHandles( SdrHdlList& rHandlerList );

    /** returns true if the currently selected smart tag has
        a special context, returned in rContext. */
    bool getContext( SdrViewContext& rContext ) const;

    // support point editing
    sal_Bool HasMarkablePoints() const;
    sal_uLong GetMarkablePointCount() const;
    sal_Bool HasMarkedPoints() const;
    sal_uLong GetMarkedPointCount() const;
    sal_Bool IsPointMarkable(const SdrHdl& rHdl) const;
    sal_Bool MarkPoint(SdrHdl& rHdl, sal_Bool bUnmark=sal_False);
    sal_Bool MarkPoints(const Rectangle* pRect, sal_Bool bUnmark);

    void CheckPossibilities();

private:
    SmartTagSet( const SmartTagSet& );              // not implemented
    SmartTagSet& operator=( const SmartTagSet& );   // not implemented

    /** adds a new smart tag to this set */
    void add( const SmartTagReference& xTag );

    /** removes the given smart tag from this set */
    void remove( const SmartTagReference& xTag );

     std::set< SmartTagReference > maSet;

    ::sd::View& mrView;
    SmartTagReference mxSelectedTag;
    SmartTagReference mxMouseOverTag;
};

/** a derivation from this handle is the visual representation for a smart tag.
    One smart tag can have more than one handle.
*/
class SmartHdl : public SdrHdl
{
public:
    SmartHdl( const SmartTagReference& xTag, SdrObject* pObject, const Point& rPnt, SdrHdlKind eNewKind=HDL_SMARTTAG );
    SmartHdl( const SmartTagReference& xTag, const Point& rPnt, SdrHdlKind eNewKind=HDL_SMARTTAG );

    const SmartTagReference& getTag() const { return mxTag; }

    virtual bool isMarkable() const;
protected:
    SmartTagReference mxTag;
};

} // end of namespace sd

#endif      // _SD_SMARTTAG_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
