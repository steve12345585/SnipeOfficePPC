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

$(eval $(call gb_Module_Module,svtools))

$(eval $(call gb_Module_add_targets,svtools,\
    AllLangResTarget_svt \
    Library_hatchwindowfactory \
    Library_svt \
    Package_inc \
    UI_svt \
))

ifneq ($(CROSS_COMPILING),YES)
$(eval $(call gb_Module_add_targets,svtools,\
    Executable_bmp \
    Executable_bmpsum \
    Executable_g2g \
))

ifeq ($(GUI),UNX)
$(eval $(call gb_Module_add_targets,svtools,\
    Executable_langsupport \
))
endif

endif

ifneq ($(OOO_JUNIT_JAR),)
$(eval $(call gb_Module_add_subsequentcheck_targets,svtools,\
    JunitTest_svtools_unoapi \
))
endif

#todo: javapatchres
#todo: jpeg on mac in svtools/util/makefile.mk
#todo: deliver errtxt.src as ehdl.srs
#todo: nooptfiles filter, filterconfigitem, FilterConfigCache, SvFilterOptionsDialog
#todo: map file

# vim: set noet sw=4 ts=4:
