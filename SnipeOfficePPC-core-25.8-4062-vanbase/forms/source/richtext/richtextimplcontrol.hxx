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
#ifndef FORMS_SOURCE_RICHTEXT_RICHTEXTIMPLCONTOL_HXX
#define FORMS_SOURCE_RICHTEXT_RICHTEXTIMPLCONTOL_HXX

#include "rtattributehandler.hxx"
#include "richtextviewport.hxx"
#include "richtextengine.hxx"
#include <vcl/scrbar.hxx>
#include <editeng/editdata.hxx>

#include <map>

class EditView;
class EditStatus;
class Window;
class SvxScriptSetItem;
//........................................................................
namespace frm
{
//........................................................................

    class ITextAttributeListener;
    class ITextSelectionListener;
    class RichTextViewPort;
    //====================================================================
    //= RichTextControlImpl
    //====================================================================
    class RichTextControlImpl : public IEngineStatusListener
    {
        typedef ::std::map< AttributeId, AttributeState >                           StateCache;
        typedef ::std::map< AttributeId, ::rtl::Reference< IAttributeHandler > >    AttributeHandlerPool;
        typedef ::std::map< AttributeId, ITextAttributeListener* >                  AttributeListenerPool;

        StateCache              m_aLastKnownStates;
        AttributeHandlerPool    m_aAttributeHandlers;
        AttributeListenerPool   m_aAttributeListeners;

        ESelection              m_aLastKnownSelection;

        Control*                m_pAntiImpl;
        RichTextViewPort*       m_pViewport;
        ScrollBar*              m_pHScroll;
        ScrollBar*              m_pVScroll;
        ScrollBarBox*           m_pScrollCorner;
        RichTextEngine*         m_pEngine;
        EditView*               m_pView;
        ITextAttributeListener* m_pTextAttrListener;
        ITextSelectionListener* m_pSelectionListener;
        bool                    m_bHasEverBeenShown;

    public:
        struct GrantAccess { friend class RichTextControl; private: GrantAccess() { } };
        inline EditView*        getView( const GrantAccess& ) const     { return m_pView; }
        inline RichTextEngine*  getEngine( const GrantAccess& ) const   { return m_pEngine; }
        inline Window*          getViewport( const GrantAccess& ) const { return m_pViewport; }

    public:
        RichTextControlImpl( Control* _pAntiImpl, RichTextEngine* _pEngine,
            ITextAttributeListener* _pTextAttrListener, ITextSelectionListener* _pSelectionListener );
        virtual ~RichTextControlImpl();

        /** updates the cache with the state of all attribute values from the given set, notifies
            the listener if the state changed
        */
        void    updateAllAttributes( );

        /** updates the cache with the state of the attribute given by which id, notifies
            the listener if the state changed
        */
        void    updateAttribute( AttributeId _nAttribute );

        /// enables the callback for a particular attribute
        void    enableAttributeNotification( AttributeId _nAttributeId, ITextAttributeListener* _pListener = NULL );

        /// disables the change notifications for a particular attribute
        void    disableAttributeNotification( AttributeId _nAttributeId );

        /// executes a toggle of the given attribute
        bool    executeAttribute( const SfxItemSet& _rCurrentAttribs, SfxItemSet& _rNewAttribs, AttributeId _nAttribute, const SfxPoolItem* _pArgument, ScriptType _nForScriptType );

        /// retrieves the state of the given attribute from the cache
        AttributeState  getAttributeState( AttributeId _nAttributeId ) const;

        /** normalizes the given item so that the state of script dependent attributes
            is correct considering the current script type

            There are some attributes which are script dependent, e.g. the CharPosture. This means
            that in real, there are 3 attributes for this, one for every possible script type (latin,
            asian, complex). However, to the out world, we behave as if there is only one attribute:
            E.g., if the outter world asks for the state of the "CharPosture" attribute, we return
            the state of either CharPostureLatin, CharPostureAsian, or CharPostureComplex, depending
            on the script type of the current selection. (In real, it may be more complex since
            the current selection may contain more than one script type.)

            This method normalizes a script dependent attribute, so that it's state takes into account
            the currently selected script type.
        */
        void        normalizeScriptDependentAttribute( SvxScriptSetItem& _rScriptSetItem );

        // gets the script type of the selection in our edit view (with fallback)
        ScriptType  getSelectedScriptType() const;

        /** re-arranges the view and the scrollbars
        */
        void    layoutWindow();

        /** to be called when the style of our window changed
        */
        void    notifyStyleChanged();

        /** to be called when the zoom of our window changed
        */
        void    notifyZoomChanged();

        /** to be called when the STATE_CHANGE_INITSHOW event arrives
        */
        void    notifyInitShow();

        // VCL "overrides"
        void    SetBackgroundColor( );
        void    SetBackgroundColor( const Color& _rColor );

        void    SetReadOnly( bool _bReadOnly );
        bool    IsReadOnly() const;

        void    SetHideInactiveSelection( bool _bHide );
        bool    GetHideInactiveSelection() const;

        /// draws the control onto a given output device
        void    Draw( OutputDevice* _pDev, const Point& _rPos, const Size& _rSize, sal_uLong _nFlags );

        /// handles command events arrived at the anti-impl control
        long    HandleCommand( const CommandEvent& _rEvent );

    private:
        // updates the cache with the state provided by the given attribut handler
        void    implUpdateAttribute( AttributeHandlerPool::const_iterator _pHandler );

        // updates the cache with the given state, and calls listeners (if necessary)
        void    implCheckUpdateCache( AttributeId _nAttribute, const AttributeState& _rState );

        // updates range and position of our scrollbars
        void    updateScrollbars();

        // determines whether automatic (soft) line breaks are ON
        bool    windowHasAutomaticLineBreak();

        /// hides or shows our scrollbars, according to the current WinBits of the window
        void    ensureScrollbars();

        /// ensures that our "automatic line break" setting matches the current WinBits of the window
        void    ensureLineBreakSetting();

        inline  bool    hasVScrollBar( ) const { return m_pVScroll != NULL; }
        inline  bool    hasHScrollBar( ) const { return m_pHScroll != NULL; }

        // IEngineStatusListener overridables
        virtual void EditEngineStatusChanged( const EditStatus& _rStatus );

    private:
        DECL_LINK( OnInvalidateAllAttributes, void* );
        DECL_LINK( OnHScroll, ScrollBar* );
        DECL_LINK( OnVScroll, ScrollBar* );
    };

//........................................................................
} // namespace frm
//........................................................................

#endif // FORMS_SOURCE_RICHTEXT_RICHTEXTIMPLCONTOL_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
