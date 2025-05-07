from LetterWizardDialog import *
from LetterDocument import *
from wizards.common.NoValidPathException import NoValidPathException
from wizards.common.FileAccess import FileAccess
from LocaleCodes import LocaleCodes
from wizards.ui.PathSelection import PathSelection
from wizards.common.Configuration import Configuration
from CGLetterWizard import CGLetterWizard
from wizards.ui.event.UnoDataAware import UnoDataAware
from wizards.ui.event.RadioDataAware import RadioDataAware
from wizards.text.TextFieldHandler import TextFieldHandler
from wizards.common.SystemDialog import SystemDialog

from com.sun.star.awt.VclWindowPeerAttribute import YES_NO, DEF_NO
from com.sun.star.view.DocumentZoomType import OPTIMAL
from com.sun.star.document.UpdateDocMode import FULL_UPDATE
from com.sun.star.document.MacroExecMode import ALWAYS_EXECUTE

class LetterWizardDialogImpl(LetterWizardDialog):

    RM_TYPESTYLE = 1
    RM_BUSINESSPAPER = 2
    RM_ELEMENTS = 3
    RM_SENDERRECEIVER = 4
    RM_FOOTER = 5
    RM_FINALSETTINGS = 6

    lstBusinessStylePos = None
    lstPrivateStylePos = None
    lstPrivOfficialStylePos = None

    def enterStep(self, OldStep, NewStep):
        pass

    def leaveStep(self, OldStep, NewStep):
        pass

    def __init__(self, xmsf):
        super(LetterWizardDialogImpl, self).__init__(xmsf)
        self.xmsf = xmsf
        self.bSaveSuccess = False
        self.filenameChanged = False
        self.BusCompanyLogo = None
        self.BusCompanyAddress = None
        self.BusCompanyAddressReceiver = None
        self.BusFooter = None
        self.Norms = []
        self.NormPaths = []

    @classmethod
    def main(self, args):
        ConnectStr = \
            "uno:socket,host=localhost,port=2002;urp;StarOffice.ComponentContext"
        xLocMSF = None
        try:
            xLocMSF = Desktop.connect(ConnectStr)
        except Exception, e:
            traceback.print_exc()

        lw = LetterWizardDialogImpl(xLocMSF)
        lw.startWizard(xLocMSF, None)

    def startWizard(self, xMSF, CurPropertyValue):
        self.running = True
        try:
            #Number of steps on WizardDialog
            self.nMaxStep = 6

            #instatiate The Document Frame for the Preview
            self.myLetterDoc = LetterDocument(xMSF, self)

            #create the dialog
            self.drawNaviBar()
            self.buildStep1()
            self.buildStep2()
            self.buildStep3()
            self.buildStep4()
            self.buildStep5()
            self.buildStep6()
            self.__initializePaths()
            self.initializeNorms()
            self.initializeSalutation()
            self.initializeGreeting()

            #special Control fFrameor setting the save Path:
            self.insertPathSelectionControl()

            self.myConfig = CGLetterWizard()

            oL = self.getOfficeLinguistic()
            self.myConfig.cp_BusinessLetter.cp_Norm = oL
            self.myConfig.cp_PrivateOfficialLetter.cp_Norm = oL
            self.myConfig.cp_PrivateLetter.cp_Norm = oL
            self.initializeTemplates(xMSF)

            #load the last used settings
            #from the registry and apply listeners to the controls:
            self.initConfiguration()

            if self.myConfig.cp_BusinessLetter.cp_Greeting == "":
                self.myConfig.cp_BusinessLetter.cp_Greeting = \
                    self.resources.GreetingLabels[0]

            if self.myConfig.cp_BusinessLetter.cp_Salutation == "":
                self.myConfig.cp_BusinessLetter.cp_Salutation = \
                    self.resources.SalutationLabels[0]

            if self.myConfig.cp_PrivateOfficialLetter.cp_Greeting == "":
                self.myConfig.cp_PrivateOfficialLetter.cp_Greeting = \
                    self.resources.GreetingLabels[1]

            if self.myConfig.cp_PrivateOfficialLetter.cp_Salutation == "":
                self.myConfig.cp_PrivateOfficialLetter.cp_Salutation = \
                    self.resources.SalutationLabels[1]

            if self.myConfig.cp_PrivateLetter.cp_Greeting == "":
                self.myConfig.cp_PrivateLetter.cp_Greeting = \
                    self.resources.GreetingLabels[2]

            if self.myConfig.cp_PrivateLetter.cp_Salutation == "":
                self.myConfig.cp_PrivateLetter.cp_Salutation = \
                    self.resources.SalutationLabels[2]

            if self.myPathSelection.xSaveTextBox.Text.lower() == "":
                self.myPathSelection.initializePath()

            xContainerWindow = self.myLetterDoc.xFrame.ContainerWindow
            self.createWindowPeer(xContainerWindow)
            self.insertRoadmap()
            self.setConfiguration()
            self.setDefaultForGreetingAndSalutation()
            self.initializeElements()
            self.myLetterDoc.xFrame.ComponentWindow.Enable = False
            self.executeDialogFromComponent(self.myLetterDoc.xFrame)
            self.removeTerminateListener()
            self.closeDocument()
            self.running = False
        except Exception, exception:
            self.removeTerminateListener()
            traceback.print_exc()
            self.running = False
            return

    def cancelWizard(self):
        self.xUnoDialog.endExecute()
        self.running = False

    def finishWizard(self):
        self.switchToStep(self.getCurrentStep(), self.nMaxStep)
        endWizard = True
        try:
            fileAccess = FileAccess(self.xMSF)
            self.sPath = self.myPathSelection.getSelectedPath()
            if self.sPath == "":
                self.myPathSelection.triggerPathPicker()
                self.sPath = self.myPathSelection.getSelectedPath()

            self.sPath = fileAccess.getURL(self.sPath)
            if not self.filenameChanged:
                if fileAccess.exists(self.sPath, True):
                    answer = SystemDialog.showMessageBox(
                        self.xMSF, "MessBox", YES_NO + DEF_NO,
                        self.resources.resOverwriteWarning,
                        self.xUnoDialog.Peer)
                    if answer == 3:
                        # user said: no, do not overwrite...
                        endWizard = False
                        return False

            self.myLetterDoc.setWizardTemplateDocInfo(
                self.resources.resLetterWizardDialog_title,
                self.resources.resTemplateDescription)
            self.myLetterDoc.killEmptyUserFields()
            self.myLetterDoc.keepLogoFrame = self.chkUseLogo.State != 0
            if self.chkBusinessPaper.State != 0 \
                    and self.chkPaperCompanyLogo.State != 0:
                self.myLetterDoc.keepLogoFrame = False

            self.myLetterDoc.keepBendMarksFrame = \
                self.chkUseBendMarks.State != 0
            self.myLetterDoc.keepLetterSignsFrame = \
                self.chkUseSigns.State != 0
            self.myLetterDoc.keepSenderAddressRepeatedFrame = \
                self.chkUseAddressReceiver.State != 0
            if self.optBusinessLetter.State:
                if self.chkBusinessPaper.State != 0 \
                        and self.chkCompanyReceiver.State != 0:
                    self.myLetterDoc.keepSenderAddressRepeatedFrame = False

                if self.chkBusinessPaper.State != 0 \
                        and self.chkPaperCompanyAddress.State != 0:
                    self.myLetterDoc.keepAddressFrame = False

            self.myLetterDoc.killEmptyFrames()
            self.bSaveSuccess = \
                OfficeDocument.store(
                    self.xMSF, TextDocument.xTextDocument,
                    self.sPath, "writer8_template")
            if self.bSaveSuccess:
                self.saveConfiguration()
                xIH = self.xMSF.createInstance(
                    "com.sun.star.comp.uui.UUIInteractionHandler")
                loadValues = range(4)
                loadValues[0] = uno.createUnoStruct( \
                    'com.sun.star.beans.PropertyValue')
                loadValues[0].Name = "AsTemplate"
                loadValues[1] = uno.createUnoStruct( \
                    'com.sun.star.beans.PropertyValue')
                loadValues[1].Name = "MacroExecutionMode"
                loadValues[1].Value = ALWAYS_EXECUTE
                loadValues[2] = uno.createUnoStruct( \
                    'com.sun.star.beans.PropertyValue')
                loadValues[2].Name = "UpdateDocMode"
                loadValues[2].Value = FULL_UPDATE
                loadValues[3] = uno.createUnoStruct( \
                    'com.sun.star.beans.PropertyValue')
                loadValues[3].Name = "InteractionHandler"
                loadValues[3].Value = xIH
                if self.bEditTemplate:
                    loadValues[0].Value = False
                else:
                    loadValues[0].Value = True

                oDoc = OfficeDocument.load(
                    Desktop.getDesktop(self.xMSF),
                    self.sPath, "_default", loadValues)
                myViewHandler = ViewHandler(self.xMSF, oDoc)
                myViewHandler.setViewSetting("ZoomType", OPTIMAL)
            else:
                pass

        except Exception, e:
            traceback.print_exc()
        finally:
            if endWizard:
                self.xUnoDialog.endExecute()
                self.running = False

        return True;

    def closeDocument(self):
        try:
            xCloseable = self.myLetterDoc.xFrame
            xCloseable.close(False)
        except CloseVetoException, e:
            traceback.print_exc()

    def optBusinessLetterItemChanged(self):
        LetterWizardDialogImpl.lstPrivateStylePos = None
        LetterWizardDialogImpl.lstPrivOfficialStylePos = None
        self.setControlProperty(
            "lblBusinessStyle", PropertyNames.PROPERTY_ENABLED, True)
        self.setControlProperty(
            "lstBusinessStyle", PropertyNames.PROPERTY_ENABLED, True)
        self.setControlProperty(
            "chkBusinessPaper", PropertyNames.PROPERTY_ENABLED, True)
        self.setControlProperty(
            "lblPrivOfficialStyle", PropertyNames.PROPERTY_ENABLED, False)
        self.setControlProperty(
            "lstPrivOfficialStyle", PropertyNames.PROPERTY_ENABLED, False)
        self.setControlProperty(
            "lblPrivateStyle", PropertyNames.PROPERTY_ENABLED, False)
        self.setControlProperty(
            "lstPrivateStyle", PropertyNames.PROPERTY_ENABLED, False)
        self.lstBusinessStyleItemChanged()
        self.enableSenderReceiver()
        self.setPossibleFooter(True)
        if self.myPathSelection.xSaveTextBox.Text.lower() == "":
            self.myPathSelection.initializePath()

    def optPrivOfficialLetterItemChanged(self):
        LetterWizardDialogImpl.lstBusinessStylePos = None
        LetterWizardDialogImpl.lstPrivateStylePos = None
        self.setControlProperty(
            "lblBusinessStyle", PropertyNames.PROPERTY_ENABLED, False)
        self.setControlProperty(
            "lstBusinessStyle", PropertyNames.PROPERTY_ENABLED, False)
        self.setControlProperty(
            "chkBusinessPaper", PropertyNames.PROPERTY_ENABLED, False)
        self.setControlProperty(
            "lblPrivOfficialStyle", PropertyNames.PROPERTY_ENABLED, True)
        self.setControlProperty(
            "lstPrivOfficialStyle", PropertyNames.PROPERTY_ENABLED, True)
        self.setControlProperty(
            "lblPrivateStyle", PropertyNames.PROPERTY_ENABLED, False)
        self.setControlProperty(
            "lstPrivateStyle", PropertyNames.PROPERTY_ENABLED, False)
        self.lstPrivOfficialStyleItemChanged()
        self.disableBusinessPaper()
        self.enableSenderReceiver()
        self.setPossibleFooter(True)
        if self.myPathSelection.xSaveTextBox.Text.lower() == "":
            self.myPathSelection.initializePath()

    def optPrivateLetterItemChanged(self):
        LetterWizardDialogImpl.lstBusinessStylePos = None
        LetterWizardDialogImpl.lstPrivOfficialStylePos = None
        self.setControlProperty(
            "lblBusinessStyle", PropertyNames.PROPERTY_ENABLED, False)
        self.setControlProperty(
            "lstBusinessStyle", PropertyNames.PROPERTY_ENABLED, False)
        self.setControlProperty(
            "chkBusinessPaper", PropertyNames.PROPERTY_ENABLED, False)
        self.setControlProperty(
            "lblPrivOfficialStyle", PropertyNames.PROPERTY_ENABLED, False)
        self.setControlProperty(
            "lstPrivOfficialStyle", PropertyNames.PROPERTY_ENABLED, False)
        self.setControlProperty(
            "lblPrivateStyle", PropertyNames.PROPERTY_ENABLED, True)
        self.setControlProperty(
            "lstPrivateStyle", PropertyNames.PROPERTY_ENABLED, True)
        self.lstPrivateStyleItemChanged()
        self.disableBusinessPaper()
        self.disableSenderReceiver()
        self.setPossibleFooter(False)
        if self.myPathSelection.xSaveTextBox.Text.lower() == "":
            self.myPathSelection.initializePath()

    def optSenderPlaceholderItemChanged(self):
        self.setControlProperty(
            "lblSenderName", PropertyNames.PROPERTY_ENABLED, False)
        self.setControlProperty(
            "lblSenderStreet", PropertyNames.PROPERTY_ENABLED, False)
        self.setControlProperty(
            "lblPostCodeCity", PropertyNames.PROPERTY_ENABLED, False)
        self.setControlProperty(
            "txtSenderName", PropertyNames.PROPERTY_ENABLED, False)
        self.setControlProperty(
            "txtSenderStreet", PropertyNames.PROPERTY_ENABLED, False)
        self.setControlProperty(
            "txtSenderPostCode", PropertyNames.PROPERTY_ENABLED, False)
        self.setControlProperty(
            "txtSenderState", PropertyNames.PROPERTY_ENABLED, False)
        self.setControlProperty(
            "txtSenderCity", PropertyNames.PROPERTY_ENABLED, False)
        self.myLetterDoc.fillSenderWithUserData()

    def optSenderDefineItemChanged(self):
        self.setControlProperty(
            "lblSenderName", PropertyNames.PROPERTY_ENABLED, True)
        self.setControlProperty(
            "lblSenderStreet", PropertyNames.PROPERTY_ENABLED, True)
        self.setControlProperty(
            "lblPostCodeCity", PropertyNames.PROPERTY_ENABLED, True)
        self.setControlProperty(
            "txtSenderName", PropertyNames.PROPERTY_ENABLED, True)
        self.setControlProperty(
            "txtSenderStreet", PropertyNames.PROPERTY_ENABLED, True)
        self.setControlProperty(
            "txtSenderPostCode", PropertyNames.PROPERTY_ENABLED, True)
        self.setControlProperty(
            "txtSenderState", PropertyNames.PROPERTY_ENABLED, True)
        self.setControlProperty(
            "txtSenderCity", PropertyNames.PROPERTY_ENABLED, True)
        self.txtSenderNameTextChanged()
        self.txtSenderStreetTextChanged()
        self.txtSenderPostCodeTextChanged()
        self.txtSenderStateTextChanged()
        self.txtSenderCityTextChanged()

    def lstBusinessStyleItemChanged(self):
        selectedItemPos = self.lstBusinessStyle.SelectedItemPos
        if LetterWizardDialogImpl.lstBusinessStylePos is not selectedItemPos:
            LetterWizardDialogImpl.lstBusinessStylePos = selectedItemPos
            TextDocument.xTextDocument = \
                self.myLetterDoc.loadAsPreview(
                    self.BusinessFiles[1][selectedItemPos],
                    False)
            self.myLetterDoc.xTextDocument.lockControllers()
            self.initializeElements()
            self.chkBusinessPaperItemChanged()
            self.setElements(False)
            self.myLetterDoc.xTextDocument.unlockControllers()

    def lstPrivOfficialStyleItemChanged(self):
        selectedItemPos = self.lstPrivOfficialStyle.SelectedItemPos
        if LetterWizardDialogImpl.lstPrivOfficialStylePos is not selectedItemPos:
            LetterWizardDialogImpl.lstPrivOfficialStylePos = selectedItemPos
            TextDocument.xTextDocument = \
                self.myLetterDoc.loadAsPreview(
                    self.OfficialFiles[1][selectedItemPos],
                    False)
            self.myLetterDoc.xTextDocument.lockControllers()
            self.initializeElements()
            self.setPossibleSenderData(True)
            self.setElements(False)
            self.myLetterDoc.xTextDocument.unlockControllers()

    def lstPrivateStyleItemChanged(self):
        selectedItemPos = self.lstPrivateStyle.SelectedItemPos
        if LetterWizardDialogImpl.lstPrivateStylePos is not selectedItemPos:
            LetterWizardDialogImpl.lstPrivateStylePos = selectedItemPos
            TextDocument.xTextDocument = \
                self.myLetterDoc.loadAsPreview(
                    self.PrivateFiles[1][selectedItemPos],
                    False)
            self.myLetterDoc.xTextDocument.lockControllers()
            self.initializeElements()
            self.setElements(True)
            self.myLetterDoc.xTextDocument.unlockControllers()

    def numLogoHeightTextChanged(self):
        self.BusCompanyLogo.iHeight = int(self.numLogoHeight.Value * 1000)
        self.BusCompanyLogo.setFramePosition()

    def numLogoWidthTextChanged(self):
        self.BusCompanyLogo.iWidth = int(self.numLogoWidth.Value * 1000)
        self.BusCompanyLogo.setFramePosition()

    def numLogoXTextChanged(self):
        self.BusCompanyLogo.iXPos = int(self.numLogoX.Value * 1000)
        self.BusCompanyLogo.setFramePosition()

    def numLogoYTextChanged(self):
        self.BusCompanyLogo.iYPos = int(self.numLogoY.Value * 1000)
        self.BusCompanyLogo.setFramePosition()

    def numAddressWidthTextChanged(self):
        self.BusCompanyAddress.iWidth = int(self.numAddressWidth.Value * 1000)
        self.BusCompanyAddress.setFramePosition()

    def numAddressXTextChanged(self):
        self.BusCompanyAddress.iXPos = int(self.numAddressX.Value * 1000)
        self.BusCompanyAddress.setFramePosition()

    def numAddressYTextChanged(self):
        self.BusCompanyAddress.iYPos = int(self.numAddressY.Value * 1000)
        self.BusCompanyAddress.setFramePosition()

    def numAddressHeightTextChanged(self):
        self.BusCompanyAddress.iHeight = int(self.numAddressHeight.Value * 1000)
        self.BusCompanyAddress.setFramePosition()

    def numFooterHeightTextChanged(self):
        self.BusFooter.iHeight = int(self.numFooterHeight.Value * 1000)
        self.BusFooter.iYPos = \
            self.myLetterDoc.DocSize.Height - self.BusFooter.iHeight
        self.BusFooter.setFramePosition()

    def chkPaperCompanyLogoItemChanged(self):
        if self.chkPaperCompanyLogo.State != 0:
            if self.numLogoWidth.Value == 0:
                self.numLogoWidth.Value = 0.1

            if self.numLogoHeight.Value == 0:
                self.numLogoHeight.Value = 0.1
            self.BusCompanyLogo = BusinessPaperObject(
                "Company Logo", int(self.numLogoWidth.Value * 1000),
                int(self.numLogoHeight.Value * 1000),
                int(self.numLogoX.Value * 1000),
                self.numLogoY.Value * 1000)
            self.setControlProperty(
            "numLogoHeight", PropertyNames.PROPERTY_ENABLED, True)
            self.setControlProperty(
            "lblCompanyLogoHeight", PropertyNames.PROPERTY_ENABLED, True)
            self.setControlProperty(
            "numLogoWidth", PropertyNames.PROPERTY_ENABLED, True)
            self.setControlProperty(
            "lblCompanyLogoWidth", PropertyNames.PROPERTY_ENABLED, True)
            self.setControlProperty(
            "numLogoX", PropertyNames.PROPERTY_ENABLED, True)
            self.setControlProperty(
            "lblCompanyLogoX", PropertyNames.PROPERTY_ENABLED, True)
            self.setControlProperty(
            "numLogoY", PropertyNames.PROPERTY_ENABLED, True)
            self.setControlProperty(
            "lblCompanyLogoY", PropertyNames.PROPERTY_ENABLED, True)
            self.setPossibleLogo(False)
        else:
            if self.BusCompanyLogo != None:
                self.BusCompanyLogo.removeFrame()

            self.setControlProperty(
            "numLogoHeight", PropertyNames.PROPERTY_ENABLED, False)
            self.setControlProperty(
            "lblCompanyLogoHeight", PropertyNames.PROPERTY_ENABLED, False)
            self.setControlProperty(
            "numLogoWidth", PropertyNames.PROPERTY_ENABLED, False)
            self.setControlProperty(
            "lblCompanyLogoWidth", PropertyNames.PROPERTY_ENABLED, False)
            self.setControlProperty(
            "numLogoX", PropertyNames.PROPERTY_ENABLED, False)
            self.setControlProperty(
            "lblCompanyLogoX", PropertyNames.PROPERTY_ENABLED, False)
            self.setControlProperty(
            "numLogoY", PropertyNames.PROPERTY_ENABLED, False)
            self.setControlProperty(
            "lblCompanyLogoY", PropertyNames.PROPERTY_ENABLED, False)
            self.setPossibleLogo(True)

    def chkPaperCompanyAddressItemChanged(self):
        if self.chkPaperCompanyAddress.State != 0:
            if self.numAddressWidth.Value == 0:
                self.numAddressWidth.Value = 0.1

            if self.numAddressHeight.Value == 0:
                self.numAddressHeight.Value = 0.1

            self.BusCompanyAddress = BusinessPaperObject(
                "Company Address", int(self.numAddressWidth.Value * 1000),
                int(self.numAddressHeight.Value * 1000),
                int(self.numAddressX.Value * 1000),
                int(self.numAddressY.Value * 1000))
            self.setControlProperty(
            "numAddressHeight", PropertyNames.PROPERTY_ENABLED, True)
            self.setControlProperty(
            "lblCompanyAddressHeight", PropertyNames.PROPERTY_ENABLED, True)
            self.setControlProperty(
            "numAddressWidth", PropertyNames.PROPERTY_ENABLED, True)
            self.setControlProperty(
            "lblCompanyAddressWidth", PropertyNames.PROPERTY_ENABLED, True)
            self.setControlProperty(
            "numAddressX", PropertyNames.PROPERTY_ENABLED, True)
            self.setControlProperty(
            "lblCompanyAddressX", PropertyNames.PROPERTY_ENABLED, True)
            self.setControlProperty(
            "numAddressY", PropertyNames.PROPERTY_ENABLED, True)
            self.setControlProperty(
            "lblCompanyAddressY", PropertyNames.PROPERTY_ENABLED, True)
            if self.myLetterDoc.hasElement("Sender Address"):
                self.myLetterDoc.switchElement(
                "Sender Address", False)

            if self.chkCompanyReceiver.State != 0:
                self.setPossibleSenderData(False)

        else:
            if self.BusCompanyAddress is not None:
                self.BusCompanyAddress.removeFrame()
            self.setControlProperty(
            "numAddressHeight", PropertyNames.PROPERTY_ENABLED, False)
            self.setControlProperty(
            "lblCompanyAddressHeight", PropertyNames.PROPERTY_ENABLED, False)
            self.setControlProperty(
            "numAddressWidth", PropertyNames.PROPERTY_ENABLED, False)
            self.setControlProperty(
            "lblCompanyAddressWidth", PropertyNames.PROPERTY_ENABLED, False)
            self.setControlProperty(
            "numAddressX", PropertyNames.PROPERTY_ENABLED, False)
            self.setControlProperty(
            "lblCompanyAddressX", PropertyNames.PROPERTY_ENABLED, False)
            self.setControlProperty(
            "numAddressY", PropertyNames.PROPERTY_ENABLED, False)
            self.setControlProperty(
            "lblCompanyAddressY", PropertyNames.PROPERTY_ENABLED, False)
            if self.myLetterDoc.hasElement("Sender Address"):
                self.myLetterDoc.switchElement("Sender Address", True)

            self.setPossibleSenderData(True)
            if self.optSenderDefine.State:
                self.optSenderDefineItemChanged()

            if self.optSenderPlaceholder.State:
                self.optSenderPlaceholderItemChanged()

    def chkCompanyReceiverItemChanged(self):
        xReceiverFrame = None
        if self.chkCompanyReceiver.State != 0:
            try:
                xReceiverFrame = TextDocument.getFrameByName(
                    "Receiver Address", TextDocument.xTextDocument)
                iFrameWidth = int(Helper.getUnoPropertyValue(
                    xReceiverFrame, PropertyNames.PROPERTY_WIDTH))
                iFrameX = int(Helper.getUnoPropertyValue(
                    xReceiverFrame, "HoriOrientPosition"))
                iFrameY = int(Helper.getUnoPropertyValue(
                    xReceiverFrame, "VertOrientPosition"))
                iReceiverHeight = int(0.5 * 1000)
                self.BusCompanyAddressReceiver = BusinessPaperObject(
                    " ", iFrameWidth, iReceiverHeight, iFrameX,
                    iFrameY - iReceiverHeight)
                self.setPossibleAddressReceiver(False)
            except NoSuchElementException:
                traceback.print_exc()
            except WrappedTargetException:
                traceback.print_exc()

            if self.chkPaperCompanyAddress.State != 0:
                self.setPossibleSenderData(False)

        else:
            if self.BusCompanyAddressReceiver != None:
                self.BusCompanyAddressReceiver.removeFrame()

            self.setPossibleAddressReceiver(True)
            self.setPossibleSenderData(True)
            if self.optSenderDefine.State:
                self.optSenderDefineItemChanged()

            if self.optSenderPlaceholder.State:
                self.optSenderPlaceholderItemChanged()

    def chkPaperFooterItemChanged(self):
        if self.chkPaperFooter.State != 0:
            if self.numFooterHeight.Value == 0:
                self.numFooterHeight.Value = 0.1

            self.BusFooter = BusinessPaperObject(
                "Footer", self.myLetterDoc.DocSize.Width,
                int(self.numFooterHeight.Value * 1000), 0,
                int(self.myLetterDoc.DocSize.Height - \
                    (self.numFooterHeight.Value * 1000)))
            self.setControlProperty(
            "numFooterHeight", PropertyNames.PROPERTY_ENABLED, True)
            self.setControlProperty(
            "lblFooterHeight", PropertyNames.PROPERTY_ENABLED, True)
            self.setPossibleFooter(False)
        else:
            if self.BusFooter != None:
                self.BusFooter.removeFrame()

            self.setControlProperty(
            "numFooterHeight", PropertyNames.PROPERTY_ENABLED, False)
            self.setControlProperty(
            "lblFooterHeight", PropertyNames.PROPERTY_ENABLED, False)
            self.setPossibleFooter(True)

    def chkUseLogoItemChanged(self):
        try:
            if self.myLetterDoc.hasElement("Company Logo"):
                logostatus = \
                    bool(self.getControlProperty(
                        "chkUseLogo", PropertyNames.PROPERTY_ENABLED)) \
                    and (self.chkUseLogo.State != 0)
                self.myLetterDoc.switchElement(
                "Company Logo", logostatus)
        except IllegalArgumentException, e:
            traceback.print_exc()

    def chkUseAddressReceiverItemChanged(self):
        try:
            if self.myLetterDoc.hasElement("Sender Address Repeated"):
                rstatus = \
                    bool(self.getControlProperty(
                        "chkUseAddressReceiver",
                        PropertyNames.PROPERTY_ENABLED)) \
                    and (self.chkUseAddressReceiver.State != 0)
                self.myLetterDoc.switchElement(
                    "Sender Address Repeated", rstatus)

        except IllegalArgumentException, e:
            traceback.print_exc()

    def chkUseSignsItemChanged(self):
        if self.myLetterDoc.hasElement("Letter Signs"):
            self.myLetterDoc.switchElement(
                "Letter Signs", self.chkUseSigns.State != 0)

    def chkUseSubjectItemChanged(self):
        if self.myLetterDoc.hasElement("Subject Line"):
            self.myLetterDoc.switchElement(
                "Subject Line", self.chkUseSubject.State != 0)

    def chkUseBendMarksItemChanged(self):
        if self.myLetterDoc.hasElement("Bend Marks"):
            self.myLetterDoc.switchElement(
                "Bend Marks", self.chkUseBendMarks.State != 0)

    def chkUseFooterItemChanged(self):
        try:
            bFooterPossible = (self.chkUseFooter.State != 0) \
                and bool(self.getControlProperty(
                    "chkUseFooter", PropertyNames.PROPERTY_ENABLED))
            if self.chkFooterNextPages.State != 0:
                self.myLetterDoc.switchFooter(
                    "First Page", False, self.chkFooterPageNumbers.State != 0,
                    txtFooter.Text)
                self.myLetterDoc.switchFooter("Standard", bFooterPossible,
                    self.chkFooterPageNumbers.State != 0, self.txtFooter.Text)
            else:
                self.myLetterDoc.switchFooter(
                    "First Page", bFooterPossible,
                    self.chkFooterPageNumbers.State != 0, self.txtFooter.Text)
                self.myLetterDoc.switchFooter(
                    "Standard", bFooterPossible,
                    self.chkFooterPageNumbers.State != 0, self.txtFooter.Text)

            BPaperItem = \
                self.getRoadmapItemByID(LetterWizardDialogImpl.RM_FOOTER)
            Helper.setUnoPropertyValue(
                BPaperItem, PropertyNames.PROPERTY_ENABLED, bFooterPossible)
        except Exception, exception:
            traceback.print_exc()

    def chkFooterNextPagesItemChanged(self):
        self.chkUseFooterItemChanged()

    def chkFooterPageNumbersItemChanged(self):
        self.chkUseFooterItemChanged()

    def setPossibleFooter(self, bState):
        self.setControlProperty(
            "chkUseFooter", PropertyNames.PROPERTY_ENABLED, bState)
        self.chkUseFooterItemChanged()

    def setPossibleAddressReceiver(self, bState):
        if self.myLetterDoc.hasElement("Sender Address Repeated"):
            self.setControlProperty(
            "chkUseAddressReceiver", PropertyNames.PROPERTY_ENABLED, bState)
            self.chkUseAddressReceiverItemChanged()

    def setPossibleLogo(self, bState):
        if self.myLetterDoc.hasElement("Company Logo"):
            self.setControlProperty(
            "chkUseLogo", PropertyNames.PROPERTY_ENABLED, bState)
            self.chkUseLogoItemChanged()

    def txtFooterTextChanged(self):
        self.chkUseFooterItemChanged()

    def txtSenderNameTextChanged(self):
        myFieldHandler = TextFieldHandler(
            self.myLetterDoc.xMSF, TextDocument.xTextDocument)
        myFieldHandler.changeUserFieldContent(
            "Company", self.txtSenderName.Text)

    def txtSenderStreetTextChanged(self):
        myFieldHandler = TextFieldHandler(
            self.myLetterDoc.xMSF, TextDocument.xTextDocument)
        myFieldHandler.changeUserFieldContent(
            "Street", self.txtSenderStreet.Text)

    def txtSenderCityTextChanged(self):
        myFieldHandler = TextFieldHandler(
            self.myLetterDoc.xMSF, TextDocument.xTextDocument)
        myFieldHandler.changeUserFieldContent(
            "City", self.txtSenderCity.Text)

    def txtSenderPostCodeTextChanged(self):
        myFieldHandler = TextFieldHandler(
            self.myLetterDoc.xMSF, TextDocument.xTextDocument)
        myFieldHandler.changeUserFieldContent(
            "PostCode", self.txtSenderPostCode.Text)

    def txtSenderStateTextChanged(self):
        myFieldHandler = TextFieldHandler(
            self.myLetterDoc.xMSF, TextDocument.xTextDocument)
        myFieldHandler.changeUserFieldContent(
            PropertyNames.PROPERTY_STATE, self.txtSenderState.Text)

    def txtTemplateNameTextChanged(self):
        xDocProps = TextDocument.xTextDocument.DocumentProperties
        TitleName = self.txtTemplateName.Text
        xDocProps.Title = TitleName

    def chkUseSalutationItemChanged(self):
        self.myLetterDoc.switchUserField(
            "Salutation", self.lstSalutation.Text,
            self.chkUseSalutation.State != 0)
        self.setControlProperty(
            "lstSalutation", PropertyNames.PROPERTY_ENABLED,
            self.chkUseSalutation.State != 0)

    def lstSalutationItemChanged(self):
        self.myLetterDoc.switchUserField(
            "Salutation", self.lstSalutation.Text,
            self.chkUseSalutation.State != 0)

    def chkUseGreetingItemChanged(self):
        self.myLetterDoc.switchUserField(
            "Greeting", self.lstGreeting.Text, self.chkUseGreeting.State != 0)
        self.setControlProperty(
            "lstGreeting", PropertyNames.PROPERTY_ENABLED,
            self.chkUseGreeting.State != 0)

    def setDefaultForGreetingAndSalutation(self):
        if self.lstSalutation.Text == "":
            self.lstSalutation.Text = self.resources.SalutationLabels[0]

        if self.lstGreeting.Text == "":
            self.lstGreeting.Text = self.resources.GreetingLabels[0]

    def lstGreetingItemChanged(self):
        self.myLetterDoc.switchUserField(
            "Greeting", self.lstGreeting.Text, self.chkUseGreeting.State != 0)

    def chkBusinessPaperItemChanged(self):
        if self.chkBusinessPaper.State != 0:
            self.enableBusinessPaper()
        else:
            self.disableBusinessPaper()
            self.setPossibleSenderData(True)

    def getOfficeLinguistic(self):
        oL = 0
        found = False
        OfficeLinguistic = Configuration.getOfficeLinguistic(self.xMSF)
        i = 0
        for index, workwith in enumerate(self.Norms):
            if workwith.lower() == OfficeLinguistic.lower():
                oL = index
                found = True
                break

        if not found:
            for index, workwith in enumerate(self.Norms):
                if workwith.lower() == "en-US".lower():
                    oL = index
                    found = True
                    break
        return oL

    def setPossibleSenderData(self, bState):
        self.setControlProperty(
            "optSenderDefine", PropertyNames.PROPERTY_ENABLED, bState)
        self.setControlProperty(
            "optSenderPlaceholder", PropertyNames.PROPERTY_ENABLED, bState)
        self.setControlProperty(
            "lblSenderAddress", PropertyNames.PROPERTY_ENABLED, bState)
        if not bState:
            self.setControlProperty(
            "txtSenderCity", PropertyNames.PROPERTY_ENABLED, bState)
            self.setControlProperty(
            "txtSenderName", PropertyNames.PROPERTY_ENABLED, bState)
            self.setControlProperty(
            "txtSenderPostCode", PropertyNames.PROPERTY_ENABLED, bState)
            self.setControlProperty(
            "txtSenderStreet", PropertyNames.PROPERTY_ENABLED, bState)
            self.setControlProperty(
            "txtSenderCity", PropertyNames.PROPERTY_ENABLED, bState)
            self.setControlProperty(
            "txtSenderState", PropertyNames.PROPERTY_ENABLED, bState)
            self.setControlProperty(
            "lblSenderName", PropertyNames.PROPERTY_ENABLED, bState)
            self.setControlProperty(
            "lblSenderStreet", PropertyNames.PROPERTY_ENABLED, bState)
            self.setControlProperty(
            "lblPostCodeCity", PropertyNames.PROPERTY_ENABLED, bState)

    def enableSenderReceiver(self):
        BPaperItem = self.getRoadmapItemByID(
            LetterWizardDialogImpl.RM_SENDERRECEIVER)
        Helper.setUnoPropertyValue(
            BPaperItem, PropertyNames.PROPERTY_ENABLED, True)

    def disableSenderReceiver(self):
        BPaperItem = self.getRoadmapItemByID(
            LetterWizardDialogImpl.RM_SENDERRECEIVER)
        Helper.setUnoPropertyValue(
            BPaperItem, PropertyNames.PROPERTY_ENABLED, False)

    def enableBusinessPaper(self):
        try:
            BPaperItem = self.getRoadmapItemByID(
                LetterWizardDialogImpl.RM_BUSINESSPAPER)
            Helper.setUnoPropertyValue(
                BPaperItem, PropertyNames.PROPERTY_ENABLED, True)
            self.chkPaperCompanyLogoItemChanged()
            self.chkPaperCompanyAddressItemChanged()
            self.chkPaperFooterItemChanged()
            self.chkCompanyReceiverItemChanged()
        except Exception:
            traceback.print_exc()

    def disableBusinessPaper(self):
        try:
            BPaperItem = self.getRoadmapItemByID(
                LetterWizardDialogImpl.RM_BUSINESSPAPER)
            Helper.setUnoPropertyValue(
                BPaperItem, PropertyNames.PROPERTY_ENABLED, False)
            if self.BusCompanyLogo != None:
                self.BusCompanyLogo.removeFrame()

            if self.BusCompanyAddress != None:
                self.BusCompanyAddress.removeFrame()

            if self.BusFooter != None:
                self.BusFooter.removeFrame()

            if self.BusCompanyAddressReceiver != None:
                self.BusCompanyAddressReceiver.removeFrame()

            self.setPossibleAddressReceiver(True)
            self.setPossibleFooter(True)
            self.setPossibleLogo(True)
            if self.myLetterDoc.hasElement("Sender Address"):
                self.myLetterDoc.switchElement(
                "Sender Address", True)
        except Exception:
            traceback.print_exc()

    def lstLetterNormItemChanged(self):
        sCurrentNorm = self.Norms[getCurrentLetter().cp_Norm]
        initializeTemplates(xMSF)
        if self.optBusinessLetter.State:
            self.lstBusinessStyleItemChanged()

        elif optPrivOfficialLetter.State:
            self.lstPrivOfficialStyleItemChanged()

        elif optPrivateLetter.State:
            self.lstPrivateStyleItemChanged()

    def initializeSalutation(self):
        self.setControlProperty(
            "lstSalutation", "StringItemList",
            self.resources.SalutationLabels)

    def initializeGreeting(self):
        self.setControlProperty(
            "lstGreeting", "StringItemList", self.resources.GreetingLabels)

    def initializeNorms(self):
        lc = LocaleCodes(self.xmsf)
        allLocales = lc.getIDs()
        nameList = []
        sLetterSubPath = "/wizard/letter/"
        try:
            self.sTemplatePath = \
                FileAccess.deleteLastSlashfromUrl(self.sTemplatePath)
            nuString = \
                self.sTemplatePath[:self.sTemplatePath.rfind("/")] + "/"
            sMainPath = FileAccess.deleteLastSlashfromUrl(nuString)
            self.sLetterPath = sMainPath + sLetterSubPath
            xInterface = \
                self.xmsf.createInstance("com.sun.star.ucb.SimpleFileAccess")
            nameList = xInterface.getFolderContents(self.sLetterPath, True)
        except Exception, e:
            traceback.print_exc()

        found = False
        cIsoCode = ""
        MSID = ""
        LanguageLabels = []

        for i in nameList:
            found = False
            cIsoCode = FileAccess.getFilename(i)
            for j in allLocales:
                aLang = j.split(";")
                if cIsoCode.lower() == aLang[1].lower():
                    MSID = aLang[2]
                    found = True
                    break

            if not found:
                for j in allLocales:
                    aLang = j.split(";")
                    if cIsoCode.lower() == aLang[1][:2]:
                        MSID = aLang[2]
                        found = True
                        break

            if found:
                self.Norms.append(cIsoCode)
                self.NormPaths.append(i)
                #LanguageLabels.append(lc.getLanguageString(MSID))

        self.setControlProperty(
            "lstLetterNorm", "StringItemList", tuple(LanguageLabels))

    def getCurrentLetter(self):
        if self.myConfig.cp_LetterType == 0:
            return self.myConfig.cp_BusinessLetter
        elif self.myConfig.cp_LetterType == 1:
            return self.myConfig.cp_PrivateOfficialLetter
        elif self.myConfig.cp_LetterType == 2:
            return self.myConfig.cp_PrivateLetter
        else:
            return None

    def __initializePaths(self):
        try:
            self.sTemplatePath = \
                FileAccess.getOfficePath2(
                    self.xMSF, "Template", "share", "/wizard")
            self.sUserTemplatePath = \
                FileAccess.getOfficePath2(self.xMSF, "Template", "user", "")
            self.sBitmapPath = \
                FileAccess.combinePaths(
                    self.xMSF, self.sTemplatePath, "/../wizard/bitmap")
        except NoValidPathException, e:
            traceback.print_exc()

    def initializeTemplates(self, xMSF):
        self.sCurrentNorm = self.Norms[self.getCurrentLetter().cp_Norm]
        sLetterPath = self.NormPaths[self.getCurrentLetter().cp_Norm]
        self.BusinessFiles = \
            FileAccess.getFolderTitles(xMSF, "bus", sLetterPath)
        self.OfficialFiles = \
            FileAccess.getFolderTitles(xMSF, "off", sLetterPath)
        self.PrivateFiles = \
            FileAccess.getFolderTitles(xMSF, "pri", sLetterPath)
        self.setControlProperty(
            "lstBusinessStyle", "StringItemList",
            tuple(self.BusinessFiles[0]))
        self.setControlProperty(
            "lstPrivOfficialStyle", "StringItemList",
            tuple(self.OfficialFiles[0]))
        self.setControlProperty(
            "lstPrivateStyle", "StringItemList",
            tuple(self.PrivateFiles[0]))
        self.setControlProperty(
            "lstBusinessStyle", "SelectedItems", (0,))
        self.setControlProperty(
            "lstPrivOfficialStyle", "SelectedItems", (0,))
        self.setControlProperty(
            "lstPrivateStyle", "SelectedItems", (0,))
        return True

    def initializeElements(self):
        self.setControlProperty(
            "chkUseLogo", PropertyNames.PROPERTY_ENABLED,
            self.myLetterDoc.hasElement("Company Logo"))
        self.setControlProperty(
            "chkUseBendMarks", PropertyNames.PROPERTY_ENABLED,
            self.myLetterDoc.hasElement("Bend Marks"))
        self.setControlProperty(
            "chkUseAddressReceiver", PropertyNames.PROPERTY_ENABLED,
            self.myLetterDoc.hasElement("Sender Address Repeated"))
        self.setControlProperty(
            "chkUseSubject", PropertyNames.PROPERTY_ENABLED,
            self.myLetterDoc.hasElement("Subject Line"))
        self.setControlProperty(
            "chkUseSigns", PropertyNames.PROPERTY_ENABLED,
            self.myLetterDoc.hasElement("Letter Signs"))
        self.myLetterDoc.updateDateFields()

    def setConfiguration(self):
        if self.optBusinessLetter.State:
            self.optBusinessLetterItemChanged()

        elif self.optPrivOfficialLetter.State:
            self.optPrivOfficialLetterItemChanged()

        elif self.optPrivateLetter.State:
            self.optPrivateLetterItemChanged()

    def setElements(self, privLetter):
        if self.optSenderDefine.State:
            self.optSenderDefineItemChanged()

        if self.optSenderPlaceholder.State:
            self.optSenderPlaceholderItemChanged()

        self.chkUseSignsItemChanged()
        self.chkUseSubjectItemChanged()
        self.chkUseSalutationItemChanged()
        self.chkUseGreetingItemChanged()
        self.chkUseBendMarksItemChanged()
        self.chkUseAddressReceiverItemChanged()
        self.txtTemplateNameTextChanged()
        if self.optReceiverDatabase.State and not privLetter:
            self.optReceiverDatabaseItemChanged()

        if self.optReceiverPlaceholder.State and not privLetter:
            self.optReceiverPlaceholderItemChanged()

        if self.optCreateLetter.State:
            self.optCreateFromTemplateItemChanged()

        if self.optMakeChanges.State:
            self.optMakeChangesItemChanged()

    def insertRoadmap(self):
        self.addRoadmap()

        self.insertRoadMapItems(
                [True, False, True, True, False, True],
                self.resources.RoadmapLabels)

        self.setRoadmapInteractive(True)
        self.setRoadmapComplete(True)
        self.setCurrentRoadmapItemID(1)

    def insertPathSelectionControl(self):
        self.myPathSelection = \
            PathSelection(self.xMSF, self, PathSelection.TransferMode.SAVE,
                PathSelection.DialogTypes.FILE)
        self.myPathSelection.insert(
            6, 97, 70, 205, 45, self.resources.reslblTemplatePath_value,
            True, HelpIds.getHelpIdString(HID + 47),
            HelpIds.getHelpIdString(HID + 48))
        self.myPathSelection.sDefaultDirectory = self.sUserTemplatePath
        self.myPathSelection.sDefaultName = "myLetterTemplate.ott"
        self.myPathSelection.sDefaultFilter = "writer8_template"
        self.myPathSelection.addSelectionListener(
            self.myPathSelectionListener())

    def initConfiguration(self):
        try:
            root = Configuration.getConfigurationRoot(
                self.xMSF, "/org.openoffice.Office.Writer/Wizards/Letter",
                False)
            self.myConfig.readConfiguration(root, "cp_")
            RadioDataAware.attachRadioButtons(self.myConfig, "cp_LetterType",
                (self.optBusinessLetter, self.optPrivOfficialLetter,
                    self.optPrivateLetter), True).updateUI()
            UnoDataAware.attachListBox(
                self.myConfig.cp_BusinessLetter, "cp_Style",
                self.lstBusinessStyle, True).updateUI()
            UnoDataAware.attachListBox(
                self.myConfig.cp_PrivateOfficialLetter, "cp_Style",
                self.lstPrivOfficialStyle, True).updateUI()
            UnoDataAware.attachListBox(
                self.myConfig.cp_PrivateLetter, "cp_Style",
                self.lstPrivateStyle, True).updateUI()
            UnoDataAware.attachCheckBox(
                self.myConfig.cp_BusinessLetter, "cp_BusinessPaper",
                self.chkBusinessPaper, True).updateUI()
            cgl = self.myConfig.cp_BusinessLetter
            cgpl = self.myConfig.cp_BusinessLetter.cp_CompanyLogo
            cgpa = self.myConfig.cp_BusinessLetter.cp_CompanyAddress
            UnoDataAware.attachCheckBox(
                cgpl, "cp_Display", self.chkPaperCompanyLogo, True).updateUI()
            UnoDataAware.attachNumericControl(
                cgpl, "cp_Width", self.numLogoWidth, True).updateUI()
            UnoDataAware.attachNumericControl(
                cgpl, "cp_Height", self.numLogoHeight, True).updateUI()
            UnoDataAware.attachNumericControl(
                cgpl, "cp_X", self.numLogoX, True).updateUI()
            UnoDataAware.attachNumericControl(
                cgpl, "cp_Y", self.numLogoY, True).updateUI()
            UnoDataAware.attachCheckBox(
                cgpa, "cp_Display", self.chkPaperCompanyAddress, True).updateUI()
            UnoDataAware.attachNumericControl(
                cgpa, "cp_Width", self.numAddressWidth, True).updateUI()
            UnoDataAware.attachNumericControl(
                cgpa, "cp_Height", self.numAddressHeight, True).updateUI()
            UnoDataAware.attachNumericControl(
                cgpa, "cp_X", self.numAddressX, True).updateUI()
            UnoDataAware.attachNumericControl(
                cgpa, "cp_Y", self.numAddressY, True).updateUI()
            UnoDataAware.attachCheckBox(
                cgl, "cp_PaperCompanyAddressReceiverField",
                self.chkCompanyReceiver, True).updateUI()
            UnoDataAware.attachCheckBox(
                cgl, "cp_PaperFooter", self.chkPaperFooter, True).updateUI()
            UnoDataAware.attachNumericControl(
                cgl, "cp_PaperFooterHeight", self.numFooterHeight, True).updateUI()
            UnoDataAware.attachListBox(
                cgl, "cp_Norm", self.lstLetterNorm, True).updateUI()
            UnoDataAware.attachCheckBox(
                cgl, "cp_PrintCompanyLogo", self.chkUseLogo, True).updateUI()
            UnoDataAware.attachCheckBox(
                cgl, "cp_PrintCompanyAddressReceiverField",
                self.chkUseAddressReceiver, True).updateUI()
            UnoDataAware.attachCheckBox(
                cgl, "cp_PrintLetterSigns", self.chkUseSigns, True).updateUI()
            UnoDataAware.attachCheckBox(
                cgl, "cp_PrintSubjectLine", self.chkUseSubject, True).updateUI()
            UnoDataAware.attachCheckBox(
                cgl, "cp_PrintSalutation", self.chkUseSalutation, True).updateUI()
            UnoDataAware.attachCheckBox(
                cgl, "cp_PrintBendMarks", self.chkUseBendMarks, True).updateUI()
            UnoDataAware.attachCheckBox(
                cgl, "cp_PrintGreeting", self.chkUseGreeting, True).updateUI()
            UnoDataAware.attachCheckBox(
                cgl, "cp_PrintFooter", self.chkUseFooter, True).updateUI()
            UnoDataAware.attachEditControl(
                cgl, "cp_Salutation", self.lstSalutation, True).updateUI()
            UnoDataAware.attachEditControl(
                cgl, "cp_Greeting", self.lstGreeting, True).updateUI()
            RadioDataAware.attachRadioButtons(
                cgl, "cp_SenderAddressType",
                (self.optSenderDefine, self.optSenderPlaceholder), True).updateUI()
            UnoDataAware.attachEditControl(
                cgl, "cp_SenderCompanyName", self.txtSenderName, True).updateUI()
            UnoDataAware.attachEditControl(
                cgl, "cp_SenderStreet", self.txtSenderStreet, True).updateUI()
            UnoDataAware.attachEditControl(
                cgl, "cp_SenderPostCode", self.txtSenderPostCode, True).updateUI()
            UnoDataAware.attachEditControl(
                cgl, "cp_SenderState", self.txtSenderState, True).updateUI()
            UnoDataAware.attachEditControl(
                cgl, "cp_SenderCity", self.txtSenderCity, True).updateUI()
            RadioDataAware.attachRadioButtons(
                cgl, "cp_ReceiverAddressType",
                (self.optReceiverDatabase, self.optReceiverPlaceholder),
                True).updateUI()
            UnoDataAware.attachEditControl(
                cgl, "cp_Footer", self.txtFooter, True).updateUI()
            UnoDataAware.attachCheckBox(
                cgl, "cp_FooterOnlySecondPage",
                self.chkFooterNextPages, True).updateUI()
            UnoDataAware.attachCheckBox(
                cgl, "cp_FooterPageNumbers",
                self.chkFooterPageNumbers, True).updateUI()
            RadioDataAware.attachRadioButtons(
                cgl, "cp_CreationType",
                (self.optCreateLetter, self.optMakeChanges), True).updateUI()
            UnoDataAware.attachEditControl(
                cgl, "cp_TemplateName", self.txtTemplateName, True).updateUI()
            UnoDataAware.attachEditControl(
                cgl, "cp_TemplatePath", self.myPathSelection.xSaveTextBox,
                True).updateUI()
        except Exception, exception:
            traceback.print_exc()

    def saveConfiguration(self):
        try:
            root = Configuration.getConfigurationRoot(self.xMSF, "/org.openoffice.Office.Writer/Wizards/Letter", True)
            self.myConfig.writeConfiguration(root, "cp_")
            root.commitChanges()
        except Exception, e:
            traceback.print_exc()
