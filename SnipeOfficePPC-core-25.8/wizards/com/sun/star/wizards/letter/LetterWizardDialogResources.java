/*
 ************************************************************************
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
package com.sun.star.wizards.letter;

import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.wizards.common.Resource;

public class LetterWizardDialogResources extends Resource
{

    final static String UNIT_NAME = "dbwizres";
    final static String MODULE_NAME = "dbw";
    final static int RID_LETTERWIZARDDIALOG_START = 3000;
    final static int RID_LETTERWIZARDGREETING_START = 3080;
    final static int RID_LETTERWIZARDSALUTATION_START = 3090;
    final static int RID_LETTERWIZARDROADMAP_START = 3100;
    final static int RID_LETTERWIZARDLANGUAGE_START = 3110;
    final static int RID_RID_COMMON_START = 500;
    protected String[] RoadmapLabels = new String[7];
    protected String[] SalutationLabels = new String[3];
    protected String[] GreetingLabels = new String[3];
    protected String[] LanguageLabels = new String[16];
    String resOverwriteWarning;
    String resTemplateDescription;
    String resLetterWizardDialog_title;
    String resLabel9_value;
    String resoptBusinessLetter_value;
    String resoptPrivOfficialLetter_value;
    String resoptPrivateLetter_value;
    String reschkBusinessPaper_value;
    String reschkPaperCompanyLogo_value;
    String reschkPaperCompanyAddress_value;
    String reschkPaperFooter_value;
    String reschkCompanyReceiver_value;
    String reschkUseLogo_value;
    String reschkUseAddressReceiver_value;
    String reschkUseSigns_value;
    String reschkUseSubject_value;
    String reschkUseSalutation_value;
    String reschkUseBendMarks_value;
    String reschkUseGreeting_value;
    String reschkUseFooter_value;
    String resoptSenderPlaceholder_value;
    String resoptSenderDefine_value;
    String resoptReceiverPlaceholder_value;
    String resoptReceiverDatabase_value;
    String reschkFooterNextPages_value;
    String reschkFooterPageNumbers_value;
    String restxtTemplateName_value;
    String resoptCreateLetter_value;
    String resoptMakeChanges_value;
    String reslblBusinessStyle_value;
    String reslblPrivOfficialStyle_value;
    String reslblPrivateStyle_value;
    String reslblIntroduction_value;
    String reslblLogoHeight_value;
    String reslblLogoWidth_value;
    String reslblLogoX_value;
    String reslblLogoY_value;
    String reslblAddressHeight_value;
    String reslblAddressWidth_value;
    String reslblAddressX_value;
    String reslblAddressY_value;
    String reslblFooterHeight_value;
    String reslblLetterNorm_value;
    String reslblSenderAddress_value;
    String reslblSenderName_value;
    String reslblSenderStreet_value;
    String reslblPostCodeCity_value;
    String reslblReceiverAddress_value;
    String reslblFooter_value;
    String reslblFinalExplanation1_value;
    String reslblFinalExplanation2_value;
    String reslblTemplateName_value;
    String reslblTemplatePath_value;
    String reslblProceed_value;
    String reslblTitle1_value;
    String reslblTitle3_value;
    String reslblTitle2_value;
    String reslblTitle4_value;
    String reslblTitle5_value;
    String reslblTitle6_value;

    public LetterWizardDialogResources(XMultiServiceFactory xmsf)
    {
        super(xmsf, UNIT_NAME, MODULE_NAME);

        resLetterWizardDialog_title = getResText(RID_LETTERWIZARDDIALOG_START + 1);
        resLabel9_value = getResText(RID_LETTERWIZARDDIALOG_START + 2);
        resoptBusinessLetter_value = getResText(RID_LETTERWIZARDDIALOG_START + 3);
        resoptPrivOfficialLetter_value = getResText(RID_LETTERWIZARDDIALOG_START + 4);
        resoptPrivateLetter_value = getResText(RID_LETTERWIZARDDIALOG_START + 5);
        reschkBusinessPaper_value = getResText(RID_LETTERWIZARDDIALOG_START + 6);
        reschkPaperCompanyLogo_value = getResText(RID_LETTERWIZARDDIALOG_START + 7);
        reschkPaperCompanyAddress_value = getResText(RID_LETTERWIZARDDIALOG_START + 8);
        reschkPaperFooter_value = getResText(RID_LETTERWIZARDDIALOG_START + 9);
        reschkCompanyReceiver_value = getResText(RID_LETTERWIZARDDIALOG_START + 10);
        reschkUseLogo_value = getResText(RID_LETTERWIZARDDIALOG_START + 11);
        reschkUseAddressReceiver_value = getResText(RID_LETTERWIZARDDIALOG_START + 12);
        reschkUseSigns_value = getResText(RID_LETTERWIZARDDIALOG_START + 13);
        reschkUseSubject_value = getResText(RID_LETTERWIZARDDIALOG_START + 14);
        reschkUseSalutation_value = getResText(RID_LETTERWIZARDDIALOG_START + 15);
        reschkUseBendMarks_value = getResText(RID_LETTERWIZARDDIALOG_START + 16);
        reschkUseGreeting_value = getResText(RID_LETTERWIZARDDIALOG_START + 17);
        reschkUseFooter_value = getResText(RID_LETTERWIZARDDIALOG_START + 18);
        resoptSenderPlaceholder_value = getResText(RID_LETTERWIZARDDIALOG_START + 19);
        resoptSenderDefine_value = getResText(RID_LETTERWIZARDDIALOG_START + 20);
        resoptReceiverPlaceholder_value = getResText(RID_LETTERWIZARDDIALOG_START + 21);
        resoptReceiverDatabase_value = getResText(RID_LETTERWIZARDDIALOG_START + 22);
        reschkFooterNextPages_value = getResText(RID_LETTERWIZARDDIALOG_START + 23);
        reschkFooterPageNumbers_value = getResText(RID_LETTERWIZARDDIALOG_START + 24);
        restxtTemplateName_value = getResText(RID_LETTERWIZARDDIALOG_START + 25);
        resoptCreateLetter_value = getResText(RID_LETTERWIZARDDIALOG_START + 26);
        resoptMakeChanges_value = getResText(RID_LETTERWIZARDDIALOG_START + 27);
        reslblBusinessStyle_value = getResText(RID_LETTERWIZARDDIALOG_START + 28);
        reslblPrivOfficialStyle_value = getResText(RID_LETTERWIZARDDIALOG_START + 29);
        reslblPrivateStyle_value = getResText(RID_LETTERWIZARDDIALOG_START + 30);
        reslblIntroduction_value = getResText(RID_LETTERWIZARDDIALOG_START + 31);
        reslblLogoHeight_value = getResText(RID_LETTERWIZARDDIALOG_START + 32);
        reslblLogoWidth_value = getResText(RID_LETTERWIZARDDIALOG_START + 33);
        reslblLogoX_value = getResText(RID_LETTERWIZARDDIALOG_START + 34);
        reslblLogoY_value = getResText(RID_LETTERWIZARDDIALOG_START + 35);
        reslblAddressHeight_value = getResText(RID_LETTERWIZARDDIALOG_START + 36);
        reslblAddressWidth_value = getResText(RID_LETTERWIZARDDIALOG_START + 37);
        reslblAddressX_value = getResText(RID_LETTERWIZARDDIALOG_START + 38);
        reslblAddressY_value = getResText(RID_LETTERWIZARDDIALOG_START + 39);
        reslblFooterHeight_value = getResText(RID_LETTERWIZARDDIALOG_START + 40);
        reslblLetterNorm_value = getResText(RID_LETTERWIZARDDIALOG_START + 41);
        reslblSenderAddress_value = getResText(RID_LETTERWIZARDDIALOG_START + 42);
        reslblSenderName_value = getResText(RID_LETTERWIZARDDIALOG_START + 43);
        reslblSenderStreet_value = getResText(RID_LETTERWIZARDDIALOG_START + 44);
        reslblPostCodeCity_value = getResText(RID_LETTERWIZARDDIALOG_START + 45);
        reslblReceiverAddress_value = getResText(RID_LETTERWIZARDDIALOG_START + 46);
        reslblFooter_value = getResText(RID_LETTERWIZARDDIALOG_START + 47);
        reslblFinalExplanation1_value = getResText(RID_LETTERWIZARDDIALOG_START + 48);
        reslblFinalExplanation2_value = getResText(RID_LETTERWIZARDDIALOG_START + 49);
        reslblTemplateName_value = getResText(RID_LETTERWIZARDDIALOG_START + 50);
        reslblTemplatePath_value = getResText(RID_LETTERWIZARDDIALOG_START + 51);
        reslblProceed_value = getResText(RID_LETTERWIZARDDIALOG_START + 52);
        reslblTitle1_value = getResText(RID_LETTERWIZARDDIALOG_START + 53);
        reslblTitle3_value = getResText(RID_LETTERWIZARDDIALOG_START + 54);
        reslblTitle2_value = getResText(RID_LETTERWIZARDDIALOG_START + 55);
        reslblTitle4_value = getResText(RID_LETTERWIZARDDIALOG_START + 56);
        reslblTitle5_value = getResText(RID_LETTERWIZARDDIALOG_START + 57);
        reslblTitle6_value = getResText(RID_LETTERWIZARDDIALOG_START + 58);

        loadRoadmapResources();
        loadSalutationResources();
        loadGreetingResources();
        loadCommonResources();
    }

    private void loadCommonResources()
    {
        resOverwriteWarning = getResText(RID_RID_COMMON_START + 19);
        resTemplateDescription = getResText(RID_RID_COMMON_START + 20);
    }

    private void loadRoadmapResources()
    {
        for (int i = 1; i < 7; i++)
        {
            RoadmapLabels[i] = getResText(RID_LETTERWIZARDROADMAP_START + i);
        }
    }

    private void loadSalutationResources()
    {
        for (int i = 1; i < 4; i++)
        {
            SalutationLabels[i - 1] = getResText(RID_LETTERWIZARDSALUTATION_START + i);
        }
    }

    private void loadGreetingResources()
    {
        for (int i = 1; i < 4; i++)
        {
            GreetingLabels[i - 1] = getResText(RID_LETTERWIZARDGREETING_START + i);
        }
    }
}
