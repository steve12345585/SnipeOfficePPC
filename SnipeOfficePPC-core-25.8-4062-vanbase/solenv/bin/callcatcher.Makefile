# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

.PHONY: all

include config_host_callcatcher.mk

export CC:=callcatcher $(CC)
export CXX:=callcatcher $(CXX)
ifeq ($(AR),)
export AR:=callarchive ar
else
export AR:=callarchive $(AR)
endif
#old-school ones, can go post-gbuildification is complete
export LINK:=$(CXX)
ifeq ($(LIBMGR),)
export LIBMGR:=callarchive ar
else
export LIBMGR:=callarchive $(LIBMGR)
endif
export dbglevel:=2

include $(SOLARENV)/gbuild/gbuild.mk

findunusedcode:
	cd instsetoo_native && unset MAKEFLAGS && \
        $(SOLARENV)/bin/build.pl -P$(PARALLELISM) --all -- -P$(PARALLELISM)
	ooinstall -l $(DEVINSTALLDIR)/opt
	$(GNUMAKE) -j $(PARALLELISM) $(GMAKE_OPTIONS) -f Makefile.post subsequentcheck
	callanalyse \
        $(WORKDIR)/LinkTarget/*/* \
	    */$(OUTPATH)/bin/* \
	    */$(OUTPATH)/lib/* > unusedcode.all

# vim: set noet sw=4 ts=4:
