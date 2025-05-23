# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
$(eval $(call gb_Executable_Executable,rdbedit))

$(eval $(call gb_Executable_use_libraries,rdbedit,\
    sal \
    reg \
))

$(eval $(call gb_Executable_use_static_libraries,rdbedit,\
    registry_helper \
))

$(eval $(call gb_Executable_add_exception_objects,rdbedit,\
    registry/tools/rdbedit \
))

# vim:set shiftwidth=4 softtabstop=4 expandtab:
