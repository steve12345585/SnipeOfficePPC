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

#ifndef _UNOCONTROLS_PROGRESSMONITOR_CTRL_HXX
#define _UNOCONTROLS_PROGRESSMONITOR_CTRL_HXX

//____________________________________________________________________________________________________________
//  includes of other projects
//____________________________________________________________________________________________________________

#include <com/sun/star/awt/XFileDialog.hpp>
#include <com/sun/star/awt/XTextComponent.hpp>
#include <com/sun/star/awt/XListBox.hpp>
#include <com/sun/star/awt/XProgressMonitor.hpp>
#include <com/sun/star/awt/TextAlign.hpp>
#include <com/sun/star/awt/XScrollBar.hpp>
#include <com/sun/star/awt/XVclContainerPeer.hpp>
#include <com/sun/star/awt/XTabControllerModel.hpp>
#include <com/sun/star/awt/XMessageBox.hpp>
#include <com/sun/star/awt/XTextEditField.hpp>
#include <com/sun/star/awt/Style.hpp>
#include <com/sun/star/awt/XTimeField.hpp>
#include <com/sun/star/awt/XVclWindowPeer.hpp>
#include <com/sun/star/awt/XControlModel.hpp>
#include <com/sun/star/awt/XSpinField.hpp>
#include <com/sun/star/awt/XUnoControlContainer.hpp>
#include <com/sun/star/awt/XTextLayoutConstrains.hpp>
#include <com/sun/star/awt/XNumericField.hpp>
#include <com/sun/star/awt/XButton.hpp>
#include <com/sun/star/awt/XTextArea.hpp>
#include <com/sun/star/awt/XImageButton.hpp>
#include <com/sun/star/awt/XFixedText.hpp>
#include <com/sun/star/awt/XControlContainer.hpp>
#include <com/sun/star/awt/XDialog.hpp>
#include <com/sun/star/awt/ScrollBarOrientation.hpp>
#include <com/sun/star/awt/XRadioButton.hpp>
#include <com/sun/star/awt/XCurrencyField.hpp>
#include <com/sun/star/awt/XPatternField.hpp>
#include <com/sun/star/awt/VclWindowPeerAttribute.hpp>
#include <com/sun/star/awt/XTabController.hpp>
#include <com/sun/star/awt/XVclContainer.hpp>
#include <com/sun/star/awt/XDateField.hpp>
#include <com/sun/star/awt/XComboBox.hpp>
#include <com/sun/star/awt/XControl.hpp>
#include <com/sun/star/awt/XCheckBox.hpp>
#include <com/sun/star/awt/MessageBoxCommand.hpp>
#include <com/sun/star/awt/XLayoutConstrains.hpp>
#include <com/sun/star/awt/XProgressBar.hpp>

#include <vector>

//____________________________________________________________________________________________________________
//  includes of my own project
//____________________________________________________________________________________________________________
#include "basecontainercontrol.hxx"

//____________________________________________________________________________________________________________
//  namespace
//____________________________________________________________________________________________________________

namespace unocontrols{

//____________________________________________________________________________________________________________
//  defines
//____________________________________________________________________________________________________________

#define SERVICENAME_PROGRESSMONITOR                     "com.sun.star.awt.XProgressMonitor"
#define IMPLEMENTATIONNAME_PROGRESSMONITOR              "stardiv.UnoControls.ProgressMonitor"
#define PROGRESSMONITOR_FREEBORDER                      10                                                      // border around and between the controls
#define FIXEDTEXT_SERVICENAME                           "com.sun.star.awt.UnoControlFixedText"
#define BUTTON_SERVICENAME                              "com.sun.star.awt.UnoControlButton"
#define FIXEDTEXT_MODELNAME                             "com.sun.star.awt.UnoControlFixedTextModel"
#define BUTTON_MODELNAME                                "com.sun.star.awt.UnoControlButtonModel"
#define CONTROLNAME_TEXT                                "Text"                                                  // identifier the control in container
#define CONTROLNAME_BUTTON                              "Button"                                                //              -||-
#define CONTROLNAME_PROGRESSBAR                         "ProgressBar"                                           //              -||-
#define DEFAULT_BUTTONLABEL                             "Abbrechen"
#define PROGRESSMONITOR_DEFAULT_TOPIC                   "\0"
#define PROGRESSMONITOR_DEFAULT_TEXT                    "\0"
#define PROGRESSMONITOR_BACKGROUNDCOLOR                 TRGB_COLORDATA( 0x00, 0xC0, 0xC0, 0xC0 )                // lighgray
#define PROGRESSMONITOR_LINECOLOR_BRIGHT                TRGB_COLORDATA( 0x00, 0xFF, 0xFF, 0xFF )                // white
#define PROGRESSMONITOR_LINECOLOR_SHADOW                TRGB_COLORDATA( 0x00, 0x00, 0x00, 0x00 )                // black
#define PROGRESSMONITOR_DEFAULT_WIDTH                   350
#define PROGRESSMONITOR_DEFAULT_HEIGHT                  100

//____________________________________________________________________________________________________________
//  structs, types
//____________________________________________________________________________________________________________

/// Item of TextList
struct IMPL_TextlistItem
{
    ::rtl::OUString sTopic  ;   /// Left site of textline in dialog
    ::rtl::OUString sText   ;   /// Right site of textline in dialog
};

//____________________________________________________________________________________________________________
//  class declaration
//____________________________________________________________________________________________________________

class ProgressMonitor   : public ::com::sun::star::awt::XLayoutConstrains
                        , public ::com::sun::star::awt::XButton
                        , public ::com::sun::star::awt::XProgressMonitor
                        , public BaseContainerControl
{

//____________________________________________________________________________________________________________
//  public methods
//____________________________________________________________________________________________________________

public:

    //________________________________________________________________________________________________________
    //  construct/destruct
    //________________________________________________________________________________________________________

    /**_______________________________________________________________________________________________________
        @short
        @descr

        @seealso

        @param

        @return

        @onerror
    */

    ProgressMonitor( const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xFactory );

    /**_______________________________________________________________________________________________________
        @short
        @descr

        @seealso

        @param

        @return

        @onerror
    */

    virtual ~ProgressMonitor();

    //________________________________________________________________________________________________________
    //  XInterface
    //________________________________________________________________________________________________________

    /**_______________________________________________________________________________________________________
        @short      give answer, if interface is supported
        @descr      The interfaces are searched by type.

        @seealso    XInterface

        @param      "rType" is the type of searched interface.

        @return     Any     information about found interface

        @onerror    A RuntimeException is thrown.
    */

    virtual ::com::sun::star::uno::Any SAL_CALL queryInterface( const ::com::sun::star::uno::Type& aType )
        throw( ::com::sun::star::uno::RuntimeException );

    /**_______________________________________________________________________________________________________
        @short      increment refcount
        @descr      -

        @seealso    XInterface
        @seealso    release()

        @param      -

        @return     -

        @onerror    A RuntimeException is thrown.
    */

    virtual void SAL_CALL acquire() throw();

    /**_______________________________________________________________________________________________________
        @short      decrement refcount
        @descr      -

        @seealso    XInterface
        @seealso    acquire()

        @param      -

        @return     -

        @onerror    A RuntimeException is thrown.
    */

    virtual void SAL_CALL release() throw();

    //________________________________________________________________________________________________________
    //  XTypeProvider
    //________________________________________________________________________________________________________

    /**_______________________________________________________________________________________________________
        @short      get information about supported interfaces
        @descr      -

        @seealso    XTypeProvider

        @param      -

        @return     Sequence of types of all supported interfaces

        @onerror    A RuntimeException is thrown.
    */

    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type > SAL_CALL getTypes() throw( ::com::sun::star::uno::RuntimeException );

    //________________________________________________________________________________________________________
    //  XAggregation
    //________________________________________________________________________________________________________

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    virtual ::com::sun::star::uno::Any SAL_CALL queryAggregation( const ::com::sun::star::uno::Type& aType )
        throw( ::com::sun::star::uno::RuntimeException );

    //________________________________________________________________________________________________________
    //  XProgressMonitor
    //________________________________________________________________________________________________________

    /**_______________________________________________________________________________________________________
        @short      add topic to dialog
        @descr      Add a topic with a text in right textlist (used for FixedText-member).<BR>
                    ( "beforeProgress" fix the right list ). The dialog metric is recalculated.

        @seealso    removeText(), updateText()

        @param      sTopic              Name of topic<BR>
                                         [sTopic != "" && sTopic != NULL]
        @param      sText               Value of topic<BR>
                                         [sText != "" && sText != NULL]
        @param      bbeforeProgress     Position of topic<BR>
                                         [True => before progressbar / False => below progressbar]

        @return     -

        @onerror    DEBUG   = Assertion<BR>
                    RELEASE = nothing
    */

    virtual void SAL_CALL addText(
        const ::rtl::OUString& sTopic ,
        const ::rtl::OUString& sText ,
        sal_Bool bbeforeProgress
    ) throw( ::com::sun::star::uno::RuntimeException );

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    virtual void SAL_CALL removeText(
        const ::rtl::OUString& sTopic ,
        sal_Bool bbeforeProgress
    ) throw( ::com::sun::star::uno::RuntimeException );

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    virtual void SAL_CALL updateText(
        const ::rtl::OUString&  sTopic ,
        const ::rtl::OUString&  sText ,
        sal_Bool bbeforeProgress
    ) throw( ::com::sun::star::uno::RuntimeException );

    //________________________________________________________________________________________________________
    //  XProgressBar
    //________________________________________________________________________________________________________

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    virtual void SAL_CALL setForegroundColor( sal_Int32 nColor ) throw( ::com::sun::star::uno::RuntimeException );

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    virtual void SAL_CALL setBackgroundColor( sal_Int32 nColor ) throw( ::com::sun::star::uno::RuntimeException );

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    virtual void SAL_CALL setValue( sal_Int32 nValue ) throw( ::com::sun::star::uno::RuntimeException );

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    virtual void SAL_CALL setRange( sal_Int32   nMin    ,
                                    sal_Int32   nMax    ) throw( ::com::sun::star::uno::RuntimeException );

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    virtual sal_Int32 SAL_CALL getValue() throw( ::com::sun::star::uno::RuntimeException );

    //________________________________________________________________________________________________________
    //  XButton
    //________________________________________________________________________________________________________

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    virtual void SAL_CALL addActionListener(
        const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XActionListener >& xListener
    ) throw( ::com::sun::star::uno::RuntimeException );

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    virtual void SAL_CALL removeActionListener(
        const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XActionListener >& xListener
    ) throw( ::com::sun::star::uno::RuntimeException );

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    virtual void SAL_CALL setLabel( const ::rtl::OUString& sLabel ) throw( ::com::sun::star::uno::RuntimeException );

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    virtual void SAL_CALL setActionCommand( const ::rtl::OUString& sCommand )
        throw( ::com::sun::star::uno::RuntimeException );

    //________________________________________________________________________________________________________
    //  XLayoutConstrains
    //________________________________________________________________________________________________________

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    virtual ::com::sun::star::awt::Size SAL_CALL getMinimumSize() throw( ::com::sun::star::uno::RuntimeException );

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    virtual ::com::sun::star::awt::Size SAL_CALL getPreferredSize() throw( ::com::sun::star::uno::RuntimeException );

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    virtual ::com::sun::star::awt::Size SAL_CALL calcAdjustedSize( const ::com::sun::star::awt::Size& aNewSize )
        throw( ::com::sun::star::uno::RuntimeException );

    //________________________________________________________________________________________________________
    //  XControl
    //________________________________________________________________________________________________________

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    virtual void SAL_CALL createPeer(
        const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XToolkit     >& xToolkit ,
        const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindowPeer  >& xParent
    ) throw( ::com::sun::star::uno::RuntimeException );

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    virtual sal_Bool SAL_CALL setModel( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel >& xModel )
        throw( ::com::sun::star::uno::RuntimeException );

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    virtual ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel > SAL_CALL getModel()
        throw( ::com::sun::star::uno::RuntimeException );

    //________________________________________________________________________________________________________
    //  XComponent
    //________________________________________________________________________________________________________

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    virtual void SAL_CALL dispose() throw( ::com::sun::star::uno::RuntimeException );

    //________________________________________________________________________________________________________
    //  XWindow
    //________________________________________________________________________________________________________

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    virtual void SAL_CALL setPosSize(   sal_Int32   nX      ,
                                        sal_Int32   nY      ,
                                        sal_Int32   nWidth  ,
                                        sal_Int32   nHeight ,
                                        sal_Int16   nFlags  ) throw( ::com::sun::star::uno::RuntimeException );

    //________________________________________________________________________________________________________
    //  BaseControl
    //________________________________________________________________________________________________________

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    static const ::com::sun::star::uno::Sequence< ::rtl::OUString > impl_getStaticSupportedServiceNames();

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    static const ::rtl::OUString impl_getStaticImplementationName();

//____________________________________________________________________________________________________________
//  protected methods
//____________________________________________________________________________________________________________

protected:

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    virtual void impl_paint( sal_Int32 nX ,
                             sal_Int32 nY ,
                             const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XGraphics >& xGraphics );

//____________________________________________________________________________________________________________
// private methods
//____________________________________________________________________________________________________________

private:
    using BaseControl::impl_recalcLayout;
    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    void impl_recalcLayout();

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    void impl_rebuildFixedText();

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    void impl_cleanMemory();

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    IMPL_TextlistItem* impl_searchTopic( const ::rtl::OUString& sTopic , sal_Bool bbeforeProgress );

//____________________________________________________________________________________________________________
// debug methods
//____________________________________________________________________________________________________________

private:

    /**_______________________________________________________________________________________________________
        @short      -
        @descr      -

        @seealso    -

        @param      -

        @return     -

        @onerror    -
    */

    #ifdef DBG_UTIL

    sal_Bool impl_debug_checkParameter( const ::rtl::OUString& sTopic, const ::rtl::OUString& sText, sal_Bool bbeforeProgress );    // addText, updateText
    sal_Bool impl_debug_checkParameter( const ::rtl::OUString& rTopic, sal_Bool bbeforeProgress );                              // removeText

    #endif
//____________________________________________________________________________________________________________
// private variables
//____________________________________________________________________________________________________________

private:
    ::std::vector < IMPL_TextlistItem* >        maTextlist_Top;         // Elements before progress
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFixedText >   m_xTopic_Top        ;   // (used, if parameter "beforeProgress"=sal_True in "addText, updateText, removeText")
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFixedText >   m_xText_Top         ;

    ::std::vector < IMPL_TextlistItem* >        maTextlist_Bottom;      // Elements below of progress
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFixedText >   m_xTopic_Bottom     ;   // (used, if parameter "beforeProgress"=sal_False in "addText, updateText, removeText")
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XFixedText >   m_xText_Bottom      ;

    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XProgressBar > m_xProgressBar      ;
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XButton >      m_xButton           ;
    ::com::sun::star::awt::Rectangle                          m_a3DLine           ;

};  // class ProgressMonitor

}   // namespace unocontrols

#endif  // #ifndef _UNOCONTROLS_PROGRESSMONITOR_CTRL_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
