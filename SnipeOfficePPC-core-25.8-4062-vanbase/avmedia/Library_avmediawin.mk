# -*- Mode: makefile; tab-width: 4; indent-tabs-mode: t -*-
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
# The Initial Developer of the Original Code is
#       Matúš Kukan <matus.kukan@gmail.com>
# Portions created by the Initial Developer are Copyright (C) 2011 the
# Initial Developer. All Rights Reserved.
#
# Major Contributor(s):
#
# For minor contributions see the git repository.
#
# Alternatively, the contents of this file may be used under the terms of
# either the GNU General Public License Version 3 or later (the "GPLv3+"), or
# the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
# in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
# instead of those above.

$(eval $(call gb_Library_Library,avmediawin))

$(eval $(call gb_Library_set_componentfile,avmediawin,avmedia/source/win/avmediawin))

$(eval $(call gb_Library_set_include,avmediawin,\
	$$(INCLUDE) \
	-I$(SRCDIR)/avmedia/source/inc \
))

$(eval $(call gb_Library_use_sdk_api,avmediawin))

$(eval $(call gb_Library_use_libraries,avmediawin,\
	comphelper \
	cppu \
	cppuhelper \
	sal \
	tl \
	utl \
	vcl \
	$(gb_UWINAPI) \
))

$(eval $(call gb_Library_use_system_win32_libs,avmediawin,\
	gdi32 \
	ole32 \
	oleaut32 \
	strmiids \
))

$(eval $(call gb_Library_add_exception_objects,avmediawin,\
	avmedia/source/win/framegrabber \
	avmedia/source/win/manager \
	avmedia/source/win/player \
	avmedia/source/win/window \
	avmedia/source/win/winuno \
))

# vim: set noet sw=4 ts=4:
