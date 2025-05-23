# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2000, 2011 Oracle and/or its affiliates.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# This file is part of SnipeOffice.
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

# Convert path to file URL.
define gb_Helper_make_url
file:///$(strip $(1))
endef

# YaccTarget class

define gb_YaccTarget__command
$(call gb_Output_announce,$(2),$(true),YAC,3)
$(call gb_Helper_abbreviate_dirs,\
	mkdir -p $(dir $(3)) && \
	$(gb_YACC) $(T_YACCFLAGS) --defines=$(4) -o $(5) $(1) && touch $(3) )

endef

# CObject class

# $(call gb_CObject__command,object,relative-source,source,dep-file)
define gb_CObject__command
$(call gb_Output_announce,$(2).c,$(true),C  ,3)
$(call gb_Helper_abbreviate_dirs,\
	mkdir -p $(dir $(1)) $(dir $(4)) && \
	unset INCLUDE && \
	$(if $(filter YES,$(COBJECT_X64)), $(CXX_X64_BINARY), $(gb_CC)) \
		$(DEFS) \
		$(if $(filter Library,$(TARGETTYPE)),$(gb_COMPILER_LTOFLAGS)) \
		$(T_CFLAGS) \
		$(if $(WARNINGS_NOT_ERRORS),,$(gb_CFLAGS_WERROR)) \
		-Fd$(PDBFILE) \
		$(gb_COMPILERDEPFLAGS) \
		-I$(dir $(3)) \
		$(INCLUDE) \
		$(if $(filter YES,$(COBJECT_X64)), -U_X86_ -D_AMD64_,) \
		-c $(3) \
		-Fo$(1)) $(call gb_create_deps,$(4),$(1),$(3))
endef


# CxxObject class

# $(call gb_CxxObject__command,object,relative-source,source,dep-file)
define gb_CxxObject__command
$(call gb_Output_announce,$(2).cxx,$(true),CXX,3)
$(call gb_Helper_abbreviate_dirs,\
	mkdir -p $(dir $(1)) $(dir $(4)) && \
	unset INCLUDE && \
	$(if $(filter YES,$(CXXOBJECT_X64)), $(CXX_X64_BINARY), $(gb_CXX)) \
		$(DEFS) \
		$(if $(filter Library,$(TARGETTYPE)),$(gb_COMPILER_LTOFLAGS)) \
		$(T_CXXFLAGS) \
		$(if $(WARNINGS_NOT_ERRORS),,$(gb_CXXFLAGS_WERROR)) \
		-Fd$(PDBFILE) \
		$(gb_COMPILERDEPFLAGS) \
		-I$(dir $(3)) \
		$(INCLUDE) \
		$(if $(filter YES,$(CXXOBJECT_X64)), -U_X86_ -D_AMD64_,) \
		-c $(3) \
		-Fo$(1)) $(call gb_create_deps,$(4),$(1),$(3))
endef


# AsmObject class

gb_AsmObject_get_source = $(1)/$(2).asm

define gb_AsmObject__command
$(call gb_Output_announce,$(2),$(true),ASM,3)
$(call gb_Helper_abbreviate_dirs,\
	mkdir -p $(dir $(1)) $(dir $(4)) && \
	"$(ML_EXE)" $(gb_AFLAGS) -D$(COM) /Fo$(1) $(3)) && \
	echo "$(1) : $(3)" > $(4)
endef


# LinkTarget class

gb_LinkTarget_CFLAGS := $(gb_CFLAGS)
gb_LinkTarget_CXXFLAGS := $(gb_CXXFLAGS)

gb_LinkTarget_INCLUDE :=\
	$(subst -I. , ,$(SOLARINC)) \
	$(foreach inc,$(subst ;, ,$(JDKINC)),-I$(inc)) \
       -I$(SRCDIR)/config \

gb_LinkTarget_get_pdbfile = $(call gb_LinkTarget_get_target,)pdb/$(1).pdb

# avoid fatal error LNK1170 for Library_merged
define gb_LinkTarget_MergedResponseFile
cut -f -1000 -d ' ' $${RESPONSEFILE} > $${RESPONSEFILE}.1 && \
cut -f 1001- -d ' ' $${RESPONSEFILE} >> $${RESPONSEFILE}.1 && \
mv $${RESPONSEFILE}.1 $${RESPONSEFILE} &&
endef

define gb_LinkTarget__command
$(call gb_Output_announce,$(2),$(true),LNK,4)
$(call gb_Helper_abbreviate_dirs,\
	mkdir -p $(dir $(1)) && \
	rm -f $(1) && \
	RESPONSEFILE=$(call var2file,$(shell $(gb_MKTEMP)),100, \
		$(foreach object,$(CXXOBJECTS),$(call gb_CxxObject_get_target,$(object))) \
		$(foreach object,$(GENCXXOBJECTS),$(call gb_GenCxxObject_get_target,$(object))) \
		$(foreach object,$(COBJECTS),$(call gb_CObject_get_target,$(object))) \
		$(foreach object,$(GENCOBJECTS),$(call gb_GenCObject_get_target,$(object))) \
		$(foreach object,$(ASMOBJECTS),$(call gb_AsmObject_get_target,$(object))) \
		$(foreach extraobjectlist,$(EXTRAOBJECTLISTS),$(shell cat $(extraobjectlist))) \
		$(NATIVERES)) && \
		$(if $(filter $(call gb_Library_get_linktargetname,merged),$(2)),$(call gb_LinkTarget_MergedResponseFile)) \
	unset INCLUDE && \
	$(if $(filter YES,$(LIBRARY_X64)), $(LINK_X64_BINARY), $(gb_LINK)) \
		$(if $(filter Library CppunitTest,$(TARGETTYPE)),$(gb_Library_TARGETTYPEFLAGS)) \
		$(if $(filter StaticLibrary,$(TARGETTYPE)),$(gb_StaticLibrary_TARGETTYPEFLAGS)) \
		$(if $(filter Executable,$(TARGETTYPE)),$(gb_Executable_TARGETTYPEFLAGS)) \
		$(if $(filter YES,$(TARGETGUI)), -SUBSYSTEM:WINDOWS, -SUBSYSTEM:CONSOLE) \
		$(if $(filter YES,$(LIBRARY_X64)), -MACHINE:X64) \
		$(if $(filter YES,$(LIBRARY_X64)), -LIBPATH:$(OUTDIR)/lib/x64 -LIBPATH:$(COMPATH)/lib/amd64 -LIBPATH:$(WINDOWS_SDK_HOME)/lib/x64 \
		$(if $(filter 80,$(WINDOWS_SDK_VERSION)),-LIBPATH:$(WINDOWS_SDK_HOME)/lib/win8/um/x64,),) \
		$(T_LDFLAGS) \
		@$${RESPONSEFILE} \
		$(foreach lib,$(LINKED_LIBS),$(call gb_Library_get_filename,$(lib))) \
		$(foreach lib,$(LINKED_STATIC_LIBS),$(call gb_StaticLibrary_get_filename,$(lib))) \
		$(LIBS) \
		$(if $(filter-out StaticLibrary,$(TARGETTYPE)),user32.lib) \
		$(if $(DLLTARGET),-out:$(DLLTARGET) -implib:$(1),-out:$(1)); RC=$$?; rm $${RESPONSEFILE} \
	$(if $(DLLTARGET),; if [ ! -f $(DLLTARGET) ]; then rm -f $(1) && false; fi) \
	$(if $(filter Library,$(TARGETTYPE)),; if [ -f $(DLLTARGET).manifest ]; then mt.exe $(MTFLAGS) -nologo -manifest $(DLLTARGET).manifest -outputresource:$(DLLTARGET)\;2; fi) \
	$(if $(filter Executable,$(TARGETTYPE)),; if [ -f $(1).manifest ]; then mt.exe $(MTFLAGS) -nologo -manifest $(1).manifest -outputresource:$(1)\;1; fi) \
	$(if $(filter YES,$(TARGETGUI)),; if [ -f $(SRCDIR)/solenv/inc/DeclareDPIAware.manifest ]; then mt.exe $(MTFLAGS) -nologo -manifest $(SRCDIR)/solenv/inc/DeclareDPIAware.manifest -updateresource:$(1)\;1 ; fi) \
	; exit $$RC)
endef

define gb_LinkTarget_use_system_win32_libs
$(call gb_LinkTarget_add_libs,$(1),$(foreach lib,$(2),$(lib).lib))
endef

# Flags common for PE executables (EXEs and DLLs) 
gb_Windows_PE_TARGETTYPEFLAGS := \
	-release \
	-opt:noref \
	-incremental:no \
	-debug \
	-nxcompat \
	-dynamicbase \

# Library class


gb_Library_DEFS := -D_DLL
gb_Library_TARGETTYPEFLAGS := \
	-DLL \
	$(gb_Windows_PE_TARGETTYPEFLAGS)

ifeq ($(ENABLE_LTO),TRUE)
gb_Library_TARGETTYPEFLAGS += -LTCG
endif

gb_Library_get_rpath :=

gb_Library_SYSPRE := i
gb_Library_PLAINEXT := .lib

gb_Library_LAYER := \
	$(foreach lib,$(gb_Library_OOOLIBS),$(lib):OOO) \
	$(foreach lib,$(gb_Library_PLAINLIBS_NONE),$(lib):OOO) \
	$(foreach lib,$(gb_Library_PLAINLIBS_URE),$(lib):URELIB) \
	$(foreach lib,$(gb_Library_PLAINLIBS_OOO),$(lib):OOO) \
	$(foreach lib,$(gb_Library_RTLIBS),$(lib):OOO) \
	$(foreach lib,$(gb_Library_RTVERLIBS),$(lib):URELIB) \
	$(foreach lib,$(gb_Library_UNOLIBS_URE),$(lib):URELIB) \
	$(foreach lib,$(gb_Library_UNOLIBS_OOO),$(lib):OOO) \
	$(foreach lib,$(gb_Library_UNOVERLIBS),$(lib):URELIB) \
	$(foreach lib,$(gb_Library_EXTENSIONLIBS),$(lib):OXT) \

gb_Library_FILENAMES :=\
	$(foreach lib,$(gb_Library_KNOWNLIBS),$(lib):$(gb_Library_SYSPRE)$(lib)$(gb_Library_PLAINEXT)) \

gb_Library_DLLEXT := .dll
gb_Library_MAJORVER := 3
gb_Library_RTEXT := MSC$(gb_Library_DLLEXT)
gb_Library_OOOEXT := $(gb_Library_DLLPOSTFIX)$(gb_Library_DLLEXT)
gb_Library_UNOEXT := .uno$(gb_Library_DLLEXT)
gb_Library_UNOVEREXT := $(gb_Library_MAJORVER)$(gb_Library_DLLEXT)
gb_Library_RTVEREXT := $(gb_Library_MAJORVER)$(gb_Library_RTEXT)

gb_Library_DLLFILENAMES :=\
	$(foreach lib,$(gb_Library_OOOLIBS),$(lib):$(lib)$(gb_Library_OOOEXT)) \
	$(foreach lib,$(gb_Library_PLAINLIBS_NONE),$(lib):$(lib)$(gb_Library_DLLEXT)) \
	$(foreach lib,$(gb_Library_PLAINLIBS_URE),$(lib):$(lib)$(gb_Library_DLLEXT)) \
	$(foreach lib,$(gb_Library_PLAINLIBS_OOO),$(lib):$(lib)$(gb_Library_DLLEXT)) \
	$(foreach lib,$(gb_Library_RTLIBS),$(lib):$(lib)$(gb_Library_RTEXT)) \
	$(foreach lib,$(gb_Library_RTVERLIBS),$(lib):$(lib)$(gb_Library_RTVEREXT)) \
	$(foreach lib,$(gb_Library_UNOLIBS_URE),$(lib):$(lib)$(gb_Library_UNOEXT)) \
	$(foreach lib,$(gb_Library_UNOLIBS_OOO),$(lib):$(lib)$(gb_Library_UNOEXT)) \
	$(foreach lib,$(gb_Library_UNOVERLIBS),$(lib):$(lib)$(gb_Library_UNOVEREXT)) \
	$(foreach lib,$(gb_Library_EXTENSIONLIBS),$(lib):$(lib)$(gb_Library_UNOEXT)) \

# An assembly is a special kind of library for CLI
define gb_Library_Assembly
$(call gb_Library_Library,$(1))
$(call gb_LinkTarget_get_target,$(call gb_Library_get_linktargetname,$(1))) : NATIVERES :=

endef

define gb_Library_Library_platform
$(call gb_LinkTarget_set_dlltarget,$(2),$(3))

$(call gb_LinkTarget_add_auxtargets,$(2),\
	$(patsubst %.lib,%.exp,$(call gb_LinkTarget_get_target,$(2))) \
	$(3).manifest \
	$(call gb_LinkTarget_get_pdbfile,$(2)) \
)

$(if $(filter $(gb_MERGEDLIBS),$(1)),,\
$(call gb_Library_add_auxtarget,$(1),$(OUTDIR)/bin/$(notdir $(3))))

ifneq ($(ENABLE_CRASHDUMP),)
$(call gb_Library_add_auxtargets,$(1),\
		$(OUTDIR)/bin/$(notdir $(patsubst %.dll,%.pdb,$(3))) \
		$(OUTDIR)/bin/$(notdir $(patsubst %.dll,%.ilk,$(3))) \
)
else
$(call gb_LinkTarget_add_auxtargets,$(2),\
	$(patsubst %.dll,%.pdb,$(3)) \
	$(patsubst %.dll,%.ilk,$(3)) \
)
endif

$(call gb_Library_add_default_nativeres,$(1),$(1)/default)

$(if $(filter $(gb_MERGEDLIBS),$(1)),,$(call gb_Deliver_add_deliverable,$(OUTDIR)/bin/$(notdir $(3)),$(3),$(1)))

$(call gb_LinkTarget_get_target,$(2)) \
$(call gb_LinkTarget_get_headers_target,$(2)) : PDBFILE = $(call gb_LinkTarget_get_pdbfile,$(2))

endef

define gb_Library_add_default_nativeres
$(call gb_WinResTarget_WinResTarget_init,$(2))
$(call gb_WinResTarget_set_rcfile,$(2),solenv/inc/shlinfo)
$(call gb_WinResTarget_add_defs,$(2),\
		-DVERVARIANT="$(BUILD)" \
		-DRES_APP_VENDOR="$(OOO_VENDOR)" \
		-DORG_NAME="$(call gb_Library_get_dllname,$(1))"\
		-DINTERNAL_NAME="$(subst $(gb_Library_DLLEXT),,$(call gb_Library_get_dllname,$(1)))" \
		-DADDITIONAL_VERINFO1="" \
		-DADDITIONAL_VERINFO2="" \
		-DADDITIONAL_VERINFO3="" \
)
$(call gb_Library_add_nativeres,$(1),$(2))
$(call gb_Library_get_clean_target,$(1)) : $(call gb_WinResTarget_get_clean_target,$(2))

endef

define gb_LinkTarget_add_nativeres
$(call gb_LinkTarget_get_target,$(1)) : $(call gb_WinResTarget_get_target,$(2))
$(call gb_LinkTarget_get_target,$(1)) : NATIVERES += $(call gb_WinResTarget_get_target,$(2))

endef

define gb_LinkTarget_set_nativeres
$(call gb_LinkTarget_get_target,$(1)) : $(call gb_WinResTarget_get_target,$(2))
$(call gb_LinkTarget_get_target,$(1)) : NATIVERES := $(call gb_WinResTarget_get_target,$(2))

endef

define gb_Library_get_dllname
$(patsubst $(1):%,%,$(filter $(1):%,$(gb_Library_DLLFILENAMES)))
endef


# StaticLibrary class

gb_StaticLibrary_TARGETTYPEFLAGS := -LIB
gb_StaticLibrary_SYSPRE :=
gb_StaticLibrary_PLAINEXT := .lib

gb_StaticLibrary_FILENAMES := \
	$(foreach lib,$(gb_StaticLibrary_PLAINLIBS),$(lib):$(gb_StaticLibrary_SYSPRE)$(lib)$(gb_StaticLibrary_PLAINEXT)) \

define gb_StaticLibrary_StaticLibrary_platform
$(call gb_LinkTarget_get_target,$(2)) \
$(call gb_LinkTarget_get_headers_target,$(2)) : PDBFILE = $(call gb_LinkTarget_get_pdbfile,$(2))

$(call gb_LinkTarget_add_auxtargets,$(2),\
	$(call gb_LinkTarget_get_pdbfile,$(2)) \
)

endef

# Executable class

gb_Executable_EXT := .exe
gb_Executable_EXT_for_build := .exe
gb_Executable_TARGETTYPEFLAGS := $(gb_Windows_PE_TARGETTYPEFLAGS)

gb_Executable_get_rpath :=

define gb_Executable_Executable_platform
$(call gb_LinkTarget_add_auxtargets,$(2),\
	$(patsubst %.exe,%.pdb,$(call gb_LinkTarget_get_target,$(2))) \
	$(call gb_LinkTarget_get_pdbfile,$(2)) \
)

$(call gb_Executable_add_auxtarget,$(1),$(call gb_Executable_get_target,$(1)).manifest)
$(call gb_Deliver_add_deliverable,$(call gb_Executable_get_target,$(1)).manifest,$(call gb_LinkTarget_get_target,$(2)).manifest,$(1))

$(call gb_LinkTarget_get_target,$(2)) \
$(call gb_LinkTarget_get_headers_target,$(2)) : PDBFILE := $(call gb_LinkTarget_get_pdbfile,$(2))
$(call gb_LinkTarget_get_target,$(2)) : TARGETGUI :=

endef

# CppunitTest class

gb_CppunitTest_DEFS := -D_DLL
# cppunittester.exe is in the cppunit subdirectory of ${OUTDIR}/bin,
# thus it won't find its DLLs unless ${OUTDIR}/bin is added to PATH.
gb_CppunitTest_CPPTESTPRECOMMAND := $(gb_Helper_set_ld_path)

gb_CppunitTest_SYSPRE := itest_
gb_CppunitTest_EXT := .lib
gb_CppunitTest_LIBDIR := $(gb_Helper_OUTDIRLIBDIR)
gb_CppunitTest_get_filename = $(gb_CppunitTest_SYSPRE)$(1)$(gb_CppunitTest_EXT)
gb_CppunitTest_get_libfilename = test_$(1).dll

define gb_CppunitTest_CppunitTest_platform
$(call gb_LinkTarget_set_dlltarget,$(2),$(3))

$(call gb_LinkTarget_add_auxtargets,$(2),\
	$(patsubst %.lib,%.exp,$(call gb_LinkTarget_get_target,$(2))) \
	$(3) \
	$(3).manifest \
	$(patsubst %.dll,%.pdb,$(3)) \
	$(call gb_LinkTarget_get_pdbfile,$(2)) \
	$(patsubst %.dll,%.ilk,$(3)) \
)

$(call gb_LinkTarget_get_target,$(2)) \
$(call gb_LinkTarget_get_headers_target,$(2)) : PDBFILE = $(call gb_LinkTarget_get_pdbfile,$(2))

endef

# JunitTest class

gb_defaultlangiso := en-US
gb_smoketest_instset := $(SRCDIR)/instsetoo_native/$(INPATH)/OpenOffice/archive/install/$(gb_defaultlangiso)/OOo_*_install-arc_$(gb_defaultlangiso).zip

ifeq ($(OOO_TEST_SOFFICE),)


# Work around Windows problems with long pathnames (see issue 50885) by
# installing into the temp directory instead of the module output tree (in which
# case $(target).instpath contains the path to the temp installation,
# which is removed after smoketest); can be removed once issue 50885 is fixed;
# on other platforms, a single installation to solver is created in
# smoketestoo_native.

# for now, no dependency on $(shell ls $(gb_smoketest_instset))
# because that doesn't work before the instset is built
# and there is not much of a benefit anyway (gbuild not knowing about smoketest)
define gb_JunitTest_JunitTest_platform_longpathname_hack
$(call gb_JunitTest_get_target,$(1)) : $(call gb_JunitTest_get_target,$(1)).instpath
$(call gb_JunitTest_get_target,$(1)) : CLEAN_CMD = $(call gb_Helper_abbreviate_dirs,rm -rf `cat $$@.instpath` $$@.instpath)

$(call gb_JunitTest_get_target,$(1)).instpath : 
	INST_DIR=$$$$(cygpath -m `mktemp -d -t testinst.XXXXXX`) \
	&& unzip -d "$$$${INST_DIR}"  $$(gb_smoketest_instset) \
	&& mv "$$$${INST_DIR}"/OOo_*_install-arc_$$(gb_defaultlangiso) "$$$${INST_DIR}"/opt\
	&& mkdir -p $$(dir $$@) \
	&& echo "$$$${INST_DIR}" > $$@

endef
else # OOO_TEST_SOFFICE
gb_JunitTest_JunitTest_platform_longpathname_hack =
endif # OOO_TEST_SOFFICE

define gb_JunitTest_JunitTest_platform
$(call gb_JunitTest_get_target,$(1)) : DEFS := \
	-Dorg.openoffice.test.arg.soffice="$$$${OOO_TEST_SOFFICE:-path:$(OUTDIR)/installation/opt/program/soffice.exe}" \
	-Dorg.openoffice.test.arg.env=PATH="$$$$PATH" \
	-Dorg.openoffice.test.arg.user=$(call gb_Helper_make_url,$(call gb_JunitTest_get_userdir,$(1)))

endef

# SrsPartTarget class

ifeq ($(gb_FULLDEPS),$(true))
define gb_SrsPartTarget__command_dep
$(call gb_Helper_abbreviate_dirs,\
	$(OUTDIR)/bin/makedepend$(gb_Executable_EXT) \
		$(INCLUDE) \
		$(DEFS) \
		$(2) \
		-f - \
	| $(gb_AWK) -f $(GBUILDDIR)/processdeps.awk \
		-v OBJECTFILE=$(call gb_SrsPartTarget_get_target,$(1)) \
		-v OUTDIR=$(OUTDIR)/ \
		-v WORKDIR=$(WORKDIR)/ \
		-v SRCDIR=$(SRCDIR)/ \
	> $(call gb_SrsPartTarget_get_dep_target,$(1)))
endef
else
gb_SrsPartTarget__command_dep =
endif

# WinResTarget class

gb_WinResTarget_POSTFIX :=.res

define gb_WinResTarget__command
$(call gb_Output_announce,$(2),$(true),RC ,1)
$(call gb_Helper_abbreviate_dirs,\
	mkdir -p $(dir $(1)) && \
	$(gb_RC) \
		$(DEFS) $(FLAGS) \
		$(INCLUDE) \
		-Fo$(1) \
		$(RCFILE) )
endef

$(eval $(call gb_Helper_make_dep_targets,\
	WinResTarget \
))

ifeq ($(gb_FULLDEPS),$(true))
define gb_WinResTarget__command_dep
$(call gb_Output_announce,RC:$(2),$(true),DEP,1)
$(call gb_Helper_abbreviate_dirs,\
	mkdir -p $(dir $(1)) && \
	$(OUTDIR)/bin/makedepend$(gb_Executable_EXT) \
		$(INCLUDE) \
		$(DEFS) \
		$(RCFILE) \
		-f - \
	| $(gb_AWK) -f $(GBUILDDIR)/processdeps.awk \
		-v OBJECTFILE=$(3) \
		-v OUTDIR=$(OUTDIR)/ \
		-v WORKDIR=$(WORKDIR)/ \
		-v SRCDIR=$(SRCDIR)/ \
	> $(1))
endef
else
gb_WinResTarget__command_dep =
endif

# InstallModuleTarget class

define gb_InstallModuleTarget_InstallModuleTarget_platform
$(call gb_InstallModuleTarget_add_defs,$(1),\
	$(gb_CPUDEFS) \
	$(gb_OSDEFS) \
	-DCOMID=MSC \
	-DCOMNAME=$(if $(filter INTEL,$(CPUNAME)),msci,mscx) \
	$(if $(filter TRUE,$(SOLAR_JAVA)),-DSOLAR_JAVA) \
)

endef

# ScpConvertTarget class

define gb_ScpConvertTarget_ScpConvertTarget_platform
$(call gb_ScpConvertTarget_get_target,$(1)) :| $(OUTDIR)/bin/msi-encodinglist.txt
$(call gb_ScpConvertTarget_get_target,$(1)) : SCP_FLAGS := -t $(OUTDIR)/bin/msi-encodinglist.txt

endef

# InstallScript class

gb_InstallScript_EXT := .inf

# CliAssemblyTarget class

gb_CliAssemblyTarget_POLICYEXT := $(gb_Library_DLLEXT)
gb_CliAssemblyTarget_get_dll = $(OUTDIR)/bin/$(1)$(gb_CliAssemblyTarget_POLICYEXT)

# ExtensionTarget class

gb_ExtensionTarget_LICENSEFILE_DEFAULT := $(OUTDIR)/bin/osl/license.txt

# UnpackedTarget class

gb_UnpackedTarget_TARFILE_LOCATION := $(shell cygpath -u $(TARFILE_LOCATION))

# UnoApiHeadersTarget class

ifeq ($(DISABLE_DYNLOADING),TRUE)
gb_UnoApiHeadersTarget_select_variant = $(if $(filter udkapi,$(1)),comprehensive,$(2))
else
gb_UnoApiHeadersTarget_select_variant = $(2)
endif

# Python
gb_PYTHON_PRECOMMAND := $(gb_Helper_set_ld_path) PYTHONHOME="$(OUTDIR_FOR_BUILD)/lib/python" PYTHONPATH="$(OUTDIR_FOR_BUILD)/lib/python;$(OUTDIR_FOR_BUILD)/lib/python/lib-dynload"

# vim: set noet sw=4:
