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
#ifndef _DRAWDOC_HXX
#define _DRAWDOC_HXX

#include <svx/fmmodel.hxx>

class SwDoc;
class SwDocShell;

//==================================================================

class SwDrawDocument : public FmFormModel
{
    SwDoc* pDoc;
public:
    SwDrawDocument( SwDoc* pDoc );
    ~SwDrawDocument();

    const SwDoc& GetDoc() const { return *pDoc; }
          SwDoc& GetDoc()       { return *pDoc; }

    virtual SdrPage* AllocPage(bool bMasterPage);

    virtual ::com::sun::star::uno::Reference<
                ::com::sun::star::embed::XStorage> GetDocumentStorage() const;

    /// For saving of rectangles as control-replacement for versions < 5.0.
    virtual SdrLayerID GetControlExportLayerId( const SdrObject & ) const;

protected:
    /// overload of <SdrModel::createUnoModel()> is needed to provide corresponding uno model.
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > createUnoModel();
};


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
