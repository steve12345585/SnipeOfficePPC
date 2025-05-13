# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
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
# Copyright (C) 2012 David Ostrovsky <d.ostrovsky@gmx.de> (initial developer)
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

$(eval $(call gb_CustomTarget_CustomTarget,testtools/bridgetest_climaker))

testtools_CLIDIR := $(call gb_CustomTarget_get_workdir,testtools/bridgetest_climaker)

$(call gb_CustomTarget_get_target,testtools/bridgetest_climaker) : \
	$(testtools_CLIDIR)/cli_types_bridgetest.dll

$(testtools_CLIDIR)/cli_types_bridgetest.dll : \
		$(OUTDIR)/bin/types.rdb $(OUTDIR)/bin/cli_uretypes.dll \
		$(call gb_Executable_get_target_for_build,climaker) \
		| $(testtools_CLIDIR)/.dir
	$(call gb_Output_announce,$(subst $(WORKDIR)/,,$@),$(true),CLM,1)
	$(call gb_Helper_abbreviate_dirs, \
	$(call gb_Helper_execute,climaker) \
		$(if $(filter -s,$(MAKEFLAGS)),,--verbose) \
		--out $@ \
		-X $(OUTDIR)/bin/types.rdb \
		-r $(OUTDIR)/bin/cli_uretypes.dll \
		$< > /dev/null)

# vim:set shiftwidth=4 tabstop=4 noexpandtab:
