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

$(eval $(call gb_Library_Library,oox))

$(eval $(call gb_Library_use_packages,oox,\
    oox_inc \
    oox_generated \
    oox_tokens \
))

$(eval $(call gb_Library_use_custom_headers,oox,oox/generated))

$(eval $(call gb_Library_set_include,oox,\
    $$(INCLUDE) \
    -I$(SRCDIR)/oox/inc \
    -I$(call gb_CustomTarget_get_workdir,oox/generated)/inc \
))

ifeq ($(COM)-$(OS)-$(CPUNAME),GCC-LINUX-IA64)
# at least Debian Linux ia64 fails at compile time on
# link libooxlo.so which is apparently too large
# for the toolchain to handle with default optimization
$(eval $(call gb_Library_add_cxxflags,oox,\
    -Os \
))
endif

$(eval $(call gb_Library_add_defs,oox,\
    -DOOX_DLLIMPLEMENTATION \
))

$(eval $(call gb_Library_use_sdk_api,oox))

$(eval $(call gb_Library_use_libraries,oox,\
    basegfx \
    comphelper \
    cppu \
    cppuhelper \
    editeng \
    msfilter \
    sal \
	i18nisolang1 \
    sax \
    sfx \
    svl \
    svt \
    svxcore \
    sot \
    tl \
    utl \
    vcl \
    xo \
    xmlscript \
	$(gb_UWINAPI) \
))

$(eval $(call gb_Library_use_external,oox,openssl))

$(eval $(call gb_Library_set_componentfile,oox,oox/util/oox))

$(eval $(call gb_Library_add_exception_objects,oox,\
    oox/source/core/binarycodec \
    oox/source/core/contexthandler2 \
    oox/source/core/contexthandler \
    oox/source/core/fastparser \
    oox/source/core/fasttokenhandler \
    oox/source/core/filterbase \
    oox/source/core/filterdetect \
    oox/source/core/fragmenthandler2 \
    oox/source/core/fragmenthandler \
    oox/source/core/recordparser \
    oox/source/core/relations \
    oox/source/core/relationshandler \
    oox/source/core/services \
    oox/source/core/xmlfilterbase \
    oox/source/docprop/docprophandler \
    oox/source/docprop/ooxmldocpropimport \
    oox/source/drawingml/chart/axiscontext \
    oox/source/drawingml/chart/axisconverter \
    oox/source/drawingml/chart/axismodel \
    oox/source/drawingml/chart/chartcontextbase \
    oox/source/drawingml/chart/chartconverter \
    oox/source/drawingml/chart/chartdrawingfragment \
    oox/source/drawingml/chart/chartspaceconverter \
    oox/source/drawingml/chart/chartspacefragment \
    oox/source/drawingml/chart/chartspacemodel \
    oox/source/drawingml/chart/converterbase \
    oox/source/drawingml/chart/datasourcecontext \
    oox/source/drawingml/chart/datasourceconverter \
    oox/source/drawingml/chart/datasourcemodel \
    oox/source/drawingml/chart/modelbase \
    oox/source/drawingml/chart/objectformatter \
    oox/source/drawingml/chart/plotareacontext \
    oox/source/drawingml/chart/plotareaconverter \
    oox/source/drawingml/chart/plotareamodel \
    oox/source/drawingml/chart/seriescontext \
    oox/source/drawingml/chart/seriesconverter \
    oox/source/drawingml/chart/seriesmodel \
    oox/source/drawingml/chart/titlecontext \
    oox/source/drawingml/chart/titleconverter \
    oox/source/drawingml/chart/titlemodel \
    oox/source/drawingml/chart/typegroupcontext \
    oox/source/drawingml/chart/typegroupconverter \
    oox/source/drawingml/chart/typegroupmodel \
    oox/source/drawingml/clrschemecontext \
    oox/source/drawingml/clrscheme \
    oox/source/drawingml/colorchoicecontext \
    oox/source/drawingml/color \
    oox/source/drawingml/connectorshapecontext \
    oox/source/drawingml/customshapegeometry \
    oox/source/drawingml/customshapepresets1 \
    oox/source/drawingml/customshapepresets2 \
    oox/source/drawingml/customshapepresets3 \
    oox/source/drawingml/customshapepresets4 \
    oox/source/drawingml/customshapepresets5 \
    oox/source/drawingml/customshapepresets6 \
    oox/source/drawingml/customshapeproperties \
    oox/source/drawingml/diagram/constraintlistcontext \
    oox/source/drawingml/diagram/datamodelcontext \
    oox/source/drawingml/diagram/diagram \
    oox/source/drawingml/diagram/diagramdefinitioncontext \
    oox/source/drawingml/diagram/diagramfragmenthandler \
    oox/source/drawingml/diagram/diagramlayoutatoms \
    oox/source/drawingml/diagram/layoutnodecontext \
    oox/source/drawingml/drawingmltypes \
    oox/source/drawingml/effectproperties \
    oox/source/drawingml/effectpropertiescontext \
    oox/source/drawingml/embeddedwavaudiofile \
    oox/source/drawingml/fillproperties \
    oox/source/drawingml/fillpropertiesgroupcontext \
    oox/source/drawingml/graphicshapecontext \
    oox/source/drawingml/guidcontext \
    oox/source/drawingml/hyperlinkcontext \
    oox/source/drawingml/linepropertiescontext \
    oox/source/drawingml/lineproperties \
    oox/source/drawingml/objectdefaultcontext \
    oox/source/drawingml/shapecontext \
    oox/source/drawingml/shape \
    oox/source/drawingml/shape3dproperties \
    oox/source/drawingml/shapegroupcontext \
    oox/source/drawingml/shapepropertiescontext \
    oox/source/drawingml/shapepropertymap \
    oox/source/drawingml/shapestylecontext \
    oox/source/drawingml/spdefcontext \
    oox/source/drawingml/table/tablebackgroundstylecontext \
    oox/source/drawingml/table/tablecellcontext \
    oox/source/drawingml/table/tablecell \
    oox/source/drawingml/table/tablecontext \
    oox/source/drawingml/table/tablepartstylecontext \
    oox/source/drawingml/table/tableproperties \
    oox/source/drawingml/table/tablerowcontext \
    oox/source/drawingml/table/tablerow \
    oox/source/drawingml/table/tablestylecellstylecontext \
    oox/source/drawingml/table/tablestylecontext \
    oox/source/drawingml/table/tablestyle \
    oox/source/drawingml/table/tablestylelist \
    oox/source/drawingml/table/tablestylelistfragmenthandler \
    oox/source/drawingml/table/tablestylepart \
    oox/source/drawingml/table/tablestyletextstylecontext \
    oox/source/drawingml/textbodycontext \
    oox/source/drawingml/textbody \
    oox/source/drawingml/textbodypropertiescontext \
    oox/source/drawingml/textbodyproperties \
    oox/source/drawingml/textcharacterpropertiescontext \
    oox/source/drawingml/textcharacterproperties \
    oox/source/drawingml/textfieldcontext \
    oox/source/drawingml/textfield \
    oox/source/drawingml/textfont \
    oox/source/drawingml/textliststylecontext \
    oox/source/drawingml/textliststyle \
    oox/source/drawingml/textparagraph \
    oox/source/drawingml/textparagraphpropertiescontext \
    oox/source/drawingml/textparagraphproperties \
    oox/source/drawingml/textrun \
    oox/source/drawingml/textspacingcontext \
    oox/source/drawingml/texttabstoplistcontext \
    oox/source/drawingml/theme \
    oox/source/drawingml/themeelementscontext \
    oox/source/drawingml/themefragmenthandler \
    oox/source/drawingml/transform2dcontext \
    oox/source/dump/dffdumper \
    oox/source/dump/dumperbase \
    oox/source/dump/oledumper \
    oox/source/dump/pptxdumper \
    oox/source/export/chartexport \
    oox/source/export/ColorPropertySet \
    oox/source/export/drawingml \
    oox/source/export/SchXMLSeriesHelper \
    oox/source/export/shapes \
    oox/source/export/vmlexport \
    oox/source/helper/attributelist \
    oox/source/helper/binaryinputstream \
    oox/source/helper/binaryoutputstream \
    oox/source/helper/binarystreambase \
    oox/source/helper/containerhelper \
    oox/source/helper/graphichelper \
    oox/source/helper/modelobjecthelper \
    oox/source/helper/progressbar \
    oox/source/helper/propertymap \
    oox/source/helper/propertyset \
    oox/source/helper/storagebase \
    oox/source/helper/textinputstream \
    oox/source/helper/zipstorage \
    oox/source/mathml/export \
    oox/source/mathml/import \
    oox/source/mathml/importutils \
    oox/source/ole/axbinaryreader \
    oox/source/ole/axbinarywriter \
    oox/source/ole/axfontdata \
    oox/source/ole/axcontrol \
    oox/source/ole/axcontrolfragment \
    oox/source/ole/olehelper \
    oox/source/ole/oleobjecthelper \
    oox/source/ole/olestorage \
    oox/source/ole/vbacontrol \
    oox/source/ole/vbahelper \
    oox/source/ole/vbainputstream \
    oox/source/ole/vbamodule \
    oox/source/ole/vbaproject \
    oox/source/ppt/animationspersist \
    oox/source/ppt/animationtypes \
    oox/source/ppt/animvariantcontext \
    oox/source/ppt/backgroundproperties \
    oox/source/ppt/buildlistcontext \
    oox/source/ppt/commonbehaviorcontext \
    oox/source/ppt/commontimenodecontext \
    oox/source/ppt/conditioncontext \
    oox/source/ppt/customshowlistcontext \
    oox/source/ppt/dgmimport \
    oox/source/ppt/dgmlayout \
    oox/source/ppt/headerfootercontext \
    oox/source/ppt/layoutfragmenthandler \
    oox/source/ppt/pptfilterhelpers \
    oox/source/ppt/pptgraphicshapecontext \
    oox/source/ppt/pptimport \
    oox/source/ppt/pptshapecontext \
    oox/source/ppt/pptshape \
    oox/source/ppt/pptshapegroupcontext \
    oox/source/ppt/pptshapepropertiescontext \
    oox/source/ppt/presentationfragmenthandler \
    oox/source/ppt/slidefragmenthandler \
    oox/source/ppt/slidemastertextstylescontext \
    oox/source/ppt/slidepersist \
    oox/source/ppt/slidetimingcontext \
    oox/source/ppt/slidetransitioncontext \
    oox/source/ppt/slidetransition \
    oox/source/ppt/soundactioncontext \
    oox/source/ppt/timeanimvaluecontext \
    oox/source/ppt/timenode \
    oox/source/ppt/timenodelistcontext \
    oox/source/ppt/timetargetelementcontext \
    oox/source/ppt/extdrawingfragmenthandler \
    oox/source/shape/ShapeContextHandler \
    oox/source/shape/ShapeDrawingFragmentHandler \
    oox/source/shape/ShapeFilterBase \
    oox/source/token/namespacemap \
    oox/source/token/propertynames \
    oox/source/token/tokenmap \
    oox/source/vml/vmldrawing \
    oox/source/vml/vmldrawingfragment \
    oox/source/vml/vmlformatting \
    oox/source/vml/vmlinputstream \
    oox/source/vml/vmlshapecontainer \
    oox/source/vml/vmlshapecontext \
    oox/source/vml/vmlshape \
    oox/source/vml/vmltextboxcontext \
    oox/source/vml/vmltextbox \
))

$(eval $(call gb_Library_add_generated_exception_objects,oox,\
    CustomTarget/oox/generated/misc/vmlexport-shape-types \
))

# vim: set noet sw=4 ts=4:
