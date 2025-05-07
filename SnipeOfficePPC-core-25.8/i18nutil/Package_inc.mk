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

$(eval $(call gb_Package_Package,i18nutil_inc,$(SRCDIR)/i18nutil/inc))

$(eval $(call gb_Package_add_file,i18nutil_inc,inc/i18nutil/i18nutildllapi.h,i18nutil/i18nutildllapi.h))
$(eval $(call gb_Package_add_file,i18nutil_inc,inc/i18nutil/casefolding.hxx,i18nutil/casefolding.hxx))
$(eval $(call gb_Package_add_file,i18nutil_inc,inc/i18nutil/oneToOneMapping.hxx,i18nutil/oneToOneMapping.hxx))
$(eval $(call gb_Package_add_file,i18nutil_inc,inc/i18nutil/paper.hxx,i18nutil/paper.hxx))
$(eval $(call gb_Package_add_file,i18nutil_inc,inc/i18nutil/scripttypedetector.hxx,i18nutil/scripttypedetector.hxx))
$(eval $(call gb_Package_add_file,i18nutil_inc,inc/i18nutil/unicode.hxx,i18nutil/unicode.hxx))
$(eval $(call gb_Package_add_file,i18nutil_inc,inc/i18nutil/widthfolding.hxx,i18nutil/widthfolding.hxx))

# vim: set noet sw=4:
