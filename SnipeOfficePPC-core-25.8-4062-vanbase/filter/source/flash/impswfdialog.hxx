/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the SnipeOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#ifndef IMPSWFDIALOG_HXX
#define IMPSWFDIALOG_HXX

#include <com/sun/star/uno/Sequence.h>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <svtools/stdctrl.hxx>

#include <vcl/dialog.hxx>
#include <vcl/button.hxx>
#include <vcl/fixed.hxx>
#include <vcl/field.hxx>

#include <vcl/FilterConfigItem.hxx>

// ----------------
// - ImpSWFDialog -
// ----------------

class ResMgr;
class Window;

class ImpSWFDialog : public ModalDialog
{
private:
    FixedInfo           maFiDescr;
    NumericField        maNumFldQuality;
    FixedInfo           maFiExportAllDescr;
    CheckBox            maCheckExportAll;
    FixedInfo           maFiExportBackgroundsDescr;
    CheckBox            maCheckExportBackgrounds;
    FixedInfo           maFiExportBackgroundObjectsDescr;
    CheckBox            maCheckExportBackgroundObjects;
    FixedInfo           maFiExportSlideContentsDescr;
    CheckBox            maCheckExportSlideContents;
    FixedInfo           maFiExportSoundDescr;
    CheckBox            maCheckExportSound;
    FixedInfo           maFiExportOLEAsJPEGDescr;
    CheckBox            maCheckExportOLEAsJPEG;
    FixedInfo           maFiExportMultipleFilesDescr;
    CheckBox            maCheckExportMultipleFiles;

    OKButton            maBtnOK;
    CancelButton        maBtnCancel;
    HelpButton          maBtnHelp;

    FilterConfigItem    maConfigItem;

    DECL_LINK( OnToggleCheckbox, CheckBox* );

public:
    ImpSWFDialog( Window* pParent, ResMgr& rResMgr,
                  com::sun::star::uno::Sequence< com::sun::star::beans::PropertyValue >& rFilterData );
    ~ImpSWFDialog();

    com::sun::star::uno::Sequence< com::sun::star::beans::PropertyValue > GetFilterData();
};

#endif // IMPDIALOG_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
