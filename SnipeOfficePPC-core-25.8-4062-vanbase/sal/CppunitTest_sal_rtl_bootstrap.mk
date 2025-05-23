# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_CppunitTest_CppunitTest,sal_rtl_bootstrap))

$(eval $(call gb_CppunitTest_add_exception_objects,sal_rtl_bootstrap, \
    sal/qa/rtl/bootstrap/expand \
))

$(eval $(call gb_CppunitTest_use_libraries,sal_rtl_bootstrap, \
    sal \
    $(gb_UWINAPI) \
))

# vim: set noet sw=4 ts=4:
