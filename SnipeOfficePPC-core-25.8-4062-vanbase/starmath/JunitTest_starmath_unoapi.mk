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
# The Initial Developer of the Original Code is
#       Bjoern Michaelsen, Canonical Ltd. <bjoern.michaelsen@canonical.com>
# Portions created by the Initial Developer are Copyright (C) 2010 the
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

$(eval $(call gb_JunitTest_JunitTest,starmath_unoapi))

$(eval $(call gb_JunitTest_set_defs,starmath_unoapi,\
    $$(DEFS) \
    -Dorg.openoffice.test.arg.sce=$(SRCDIR)/starmath/qa/unoapi/sm.sce \
    -Dorg.openoffice.test.arg.xcl=$(SRCDIR)/starmath/qa/unoapi/knownissues.xcl \
    -Dorg.openoffice.test.arg.tdoc=$(SRCDIR)/starmath/qa/unoapi/testdocuments \
))

$(eval $(call gb_JunitTest_use_jars,starmath_unoapi,\
    OOoRunner \
    ridl \
    test \
    unoil \
    jurt \
))

$(eval $(call gb_JunitTest_add_classes,starmath_unoapi,\
    org.openoffice.test.UnoApiTest \
))

# vim: set noet sw=4 ts=4:
