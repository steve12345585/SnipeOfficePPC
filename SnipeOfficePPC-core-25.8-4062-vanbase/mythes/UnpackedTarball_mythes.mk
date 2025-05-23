# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_UnpackedTarball_UnpackedTarball,mythes))

$(eval $(call gb_UnpackedTarball_set_tarball,mythes,$(MYTHES_TARBALL)))

$(eval $(call gb_UnpackedTarball_add_patches,mythes,\
	mythes/mythes-1.2.0-vanilla-th-gen-idx.patch \
	mythes/mythes-1.2.0-android.patch \
	mythes/mythes-ssizet.patch \
))

# vim: set noet sw=4 ts=4:
