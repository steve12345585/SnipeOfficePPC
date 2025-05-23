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

PRJ=..$/..
PRJNAME=odk
TARGET=cli

#----------------------------------------------------------------
.INCLUDE: settings.mk
.INCLUDE: $(PRJ)$/util$/makefile.pmk
#----------------------------------------------------------------

#----------------------------------------------------
# this makefile is only used for copying the example 
# files into the SDK
#----------------------------------------------------

CSHARP=\
    $(DESTDIRCLIEXAMPLES)$/CSharp$/Spreadsheet$/GeneralTableSample.cs \
    $(DESTDIRCLIEXAMPLES)$/CSharp$/Spreadsheet$/SpreadsheetDocHelper.cs \
    $(DESTDIRCLIEXAMPLES)$/CSharp$/Spreadsheet$/SpreadsheetSample.cs \
    $(DESTDIRCLIEXAMPLES)$/CSharp$/Spreadsheet$/ViewSample.cs \
    $(DESTDIRCLIEXAMPLES)$/CSharp$/Spreadsheet$/Makefile

VBDOTNET=\
    $(DESTDIRCLIEXAMPLES)$/VB.NET$/WriterDemo$/WriterDemo.vb \
    $(DESTDIRCLIEXAMPLES)$/VB.NET$/WriterDemo$/Makefile


DIR_FILE_LIST= \
    $(CSHARP) \
    $(VBDOTNET)

DIR_DIRECTORY_LIST=$(uniq $(DIR_FILE_LIST:d))
DIR_CREATE_FLAG=$(MISC)$/ex_$(TARGET)_dirs_created.txt
DIR_FILE_FLAG=$(MISC)$/ex_$(TARGET)_files.txt

#--------------------------------------------------
# TARGETS
#--------------------------------------------------
all : \
    $(DIR_FILE_FLAG)

#--------------------------------------------------
# use global rules
#--------------------------------------------------   
.INCLUDE: $(PRJ)$/util$/odk_rules.pmk

