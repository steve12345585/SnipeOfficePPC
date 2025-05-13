#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file incorporates work covered by the following license notice:
#
#   Licensed to the Apache Software Foundation (ASF) under one or more
#   contributor license agreements. See the NOTICE file distributed
#   with this work for additional information regarding copyright
#   ownership. The ASF licenses this file to you under the Apache
#   License, Version 2.0 (the "License"); you may not use this file
#   except in compliance with the License. You may obtain a copy of
#   the License at http://www.apache.org/licenses/LICENSE-2.0 .
#

PRJ=..$/..
PRJNAME=odk
TARGET=copying

#----------------------------------------------------------------
.INCLUDE: settings.mk
.INCLUDE: $(PRJ)$/util$/makefile.pmk
#----------------------------------------------------------------

#----------------------------------------------------
# this makefile is only used for copying the example 
# files into the SDK
#----------------------------------------------------

DRAWING_FILES= \
    $(DESTDIRJAVAEXAMPLES)$/Drawing$/SDraw.java \
    $(DESTDIRJAVAEXAMPLES)$/Drawing$/Makefile

SPREADSHEET_FILES= \
    $(DESTDIRJAVAEXAMPLES)$/Spreadsheet$/SCalc.java		   	\
    $(DESTDIRJAVAEXAMPLES)$/Spreadsheet$/EuroAdaption.java		\
    $(DESTDIRJAVAEXAMPLES)$/Spreadsheet$/ChartTypeChange.java   	\
    $(DESTDIRJAVAEXAMPLES)$/Spreadsheet$/CalcAddins.java	   	\
    $(DESTDIRJAVAEXAMPLES)$/Spreadsheet$/CalcAddins.ods	   	\
    $(DESTDIRJAVAEXAMPLES)$/Spreadsheet$/XCalcAddins.idl	   	\
    $(DESTDIRJAVAEXAMPLES)$/Spreadsheet$/Makefile

TEXT_FILES= \
    $(DESTDIRJAVAEXAMPLES)$/Text$/SWriter.java	   		\
    $(DESTDIRJAVAEXAMPLES)$/Text$/StyleInitialization.java 	\
    $(DESTDIRJAVAEXAMPLES)$/Text$/TextDocumentStructure.java 	\
    $(DESTDIRJAVAEXAMPLES)$/Text$/TextReplace.java 		\
    $(DESTDIRJAVAEXAMPLES)$/Text$/BookmarkInsertion.java 	\
    $(DESTDIRJAVAEXAMPLES)$/Text$/HardFormatting.java 	\
    $(DESTDIRJAVAEXAMPLES)$/Text$/StyleCreation.java  	\
    $(DESTDIRJAVAEXAMPLES)$/Text$/WriterSelector.java  	\
    $(DESTDIRJAVAEXAMPLES)$/Text$/GraphicsInserter.java  	\
    $(DESTDIRJAVAEXAMPLES)$/Text$/oo_smiley.gif  	\
    $(DESTDIRJAVAEXAMPLES)$/Text$/Makefile

DOCUMENTHANDLING_FILES= \
    $(DESTDIRJAVAEXAMPLES)$/DocumentHandling$/DocumentConverter.java	\
    $(DESTDIRJAVAEXAMPLES)$/DocumentHandling$/DocumentLoader.java	\
    $(DESTDIRJAVAEXAMPLES)$/DocumentHandling$/DocumentPrinter.java	\
    $(DESTDIRJAVAEXAMPLES)$/DocumentHandling$/DocumentSaver.java	\
    $(DESTDIRJAVAEXAMPLES)$/DocumentHandling$/Makefile \
    $(DESTDIRJAVAEXAMPLES)$/DocumentHandling$/test$/test1.odt

CONVERTERSERVLET_FILES= \
    $(DESTDIRJAVAEXAMPLES)$/ConverterServlet$/ConverterServlet.html	\
    $(DESTDIRJAVAEXAMPLES)$/ConverterServlet$/ConverterServlet.java	\
    $(DESTDIRJAVAEXAMPLES)$/ConverterServlet$/web.xml	\
    $(DESTDIRJAVAEXAMPLES)$/ConverterServlet$/Makefile			\
    $(DESTDIRJAVAEXAMPLES)$/ConverterServlet$/README

EMBEDDEDOBJECT_FILES= \
    $(DESTDIRJAVAEXAMPLES)$/EmbedDocument$/EmbeddedObject$/EmbeddedObject.xcu \
    $(DESTDIRJAVAEXAMPLES)$/EmbedDocument$/EmbeddedObject$/OwnEmbeddedObject.java \
    $(DESTDIRJAVAEXAMPLES)$/EmbedDocument$/EmbeddedObject$/EditorFrame.java  \
    $(DESTDIRJAVAEXAMPLES)$/EmbedDocument$/EmbeddedObject$/Makefile \
    $(DESTDIRJAVAEXAMPLES)$/EmbedDocument$/EmbeddedObject$/OwnEmbeddedObjectFactory.java

INSPECTOR_FILES= \
    $(DESTDIRJAVAEXAMPLES)$/Inspector$/Inspector.jpg \
    $(DESTDIRJAVAEXAMPLES)$/Inspector$/InspectorThumbnail.jpg \
    $(DESTDIRJAVAEXAMPLES)$/Inspector$/Inspector.uno.xml \
    $(DESTDIRJAVAEXAMPLES)$/Inspector$/Inspector.java \
    $(DESTDIRJAVAEXAMPLES)$/Inspector$/Makefile \
    $(DESTDIRJAVAEXAMPLES)$/Inspector$/TestInspector.java \
    $(DESTDIRJAVAEXAMPLES)$/Inspector$/XInstanceInspector.idl \
    $(DESTDIRJAVAEXAMPLES)$/Inspector$/InstanceInspector.idl \
    $(DESTDIRJAVAEXAMPLES)$/Inspector$/InstanceInspectorTest.odt

NOTESACCESS_FILES= \
    $(DESTDIRJAVAEXAMPLES)$/NotesAccess$/NotesAccess.java \
    $(DESTDIRJAVAEXAMPLES)$/NotesAccess$/Makefile \
    $(DESTDIRJAVAEXAMPLES)$/NotesAccess$/Stocks.nsf

TODO_FILES= \
    $(DESTDIRJAVAEXAMPLES)$/ToDo$/ToDo.java \
    $(DESTDIRJAVAEXAMPLES)$/ToDo$/ToDo.ods  \
    $(DESTDIRJAVAEXAMPLES)$/ToDo$/ToDo.uno.xml  \
    $(DESTDIRJAVAEXAMPLES)$/ToDo$/ToDo.idl  \
    $(DESTDIRJAVAEXAMPLES)$/ToDo$/XToDo.idl  \
    $(DESTDIRJAVAEXAMPLES)$/ToDo$/Makefile

MINIMALCOMPONENT_FILES= \
    $(DESTDIRJAVAEXAMPLES)$/MinimalComponent$/MinimalComponent.idl \
    $(DESTDIRJAVAEXAMPLES)$/MinimalComponent$/MinimalComponent.java\
    $(DESTDIRJAVAEXAMPLES)$/MinimalComponent$/MinimalComponent.uno.xml \
    $(DESTDIRJAVAEXAMPLES)$/MinimalComponent$/TestMinimalComponent.java\
    $(DESTDIRJAVAEXAMPLES)$/MinimalComponent$/Makefile

PROPERTYSET_FILES= \
    $(DESTDIRJAVAEXAMPLES)$/PropertySet$/PropTest.java \
    $(DESTDIRJAVAEXAMPLES)$/PropertySet$/PropertySet.odt \
    $(DESTDIRJAVAEXAMPLES)$/PropertySet$/Makefile

DIR_FILE_LIST= \
    $(DRAWING_FILES) \
    $(SPREADSHEET_FILES) \
    $(TEXT_FILES) \
    $(DOCUMENTHANDLING_FILES) \
    $(CONVERTERSERVLET_FILES) \
    $(EMBEDDEDOBJECT_FILES) \
    $(INSPECTOR_FILES) \
    $(NOTESACCESS_FILES) \
    $(TODO_FILES) \
    $(MINIMALCOMPONENT_FILES) \
    $(PROPERTYSET_FILES)

DIR_DIRECTORY_LIST=$(uniq $(DIR_FILE_LIST:d))
DIR_CREATE_FLAG=$(MISC)$/ex_java_dirs_created.txt
DIR_FILE_FLAG=$(MISC)$/ex_java_files.txt

#--------------------------------------------------
# TARGETS
#--------------------------------------------------
all : \
    $(DIR_FILE_LIST) \
    $(DIR_FILE_FLAG)

#--------------------------------------------------
# use global rules
#--------------------------------------------------   
.INCLUDE: $(PRJ)$/util$/odk_rules.pmk


