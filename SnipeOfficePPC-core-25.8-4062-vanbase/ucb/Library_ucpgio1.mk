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

$(eval $(call gb_Library_Library,ucpgio1))

$(eval $(call gb_Library_set_componentfile,ucpgio1,ucb/source/ucp/gio/ucpgio))

$(eval $(call gb_Library_use_sdk_api,ucpgio1))

$(eval $(call gb_Library_use_libraries,ucpgio1,\
	comphelper \
	cppu \
	cppuhelper \
	sal \
	salhelper \
	ucbhelper \
))

$(eval $(call gb_Library_use_externals,ucpgio1,\
	gio \
))

$(eval $(call gb_Library_add_exception_objects,ucpgio1,\
	ucb/source/ucp/gio/gio_content \
	ucb/source/ucp/gio/gio_datasupplier \
	ucb/source/ucp/gio/gio_inputstream \
	ucb/source/ucp/gio/gio_mount \
	ucb/source/ucp/gio/gio_outputstream \
	ucb/source/ucp/gio/gio_provider \
	ucb/source/ucp/gio/gio_resultset \
	ucb/source/ucp/gio/gio_seekable \
))

# vim: set noet sw=4 ts=4:
