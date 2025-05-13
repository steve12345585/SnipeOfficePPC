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
# Copyright (C) 2010 Red Hat, Inc., David Tardon <dtardon@redhat.com>
#  (initial developer)
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

include $(SRCDIR)/unoil/climaker/version.txt

$(eval $(call gb_CustomTarget_CustomTarget,unoil/climaker))

unoil_CLIDIR := $(call gb_CustomTarget_get_workdir,unoil/climaker)

$(call gb_CustomTarget_get_target,unoil/climaker) : \
	$(unoil_CLIDIR)/cli_oootypes.dll \
	$(unoil_CLIDIR)/cli_oootypes.config \
	$(unoil_CLIDIR)/$(CLI_OOOTYPES_POLICY_ASSEMBLY).dll

$(unoil_CLIDIR)/cli_oootypes.dll : $(SRCDIR)/unoil/climaker/version.txt \
		$(OUTDIR)/bin/offapi.rdb $(OUTDIR)/bin/udkapi.rdb \
		$(OUTDIR)/bin/cliuno.snk $(OUTDIR)/bin/cli_uretypes.dll \
		$(call gb_Executable_get_target_for_build,climaker) \
		| $(unoil_CLIDIR)/.dir
	$(call gb_Output_announce,$(subst $(WORKDIR)/,,$@),$(true),CLM,1)
	$(call gb_Helper_abbreviate_dirs, \
	$(call gb_Helper_execute,climaker \
		$(if $(filter -s,$(MAKEFLAGS)),,--verbose) \
		--out $@ \
		--assembly-version $(CLI_OOOTYPES_NEW_VERSION) \
		--assembly-company "LibreOffice" \
		--assembly-description "This assembly contains metadata for the LibreOffice API." \
		-X $(OUTDIR)/bin/udkapi.rdb \
		-r $(OUTDIR)/bin/cli_uretypes.dll \
		--keyfile $(OUTDIR)/bin/cliuno.snk \
		$(OUTDIR)/bin/offapi.rdb) > /dev/null)

$(unoil_CLIDIR)/cli_oootypes.config : \
		$(SRCDIR)/unoil/climaker/cli_oootypes_config \
		$(SRCDIR)/unoil/climaker/version.txt | $(unoil_CLIDIR)/.dir
	$(call gb_Output_announce,$(subst $(WORKDIR)/,,$@),$(true),PRL,1)
	$(call gb_Helper_abbreviate_dirs, \
	perl $(SRCDIR)/solenv/bin/clipatchconfig.pl $^ $@)

$(unoil_CLIDIR)/$(CLI_OOOTYPES_POLICY_ASSEMBLY).dll : \
		$(unoil_CLIDIR)/cli_oootypes.config \
		$(unoil_CLIDIR)/cli_oootypes.dll $(OUTDIR)/bin/cliuno.snk
	$(call gb_Output_announce,$(subst $(WORKDIR)/,,$@),$(true),AL ,1)
	$(call gb_Helper_abbreviate_dirs, \
	al -out:$@ \
		-version:$(CLI_OOOTYPES_POLICY_VERSION) \
		-keyfile:$(OUTDIR)/bin/cliuno.snk \
		-link:$<)

# vim:set shiftwidth=4 tabstop=4 noexpandtab:
