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

#ifndef EXTENSIONS_SOURCE_PROPCTRLR_FORMCOMPONENTHANDLER_HXX
#define EXTENSIONS_SOURCE_PROPCTRLR_FORMCOMPONENTHANDLER_HXX

#include "propertyhandler.hxx"
#include "sqlcommanddesign.hxx"
#include "pcrcommon.hxx"
#include <comphelper/uno3.hxx>
#include <comphelper/proparrhlp.hxx>
#include <comphelper/propertycontainer.hxx>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/beans/XPropertyState.hpp>
#include <com/sun/star/sdbc/XRowSet.hpp>
#include <com/sun/star/awt/XControlContainer.hpp>
#include <com/sun/star/form/XForm.hpp>
#include <tools/fldunit.hxx>
#include <vcl/waitobj.hxx>
#include <connectivity/dbtools.hxx>

#include <set>

//........................................................................
namespace pcr
{
//........................................................................

    //====================================================================
    //= ComponentClassification
    //====================================================================
    enum ComponentClassification
    {
        eFormControl,
        eDialogControl,
        eUnknown
    };

    //====================================================================
    //= FormComponentPropertyHandler
    //====================================================================
    class FormComponentPropertyHandler;
    typedef HandlerComponentBase< FormComponentPropertyHandler > FormComponentPropertyHandler_Base;
    typedef ::comphelper::OPropertyArrayUsageHelper<FormComponentPropertyHandler> FormComponentPropertyHandler_PROP;
    /** default ->XPropertyHandler for all form components.
    */
    class FormComponentPropertyHandler :    public FormComponentPropertyHandler_Base,
                                            public ::comphelper::OPropertyContainer,
                                            public FormComponentPropertyHandler_PROP
    {
    private:
        /// access to property states
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyState >             m_xPropertyState;
        /// the parent of our component
        ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >                   m_xObjectParent;

        /// the database connection. Owned by us if and only if we created it ourself.
        mutable ::dbtools::SharedConnection                                                     m_xRowSetConnection;
        ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowSet >                     m_xRowSet;
        /** helper component encapsulating the handling for the QueryDesign component for
            interactively designing an SQL command
        */
        ::rtl::Reference< SQLCommandDesigner >                                                  m_xCommandDesigner;
        ::com::sun::star::uno::Reference< ::com::sun::star::inspection::XObjectInspectorUI >    m_xBrowserUI;

        /// the string indicating a "default" (VOID) value in list-like controls
        ::rtl::OUString                 m_sDefaultValueString;
        /// all properties to whose control's we added ->m_sDefaultValueString
        ::std::set< ::rtl::OUString >   m_aPropertiesWithDefListEntry;
        /// type of our component
        ComponentClassification         m_eComponentClass;
        /// is our component a (database) sub form?
        bool                            m_bComponentIsSubForm : 1;
        /// our component has a "ListSource" property
        bool                            m_bHaveListSource : 1;
        /// our component has a "Command" property
        bool                            m_bHaveCommand : 1;
        /// the class id of the component - if appliable
        sal_Int16                       m_nClassId;

    public:
        FormComponentPropertyHandler(
            const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext >& _rxContext
        );

        DECLARE_XINTERFACE( )

        // XPropertySet
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySetInfo > SAL_CALL getPropertySetInfo(  ) throw(::com::sun::star::uno::RuntimeException);

        static ::rtl::OUString SAL_CALL getImplementationName_static(  ) throw (::com::sun::star::uno::RuntimeException);
        static ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames_static(  ) throw (::com::sun::star::uno::RuntimeException);

    protected:
        ~FormComponentPropertyHandler();

    protected:
        virtual ::cppu::IPropertyArrayHelper* createArrayHelper( ) const;
        virtual ::cppu::IPropertyArrayHelper& SAL_CALL getInfoHelper();
        // XPropertyHandler overridables
        virtual ::com::sun::star::uno::Any                          SAL_CALL getPropertyValue( const ::rtl::OUString& _rPropertyName ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException);
        virtual void                                                SAL_CALL setPropertyValue( const ::rtl::OUString& _rPropertyName, const ::com::sun::star::uno::Any& _rValue ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Any                          SAL_CALL convertToPropertyValue( const ::rtl::OUString& _rPropertyName, const ::com::sun::star::uno::Any& _rControlValue ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Any                          SAL_CALL convertToControlValue( const ::rtl::OUString& _rPropertyName, const ::com::sun::star::uno::Any& _rPropertyValue, const ::com::sun::star::uno::Type& _rControlValueType ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::beans::PropertyState              SAL_CALL getPropertyState( const ::rtl::OUString& _rPropertyName ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::uno::RuntimeException);
        virtual void                                                SAL_CALL addPropertyChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& _rxListener ) throw (::com::sun::star::uno::RuntimeException);
        virtual void                                                SAL_CALL removePropertyChangeListener( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener >& _rxListener ) throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString >  SAL_CALL getSupersededProperties() throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< ::rtl::OUString >  SAL_CALL getActuatingProperties() throw (::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::inspection::LineDescriptor        SAL_CALL describePropertyLine( const ::rtl::OUString& _rPropertyName, const ::com::sun::star::uno::Reference< ::com::sun::star::inspection::XPropertyControlFactory >& _rxControlFactory ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::NullPointerException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::inspection::InteractiveSelectionResult
                                                                    SAL_CALL onInteractivePropertySelection( const ::rtl::OUString& _rPropertyName, sal_Bool _bPrimary, ::com::sun::star::uno::Any& _rData, const ::com::sun::star::uno::Reference< ::com::sun::star::inspection::XObjectInspectorUI >& _rxInspectorUI ) throw (::com::sun::star::beans::UnknownPropertyException, ::com::sun::star::lang::NullPointerException, ::com::sun::star::uno::RuntimeException);
        virtual void                                                SAL_CALL actuatingPropertyChanged( const ::rtl::OUString& _rActuatingPropertyName, const ::com::sun::star::uno::Any& _rNewValue, const ::com::sun::star::uno::Any& _rOldValue, const ::com::sun::star::uno::Reference< ::com::sun::star::inspection::XObjectInspectorUI >& _rxInspectorUI, sal_Bool _bFirstTimeInit ) throw (::com::sun::star::lang::NullPointerException, ::com::sun::star::uno::RuntimeException);
        virtual sal_Bool                                            SAL_CALL suspend( sal_Bool _bSuspend ) throw (::com::sun::star::uno::RuntimeException);

        // XComponent
        virtual void                                                SAL_CALL disposing();

        // PropertyHandler
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::Property >
                                            SAL_CALL doDescribeSupportedProperties() const;
        virtual void onNewComponent();

    private:
        /** initializes some (cached) meta data about the component
            @throws RuntimeException
                if a serious error occurs, for instance if the component does not provide an XPropertySetInfo instance
        */
        void    impl_initComponentMetaData_throw();

        /** classifies our component, in case it's a control model, by ClassId

            Note that UNO dialog controls are also classified, though they don't have the ClassId property
        */
        void    impl_classifyControlModel_throw();

        /** const-version of ->getPropertyValue
        */
        ::com::sun::star::uno::Any impl_getPropertyValue_throw( const ::rtl::OUString& _rPropertyName ) const;

        // some property values are faked, and not used in the way they're provided by our component
        void impl_normalizePropertyValue_nothrow( ::com::sun::star::uno::Any& _rValue, PropertyId _nPropId ) const;

        /** determines whether we should exclude a given property from our "supported properties"
        */
        bool impl_shouldExcludeProperty_nothrow( const ::com::sun::star::beans::Property& _rProperty ) const;

        /** initializes the list of field names, if we're handling a control which supports the
            DataField property
        */
        void impl_initFieldList_nothrow( ::std::vector< ::rtl::OUString >& rFieldNames ) const;

        /** obtaines the RowSet to which our component belongs

            If the component is a RowSet itself, it's returned directly. Else, the parent
            is examined for the XRowSet interface. If the parent is no XRowSet, then
            a check is made whether our component is a grid control column, and if so,
            the parent of the grid control is examied for the XRowSet interace.

            Normally, at least one of those methods should succeed.
        */
        ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowSet > impl_getRowSet_throw( ) const;

        /** nothrow-version of ->impl_getRowSet_throw
        */
        ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XRowSet > impl_getRowSet_nothrow( ) const;

        /** connects the row set belonging to our introspected data aware form component,
            and remembers the connection in ->m_xRowSetConnection.

            If the row set already is connected, ->m_xRowSetConnection will be set, too, but
            not take the ownership of the connection.

            If ->m_xRowSetConnection is already set, nothing happens, so if you want to
            force creation of a connection, you need to clear ->m_xRowSetConnection.
        */
        bool impl_ensureRowsetConnection_nothrow() const;

        /** clears ->m_xRowSetConnection
        */
        void impl_clearRowsetConnection_nothrow();

        /** fills an ->LineDescriptor with information to represent a cursor source
            of our form - that is, a table, a query, or an SQL statement.

            As an example, if our form has currently a CommandType of TABLE, then the
            value list in the LineDescriptor will contain a list of all tables
            of the data source which the form is bound to.

            @seealso impl_fillTableNames_throw
            @seealso impl_fillQueryNames_throw
        */
        void impl_describeCursorSource_nothrow(
                ::com::sun::star::inspection::LineDescriptor& _out_rProperty,
                const ::com::sun::star::uno::Reference< ::com::sun::star::inspection::XPropertyControlFactory >& _rxControlFactory
            ) const;

        /** describes the UI for selecting a table name

            @precond
                m_xRowSetConnection is not <NULL/>
        */
        void impl_fillTableNames_throw( ::std::vector< ::rtl::OUString >& _out_rNames ) const;

        /** describes the UI for selecting a query name

            @precond
                m_xRowSetConnection is not <NULL/>
        */
        void impl_fillQueryNames_throw( ::std::vector< ::rtl::OUString >& _out_rNames ) const;

        /** describes the UI for selecting a query name

            @precond
                m_xRowSetConnection is not <NULL/>
        */
        void impl_fillQueryNames_throw( const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess >& _xQueryNames
                    ,::std::vector< ::rtl::OUString >& _out_rNames
                    ,const ::rtl::OUString& _sName = ::rtl::OUString() ) const;

        /** describes the UI for selecting a ListSource (for list-like form controls)
            @precond
                ->m_xRowSetConnection is not <NULL/>
            @precond
                ->m_xComponent is not <NULL/>
        */
        void impl_describeListSourceUI_throw(
                ::com::sun::star::inspection::LineDescriptor& _out_rDescriptor,
                const ::com::sun::star::uno::Reference< ::com::sun::star::inspection::XPropertyControlFactory >& _rxControlFactory
            ) const;

        /** displays a datbase-related error to the user
        */
        void impl_displaySQLError_nothrow( const ::dbtools::SQLExceptionInfo& _rErrorDescriptor ) const;

        /** let's the user chose a selection of entries from a string list, and stores this
            selection in the given property
            @return
                <TRUE/> if and only if the user successfully changed the property
        */
        bool impl_dialogListSelection_nothrow( const ::rtl::OUString& _rProperty, ::osl::ClearableMutexGuard& _rClearBeforeDialog ) const;

        /** executes a dialog for chosing a filter or sort criterion for a database form
            @param _bFilter
                <TRUE/> if the Filter property should be used, <FALSE/> if it's the Order
                property
            @param _out_rSelectedClause
                the filter or order clause as chosen by the user
            @precond
                we're really inspecting a database form (well, a RowSet at least)
            @return
                <TRUE/> if and only if the user successfully chose a clause
        */
        bool impl_dialogFilterOrSort_nothrow( bool _bFilter, ::rtl::OUString& _out_rSelectedClause, ::osl::ClearableMutexGuard& _rClearBeforeDialog ) const;

        /** executes a dialog which allows the user to chose the columns linking
            a sub to a master form, and sets the respective MasterFields / SlaveFields
            properties at the form.
            @precond
                we're inspecting (sub) database form
            @return
                <TRUE/> if and only if the user successfully eneter master and slave fields
        */
        bool impl_dialogLinkedFormFields_nothrow( ::osl::ClearableMutexGuard& _rClearBeforeDialog ) const;

        /** executes a dialog which allows the user to modify the FormatKey
            property of our component, by chosing a (number) format.
            @precond
                Our component actually has a FormatKey property.
            @param _out_rNewValue
                the new property value, if the user chose a new formatting
            @return
                <TRUE/> if and only if a new formatting has been chosen by the user.
                In this case, ->_out_rNewValue is filled with the new property value
        */
        bool impl_dialogFormatting_nothrow( ::com::sun::star::uno::Any& _out_rNewValue, ::osl::ClearableMutexGuard& _rClearBeforeDialog ) const;

        /** executes a dialog which allows to the user to change the ImageURL property
            of our component by browsing for an image file.
            @precond
                our component actually has a ImageURL property
            @param _out_rNewValue
                the new property value, if the user chose a new image url
            @return
                <TRUE/> if and only if a new image URL has been chosen by the user.
                In this case, ->_out_rNewValue is filled with the new property value
        */
        bool impl_browseForImage_nothrow( ::com::sun::star::uno::Any& _out_rNewValue, ::osl::ClearableMutexGuard& _rClearBeforeDialog ) const;

        /** executes a dialog which allows the user to change the TargetURL property of
            our component
            @precond
                our component actually has a TargetURL property
            @param _out_rNewValue
                the new property value, if the user chose a new TargetURL
            @return
                <TRUE/> if and only if a new TargetURL has been chosen by the user.
                In this case, ->_out_rNewValue is filled with the new property value
        */
        bool impl_browseForTargetURL_nothrow( ::com::sun::star::uno::Any& _out_rNewValue, ::osl::ClearableMutexGuard& _rClearBeforeDialog ) const;

        /** executes a dialog which allows the user to change the font, plus related properties,
            of our component
            @precond
                our component actually has a Font property
            @param _out_rNewValue
                a value desribing the new font, as <code>Sequence&lt; NamedValue &gt;</code>
            @return
                <TRUE/> if and only if the user successfully changed the font of our component
        */
        bool impl_executeFontDialog_nothrow( ::com::sun::star::uno::Any& _out_rNewValue, ::osl::ClearableMutexGuard& _rClearBeforeDialog ) const;

        /** allows the user browsing for a database document
            @precond
                our component actually has a DataSource property
            @param _out_rNewValue
                the new property value, if the user chose a new DataSource
            @return
                <TRUE/> if and only if a new DataSource has been chosen by the user.
                In this case, ->_out_rNewValue is filled with the new property value
        */
        bool impl_browseForDatabaseDocument_throw( ::com::sun::star::uno::Any& _out_rNewValue, ::osl::ClearableMutexGuard& _rClearBeforeDialog ) const;

        /** raises a dialog which allows the user to choose a color
            @param  _nColorPropertyId
                the ID of the color property
            @param  _out_rNewValue
                the chosen color value
            @return
                <TRUE/> if and only if a color was chosen by the user
        */
        bool impl_dialogColorChooser_throw( sal_Int32 _nColorPropertyId, ::com::sun::star::uno::Any& _out_rNewValue, ::osl::ClearableMutexGuard& _rClearBeforeDialog ) const;

        /** raises a dialog which allows the user to choose a label control for our component
            @param  _out_rNewValue
                the chosen label control, if any
            @return
                <TRUE/> if and only if a label control was chosen by the user
        */
        bool impl_dialogChooseLabelControl_nothrow( ::com::sun::star::uno::Any& _out_rNewValue, ::osl::ClearableMutexGuard& _rClearBeforeDialog ) const;

        /** raises a dialog which lets the user chose the tab order of controls of a form
            @precond
                we have a view control container in which our controls live
            @return
                <TRUE/> if and only if the user successfully changed the tab order
            @seealso impl_getContextControlContainer_nothrow
        */
        bool impl_dialogChangeTabOrder_nothrow( ::osl::ClearableMutexGuard& _rClearBeforeDialog ) const;

        /** retrieves the context for controls, whose model(s) we're inspecting

            If we're inspecting a control model, this is usually part of a set of controls
            and control models, where the controls live in a certain context (a ->XControlContainer).
            If we know this context, we can enable additional special functionality.

            The ->XComponentContext in which we were created is examined for a value
            named "ControlContext", and this value is returned.
        */
        ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer >
            impl_getContextControlContainer_nothrow() const;

        /** opens a query design window for interactively designing the SQL command of a
            database form
            @param _rxUIUpdate
                access to the property browser UI
            @param _nDesignForProperty
                the ID for the property for which the designer is opened
            @return
                <TRUE/> if the window was successfully opened, or was previously open,
                <FALSE/> otherwise
        */
        bool impl_doDesignSQLCommand_nothrow(
            const ::com::sun::star::uno::Reference< ::com::sun::star::inspection::XObjectInspectorUI >& _rxInspectorUI,
            PropertyId _nDesignForProperty
        );

        /** updates a property (UI) whose state depends on more than one other property

            ->actuatingPropertyChanged is called for certain properties in whose changes
            we expressed interes (->getActuatingProperty). Now such a property change can
            result in simple UI updates, for instance another property being enabled or disabled.

            However, it can also result in a more complex change: The current (UI) state might
            depend on the value of more than one other property. Those dependent properties (their
            UI, more precisly) are updated in this method.

            @param _nPropid
                the ->PropertyId of the dependent property whose UI state is to be updated

            @param _rxInspectorUI
                provides access to the property browser UI. Must not be <NULL/>.
        */
        void impl_updateDependentProperty_nothrow( PropertyId _nPropId, const ::com::sun::star::uno::Reference< ::com::sun::star::inspection::XObjectInspectorUI >& _rxInspectorUI ) const;

        /** determines whether the given form has a valid data source signature.

            Valid here means that the DataSource property denotes an existing data source, and the
            Command property is not empty. No check is made whether the value of the Command property
            denotes an existent object, since this would be way too expensive.

            @param _xFormProperties
                the form to check. Must not be <NULL/>.
            @param _bAllowEmptyDataSourceName
                determine whether an empty data source name is allowed (<TRUE/>), and should not
                lead to rejection
        */
        static bool impl_hasValidDataSourceSignature_nothrow(
                const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _xFormProperties,
                bool _bAllowEmptyDataSourceName );

        /** returns the URL of our context document
            @return
        */
        ::rtl::OUString impl_getDocumentURL_nothrow() const;

    private:
        DECL_LINK( OnDesignerClosed, void* );

    private:
        FormComponentPropertyHandler();                                                 // never implemented
        FormComponentPropertyHandler( const FormComponentPropertyHandler& );            // never implemented
        FormComponentPropertyHandler& operator=( const FormComponentPropertyHandler& ); // never implemented

    private:
        using ::comphelper::OPropertyContainer::addPropertyChangeListener;
        using ::comphelper::OPropertyContainer::removePropertyChangeListener;
    };

    //====================================================================
    //= WaitCursor
    //====================================================================
    /** wrapper around a ->WaitObject which can cope with a NULL window
    */
    class WaitCursor
    {
    private:
        ::std::auto_ptr< WaitObject >       m_aWaitObject;

    public:
        WaitCursor( Window* _pWindow )
        {
            if ( _pWindow )
                m_aWaitObject.reset( new WaitObject( _pWindow ) );
        }
    };

//........................................................................
} // namespace pcr
//........................................................................

#endif // EXTENSIONS_SOURCE_PROPCTRLR_FORMCOMPONENTHANDLER_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
