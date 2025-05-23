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
#ifndef CONNECTIVITY_java_util_Properties
#define CONNECTIVITY_java_util_Properties

#include "java/lang/Object.hxx"

namespace connectivity
{
    class java_util_Properties : public java_lang_Object
    {
    protected:
    // Static data for the class
        static jclass theClass;
    public:
        virtual jclass getMyClass() const;
        virtual ~java_util_Properties();
        // A ctor that is needed for returning the object
        java_util_Properties( JNIEnv * pEnv, jobject myObj ) : java_lang_Object( pEnv, myObj ){}
        java_util_Properties( );
        void setProperty(const ::rtl::OUString key, const ::rtl::OUString& value);
    };
}

#endif // CONNECTIVITY_java_util_Properties

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
