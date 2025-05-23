# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file incorporates work covered by the following license notice:
#
#   Licensed to the Apache Software Foundation (ASF) under one or more
#   contributor license agreements. See the NOTICE file distributed
#   with this work for additional information regarding copyright
#   ownership. The ASF licenses this file to you under the Apache
#   License, Version 2.0 (the "License"); you may not use this file
#   except in compliance with the License. You may obtain a copy of
#   the License at http://www.apache.org/licenses/LICENSE-2.0 .
#

$(eval $(call gb_Package_Package,vcl_inc,$(SRCDIR)/vcl/inc))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/FilterConfigItem.hxx,vcl/FilterConfigItem.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/abstdlg.hxx,vcl/abstdlg.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/accel.hxx,vcl/accel.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/alpha.hxx,vcl/alpha.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/animate.hxx,vcl/animate.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/apptypes.hxx,vcl/apptypes.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/bitmapex.hxx,vcl/bitmapex.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/bitmap.hxx,vcl/bitmap.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/bmpacc.hxx,vcl/bmpacc.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/btndlg.hxx,vcl/btndlg.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/builder.hxx,vcl/builder.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/button.hxx,vcl/button.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/canvastools.hxx,vcl/canvastools.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/cmdevt.h,vcl/cmdevt.h))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/cmdevt.hxx,vcl/cmdevt.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/combobox.h,vcl/combobox.h))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/combobox.hxx,vcl/combobox.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/configsettings.hxx,vcl/configsettings.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/controllayout.hxx,vcl/controllayout.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/ctrl.hxx,vcl/ctrl.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/cursor.hxx,vcl/cursor.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/cvtgrf.hxx,vcl/cvtgrf.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/cvtsvm.hxx,vcl/cvtsvm.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/decoview.hxx,vcl/decoview.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/dialog.hxx,vcl/dialog.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/dllapi.h,vcl/dllapi.h))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/dndhelp.hxx,vcl/dndhelp.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/dockingarea.hxx,vcl/dockingarea.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/dockwin.hxx,vcl/dockwin.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/edit.hxx,vcl/edit.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/event.hxx,vcl/event.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/evntpost.hxx,vcl/evntpost.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/extoutdevdata.hxx,vcl/extoutdevdata.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/field.hxx,vcl/field.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/fixed.hxx,vcl/fixed.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/fixedhyper.hxx,vcl/fixedhyper.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/floatwin.hxx,vcl/floatwin.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/fntstyle.hxx,vcl/fntstyle.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/font.hxx,vcl/font.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/fontcapabilities.hxx,vcl/fontcapabilities.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/fontmanager.hxx,vcl/fontmanager.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/fpicker.hrc,vcl/fpicker.hrc))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/gdimtf.hxx,vcl/gdimtf.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/gfxlink.hxx,vcl/gfxlink.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/gradient.hxx,vcl/gradient.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/graph.h,vcl/graph.h))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/graph.hxx,vcl/graph.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/graphicfilter.hxx,vcl/graphicfilter.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/graphictools.hxx,vcl/graphictools.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/group.hxx,vcl/group.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/hatch.hxx,vcl/hatch.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/helper.hxx,vcl/helper.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/help.hxx,vcl/help.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/i18nhelp.hxx,vcl/i18nhelp.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/image.hxx,vcl/image.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/ImageListProvider.hxx,vcl/ImageListProvider.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/imagerepository.hxx,vcl/imagerepository.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/imgctrl.hxx,vcl/imgctrl.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/impdel.hxx,vcl/impdel.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/inputctx.hxx,vcl/inputctx.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/introwin.hxx,vcl/introwin.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/jobdata.hxx,vcl/jobdata.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/jobset.hxx,vcl/jobset.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/keycodes.hxx,vcl/keycodes.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/keycod.hxx,vcl/keycod.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/layout.hxx,vcl/layout.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/lazydelete.hxx,vcl/lazydelete.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/lineinfo.hxx,vcl/lineinfo.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/longcurr.hxx,vcl/longcurr.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/lstbox.h,vcl/lstbox.h))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/lstbox.hxx,vcl/lstbox.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/mapmod.hxx,vcl/mapmod.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/menubtn.hxx,vcl/menubtn.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/menu.hxx,vcl/menu.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/metaact.hxx,vcl/metaact.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/metric.hxx,vcl/metric.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/mnemonicengine.hxx,vcl/mnemonicengine.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/mnemonic.hxx,vcl/mnemonic.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/morebtn.hxx,vcl/morebtn.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/msgbox.hxx,vcl/msgbox.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/octree.hxx,vcl/octree.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/oldprintadaptor.hxx,vcl/oldprintadaptor.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/outdev.hxx,vcl/outdev.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/pdfextoutdevdata.hxx,vcl/pdfextoutdevdata.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/pdfwriter.hxx,vcl/pdfwriter.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/pngread.hxx,vcl/pngread.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/pngwrite.hxx,vcl/pngwrite.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/pointr.hxx,vcl/pointr.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/popupmenuwindow.hxx,vcl/popupmenuwindow.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/ppdparser.hxx,vcl/ppdparser.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/prgsbar.hxx,vcl/prgsbar.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/printerinfomanager.hxx,vcl/printerinfomanager.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/print.hxx,vcl/print.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/prntypes.hxx,vcl/prntypes.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/ptrstyle.hxx,vcl/ptrstyle.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/quickselectionengine.hxx,vcl/quickselectionengine.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/regband.hxx,vcl/regband.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/region.hxx,vcl/region.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/salbtype.hxx,vcl/salbtype.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/salctype.hxx,vcl/salctype.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/salgtype.hxx,vcl/salgtype.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/salnativewidgets.hxx,vcl/salnativewidgets.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/scopedbitmapaccess.hxx,vcl/scopedbitmapaccess.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/scrbar.hxx,vcl/scrbar.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/seleng.hxx,vcl/seleng.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/settings.hxx,vcl/settings.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/slider.hxx,vcl/slider.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/solarmutex.hxx,vcl/solarmutex.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/sound.hxx,vcl/sound.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/spinfld.hxx,vcl/spinfld.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/spin.h,vcl/spin.h))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/spin.hxx,vcl/spin.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/split.hxx,vcl/split.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/splitwin.hxx,vcl/splitwin.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/status.hxx,vcl/status.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/stdtext.hxx,vcl/stdtext.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/strhelper.hxx,vcl/strhelper.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/svapp.hxx,vcl/svapp.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/svgdata.hxx,vcl/svgdata.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/symbol.hxx,vcl/symbol.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/syschild.hxx,vcl/syschild.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/sysdata.hxx,vcl/sysdata.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/syswin.hxx,vcl/syswin.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/tabctrl.hxx,vcl/tabctrl.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/tabdlg.hxx,vcl/tabdlg.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/tabpage.hxx,vcl/tabpage.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/taskpanelist.hxx,vcl/taskpanelist.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/temporaryfonts.hxx,vcl/temporaryfonts.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/textdata.hxx,vcl/textdata.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/texteng.hxx,vcl/texteng.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/textview.hxx,vcl/textview.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/threadex.hxx,vcl/threadex.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/throbber.hxx,vcl/throbber.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/timer.hxx,vcl/timer.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/toolbox.hxx,vcl/toolbox.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/txtattr.hxx,vcl/txtattr.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/unohelp2.hxx,vcl/unohelp2.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/unohelp.hxx,vcl/unohelp.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/unowrap.hxx,vcl/unowrap.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/vclenum.hxx,vcl/vclenum.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/vclevent.hxx,vcl/vclevent.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/vclmain.hxx,vcl/vclmain.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/vclmedit.hxx,vcl/vclmedit.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/virdev.hxx,vcl/virdev.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/waitobj.hxx,vcl/waitobj.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/wall.hxx,vcl/wall.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/window.hxx,vcl/window.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/wmf.hxx,vcl/wmf.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/wrkwin.hxx,vcl/wrkwin.hxx))
$(eval $(call gb_Package_add_file,vcl_inc,inc/vcl/xtextedt.hxx,vcl/xtextedt.hxx))


# vim: set noet sw=4 ts=4:
