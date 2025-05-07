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

#ifndef _DBAUI_DETAILPAGES_HXX_
#define _DBAUI_DETAILPAGES_HXX_

#include "adminpages.hxx"
#include "charsets.hxx"
#include "charsetlistbox.hxx"
#include <vcl/field.hxx>
#include <vcl/fixed.hxx>
#include <vcl/lstbox.hxx>
#include <vcl/edit.hxx>
#include <vcl/button.hxx>
#include "TextConnectionHelper.hxx"
#include "admincontrols.hxx"

#include <svtools/dialogcontrolling.hxx>


//.........................................................................
namespace dbaui
{
//.........................................................................
    //=========================================================================
    //= OCommonBehaviourTabPage
    //=========================================================================
    #define     CBTP_NONE                           0x00000000
    #define     CBTP_USE_CHARSET                    0x00000002
    #define     CBTP_USE_OPTIONS                    0x00000004

    /** eases the implementation of tab pages handling user/password and/or character
        set and/or generic options input
        <BR>
        The controls to be used habe to be defined within the resource, as usual, but
        this class does all the handling necessary.
    */
    class OCommonBehaviourTabPage : public OGenericAdministrationPage
    {
    protected:

        FixedText*          m_pOptionsLabel;
        Edit*               m_pOptions;

        FixedLine*          m_pDataConvertFixedLine;
        FixedText*          m_pCharsetLabel;
        CharSetListBox*     m_pCharset;

        FixedLine*          m_pAutoFixedLine;
        CheckBox*           m_pAutoRetrievingEnabled;
        FixedText*          m_pAutoIncrementLabel;
        Edit*               m_pAutoIncrement;
        FixedText*          m_pAutoRetrievingLabel;
        Edit*               m_pAutoRetrieving;

        sal_uInt32          m_nControlFlags;

    public:
        virtual sal_Bool        FillItemSet (SfxItemSet& _rCoreAttrs);

        OCommonBehaviourTabPage(Window* pParent, sal_uInt16 nResId, const SfxItemSet& _rCoreAttrs, sal_uInt32 nControlFlags,bool _bFreeResource = true);
    protected:

            // nControlFlags ist eine Kombination der CBTP_xxx-Konstanten
        virtual ~OCommonBehaviourTabPage();

        // must be overloaded by subclasses, but it isn't pure virtual
        virtual void        implInitControls(const SfxItemSet& _rSet, sal_Bool _bSaveValue);

        // <method>OGenericAdministrationPage::fillControls</method>
        virtual void fillControls(::std::vector< ISaveValueWrapper* >& _rControlList);

        // <method>OGenericAdministrationPage::fillWindows</method>
        virtual void fillWindows(::std::vector< ISaveValueWrapper* >& _rControlList);
    };

    //========================================================================
    //= ODbaseDetailsPage
    //========================================================================
    class ODbaseDetailsPage : public OCommonBehaviourTabPage
    {
    public:
        virtual sal_Bool        FillItemSet ( SfxItemSet& _rCoreAttrs );

        ODbaseDetailsPage(Window* pParent, const SfxItemSet& _rCoreAttrs);
    private:
        // please add new controls also to <method>fillControls</method> or <method>fillWindows</method>
        CheckBox            m_aShowDeleted;
        FixedLine           m_aFL_1;
        FixedText           m_aFT_Message;
        PushButton          m_aIndexes;

        String              m_sDsn;

    protected:

        virtual ~ODbaseDetailsPage();

    protected:
        virtual void implInitControls(const SfxItemSet& _rSet, sal_Bool _bSaveValue);
        virtual void fillControls(::std::vector< ISaveValueWrapper* >& _rControlList);
        virtual void fillWindows(::std::vector< ISaveValueWrapper* >& _rControlList);

    private:
        DECL_LINK( OnButtonClicked, Button * );
    };

    //========================================================================
    //= OAdoDetailsPage
    //========================================================================
    class OAdoDetailsPage : public OCommonBehaviourTabPage
    {
    protected:
        virtual ~OAdoDetailsPage();
    public:

        OAdoDetailsPage( Window* pParent, const SfxItemSet& _rCoreAttrs );
    };

    //========================================================================
    //= OOdbcDetailsPage
    //========================================================================
    class OOdbcDetailsPage : public OCommonBehaviourTabPage
    {
    public:
        virtual sal_Bool        FillItemSet ( SfxItemSet& _rCoreAttrs );

        OOdbcDetailsPage( Window* pParent, const SfxItemSet& _rCoreAttrs );
    protected:
        virtual void implInitControls(const SfxItemSet& _rSet, sal_Bool _bSaveValue);
        virtual void fillControls(::std::vector< ISaveValueWrapper* >& _rControlList);
        virtual void fillWindows(::std::vector< ISaveValueWrapper* >& _rControlList);
    private:
        FixedLine           m_aFL_1;
        CheckBox            m_aUseCatalog;
    };


    //========================================================================
    //= OUserDriverDetailsPage
    //========================================================================
    class OUserDriverDetailsPage : public OCommonBehaviourTabPage
    {
    public:
        virtual sal_Bool        FillItemSet ( SfxItemSet& _rCoreAttrs );

        OUserDriverDetailsPage( Window* pParent, const SfxItemSet& _rCoreAttrs );
    protected:
        virtual void implInitControls(const SfxItemSet& _rSet, sal_Bool _bSaveValue);
        virtual void fillControls(::std::vector< ISaveValueWrapper* >& _rControlList);
        virtual void fillWindows(::std::vector< ISaveValueWrapper* >& _rControlList);
    private:
        FixedText           m_aFTHostname;
        Edit                m_aEDHostname;
        FixedText           m_aPortNumber;
        NumericField        m_aNFPortNumber;
        CheckBox            m_aUseCatalog;
    };

    //========================================================================
    //= OMySQLODBCDetailsPage
    //========================================================================
    class OMySQLODBCDetailsPage : public OCommonBehaviourTabPage
    {
    public:
        OMySQLODBCDetailsPage( Window* pParent, const SfxItemSet& _rCoreAttrs );
    };

    //========================================================================
    //= OGeneralSpecialJDBCDetailsPage
    //========================================================================
    class OGeneralSpecialJDBCDetailsPage : public OCommonBehaviourTabPage
    {
    public:
        OGeneralSpecialJDBCDetailsPage(   Window* pParent
                                        , sal_uInt16 _nResId
                                        , const SfxItemSet& _rCoreAttrs
                                        , sal_uInt16 _nPortId
                                        );

    protected:


        virtual sal_Bool FillItemSet( SfxItemSet& _rCoreAttrs );
        virtual void implInitControls(const SfxItemSet& _rSet, sal_Bool _bSaveValue);
        virtual void fillControls(::std::vector< ISaveValueWrapper* >& _rControlList);
        virtual void fillWindows(::std::vector< ISaveValueWrapper* >& _rControlList);

        DECL_LINK(OnTestJavaClickHdl,PushButton*);
        DECL_LINK(OnEditModified,Edit*);

        FixedLine           m_aFL_1;
        FixedText           m_aFTHostname;
        Edit                m_aEDHostname;
        FixedText           m_aPortNumber;
        NumericField        m_aNFPortNumber;
        FixedText           m_aFTSocket;
        Edit                m_aEDSocket;

        FixedText           m_aFTDriverClass;
        Edit                m_aEDDriverClass;
        PushButton          m_aTestJavaDriver;

        String              m_sDefaultJdbcDriverName;
        sal_uInt16              m_nPortId;
        bool                m_bUseClass;
    };

    //========================================================================
    //= MySQLNativePage
    //========================================================================
    class MySQLNativePage : public OCommonBehaviourTabPage
    {
    public:
        MySQLNativePage(    Window* pParent,
                            const SfxItemSet& _rCoreAttrs );

    private:
        FixedLine           m_aSeparator1;
        MySQLNativeSettings m_aMySQLSettings;

        FixedLine           m_aSeparator2;
        FixedText           m_aUserNameLabel;
        Edit                m_aUserName;
        CheckBox            m_aPasswordRequired;

    protected:
        virtual sal_Bool FillItemSet( SfxItemSet& _rCoreAttrs );
        virtual void implInitControls(const SfxItemSet& _rSet, sal_Bool _bSaveValue);
        virtual void fillControls(::std::vector< ISaveValueWrapper* >& _rControlList);
        virtual void fillWindows(::std::vector< ISaveValueWrapper* >& _rControlList);
    };

    //========================================================================
    //= OOdbcDetailsPage
    //========================================================================
    class OLDAPDetailsPage : public OCommonBehaviourTabPage
    {
    public:
        virtual sal_Bool        FillItemSet ( SfxItemSet& _rCoreAttrs );

        OLDAPDetailsPage( Window* pParent, const SfxItemSet& _rCoreAttrs );
    protected:
        virtual void implInitControls(const SfxItemSet& _rSet, sal_Bool _bSaveValue);
        virtual void fillControls(::std::vector< ISaveValueWrapper* >& _rControlList);
        virtual void fillWindows(::std::vector< ISaveValueWrapper* >& _rControlList);
    private:
        FixedLine           m_aFL_1;
        FixedText           m_aBaseDN;
        Edit                m_aETBaseDN;
        CheckBox            m_aCBUseSSL;
        FixedText           m_aPortNumber;
        NumericField        m_aNFPortNumber;
        FixedText           m_aFTRowCount;
        NumericField        m_aNFRowCount;

        sal_Int32           m_iSSLPort;
        sal_Int32           m_iNormalPort;
        DECL_LINK( OnCheckBoxClick, CheckBox * );
    };

    //========================================================================
    //= OMozillaDetailsPage Detail page for Mozilla and Thunderbird addressbook
    //========================================================================
    class OMozillaDetailsPage : public OCommonBehaviourTabPage
    {
    protected:
        virtual ~OMozillaDetailsPage();
    public:

        OMozillaDetailsPage( Window* pParent, const SfxItemSet& _rCoreAttrs );
    };

    //========================================================================
    //= OTextDetailsPage
    //========================================================================
    class OTextDetailsPage : public OCommonBehaviourTabPage
    {
    public:
        virtual sal_Bool        FillItemSet ( SfxItemSet& _rCoreAttrs );

        OTextDetailsPage( Window* pParent, const SfxItemSet& _rCoreAttrs );
        OTextConnectionHelper*  m_pTextConnectionHelper;

    private:

        String      m_aFieldSeparatorList;
        String      m_aTextSeparatorList;
        String      m_aTextNone;
    protected:
        virtual ~OTextDetailsPage();
        virtual sal_Bool prepareLeave();

        virtual void implInitControls(const SfxItemSet& _rSet, sal_Bool _bSaveValue);
        virtual void fillControls(::std::vector< ISaveValueWrapper* >& _rControlList);
        virtual void fillWindows(::std::vector< ISaveValueWrapper* >& _rControlList);

    private:
    };

//.........................................................................
}   // namespace dbaui
//.........................................................................

#endif // _DBAUI_DETAILPAGES_HXX_

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
