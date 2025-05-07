# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
# Version: MPL 1.1 / GPLv3+ / LGPLv3+
#
# The contents of this file are subject to the Mozilla Public License Version
# 1.1 (the "License"); you may not use this file except in compliance with
# the License or as specified alternatively below. You may obtain a copy of
# the License at http://www.mozilla.org/MPL/
#
# Software distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
# for the specific language governing rights and limitations under the
# License.
#
# Major Contributor(s):
# Copyright (C) 2012 Red Hat, Inc., David Tardon <dtardon@redhat.com>
#  (initial developer)
#
# All Rights Reserved.
#
# For minor contributions see the git repository.
#
# Alternatively, the contents of this file may be used under the terms of
# either the GNU General Public License Version 3 or later (the "GPLv3+"), or
# the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
# in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
# instead of those above.

$(eval $(call gb_Library_Library,shlxthdl))

$(eval $(call gb_Library_set_include,shlxthdl,\
	-I$(SRCDIR)/shell/inc \
	$$(INCLUDE) \
))

$(eval $(call gb_Library_add_defs,shlxthdl,\
	-DISOLATION_AWARE_ENABLED \
	-DUNICODE \
	-DWIN32_LEAN_AND_MEAN \
	-DXML_UNICODE \
	-D_NTSDK \
	-D_UNICODE \
	-UNOMINMAX \
))
$(eval $(call gb_Library_add_defs,shlxthdl,\
	-U_WIN32_IE \
	-D_WIN32_IE=0x501 \
	-U_WIN32_WINNT \
	-D_WIN32_WINNT=0x0501 \
))

$(eval $(call gb_Library_use_externals,shlxthdl,\
	expat_utf16 \
	zlib \
))

$(eval $(call gb_Library_use_libraries,shlxthdl,\
	advapi32 \
	comctl32 \
	gdi32 \
	gdiplus \
	kernel32 \
	ole32 \
	oleaut32 \
	shell32 \
	shlwapi \
	uuid \
))

$(eval $(call gb_Library_use_static_libraries,shlxthdl,\
	shlxthandler_common \
	shell_xmlparser \
))

$(eval $(call gb_Library_add_nativeres,shlxthdl,shlxthdl))

$(eval $(call gb_Library_add_ldflags,shlxthdl,\
	/EXPORT:DllCanUnloadNow \
	/EXPORT:DllGetClassObject \
	/EXPORT:DllRegisterServer \
	/EXPORT:DllUnregisterServer \
	/NODEFAULTLIB:libcmt.lib \
))

$(eval $(call gb_Library_add_exception_objects,shlxthdl,\
    shell/source/win32/shlxthandler/classfactory \
    shell/source/win32/shlxthandler/columninfo/columninfo \
    shell/source/win32/shlxthandler/infotips/infotips \
    shell/source/win32/shlxthandler/propsheets/document_statistic \
    shell/source/win32/shlxthandler/propsheets/listviewbuilder \
    shell/source/win32/shlxthandler/propsheets/propsheets \
    shell/source/win32/shlxthandler/shlxthdl \
    shell/source/win32/shlxthandler/thumbviewer/thumbviewer \
))

# vim: set shiftwidth=4 tabstop=4 noexpandtab:
