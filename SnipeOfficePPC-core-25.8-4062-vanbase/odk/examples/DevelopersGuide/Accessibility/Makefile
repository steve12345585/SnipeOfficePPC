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

# Build the simple screen reader (SSR) described in the Developers 
# Guide Accessibility chapter.

PRJ=../../..
SETTINGS=$(PRJ)/settings

include $(SETTINGS)/settings.mk
include $(SETTINGS)/std.mk
include $(SETTINGS)/dk.mk

# Define non-platform/compiler specific settings
EXAMPLE_NAME=DevGuideAccessibilityExample
OUT_APP_CLASS = $(OUT_CLASS)/$(EXAMPLE_NAME)

APP1_NAME =SSR
APP1_JAR =$(OUT_APP_CLASS)/$(APP1_NAME).jar

JAVAFILES = \
		MessageArea.java \
		ConnectionTask.java	\
		EventHandler.java \
		EventListenerProxy.java	\
		GraphicalDisplay.java \
		NameProvider.java \
		IAccessibleObjectDisplay.java \
		RegistrationThread.java \
		SSR.java \
		TextualDisplay.java

APP1_CLASSFILES = $(patsubst %.java,$(OUT_APP_CLASS)/%.class,$(JAVAFILES))
APP1_CLASSNAMES = $(patsubst %.java,%.class,$(JAVAFILES)) \
	 EventListenerProxy$(ICL)1.class \
	 EventListenerProxy$(ICL)2.class \
	 EventListenerProxy$(ICL)3.class \
	 EventListenerProxy$(ICL)4.class

SDK_CLASSPATH = $(subst $(EMPTYSTRING) $(PATH_SEPARATOR),$(PATH_SEPARATOR),$(CLASSPATH)\
		$(PATH_SEPARATOR)$(OUT_APP_CLASS))


# Targets
.PHONY: ALL
ALL : $(EXAMPLE_NAME)

include $(SETTINGS)/stdtarget.mk

$(OUT_APP_CLASS)/%.class : %.java
	-$(MKDIR) $(subst /,$(PS),$(@D))
	$(SDK_JAVAC) $(JAVAC_FLAGS) -classpath "$(SDK_CLASSPATH)" -d $(OUT_APP_CLASS) $(JAVAFILES)

$(OUT_APP_CLASS)/%.mf :
	-$(MKDIR) $(subst /,$(PS),$(@D))
	@echo Main-Class: com.sun.star.lib.loader.Loader> $@
	$(ECHOLINE)>> $@
	@echo Name: com/sun/star/lib/loader/Loader.class>> $@
	@echo Application-Class: $*>> $@

$(APP1_JAR) : $(OUT_APP_CLASS)/$(APP1_NAME).mf $(APP1_CLASSFILES)
	-$(DEL) $(subst \\,\,$(subst /,$(PS),$@))
	-$(MKDIR) $(subst /,$(PS),$(@D))
	+cd $(subst /,$(PS),$(OUT_APP_CLASS)) && $(SDK_JAR) cvfm $(@F) $(basename $(@F)).mf $(APP1_CLASSNAMES)
	+$(SDK_JAR) uvf $@ $(SDK_JAVA_UNO_BOOTSTRAP_FILES)


$(EXAMPLE_NAME) : $(APP1_JAR)
	@echo --------------------------------------------------------------------------------
	@echo Before you run this example you should start your office with at least a 
	@echo new empty document and you should have enabled the accessibility support.
	@echo Please use the following command to execute the Simple Screen Reader example!
	@echo -
	@echo $(MAKE) $(APP1_NAME).run
	@echo --------------------------------------------------------------------------------

%.run: $(OUT_APP_CLASS)/%.jar
	$(SDK_JAVA) -Dcom.sun.star.lib.loader.unopath="$(OFFICE_PROGRAM_PATH)" -jar $<

.PHONY: clean
clean :
	-$(DELRECURSIVE) $(subst /,$(PS),$(OUT_APP_CLASS))
