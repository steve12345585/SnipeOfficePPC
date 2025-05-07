/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

#ifndef _SFX_DOCINSERT_HXX
#define _SFX_DOCINSERT_HXX

#include <tools/errcode.hxx>
#include <tools/link.hxx>
#include <tools/string.hxx>
#include <vector>

#include "sfx2/dllapi.h"

namespace sfx2 { class FileDialogHelper; }
class SfxMedium;
class SfxItemSet;

typedef ::std::vector< SfxMedium* > SfxMediumList;

namespace sfx2 {

class SFX2_DLLPUBLIC DocumentInserter
{
private:
    String                  m_sDocFactory;
    String                  m_sFilter;
    Link                    m_aDialogClosedLink;

    sal_Int64 const         m_nDlgFlags;
    ErrCode                 m_nError;

    sfx2::FileDialogHelper* m_pFileDlg;
    SfxItemSet*             m_pItemSet;
    std::vector<rtl::OUString> m_pURLList;

    DECL_LINK(DialogClosedHdl, void *);

public:
    DocumentInserter(const String& rFactory,
                     bool const bEnableMultiSelection = false);
    ~DocumentInserter();

    void                    StartExecuteModal( const Link& _rDialogClosedLink );
    SfxMedium*              CreateMedium();
    SfxMediumList*          CreateMediumList();

    inline ErrCode          GetError() const { return m_nError; }
    inline String           GetFilter() const { return m_sFilter; }
};

} // namespace sfx2

#endif // _SFX_DOCINSERT_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
