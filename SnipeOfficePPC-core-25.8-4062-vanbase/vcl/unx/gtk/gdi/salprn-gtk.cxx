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
 * Copyright (C) 2011 Red Hat, Inc., Caolán McNamara <caolanm@redhat.com>
 *  (initial developer)
 * Copyright (C) 2011 Red Hat, Inc., David Tardon <dtardon@redhat.com> ]
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

#include "gtkprintwrapper.hxx"

#include "unx/gtk/gtkdata.hxx"
#include "unx/gtk/gtkframe.hxx"
#include "unx/gtk/gtkinst.hxx"
#include "unx/gtk/gtkprn.hxx"

#include "vcl/configsettings.hxx"
#include "vcl/help.hxx"
#include "vcl/print.hxx"
#include "vcl/svapp.hxx"
#include "vcl/window.hxx"

#include <gtk/gtk.h>
#include <gtk/gtkunixprint.h>

#include <comphelper/processfactory.hxx>

#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/container/XNamed.hpp>
#include <com/sun/star/document/XExporter.hpp>
#include <com/sun/star/document/XFilter.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/io/XOutputStream.hpp>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <com/sun/star/sheet/XSpreadsheetView.hpp>
#include <com/sun/star/view/PrintableState.hpp>
#include <com/sun/star/view/XSelectionSupplier.hpp>

#include <officecfg/Office/Common.hxx>

#include <rtl/ustring.hxx>

#include <unotools/streamwrap.hxx>

#include <cstring>
#include <map>

namespace frame = com::sun::star::frame;
namespace beans = com::sun::star::beans;
namespace container = com::sun::star::container;
namespace uno = com::sun::star::uno;
namespace document = com::sun::star::document;
namespace sheet = com::sun::star::sheet;
namespace io = com::sun::star::io;
namespace view = com::sun::star::view;

using vcl::unx::GtkPrintWrapper;

using uno::UNO_QUERY;

class GtkPrintDialog
{
public:
    GtkPrintDialog(vcl::PrinterController& io_rController);
    bool run();
    GtkPrinter* getPrinter() const
    {
        return m_pWrapper->print_unix_dialog_get_selected_printer(GTK_PRINT_UNIX_DIALOG(m_pDialog));
    }
    GtkPrintSettings* getSettings() const
    {
        return m_pWrapper->print_unix_dialog_get_settings(GTK_PRINT_UNIX_DIALOG(m_pDialog));
    }
    void updateControllerPrintRange();
#if 0
    void ExportAsPDF(const rtl::OUString &rFileURL, GtkPrintSettings* pSettings) const;
#endif
    ~GtkPrintDialog();

    static void UIOption_CheckHdl(GtkWidget* i_pWidget, GtkPrintDialog* io_pThis)
    {
        io_pThis->impl_UIOption_CheckHdl(i_pWidget);
    }
    static void UIOption_RadioHdl(GtkWidget* i_pWidget, GtkPrintDialog* io_pThis)
    {
        io_pThis->impl_UIOption_RadioHdl(i_pWidget);
    }
    static void UIOption_SelectHdl(GtkWidget* i_pWidget, GtkPrintDialog* io_pThis)
    {
        io_pThis->impl_UIOption_SelectHdl(i_pWidget);
    }

private:
    beans::PropertyValue* impl_queryPropertyValue(GtkWidget* i_pWidget) const;
    void impl_checkOptionalControlDependencies();

    void impl_UIOption_CheckHdl(GtkWidget* i_pWidget);
    void impl_UIOption_RadioHdl(GtkWidget* i_pWidget);
    void impl_UIOption_SelectHdl(GtkWidget* i_pWidget);

    void impl_initDialog();
    void impl_initCustomTab();
    void impl_initPrintContent(uno::Sequence<sal_Bool> const& i_rDisabled);

    void impl_readFromSettings();
    void impl_storeToSettings() const;

private:
    GtkWidget* m_pDialog;
    vcl::PrinterController& m_rController;
    std::map<GtkWidget*, rtl::OUString> m_aControlToPropertyMap;
    std::map<GtkWidget*, sal_Int32> m_aControlToNumValMap;
    boost::shared_ptr<GtkPrintWrapper> m_pWrapper;
};


struct GtkSalPrinter_Impl
{
    rtl::OString m_sSpoolFile;
    rtl::OUString m_sJobName;
    GtkPrinter* m_pPrinter;
    GtkPrintSettings* m_pSettings;

    GtkSalPrinter_Impl();
    ~GtkSalPrinter_Impl();
};


GtkSalPrinter_Impl::GtkSalPrinter_Impl()
    : m_pPrinter(0)
    , m_pSettings(0)
{
}


GtkSalPrinter_Impl::~GtkSalPrinter_Impl()
{
    if (m_pPrinter)
    {
        g_object_unref(G_OBJECT(m_pPrinter));
        m_pPrinter = NULL;
    }
    if (m_pSettings)
    {
        g_object_unref(G_OBJECT(m_pSettings));
        m_pSettings = NULL;
    }
}

namespace
{

GtkInstance const&
lcl_getGtkSalInstance()
{
    // we _know_ this is GtkInstance
    return *static_cast<GtkInstance*>(GetGtkSalData()->m_pInstance);
}

bool
lcl_useSystemPrintDialog()
{
    return officecfg::Office::Common::Misc::UseSystemPrintDialog::get()
        && officecfg::Office::Common::Misc::ExperimentalMode::get()
        && lcl_getGtkSalInstance().getPrintWrapper()->supportsPrinting();
}

}

GtkSalPrinter::GtkSalPrinter(SalInfoPrinter* const i_pInfoPrinter)
    : PspSalPrinter(i_pInfoPrinter)
{
}


bool
GtkSalPrinter::impl_doJob(
        const rtl::OUString* const i_pFileName,
        const rtl::OUString& i_rJobName,
        const rtl::OUString& i_rAppName,
        ImplJobSetup* const io_pSetupData,
        const int i_nCopies,
        const bool i_bCollate,
        vcl::PrinterController& io_rController)
{
    io_rController.setJobState(view::PrintableState_JOB_STARTED);
    io_rController.jobStarted();
    const bool bJobStarted(
            PspSalPrinter::StartJob(i_pFileName, i_rJobName, i_rAppName,
                i_nCopies, i_bCollate, true, io_pSetupData))
        ;

    if (bJobStarted)
    {
        io_rController.createProgressDialog();
        const int nPages(io_rController.getFilteredPageCount());
        for (int nPage(0); nPage != nPages; ++nPage)
        {
            if (nPage == nPages - 1)
                io_rController.setLastPage(sal_True);
            io_rController.printFilteredPage(nPage);
        }
        io_rController.setJobState(view::PrintableState_JOB_COMPLETED);
    }

    return bJobStarted;
}


sal_Bool
GtkSalPrinter::StartJob(
        const rtl::OUString* const i_pFileName,
        const rtl::OUString& i_rJobName,
        const rtl::OUString& i_rAppName,
        ImplJobSetup* io_pSetupData,
        vcl::PrinterController& io_rController)
{
    if (!lcl_useSystemPrintDialog())
        return PspSalPrinter::StartJob(i_pFileName, i_rJobName, i_rAppName, io_pSetupData, io_rController);

    assert(!m_pImpl);

    m_pImpl.reset(new GtkSalPrinter_Impl());
    m_pImpl->m_sJobName = i_rJobName;

    rtl::OString sFileName;
    if (i_pFileName)
        sFileName = rtl::OUStringToOString(*i_pFileName, osl_getThreadTextEncoding());

    GtkPrintDialog aDialog(io_rController);
    if (!aDialog.run())
    {
        io_rController.abortJob();
        return sal_False;
    }
    aDialog.updateControllerPrintRange();
    m_pImpl->m_pPrinter = aDialog.getPrinter();
    m_pImpl->m_pSettings = aDialog.getSettings();

#if 0
    if (const gchar *uri = gtk_print_settings_get(m_pImpl->m_pSettings, GTK_PRINT_SETTINGS_OUTPUT_URI))
    {
        const gchar *pStr = gtk_print_settings_get(m_pImpl->m_pSettings, GTK_PRINT_SETTINGS_OUTPUT_FILE_FORMAT);
        if (pStr && !strcmp(pStr, "pdf"))
        {
            aDialog.ExportAsPDF(rtl::OUString((const sal_Char *)uri, strlen((const sal_Char *)uri), RTL_TEXTENCODING_UTF8), m_pImpl->m_pSettings);
            bDoJob = false;
        }
    }

    if (!bDoJob)
        return false;
#endif
    int nCopies = 1;
    bool bCollate = false;

    //To-Do proper name, watch for encodings
    sFileName = rtl::OString("/tmp/hacking.ps");
    m_pImpl->m_sSpoolFile = sFileName;

    rtl::OUString aFileName = rtl::OStringToOUString(sFileName, osl_getThreadTextEncoding());

    //To-Do, swap ps/pdf for gtk_printer_accepts_ps()/gtk_printer_accepts_pdf() ?

    return impl_doJob(&aFileName, i_rJobName, i_rAppName, io_pSetupData, nCopies, bCollate, io_rController);
}


sal_Bool
GtkSalPrinter::EndJob()
{
    sal_Bool bRet = PspSalPrinter::EndJob();

    if (!lcl_useSystemPrintDialog())
        return bRet;

    assert(m_pImpl);

    if (!bRet || m_pImpl->m_sSpoolFile.isEmpty())
        return bRet;

    boost::shared_ptr<GtkPrintWrapper> const pWrapper(lcl_getGtkSalInstance().getPrintWrapper());

    GtkPageSetup* pPageSetup = pWrapper->page_setup_new();
#if 0
    //todo
    gtk_page_setup_set_orientation(pPageSetup,);
    gtk_page_setup_set_paper_size(pPageSetup,);
    gtk_page_setup_set_top_margin(pPageSetup,);
    gtk_page_setup_set_bottom_margin(pPageSetup,);
    gtk_page_setup_set_left_margin(pPageSetup,);
    gtk_page_setup_set_right_margin(pPageSetup,);
#endif

    GtkPrintJob* const pJob = pWrapper->print_job_new(
        rtl::OUStringToOString(m_pImpl->m_sJobName, RTL_TEXTENCODING_UTF8).getStr(),
        m_pImpl->m_pPrinter, m_pImpl->m_pSettings, pPageSetup);

    GError* error = NULL;
    bRet = pWrapper->print_job_set_source_file(pJob, m_pImpl->m_sSpoolFile.getStr(), &error);
    if (bRet)
        pWrapper->print_job_send(pJob, NULL, NULL, NULL);
    else
    {
        //To-Do, do something with this
        fprintf(stderr, "error was %s\n", error->message);
        g_error_free(error);
    }

    g_object_unref(pPageSetup);
    m_pImpl.reset();

    //To-Do, remove temp spool file

    return bRet;
}


namespace
{

void
lcl_setHelpText(
        GtkWidget* const io_pWidget,
        const uno::Sequence<rtl::OUString>& i_rHelpTexts,
        const sal_Int32 i_nIndex)
{
#if GTK_CHECK_VERSION(2,12,0)
    if (i_nIndex >= 0 && i_nIndex < i_rHelpTexts.getLength())
        gtk_widget_set_tooltip_text(io_pWidget,
            rtl::OUStringToOString(i_rHelpTexts.getConstArray()[i_nIndex], RTL_TEXTENCODING_UTF8).getStr());
#else
    (void)io_pWidget;
    (void)i_rHelpTexts;
    (void)i_nIndex;
#endif
}


static GtkWidget*
lcl_makeFrame(
        GtkWidget* const i_pChild,
        const rtl::OUString &i_rText,
        const uno::Sequence<rtl::OUString> &i_rHelpTexts,
        sal_Int32* const io_pCurHelpText)
{
    GtkWidget* const pLabel = gtk_label_new(NULL);
    lcl_setHelpText(pLabel, i_rHelpTexts, !io_pCurHelpText ? 0 : (*io_pCurHelpText)++);
    gtk_misc_set_alignment(GTK_MISC(pLabel), 0.0, 0.5);

    {
        gchar* const pText = g_markup_printf_escaped("<b>%s</b>",
            rtl::OUStringToOString(i_rText, RTL_TEXTENCODING_UTF8).getStr());
        gtk_label_set_markup_with_mnemonic(GTK_LABEL(pLabel), pText);
        g_free(pText);
    }

    GtkWidget* const pFrame = gtk_vbox_new(FALSE, 6);
    gtk_box_pack_start(GTK_BOX(pFrame), pLabel, FALSE, FALSE, 0);

    GtkWidget* const pAlignment = gtk_alignment_new(0.0, 0.0, 1.0, 1.0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(pAlignment), 0, 0, 12, 0);
    gtk_box_pack_start(GTK_BOX(pFrame), pAlignment, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(pAlignment), i_pChild);
    return pFrame;
}

void
lcl_extractHelpTextsOrIds(
        const beans::PropertyValue& rEntry,
        uno::Sequence<rtl::OUString>& rHelpStrings)
{
    if (!(rEntry.Value >>= rHelpStrings))
    {
        rtl::OUString aHelpString;
        if ((rEntry.Value >>= aHelpString))
        {
            rHelpStrings.realloc(1);
            *rHelpStrings.getArray() = aHelpString;
        }
    }
}

GtkWidget*
lcl_combo_box_text_new()
{
#if GTK_CHECK_VERSION(3,0,0)
    return gtk_combo_box_text_new();
#else
    return gtk_combo_box_new_text();
#endif
}

void
lcl_combo_box_text_append(GtkWidget* const pWidget, gchar const* const pText)
{
#if GTK_CHECK_VERSION(3,0,0)
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(pWidget), pText);
#else
    gtk_combo_box_append_text(GTK_COMBO_BOX(pWidget), pText);
#endif
}

}

GtkPrintDialog::GtkPrintDialog(vcl::PrinterController& io_rController)
    : m_rController(io_rController)
    , m_pWrapper(lcl_getGtkSalInstance().getPrintWrapper())
{
    assert(m_pWrapper->supportsPrinting());
    impl_initDialog();
    impl_initCustomTab();
    impl_readFromSettings();
}


void
GtkPrintDialog::impl_initDialog()
{
    //To-Do, like fpicker, set UI language
    m_pDialog = m_pWrapper->print_unix_dialog_new(NULL, NULL);

    Window* const pTopWindow(Application::GetActiveTopWindow());
    if (pTopWindow)
    {
        GtkSalFrame* const pFrame(dynamic_cast<GtkSalFrame*>(pTopWindow->ImplGetFrame()));
        if (pFrame)
        {
            GtkWindow* const pParent(GTK_WINDOW(pFrame->getWindow()));
            if (pParent)
                gtk_window_set_transient_for(GTK_WINDOW(m_pDialog), pParent);
        }
    }

    m_pWrapper->print_unix_dialog_set_manual_capabilities(GTK_PRINT_UNIX_DIALOG(m_pDialog),
        GtkPrintCapabilities(GTK_PRINT_CAPABILITY_COPIES
            | GTK_PRINT_CAPABILITY_COLLATE
            | GTK_PRINT_CAPABILITY_REVERSE
            | GTK_PRINT_CAPABILITY_GENERATE_PS
#if GTK_CHECK_VERSION(2,12,0)
            | GTK_PRINT_CAPABILITY_NUMBER_UP
#endif
#if GTK_CHECK_VERSION(2,14,0)
            | GTK_PRINT_CAPABILITY_NUMBER_UP_LAYOUT
#endif
       ));
}


void
GtkPrintDialog::impl_initCustomTab()
{
    typedef std::map<rtl::OUString, GtkWidget*> DependencyMap_t;
    typedef std::vector<std::pair<GtkWidget*, rtl::OUString> > CustomTabs_t;

    const uno::Sequence<beans::PropertyValue>& rOptions(m_rController.getUIOptions());
    DependencyMap_t aPropertyToDependencyRowMap;
    CustomTabs_t aCustomTabs;
    GtkWidget* pCurParent = NULL;
    GtkWidget* pCurTabPage = NULL;
    GtkWidget* pCurSubGroup = NULL;
    GtkWidget* pStandardPrintRangeContainer = NULL;
    bool bIgnoreSubgroup = false;
    for (int i = 0; i != rOptions.getLength(); i++)
    {
        uno::Sequence<beans::PropertyValue> aOptProp;
        rOptions[i].Value >>= aOptProp;

        rtl::OUString aCtrlType;
        rtl::OUString aText;
        rtl::OUString aPropertyName;
        uno::Sequence<rtl::OUString> aChoices;
        uno::Sequence<sal_Bool> aChoicesDisabled;
        uno::Sequence<rtl::OUString> aHelpTexts;
        sal_Int64 nMinValue = 0, nMaxValue = 0;
        sal_Int32 nCurHelpText = 0;
        rtl::OUString aDependsOnName;
        sal_Int32 nDependsOnValue = 0;
        sal_Bool bUseDependencyRow = sal_False;
        sal_Bool bIgnore = sal_False;
        GtkWidget* pGroup = NULL;
        bool bGtkInternal = false;

        for (int n = 0; n != aOptProp.getLength(); n++)
        {
            const beans::PropertyValue& rEntry(aOptProp[ n ]);
            if ( rEntry.Name == "Text" )
            {
                rtl::OUString aValue;
                rEntry.Value >>= aValue;
                aText = aValue.replace('~', '_');
            }
            else if ( rEntry.Name == "ControlType" )
                rEntry.Value >>= aCtrlType;
            else if ( rEntry.Name == "Choices" )
                rEntry.Value >>= aChoices;
            else if ( rEntry.Name == "ChoicesDisabled" )
                rEntry.Value >>= aChoicesDisabled;
            else if ( rEntry.Name == "Property" )
            {
                beans::PropertyValue aVal;
                rEntry.Value >>= aVal;
                aPropertyName = aVal.Name;
            }
            else if ( rEntry.Name == "DependsOnName" )
                rEntry.Value >>= aDependsOnName;
            else if ( rEntry.Name == "DependsOnEntry" )
                rEntry.Value >>= nDependsOnValue;
            else if ( rEntry.Name == "AttachToDependency" )
                rEntry.Value >>= bUseDependencyRow;
            else if ( rEntry.Name == "MinValue" )
                rEntry.Value >>= nMinValue;
            else if ( rEntry.Name == "MaxValue" )
                rEntry.Value >>= nMaxValue;
            else if ( rEntry.Name == "HelpId" )
            {
                uno::Sequence<rtl::OUString> aHelpIds;
                lcl_extractHelpTextsOrIds(rEntry, aHelpIds);
                Help* const pHelp = Application::GetHelp();
                if (pHelp)
                {
                    const int nLen = aHelpIds.getLength();
                    aHelpTexts.realloc(nLen);
                    for (int j = 0; j != nLen; ++j)
                        aHelpTexts[j] = pHelp->GetHelpText(aHelpIds[j], 0);
                }
                else // fallback
                    aHelpTexts = aHelpIds;
            }
            else if ( rEntry.Name == "HelpText" )
                lcl_extractHelpTextsOrIds(rEntry, aHelpTexts);
            else if ( rEntry.Name == "InternalUIOnly" )
                rEntry.Value >>= bIgnore;
            else if ( rEntry.Name == "Enabled" )
            {
                // Ignore this. We use UIControlOptions::isUIOptionEnabled
                // to check whether a control should be enabled.
            }
            else if ( rEntry.Name == "GroupingHint" )
            {
                // Ignore this. We cannot add/modify controls to/on existing
                // tabs of the Gtk print dialog.
            }
            else
            {
                SAL_INFO("vcl.gtk", "unhandled UI option entry: " << rEntry.Name);
            }
        }

        if ( aPropertyName == "PrintContent" )
            bGtkInternal = true;

        if (aCtrlType.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("Group")) || !pCurParent)
        {
            pCurTabPage = gtk_vbox_new(FALSE, 12);
            gtk_container_set_border_width(GTK_CONTAINER(pCurTabPage), 6);
            lcl_setHelpText(pCurTabPage, aHelpTexts, 0);

            pCurParent = pCurTabPage;
            aCustomTabs.push_back(std::make_pair(pCurTabPage, aText));
        }
        else if (aCtrlType.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("Subgroup")) && (pCurParent /*|| bOnJobPageValue*/))
        {
            bIgnoreSubgroup = bIgnore;
            if (bIgnore)
                continue;
            pCurParent = gtk_vbox_new(FALSE, 12);
            gtk_container_set_border_width(GTK_CONTAINER(pCurParent), 0);

            pCurSubGroup = lcl_makeFrame(pCurParent, aText, aHelpTexts, NULL);
            gtk_box_pack_start(GTK_BOX(pCurTabPage), pCurSubGroup, FALSE, FALSE, 0);
        }
        // special case: we need to map these to controls of the gtk print dialog
        else if (bGtkInternal)
        {
            if ( aPropertyName == "PrintContent" )
            {
                // What to print? And, more importantly, is there a selection?
                impl_initPrintContent(aChoicesDisabled);
            }
        }
        else if (bIgnoreSubgroup || bIgnore)
            continue;
        else
        {
            // change handlers for all the controls set up in this block
            // should be set _after_ the control has been made (in)active,
            // because:
            // 1. value of the property is _known_--we are using it to
            //    _set_ the control, right?--no need to change it back .-)
            // 2. it may cause warning because the widget may not
            //    have been placed in m_aControlToPropertyMap yet

            GtkWidget* pWidget = NULL;
            beans::PropertyValue* pVal = NULL;
            if (aCtrlType.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("Bool")) && pCurParent)
            {
                pWidget = gtk_check_button_new_with_mnemonic(
                    rtl::OUStringToOString(aText, RTL_TEXTENCODING_UTF8).getStr());
                lcl_setHelpText(pWidget, aHelpTexts, 0);
                m_aControlToPropertyMap[pWidget] = aPropertyName;

                sal_Bool bVal = sal_False;
                pVal = m_rController.getValue(aPropertyName);
                if (pVal)
                    pVal->Value >>= bVal;
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pWidget), bVal);
                gtk_widget_set_sensitive(pWidget,
                    m_rController.isUIOptionEnabled(aPropertyName) && pVal != NULL);
                g_signal_connect(pWidget, "toggled", G_CALLBACK(GtkPrintDialog::UIOption_CheckHdl), this);
            }
            else if (aCtrlType.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("Radio")) && pCurParent)
            {
                GtkWidget* const pVbox = gtk_vbox_new(FALSE, 12);
                gtk_container_set_border_width(GTK_CONTAINER(pVbox), 0);

                if (!aText.isEmpty())
                    pGroup = lcl_makeFrame(pVbox, aText, aHelpTexts, &nCurHelpText);

                sal_Int32 nSelectVal = 0;
                pVal = m_rController.getValue(aPropertyName);
                if (pVal && pVal->Value.hasValue())
                    pVal->Value >>= nSelectVal;

                for (sal_Int32 m = 0; m != aChoices.getLength(); m++)
                {
                    pWidget = gtk_radio_button_new_with_mnemonic_from_widget(
                        GTK_RADIO_BUTTON(m == 0 ? NULL : pWidget),
                        rtl::OUStringToOString(aChoices[m].replace('~', '_'), RTL_TEXTENCODING_UTF8).getStr());
                    lcl_setHelpText(pWidget, aHelpTexts, nCurHelpText++);
                    m_aControlToPropertyMap[pWidget] = aPropertyName;
                    m_aControlToNumValMap[pWidget] = m;
                    GtkWidget* const pRow = gtk_hbox_new(FALSE, 12);
                    gtk_box_pack_start(GTK_BOX(pVbox), pRow, FALSE, FALSE, 0);
                    gtk_box_pack_start(GTK_BOX(pRow), pWidget, FALSE, FALSE, 0);
                    aPropertyToDependencyRowMap[aPropertyName + rtl::OUString::valueOf(m)] = pRow;
                    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pWidget), m == nSelectVal);
                    gtk_widget_set_sensitive(pWidget,
                        m_rController.isUIOptionEnabled(aPropertyName) && pVal != NULL);
                    g_signal_connect(pWidget, "toggled",
                            G_CALLBACK(GtkPrintDialog::UIOption_RadioHdl), this);
                }

                if (pGroup)
                    pWidget = pGroup;
                else
                    pWidget = pVbox;
            }
            else if ((aCtrlType.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("List"))   ||
                       aCtrlType.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("Range"))  ||
                       aCtrlType.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("Edit"))
                    ) && pCurParent)
            {
                GtkWidget* const pHbox = gtk_hbox_new(FALSE, 12);
                gtk_container_set_border_width(GTK_CONTAINER(pHbox), 0);

                if ( aCtrlType == "List" )
                {
                   pWidget = lcl_combo_box_text_new();

                   for (sal_Int32 m = 0; m != aChoices.getLength(); m++)
                   {
                       lcl_combo_box_text_append(pWidget,
                           rtl::OUStringToOString(aChoices[m], RTL_TEXTENCODING_UTF8).getStr());
                   }

                   sal_Int32 nSelectVal = 0;
                   pVal = m_rController.getValue(aPropertyName);
                   if (pVal && pVal->Value.hasValue())
                       pVal->Value >>= nSelectVal;
                   gtk_combo_box_set_active(GTK_COMBO_BOX(pWidget), nSelectVal);
                   g_signal_connect(pWidget, "changed", G_CALLBACK(GtkPrintDialog::UIOption_SelectHdl), this);
                }
                else if (aCtrlType.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("Edit")) && pCurParent)
                {
                   pWidget = gtk_entry_new();

                   rtl::OUString aCurVal;
                   pVal = m_rController.getValue(aPropertyName);
                   if (pVal && pVal->Value.hasValue())
                       pVal->Value >>= aCurVal;
                   gtk_entry_set_text(GTK_ENTRY(pWidget),
                       rtl::OUStringToOString(aCurVal, RTL_TEXTENCODING_UTF8).getStr());
                }
                else if (aCtrlType.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("Range")) && pCurParent)
                {
                    pWidget = gtk_spin_button_new_with_range(nMinValue, nMaxValue, 1.0);

                    sal_Int64 nCurVal = 0;
                    pVal = m_rController.getValue(aPropertyName);
                    if (pVal && pVal->Value.hasValue())
                        pVal->Value >>= nCurVal;
                    gtk_spin_button_set_value(GTK_SPIN_BUTTON(pWidget), nCurVal);
                }

                lcl_setHelpText(pWidget, aHelpTexts, 0);
                m_aControlToPropertyMap[pWidget] = aPropertyName;

                gtk_widget_set_sensitive(pWidget,
                    m_rController.isUIOptionEnabled(aPropertyName) && pVal != NULL);

                if (!aText.isEmpty())
                {
                    GtkWidget* const pLabel = gtk_label_new_with_mnemonic(
                        rtl::OUStringToOString(aText, RTL_TEXTENCODING_UTF8).getStr());
                    gtk_label_set_mnemonic_widget(GTK_LABEL(pLabel), pWidget);
                    gtk_box_pack_start(GTK_BOX(pHbox), pLabel, FALSE, FALSE, 0);
                }

                gtk_box_pack_start(GTK_BOX(pHbox), pWidget, FALSE, FALSE, 0);

                pWidget = pHbox;

            }
            else
                SAL_INFO("vcl.gtk", "unhandled option type: " << aCtrlType);

            GtkWidget* pRow = NULL;
            if (pWidget)
            {
                if (bUseDependencyRow && !aDependsOnName.isEmpty())
                {
                    pRow = aPropertyToDependencyRowMap[aDependsOnName + rtl::OUString::valueOf(nDependsOnValue)];
                    if (!pRow)
                    {
                        gtk_widget_destroy(pWidget);
                        pWidget = NULL;
                    }
                }
            }
            if (pWidget)
            {
                if (!pRow)
                {
                    pRow = gtk_hbox_new(FALSE, 12);
                    gtk_box_pack_start(GTK_BOX(pCurParent), pRow, FALSE, FALSE, 0);
                }
                if (!pGroup)
                    aPropertyToDependencyRowMap[aPropertyName + rtl::OUString::valueOf(sal_Int32(0))] = pRow;
                gtk_box_pack_start(GTK_BOX(pRow), pWidget, FALSE, FALSE, 0);
            }
        }
    }

    if (pStandardPrintRangeContainer)
        gtk_widget_destroy(pStandardPrintRangeContainer);

    CustomTabs_t::const_reverse_iterator aEnd = aCustomTabs.rend();
    for (CustomTabs_t::const_reverse_iterator aI = aCustomTabs.rbegin(); aI != aEnd; ++aI)
    {
        gtk_widget_show_all(aI->first);
        m_pWrapper->print_unix_dialog_add_custom_tab(GTK_PRINT_UNIX_DIALOG(m_pDialog), aI->first,
            gtk_label_new(rtl::OUStringToOString(aI->second, RTL_TEXTENCODING_UTF8).getStr()));
    }
}


void
GtkPrintDialog::impl_initPrintContent(uno::Sequence<sal_Bool> const& i_rDisabled)
{
    SAL_WARN_IF(i_rDisabled.getLength() != 3, "vcl.gtk", "there is more choices than we expected");
    if (i_rDisabled.getLength() != 3)
        return;

    GtkPrintUnixDialog* const pDialog(GTK_PRINT_UNIX_DIALOG(m_pDialog));

    // XXX: This is a hack that depends on the number and the ordering of
    // the controls in the rDisabled sequence (cf. the intialization of
    // the "PrintContent" UI option in SwPrintUIOptions::SwPrintUIOptions,
    // sw/source/core/view/printdata.cxx)
    if (m_pWrapper->supportsPrintSelection() && !i_rDisabled[2])
    {
        m_pWrapper->print_unix_dialog_set_support_selection(pDialog, TRUE);
        m_pWrapper->print_unix_dialog_set_has_selection(pDialog, TRUE);
    }

    beans::PropertyValue* const pPrintContent(
            m_rController.getValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("PrintContent"))));

    if (pPrintContent)
    {
        sal_Int32 nSelectionType(0);
        pPrintContent->Value >>= nSelectionType;
        GtkPrintSettings* const pSettings(getSettings());
        GtkPrintPages ePrintPages(GTK_PRINT_PAGES_ALL);
        switch (nSelectionType)
        {
            case 0:
                ePrintPages = GTK_PRINT_PAGES_ALL;
                break;
            case 1:
                ePrintPages = GTK_PRINT_PAGES_RANGES;
                break;
            case 2:
#if GTK_CHECK_VERSION(2,14,0)
                if (m_pWrapper->supportsPrintSelection())
                    ePrintPages = GTK_PRINT_PAGES_SELECTION;
                else
#endif
                    SAL_INFO("vcl.gtk", "the application wants to print a selection, but the present gtk version does not support it");
                break;
            default:
                SAL_WARN("vcl.gtk", "unexpected selection type: " << nSelectionType);
        }
        m_pWrapper->print_settings_set_print_pages(pSettings, ePrintPages);
        m_pWrapper->print_unix_dialog_set_settings(pDialog, pSettings);
        g_object_unref(G_OBJECT(pSettings));
    }
}


void
GtkPrintDialog::impl_checkOptionalControlDependencies()
{
    for (std::map<GtkWidget*, rtl::OUString>::iterator it = m_aControlToPropertyMap.begin();
         it != m_aControlToPropertyMap.end(); ++it)
    {
        gtk_widget_set_sensitive(it->first, m_rController.isUIOptionEnabled(it->second));
    }
}


beans::PropertyValue*
GtkPrintDialog::impl_queryPropertyValue(GtkWidget* const i_pWidget) const
{
    beans::PropertyValue* pVal(0);
    std::map<GtkWidget*, rtl::OUString>::const_iterator aIt(m_aControlToPropertyMap.find(i_pWidget));
    if (aIt != m_aControlToPropertyMap.end())
    {
        pVal = m_rController.getValue(aIt->second);
        SAL_WARN_IF(!pVal, "vcl.gtk", "property value not found");
    }
    else
    {
        SAL_WARN("vcl.gtk", "changed control not in property map");
    }
    return pVal;
}


void
GtkPrintDialog::impl_UIOption_CheckHdl(GtkWidget* const i_pWidget)
{
    beans::PropertyValue* const pVal = impl_queryPropertyValue(i_pWidget);
    if (pVal)
    {
        const bool bVal = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(i_pWidget));
        pVal->Value <<= bVal;

        impl_checkOptionalControlDependencies();
    }
}


void
GtkPrintDialog::impl_UIOption_RadioHdl(GtkWidget* const i_pWidget)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(i_pWidget)))
    {
        beans::PropertyValue* const pVal = impl_queryPropertyValue(i_pWidget);
        std::map<GtkWidget*, sal_Int32>::const_iterator it = m_aControlToNumValMap.find(i_pWidget);
        if (pVal && it != m_aControlToNumValMap.end())
        {

            const sal_Int32 nVal = it->second;
            pVal->Value <<= nVal;

            impl_checkOptionalControlDependencies();
        }
    }
}


void
GtkPrintDialog::impl_UIOption_SelectHdl(GtkWidget* const i_pWidget)
{
    beans::PropertyValue* const pVal = impl_queryPropertyValue(i_pWidget);
    if (pVal)
    {
        const sal_Int32 nVal(gtk_combo_box_get_active(GTK_COMBO_BOX(i_pWidget)));
        pVal->Value <<= nVal;

        impl_checkOptionalControlDependencies();
    }
}


bool
GtkPrintDialog::run()
{
    bool bDoJob = false;
    bool bContinue = true;
    while (bContinue)
    {
        bContinue = false;
        const gint nStatus = gtk_dialog_run(GTK_DIALOG(m_pDialog));
        switch (nStatus)
        {
            case GTK_RESPONSE_HELP:
                fprintf(stderr, "To-Do: Help ?\n");
                bContinue = true;
                break;
            case GTK_RESPONSE_OK:
                bDoJob = true;
                break;
            default:
                break;
        }
    }
    gtk_widget_hide(m_pDialog);
    impl_storeToSettings();
    return bDoJob;
}

#if 0
void GtkPrintDialog::ExportAsPDF(const rtl::OUString &rFileURL, GtkPrintSettings *pSettings) const
{
    uno::Reference < XFramesSupplier > xDesktop =
            uno::Reference < XFramesSupplier >(
    ::comphelper::getProcessServiceFactory()->
        createInstance(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.frame.Desktop"))), UNO_QUERY);
    uno::Reference < XFrame > xFrame(xDesktop->getActiveFrame());
    if (!xFrame.is())
        xFrame = uno::Reference < XFrame >(xDesktop, UNO_QUERY);

    uno::Reference < XFilter > xFilter(
        ::comphelper::getProcessServiceFactory()->createInstance(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("com.sun.star.document.PDFFilter"))),
        UNO_QUERY);

    if (xFilter.is())
    {
        uno::Reference< XController > xController;
        uno::Reference< XComponent > xDoc;
        if (xFrame.is())
            xController = xFrame->getController();
        if (xController.is())
            xDoc = uno::Reference< XComponent >(xController->getModel(), UNO_QUERY);

        SvFileStream aStream(rFileURL, STREAM_READWRITE | STREAM_SHARE_DENYWRITE | STREAM_TRUNC);
        uno::Reference< XOutputStream > xOStm(new utl::OOutputStreamWrapper(aStream));

        uno::Reference< XExporter > xExport(xFilter, UNO_QUERY);
        xExport->setSourceDocument(xDoc);
        uno::Sequence<beans::PropertyValue> aFilterData(2);
        aFilterData[0].Name = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("PageLayout"));
        aFilterData[0].Value <<= sal_Int32(0);
        aFilterData[1].Name = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("FirstPageOnLeft"));
        aFilterData[1].Value <<= sal_False;


        const gchar *pStr = gtk_print_settings_get(pSettings, GTK_PRINT_SETTINGS_PRINT_PAGES);
        if (pStr && !strcmp(pStr, "ranges"))
        {
            String aRangeText;
            gint num_ranges;
            const GtkPageRange* pRanges = gtk_print_settings_get_page_ranges(pSettings, &num_ranges);
            for (gint i = 0; i < num_ranges; ++i)
            {
                aRangeText.Append(String::CreateFromInt32(pRanges[i].start+1));
                if (pRanges[i].start != pRanges[i].end)
                {
                    aRangeText.AppendAscii("-");
                    aRangeText.Append(String::CreateFromInt32(pRanges[i].end+1));
                }

                if (i != num_ranges-1)
                    aRangeText.AppendAscii(",");
            }
            aFilterData.realloc(aFilterData.getLength()+1);
            aFilterData[aFilterData.getLength()-1].Name = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("PageRange"));
            aFilterData[aFilterData.getLength()-1].Value <<= rtl::OUString(aRangeText);
        }
        else if (pStr && !strcmp(pStr, "current"))
        {
            try
            {
                   uno::Reference< XSpreadsheetView > xSpreadsheetView;
                   uno::Reference< XSpreadsheet> xSheet;
                   uno::Reference< XSpreadsheetDocument > xSheetDoc;
                   uno::Reference< XIndexAccess > xSheets;
                   uno::Reference< XNamed > xName;

                   if (xController.is())
                       xSpreadsheetView = uno::Reference< XSpreadsheetView >(xController, UNO_QUERY);
                   if (xSpreadsheetView.is())
                       xSheet = uno::Reference< XSpreadsheet>(xSpreadsheetView->getActiveSheet());
                   if (xSheet.is())
                       xName = uno::Reference < XNamed >(xSheet, UNO_QUERY);
                   if (xName.is())
                       xSheetDoc = uno::Reference< XSpreadsheetDocument >(xController->getModel(), UNO_QUERY);
                   if (xSheetDoc.is())
                       xSheets = uno::Reference< XIndexAccess >(xSheetDoc->getSheets(), UNO_QUERY);
                   if (xSheets.is())
                   {
                       const rtl::OUString &rName = xName->getName();

                       sal_Int32 i;

                       for (i = 0; i < xSheets->getCount(); ++i)
                       {
                           uno::Reference < XNamed > xItem =
                               uno::Reference < XNamed >(xSheets->getByIndex(i), UNO_QUERY);
                           if (rName == xItem->getName())
                               break;
                       }

                       if (i < xSheets->getCount())
                       {
                            aFilterData.realloc(aFilterData.getLength()+1);
                            aFilterData[aFilterData.getLength()-1].Name = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("PageRange"));
                            aFilterData[aFilterData.getLength()-1].Value <<= rtl::OUString(String::CreateFromInt32(i + 1));
                       }
                   }
            }
            catch (...) {}
        }
#if GTK_CHECK_VERSION(2,17,5)
        if (gtk_print_unix_dialog_get_has_selection(GTK_PRINT_UNIX_DIALOG(m_pDialog)))
        {
            uno::Any aSelection;
            try
            {
                if (xController.is())
                {
                    uno::Reference<view::XSelectionSupplier> xView(xController, UNO_QUERY);
                    if (xView.is())
                        xView->getSelection() >>= aSelection;
                }
            }
            catch (const uno::RuntimeException &)
            {
            }
            if (aSelection.hasValue())
            {
                aFilterData.realloc(aFilterData.getLength()+1);
                aFilterData[aFilterData.getLength()-1].Name = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Selection"));
                aFilterData[aFilterData.getLength()-1].Value <<= aSelection;
            }
        }
#endif
        uno::Sequence<beans::PropertyValue> aArgs(2);
        aArgs[0].Name = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("FilterData"));
        aArgs[0].Value <<= aFilterData;
        aArgs[1].Name = rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("OutputStream"));
        aArgs[1].Value <<= xOStm;
        xFilter->filter(aArgs);
    }
}
#endif


void
GtkPrintDialog::updateControllerPrintRange()
{
    GtkPrintSettings* const pSettings(getSettings());
    // TODO: use get_print_pages
    if (const gchar* const pStr = m_pWrapper->print_settings_get(pSettings, GTK_PRINT_SETTINGS_PRINT_PAGES))
    {
        beans::PropertyValue* pVal = m_rController.getValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("PrintRange")));
        if (!pVal)
            pVal = m_rController.getValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("PrintContent")));
        SAL_WARN_IF(!pVal, "vcl.gtk", "Nothing to map standard print options to!");
        if (pVal)
        {
            sal_Int32 nVal = 0;
            if (!strcmp(pStr, "all"))
                nVal = 0;
            else if (!strcmp(pStr, "ranges"))
                nVal = 1;
            else if (!strcmp(pStr, "selection"))
                nVal = 2;
            pVal->Value <<= nVal;

            if (nVal == 1)
            {
                pVal = m_rController.getValue(rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("PageRange")));
                SAL_WARN_IF(!pVal, "vcl.gtk", "PageRange doesn't exist!");
                if (pVal)
                {
                    rtl::OUStringBuffer sBuf;
                    gint num_ranges;
                    const GtkPageRange* const pRanges = m_pWrapper->print_settings_get_page_ranges(pSettings, &num_ranges);
                    for (gint i = 0; i != num_ranges && pRanges; ++i)
                    {
                        sBuf.append(sal_Int32(pRanges[i].start+1));
                        if (pRanges[i].start != pRanges[i].end)
                        {
                            sBuf.append(sal_Unicode('-'));
                            sBuf.append(sal_Int32(pRanges[i].end+1));
                        }

                        if (i != num_ranges-1)
                            sBuf.append(sal_Unicode(','));
                    }
                    pVal->Value <<= sBuf.makeStringAndClear();
                }
            }
        }
    }
    g_object_unref(G_OBJECT(pSettings));
}


GtkPrintDialog::~GtkPrintDialog()
{
    gtk_widget_destroy(m_pDialog);
}


void
GtkPrintDialog::impl_readFromSettings()
{
    vcl::SettingsConfigItem* const pItem(vcl::SettingsConfigItem::get());
    GtkPrintSettings* const pSettings(getSettings());

    const rtl::OUString aPrintDialogStr(RTL_CONSTASCII_USTRINGPARAM("PrintDialog"));
    const rtl::OUString aCopyCount(pItem->getValue(aPrintDialogStr,
                rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("CopyCount"))));
    const rtl::OUString aCollate(pItem->getValue(aPrintDialogStr,
                rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Collate"))));

    bool bChanged(false);

    const gint nOldCopyCount(m_pWrapper->print_settings_get_n_copies(pSettings));
    const sal_Int32 nCopyCount(aCopyCount.toInt32());
    if (nCopyCount > 0 && nOldCopyCount != nCopyCount)
    {
        bChanged = true;
        m_pWrapper->print_settings_set_n_copies(pSettings, sal::static_int_cast<gint>(nCopyCount));
    }

    const gboolean bOldCollate(m_pWrapper->print_settings_get_collate(pSettings));
    const bool bCollate(aCollate.equalsIgnoreAsciiCaseAsciiL(RTL_CONSTASCII_STRINGPARAM("true")));
    if (bOldCollate != bCollate)
    {
        bChanged = true;
        m_pWrapper->print_settings_set_collate(pSettings, bCollate);
    }
    // TODO: wth was this var. meant for?
    (void) bChanged;

    m_pWrapper->print_unix_dialog_set_settings(GTK_PRINT_UNIX_DIALOG(m_pDialog), pSettings);
    g_object_unref(G_OBJECT(pSettings));
}


void
GtkPrintDialog::impl_storeToSettings()
const
{
    vcl::SettingsConfigItem* const pItem(vcl::SettingsConfigItem::get());
    GtkPrintSettings* const pSettings(getSettings());

    const rtl::OUString aPrintDialogStr(RTL_CONSTASCII_USTRINGPARAM("PrintDialog"));
    pItem->setValue(aPrintDialogStr,
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("CopyCount")),
            rtl::OUString::valueOf(sal_Int32(m_pWrapper->print_settings_get_n_copies(pSettings))));
    pItem->setValue(aPrintDialogStr,
            rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("Collate")),
            m_pWrapper->print_settings_get_collate(pSettings)
                ? rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("true"))
                : rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("false")))
        ;
    // pItem->setValue(aPrintDialog, rtl::OUString::createFromAscii("ToFile"), );
    g_object_unref(G_OBJECT(pSettings));
    pItem->Commit();
}


sal_uLong
GtkSalInfoPrinter::GetCapabilities(
        const ImplJobSetup* const i_pSetupData,
        const sal_uInt16 i_nType)
{
    if (i_nType == PRINTER_CAPABILITIES_EXTERNALDIALOG && lcl_useSystemPrintDialog())
        return 1;
    return PspSalInfoPrinter::GetCapabilities(i_pSetupData, i_nType);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
