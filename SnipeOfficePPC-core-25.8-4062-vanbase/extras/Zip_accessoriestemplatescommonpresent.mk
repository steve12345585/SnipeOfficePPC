# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_Zip_Zip,accessoriestemplatescommonpresent,$(call gb_UnpackedTarball_get_dir,templates-pack)/templates/common/presnt))

$(eval $(call gb_Zip_add_dependencies,accessoriestemplatescommonpresent,\
	$(call gb_UnpackedTarball_get_target,templates-pack) \
))

$(eval $(call gb_Zip_add_files,accessoriestemplatescommonpresent,\
	dummy_common_templates.txt \
))

# vim: set noet sw=4 ts=4:
