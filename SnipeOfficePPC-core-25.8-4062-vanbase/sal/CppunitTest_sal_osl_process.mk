# -*- Mode: makefile; tab-width: 4; indent-tabs-mode: t -*-
#
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
# Copyright (C) 2011 Matúš Kukan <matus.kukan@gmail.com> (initial developer)
#
# All Rights Reserved.
#
# For minor contributions see the git repository.
#
# Alternatively, the contents of this file may be used under the terms of
#se_libraries,sal_osl_process,\
    sal \
	$(g either the GNU General Public License Version 3 or later (the "GPLv3+"), or
# the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
# in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
# instead of those above.

$(eval $(call gb_CppunitTest_CppunitTest,sal_osl_process))

$(eval $(call gb_CppunitTest_add_exception_objects,sal_osl_process,\
    sal/qa/osl/process/osl_process \
))

$(eval $(call gb_CppunitTest_use_libraries,sal_osl_process,\
    sal \
	$(gb_UWINAPI) \
))

# Depend on to be tested executable
$(call gb_CppunitTest_get_target,sal_osl_process) : \
	$(call gb_Executable_get_target,osl_process_child)

# vim: set noet sw=4 ts=4:
