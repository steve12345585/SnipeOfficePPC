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
#ifndef FORMS_SOURCE_RICHTEXT_RICHTEXTENGINE_HXX
#define FORMS_SOURCE_RICHTEXT_RICHTEXTENGINE_HXX

#include <editeng/editeng.hxx>
#include <tools/link.hxx>

#include <vector>

class SfxItemPool;
class EditStatus;
//........................................................................
namespace frm
{
//........................................................................

    //====================================================================
    //= IEngineStatusListener
    //====================================================================
    class IEngineStatusListener
    {
    public:
        virtual void EditEngineStatusChanged( const EditStatus& _rStatus ) = 0;

    protected:
        ~IEngineStatusListener() {}
    };

    //====================================================================
    //= RichTextEngine
    //====================================================================
    class RichTextEngine : public EditEngine
    {
    private:
        SfxItemPool*                            m_pEnginePool;
        ::std::vector< IEngineStatusListener* > m_aStatusListeners;

    public:
        static  RichTextEngine* Create();
                RichTextEngine* Clone();

                ~RichTextEngine( );

        // for multiplexing the StatusChanged events of the edit engine
        void registerEngineStatusListener( IEngineStatusListener* _pListener );
        void revokeEngineStatusListener( IEngineStatusListener* _pListener );

        inline SfxItemPool* getPool() { return m_pEnginePool; }

    protected:
        /** constructs a new RichTextEngine. The instances takes the ownership of the given SfxItemPool
        */
        RichTextEngine( SfxItemPool* _pPool );

    private:
        RichTextEngine( );                                  // never implemented
        RichTextEngine( const RichTextEngine& );            // never implemented
        RichTextEngine& operator=( const RichTextEngine& ); // never implemented

    private:
        DECL_LINK( EditEngineStatusChanged, EditStatus* );
    };

//........................................................................
} // namespace frm
//........................................................................

#endif // FORMS_SOURCE_RICHTEXT_RICHTEXTENGINE_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
