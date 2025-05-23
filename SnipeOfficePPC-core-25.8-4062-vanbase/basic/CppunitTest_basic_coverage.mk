#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_CppunitTest_CppunitTest,basic_coverage))

$(eval $(call gb_CppunitTest_add_exception_objects,basic_coverage, \
    basic/qa/cppunit/basic_coverage \
))

#$(eval $(call gb_CppunitTest_use_library_objects,basic_coverage,sb))

# add a list of all needed libraries here
$(eval $(call gb_CppunitTest_use_libraries,basic_coverage, \
    comphelper \
    cppu \
    cppuhelper \
    sal \
    salhelper \
    sb \
    sot \
    svl \
    svt \
    test \
    tl \
    unotest \
    utl \
    vcl \
    xmlscript \
    $(gb_UWINAPI) \
))

ifeq ($(GUI),WNT)
$(eval $(call gb_CppunitTest_use_system_win32_libs,basic_coverage, \
    oleaut32 \
))
endif

$(eval $(call gb_CppunitTest_set_include,basic_coverage,\
    -I$(SRCDIR)/basic/source/inc \
    -I$(SRCDIR)/basic/inc \
    $$(INCLUDE) \
))

$(eval $(call gb_CppunitTest_use_api,basic_coverage,\
    offapi \
    udkapi \
))

$(eval $(call gb_CppunitTest_use_ure,basic_coverage))

$(eval $(call gb_CppunitTest_use_components,basic_coverage,\
    configmgr/source/configmgr \
    i18npool/util/i18npool \
))
$(eval $(call gb_CppunitTest_use_configuration,basic_coverage))
