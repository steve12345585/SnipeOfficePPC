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
# Builds the UCB examples of the Developers Guide.

PRJ=../../..
SETTINGS=$(PRJ)/settings

include $(SETTINGS)/settings.mk
include $(SETTINGS)/std.mk
include $(SETTINGS)/dk.mk

# Define non-platform/compiler specific settings
EXAMPLE_NAME=UCBExamples
OUT_APP_CLASS = $(OUT_CLASS)/$(EXAMPLE_NAME)
OUT_APP_MISC = $(OUT_MISC)/$(EXAMPLE_NAME)

APP1_NAME=ChildrenRetriever
APP1_JAR=$(OUT_APP_CLASS)/$(APP1_NAME).jar
APP2_NAME=DataStreamComposer
APP2_JAR=$(OUT_APP_CLASS)/$(APP2_NAME).jar
APP3_NAME=DataStreamRetriever
APP3_JAR=$(OUT_APP_CLASS)/$(APP3_NAME).jar
APP4_NAME=PropertiesComposer
APP4_JAR=$(OUT_APP_CLASS)/$(APP4_NAME).jar
APP5_NAME=PropertiesRetriever
APP5_JAR=$(OUT_APP_CLASS)/$(APP5_NAME).jar
APP6_NAME=ResourceCreator
APP6_JAR=$(OUT_APP_CLASS)/$(APP6_NAME).jar
APP7_NAME=ResourceManager
APP7_JAR=$(OUT_APP_CLASS)/$(APP7_NAME).jar
APP8_NAME=ResourceRemover
APP8_JAR=$(OUT_APP_CLASS)/$(APP8_NAME).jar

APP_JAVAFILES  = \
	Helper.java \
	MyActiveDataSink.java \
	MyInputStream.java

APP_CLASSFILES = $(patsubst %.java,$(OUT_APP_CLASS)/%.class,$(APP_JAVAFILES))
APP_CLASSNAMES = $(patsubst %.java,%.class,$(APP_JAVAFILES))

SDK_CLASSPATH = $(subst $(EMPTYSTRING) $(PATH_SEPARATOR),$(PATH_SEPARATOR),$(CLASSPATH)\
		$(PATH_SEPARATOR)$(OUT_APP_CLASS))

OUT_APP_CLASS = $(OUT_CLASS)/$(EXAMPLE_NAME)

# Targets
.PHONY: ALL
ALL : \
	UCBExamples

include $(SETTINGS)/stdtarget.mk

$(APP_CLASSFILES) : $(APP_JAVAFILES)
	-$(MKDIR) $(subst /,$(PS),$(@D))
	$(SDK_JAVAC) $(JAVAC_FLAGS) -classpath "$(SDK_CLASSPATH)" -d $(OUT_APP_CLASS) $(APP_JAVAFILES)

$(OUT_APP_CLASS)/%.class : %.java $(APP_CLASSFILES)
	-$(MKDIR) $(subst /,$(PS),$(@D))
	$(SDK_JAVAC) $(JAVAC_FLAGS) -classpath "$(SDK_CLASSPATH)" -d $(OUT_APP_CLASS) $<

$(OUT_APP_CLASS)/$(APP2_NAME).class : $(APP2_NAME).java $(OUT_APP_CLASS)/DataStreamRetriever.class $(APP_CLASSFILES)
	-$(MKDIR) $(subst /,$(PS),$(@D))
	$(SDK_JAVAC) $(JAVAC_FLAGS) -classpath "$(SDK_CLASSPATH)" -d $(OUT_APP_CLASS) $<

$(OUT_APP_CLASS)/$(APP6_NAME).class : $(APP6_NAME).java $(OUT_APP_CLASS)/DataStreamRetriever.class $(APP_CLASSFILES)
	-$(MKDIR) $(subst /,$(PS),$(@D))
	$(SDK_JAVAC) $(JAVAC_FLAGS) -classpath "$(SDK_CLASSPATH)" -d $(OUT_APP_CLASS) $<

$(OUT_APP_CLASS)/%.mf :
	-$(MKDIR) $(subst /,$(PS),$(@D))
	@echo Main-Class: com.sun.star.lib.loader.Loader> $@
	$(ECHOLINE)>> $@
	@echo Name: com/sun/star/lib/loader/Loader.class>> $@
	@echo Application-Class: $*>> $@

$(OUT_APP_CLASS)/%.jar : $(OUT_APP_CLASS)/%.mf $(OUT_APP_CLASS)/%.class $(APP_CLASSFILES)
	-$(DEL) $(subst \\,\,$(subst /,$(PS),$@))
	-$(MKDIR) $(subst /,$(PS),$(@D))
	+cd $(subst /,$(PS),$(OUT_APP_CLASS)) && $(SDK_JAR) cvfm $(@F) $*.mf $*.class $(APP_CLASSNAMES)
	+$(SDK_JAR) uvf $@ $(SDK_JAVA_UNO_BOOTSTRAP_FILES)

$(APP2_JAR) : $(OUT_APP_CLASS)/$(APP2_NAME).mf $(OUT_APP_CLASS)/$(APP2_NAME).class $(OUT_APP_CLASS)/DataStreamRetriever.class $(APP_CLASSFILES) 
	-$(DEL) $(subst \\,\,$(subst /,$(PS),$@))
	-$(MKDIR) $(subst /,$(PS),$(@D))
	+cd $(subst /,$(PS),$(OUT_APP_CLASS)) && $(SDK_JAR) cvfm $(@F) $(basename $(@F)).mf $(basename $(@F)).class DataStreamRetriever.class $(APP_CLASSNAMES)
	+$(SDK_JAR) uvf $@ $(SDK_JAVA_UNO_BOOTSTRAP_FILES)

$(APP6_JAR) : $(OUT_APP_CLASS)/$(APP6_NAME).mf $(OUT_APP_CLASS)/$(APP6_NAME).class $(OUT_APP_CLASS)/DataStreamRetriever.class $(APP_CLASSFILES) 
	-$(DEL) $(subst \\,\,$(subst /,$(PS),$@))
	-$(MKDIR) $(subst /,$(PS),$(@D))
	+cd $(subst /,$(PS),$(OUT_APP_CLASS)) && $(SDK_JAR) cvfm $(@F) $(basename $(@F)).mf $(basename $(@F)).class DataStreamRetriever.class $(APP_CLASSNAMES)
	+$(SDK_JAR) uvf $@ $(SDK_JAVA_UNO_BOOTSTRAP_FILES)


$(APP1_JAR) : $(OUT_APP_CLASS)/$(APP1_NAME).mf $(OUT_APP_CLASS)/$(APP1_NAME).class
$(APP2_JAR) : $(OUT_APP_CLASS)/$(APP2_NAME).mf $(OUT_APP_CLASS)/$(APP2_NAME).class
$(APP3_JAR) : $(OUT_APP_CLASS)/$(APP3_NAME).mf $(OUT_APP_CLASS)/$(APP3_NAME).class
$(APP4_JAR) : $(OUT_APP_CLASS)/$(APP4_NAME).mf $(OUT_APP_CLASS)/$(APP4_NAME).class
$(APP5_JAR) : $(OUT_APP_CLASS)/$(APP5_NAME).mf $(OUT_APP_CLASS)/$(APP5_NAME).class
$(APP6_JAR) : $(OUT_APP_CLASS)/$(APP6_NAME).mf $(OUT_APP_CLASS)/$(APP6_NAME).class
$(APP7_JAR) : $(OUT_APP_CLASS)/$(APP7_NAME).mf $(OUT_APP_CLASS)/$(APP7_NAME).class
$(APP8_JAR) : $(OUT_APP_CLASS)/$(APP8_NAME).mf $(OUT_APP_CLASS)/$(APP8_NAME).class

UCBExamples : $(APP1_JAR) $(APP2_JAR) $(APP3_JAR) $(APP4_JAR) $(APP5_JAR) $(APP6_JAR) $(APP7_JAR) $(APP8_JAR)
	@echo --------------------------------------------------------------------------------
	@echo Please use one of the following commands to execute the examples!
	@echo -
	@echo $(MAKE) $(APP1_NAME).run
	@echo $(MAKE) $(APP2_NAME).run
	@echo $(MAKE) $(APP3_NAME).run
	@echo $(MAKE) $(APP4_NAME).run
	@echo $(MAKE) $(APP5_NAME).run
	@echo $(MAKE) $(APP6_NAME).run
	@echo $(MAKE) $(APP7_NAME).run
	@echo $(MAKE) $(APP8_NAME).run
	@echo --------------------------------------------------------------------------------

%.run: $(OUT_APP_CLASS)/%.jar
	-$(MKDIR) $(subst /,$(PS),$(OUT_APP_MISC))
	$(SDK_JAVA) -Dcom.sun.star.lib.loader.unopath="$(OFFICE_PROGRAM_PATH)" -jar $< -workdir=$(OUT_APP_MISC)

.PHONY: clean
clean :
	-$(DELRECURSIVE) $(subst /,$(PS),$(OUT_APP_CLASS))
