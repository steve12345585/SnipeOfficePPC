/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Version: MPL 1.1 / GPLv3+ / LGPLv3+
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License or as specified alternatively below. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * Major Contributor(s):
 * [ Copyright (C) 2012 SUSE <cbosdonnat@suse.com> (initial developer) ]
 *
 * All Rights Reserved.
 *
 * For minor contributions see the git repository.
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPLv3+"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
 * in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
 * instead of those above.
 */
#ifndef _SERVERDETAILSCONTROLS_HXX
#define _SERVERDETAILSCONTROLS_HXX

#include <map>

#include <com/sun/star/ucb/XCommandEnvironment.hpp>

#include <tools/urlobj.hxx>
#include <vcl/builder.hxx>
#include <vcl/button.hxx>
#include <vcl/edit.hxx>
#include <vcl/field.hxx>
#include <vcl/fixed.hxx>
#include <vcl/layout.hxx>

class DetailsContainer
{
    private:
        Link m_aChangeHdl;
        VclFrame*       m_pFrame;

    public:
        DetailsContainer( VclBuilderContainer* pBuilder, const rtl::OString& rFrame );
        virtual ~DetailsContainer( );

        void setChangeHdl( const Link& rLink ) { m_aChangeHdl = rLink; }

        virtual void show( bool bShow = true );
        virtual INetURLObject getUrl( );

        /** Try to split the URL in the controls of that container.

            \param sUrl the URL to split
            \return true if the split worked, false otherwise.
         */
        virtual bool setUrl( const INetURLObject& rUrl );

        virtual void setUsername( const rtl::OUString& /*rUsername*/ ) { };

    protected:
        void notifyChange( );
        DECL_LINK ( ValueChangeHdl, void * );
};

class HostDetailsContainer : public DetailsContainer
{
    private:
        sal_uInt16 m_nDefaultPort;
        rtl::OUString m_sScheme;

    protected:
        Edit*           m_pEDHost;
        NumericField*   m_pEDPort;
        Edit*           m_pEDPath;

    public:
        HostDetailsContainer( VclBuilderContainer* pBuilder, sal_uInt16 nPort, rtl::OUString sScheme );
        virtual ~HostDetailsContainer( ) { };

        virtual void show( bool bShow = true );
        virtual INetURLObject getUrl( );
        virtual bool setUrl( const INetURLObject& rUrl );

    protected:
        void setScheme( rtl::OUString sScheme ) { m_sScheme = sScheme; }

        /** Verifies that the schement split from the URL can be handled by
            the container and set the proper controls accordingly if needed.
          */
        virtual bool verifyScheme( const rtl::OUString& rScheme );
};

class DavDetailsContainer : public HostDetailsContainer
{
    private:
        CheckBox*   m_pCBDavs;

    public:
        DavDetailsContainer( VclBuilderContainer* pBuilder );
        ~DavDetailsContainer( ) { };

        virtual void show( bool bShow = true );

    protected:
        virtual bool verifyScheme( const rtl::OUString& rScheme );

    private:
        DECL_LINK ( ToggledDavsHdl, CheckBox * pCheckBox );
};

class SmbDetailsContainer : public DetailsContainer
{
    private:
        Edit*           m_pEDHost;
        Edit*           m_pEDShare;
        Edit*           m_pEDPath;

    public:
        SmbDetailsContainer( VclBuilderContainer* pBuilder );
        ~SmbDetailsContainer( ) { };

        virtual INetURLObject getUrl( );
        virtual bool setUrl( const INetURLObject& rUrl );
};

class CmisDetailsContainer : public DetailsContainer
{
    private:
        rtl::OUString m_sUsername;
        com::sun::star::uno::Reference< com::sun::star::ucb::XCommandEnvironment > m_xCmdEnv;
        std::vector< rtl::OUString > m_aServerTypesURLs;
        std::vector< rtl::OUString > m_aRepoIds;
        rtl::OUString m_sRepoId;

        Edit*       m_pEDBinding;
        ListBox*    m_pLBRepository;
        Button*     m_pBTRepoRefresh;
        ListBox*    m_pLBServerType;
        Edit*       m_pEDPath;

    public:
        CmisDetailsContainer( VclBuilderContainer* pBuilder );
        ~CmisDetailsContainer( ) { };

        virtual INetURLObject getUrl( );
        virtual bool setUrl( const INetURLObject& rUrl );
        virtual void setUsername( const rtl::OUString& rUsername );

    private:
        void selectRepository( );
        DECL_LINK ( SelectServerTypeHdl, void * );
        DECL_LINK ( RefreshReposHdl, void * );
        DECL_LINK ( SelectRepoHdl, void * );
};

#endif
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
