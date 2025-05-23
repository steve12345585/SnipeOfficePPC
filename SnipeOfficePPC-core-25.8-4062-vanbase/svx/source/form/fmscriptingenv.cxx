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

#include "fmscriptingenv.hxx"
#include "svx/fmmodel.hxx"

#include <com/sun/star/lang/IllegalArgumentException.hpp>
#include <com/sun/star/script/XScriptListener.hpp>
#include <com/sun/star/container/XHierarchicalNameAccess.hpp>
#include <com/sun/star/reflection/XInterfaceMethodTypeDescription.hpp>
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/lang/EventObject.hpp>
#include <com/sun/star/awt/XControl.hpp>

#include <tools/diagnose_ex.h>
#include <cppuhelper/implbase1.hxx>
#include <comphelper/implementationreference.hxx>
#include <comphelper/componentcontext.hxx>
#include <comphelper/processfactory.hxx>
#include <vcl/svapp.hxx>
#include <osl/mutex.hxx>
#include <sfx2/objsh.hxx>
#include <sfx2/app.hxx>
#include <basic/basmgr.hxx>

#include <boost/shared_ptr.hpp>
#include <boost/assign/list_of.hpp>
#include <utility>
using std::pair;
#include <set>

//........................................................................
namespace svxform
{
//........................................................................

    /** === begin UNO using === **/
    using ::com::sun::star::uno::Reference;
    using ::com::sun::star::script::XEventAttacherManager;
    using ::com::sun::star::lang::IllegalArgumentException;
    using ::com::sun::star::script::XScriptListener;
    using ::com::sun::star::script::ScriptEvent;
    using ::com::sun::star::uno::RuntimeException;
    using ::com::sun::star::lang::EventObject;
    using ::com::sun::star::reflection::InvocationTargetException;
    using ::com::sun::star::uno::Any;
    using ::com::sun::star::container::XHierarchicalNameAccess;
    using ::com::sun::star::reflection::XInterfaceMethodTypeDescription;
    using ::com::sun::star::uno::UNO_QUERY_THROW;
    using ::com::sun::star::lang::DisposedException;
    using ::com::sun::star::uno::RuntimeException;
    using ::com::sun::star::uno::Exception;
    using ::com::sun::star::uno::Sequence;
    using ::com::sun::star::uno::XInterface;
    using ::com::sun::star::lang::EventObject;
    using ::com::sun::star::awt::XControl;
    using ::com::sun::star::beans::XPropertySet;
    /** === end UNO using === **/

    class FormScriptingEnvironment;

    //====================================================================
    //= FormScriptListener
    //====================================================================
    typedef ::cppu::WeakImplHelper1 <   XScriptListener
                                    >   FormScriptListener_Base;

    /** implements the XScriptListener interface, is used by FormScriptingEnvironment
    */
    class FormScriptListener    :public FormScriptListener_Base
    {
    private:
        ::osl::Mutex m_aMutex;
        FormScriptingEnvironment *m_pScriptExecutor;

    public:
        FormScriptListener( FormScriptingEnvironment * pScriptExecutor );

        // XScriptListener
        virtual void SAL_CALL firing( const ScriptEvent& aEvent ) throw (RuntimeException);
        virtual Any SAL_CALL approveFiring( const ScriptEvent& aEvent ) throw (InvocationTargetException, RuntimeException);
        // XEventListener
        virtual void SAL_CALL disposing( const EventObject& Source ) throw (RuntimeException);

        // lifetime control
        void SAL_CALL dispose();

    protected:
        ~FormScriptListener();

    private:
        /** determines whether calling a given method at a given listener interface can be done asynchronously

            @param _rListenerType
                the name of the UNO type whose method is to be checked
            @param _rMethodName
                the name of the method at the interface determined by _rListenerType

            @return
                <TRUE/> if and only if the method is declared <code>oneway</code>, i.e. can be called asynchronously
        */
        bool    impl_allowAsynchronousCall_nothrow( const ::rtl::OUString& _rListenerType, const ::rtl::OUString& _rMethodName ) const;

        /** determines whether the instance is already disposed
        */
        bool    impl_isDisposed_nothrow() const { return !m_pScriptExecutor; }

        /** fires the given script event in a thread-safe manner

            This methods calls our script executor's doFireScriptEvent, with previously releasing the given mutex guard,
            but ensuring that our script executor is not deleted between this release and the actual call.

            @param _rGuard
                a clearable guard to our mutex. Must be the only active guard to our mutex.
            @param _rEvent
                the event to fire
            @param _pSyncronousResult
                a place to take a possible result of the script call.

            @precond
                m_pScriptExecutor is not <NULL/>.
        */
        void    impl_doFireScriptEvent_nothrow( ::osl::ClearableMutexGuard& _rGuard, const ScriptEvent& _rEvent, Any* _pSyncronousResult );

    private:
        DECL_LINK( OnAsyncScriptEvent, ScriptEvent* );
    };

    //====================================================================
    //= FormScriptingEnvironment
    //====================================================================
    class FormScriptingEnvironment : public IFormScriptingEnvironment
    {
    private:
        typedef ::comphelper::ImplementationReference< FormScriptListener, XScriptListener >    ListenerImplementation;

    private:
        ::osl::Mutex            m_aMutex;
        oslInterlockedCount     m_refCount;
        ListenerImplementation  m_pScriptListener;
        FmFormModel&            m_rFormModel;
        bool                    m_bDisposed;

    public:
        FormScriptingEnvironment( FmFormModel& _rModel );
        virtual ~FormScriptingEnvironment();

        // callback for FormScriptListener
        void doFireScriptEvent( const ScriptEvent& _rEvent, Any* _pSyncronousResult );

        // IFormScriptingEnvironment
        virtual void registerEventAttacherManager( const Reference< XEventAttacherManager >& _rxManager );
        virtual void revokeEventAttacherManager( const Reference< XEventAttacherManager >& _rxManager );
        virtual void dispose();

        // IReference
        virtual oslInterlockedCount SAL_CALL acquire();
        virtual oslInterlockedCount SAL_CALL release();

    private:
        void impl_registerOrRevoke_throw( const Reference< XEventAttacherManager >& _rxManager, bool _bRegister );

    private:
        FormScriptingEnvironment();                                                 // never implemented
        FormScriptingEnvironment( const FormScriptingEnvironment& );                // never implemented
        FormScriptingEnvironment& operator=( const FormScriptingEnvironment& );     // never implemented
    };

    //====================================================================
    //= FormScriptListener
    //====================================================================
    //--------------------------------------------------------------------
    FormScriptListener::FormScriptListener( FormScriptingEnvironment* pScriptExecutor )
        :m_pScriptExecutor( pScriptExecutor )
    {
    }

    //--------------------------------------------------------------------
    FormScriptListener::~FormScriptListener()
    {
    }

    //--------------------------------------------------------------------
    bool FormScriptListener::impl_allowAsynchronousCall_nothrow( const ::rtl::OUString& _rListenerType, const ::rtl::OUString& _rMethodName ) const
    {
        // This used to be implemented as:
        // is (_rListenerType + "::" + _rMethodName) a oneway function?
        // since we got rid of the notion of oneway, this is the list
        // of oneway methods, autogenerated by postprocessing of
        // commitdiff 90eac3e69749a9227c4b6902b1f3cef1e338c6d1
        static std::set< pair< OUString, OUString > > delayed_event_listeners =
            boost::assign::list_of
            (pair<OUString,OUString>("com.sun.star.accessibility.XAccessibleComponent","grabFocus"))
            (pair<OUString,OUString>("com.sun.star.accessibility.XAccessibleEventBroadcaster","addAccessibleEventListener"))
            (pair<OUString,OUString>("com.sun.star.accessibility.XAccessibleEventBroadcaster","removeAccessibleEventListener"))
            (pair<OUString,OUString>("com.sun.star.accessibility.XAccessibleSelection","clearAccessibleSelection"))
            (pair<OUString,OUString>("com.sun.star.accessibility.XAccessibleSelection","selectAllAccessibleChildren"))
            (pair<OUString,OUString>("com.sun.star.awt.XActionListener","actionPerformed"))
            (pair<OUString,OUString>("com.sun.star.awt.XActivateListener","windowActivated"))
            (pair<OUString,OUString>("com.sun.star.awt.XActivateListener","windowDeactivated"))
            (pair<OUString,OUString>("com.sun.star.awt.XAdjustmentListener","adjustmentValueChanged"))
            (pair<OUString,OUString>("com.sun.star.awt.XButton","addActionListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XButton","removeActionListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XButton","setLabel"))
            (pair<OUString,OUString>("com.sun.star.awt.XButton","setActionCommand"))
            (pair<OUString,OUString>("com.sun.star.awt.XCheckBox","addItemListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XCheckBox","removeItemListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XCheckBox","setState"))
            (pair<OUString,OUString>("com.sun.star.awt.XCheckBox","setLabel"))
            (pair<OUString,OUString>("com.sun.star.awt.XCheckBox","enableTriState"))
            (pair<OUString,OUString>("com.sun.star.awt.XComboBox","addItemListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XComboBox","removeItemListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XComboBox","addActionListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XComboBox","removeActionListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XComboBox","addItem"))
            (pair<OUString,OUString>("com.sun.star.awt.XComboBox","addItems"))
            (pair<OUString,OUString>("com.sun.star.awt.XComboBox","removeItems"))
            (pair<OUString,OUString>("com.sun.star.awt.XComboBox","setDropDownLineCount"))
            (pair<OUString,OUString>("com.sun.star.awt.XControl","setContext"))
            (pair<OUString,OUString>("com.sun.star.awt.XControl","createPeer"))
            (pair<OUString,OUString>("com.sun.star.awt.XControl","setDesignMode"))
            (pair<OUString,OUString>("com.sun.star.awt.XControlContainer","setStatusText"))
            (pair<OUString,OUString>("com.sun.star.awt.XControlContainer","addControl"))
            (pair<OUString,OUString>("com.sun.star.awt.XControlContainer","removeControl"))
            (pair<OUString,OUString>("com.sun.star.awt.XCurrencyField","setValue"))
            (pair<OUString,OUString>("com.sun.star.awt.XCurrencyField","setMin"))
            (pair<OUString,OUString>("com.sun.star.awt.XCurrencyField","setMax"))
            (pair<OUString,OUString>("com.sun.star.awt.XCurrencyField","setFirst"))
            (pair<OUString,OUString>("com.sun.star.awt.XCurrencyField","setLast"))
            (pair<OUString,OUString>("com.sun.star.awt.XCurrencyField","setSpinSize"))
            (pair<OUString,OUString>("com.sun.star.awt.XCurrencyField","setDecimalDigits"))
            (pair<OUString,OUString>("com.sun.star.awt.XCurrencyField","setStrictFormat"))
            (pair<OUString,OUString>("com.sun.star.awt.XDateField","setDate"))
            (pair<OUString,OUString>("com.sun.star.awt.XDateField","setMin"))
            (pair<OUString,OUString>("com.sun.star.awt.XDateField","setMax"))
            (pair<OUString,OUString>("com.sun.star.awt.XDateField","setFirst"))
            (pair<OUString,OUString>("com.sun.star.awt.XDateField","setLast"))
            (pair<OUString,OUString>("com.sun.star.awt.XDateField","setLongFormat"))
            (pair<OUString,OUString>("com.sun.star.awt.XDateField","setStrictFormat"))
            (pair<OUString,OUString>("com.sun.star.awt.XDialog","setTitle"))
            (pair<OUString,OUString>("com.sun.star.awt.XDisplayConnection","addEventHandler"))
            (pair<OUString,OUString>("com.sun.star.awt.XDisplayConnection","removeEventHandler"))
            (pair<OUString,OUString>("com.sun.star.awt.XDisplayConnection","addErrorHandler"))
            (pair<OUString,OUString>("com.sun.star.awt.XDisplayConnection","removeErrorHandler"))
            (pair<OUString,OUString>("com.sun.star.awt.XExtendedToolkit","addTopWindowListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XExtendedToolkit","removeTopWindowListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XExtendedToolkit","addKeyHandler"))
            (pair<OUString,OUString>("com.sun.star.awt.XExtendedToolkit","removeKeyHandler"))
            (pair<OUString,OUString>("com.sun.star.awt.XExtendedToolkit","addFocusListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XExtendedToolkit","removeFocusListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XExtendedToolkit","fireFocusGained"))
            (pair<OUString,OUString>("com.sun.star.awt.XExtendedToolkit","fireFocusLost"))
            (pair<OUString,OUString>("com.sun.star.awt.XFileDialog","setPath"))
            (pair<OUString,OUString>("com.sun.star.awt.XFileDialog","setFilters"))
            (pair<OUString,OUString>("com.sun.star.awt.XFileDialog","setCurrentFilter"))
            (pair<OUString,OUString>("com.sun.star.awt.XFixedHyperlink","setText"))
            (pair<OUString,OUString>("com.sun.star.awt.XFixedHyperlink","setURL"))
            (pair<OUString,OUString>("com.sun.star.awt.XFixedHyperlink","setAlignment"))
            (pair<OUString,OUString>("com.sun.star.awt.XFixedHyperlink","addActionListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XFixedHyperlink","removeActionListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XFixedText","setText"))
            (pair<OUString,OUString>("com.sun.star.awt.XFixedText","setAlignment"))
            (pair<OUString,OUString>("com.sun.star.awt.XFocusListener","focusGained"))
            (pair<OUString,OUString>("com.sun.star.awt.XFocusListener","focusLost"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","setFont"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","selectFont"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","setTextColor"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","setTextFillColor"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","setLineColor"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","setFillColor"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","setRasterOp"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","setClipRegion"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","intersectClipRegion"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","push"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","pop"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","copy"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","draw"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","drawPixel"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","drawLine"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","drawRect"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","drawRoundedRect"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","drawPolyLine"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","drawPolygon"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","drawPolyPolygon"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","drawEllipse"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","drawArc"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","drawPie"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","drawChord"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","drawGradient"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","drawText"))
            (pair<OUString,OUString>("com.sun.star.awt.XGraphics","drawTextArray"))
            (pair<OUString,OUString>("com.sun.star.awt.XImageButton","addActionListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XImageButton","removeActionListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XImageButton","setActionCommand"))
            (pair<OUString,OUString>("com.sun.star.awt.XImageConsumer","init"))
            (pair<OUString,OUString>("com.sun.star.awt.XImageConsumer","setColorModel"))
            (pair<OUString,OUString>("com.sun.star.awt.XImageConsumer","setPixelsByBytes"))
            (pair<OUString,OUString>("com.sun.star.awt.XImageConsumer","setPixelsByLongs"))
            (pair<OUString,OUString>("com.sun.star.awt.XImageConsumer","complete"))
            (pair<OUString,OUString>("com.sun.star.awt.XImageProducer","addConsumer"))
            (pair<OUString,OUString>("com.sun.star.awt.XImageProducer","removeConsumer"))
            (pair<OUString,OUString>("com.sun.star.awt.XImageProducer","startProduction"))
            (pair<OUString,OUString>("com.sun.star.awt.XItemEventBroadcaster","addItemListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XItemEventBroadcaster","removeItemListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XItemListener","itemStateChanged"))
            (pair<OUString,OUString>("com.sun.star.awt.XKeyListener","keyPressed"))
            (pair<OUString,OUString>("com.sun.star.awt.XKeyListener","keyReleased"))
            (pair<OUString,OUString>("com.sun.star.awt.XListBox","addItemListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XListBox","removeItemListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XListBox","addActionListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XListBox","removeActionListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XListBox","addItem"))
            (pair<OUString,OUString>("com.sun.star.awt.XListBox","addItems"))
            (pair<OUString,OUString>("com.sun.star.awt.XListBox","removeItems"))
            (pair<OUString,OUString>("com.sun.star.awt.XListBox","selectItemPos"))
            (pair<OUString,OUString>("com.sun.star.awt.XListBox","selectItemsPos"))
            (pair<OUString,OUString>("com.sun.star.awt.XListBox","selectItem"))
            (pair<OUString,OUString>("com.sun.star.awt.XListBox","setMultipleMode"))
            (pair<OUString,OUString>("com.sun.star.awt.XListBox","setDropDownLineCount"))
            (pair<OUString,OUString>("com.sun.star.awt.XListBox","makeVisible"))
            (pair<OUString,OUString>("com.sun.star.awt.XMenu","addMenuListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XMenu","removeMenuListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XMenu","insertItem"))
            (pair<OUString,OUString>("com.sun.star.awt.XMenu","removeItem"))
            (pair<OUString,OUString>("com.sun.star.awt.XMenu","enableItem"))
            (pair<OUString,OUString>("com.sun.star.awt.XMenu","setItemText"))
            (pair<OUString,OUString>("com.sun.star.awt.XMenu","setPopupMenu"))
            (pair<OUString,OUString>("com.sun.star.awt.XMenuListener","highlight"))
            (pair<OUString,OUString>("com.sun.star.awt.XMenuListener","select"))
            (pair<OUString,OUString>("com.sun.star.awt.XMenuListener","activate"))
            (pair<OUString,OUString>("com.sun.star.awt.XMenuListener","deactivate"))
            (pair<OUString,OUString>("com.sun.star.awt.XMessageBox","setCaptionText"))
            (pair<OUString,OUString>("com.sun.star.awt.XMessageBox","setMessageText"))
            (pair<OUString,OUString>("com.sun.star.awt.XMouseListener","mousePressed"))
            (pair<OUString,OUString>("com.sun.star.awt.XMouseListener","mouseReleased"))
            (pair<OUString,OUString>("com.sun.star.awt.XMouseListener","mouseEntered"))
            (pair<OUString,OUString>("com.sun.star.awt.XMouseListener","mouseExited"))
            (pair<OUString,OUString>("com.sun.star.awt.XNumericField","setValue"))
            (pair<OUString,OUString>("com.sun.star.awt.XNumericField","setMin"))
            (pair<OUString,OUString>("com.sun.star.awt.XNumericField","setMax"))
            (pair<OUString,OUString>("com.sun.star.awt.XNumericField","setFirst"))
            (pair<OUString,OUString>("com.sun.star.awt.XNumericField","setLast"))
            (pair<OUString,OUString>("com.sun.star.awt.XNumericField","setSpinSize"))
            (pair<OUString,OUString>("com.sun.star.awt.XNumericField","setDecimalDigits"))
            (pair<OUString,OUString>("com.sun.star.awt.XNumericField","setStrictFormat"))
            (pair<OUString,OUString>("com.sun.star.awt.XPaintListener","windowPaint"))
            (pair<OUString,OUString>("com.sun.star.awt.XPatternField","setMasks"))
            (pair<OUString,OUString>("com.sun.star.awt.XPatternField","setString"))
            (pair<OUString,OUString>("com.sun.star.awt.XPatternField","setStrictFormat"))
            (pair<OUString,OUString>("com.sun.star.awt.XPointer","setType"))
            (pair<OUString,OUString>("com.sun.star.awt.XPopupMenu","insertSeparator"))
            (pair<OUString,OUString>("com.sun.star.awt.XPopupMenu","setDefaultItem"))
            (pair<OUString,OUString>("com.sun.star.awt.XPopupMenu","checkItem"))
            (pair<OUString,OUString>("com.sun.star.awt.XProgressBar","setForegroundColor"))
            (pair<OUString,OUString>("com.sun.star.awt.XProgressBar","setBackgroundColor"))
            (pair<OUString,OUString>("com.sun.star.awt.XProgressBar","setRange"))
            (pair<OUString,OUString>("com.sun.star.awt.XProgressBar","setValue"))
            (pair<OUString,OUString>("com.sun.star.awt.XProgressMonitor","addText"))
            (pair<OUString,OUString>("com.sun.star.awt.XProgressMonitor","removeText"))
            (pair<OUString,OUString>("com.sun.star.awt.XProgressMonitor","updateText"))
            (pair<OUString,OUString>("com.sun.star.awt.XRadioButton","addItemListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XRadioButton","removeItemListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XRadioButton","setState"))
            (pair<OUString,OUString>("com.sun.star.awt.XRadioButton","setLabel"))
            (pair<OUString,OUString>("com.sun.star.awt.XRegion","clear"))
            (pair<OUString,OUString>("com.sun.star.awt.XRegion","move"))
            (pair<OUString,OUString>("com.sun.star.awt.XRegion","unionRectangle"))
            (pair<OUString,OUString>("com.sun.star.awt.XRegion","intersectRectangle"))
            (pair<OUString,OUString>("com.sun.star.awt.XRegion","excludeRectangle"))
            (pair<OUString,OUString>("com.sun.star.awt.XRegion","xOrRectangle"))
            (pair<OUString,OUString>("com.sun.star.awt.XRegion","unionRegion"))
            (pair<OUString,OUString>("com.sun.star.awt.XRegion","intersectRegion"))
            (pair<OUString,OUString>("com.sun.star.awt.XRegion","excludeRegion"))
            (pair<OUString,OUString>("com.sun.star.awt.XRegion","xOrRegion"))
            (pair<OUString,OUString>("com.sun.star.awt.XScrollBar","addAdjustmentListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XScrollBar","removeAdjustmentListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XScrollBar","setValue"))
            (pair<OUString,OUString>("com.sun.star.awt.XScrollBar","setValues"))
            (pair<OUString,OUString>("com.sun.star.awt.XScrollBar","setMaximum"))
            (pair<OUString,OUString>("com.sun.star.awt.XScrollBar","setLineIncrement"))
            (pair<OUString,OUString>("com.sun.star.awt.XScrollBar","setBlockIncrement"))
            (pair<OUString,OUString>("com.sun.star.awt.XScrollBar","setVisibleSize"))
            (pair<OUString,OUString>("com.sun.star.awt.XScrollBar","setOrientation"))
            (pair<OUString,OUString>("com.sun.star.awt.XSpinField","addSpinListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XSpinField","removeSpinListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XSpinField","up"))
            (pair<OUString,OUString>("com.sun.star.awt.XSpinField","down"))
            (pair<OUString,OUString>("com.sun.star.awt.XSpinField","first"))
            (pair<OUString,OUString>("com.sun.star.awt.XSpinField","last"))
            (pair<OUString,OUString>("com.sun.star.awt.XSpinField","enableRepeat"))
            (pair<OUString,OUString>("com.sun.star.awt.XSpinListener","up"))
            (pair<OUString,OUString>("com.sun.star.awt.XSpinListener","down"))
            (pair<OUString,OUString>("com.sun.star.awt.XSpinListener","first"))
            (pair<OUString,OUString>("com.sun.star.awt.XSpinListener","last"))
            (pair<OUString,OUString>("com.sun.star.awt.XSpinValue","addAdjustmentListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XSpinValue","removeAdjustmentListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XSpinValue","setValue"))
            (pair<OUString,OUString>("com.sun.star.awt.XSpinValue","setValues"))
            (pair<OUString,OUString>("com.sun.star.awt.XSpinValue","setMinimum"))
            (pair<OUString,OUString>("com.sun.star.awt.XSpinValue","setMaximum"))
            (pair<OUString,OUString>("com.sun.star.awt.XSpinValue","setSpinIncrement"))
            (pair<OUString,OUString>("com.sun.star.awt.XTabController","setModel"))
            (pair<OUString,OUString>("com.sun.star.awt.XTabController","setContainer"))
            (pair<OUString,OUString>("com.sun.star.awt.XTabController","autoTabOrder"))
            (pair<OUString,OUString>("com.sun.star.awt.XTabController","activateTabOrder"))
            (pair<OUString,OUString>("com.sun.star.awt.XTabController","activateFirst"))
            (pair<OUString,OUString>("com.sun.star.awt.XTabController","activateLast"))
            (pair<OUString,OUString>("com.sun.star.awt.XTabControllerModel","setGroupControl"))
            (pair<OUString,OUString>("com.sun.star.awt.XTabControllerModel","setControlModels"))
            (pair<OUString,OUString>("com.sun.star.awt.XTabControllerModel","setGroup"))
            (pair<OUString,OUString>("com.sun.star.awt.XTextComponent","addTextListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XTextComponent","removeTextListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XTextComponent","setText"))
            (pair<OUString,OUString>("com.sun.star.awt.XTextComponent","insertText"))
            (pair<OUString,OUString>("com.sun.star.awt.XTextComponent","setSelection"))
            (pair<OUString,OUString>("com.sun.star.awt.XTextComponent","setEditable"))
            (pair<OUString,OUString>("com.sun.star.awt.XTextComponent","setMaxTextLen"))
            (pair<OUString,OUString>("com.sun.star.awt.XTextEditField","setEchoChar"))
            (pair<OUString,OUString>("com.sun.star.awt.XTextListener","textChanged"))
            (pair<OUString,OUString>("com.sun.star.awt.XTimeField","setTime"))
            (pair<OUString,OUString>("com.sun.star.awt.XTimeField","setMin"))
            (pair<OUString,OUString>("com.sun.star.awt.XTimeField","setMax"))
            (pair<OUString,OUString>("com.sun.star.awt.XTimeField","setFirst"))
            (pair<OUString,OUString>("com.sun.star.awt.XTimeField","setLast"))
            (pair<OUString,OUString>("com.sun.star.awt.XTimeField","setStrictFormat"))
            (pair<OUString,OUString>("com.sun.star.awt.XTopWindow","addTopWindowListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XTopWindow","removeTopWindowListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XTopWindow","toFront"))
            (pair<OUString,OUString>("com.sun.star.awt.XTopWindow","toBack"))
            (pair<OUString,OUString>("com.sun.star.awt.XTopWindow","setMenuBar"))
            (pair<OUString,OUString>("com.sun.star.awt.XTopWindowListener","windowOpened"))
            (pair<OUString,OUString>("com.sun.star.awt.XTopWindowListener","windowClosing"))
            (pair<OUString,OUString>("com.sun.star.awt.XTopWindowListener","windowClosed"))
            (pair<OUString,OUString>("com.sun.star.awt.XTopWindowListener","windowMinimized"))
            (pair<OUString,OUString>("com.sun.star.awt.XTopWindowListener","windowNormalized"))
            (pair<OUString,OUString>("com.sun.star.awt.XTopWindowListener","windowActivated"))
            (pair<OUString,OUString>("com.sun.star.awt.XTopWindowListener","windowDeactivated"))
            (pair<OUString,OUString>("com.sun.star.awt.XUnoControlContainer","setTabControllers"))
            (pair<OUString,OUString>("com.sun.star.awt.XUnoControlContainer","addTabController"))
            (pair<OUString,OUString>("com.sun.star.awt.XUnoControlContainer","removeTabController"))
            (pair<OUString,OUString>("com.sun.star.awt.XUserInputInterception","addKeyHandler"))
            (pair<OUString,OUString>("com.sun.star.awt.XUserInputInterception","removeKeyHandler"))
            (pair<OUString,OUString>("com.sun.star.awt.XUserInputInterception","addMouseClickHandler"))
            (pair<OUString,OUString>("com.sun.star.awt.XUserInputInterception","removeMouseClickHandler"))
            (pair<OUString,OUString>("com.sun.star.awt.XVclContainer","addVclContainerListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XVclContainer","removeVclContainerListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XVclContainerListener","windowAdded"))
            (pair<OUString,OUString>("com.sun.star.awt.XVclContainerListener","windowRemoved"))
            (pair<OUString,OUString>("com.sun.star.awt.XVclContainerPeer","enableDialogControl"))
            (pair<OUString,OUString>("com.sun.star.awt.XVclContainerPeer","setTabOrder"))
            (pair<OUString,OUString>("com.sun.star.awt.XVclContainerPeer","setGroup"))
            (pair<OUString,OUString>("com.sun.star.awt.XVclWindowPeer","setDesignMode"))
            (pair<OUString,OUString>("com.sun.star.awt.XVclWindowPeer","enableClipSiblings"))
            (pair<OUString,OUString>("com.sun.star.awt.XVclWindowPeer","setForeground"))
            (pair<OUString,OUString>("com.sun.star.awt.XVclWindowPeer","setControlFont"))
            (pair<OUString,OUString>("com.sun.star.awt.XView","draw"))
            (pair<OUString,OUString>("com.sun.star.awt.XView","setZoom"))
            (pair<OUString,OUString>("com.sun.star.awt.XWindow","setPosSize"))
            (pair<OUString,OUString>("com.sun.star.awt.XWindow","setVisible"))
            (pair<OUString,OUString>("com.sun.star.awt.XWindow","setEnable"))
            (pair<OUString,OUString>("com.sun.star.awt.XWindow","setFocus"))
            (pair<OUString,OUString>("com.sun.star.awt.XWindow","addWindowListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XWindow","removeWindowListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XWindow","addFocusListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XWindow","removeFocusListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XWindow","addKeyListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XWindow","removeKeyListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XWindow","addMouseListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XWindow","removeMouseListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XWindow","addMouseMotionListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XWindow","removeMouseMotionListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XWindow","addPaintListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XWindow","removePaintListener"))
            (pair<OUString,OUString>("com.sun.star.awt.XWindowListener","windowResized"))
            (pair<OUString,OUString>("com.sun.star.awt.XWindowListener","windowMoved"))
            (pair<OUString,OUString>("com.sun.star.awt.XWindowListener","windowShown"))
            (pair<OUString,OUString>("com.sun.star.awt.XWindowListener","windowHidden"))
            (pair<OUString,OUString>("com.sun.star.awt.XWindowListener2","windowEnabled"))
            (pair<OUString,OUString>("com.sun.star.awt.XWindowListener2","windowDisabled"))
            (pair<OUString,OUString>("com.sun.star.awt.XWindowPeer","setPointer"))
            (pair<OUString,OUString>("com.sun.star.awt.XWindowPeer","setBackground"))
            (pair<OUString,OUString>("com.sun.star.awt.XWindowPeer","invalidate"))
            (pair<OUString,OUString>("com.sun.star.awt.XWindowPeer","invalidateRect"))
            (pair<OUString,OUString>("com.sun.star.awt.grid.XGridSelectionListener","selectionChanged"))
            (pair<OUString,OUString>("com.sun.star.awt.tab.XTabPageContainer","addTabPageContainerListener"))
            (pair<OUString,OUString>("com.sun.star.awt.tab.XTabPageContainer","removeTabPageContainerListener"))
            (pair<OUString,OUString>("com.sun.star.awt.tab.XTabPageContainerListener","tabPageActivated"))
            (pair<OUString,OUString>("com.sun.star.configuration.backend.XBackendChangesNotifier","addChangesListener"))
            (pair<OUString,OUString>("com.sun.star.configuration.backend.XBackendChangesNotifier","removeChangesListener"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.clipboard.XClipboard","setContents"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.clipboard.XClipboardListener","changedContents"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.clipboard.XClipboardNotifier","addClipboardListener"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.clipboard.XClipboardNotifier","removeClipboardListener"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.clipboard.XClipboardOwner","lostOwnership"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.dnd.XAutoscroll","autoscroll"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.dnd.XDragGestureListener","dragGestureRecognized"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.dnd.XDragGestureRecognizer","addDragGestureListener"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.dnd.XDragGestureRecognizer","removeDragGestureListener"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.dnd.XDragSource","startDrag"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.dnd.XDragSourceContext","setCursor"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.dnd.XDragSourceContext","setImage"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.dnd.XDragSourceContext","transferablesFlavorsChanged"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.dnd.XDragSourceListener","dragDropEnd"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.dnd.XDragSourceListener","dragEnter"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.dnd.XDragSourceListener","dragExit"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.dnd.XDragSourceListener","dragOver"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.dnd.XDragSourceListener","dropActionChanged"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.dnd.XDropTarget","addDropTargetListener"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.dnd.XDropTarget","removeDropTargetListener"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.dnd.XDropTarget","setDefaultActions"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.dnd.XDropTargetDragContext","acceptDrag"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.dnd.XDropTargetDragContext","rejectDrag"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.dnd.XDropTargetDropContext","acceptDrop"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.dnd.XDropTargetDropContext","rejectDrop"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.dnd.XDropTargetDropContext","dropComplete"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.dnd.XDropTargetListener","dragEnter"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.dnd.XDropTargetListener","dragExit"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.dnd.XDropTargetListener","dragOver"))
            (pair<OUString,OUString>("com.sun.star.datatransfer.dnd.XDropTargetListener","dropActionChanged"))
            (pair<OUString,OUString>("com.sun.star.document.XEventBroadcaster","addEventListener"))
            (pair<OUString,OUString>("com.sun.star.document.XEventBroadcaster","removeEventListener"))
            (pair<OUString,OUString>("com.sun.star.document.XEventListener","notifyEvent"))
            (pair<OUString,OUString>("com.sun.star.document.XStorageChangeListener","notifyStorageChange"))
            (pair<OUString,OUString>("com.sun.star.drawing.XControlShape","setControl"))
            (pair<OUString,OUString>("com.sun.star.form.XApproveActionBroadcaster","addApproveActionListener"))
            (pair<OUString,OUString>("com.sun.star.form.XApproveActionBroadcaster","removeApproveActionListener"))
            (pair<OUString,OUString>("com.sun.star.form.XBoundControl","setLock"))
            (pair<OUString,OUString>("com.sun.star.form.XChangeBroadcaster","addChangeListener"))
            (pair<OUString,OUString>("com.sun.star.form.XChangeBroadcaster","removeChangeListener"))
            (pair<OUString,OUString>("com.sun.star.form.XChangeListener","changed"))
            (pair<OUString,OUString>("com.sun.star.form.XConfirmDeleteBroadcaster","addConfirmDeleteListener"))
            (pair<OUString,OUString>("com.sun.star.form.XConfirmDeleteBroadcaster","removeConfirmDeleteListener"))
            (pair<OUString,OUString>("com.sun.star.form.XDatabaseParameterBroadcaster","addParameterListener"))
            (pair<OUString,OUString>("com.sun.star.form.XDatabaseParameterBroadcaster","removeParameterListener"))
            (pair<OUString,OUString>("com.sun.star.form.XDatabaseParameterBroadcaster2","addDatabaseParameterListener"))
            (pair<OUString,OUString>("com.sun.star.form.XDatabaseParameterBroadcaster2","removeDatabaseParameterListener"))
            (pair<OUString,OUString>("com.sun.star.form.XErrorBroadcaster","addErrorListener"))
            (pair<OUString,OUString>("com.sun.star.form.XErrorBroadcaster","removeErrorListener"))
            (pair<OUString,OUString>("com.sun.star.form.XFormController","addActivateListener"))
            (pair<OUString,OUString>("com.sun.star.form.XFormController","removeActivateListener"))
            (pair<OUString,OUString>("com.sun.star.form.XFormControllerListener","formActivated"))
            (pair<OUString,OUString>("com.sun.star.form.XFormControllerListener","formDeactivated"))
            (pair<OUString,OUString>("com.sun.star.form.XGrid","setCurrentColumnPosition"))
            (pair<OUString,OUString>("com.sun.star.form.XGridPeer","setColumns"))
            (pair<OUString,OUString>("com.sun.star.form.XLoadListener","loaded"))
            (pair<OUString,OUString>("com.sun.star.form.XLoadListener","unloading"))
            (pair<OUString,OUString>("com.sun.star.form.XLoadListener","unloaded"))
            (pair<OUString,OUString>("com.sun.star.form.XLoadListener","reloading"))
            (pair<OUString,OUString>("com.sun.star.form.XLoadListener","reloaded"))
            (pair<OUString,OUString>("com.sun.star.form.XLoadable","load"))
            (pair<OUString,OUString>("com.sun.star.form.XLoadable","unload"))
            (pair<OUString,OUString>("com.sun.star.form.XLoadable","reload"))
            (pair<OUString,OUString>("com.sun.star.form.XLoadable","addLoadListener"))
            (pair<OUString,OUString>("com.sun.star.form.XLoadable","removeLoadListener"))
            (pair<OUString,OUString>("com.sun.star.form.XPositioningListener","positioned"))
            (pair<OUString,OUString>("com.sun.star.form.XReset","reset"))
            (pair<OUString,OUString>("com.sun.star.form.XReset","addResetListener"))
            (pair<OUString,OUString>("com.sun.star.form.XReset","removeResetListener"))
            (pair<OUString,OUString>("com.sun.star.form.XResetListener","resetted"))
            (pair<OUString,OUString>("com.sun.star.form.XSubmit","submit"))
            (pair<OUString,OUString>("com.sun.star.form.XSubmit","addSubmitListener"))
            (pair<OUString,OUString>("com.sun.star.form.XSubmit","removeSubmitListener"))
            (pair<OUString,OUString>("com.sun.star.form.XUpdateBroadcaster","addUpdateListener"))
            (pair<OUString,OUString>("com.sun.star.form.XUpdateBroadcaster","removeUpdateListener"))
            (pair<OUString,OUString>("com.sun.star.frame.XBrowseHistoryRegistry","updateViewData"))
            (pair<OUString,OUString>("com.sun.star.frame.XBrowseHistoryRegistry","createNewEntry"))
            (pair<OUString,OUString>("com.sun.star.frame.XConfigManager","addPropertyChangeListener"))
            (pair<OUString,OUString>("com.sun.star.frame.XConfigManager","removePropertyChangeListener"))
            (pair<OUString,OUString>("com.sun.star.frame.XConfigManager","flush"))
            (pair<OUString,OUString>("com.sun.star.frame.XDesktop","addTerminateListener"))
            (pair<OUString,OUString>("com.sun.star.frame.XDesktop","removeTerminateListener"))
            (pair<OUString,OUString>("com.sun.star.frame.XDispatch","dispatch"))
            (pair<OUString,OUString>("com.sun.star.frame.XDispatch","addStatusListener"))
            (pair<OUString,OUString>("com.sun.star.frame.XDispatch","removeStatusListener"))
            (pair<OUString,OUString>("com.sun.star.frame.XDocumentTemplates","update"))
            (pair<OUString,OUString>("com.sun.star.frame.XFrame","setCreator"))
            (pair<OUString,OUString>("com.sun.star.frame.XFrame","setName"))
            (pair<OUString,OUString>("com.sun.star.frame.XFrame","activate"))
            (pair<OUString,OUString>("com.sun.star.frame.XFrame","deactivate"))
            (pair<OUString,OUString>("com.sun.star.frame.XFrame","addFrameActionListener"))
            (pair<OUString,OUString>("com.sun.star.frame.XFrame","removeFrameActionListener"))
            (pair<OUString,OUString>("com.sun.star.frame.XFrameActionListener","frameAction"))
            (pair<OUString,OUString>("com.sun.star.frame.XFrameLoader","load"))
            (pair<OUString,OUString>("com.sun.star.frame.XFrameLoader","cancel"))
            (pair<OUString,OUString>("com.sun.star.frame.XLoadEventListener","loadFinished"))
            (pair<OUString,OUString>("com.sun.star.frame.XLoadEventListener","loadCancelled"))
            (pair<OUString,OUString>("com.sun.star.frame.XModel","connectController"))
            (pair<OUString,OUString>("com.sun.star.frame.XModel","disconnectController"))
            (pair<OUString,OUString>("com.sun.star.frame.XModel","lockControllers"))
            (pair<OUString,OUString>("com.sun.star.frame.XModel","unlockControllers"))
            (pair<OUString,OUString>("com.sun.star.frame.XNotifyingDispatch","dispatchWithNotification"))
            (pair<OUString,OUString>("com.sun.star.frame.XRecordableDispatch","dispatchAndRecord"))
            (pair<OUString,OUString>("com.sun.star.frame.XSessionManagerClient","addSessionManagerListener"))
            (pair<OUString,OUString>("com.sun.star.frame.XSessionManagerClient","removeSessionManagerListener"))
            (pair<OUString,OUString>("com.sun.star.frame.XSessionManagerClient","queryInteraction"))
            (pair<OUString,OUString>("com.sun.star.frame.XSessionManagerClient","interactionDone"))
            (pair<OUString,OUString>("com.sun.star.frame.XSessionManagerClient","saveDone"))
            (pair<OUString,OUString>("com.sun.star.frame.XSessionManagerListener","doSave"))
            (pair<OUString,OUString>("com.sun.star.frame.XSessionManagerListener","approveInteraction"))
            (pair<OUString,OUString>("com.sun.star.frame.XSessionManagerListener","shutdownCanceled"))
            (pair<OUString,OUString>("com.sun.star.frame.XSessionManagerListener2","doQuit"))
            (pair<OUString,OUString>("com.sun.star.frame.XStatusListener","statusChanged"))
            (pair<OUString,OUString>("com.sun.star.frame.XTask","tileWindows"))
            (pair<OUString,OUString>("com.sun.star.frame.XTask","arrangeWindowsVertical"))
            (pair<OUString,OUString>("com.sun.star.frame.XTask","arrangeWindowsHorizontal"))
            (pair<OUString,OUString>("com.sun.star.frame.XWindowArranger","arrange"))
            (pair<OUString,OUString>("com.sun.star.inspection.XPropertyControlContext","activateNextControl"))
            (pair<OUString,OUString>("com.sun.star.inspection.XPropertyControlObserver","focusGained"))
            (pair<OUString,OUString>("com.sun.star.inspection.XPropertyControlObserver","valueChanged"))
            (pair<OUString,OUString>("com.sun.star.mozilla.XCloseSessionListener","sessionClosed"))
            (pair<OUString,OUString>("com.sun.star.mozilla.XMenuProxy","addMenuProxyListener"))
            (pair<OUString,OUString>("com.sun.star.mozilla.XMenuProxy","removeMenuProxyListener"))
            (pair<OUString,OUString>("com.sun.star.mozilla.XPluginInstance","start"))
            (pair<OUString,OUString>("com.sun.star.mozilla.XPluginInstance","stop"))
            (pair<OUString,OUString>("com.sun.star.mozilla.XPluginInstance","destroy"))
            (pair<OUString,OUString>("com.sun.star.mozilla.XPluginInstance","createWindow"))
            (pair<OUString,OUString>("com.sun.star.mozilla.XPluginInstance","newStream"))
            (pair<OUString,OUString>("com.sun.star.mozilla.XPluginInstance","newURL"))
            (pair<OUString,OUString>("com.sun.star.mozilla.XPluginInstanceNotifySink","notifyURL"))
            (pair<OUString,OUString>("com.sun.star.mozilla.XPluginInstancePeer","showStatusMessage"))
            (pair<OUString,OUString>("com.sun.star.mozilla.XPluginInstancePeer","enableScripting"))
            (pair<OUString,OUString>("com.sun.star.mozilla.XPluginInstancePeer","newStream"))
            (pair<OUString,OUString>("com.sun.star.mozilla.XPluginInstancePeer","getURL"))
            (pair<OUString,OUString>("com.sun.star.mozilla.XPluginInstancePeer","postURL"))
            (pair<OUString,OUString>("com.sun.star.mozilla.XPluginWindowPeer","setChildWindow"))
            (pair<OUString,OUString>("com.sun.star.script.vba.XVBACompatibility","addVBAScriptListener"))
            (pair<OUString,OUString>("com.sun.star.script.vba.XVBACompatibility","removeVBAScriptListener"))
            (pair<OUString,OUString>("com.sun.star.sdb.XDatabaseAccess","addDatabaseAccessListener"))
            (pair<OUString,OUString>("com.sun.star.sdb.XDatabaseAccess","removeDatabaseAccessListener"))
            (pair<OUString,OUString>("com.sun.star.sdb.XDatabaseAccessListener","connectionChanged"))
            (pair<OUString,OUString>("com.sun.star.sdb.XDatabaseAccessListener","connectionClosing"))
            (pair<OUString,OUString>("com.sun.star.sdb.XRowSetApproveBroadcaster","addRowSetApproveListener"))
            (pair<OUString,OUString>("com.sun.star.sdb.XRowSetApproveBroadcaster","removeRowSetApproveListener"))
            (pair<OUString,OUString>("com.sun.star.sdb.XRowSetChangeListener","onRowSetChanged"))
            (pair<OUString,OUString>("com.sun.star.sdb.XRowSetSupplier","setRowSet"))
            (pair<OUString,OUString>("com.sun.star.sdb.XRowsChangeListener","rowsChanged"))
            (pair<OUString,OUString>("com.sun.star.sdb.XSQLErrorBroadcaster","addSQLErrorListener"))
            (pair<OUString,OUString>("com.sun.star.sdb.XSQLErrorBroadcaster","removeSQLErrorListener"))
            (pair<OUString,OUString>("com.sun.star.sdbc.XRowSet","addRowSetListener"))
            (pair<OUString,OUString>("com.sun.star.sdbc.XRowSet","removeRowSetListener"))
            (pair<OUString,OUString>("com.sun.star.sdbc.XRowSetListener","cursorMoved"))
            (pair<OUString,OUString>("com.sun.star.sdbc.XRowSetListener","rowChanged"))
            (pair<OUString,OUString>("com.sun.star.sdbc.XRowSetListener","rowSetChanged"))
            (pair<OUString,OUString>("com.sun.star.sheet.XCalculatable","enableAutomaticCalculation"))
            (pair<OUString,OUString>("com.sun.star.sheet.XVolatileResult","addResultListener"))
            (pair<OUString,OUString>("com.sun.star.sheet.XVolatileResult","removeResultListener"))
            (pair<OUString,OUString>("com.sun.star.task.XJobExecutor","trigger"))
            (pair<OUString,OUString>("com.sun.star.task.XStatusIndicator","start"))
            (pair<OUString,OUString>("com.sun.star.task.XStatusIndicator","end"))
            (pair<OUString,OUString>("com.sun.star.task.XStatusIndicator","setText"))
            (pair<OUString,OUString>("com.sun.star.task.XStatusIndicator","setValue"))
            (pair<OUString,OUString>("com.sun.star.task.XStatusIndicator","reset"))
            (pair<OUString,OUString>("com.sun.star.text.XSimpleText","insertString"))
            (pair<OUString,OUString>("com.sun.star.text.XTextCursor","collapseToStart"))
            (pair<OUString,OUString>("com.sun.star.text.XTextCursor","collapseToEnd"))
            (pair<OUString,OUString>("com.sun.star.text.XTextRange","setString"))
            (pair<OUString,OUString>("com.sun.star.text.XTextViewCursor","setVisible"))
            (pair<OUString,OUString>("com.sun.star.ucb.XCommandProcessor","abort"))
            (pair<OUString,OUString>("com.sun.star.ucb.XCommandProcessor2","releaseCommandIdentifier"))
            (pair<OUString,OUString>("com.sun.star.ucb.XContent","addContentEventListener"))
            (pair<OUString,OUString>("com.sun.star.ucb.XContent","removeContentEventListener"))
            (pair<OUString,OUString>("com.sun.star.ucb.XContentProviderManager","deregisterContentProvider"))
            (pair<OUString,OUString>("com.sun.star.ucb.XContentTransmitter","transmit"))
            (pair<OUString,OUString>("com.sun.star.ucb.XPropertySetRegistry","removePropertySet"))
            (pair<OUString,OUString>("com.sun.star.ui.XUIConfigurationListener","elementInserted"))
            (pair<OUString,OUString>("com.sun.star.ui.XUIConfigurationListener","elementRemoved"))
            (pair<OUString,OUString>("com.sun.star.ui.XUIConfigurationListener","elementReplaced"))
            (pair<OUString,OUString>("com.sun.star.ui.dialogs.XFilePickerNotifier","addFilePickerListener"))
            (pair<OUString,OUString>("com.sun.star.ui.dialogs.XFilePickerNotifier","removeFilePickerListener"))
            (pair<OUString,OUString>("com.sun.star.util.XBroadcaster","lockBroadcasts"))
            (pair<OUString,OUString>("com.sun.star.util.XBroadcaster","unlockBroadcasts"))
            (pair<OUString,OUString>("com.sun.star.util.XChangesListener","changesOccurred"))
            (pair<OUString,OUString>("com.sun.star.util.XChangesNotifier","addChangesListener"))
            (pair<OUString,OUString>("com.sun.star.util.XChangesNotifier","removeChangesListener"))
            (pair<OUString,OUString>("com.sun.star.util.XCloseBroadcaster","addCloseListener"))
            (pair<OUString,OUString>("com.sun.star.util.XCloseBroadcaster","removeCloseListener"))
            (pair<OUString,OUString>("com.sun.star.util.XFlushable","addFlushListener"))
            (pair<OUString,OUString>("com.sun.star.util.XFlushable","removeFlushListener"))
            (pair<OUString,OUString>("com.sun.star.util.XModeChangeListener","modeChanged"))
            (pair<OUString,OUString>("com.sun.star.util.XModifyBroadcaster","addModifyListener"))
            (pair<OUString,OUString>("com.sun.star.util.XModifyBroadcaster","removeModifyListener"))
            (pair<OUString,OUString>("com.sun.star.util.XRefreshable","addRefreshListener"))
            (pair<OUString,OUString>("com.sun.star.util.XRefreshable","removeRefreshListener"))
            (pair<OUString,OUString>("com.sun.star.util.XSearchDescriptor","setSearchString"))
            (pair<OUString,OUString>("com.sun.star.view.XPrintJobBroadcaster","addPrintJobListener"))
            (pair<OUString,OUString>("com.sun.star.view.XPrintJobBroadcaster","removePrintJobListener"))
            (pair<OUString,OUString>("com.sun.star.view.XPrintJobListener","printJobEvent"))
            (pair<OUString,OUString>("com.sun.star.view.XPrintableBroadcaster","addPrintableListener"))
            (pair<OUString,OUString>("com.sun.star.view.XPrintableBroadcaster","removePrintableListener"))
            (pair<OUString,OUString>("com.sun.star.view.XPrintableListener","stateChanged"))
            (pair<OUString,OUString>("com.sun.star.view.XSelectionChangeListener","selectionChanged"))
            (pair<OUString,OUString>("com.sun.star.beans.XMultiPropertySet","addPropertiesChangeListener"))
            (pair<OUString,OUString>("com.sun.star.beans.XMultiPropertySet","removePropertiesChangeListener"))
            (pair<OUString,OUString>("com.sun.star.beans.XMultiPropertySet","firePropertiesChangeEvent"))
            (pair<OUString,OUString>("com.sun.star.beans.XPropertiesChangeNotifier","addPropertiesChangeListener"))
            (pair<OUString,OUString>("com.sun.star.beans.XPropertiesChangeNotifier","removePropertiesChangeListener"))
            (pair<OUString,OUString>("com.sun.star.container.XContainer","addContainerListener"))
            (pair<OUString,OUString>("com.sun.star.container.XContainer","removeContainerListener"))
            (pair<OUString,OUString>("com.sun.star.container.XContainerListener","elementInserted"))
            (pair<OUString,OUString>("com.sun.star.container.XContainerListener","elementRemoved"))
            (pair<OUString,OUString>("com.sun.star.container.XContainerListener","elementReplaced"))
            (pair<OUString,OUString>("com.sun.star.container.XNamed","setName"))
            (pair<OUString,OUString>("com.sun.star.io.XDataExporter","exportData"))
            (pair<OUString,OUString>("com.sun.star.io.XDataExporter","cancel"))
            (pair<OUString,OUString>("com.sun.star.io.XDataImporter","importData"))
            (pair<OUString,OUString>("com.sun.star.io.XDataImporter","cancel"))
            (pair<OUString,OUString>("com.sun.star.io.XDataTransferEventListener","finished"))
            (pair<OUString,OUString>("com.sun.star.io.XDataTransferEventListener","cancelled"))
            (pair<OUString,OUString>("com.sun.star.lang.XConnectionPointContainer","advise"))
            (pair<OUString,OUString>("com.sun.star.lang.XConnectionPointContainer","unadvise"))
            (pair<OUString,OUString>("com.sun.star.script.XAllListener","firing"))
            (pair<OUString,OUString>("com.sun.star.uno.XInterface","acquire"))
            (pair<OUString,OUString>("com.sun.star.uno.XInterface","release"))
            (pair<OUString,OUString>("com.sun.star.uno.XReference","dispose"));

        pair<OUString,OUString> k(_rListenerType, _rMethodName);
        return delayed_event_listeners.find(k) != delayed_event_listeners.end();
    }

    //--------------------------------------------------------------------
    void FormScriptListener::impl_doFireScriptEvent_nothrow( ::osl::ClearableMutexGuard& _rGuard, const ScriptEvent& _rEvent, Any* _pSyncronousResult )
    {
        OSL_PRECOND( m_pScriptExecutor, "FormScriptListener::impl_doFireScriptEvent_nothrow: this will crash!" );

        _rGuard.clear();
        m_pScriptExecutor->doFireScriptEvent( _rEvent, _pSyncronousResult );
    }

    //--------------------------------------------------------------------
    void SAL_CALL FormScriptListener::firing( const ScriptEvent& _rEvent ) throw (RuntimeException)
    {
        ::osl::ClearableMutexGuard aGuard( m_aMutex );
       static const ::rtl::OUString vbaInterOp( RTL_CONSTASCII_USTRINGPARAM("VBAInterop") );
       if ( _rEvent.ScriptType.equals(vbaInterOp) )
           return; // not handled here

        if ( impl_isDisposed_nothrow() )
            return;

        if ( !impl_allowAsynchronousCall_nothrow( _rEvent.ListenerType.getTypeName(), _rEvent.MethodName ) )
        {
            impl_doFireScriptEvent_nothrow( aGuard, _rEvent, NULL );
            return;
        }

        acquire();
        Application::PostUserEvent( LINK( this, FormScriptListener, OnAsyncScriptEvent ), new ScriptEvent( _rEvent ) );
    }

    //--------------------------------------------------------------------
    Any SAL_CALL FormScriptListener::approveFiring( const ScriptEvent& _rEvent ) throw (InvocationTargetException, RuntimeException)
    {
        Any aResult;

        ::osl::ClearableMutexGuard aGuard( m_aMutex );
        if ( !impl_isDisposed_nothrow() )
            impl_doFireScriptEvent_nothrow( aGuard, _rEvent, &aResult );

        return aResult;
    }

    //--------------------------------------------------------------------
    void SAL_CALL FormScriptListener::disposing( const EventObject& /*Source*/ ) throw (RuntimeException)
    {
        // not interested in
    }

    //--------------------------------------------------------------------
    void SAL_CALL FormScriptListener::dispose()
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        m_pScriptExecutor = NULL;
    }

    //--------------------------------------------------------------------
    IMPL_LINK( FormScriptListener, OnAsyncScriptEvent, ScriptEvent*, _pEvent )
    {
        OSL_PRECOND( _pEvent != NULL, "FormScriptListener::OnAsyncScriptEvent: invalid event!" );
        if ( !_pEvent )
            return 1L;

        {
            ::osl::ClearableMutexGuard aGuard( m_aMutex );

            if ( !impl_isDisposed_nothrow() )
                impl_doFireScriptEvent_nothrow( aGuard, *_pEvent, NULL );
        }

        delete _pEvent;
        // we acquired ourself immediately before posting the event
        release();
        return 0L;
    }

    //====================================================================
    //= FormScriptingEnvironment
    //====================================================================
    //--------------------------------------------------------------------
    FormScriptingEnvironment::FormScriptingEnvironment( FmFormModel& _rModel )
        :m_refCount( 0 )
        ,m_pScriptListener( NULL )
        ,m_rFormModel( _rModel )
        ,m_bDisposed( false )
    {
        m_pScriptListener = ListenerImplementation( new FormScriptListener( this ) );
        // note that this is a cyclic reference between the FormScriptListener and the FormScriptingEnvironment
        // This cycle is broken up when our instance is disposed.
    }

    //--------------------------------------------------------------------
    FormScriptingEnvironment::~FormScriptingEnvironment()
    {
    }

    //--------------------------------------------------------------------
    void FormScriptingEnvironment::impl_registerOrRevoke_throw( const Reference< XEventAttacherManager >& _rxManager, bool _bRegister )
    {
        ::osl::MutexGuard aGuard( m_aMutex );

        if ( !_rxManager.is() )
            throw IllegalArgumentException();
        if ( m_bDisposed )
            throw DisposedException();

        try
        {
            if ( _bRegister )
                _rxManager->addScriptListener( m_pScriptListener.getRef() );
            else
                _rxManager->removeScriptListener( m_pScriptListener.getRef() );
        }
        catch( const RuntimeException& ) { throw; }
        catch( const Exception& )
        {
            DBG_UNHANDLED_EXCEPTION();
        }
    }

    //--------------------------------------------------------------------
    void FormScriptingEnvironment::registerEventAttacherManager( const Reference< XEventAttacherManager >& _rxManager )
    {
        impl_registerOrRevoke_throw( _rxManager, true );
    }

    //--------------------------------------------------------------------
    void FormScriptingEnvironment::revokeEventAttacherManager( const Reference< XEventAttacherManager >& _rxManager )
    {
        impl_registerOrRevoke_throw( _rxManager, false );
    }

    //--------------------------------------------------------------------
    oslInterlockedCount SAL_CALL FormScriptingEnvironment::acquire()
    {
        return osl_atomic_increment( &m_refCount );
    }

    //--------------------------------------------------------------------
    oslInterlockedCount SAL_CALL FormScriptingEnvironment::release()
    {
        if ( 0 == osl_atomic_decrement( &m_refCount ) )
        {
           delete this;
           return 0;
        }
        return m_refCount;
    }

    //--------------------------------------------------------------------
    IFormScriptingEnvironment::~IFormScriptingEnvironment()
    {
    }

    //--------------------------------------------------------------------
    namespace
    {
        //................................................................
        //. NewStyleUNOScript
        //................................................................
        class SAL_NO_VTABLE IScript
        {
        public:
            virtual void invoke( const Sequence< Any >& _rArguments, Any& _rSynchronousResult ) = 0;

            virtual ~IScript() { }
        };
        typedef ::boost::shared_ptr< IScript >  PScript;

        //................................................................
        //. NewStyleUNOScript
        //................................................................
        class NewStyleUNOScript : public IScript
        {
            SfxObjectShell&         m_rObjectShell;
            const ::rtl::OUString   m_sScriptCode;

        public:
            NewStyleUNOScript( SfxObjectShell& _rObjectShell, const ::rtl::OUString& _rScriptCode )
                :m_rObjectShell( _rObjectShell )
                ,m_sScriptCode( _rScriptCode )
            {
            }

            // IScript
            virtual void invoke( const Sequence< Any >& _rArguments, Any& _rSynchronousResult );
        };

        //................................................................
        void NewStyleUNOScript::invoke( const Sequence< Any >& _rArguments, Any& _rSynchronousResult )
        {
            Sequence< sal_Int16 > aOutArgsIndex;
            Sequence< Any > aOutArgs;
            EventObject aEvent;
            Any aCaller;
            if ( ( _rArguments.getLength() > 0 ) && ( _rArguments[ 0 ] >>= aEvent ) )
            {
                try
                {
                    Reference< XControl > xControl( aEvent.Source, UNO_QUERY_THROW );
                    Reference< XPropertySet > xProps( xControl->getModel(), UNO_QUERY_THROW );
                    aCaller = xProps->getPropertyValue( rtl::OUString( RTL_CONSTASCII_USTRINGPARAM("Name") ) );
                }
                catch( Exception& ) {}
            }
            m_rObjectShell.CallXScript( m_sScriptCode, _rArguments, _rSynchronousResult, aOutArgsIndex, aOutArgs, true, aCaller.hasValue() ? &aCaller : 0 );
        }
    }

    //--------------------------------------------------------------------
    void FormScriptingEnvironment::doFireScriptEvent( const ScriptEvent& _rEvent, Any* _pSyncronousResult )
    {
#ifdef DISABLE_SCRIPTING
        (void) _rEvent;
        (void) _pSyncronousResult;
#else
        SolarMutexClearableGuard aSolarGuard;
        ::osl::ClearableMutexGuard aGuard( m_aMutex );

        if ( m_bDisposed )
            return;

        // SfxObjectShellRef is good here since the model controls the lifetime of the object
        SfxObjectShellRef xObjectShell = m_rFormModel.GetObjectShell();
        if( !xObjectShell.Is() )
            return;

        // the script to execute
        PScript pScript;

        if ( _rEvent.ScriptType != "StarBasic" )
        {
            pScript.reset( new NewStyleUNOScript( *xObjectShell, _rEvent.ScriptCode ) );
        }
        else
        {
            ::rtl::OUString sScriptCode = _rEvent.ScriptCode;
            ::rtl::OUString sMacroLocation;

            // is there a location in the script name ("application" or "document")?
            sal_Int32 nPrefixLen = sScriptCode.indexOf( ':' );
            DBG_ASSERT( 0 <= nPrefixLen, "FormScriptingEnvironment::doFireScriptEvent: Basic script name in old format encountered!" );

            if ( 0 <= nPrefixLen )
            {
                // and it has such a prefix
                sMacroLocation = sScriptCode.copy( 0, nPrefixLen );
                DBG_ASSERT( 0 == sMacroLocation.compareToAscii( "document" )
                        ||  0 == sMacroLocation.compareToAscii( "application" ),
                        "FormScriptingEnvironment::doFireScriptEvent: invalid (unknown) prefix!" );

                // strip the prefix: the SfxObjectShell::CallScript knows nothing about such prefixes
                sScriptCode = sScriptCode.copy( nPrefixLen + 1 );
            }

            if ( sMacroLocation.isEmpty() )
            {
                // legacy format: use the app-wide Basic, if it has a respective method, otherwise fall back to the doc's Basic
                if ( SFX_APP()->GetBasicManager()->HasMacro( sScriptCode ) )
                    sMacroLocation = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "application" ) );
                else
                    sMacroLocation = ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "document" ) );
            }

            ::rtl::OUStringBuffer aScriptURI;
            aScriptURI.appendAscii( "vnd.sun.star.script:" );
            aScriptURI.append( sScriptCode );
            aScriptURI.appendAscii( "?language=Basic" );
            aScriptURI.appendAscii( "&location=" );
            aScriptURI.append( sMacroLocation );

            const ::rtl::OUString sScriptURI( aScriptURI.makeStringAndClear() );
            pScript.reset( new NewStyleUNOScript( *xObjectShell, sScriptURI ) );
        }

        OSL_ENSURE( pScript.get(), "FormScriptingEnvironment::doFireScriptEvent: no script to execute!" );
        if ( !pScript.get() )
            // this is an internal error in the above code
            throw RuntimeException();

        aGuard.clear();
        aSolarGuard.clear();

        Any aIgnoreResult;
        pScript->invoke( _rEvent.Arguments, _pSyncronousResult ? *_pSyncronousResult : aIgnoreResult );
        pScript.reset();

        {
            // object shells are not thread safe, so guard the destruction
            SolarMutexGuard aSolarGuarsReset;
            xObjectShell = NULL;
        }
#endif
    }

    //--------------------------------------------------------------------
    void FormScriptingEnvironment::dispose()
    {
        ::osl::MutexGuard aGuard( m_aMutex );
        m_bDisposed = true;
        m_pScriptListener->dispose();
    }

    //--------------------------------------------------------------------
    PFormScriptingEnvironment createDefaultFormScriptingEnvironment( FmFormModel& _rModel )
    {
        return new FormScriptingEnvironment( _rModel );
    }

//........................................................................
} // namespace svxform
//........................................................................

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
