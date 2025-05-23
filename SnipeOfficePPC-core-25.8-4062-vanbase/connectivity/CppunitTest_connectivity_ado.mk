# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_CppunitTest_CppunitTest,connectivity_ado))

$(eval $(call gb_CppunitTest_set_include,connectivity_ado,\
	-I$(SRCDIR)/connectivity/source/inc \
	-I$(SRCDIR)/connectivity/source/drivers/ado \
	$$(INCLUDE) \
))

$(eval $(call gb_CppunitTest_use_api,connectivity_ado,\
    udkapi \
    offapi \
))

$(eval $(call gb_CppunitTest_use_ure,connectivity_ado))

$(eval $(call gb_CppunitTest_use_sdk_api,connectivity_ado))

ifeq ($(COM),GCC)
$(eval $(call gb_CppunitTest_add_cxxflags,connectivity_ado,\
	-fpermissive \
))
endif

ifeq ($(WINDOWS_SDK_VERSION),80)
$(eval $(call gb_CppunitTest_add_defs,connectivity_ado,\
	-DNTDDI_VERSION=0x0601 \
))
endif

$(eval $(call gb_CppunitTest_add_exception_objects,connectivity_ado, \
	connectivity/qa/connectivity/ado/DriverTest \
))

$(eval $(call gb_CppunitTest_use_libraries,connectivity_ado, \
    comphelper \
    cppu \
    cppuhelper \
	dbtools \
    i18nisolang1 \
    ado \
    sal \
    salhelper \
    sb \
    test \
    unotest \
    ucbhelper \
    utl \
	$(gb_UWINAPI) \
))

$(eval $(call gb_CppunitTest_use_components,connectivity_ado,\
    configmgr/source/configmgr \
    i18npool/util/i18npool \
    connectivity/source/drivers/ado/ado \
    ucb/source/core/ucb1 \
    ucb/source/ucp/file/ucpfile1 \
))

$(eval $(call gb_CppunitTest_use_configuration,connectivity_ado))

$(call gb_CppunitTest_get_target,connectivity_ado) : $(WORKDIR)/CppunitTest/TS001018407.mdb
$(WORKDIR)/CppunitTest/TS001018407.mdb : $(SRCDIR)/connectivity/qa/connectivity/ado/TS001018407.mdb
	mkdir -p $(dir $@)
	$(call gb_Deliver_deliver,$<,$@)

# vim: set noet sw=4 ts=4:
