# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_ExternalProject_ExternalProject,exttextcat))

$(eval $(call gb_ExternalProject_use_unpacked,exttextcat,exttextcat))

$(eval $(call gb_ExternalProject_register_targets,exttextcat,\
	build \
))

$(call gb_ExternalProject_get_state_target,exttextcat,build):
	cd $(EXTERNAL_WORKDIR) \
	&& ./configure --disable-shared --with-pic \
	$(if $(filter YES,$(CROSS_COMPILING)),--build=$(BUILD_PLATFORM) --host=$(HOST_PLATFORM)) \
	CFLAGS="$(if $(filter TRUE,$(HAVE_GCC_VISIBILITY_FEATURE)),-fvisibility=hidden) \
	$(if $(filter AIX,$(OS)),-D_LINUX_SOURCE_COMPAT)" \
	&& $(MAKE) \
	&& touch $@

# vim: set noet sw=4 ts=4:
