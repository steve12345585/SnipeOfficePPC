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

$(eval $(call gb_AllLangResTarget_AllLangResTarget,abp))

$(eval $(call gb_AllLangResTarget_set_reslocation,abp,extensions/res))

$(eval $(call gb_AllLangResTarget_add_srs,abp,abp/res))

$(eval $(call gb_SrsTarget_SrsTarget,abp/res))

$(eval $(call gb_SrsTarget_use_packages,abp/res,\
    svl_inc \
))

$(eval $(call gb_SrsTarget_set_include,abp/res,\
	$$(INCLUDE) \
	-I$(SRCDIR)/extensions/inc \
))

$(eval $(call gb_SrsTarget_add_files,abp/res,\
	extensions/source/abpilot/abspilot.src \
))

# vim:set shiftwidth=4 softtabstop=4 expandtab:
