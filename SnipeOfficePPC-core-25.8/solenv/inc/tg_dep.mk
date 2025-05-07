#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2000, 2010 Oracle and/or its affiliates.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# This file is part of OpenOffice.org.
#
# OpenOffice.org is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# OpenOffice.org is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.  If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************

.IF "$(SLOFILES)$(OBJFILES)$(DEPOBJFILES)$(SRCFILES)$(SRC1FILES)$(SRC2FILES)$(SRC3FILES)$(RCFILES)$(HDBDEPNTARGET)$(IDLFILES)$(PARFILES)$(ZIP1TARGET)$(ZIP2TARGET)$(ZIP3TARGET)$(ZIP4TARGET)$(ZIP5TARGET)$(ZIP6TARGET)$(ZIP7TARGET)$(ZIP8TARGET)$(ZIP9TARGET)$(COMP1TYPELIST)$(COMP2TYPELIST)$(COMP3TYPELIST)$(COMP4TYPELIST)$(COMP5TYPELIST)$(COMP6TYPELIST)$(COMP7TYPELIST)$(COMP8TYPELIST)$(COMP9TYPELIST)"!=""
ALLDEP .PHONY: 
    @-$(IFEXIST) $(SRS)/$(TARGET).*.dpr $(THEN) $(RM:s/+//) $(SRS)/$(TARGET).*.dpr >& $(NULLDEV) $(FI)
    @@-$(RM) $(MISC)/$(TARGET).dpr
    @@-$(RM) $(MISC)/$(TARGET).*.dprr
    @@-$(RM) $(MISC)/$(TARGET).dpj
    @@-$(RM) $(MISC)/$(TARGET).dpz
    @@-$(RM) $(MISC)/$(COMP1TYPELIST).mk $(MISC)/$(COMP2TYPELIST).mk $(MISC)/$(COMP3TYPELIST).mk $(MISC)/$(COMP4TYPELIST).mk $(MISC)/$(COMP5TYPELIST).mk $(MISC)/$(COMP6TYPELIST).mk $(MISC)/$(COMP7TYPELIST).mk $(MISC)/$(COMP8TYPELIST).mk $(MISC)/$(COMP9TYPELIST).mk
.IF "$(nodep)"==""
.IF "$(DEPFILE_SLO)"!=""	
    @@-$(RM) $(DEPFILE_SLO)
    $(foreach,i,$(all_local_slo) $(shell @$(MAKEDEPEND) @$(mktmp -f - -p$(SLO) $(MKDEPFLAGS) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $i ) > $(MISC)/s_$(i:b).dpcc ))
    $(foreach,i,$(all_misc_slo) $(shell @$(MAKEDEPEND) @$(mktmp -f - -p$(SLO) $(MKDEPFLAGS) $(CDEFS) $(CDEFSSLO) $(CDEFSMT) $i ) > $(MISC)/s_$(i:b).dpcc ))
.ENDIF			# "$(DEPFILE_SLO)"!=""	
.IF "$(DEPFILE_OBJ)"!=""	
    @@-$(RM) $(DEPFILE_OBJ)
    $(foreach,i,$(all_local_obj) $(shell @$(MAKEDEPEND) @$(mktmp -f - -p$(OBJ) $(MKDEPFLAGS) $(CDEFS) $(CDEFSOBJ) $(CDEFSMT) $i ) > $(MISC)/o_$(i:b).dpcc ))
    $(foreach,i,$(all_misc_obj) $(shell @$(MAKEDEPEND) @$(mktmp -f - -p$(OBJ) $(MKDEPFLAGS) $(CDEFS) $(CDEFSOBJ) $(CDEFSMT) $i ) > $(MISC)/o_$(i:b).dpcc ))
.ENDIF			# "$(DEPFILE_OBJ)"!=""	
.ENDIF			# "$(nodep)"==""
.IF "$(DEPFILES)" != ""
    echo xxx$(DEPFILES)xxx
    @@-$(RM) $(DEPFILES)
.ENDIF			# "$(DEPFILES)" != ""


ALLDPC: \
        $(DEPFILES) \
        $(RCFILES) \
        $(IDLFILES) \
        $(HDBDEPNTARGET)
.IF "$(nodep)"!=""
    @echo NOT-Making: Depend-Lists
    @echo ttt: ppp > $(MISC)/$(TARGET).dpc
.ELSE
    @echo Making : Dependencies 
.IF "$(CFILES)$(CXXFILES)"!=""
.IF "$(SLOFILES)$(OBJFILES)$(DEPOBJFILES)"==""
    @echo ++++++++++++++++++++++++++++++++++++++++++++++++
    @echo 		something is wrong!!
    @echo 		source files but no obj
    @echo ++++++++++++++++++++++++++++++++++++++++++++++++
    force_dmake_to_error
.ENDIF
.ENDIF
.IF "$(HDBDEPNTARGET)$(OBJFILES)$(SLOFILES)$(DEPOBJFILES)$(RCFILES)$(PARFILES)"!=""
    @$(IFEXIST) $(MISC)/$(TARGET).dpj $(THEN) $(RM:s/+//) $(MISC)/$(TARGET).dpj >& $(NULLDEV) $(FI)
    @$(IFEXIST) $(MISC)/genjava.mk $(THEN) $(RM:s/+//) $(MISC)/genjava.mk >& $(NULLDEV) $(FI)
    $(TOUCH) $(MISC)/$(TARGET).dpc
.IF "$($(SECOND_BUILD)_SLOFILES)"!=""
    @$(TYPE) $(mktmp $(foreach,i,$($(SECOND_BUILD)_SLOFILES) $(i:d:^"\n")$(SECOND_BUILD)_$(i:f) : $i )) >> $(MISC)/$(TARGET).dpc
.ENDIF
.IF "$($(SECOND_BUILD)_OBJFILES)"!=""
    @$(TYPE) $(mktmp $(foreach,i,$($(SECOND_BUILD)_OBJFILES) $(i:d:^"\n")$(SECOND_BUILD)_$(i:f) : $(i:d:s/obj/slo/)$(i:b).obj )) >> $(MISC)/$(TARGET).dpc
.ENDIF
.ELSE			# "$(HDBDEPNTARGET)$(OBJFILES)$(SLOFILES)$(DEPOBJFILES)$(RCFILES)"!=""
    @echo ttt: ppp > $(MISC)/$(TARGET).dpc
.ENDIF			# "$(HDBDEPNTARGET)$(OBJFILES)$(SLOFILES)$(DEPOBJFILES)$(RCFILES)"!=""
.ENDIF			# "$(nodep)"!=""
.ELSE		# anything requiring dependencies

ALLDPC:
    @echo $(EMQ)# > $(MISC)/$(TARGET).dpc

ALLDEP:
    @echo nothing to do here...

.ENDIF

