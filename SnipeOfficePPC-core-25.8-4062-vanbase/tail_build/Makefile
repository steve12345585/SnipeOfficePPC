# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

gb_PARTIALBUILD := T
ifeq ($(strip $(SOLARENV)),)
include $(dir $(realpath $(firstword $(MAKEFILE_LIST))))/../config_host.mk
endif

include $(SOLARENV)/gbuild/gbuild.mk

$(eval $(call gb_Module_make_global_targets,$(SRCDIR)/Module_tail_build.mk))

# vim: set noet sw=4 ts=4:
