# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_CustomTarget_CustomTarget,bridges/source/cpp_uno/gcc3_ios_arm))

$(call gb_CustomTarget_get_target,bridges/source/cpp_uno/gcc3_ios_arm) : \
	$(call gb_CustomTarget_get_workdir,bridges/source/cpp_uno/gcc3_ios_arm)/codesnippets.S

$(call gb_CustomTarget_get_workdir,bridges/source/cpp_uno/gcc3_ios_arm)/codesnippets.S : \
	$(SRCDIR)/bridges/source/cpp_uno/gcc3_ios_arm/generate-snippets.pl \
	| $(call gb_CustomTarget_get_workdir,bridges/source/cpp_uno/gcc3_ios_arm)/.dir
	$(PERL) $< > $@

# vim: set noet sw=4 ts=4:
