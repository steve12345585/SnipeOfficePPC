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

$(eval $(call gb_StaticLibrary_StaticLibrary,codemaker))

$(eval $(call gb_StaticLibrary_use_external,codemaker,boost_headers))

$(eval $(call gb_StaticLibrary_use_packages,codemaker,\
	codemaker_inc \
	registry_inc \
	sal_generated \
	sal_inc \
))

$(eval $(call gb_StaticLibrary_add_exception_objects,codemaker,\
	codemaker/source/codemaker/dependencies \
	codemaker/source/codemaker/exceptiontree \
	codemaker/source/codemaker/global \
	codemaker/source/codemaker/options \
	codemaker/source/codemaker/typemanager \
	codemaker/source/codemaker/unotype \
	codemaker/source/codemaker/codemaker \
))

# vim: set noet sw=4 ts=4:
