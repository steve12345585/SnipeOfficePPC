# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_Module_Module,libwpg))

ifeq ($(SYSTEM_LIBWPG),NO)

$(eval $(call gb_Module_add_targets,libwpg,\
	ExternalPackage_libwpg \
	StaticLibrary_wpg \
	UnpackedTarball_wpg \
))

endif

# vim: set noet sw=4 ts=4:
