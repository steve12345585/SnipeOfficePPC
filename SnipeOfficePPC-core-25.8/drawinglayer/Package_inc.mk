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

$(eval $(call gb_Package_Package,drawinglayer_inc,$(SRCDIR)/drawinglayer/inc))

$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/animation/animationtiming.hxx,drawinglayer/animation/animationtiming.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/attribute/fillbitmapattribute.hxx,drawinglayer/attribute/fillbitmapattribute.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/attribute/fillgradientattribute.hxx,drawinglayer/attribute/fillgradientattribute.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/attribute/fillhatchattribute.hxx,drawinglayer/attribute/fillhatchattribute.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/attribute/fontattribute.hxx,drawinglayer/attribute/fontattribute.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/attribute/lineattribute.hxx,drawinglayer/attribute/lineattribute.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/attribute/linestartendattribute.hxx,drawinglayer/attribute/linestartendattribute.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/attribute/materialattribute3d.hxx,drawinglayer/attribute/materialattribute3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/attribute/sdrallattribute3d.hxx,drawinglayer/attribute/sdrallattribute3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/attribute/sdrfillattribute.hxx,drawinglayer/attribute/sdrfillattribute.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/attribute/sdrfillbitmapattribute.hxx,drawinglayer/attribute/sdrfillbitmapattribute.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/attribute/sdrlightattribute3d.hxx,drawinglayer/attribute/sdrlightattribute3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/attribute/sdrlightingattribute3d.hxx,drawinglayer/attribute/sdrlightingattribute3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/attribute/sdrlineattribute.hxx,drawinglayer/attribute/sdrlineattribute.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/attribute/sdrlinestartendattribute.hxx,drawinglayer/attribute/sdrlinestartendattribute.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/attribute/sdrobjectattribute3d.hxx,drawinglayer/attribute/sdrobjectattribute3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/attribute/sdrsceneattribute3d.hxx,drawinglayer/attribute/sdrsceneattribute3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/attribute/sdrshadowattribute.hxx,drawinglayer/attribute/sdrshadowattribute.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/attribute/strokeattribute.hxx,drawinglayer/attribute/strokeattribute.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/drawinglayerdllapi.h,drawinglayer/drawinglayerdllapi.h))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/geometry/viewinformation2d.hxx,drawinglayer/geometry/viewinformation2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/geometry/viewinformation3d.hxx,drawinglayer/geometry/viewinformation3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/animatedprimitive2d.hxx,drawinglayer/primitive2d/animatedprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/backgroundcolorprimitive2d.hxx,drawinglayer/primitive2d/backgroundcolorprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/baseprimitive2d.hxx,drawinglayer/primitive2d/baseprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/bitmapprimitive2d.hxx,drawinglayer/primitive2d/bitmapprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/borderlineprimitive2d.hxx,drawinglayer/primitive2d/borderlineprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/chartprimitive2d.hxx,drawinglayer/primitive2d/chartprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/clippedborderlineprimitive2d.hxx,drawinglayer/primitive2d/clippedborderlineprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/controlprimitive2d.hxx,drawinglayer/primitive2d/controlprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/discretebitmapprimitive2d.hxx,drawinglayer/primitive2d/discretebitmapprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/discreteshadowprimitive2d.hxx,drawinglayer/primitive2d/discreteshadowprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/drawinglayer_primitivetypes2d.hxx,drawinglayer/primitive2d/drawinglayer_primitivetypes2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/embedded3dprimitive2d.hxx,drawinglayer/primitive2d/embedded3dprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/epsprimitive2d.hxx,drawinglayer/primitive2d/epsprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/fillbitmapprimitive2d.hxx,drawinglayer/primitive2d/fillbitmapprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/fillgradientprimitive2d.hxx,drawinglayer/primitive2d/fillgradientprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/fillhatchprimitive2d.hxx,drawinglayer/primitive2d/fillhatchprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/graphicprimitive2d.hxx,drawinglayer/primitive2d/graphicprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/gridprimitive2d.hxx,drawinglayer/primitive2d/gridprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/groupprimitive2d.hxx,drawinglayer/primitive2d/groupprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/helplineprimitive2d.hxx,drawinglayer/primitive2d/helplineprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/hiddengeometryprimitive2d.hxx,drawinglayer/primitive2d/hiddengeometryprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/invertprimitive2d.hxx,drawinglayer/primitive2d/invertprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/markerarrayprimitive2d.hxx,drawinglayer/primitive2d/markerarrayprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/maskprimitive2d.hxx,drawinglayer/primitive2d/maskprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/mediaprimitive2d.hxx,drawinglayer/primitive2d/mediaprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/metafileprimitive2d.hxx,drawinglayer/primitive2d/metafileprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/modifiedcolorprimitive2d.hxx,drawinglayer/primitive2d/modifiedcolorprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/pagepreviewprimitive2d.hxx,drawinglayer/primitive2d/pagepreviewprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/pointarrayprimitive2d.hxx,drawinglayer/primitive2d/pointarrayprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/polygonprimitive2d.hxx,drawinglayer/primitive2d/polygonprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/polypolygonprimitive2d.hxx,drawinglayer/primitive2d/polypolygonprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/primitivetools2d.hxx,drawinglayer/primitive2d/primitivetools2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/rendergraphicprimitive2d.hxx,drawinglayer/primitive2d/rendergraphicprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/sceneprimitive2d.hxx,drawinglayer/primitive2d/sceneprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/sdrdecompositiontools2d.hxx,drawinglayer/primitive2d/sdrdecompositiontools2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/shadowprimitive2d.hxx,drawinglayer/primitive2d/shadowprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/structuretagprimitive2d.hxx,drawinglayer/primitive2d/structuretagprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/textdecoratedprimitive2d.hxx,drawinglayer/primitive2d/textdecoratedprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/texteffectprimitive2d.hxx,drawinglayer/primitive2d/texteffectprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/textenumsprimitive2d.hxx,drawinglayer/primitive2d/textenumsprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/texthierarchyprimitive2d.hxx,drawinglayer/primitive2d/texthierarchyprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/textlayoutdevice.hxx,drawinglayer/primitive2d/textlayoutdevice.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/textlineprimitive2d.hxx,drawinglayer/primitive2d/textlineprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/textprimitive2d.hxx,drawinglayer/primitive2d/textprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/textstrikeoutprimitive2d.hxx,drawinglayer/primitive2d/textstrikeoutprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/transformprimitive2d.hxx,drawinglayer/primitive2d/transformprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/transparenceprimitive2d.hxx,drawinglayer/primitive2d/transparenceprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/unifiedtransparenceprimitive2d.hxx,drawinglayer/primitive2d/unifiedtransparenceprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/wallpaperprimitive2d.hxx,drawinglayer/primitive2d/wallpaperprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive2d/wrongspellprimitive2d.hxx,drawinglayer/primitive2d/wrongspellprimitive2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive3d/baseprimitive3d.hxx,drawinglayer/primitive3d/baseprimitive3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive3d/drawinglayer_primitivetypes3d.hxx,drawinglayer/primitive3d/drawinglayer_primitivetypes3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive3d/groupprimitive3d.hxx,drawinglayer/primitive3d/groupprimitive3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive3d/hatchtextureprimitive3d.hxx,drawinglayer/primitive3d/hatchtextureprimitive3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive3d/hiddengeometryprimitive3d.hxx,drawinglayer/primitive3d/hiddengeometryprimitive3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive3d/modifiedcolorprimitive3d.hxx,drawinglayer/primitive3d/modifiedcolorprimitive3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive3d/polygonprimitive3d.hxx,drawinglayer/primitive3d/polygonprimitive3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive3d/polygontubeprimitive3d.hxx,drawinglayer/primitive3d/polygontubeprimitive3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive3d/polypolygonprimitive3d.hxx,drawinglayer/primitive3d/polypolygonprimitive3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive3d/sdrcubeprimitive3d.hxx,drawinglayer/primitive3d/sdrcubeprimitive3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive3d/sdrdecompositiontools3d.hxx,drawinglayer/primitive3d/sdrdecompositiontools3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive3d/sdrextrudelathetools3d.hxx,drawinglayer/primitive3d/sdrextrudelathetools3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive3d/sdrextrudeprimitive3d.hxx,drawinglayer/primitive3d/sdrextrudeprimitive3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive3d/sdrlatheprimitive3d.hxx,drawinglayer/primitive3d/sdrlatheprimitive3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive3d/sdrpolypolygonprimitive3d.hxx,drawinglayer/primitive3d/sdrpolypolygonprimitive3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive3d/sdrprimitive3d.hxx,drawinglayer/primitive3d/sdrprimitive3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive3d/sdrsphereprimitive3d.hxx,drawinglayer/primitive3d/sdrsphereprimitive3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive3d/shadowprimitive3d.hxx,drawinglayer/primitive3d/shadowprimitive3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive3d/textureprimitive3d.hxx,drawinglayer/primitive3d/textureprimitive3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/primitive3d/transformprimitive3d.hxx,drawinglayer/primitive3d/transformprimitive3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/processor2d/baseprocessor2d.hxx,drawinglayer/processor2d/baseprocessor2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/processor2d/canvasprocessor.hxx,drawinglayer/processor2d/canvasprocessor.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/processor2d/contourextractor2d.hxx,drawinglayer/processor2d/contourextractor2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/processor2d/hittestprocessor2d.hxx,drawinglayer/processor2d/hittestprocessor2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/processor2d/linegeometryextractor2d.hxx,drawinglayer/processor2d/linegeometryextractor2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/processor2d/textaspolygonextractor2d.hxx,drawinglayer/processor2d/textaspolygonextractor2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/processor2d/vclmetafileprocessor2d.hxx,drawinglayer/processor2d/vclmetafileprocessor2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/processor2d/vclpixelprocessor2d.hxx,drawinglayer/processor2d/vclpixelprocessor2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/processor2d/vclprocessor2d.hxx,drawinglayer/processor2d/vclprocessor2d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/processor3d/baseprocessor3d.hxx,drawinglayer/processor3d/baseprocessor3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/processor3d/cutfindprocessor3d.hxx,drawinglayer/processor3d/cutfindprocessor3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/processor3d/defaultprocessor3d.hxx,drawinglayer/processor3d/defaultprocessor3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/processor3d/geometry2dextractor.hxx,drawinglayer/processor3d/geometry2dextractor.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/processor3d/shadow3dextractor.hxx,drawinglayer/processor3d/shadow3dextractor.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/processor3d/zbufferprocessor3d.hxx,drawinglayer/processor3d/zbufferprocessor3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/texture/texture3d.hxx,drawinglayer/texture/texture3d.hxx))
$(eval $(call gb_Package_add_file,drawinglayer_inc,inc/drawinglayer/texture/texture.hxx,drawinglayer/texture/texture.hxx))

# vim: set noet sw=4 ts=4:
