# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file incorporates work covered by the following license notice:
#
#   Licensed to the Apache Software Foundation (ASF) under one or more
#   contributor license agreements. See the NOTICE file distributed
#   with this work for additional information regarding copyright
#   ownership. The ASF licenses this file to you under the Apache
#   License, Version 2.0 (the "License"); you may not use this file
#   except in compliance with the License. You may obtain a copy of
#   the License at http://www.apache.org/licenses/LICENSE-2.0 .
#

$(eval $(call gb_Module_Module,sw))

$(eval $(call gb_Module_add_targets,sw,\
    AllLangResTarget_sw \
    Library_msword \
    Library_sw \
    Library_swd \
    Library_swui \
    Package_uiconfig \
	UI_swriter \
))

ifneq ($(DISABLE_SCRIPTING),TRUE)

$(eval $(call gb_Module_add_targets,sw,\
    Library_vbaswobj \
))

endif

$(eval $(call gb_Module_add_check_targets,sw,\
    CppunitTest_sw_swdoc_test \
))

$(eval $(call gb_Module_add_slowcheck_targets,sw,\
    CppunitTest_sw_macros_test \
    CppunitTest_sw_subsequent_ooxmlexport \
    CppunitTest_sw_subsequent_ooxmlimport \
    CppunitTest_sw_subsequent_ww8export \
    CppunitTest_sw_subsequent_ww8import \
    CppunitTest_sw_subsequent_rtfexport \
    CppunitTest_sw_subsequent_rtfimport \
    CppunitTest_sw_subsequent_odfexport \
    CppunitTest_sw_subsequent_odfimport \
))

ifneq ($(DISABLE_CVE_TESTS),TRUE)
$(eval $(call gb_Module_add_slowcheck_targets,sw,\
    CppunitTest_sw_filters_test \
))
endif

ifneq ($(OOO_JUNIT_JAR),)
$(eval $(call gb_Module_add_subsequentcheck_targets,sw,\
    JunitTest_sw_complex \
    JunitTest_sw_unoapi \
))
endif

# vim: set noet sw=4 ts=4:
