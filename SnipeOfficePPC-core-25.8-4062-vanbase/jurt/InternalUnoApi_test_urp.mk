# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_InternalUnoApi_InternalUnoApi,test_urp))

$(eval $(call gb_InternalUnoApi_use_api,test_urp,\
    udkapi \
))

$(eval $(call gb_InternalUnoApi_set_include,test_urp,\
    -I$(OUTDIR)/idl \
    $$(INCLUDE) \
))

$(eval $(call gb_InternalUnoApi_add_idlfiles,test_urp,jurt/test/com/sun/star/lib/uno/protocols/urp,\
	interfaces \
))

# vim: set noet sw=4 ts=4:
