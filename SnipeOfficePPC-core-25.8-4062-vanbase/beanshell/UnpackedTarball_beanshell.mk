# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_UnpackedTarball_UnpackedTarball,beanshell))

$(eval $(call gb_UnpackedTarball_set_tarball,beanshell,$(BSH_TARBALL)))

$(eval $(call gb_UnpackedTarball_add_patches,beanshell,\
	beanshell/bsh-2.0b1-src.patch \
))

# vim: set noet sw=4 ts=4:
