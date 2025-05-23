/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the SnipeOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __SC_ORCUSXML_HXX__
#define __SC_ORCUSXML_HXX__

#include "scdllapi.h"
#include "address.hxx"
#include "vcl/image.hxx"

#include <vector>
#include <boost/ptr_container/ptr_vector.hpp>

class SvTreeListEntry;

/**
 * Parameter used during call to ScOrcusFilters::loadXMLStructure().
 */
struct ScOrcusXMLTreeParam
{
    enum EntryType { ElementDefault, ElementRepeat, Attribute };

    /** Custom data stored with each tree item. */
    struct EntryData
    {
        size_t mnNamespaceID; /// numerical ID for xml namespace
        EntryType meType;
        ScAddress maLinkedPos; /// linked cell position (invalid if unlinked)
        bool mbRangeParent:1;
        bool mbLeafNode:1; /// Leaf if it has no child elements. Child Attributes don't count.

        SC_DLLPUBLIC EntryData(EntryType eType);
    };

    typedef boost::ptr_vector<EntryData> UserDataStoreType;

    Image maImgElementDefault;
    Image maImgElementRepeat;
    Image maImgAttribute;

    /**
     * Store all custom data instances since the tree control doesn't manage
     * the life cycle of user datas.
     */
    UserDataStoreType maUserDataStore;

    static SC_DLLPUBLIC EntryData* getUserData(SvTreeListEntry& rEntry);
    static SC_DLLPUBLIC const EntryData* getUserData(const SvTreeListEntry& rEntry);
};

struct ScOrcusImportXMLParam
{
    struct CellLink
    {
        ScAddress maPos;
        rtl::OString maPath;

        CellLink(const ScAddress& rPos, const rtl::OString& rPath);
    };

    struct RangeLink
    {
        ScAddress maPos;
        std::vector<rtl::OString> maFieldPaths;
    };

    typedef std::vector<CellLink> CellLinksType;
    typedef std::vector<RangeLink> RangeLinksType;

    std::vector<size_t> maNamespaces;
    CellLinksType maCellLinks;
    RangeLinksType maRangeLinks;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
