# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#


$(eval $(call gb_Executable_Executable,treex))

$(eval $(call gb_Executable_set_include,treex,\
    -I$(SRCDIR)/l10ntools/inc \
    $$(INCLUDE) \
))

$(eval $(call gb_Executable_use_libraries,treex,\
    sal \
))

$(eval $(call gb_Executable_use_static_libraries,treex,\
    transex \
))

$(eval $(call gb_Executable_add_exception_objects,treex,\
    l10ntools/source/treemerge \
    l10ntools/source/treex \
))

$(eval $(call gb_Executable_use_externals,treex,\
    libxml2 \
	icuuc \
    icui18n \
))

# vim: set noet sw=4 ts=4:
