# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_Module_Module,sax))

$(eval $(call gb_Module_add_targets,sax,\
    Library_expwrap \
    Library_fastsax \
    Library_sax \
    Package_inc \
	StaticLibrary_sax_shared \
))

$(eval $(call gb_Module_add_check_targets,sax,\
    CppunitTest_sax \
))

# vim: set noet sw=4 ts=4:
