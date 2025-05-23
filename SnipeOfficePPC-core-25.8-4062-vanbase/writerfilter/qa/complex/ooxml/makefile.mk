#
# This file is part of the SnipeOffice project.
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

PRJ = ..$/..$/..
TARGET  = PropertyValues
PRJNAME = $(TARGET)
PACKAGE = complex$/ooxml

# --- Settings -----------------------------------------------------
.INCLUDE: settings.mk


#----- compile .java files -----------------------------------------

JARFILES = mysql.jar ridl.jar unoil.jar jurt.jar juh.jar java_uno.jar OOoRunner.jar
JAVAFILES       = LoadDocuments.java
JAVACLASSFILES  = $(foreach,i,$(JAVAFILES) $(CLASSDIR)$/$(PACKAGE)$/$(i:b).class)

#----- make a jar from compiled files ------------------------------

MAXLINELENGTH = 100000

JARCLASSDIRS    = $(PACKAGE)
JARTARGET       = $(TARGET).jar
JARCOMPRESS     = TRUE

# --- Parameters for the test --------------------------------------

# start an office if the parameter is set for the makefile
.IF "$(OFFICE)" == ""
CT_APPEXECCOMMAND =
.ELSE
CT_APPEXECCOMMAND = -AppExecutionCommand "$(OFFICE)$/soffice --accept=socket,host=localhost,port=8100;urp;"
.ENDIF

# test base is java complex
CT_TESTBASE = -TestBase java_complex

# build package name with "." instead of $/
CT_PACKAGE  = -o $(PACKAGE:s\$/\.\)

# start the runner application
CT_APP      = org.openoffice.Runner

# --- Targets ------------------------------------------------------

.IF "$(depend)" == ""
ALL: ALLTAR
.ELSE
ALL: ALLDEP
.ENDIF

.INCLUDE :  target.mk

run: \
    LoadDocuments

RUN: run

LoadDocuments:
    +java -cp $(CLASSPATH) $(CT_APP) $(CT_APPEXECCOMMAND) $(CT_TESTBASE) -tdoc $(PWD)$/testdocuments $(CT_PACKAGE).LoadDocuments

