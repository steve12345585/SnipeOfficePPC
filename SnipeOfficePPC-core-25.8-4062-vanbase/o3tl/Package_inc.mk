#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
#
# Copyright 2000, 2011 Oracle and/or its affiliates.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# This file is part of SnipeOffice.
#
# OpenOffice.org is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# OpenOffice.org is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.  If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************

$(eval $(call gb_Package_Package,o3tl_inc,$(SRCDIR)/o3tl/inc))

$(eval $(call gb_Package_add_file,o3tl_inc,inc/o3tl/compat_functional.hxx,o3tl/compat_functional.hxx))
$(eval $(call gb_Package_add_file,o3tl_inc,inc/o3tl/cow_wrapper.hxx,o3tl/cow_wrapper.hxx))
$(eval $(call gb_Package_add_file,o3tl_inc,inc/o3tl/heap_ptr.hxx,o3tl/heap_ptr.hxx))
$(eval $(call gb_Package_add_file,o3tl_inc,inc/o3tl/lazy_update.hxx,o3tl/lazy_update.hxx))
$(eval $(call gb_Package_add_file,o3tl_inc,inc/o3tl/range.hxx,o3tl/range.hxx))
$(eval $(call gb_Package_add_file,o3tl_inc,inc/o3tl/vector_pool.hxx,o3tl/vector_pool.hxx))
$(eval $(call gb_Package_add_file,o3tl_inc,inc/o3tl/sorted_vector.hxx,o3tl/sorted_vector.hxx))

# vim: set noet sw=4:
