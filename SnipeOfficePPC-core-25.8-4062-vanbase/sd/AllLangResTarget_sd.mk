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

$(eval $(call gb_AllLangResTarget_AllLangResTarget,sd))

$(eval $(call gb_AllLangResTarget_set_reslocation,sd,sd))

$(eval $(call gb_AllLangResTarget_add_srs,sd,\
    sd/res \
))

$(eval $(call gb_SrsTarget_SrsTarget,sd/res))

$(eval $(call gb_SrsTarget_use_packages,sd/res,\
	editeng_inc \
	sfx2_inc \
	svl_inc \
	svtools_inc \
	svx_globlmn_hrc \
	svx_inc \
	tools_inc \
))

$(eval $(call gb_SrsTarget_set_include,sd/res,\
    $$(INCLUDE) \
    -I$(SRCDIR)/sd/inc \
    -I$(SRCDIR)/sd/source/ui/inc \
    -I$(SRCDIR)/sd/source/ui/slidesorter/inc \
    -I$(call gb_SrsTemplateTarget_get_include_dir,sd) \
))

$(eval $(call gb_SrsTarget_add_files,sd/res,\
    sd/source/core/glob.src \
    sd/source/filter/html/pubdlg.src \
    sd/source/ui/accessibility/accessibility.src \
    sd/source/ui/animations/CustomAnimationCreateDialog.src \
    sd/source/ui/animations/CustomAnimationDialog.src \
    sd/source/ui/animations/CustomAnimationPane.src \
    sd/source/ui/animations/CustomAnimation.src \
    sd/source/ui/animations/SlideTransitionPane.src \
    sd/source/ui/annotations/annotations.src \
    sd/source/ui/app/app.src \
    sd/source/ui/app/popup.src \
    sd/source/ui/app/res_bmp.src \
    sd/source/ui/app/sdstring.src \
    sd/source/ui/app/strings.src \
    sd/source/ui/app/toolbox.src \
    sd/source/ui/dlg/animobjs.src \
    sd/source/ui/dlg/brkdlg.src \
    sd/source/ui/dlg/copydlg.src \
    sd/source/ui/dlg/custsdlg.src \
    sd/source/ui/dlg/dlgass.src \
    sd/source/ui/dlg/dlg_char.src \
    sd/source/ui/dlg/dlgfield.src \
    sd/source/ui/dlg/dlgolbul.src \
    sd/source/ui/dlg/dlgpage.src \
    sd/source/ui/dlg/dlgsnap.src \
    sd/source/ui/dlg/headerfooterdlg.src \
    sd/source/ui/dlg/inspagob.src \
    sd/source/ui/dlg/ins_paste.src \
    sd/source/ui/dlg/LayerDialog.src \
    sd/source/ui/dlg/layeroptionsdlg.src \
    sd/source/ui/dlg/masterlayoutdlg.src \
    sd/source/ui/dlg/morphdlg.src \
    sd/source/ui/dlg/navigatr.src \
    sd/source/ui/dlg/PaneDockingWindow.src \
    sd/source/ui/dlg/paragr.src \
    sd/source/ui/dlg/present.src \
    sd/source/ui/dlg/prltempl.src \
    sd/source/ui/dlg/prntopts.src \
    sd/source/ui/dlg/RemoteDialog.src \
    sd/source/ui/dlg/sdpreslt.src \
    sd/source/ui/dlg/tabtempl.src \
    sd/source/ui/dlg/tpaction.src \
    sd/source/ui/dlg/tpoption.src \
    sd/source/ui/dlg/vectdlg.src \
    sd/source/ui/slideshow/slideshow.src \
    sd/source/ui/table/TableDesignPane.src \
    sd/source/ui/view/DocumentRenderer.src \
))

$(eval $(call gb_SrsTarget_add_nonlocalized_files,sd/res,\
    sd/source/ui/slidesorter/view/SlsResource.src \
))

$(eval $(call gb_SrsTarget_add_templates,sd/res,\
    sd/source/ui/app/menuids3_tmpl.src \
    sd/source/ui/app/menuids_tmpl.src \
    sd/source/ui/app/popup2_tmpl.src \
    sd/source/ui/app/tbxids_tmpl.src \
    sd/source/ui/app/toolbox2_tmpl.src \
))

# vim: set noet sw=4 ts=4:
