/*************************************************************************
 *
 *  The Contents of this file are made available subject to the terms of
 *  the BSD license.
 *
 *  Copyright 2000, 2010 Oracle and/or its affiliates.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of Sun Microsystems, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 *  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 *  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 *  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *************************************************************************/

// __________ Imports __________

import java.util.Random;

// base classes
import com.sun.star.uno.XInterface;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;
import com.sun.star.lang.*;

// factory for creating components
import com.sun.star.comp.servicemanager.ServiceManager;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.bridge.XUnoUrlResolver;
import com.sun.star.uno.XNamingService;
import com.sun.star.frame.XDesktop;
import com.sun.star.frame.XComponentLoader;

// property access
import com.sun.star.beans.*;

// container access
import com.sun.star.container.*;

// application specific classes
import com.sun.star.sheet.XSpreadsheetDocument;
import com.sun.star.text.XTextDocument;

import com.sun.star.document.XEmbeddedObjectSupplier;
import com.sun.star.frame.XModel;
import com.sun.star.frame.XController;

// Exceptions
import com.sun.star.uno.RuntimeException;
import com.sun.star.container.NoSuchElementException;
import com.sun.star.beans.UnknownPropertyException;
import com.sun.star.lang.IndexOutOfBoundsException;

// __________ Implementation __________

/** Helper for creating a calc document adding cell values and charts
    @author Bj&ouml;rn Milcke
 */
public class Helper
{
    public Helper( String[] args )
    {
        // connect to a running office and get the ServiceManager
        try {
            // get the remote office component context
            maContext = com.sun.star.comp.helper.Bootstrap.bootstrap();
            System.out.println("Connected to a running office ...");

            // get the remote office service manager
            maMCFactory = maContext.getServiceManager();
        }
        catch( Exception e) {
            System.out.println( "Couldn't get ServiceManager: " + e );
            e.printStackTrace();
            System.exit(1);
        }
    }

    // ____________________

    public XSpreadsheetDocument createSpreadsheetDocument()
    {
        return (XSpreadsheetDocument) UnoRuntime.queryInterface(
            XSpreadsheetDocument.class, createDocument( "scalc" ));
    }

    // ____________________

    public XModel createPresentationDocument()
    {
        return createDocument( "simpress" );
    }

    // ____________________

    public XModel createDrawingDocument()
    {
        return createDocument( "sdraw" );
    }

    // ____________________

    public XModel createTextDocument()
    {
        return createDocument( "swriter" );
    }

    // ____________________

    public XModel createDocument( String sDocType )
    {
        XModel aResult = null;
        try
        {
            XComponentLoader aLoader = (XComponentLoader)
                UnoRuntime.queryInterface(XComponentLoader.class,
                maMCFactory.createInstanceWithContext("com.sun.star.frame.Desktop",
                                                      maContext) );

            aResult = (XModel) UnoRuntime.queryInterface(
                XModel.class,
                aLoader.loadComponentFromURL( "private:factory/" + sDocType,
                                              "_blank",
                                              0,
                                              new PropertyValue[ 0 ] ) );
        }
        catch( Exception e )
        {
            System.err.println("Couldn't create Document of type "+ sDocType +": "+e);
            e.printStackTrace();
            System.exit( 0 );
        }

        return aResult;
    }

    public XComponentContext getComponentContext(){
        return maContext;

    }

    // __________ private members and methods __________

    private final String  msDataSheetName  = "Data";
    private final String  msChartSheetName = "Chart";
    private final String  msChartName      = "SampleChart";

    private XComponentContext      maContext;
    private XMultiComponentFactory maMCFactory;
    private XSpreadsheetDocument   maSpreadSheetDoc;
}
