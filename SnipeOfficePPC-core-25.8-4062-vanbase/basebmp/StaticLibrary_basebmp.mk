# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_StaticLibrary_StaticLibrary,basebmp))

$(eval $(call gb_StaticLibrary_use_package,basebmp,basebmp_inc))

$(eval $(call gb_StaticLibrary_use_sdk_api,basebmp))

$(eval $(call gb_StaticLibrary_use_external,basebmp,vigra_headers))

$(eval $(call gb_StaticLibrary_use_libraries,basebmp,\
    basegfx \
    sal \
	$(gb_UWINAPI) \
))

$(eval $(call gb_StaticLibrary_set_include,basebmp,\
    -I$(SRCDIR)/basebmp/inc/ \
    $$(INCLUDE) \
))

$(eval $(call gb_StaticLibrary_add_defs,basebmp,\
	-DBASEBMP_DLLIMPLEMENTATION \
))

$(eval $(call gb_StaticLibrary_add_exception_objects,basebmp,\
	basebmp/source/bitmapdevice \
	basebmp/source/debug \
	basebmp/source/polypolygonrenderer \
))

# vim: set noet sw=4 ts=4:
