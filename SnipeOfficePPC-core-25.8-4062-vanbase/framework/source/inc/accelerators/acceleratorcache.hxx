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

#ifndef __FRAMEWORK_ACCELERATORS_ACCELERATORCACHE_HXX_
#define __FRAMEWORK_ACCELERATORS_ACCELERATORCACHE_HXX_

#include <threadhelp/threadhelpbase.hxx>
#include <general.h>
#include <stdtypes.h>

#include <com/sun/star/awt/KeyEvent.hpp>

#include <comphelper/sequenceasvector.hxx>

//__________________________________________
// definition

namespace framework
{

//__________________________________________
/**
    @short  implements a cache for any accelerator configuration.

    @descr  Its implemented threadsafe, supports copy-on-write pattern
            and a flush mechansim to support concurrent access to the same
            configuration.

            copy-on-write ... How? Do the following:
 */
class AcceleratorCache : public ThreadHelpBase // attention! Must be the first base class to guarentee right initialize lock ...
{
    //______________________________________
    // const, types

    public:

        //---------------------------------------
        /** TODO document me
            commands -> keys
        */
        typedef ::comphelper::SequenceAsVector< css::awt::KeyEvent > TKeyList;
        typedef BaseHash< TKeyList > TCommand2Keys;

        //---------------------------------------
        /** TODO document me
            keys -> commands
        */
        typedef ::boost::unordered_map< css::awt::KeyEvent ,
                                 ::rtl::OUString    ,
                                 KeyEventHashCode   ,
                                 KeyEventEqualsFunc > TKey2Commands;

    //______________________________________
    // member

    private:

        //---------------------------------------
        /** map commands to keys in relation 1:n.
            First key is interpreted as preferred one! */
        TCommand2Keys m_lCommand2Keys;

        //---------------------------------------
        /** map keys to commands in relation 1:1. */
        TKey2Commands m_lKey2Commands;

    //______________________________________
    // interface

    public:

        //---------------------------------------
        /** @short  creates a new - but empty - cache instance. */
        AcceleratorCache();

        //---------------------------------------
        /** @short  make a copy of this cache.
            @descr  Used for the copy-on-write feature.
        */
        AcceleratorCache(const AcceleratorCache& rCopy);

        //---------------------------------------
        /** @short  does nothing real. */
        virtual ~AcceleratorCache();

        //---------------------------------------
        /** @short  write changes back to the original container.

            @param  rCopy
                    the (changed!) copy, which should be written
                    back to this original container.
          */
        virtual void takeOver(const AcceleratorCache& rCopy);

        //---------------------------------------
        /** TODO document me */
        AcceleratorCache& operator=(const AcceleratorCache& rCopy);

        //---------------------------------------
        /** @short  checks if the specified key exists.

            @param  aKey
                    the key, which should be checked.

            @return [bool]
                    sal_True if the speicfied key exists inside this container.
         */
        virtual sal_Bool hasKey(const css::awt::KeyEvent& aKey) const;
        virtual sal_Bool hasCommand(const ::rtl::OUString& sCommand) const;

        //---------------------------------------
        /** TODO document me */
        virtual TKeyList getAllKeys() const;

        //---------------------------------------
        /** @short  add a new or change an existing key-command pair
                    of this container.

            @param  aKey
                    describe the key.

            @param  sCommand
                    describe the command.
          */
        virtual void setKeyCommandPair(const css::awt::KeyEvent& aKey    ,
                                       const ::rtl::OUString&    sCommand);

        //---------------------------------------
        /** @short  returns the list of keys, which are registered
                    for this command.

            @param  sCommand
                    describe the command.

            @return [TKeyList]
                    the list of registered keys. Can be empty!
          */
        virtual TKeyList getKeysByCommand(const ::rtl::OUString& sCommand) const;

        //---------------------------------------
        /** TODO */
        virtual ::rtl::OUString getCommandByKey(const css::awt::KeyEvent& aKey) const;

        //---------------------------------------
        /** TODO */
        virtual void removeKey(const css::awt::KeyEvent& aKey);
        virtual void removeCommand(const ::rtl::OUString& sCommand);
};

} // namespace framework

#endif // __FRAMEWORK_ACCELERATORS_ACCELERATORCACHE_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
