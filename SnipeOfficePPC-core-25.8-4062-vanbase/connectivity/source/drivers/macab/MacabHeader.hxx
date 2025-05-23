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

#ifndef _CONNECTIVITY_MACAB_HEADER_HXX_
#define _CONNECTIVITY_MACAB_HEADER_HXX_

#include "MacabRecord.hxx"
#include <cppuhelper/compbase3.hxx>

namespace connectivity
{
    namespace macab
    {
        class MacabHeader: public MacabRecord{
            protected:
                macabfield **sortRecord(sal_Int32 _start, sal_Int32 _length);
            public:
                MacabHeader();
                MacabHeader(const sal_Int32 _size, macabfield **_fields);
                virtual ~MacabHeader();
                void operator+= (const MacabHeader *r);
                ::rtl::OUString getString(const sal_Int32 i) const;
                void sortRecord();
                sal_Int32 getColumnNumber(const ::rtl::OUString s) const;

                static sal_Int32 compareFields(const macabfield *_field1, const macabfield *_field2);

                MacabHeader *begin();
                sal_Int32 end() const;
                class iterator{
                    protected:
                        sal_Int32 id;
                        MacabHeader *record;
                    public:
                        iterator& operator= (MacabHeader *_record);
                        iterator();
                        ~iterator();
                        void operator++ ();
                        sal_Bool operator!= (const sal_Int32 i) const;
                        sal_Bool operator== (const sal_Int32 i) const;
                        macabfield *operator* () const;
                };
        };
    }
}

#endif // _CONNECTIVITY_MACAB_HEADER_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
