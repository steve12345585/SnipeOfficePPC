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

$(eval $(call gb_Zip_Zip,ScriptsJavaScript,$(SRCDIR)/scripting/examples))

$(eval $(call gb_Zip_add_files,ScriptsJavaScript,\
	javascript/ExportSheetsToHTML/exportsheetstohtml.js \
	javascript/ExportSheetsToHTML/parcel-descriptor.xml \
	javascript/HelloWorld/helloworld.js \
	javascript/HelloWorld/parcel-descriptor.xml \
	javascript/Highlight/ButtonPressHandler.js \
	javascript/Highlight/parcel-descriptor.xml \
	javascript/Highlight/ShowDialog.js \
))

# vim: set noet sw=4 ts=4:
