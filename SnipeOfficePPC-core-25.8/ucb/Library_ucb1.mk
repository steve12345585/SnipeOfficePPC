# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# Version: MPL 1.1 / GPLv3+ / LGPLv3+
#
# The contents of this file are subject to the Mozilla Public License Version
# 1.1 (the "License"); you may not use this file except in compliance with
# the License or as specified alternatively below. You may obtain a copy of
# the License at http://www.mozilla.org/MPL/
#
# Software distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
# for the specific language governing rights and limitations under the
# License.
#
# Major Contributor(s):
# Copyright (C) 2012 Matúš Kukan <matus.kukan@gmail.com> (initial developer)
#
# All Rights Reserved.
#
# For minor contributions see the git repository.
#
# Alternatively, the contents of this file may be used under the terms of
# either the GNU General Public License Version 3 or later (the "GPLv3+"), or
# the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
# in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
# instead of those above.

$(eval $(call gb_Library_Library,ucb1))

$(eval $(call gb_Library_set_componentfile,ucb1,ucb/source/core/ucb1))

$(eval $(call gb_Library_set_include,ucb1,\
	-I$(SRCDIR)/ucb/source/inc \
	$$(INCLUDE) \
))

$(eval $(call gb_Library_use_api,ucb1,\
	offapi \
	udkapi \
))

$(eval $(call gb_Library_use_libraries,ucb1,\
	comphelper \
	cppu \
	cppuhelper \
	sal \
	ucbhelper \
	$(gb_STDLIBS) \
))

$(eval $(call gb_Library_add_exception_objects,ucb1,\
	ucb/source/core/cmdenv \
	ucb/source/core/identify \
	ucb/source/core/provprox \
	ucb/source/core/ucb \
	ucb/source/core/ucbcmds \
	ucb/source/core/ucbprops \
	ucb/source/core/ucbserv \
	ucb/source/core/ucbstore \
	ucb/source/regexp/regexp \
))

# vim: set noet sw=4 ts=4:
