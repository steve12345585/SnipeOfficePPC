#*************************************************************************
#
#  The Contents of this file are made available subject to the terms of
#  the BSD license.
#  
#  Copyright 2000, 2010 Oracle and/or its affiliates.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. Neither the name of Sun Microsystems, Inc. nor the names of its
#     contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
#  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
#  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
#  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
#  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
#  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
#  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
#  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#     
#**************************************************************************

# Builds the Java dialog component example of the SDK.

PRJ=../../../..
SETTINGS=$(PRJ)/settings

include $(SETTINGS)/settings.mk
include $(SETTINGS)/std.mk
include $(SETTINGS)/dk.mk

# Define non-platform/compiler specific settings

# we use the sample directory name dor separating this example 
# from others in the output directory
SAMPLE_NAME=DialogComponentSample
SAMPLE_CLASS_OUT=$(OUT_CLASS)/$(SAMPLE_NAME)
SAMPLE_GEN_OUT=$(OUT_MISC)/$(SAMPLE_NAME)

COMP_NAME=DialogComponent
COMP_CLASS_OUT=$(SAMPLE_CLASS_OUT)/$(COMP_NAME)
COMP_GEN_OUT=$(SAMPLE_GEN_OUT)/$(COMP_NAME)
COMP_RDB_NAME=$(COMP_NAME).uno.rdb
COMP_RDB=$(COMP_GEN_OUT)/$(COMP_RDB_NAME)
COMP_PACKAGE=$(OUT_BIN)/$(COMP_NAME).$(UNOOXT_EXT)
COMP_PACKAGE_URL=$(subst \\,\,"$(COMP_PACKAGE_DIR)$(PS)$(COMP_NAME).$(UNOOXT_EXT)")
COMP_JAR_NAME=$(COMP_NAME).uno.jar
COMP_JAR=$(SAMPLE_CLASS_OUT)/$(COMP_JAR_NAME)
COMP_JAR_MANIFEST=$(COMP_GEN_OUT)/$(COMP_NAME).uno.Manifest
COMP_UNOPKG_MANIFEST = $(COMP_GEN_OUT)/META-INF/manifest.xml
COMP_REGISTERFLAG=$(COMP_GEN_OUT)$(PS)java_$(COMP_NAME)_register_component.flag
COMP_COMPONENTS=$(COMP_NAME).components

IDLFILES = XTestDialogHandler.idl \
	TestDialogHandler.idl

# normally the idl file should be stored in a directory tree fitting the module structure,
# for the example we know the module structure
PACKAGE = com/sun/star/test
#PACKAGE = org/openoffice

COMP_JAVAFILES  = DialogComponent.java
COMP_CLASSFILES = $(patsubst %.java,$(COMP_CLASS_OUT)/%.class,$(COMP_JAVAFILES))

GEN_CLASSFILES = $(patsubst %.idl,$(SAMPLE_CLASS_OUT)/$(PACKAGE)/%.class,$(IDLFILES))
GEN_TYPELIST = $(subst /,.,$(patsubst %.idl,-T$(PACKAGE)/% ,$(IDLFILES)))
GEN_URDFILES = $(patsubst %.idl,$(SAMPLE_GEN_OUT)/%.urd,$(IDLFILES))

# the generated types are necessary for the component and the application jar
GEN_CLASSFILENAMES = $(subst $(SAMPLE_CLASS_OUT)/,,$(GEN_CLASSFILES))

SDK_CLASSPATH = $(subst $(EMPTYSTRING) $(PATH_SEPARATOR),$(PATH_SEPARATOR),$(CLASSPATH)\
		$(PATH_SEPARATOR)$(SAMPLE_CLASS_OUT)\
		$(PATH_SEPARATOR)$(COMP_CLASS_OUT)\
		$(PATH_SEPARATOR)$(APP1_CLASS_OUT))


# Targets
.PHONY: ALL 
ALL : DialogComponentExample

include $(SETTINGS)/stdtarget.mk

$(COMP_GEN_OUT)/%.Manifest :
	-$(MKDIR) $(subst /,$(PS),$(@D))
	@echo UNO-Type-Path: $(basename $(notdir $*)).uno.jar> $@
	@echo RegistrationClassName: $(basename $*)>> $@

$(SAMPLE_GEN_OUT)/%.urd : %.idl
	-$(MKDIR) $(subst /,$(PS),$(@D))
	$(IDLC) -C -I. -I$(IDL_DIR) -O$(SAMPLE_GEN_OUT) $<

$(COMP_GEN_OUT)/%.rdb : $(GEN_URDFILES)
	-$(DEL) $(subst \\,\,$(subst /,$(PS),$@))
	-$(MKDIR) $(subst /,$(PS),$(@D))
	$(REGMERGE) $@ /UCR $(GEN_URDFILES)

$(GEN_CLASSFILES) : $(COMP_RDB)
	-$(MKDIR) $(subst /,$(PS),$(@D))
	$(JAVAMAKER) -BUCR -nD $(GEN_TYPELIST) -O$(SAMPLE_CLASS_OUT) $(COMP_RDB) -X$(URE_TYPES) -X$(OFFICE_TYPES)

# component as well as application are dependent from the generated types
# rule for component class files
$(COMP_CLASS_OUT)/%.class : %.java $(GEN_CLASSFILES)
	-$(MKDIR) $(subst /,$(PS),$(@D))
	$(SDK_JAVAC) $(JAVAC_FLAGS) -classpath "$(SDK_CLASSPATH)" -d $(COMP_CLASS_OUT) $<

# rule for component jar file
$(COMP_JAR) : $(COMP_JAR_MANIFEST) $(COMP_CLASSFILES)
	-$(DEL) $(subst \\,\,$(subst /,$(PS),$@))
	-$(MKDIR) $(subst /,$(PS),$(@D))
	$(SDK_JAR) cvfm $@  $< -C $(COMP_CLASS_OUT) .
	+cd $(subst /,$(PS),$(SAMPLE_CLASS_OUT)) && $(SDK_JAR) uvf $(@F) $(GEN_CLASSFILENAMES)

# rule for component package manifest
$(COMP_GEN_OUT)/%/manifest.xml :
	-$(MKDIR) $(subst /,$(PS),$(@D))
	@echo $(OSEP)?xml version="$(QM)1.0$(QM)" encoding="$(QM)UTF-8$(QM)"?$(CSEP) > $@
	@echo $(OSEP)!DOCTYPE manifest:manifest PUBLIC "$(QM)-//OpenOffice.org//DTD Manifest 1.0//EN$(QM)" "$(QM)Manifest.dtd$(QM)"$(CSEP) >> $@
	@echo $(OSEP)manifest:manifest xmlns:manifest="$(QM)http://openoffice.org/2001/manifest$(QM)"$(CSEP) >> $@
	@echo $(SQM)  $(SQM)$(OSEP)manifest:file-entry manifest:media-type="$(QM)application/vnd.sun.star.uno-typelibrary;type=RDB$(QM)" >> $@
	@echo $(SQM)                       $(SQM)manifest:full-path="$(QM)$(subst /META-INF,,$(subst $(SAMPLE_GEN_OUT)/,,$(@D))).uno.rdb$(QM)"/$(CSEP) >> $@
	@echo $(SQM)  $(SQM)$(OSEP)manifest:file-entry manifest:media-type="$(QM)application/vnd.sun.star.uno-components$(QM)">> $@
	@echo $(SQM)                       $(SQM)manifest:full-path="$(QM)$(COMP_COMPONENTS)$(QM)"/$(CSEP)>> $@
	@echo $(OSEP)/manifest:manifest$(CSEP) >> $@

# rule for component package file
$(COMP_PACKAGE) : $(COMP_RDB) $(COMP_JAR) $(COMP_UNOPKG_MANIFEST) $(COMP_COMPONENTS)
	-$(DEL) $(subst \\,\,$(subst /,$(PS),$@))
	-$(MKDIR) $(subst /,$(PS),$(@D))
	$(SDK_ZIP) $@ $(COMP_COMPONENTS)
	cd $(subst /,$(PS),$(COMP_GEN_OUT)) && $(SDK_ZIP) -u ../../../bin/$(@F) $(COMP_RDB_NAME)
	cd $(subst /,$(PS),$(SAMPLE_CLASS_OUT)) && $(SDK_ZIP) -u ../../bin/$(@F) $(COMP_JAR_NAME)
	cd $(subst /,$(PS),$(COMP_GEN_OUT)) && $(SDK_ZIP) -u ../../../bin/$(@F) META-INF/manifest.xml

$(COMP_REGISTERFLAG) : $(COMP_PACKAGE)
ifeq "$(SDK_AUTO_DEPLOYMENT)" "YES"
	-$(DEL) $(subst \\,\,$(subst /,$(PS),$@))
	-$(MKDIR) $(subst /,$(PS),$(@D))
	$(DEPLOYTOOL) $(COMP_PACKAGE_URL)
	@echo flagged > $(subst /,$(PS),$@)
else
	@echo --------------------------------------------------------------------------------
	@echo  If you want to install your component automatically, please set the environment
	@echo  variable SDK_AUTO_DEPLOYMENT = YES. But note that auto deployment is only 
	@echo  possible if no office instance is running. 
	@echo --------------------------------------------------------------------------------
endif

DialogComponentExample : $(COMP_REGISTERFLAG)
# TODO: adapt
	@echo --------------------------------------------------------------------------------
	@echo The PropTest was installed if SDK_AUTO_DEPLOYMENT = YES.
	@echo Load the "$(QM)PropertySet.odt$(QM)" document to see how this component works. You can
	@echo use this component inside your office installation, see the example description.
	@echo -
	@echo $(MAKE) DialogComponent.odt.load
	@echo --------------------------------------------------------------------------------

DialogComponent.odt.load : $(COMP_REGISTERFLAG)
	"$(OFFICE_PROGRAM_PATH)$(PS)soffice" $(basename $@)

.PHONY: clean
clean :
	-$(DELRECURSIVE) $(subst /,$(PS),$(SAMPLE_CLASS_OUT))
	-$(DELRECURSIVE) $(subst /,$(PS),$(SAMPLE_GEN_OUT))
	-$(DEL) $(subst \\,\,$(subst /,$(PS),$(COMP_PACKAGE_URL)))
