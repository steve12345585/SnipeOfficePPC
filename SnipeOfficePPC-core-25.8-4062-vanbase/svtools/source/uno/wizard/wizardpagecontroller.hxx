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

#ifndef SVT_UNOWIZ_WIZARDPAGECONTROLLER_HXX
#define SVT_UNOWIZ_WIZARDPAGECONTROLLER_HXX

#include "svtools/wizardmachine.hxx"

#include <com/sun/star/ui/dialogs/XWizardController.hpp>

//......................................................................................................................
namespace svt { namespace uno
{
//......................................................................................................................

    class WizardShell;

    //==================================================================================================================
    //= WizardPageController
    //==================================================================================================================
    class WizardPageController : public IWizardPageController
    {
    public:
        WizardPageController(
            WizardShell& i_rParent,
            const ::com::sun::star::uno::Reference< ::com::sun::star::ui::dialogs::XWizardController >& i_rController,
            const sal_Int16 i_nPageId
        );
        virtual ~WizardPageController();

        // IWizardPageController overridables
        virtual void        initializePage();
        virtual sal_Bool    commitPage( WizardTypes::CommitPageReason _eReason );
        virtual bool        canAdvance() const;

        const ::com::sun::star::uno::Reference< ::com::sun::star::ui::dialogs::XWizardPage >&
                            getWizardPage() const { return m_xWizardPage; }
        TabPage*            getTabPage() const;

    private:
        const ::com::sun::star::uno::Reference< ::com::sun::star::ui::dialogs::XWizardController >  m_xController;
        ::com::sun::star::uno::Reference< ::com::sun::star::ui::dialogs::XWizardPage >              m_xWizardPage;
        const sal_Int16                                                                             m_nPageId;
    };

//......................................................................................................................
} } // namespace svt::uno
//......................................................................................................................

#endif // SVT_UNOWIZ_WIZARDPAGECONTROLLER_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
