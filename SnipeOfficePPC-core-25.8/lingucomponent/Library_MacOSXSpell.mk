# -*- Mode: makefile; tab-width: 4; indent-tabs-mode: t -*-
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
# Copyright (C) 2011 Matúš Kukan <matus.kukan@gmail.com> (initial developer)
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

$(eval $(call gb_Library_Library,MacOSXSpell))

$(eval $(call gb_Library_set_componentfile,MacOSXSpell,lingucomponent/source/spellcheck/macosxspell/MacOSXSpell))

$(eval $(call gb_Library_set_include,MacOSXSpell,\
	$$(INCLUDE) \
	-I$(SRCDIR)/lingucomponent/source/lingutil \
))

$(eval $(call gb_Library_use_api,MacOSXSpell,\
	udkapi \
	offapi \
))

$(eval $(call gb_Library_use_libraries,MacOSXSpell,\
	$(gb_STDLIBS) \
	cppu \
	cppuhelper \
	lng \
	sal \
	svl \
	tl \
	ucbhelper \
	utl \
))

$(eval $(call gb_Library_use_externals,MacOSXSpell,\
	cocoa \
	hunspell \
))

$(eval $(call gb_Library_add_objcxxobjects,MacOSXSpell,\
	lingucomponent/source/spellcheck/macosxspell/macreg \
	lingucomponent/source/spellcheck/macosxspell/macspellimp \
))

# vim: set noet sw=4 ts=4:
