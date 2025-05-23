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
#ifndef INCLUDED_I_DOCUMENT_EXTERNAL_DATA_HXX
#define INCLUDED_I_DOCUMENT_EXTERNAL_DATA_HXX

#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>

namespace sw
{
enum tExternalDataType { FIB, STTBF_ASSOC };

struct ExternalDataTypeHash
{
    size_t operator()(tExternalDataType eType) const { return eType; }
};

class ExternalData
{
public:
    ExternalData() {}
    virtual ~ExternalData() {}
};

typedef ::boost::shared_ptr<ExternalData> tExternalDataPointer;
}


class IDocumentExternalData
{
protected:
    typedef ::boost::unordered_map<sw::tExternalDataType, sw::tExternalDataPointer, sw::ExternalDataTypeHash>
    tExternalData;

    tExternalData m_externalData;

    virtual ~IDocumentExternalData() {};

public:
    virtual void setExternalData(sw::tExternalDataType eType,
                                 sw::tExternalDataPointer pPayload) = 0;
    virtual sw::tExternalDataPointer getExternalData(sw::tExternalDataType eType) = 0;
};

#endif //INCLUDED_I_DOCUMENT_EXTERNAL_DATA_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
