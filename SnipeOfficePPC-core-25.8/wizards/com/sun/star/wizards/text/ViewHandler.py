class ViewHandler(object):
    '''Creates a new instance of View '''

    def __init__ (self, xMSF, xTextDocument):
        self.xMSFDoc = xMSF
        self.xTextDocument = xTextDocument
        self.xTextViewCursorSupplier = xTextDocument.CurrentController

    def selectFirstPage(self, oTextTableHandler):
        try:
            xPageCursor = self.xTextViewCursorSupplier.ViewCursor
            xPageCursor.jumpToFirstPage()
            xPageCursor.jumpToStartOfPage()
            Helper.setUnoPropertyValue(
                xPageCursor, "PageDescName", "First Page")
            oPageStyles = self.xTextDocument.StyleFamilies.getByName(
                "PageStyles")
            oPageStyle = oPageStyles.getByName("First Page")
            xAllTextTables = oTextTableHandler.xTextTablesSupplier.TextTables
            xTextTable = xAllTextTables.getByIndex(0)
            xRange = xTextTable.Anchor.Text
            xPageCursor.gotoRange(xRange, False)
            if not com.sun.star.uno.AnyConverter.isVoid(XTextRange):
                xViewTextCursor.gotoRange(xHeaderRange, False)
                xViewTextCursor.collapseToStart()
            else:
                print "No Headertext available"

        except com.sun.star.uno.Exception, exception:
            exception.printStackTrace(System.out)

    def setViewSetting(self, Setting, Value):
        self.xTextViewCursorSupplier.ViewSettings.setPropertyValue(Setting, Value)

    def collapseViewCursorToStart(self):
        xTextViewCursor = self.xTextViewCursorSupplier.ViewCursor
        xTextViewCursor.collapseToStart()
