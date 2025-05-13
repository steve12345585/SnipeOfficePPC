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

PRJ     = ..$/..$/..$/..
PRJNAME = filter
TARGET  = FCFGSplit
PACKAGE = com$/sun$/star$/filter$/config$/tools$/split

# --- Settings -----------------------------------------------------

.INCLUDE: settings.mk

#----- compile .java files -----------------------------------------

OWNCOPY         =   \
                    $(MISC)$/$(TARGET)_copied.done

JARFILES        =   \
                    ridl.jar        \
                    unoil.jar       \
                    jurt.jar        \
                    juh.jar         \
                    java_uno.jar

CFGFILES        =   \
                    FCFGSplit.cfg

JAVACLASSFILES  =   \
                    $(CLASSDIR)$/$(PACKAGE)$/SplitterData.class \
                    $(CLASSDIR)$/$(PACKAGE)$/Splitter.class     \
                    $(CLASSDIR)$/$(PACKAGE)$/FCFGSplit.class

CUSTOMMANIFESTFILE     =   \
                    Manifest.mf

MAXLINELENGTH   =   100000

#----- make a jar from compiled files ------------------------------

JARCLASSDIRS    =   \
                    com$/sun$/star$/filter$/config$/tools$/utils    \
                    com$/sun$/star$/filter$/config$/tools$/split

JARTARGET       =   $(TARGET).jar

JARCOMPRESS     =   TRUE

# --- targets -----------------------------------------------------

.INCLUDE :  target.mk

ALLTAR : $(OWNCOPY)

.IF "$(JARTARGETN)" != ""
$(JARTARGETN) : $(OWNCOPY)
.ENDIF

$(OWNCOPY) : $(CFGFILES)
    -$(MKDIR) $(CLASSDIR)$/$(PACKAGE)
    $(COPY) $? $(CLASSDIR)$/$(PACKAGE) && $(TOUCH) $@

run :
    @$(MKDIR) c:\temp\fragments
    @$(JAVA) -jar $(CLASSDIR)$/FCFGSplit.jar debug=4 xmlfile=o:/src680/src.m7/officecfg/registry/data/org/openoffice/Office/TypeDetection.xcu outdir=c:/temp/fragments
