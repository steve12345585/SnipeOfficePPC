# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
#
# Copyright 2000, 2011 Oracle and/or its affiliates.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# This file is part of OpenOffice.org.
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

$(eval $(call gb_Library_Library,basegfx))

$(eval $(call gb_Library_use_packages,basegfx,\
	basegfx_inc \
	o3tl_inc \
))

$(eval $(call gb_Library_set_include,basegfx,\
	-I$(SRCDIR)/basegfx/inc \
	-I$(SRCDIR)/basegfx/source/inc \
	$$(INCLUDE) \
))

$(eval $(call gb_Library_add_defs,basegfx,\
	-DBASEGFX_DLLIMPLEMENTATION \
))

# Work around gcc bug 41847 present at least in
# the Android x-compiler 4.4.3.
# -Warray-bounds appeared in 4.3
ifeq ($(COM),GCC)
ifeq ($(shell expr \( $(gb_CCVER) \>= 40300 \) \& \( $(gb_CCVER) \<= 40403 \)),1)
$(eval $(call gb_Library_add_cxxflags,basegfx,\
	-Wno-array-bounds \
))
endif
endif

$(eval $(call gb_Library_use_api,basegfx,\
    offapi \
    udkapi \
))

$(eval $(call gb_Library_use_libraries,basegfx,\
	cppu \
	cppuhelper \
	sal \
    $(gb_STDLIBS) \
))

$(eval $(call gb_Library_add_exception_objects,basegfx,\
	basegfx/source/tools/b2dclipstate \
	basegfx/source/tools/canvastools \
	basegfx/source/tools/zoomtools \
	basegfx/source/tools/gradienttools \
	basegfx/source/tools/keystoplerp \
	basegfx/source/tools/tools \
	basegfx/source/tools/unopolypolygon \
	basegfx/source/tools/unotools \
	basegfx/source/numeric/ftools \
	basegfx/source/tuple/b3ituple \
	basegfx/source/tuple/b3dtuple \
	basegfx/source/tuple/b2ituple \
	basegfx/source/tuple/b2dtuple \
	basegfx/source/tuple/b3i64tuple \
	basegfx/source/tuple/b2i64tuple \
	basegfx/source/range/b1drange \
	basegfx/source/range/b2dpolyrange \
	basegfx/source/range/b2xrange \
	basegfx/source/range/b3drange \
	basegfx/source/range/b2drangeclipper \
	basegfx/source/range/b2drange \
	basegfx/source/vector/b3ivector \
	basegfx/source/vector/b2ivector \
	basegfx/source/vector/b2dvector \
	basegfx/source/vector/b3dvector \
	basegfx/source/polygon/b3dpolygon \
	basegfx/source/polygon/b2dtrapezoid \
	basegfx/source/polygon/b3dpolygontools \
	basegfx/source/polygon/b2dlinegeometry \
	basegfx/source/polygon/b3dpolypolygontools \
	basegfx/source/polygon/b2dpolygonclipper \
	basegfx/source/polygon/b2dpolypolygoncutter \
	basegfx/source/polygon/b2dpolygon \
	basegfx/source/polygon/b2dpolygontriangulator \
	basegfx/source/polygon/b2dpolypolygon \
	basegfx/source/polygon/b3dpolypolygon \
	basegfx/source/polygon/b2dsvgpolypolygon \
	basegfx/source/polygon/b2dpolygoncutandtouch \
	basegfx/source/polygon/b2dpolypolygontools \
	basegfx/source/polygon/b2dpolygontools \
	basegfx/source/pixel/bpixel \
	basegfx/source/matrix/b2dhommatrixtools \
	basegfx/source/matrix/b3dhommatrix \
	basegfx/source/matrix/b2dhommatrix \
	basegfx/source/point/b3ipoint \
	basegfx/source/point/b2dpoint \
	basegfx/source/point/b3dpoint \
	basegfx/source/point/b2ipoint \
	basegfx/source/curve/b2dbeziertools \
	basegfx/source/curve/b2dcubicbezier \
	basegfx/source/color/bcolormodifier \
	basegfx/source/color/bcolortools \
	basegfx/source/raster/rasterconvert3d \
))

# vim: set noet sw=4 ts=4:
