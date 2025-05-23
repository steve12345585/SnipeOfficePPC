# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_ExternalPackage_ExternalPackage,openldap,openldap))

$(eval $(call gb_ExternalPackage_use_external_project,openldap,openldap))

$(eval $(call gb_ExternalPackage_add_files,openldap,lib,\
	libraries/libldap/.libs/libldap.a \
	libraries/libldap_r/.libs/libldap_r.a \
	libraries/liblber/.libs/liblber.a \
))

# vim: set noet sw=4 ts=4:
