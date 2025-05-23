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

# Builds the Java ConverterServlet example of the SDK.

PRJ=../../..
SETTINGS=$(PRJ)/settings

include $(SETTINGS)/settings.mk
include $(SETTINGS)/std.mk
include $(SETTINGS)/dk.mk

# Define non-platform/compiler specific settings
COMPONENT_NAME=ConverterServletExample
OUT_COMP_CLASS=$(OUT_CLASS)/$(COMPONENT_NAME)

JAVAFILES  = \
	ConverterServlet.java

CLASSFILES = $(patsubst %.java,$(OUT_COMP_CLASS)/%.class,$(JAVAFILES))

SDK_CLASSPATH = $(subst $(EMPTYSTRING) $(PATH_SEPARATOR),$(PATH_SEPARATOR),$(CLASSPATH)\
		$(PATH_SEPARATOR)$(OFFICE_CLASSES_DIR)/cos.jar\
		$(PATH_SEPARATOR)$(OFFICE_CLASSES_DIR)/servlet.jar\
		$(PATH_SEPARATOR)$(OUT_COMP_CLASS))

# Targets
.PHONY: ALL
ALL : \
    ConverterServletExample

include $(SETTINGS)/stdtarget.mk

$(OFFICE_CLASSES_DIR)/cos.jar : 
	@echo --------------------------------------------------------------------------------
	@echo You need the servlet support Java archive "$(QM)cos.jar$(QM)" for this example.
	@echo You can download it from "$(QM)http://www.servlets.com/cos/index.html$(QM)"
	@echo and then please copy it into your "$(QM)$(OFFICE_CLASSES_DIR)$(QM)" directory.
	@echo --------------------------------------------------------------------------------

$(OFFICE_CLASSES_DIR)/servlet.jar : 
	@echo --------------------------------------------------------------------------------
	@echo You need the servlet API "$(QM)servlet.jar$(QM)" for this example.
	@echo You can download it from "$(QM)http://java.sun.com/products/servlet/index.html$(QM)"
	@echo and then please copy it into your "$(QM)$(OFFICE_CLASSES_DIR)$(QM)" directory.
	@echo --------------------------------------------------------------------------------

$(CLASSFILES) : $(JAVAFILES) $(OFFICE_CLASSES_DIR)/cos.jar $(OFFICE_CLASSES_DIR)/servlet.jar
	-$(MKDIR) $(subst /,$(PS),$(@D))
	$(SDK_JAVAC) $(JAVAC_FLAGS) -classpath "$(SDK_CLASSPATH)" -d $(OUT_COMP_CLASS) $(JAVAFILES) 

ConverterServletExample : $(CLASSFILES)
	@echo --------------------------------------------------------------------------------
	@echo Please see the README file for more information how to use the converter 
	@echo servlet.
	@echo --------------------------------------------------------------------------------

%.run: $(OUT_COMP_CLASS)/%.class
	$(JAVA) -classpath "$(SDK_CLASSPATH)" $(basename $@)

.PHONY: clean
clean :
	-$(DELRECURSIVE) $(subst /,$(PS),$(OUT_COMP_CLASS))
