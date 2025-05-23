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
# The Initial Developer of the Original Code is
# 	Peter Foley <pefoley2@verizon.net>
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
#

$(eval $(call gb_Library_Library,bib))

$(eval $(call gb_Library_use_packages,bib,\
    editeng_inc \
    sfx2_inc \
    svx_inc \
))

$(eval $(call gb_Library_set_componentfile,bib,extensions/source/bibliography/bib))

$(eval $(call gb_Library_set_include,bib,\
	$$(INCLUDE) \
	-I$(SRCDIR)/extensions/inc \
))

$(eval $(call gb_Library_use_sdk_api,bib))

$(eval $(call gb_Library_add_exception_objects,bib,\
	extensions/source/bibliography/bibbeam \
	extensions/source/bibliography/bibconfig \
	extensions/source/bibliography/bibcont \
	extensions/source/bibliography/bibload \
	extensions/source/bibliography/bibmod \
	extensions/source/bibliography/bibview \
	extensions/source/bibliography/datman \
	extensions/source/bibliography/formcontrolcontainer \
	extensions/source/bibliography/framectr \
	extensions/source/bibliography/general \
	extensions/source/bibliography/loadlisteneradapter \
	extensions/source/bibliography/toolbar \
))

$(eval $(call gb_Library_use_libraries,bib,\
	dbtools \
	sot \
	svt \
	svl \
	tk \
	vcl \
	utl \
	tl \
	comphelper \
	cppuhelper \
	cppu \
	sal \
	$(gb_UWINAPI) \
))

# vim:set shiftwidth=4 softtabstop=4 expandtab:
