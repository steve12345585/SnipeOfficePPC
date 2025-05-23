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
TARGET=copying

#----------------------------------------------------------------
.INCLUDE: settings.mk
.INCLUDE: $(PRJ)$/util$/makefile.pmk
#----------------------------------------------------------------

#----------------------------------------------------
# this makefile is only used for copying the example 
# files into the SDK
#----------------------------------------------------

DRAWING_FILES=\
    $(DESTDIRBASICEXAMPLES)$/drawing$/dirtree.txt \
    $(DESTDIRBASICEXAMPLES)$/drawing$/importexportofasciifiles.odg

FORMSANDCONTROLS_FILES=\
    $(DESTDIRBASICEXAMPLES)$/forms_and_controls$/beef.wmf \
    $(DESTDIRBASICEXAMPLES)$/forms_and_controls$/burger.wmf \
    $(DESTDIRBASICEXAMPLES)$/forms_and_controls$/burger_factory.odt \
    $(DESTDIRBASICEXAMPLES)$/forms_and_controls$/chicken.wmf \
    $(DESTDIRBASICEXAMPLES)$/forms_and_controls$/fish.wmf \
    $(DESTDIRBASICEXAMPLES)$/forms_and_controls$/vegetable.wmf

SHEET_FILES=\
    $(DESTDIRBASICEXAMPLES)$/sheet$/adapting_to_euroland.ods

STOCKQUOTEUPDATER_FILES=\
    $(DESTDIRBASICEXAMPLES)$/stock_quotes_updater$/stock.ods

TEXT_FILES=\
    $(DESTDIRBASICEXAMPLES)$/text$/creating_an_index$/index.odt \
    $(DESTDIRBASICEXAMPLES)$/text$/creating_an_index$/indexlist.txt \
    $(DESTDIRBASICEXAMPLES)$/text$/modifying_text_automatically$/changing_appearance.odt \
    $(DESTDIRBASICEXAMPLES)$/text$/modifying_text_automatically$/inserting_bookmarks.odt \
    $(DESTDIRBASICEXAMPLES)$/text$/modifying_text_automatically$/replacing_text.odt \
    $(DESTDIRBASICEXAMPLES)$/text$/modifying_text_automatically$/using_regular_expressions.odt

DIR_FILE_LIST= \
    $(DRAWING_FILES) \
    $(FORMSANDCONTROLS_FILES) \
    $(SHEET_FILES) \
    $(STOCKQUOTEUPDATER_FILES) \
    $(TEXT_FILES) \

DIR_DIRECTORY_LIST=$(uniq $(DIR_FILE_LIST:d))
DIR_CREATE_FLAG=$(MISC)$/ex_basic_dirs_created.txt
DIR_FILE_FLAG=$(MISC)$/ex_basic_files.txt

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
    

