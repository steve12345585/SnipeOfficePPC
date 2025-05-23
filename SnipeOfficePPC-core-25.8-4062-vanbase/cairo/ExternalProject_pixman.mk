# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_ExternalProject_ExternalProject,pixman))

$(eval $(call gb_ExternalProject_use_unpacked,pixman,pixman))

$(eval $(call gb_ExternalProject_register_targets,pixman,\
	build \
))

ifeq ($(OS)$(COM),WNTMSC)

$(call gb_ExternalProject_get_state_target,pixman,build) :
	cd $(EXTERNAL_WORKDIR)/pixman \
	&& $(MAKE) -f Makefile.win32 MMX=on SSE2=on CFG=release \
	&& touch $@

else

# ANDROID:
# The pixman-cpu.c code wants to read /proc/<pid>/auxv, but 
# the Android headers don't define Elf32_auxv_t.
#
# Maybe we should instead just patch the arm_has_* booleans in
# pixman-cpu.c to be hardcoded as TRUE and patch out the run-time
# check?
$(call gb_ExternalProject_get_state_target,pixman,build) :
	cd $(EXTERNAL_WORKDIR) \
	&& ./configure \
		$(if $(filter MACOSX IOS,$(OS)),--disable-shared,--disable-static) \
		$(if $(filter ANDROID,$(OS)),--disable-arm-simd --disable-arm-neon --disable-arm-iwmmxt) \
		$(if $(filter YES,$(CROSS_COMPILING)),--build=$(BUILD_PLATFORM) --host=$(HOST_PLATFORM)) \
	&& $(MAKE) \
	&& touch $@

endif

# vim: set noet sw=4 ts=4:
