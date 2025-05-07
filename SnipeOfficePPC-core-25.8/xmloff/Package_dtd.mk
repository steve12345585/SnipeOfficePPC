# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
# 
# Copyright 2000, 2011 Oracle and/or its affiliates.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# This file is part of OpenOffice.org.
#
# OpenOffice.org is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# OpenOffice.org is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.  If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************

$(eval $(call gb_Package_Package,xmloff_dtd,$(SRCDIR)/xmloff/dtd))
$(eval $(call gb_Package_add_file,xmloff_dtd,bin/Blocklist.dtd,Blocklist.dtd))
$(eval $(call gb_Package_add_file,xmloff_dtd,bin/chart.mod,chart.mod))
$(eval $(call gb_Package_add_file,xmloff_dtd,bin/datastyl.mod,datastyl.mod))
$(eval $(call gb_Package_add_file,xmloff_dtd,bin/defs.mod,defs.mod))
$(eval $(call gb_Package_add_file,xmloff_dtd,bin/drawing.mod,drawing.mod))
$(eval $(call gb_Package_add_file,xmloff_dtd,bin/dtypes.mod,dtypes.mod))
$(eval $(call gb_Package_add_file,xmloff_dtd,bin/form.mod,form.mod))
$(eval $(call gb_Package_add_file,xmloff_dtd,bin/meta.mod,meta.mod))
$(eval $(call gb_Package_add_file,xmloff_dtd,bin/nmspace.mod,nmspace.mod))
$(eval $(call gb_Package_add_file,xmloff_dtd,bin/office.dtd,office.dtd))
$(eval $(call gb_Package_add_file,xmloff_dtd,bin/office.mod,office.mod))
$(eval $(call gb_Package_add_file,xmloff_dtd,bin/openoffice-2.0-schema.rng,openoffice-2.0-schema.rng))
$(eval $(call gb_Package_add_file,xmloff_dtd,bin/script.mod,script.mod))
$(eval $(call gb_Package_add_file,xmloff_dtd,bin/settings.mod,settings.mod))
$(eval $(call gb_Package_add_file,xmloff_dtd,bin/style.mod,style.mod))
$(eval $(call gb_Package_add_file,xmloff_dtd,bin/table.mod,table.mod))
$(eval $(call gb_Package_add_file,xmloff_dtd,bin/text.mod,text.mod))

# vim: set noet sw=4 ts=4:
