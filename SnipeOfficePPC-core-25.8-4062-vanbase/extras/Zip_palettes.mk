# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_Zip_Zip,palettes,$(SRCDIR)/extras/source/palettes))

$(eval $(call gb_Zip_add_files,palettes,\
	arrowhd.soe \
	classic.sog \
	cmyk.soc \
	gallery.soc \
	hatching.soh \
	html.soc \
	libreoffice.soc \
	modern.sog \
	palette.soc \
	scribus.soc \
	standard.sob \
	standard.soc \
	standard.sod \
	standard.soe \
	standard.sog \
	standard.soh \
	styles.sod \
	tango.soc \
	web.soc \
))

# vim: set noet sw=4 ts=4:
