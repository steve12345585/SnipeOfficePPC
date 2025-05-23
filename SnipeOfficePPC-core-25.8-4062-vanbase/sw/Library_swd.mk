# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file incorporates work covered by the following license notice:
#
#   Licensed to the Apache Software Foundation (ASF) under one or more
#   contributor license agreements. See the NOTICE file distributed
#   with this work for additional information regarding copyright
#   ownership. The ASF licenses this file to you under the Apache
#   License, Version 2.0 (the "License"); you may not use this file
#   except in compliance with the License. You may obtain a copy of
#   the License at http://www.apache.org/licenses/LICENSE-2.0 .
#

$(eval $(call gb_Library_Library,swd))

$(eval $(call gb_Library_set_componentfile,swd,sw/util/swd))

$(eval $(call gb_Library_set_include,swd,\
    $$(INCLUDE) \
    -I$(SRCDIR)/sw/inc \
    -I$(SRCDIR)/sw/source/core/inc \
    -I$(SRCDIR)/sw/source/filter/inc \
    -I$(SRCDIR)/sw/source/ui/inc \
))

$(eval $(call gb_Library_use_sdk_api,swd))

$(eval $(call gb_Library_add_defs,swd,\
    -DSWD_DLLIMPLEMENTATION \
))

$(eval $(call gb_Library_use_libraries,swd,\
    comphelper \
    cppu \
    cppuhelper \
    sal \
    sfx \
    sot \
    svl \
    svt \
    tl \
    ucbhelper \
    utl \
    vcl \
	$(gb_UWINAPI) \
))

$(eval $(call gb_Library_add_exception_objects,swd,\
    sw/source/filter/basflt/iodetect \
    sw/source/ui/uno/detreg \
    sw/source/ui/uno/swdet2 \
    sw/source/ui/uno/swdetect \
))

# vim: set noet sw=4 ts=4:
