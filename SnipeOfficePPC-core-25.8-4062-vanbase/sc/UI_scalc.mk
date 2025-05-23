# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_UI_UI,modules/scalc))

$(eval $(call gb_UI_add_uifiles,modules/scalc,\
	sc/uiconfig/scalc/ui/printeroptions \
	sc/uiconfig/scalc/ui/sortcriteriapage \
	sc/uiconfig/scalc/ui/sortkey \
	sc/uiconfig/scalc/ui/sortoptionspage \
	sc/uiconfig/scalc/ui/textimportoptions \
))

# vim: set noet sw=4 ts=4:
