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
# Copyright (C) 2012 Red Hat, Inc., Caolán McNamara <caolanm@redhat.com>
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

$(eval $(call gb_CppunitTest_CppunitTest,filter_tiff_test))

$(eval $(call gb_CppunitTest_add_exception_objects,filter_tiff_test, \
    filter/qa/cppunit/filters-tiff-test \
))

$(eval $(call gb_CppunitTest_use_libraries,filter_tiff_test, \
    iti \
	sal \
	test \
	tl \
	unotest \
	vcl \
    $(gb_STDLIBS) \
))

$(eval $(call gb_CppunitTest_use_api,filter_tiff_test,\
    udkapi \
    offapi \
))

$(eval $(call gb_CppunitTest_use_ure,filter_tiff_test))

$(eval $(call gb_CppunitTest_use_components,filter_tiff_test,\
    configmgr/source/configmgr \
))

$(eval $(call gb_CppunitTest_use_configuration,filter_tiff_test))

# vim: set noet sw=4 ts=4:
