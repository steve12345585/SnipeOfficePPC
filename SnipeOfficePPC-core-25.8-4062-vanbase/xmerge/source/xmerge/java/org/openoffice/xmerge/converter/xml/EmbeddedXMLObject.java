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

package org.openoffice.xmerge.converter.xml;

import java.io.IOException;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Document;
import org.w3c.dom.DOMException;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

import org.xml.sax.SAXException;

/**
 * This class represents those embedded objects in an OpenOffice.org document
 * that have an XML representation.  Currently, according to the OpenOffice.org
 * File Format 1.0 document, there are 6 such objects:
 *
 *      Formulae created with Math              (application/vnd.sun.xml.math)
 *      Charts created with Chart               (application/vnd.sun.xml.chart)
 *      Spreadsheets created with Calc          (application/vnd.sun.xml.calc)
 *      Text created with Writer                (application/vnd.sun.xml.writer)
 *      Drawings created with Draw              (application/vnd.sun.xml.draw)
 *      Presentations created with Impress      (application/vnd.sun.xml.impress)
 *
 * These object types are stored using a combination of content, settings and styles
 * XML files.
 */
public class EmbeddedXMLObject extends EmbeddedObject {

    // Entries for the subdocuments that constitute this object;
    protected Document contentDOM  = null;
    protected Document settingsDOM = null;
    protected Document stylesDOM   = null;

    private DocumentBuilder builder = null;

    /**
     * Constructor for an embedded object stored using an XML representation.
     *
     * @param   name    The name of the object.
     * @param   type    The mime-type of the object.  See the class summary.
     */
    public EmbeddedXMLObject(String name, String type) {
        super(name, type);
    }

    /**
     * Package private constructor for use when reading an object from a
     * compressed SX? file.
     *
     * @param   name    The name of the object.
     * @param   type    The mime-type of the object.  See the class summary.
     * @param   source  The OfficeZip representation of the SX? file that stores
     *                  the object.
     */
    EmbeddedXMLObject(String name, String type, OfficeZip source) {
        super(name, type, source);
    }


    /**
     * Returns the content data for this embedded object.
     *
     * @return DOM representation of "content.xml"
     *
     * @throws  SAXException    If any parser error occurs
     * @throws  IOException     If any IO error occurs
     */
    public Document getContentDOM() throws SAXException, IOException {

        if (contentDOM == null) {
            contentDOM = getNamedDOM("content.xml");
        }

        return contentDOM;
    }


    /**
     * Sets the content data for the embedded object.
     *
     * @param   content     DOM representation of the object's content.
     */
    public void setContentDOM(Document content) {
        contentDOM = content;
        hasChanged = true;
    }


    /**
     * Returns the settings data for this embedded object.
     *
     * @return DOM representation of "settings.xml"
     *
     * @throws  SAXException    If any parser error occurs
     * @throws  IOException     If any IO error occurs
     */
    public Document getSettingsDOM() throws SAXException, IOException {

        if (settingsDOM == null) {
            settingsDOM = getNamedDOM("settings.xml");
        }

        return settingsDOM;
    }


    /**
     * Sets the settings data for the embedded object.
     *
     * @param   settings     DOM representation of the object's styles.
     */
    public void setSettingsDOM(Document settings) {
        settingsDOM = settings;
        hasChanged = true;
    }


    /**
     * Returns the style data for this embedded object.
     *
     * @return DOM representation of "styles.xml"
     *
     * @throws  SAXException    If any parser error occurs
     * @throws  IOException     If any IO error occurs
     */
    public Document getStylesDOM() throws SAXException, IOException {

        if (stylesDOM == null) {
            stylesDOM = getNamedDOM("styles.xml");
        }

        return stylesDOM;
    }


    /**
     * Sets the styles data for the embedded object.
     *
     * @param   styles     DOM representation of the object's styles.
     */
    public void setStylesDOM(Document styles) {
        stylesDOM = styles;
        hasChanged = true;
    }


    /**
     * This method extracts the data for the given XML file from the SX? file
     * and creates a DOM representation of it.
     *
     * @param   name    The name of the XML file to retrieve.  It is paired with
     *                  the object name to access the SX? file.
     *
     * @return  DOM representation of the named XML file.
     *
     * @throws  SAXException    If any parser error occurs
     * @throws  IOException     If any IO error occurs
     */
    private Document getNamedDOM(String name) throws SAXException, IOException {
        if (zipFile == null) {
            return null;
        }

        try {
            if (builder == null) {
                DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();

                factory.setValidating(false);
                builder = factory.newDocumentBuilder();
            }

            byte[] data = zipFile.getNamedBytes(new String(objName + "/" + name));
            if (data != null) {
                return OfficeDocument.parse(builder, data);
            }
            else {
                return null;
            }

        }
        catch (SAXException se) {
            throw se;
        }
        catch (IOException ioe) {
            throw ioe;
        }
        catch (ParserConfigurationException pce) {
            throw new SAXException(pce);
        }
    }


    /**
     * Package private method for writing the data of the EmbeddedObject to a
     * SX? file.
     *
     * @param   zip     An <code>OfficeZip</code> instance representing the file
     *                  the data is to be written to.
     */
    void write(OfficeZip zip) throws IOException {
        if (hasChanged == true) {
            if (contentDOM != null) {
                zip.setNamedBytes(new String(objName + "/content.xml"),
                                        OfficeDocument.docToBytes(contentDOM));
            }
            if (settingsDOM != null) {
                zip.setNamedBytes(new String(objName + "/settings.xml"),
                                        OfficeDocument.docToBytes(settingsDOM));
            }
            if (stylesDOM != null) {
                zip.setNamedBytes(new String(objName + "/styles.xml"),
                                        OfficeDocument.docToBytes(stylesDOM));
            }
        }
    }

    /**
     * Package private method that constructs the manifest.xml entries for this
     * embedded object.
     *
     * @param  manifestDoc    <code>Document</code> containing the manifest entries.
     */
    void writeManifestData(Document manifestDoc) throws DOMException {
        Node root = manifestDoc.getDocumentElement();

        if (contentDOM != null) {
            Element contentNode = manifestDoc.createElement(OfficeConstants.TAG_MANIFEST_FILE);

            contentNode.setAttribute(OfficeConstants.ATTRIBUTE_MANIFEST_FILE_TYPE, "text/xml");
            contentNode.setAttribute(OfficeConstants.ATTRIBUTE_MANIFEST_FILE_PATH,
                                        new String(objName + "/content.xml"));

            root.appendChild(contentNode);
        }

        if (settingsDOM != null) {
            Element settingsNode = manifestDoc.createElement(OfficeConstants.TAG_MANIFEST_FILE);

            settingsNode.setAttribute(OfficeConstants.ATTRIBUTE_MANIFEST_FILE_TYPE, "text/xml");
            settingsNode.setAttribute(OfficeConstants.ATTRIBUTE_MANIFEST_FILE_PATH,
                                        new String(objName + "/settings.xml"));

            root.appendChild(settingsNode);
        }

        if (stylesDOM != null) {
            Element stylesNode = manifestDoc.createElement(OfficeConstants.TAG_MANIFEST_FILE);

            stylesNode.setAttribute(OfficeConstants.ATTRIBUTE_MANIFEST_FILE_TYPE, "text/xml");
            stylesNode.setAttribute(OfficeConstants.ATTRIBUTE_MANIFEST_FILE_PATH,
                                        new String(objName + "/styles.xml"));
        }


        Element objectNode = manifestDoc.createElement(OfficeConstants.TAG_MANIFEST_FILE);

        objectNode.setAttribute(OfficeConstants.ATTRIBUTE_MANIFEST_FILE_TYPE, objType);
        objectNode.setAttribute(OfficeConstants.ATTRIBUTE_MANIFEST_FILE_PATH,
                                    new String(objName + "/"));

        root.appendChild(objectNode);
    }

}
