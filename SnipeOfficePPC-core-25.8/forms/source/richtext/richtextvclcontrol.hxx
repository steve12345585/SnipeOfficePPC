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
#ifndef FORMS_SOURCE_RICHTEXT_RICHTEXTVCLCONTROL_HXX
#define FORMS_SOURCE_RICHTEXT_RICHTEXTVCLCONTROL_HXX

#include <vcl/dialog.hxx>
#include <vcl/fixed.hxx>
#include <vcl/button.hxx>
#include <vcl/toolbox.hxx>
#include <com/sun/star/awt/FontDescriptor.hpp>
#include "rtattributes.hxx"
#include "textattributelistener.hxx"

/** === begin UNO includes === **/
/** === end UNO includes === **/

class EditView;
class EditEngine;
class SfxItemSet;
//........................................................................
namespace frm
{
//........................................................................

    class RichTextControlImpl;
    class RichTextEngine;
    //====================================================================
    //= RichTextControl
    //====================================================================
    class RichTextControl : public Control, public IMultiAttributeDispatcher
    {
    private:
        RichTextControlImpl*    m_pImpl;

    public:
        RichTextControl(
            RichTextEngine* _pEngine,
            Window* _pParent,
            WinBits _nStyle,
            ITextAttributeListener* _pTextAttribListener,
            ITextSelectionListener* _pSelectionListener
        );

        ~RichTextControl( );

        /* enables the change notifications for a particular attribute

           If you want to be notified of any changes in the state of an attribute, you need to call enableAttributeNotification.

           If you provide a dedicated listener for this attribute, this listener is called for every change in the state of
           the attribute.

           No matter whether you provide such a dedicated listener, the "global" listener which you specified
           in the constructor of the control is also called for all changes in the attribute state.

           If you previously already enabled the notification for this attribute, and specified a different listener,
           then the previous listener will be replaced with the new listener, provided the latter is not <NULL/>.
        */
        void        enableAttributeNotification( AttributeId _nAttributeId, ITextAttributeListener* _pListener = NULL );

        /** disables the change notifications for a particular attribute

            If there was a listener dedicated to this attribute, it will not be referenced and used anymore
            after this method had been called
        */
        void        disableAttributeNotification( AttributeId _nAttributeId );

        /** determines whether a given slot can be mapped to an aspect of an attribute of the EditEngine

            E.g. SID_ATTR_PARA_ADJUST_LEFT can, though it's not part of the EditEngine pool, be mapped
            to the SID_ATTR_PARA_ADJUST slot, which in fact *is* usable with the EditEngine.
        */
        static bool isMappableSlot( SfxSlotId _nSlotId );

        // IMultiAttributeDispatcher
        virtual AttributeState  getState( AttributeId _nAttributeId ) const;
        virtual void            executeAttribute( AttributeId _nAttributeId, const SfxPoolItem* _pArgument );

        void    SetBackgroundColor( );
        void    SetBackgroundColor( const Color& _rColor );

        void    SetReadOnly( bool _bReadOnly );
        bool    IsReadOnly() const;

        void    SetHideInactiveSelection( bool _bHide );
        bool    GetHideInactiveSelection() const;

        const EditView& getView() const;
              EditView& getView();

        // Window overridables
        virtual void        Draw( OutputDevice* _pDev, const Point& _rPos, const Size& _rSize, sal_uLong _nFlags );

    protected:
        // Window overridables
        virtual void        Resize();
        virtual void        GetFocus();
        virtual void        StateChanged( StateChangedType nStateChange );
        virtual long        PreNotify( NotifyEvent& _rNEvt );
        virtual long        Notify( NotifyEvent& _rNEvt );

    private:
                void    applyAttributes( const SfxItemSet& _rAttributesToApply );
                void    implInit( RichTextEngine* _pEngine, ITextAttributeListener* _pTextAttribListener, ITextSelectionListener* _pSelectionListener );
        static  WinBits implInitStyle( WinBits nStyle );

    private:
        EditEngine&  getEngine() const;
        Window&      getViewport() const;
    };

//........................................................................
} // namespace frm
//........................................................................

#endif // FORMS_SOURCE_RICHTEXT_RICHTEXTVCLCONTROL_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
