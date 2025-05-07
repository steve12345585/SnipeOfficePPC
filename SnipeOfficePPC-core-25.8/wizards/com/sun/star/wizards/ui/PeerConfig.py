from wizards.ui.event.CommonListener import WindowListenerProcAdapter

'''
@author rpiterman
To change the template for this generated type comment go to
Window>Preferences>Java>Code Generation>Code and Comments
'''

class PeerConfig(object):

    def __init__(self, _oUnoDialog):
        self.oUnoDialog = _oUnoDialog
        self.oUnoDialog.xUnoDialog.addWindowListener(
            WindowListenerProcAdapter(self.windowShown))
        self.m_aPeerTasks = []
        self.aImageUrlTasks = []

    class PeerTask(object):

        def __init__(self, _xControl, _propNames, _propValues):
            self.propnames = _propNames
            self.propvalues = _propValues
            self.xControl = _xControl

    class ImageUrlTask(object):

        def __init__(self, _oModel, _oResource, _oHCResource):
            self.oModel = _oModel
            self.oResource = _oResource
            self.oHCResource = _oHCResource

    def windowShown(self):
        try:
            for i in self.m_aPeerTasks:
                xVclWindowPeer = i.xControl.Peer
                xVclWindowPeer.setProperty(i.propnames, i.propvalues)

            for i in self.aImageUrlTasks:
                if isinstance(aImageUrlTask.oResource, int):
                    sImageUrl = oUnoDialog.getWizardImageUrl(aImageUrlTask.oResource, aImageUrlTask.oHCResource)
                elif isinstance(aImageUrlTask.oResource, str):
                    sImageUrl = oUnoDialog.getImageUrl(aImageUrlTask.oResource, aImageUrlTask.oHCResource)
                if sImageUrl != "":
                    Helper.setUnoPropertyValue(aImageUrlTask.oModel, PropertyNames.PROPERTY_IMAGEURL, sImageUrl)

        except Exception:
            traceback.print_exc()

    '''
    @param oAPIControl an API control that the interface
        XControl can be derived from
    @param _propnames
    @param _propvalues
    '''

    def setPeerProperties(self, _xControl, propnames, propvalues):
        oPeerTask = self.PeerTask(_xControl, propnames, propvalues)
        self.m_aPeerTasks.append(oPeerTask)

    def setImageUrl(self, _ocontrolmodel, _oResource,  _oHCResource):
        oImageUrlTask = self.ImageUrlTask(_ocontrolmodel, _oResource, _oHCResource)
        self.aImageUrlTasks.append(oImageUrlTask)
