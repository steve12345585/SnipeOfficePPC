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
package integration.forms;

/**************************************************************************/

import com.sun.star.awt.XControl;
import com.sun.star.awt.XControlModel;
import com.sun.star.awt.XWindow;
import com.sun.star.beans.PropertyValue;
import com.sun.star.beans.XPropertySet;
import com.sun.star.container.XIndexContainer;
import com.sun.star.form.FormComponentType;
import com.sun.star.form.XForm;
import com.sun.star.form.runtime.XFormController;
import com.sun.star.frame.XController;
import com.sun.star.frame.XDispatch;
import com.sun.star.frame.XDispatchProvider;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.util.URL;
import com.sun.star.util.XURLTransformer;
import com.sun.star.view.XControlAccess;
import com.sun.star.view.XFormLayerAccess;
import org.openoffice.xforms.XMLDocument;

/**************************************************************************/
/** provides a small wrapper around a document view
*/
public class DocumentViewHelper
{
    private     XMultiServiceFactory    m_orb;
    private     XController             m_controller;
    private     DocumentHelper          m_document;

    /* ------------------------------------------------------------------ */
    final protected XController getController()
    {
        return m_controller;
    }

    /* ------------------------------------------------------------------ */
    final protected DocumentHelper getDocument()
    {
        return m_document;
    }

    /* ------------------------------------------------------------------ */
    public DocumentViewHelper( XMultiServiceFactory orb, DocumentHelper document, XController controller )
    {
        m_orb = orb;
        m_document = document;
        m_controller = controller;
    }

    /* ------------------------------------------------------------------ */
    /** Quick access to a given interface of the view
        @param aInterfaceClass
                the class of the interface which shall be returned
    */
    public Object query( Class aInterfaceClass )
    {
        return UnoRuntime.queryInterface( aInterfaceClass, m_controller );
    }

    /* ------------------------------------------------------------------ */
    /** retrieves a dispatcher for the given URL, obtained at the current view of the document
        @param aURL
            a one-element array. The first element must contain a valid
            <member scope="com.sun.star.util">URL::Complete</member> value. Upon return, the URL is correctly
            parsed.
        @return
            the dispatcher for the URL in question
    */
    public XDispatch getDispatcher( URL[] aURL ) throws java.lang.Exception
    {
        XDispatch xReturn = null;

        // go get the current view
        XController xController = (XController)query( XController.class );
        // go get the dispatch provider of it's frame
        XDispatchProvider xProvider = (XDispatchProvider)UnoRuntime.queryInterface(
            XDispatchProvider.class, xController.getFrame() );
        if ( null != xProvider )
        {
            // need an URLTransformer
            XURLTransformer xTransformer = (XURLTransformer)UnoRuntime.queryInterface(
                    XURLTransformer.class, m_orb.createInstance( "com.sun.star.util.URLTransformer" ) );
            xTransformer.parseStrict( aURL );

            xReturn = xProvider.queryDispatch( aURL[0], new String( ), 0 );
        }
        return xReturn;
    }

    /* ------------------------------------------------------------------ */
    /** retrieves a dispatcher for the given URL, obtained at the current view of the document
    */
    public XDispatch getDispatcher( String url ) throws java.lang.Exception
    {
        URL[] aURL = new URL[] { new URL() };
        aURL[0].Complete = url;
        return getDispatcher( aURL );
    }

    /* ------------------------------------------------------------------ */
    /** dispatches the given URL into the view, if there's a dispatcher for it

        @return
            <TRUE/> if the URL was successfully dispatched
    */
    public boolean dispatch( String url ) throws java.lang.Exception
    {
        URL[] completeURL = new URL[] { new URL() };
        completeURL[0].Complete = url;
        XDispatch dispatcher = getDispatcher( completeURL );
        if ( dispatcher == null )
            return false;

        PropertyValue[] aDummyArgs = new PropertyValue[] { };
        dispatcher.dispatch( completeURL[0], aDummyArgs );
        return true;
    }

    /* ------------------------------------------------------------------ */
    /** retrieves a control within the current view of a document
        @param xModel
            specifies the control model whose control should be located
        @return
            the control tied to the model
    */
    public XControl getControl( XControlModel xModel ) throws com.sun.star.uno.Exception
    {
        // the current view of the document
        XControlAccess xCtrlAcc = (XControlAccess)query( XControlAccess.class );
        // delegate the task of looking for the control
        return xCtrlAcc.getControl( xModel );
    }

    /* ------------------------------------------------------------------ */
    public XControl getControl( Object aModel ) throws com.sun.star.uno.Exception
    {
        XControlModel xModel = (XControlModel)UnoRuntime.queryInterface( XControlModel.class, aModel );
        return getControl( xModel );
    }

    /* ------------------------------------------------------------------ */
    public Object getControl( Object aModel, Class aInterfaceClass ) throws com.sun.star.uno.Exception
    {
        XControlModel xModel = (XControlModel)UnoRuntime.queryInterface( XControlModel.class, aModel );
        return UnoRuntime.queryInterface( aInterfaceClass, getControl( xModel ) );
    }

    /* ------------------------------------------------------------------ */
    /** retrieves the form controller for a given logical form
     */
    public XFormController getFormController( XForm _form )
    {
        XFormLayerAccess formLayerAccess = (XFormLayerAccess)query( XFormLayerAccess.class );
        return formLayerAccess.getFormController( _form );
    }

    /* ------------------------------------------------------------------ */
    /** retrieves the form controller for a given logical form
     */
    public XFormController getFormController( Object _form )
    {
        return getFormController( UnoRuntime.queryInterface( XForm.class, _form ));
    }

    /* ------------------------------------------------------------------ */
    /** toggles the design mode of the form layer of active view of our sample document
    */
    protected void toggleFormDesignMode( ) throws java.lang.Exception
    {
        if ( m_document instanceof XMLDocument )
            dispatch( ".uno:SwitchXFormsDesignMode" );
        else
            dispatch( ".uno:SwitchControlDesignMode" );
    }

    /* ------------------------------------------------------------------ */
    /** sets the focus to a specific control
        @param xModel
            a control model. The focus is set to that control which is part of our view
            and associated with the given model.
    */
    public void grabControlFocus( Object xModel ) throws com.sun.star.uno.Exception
    {
        // look for the control from the current view which belongs to the model
        XControl xControl = getControl( xModel );

        // the focus can be set to an XWindow only
        XWindow xControlWindow = (XWindow)UnoRuntime.queryInterface( XWindow.class,
            xControl );

        // grab the focus
        xControlWindow.setFocus();
    }

    /* ------------------------------------------------------------------ */
    /** sets the focus to the first control
    */
    protected void grabControlFocus( ) throws java.lang.Exception
    {
        // the forms container of our document
        XIndexContainer xForms = dbfTools.queryIndexContainer( m_document.getFormComponentTreeRoot( ) );
        // the first form
        XIndexContainer xForm = dbfTools.queryIndexContainer( xForms.getByIndex( 0 ) );

        // the first control model which is no FixedText (FixedText's can't have the focus)
        for ( int i = 0; i<xForm.getCount(); ++i )
        {
            XPropertySet xControlProps = dbfTools.queryPropertySet( xForm.getByIndex( i ) );
            if ( FormComponentType.FIXEDTEXT != ((Short)xControlProps.getPropertyValue( "ClassId" )).shortValue() )
            {
                XControlModel xControlModel = (XControlModel)UnoRuntime.queryInterface(
                    XControlModel.class, xControlProps );
                // set the focus to this control
                grabControlFocus( xControlModel );
                // outta here
                break;
            }
        }

        // Note that we simply took the first control model from the hierarchy. This does state nothing
        // about the location of the respective control in the view. A control model is tied to a control
        // shape, and the shapes are where the geometry information such as position and size is hung up.
        // So you could easily have a document where the first control model is bound to a shape which
        // has a greater ordinate than any other control model.
    }
};

