from threading import RLock
from CGTopic import CGTopic
from wizards.ui.ControlScroller import *
from AgendaWizardDialogConst import LAST_HID
from wizards.common.Properties import Properties
from wizards.ui.event.CommonListener import FocusListenerProcAdapter, KeyListenerProcAdapter

from com.sun.star.awt.Key import DOWN, UP, TAB
from com.sun.star.awt.KeyModifier import SHIFT, MOD1

'''
@author rpiterman
This class implements the UI functionality of the topics scroller control.
<br/>
During developement, there has been a few changes which were not *fully* done
mainly in converting the topics and time boxes
from combobox and time box to normal textboxes,
so in the code they might be referenced as combobox or timebox. This should be
rather understood as topicstextbox and timetextbox.<br/><br/>
Important behaiviour of this control is that there is always a
blank row at the end, in which the user can enter data.<br/>
Once the row is not blank (thus, the user entered data...),
a new blank row is added.<br/>
Once the user removes the last *unempty* row, binsertRowy deleteing its data, it becomes
the *last empty row* and the one after is being automatically removed.<br/><br/>
The contorl shows 5 rows at a time.<br/>
If, for example, only 2 rows exist (from which the 2ed one is empty...)
then the other three rows, which do not exist in the data model, are disabled.
<br/>
The following other functionality is implemented:
<br/>
0. synchroniting data between controls, data model and live preview.
1. Tab scrolling.<br/>
2. Keyboard scrolling.<br/>
3. Removing rows and adding new rows.<br/>
4. Moving rows up and down. <br/>
<br/>
This control relays on the ControlScroller control which uses the following
Data model:<br/>
1. It uses a vector, whos members are arrays of PropertyValue.<br/>
2. Each array represents a row.<br/>
(Note: the Name and Value memebrs of the PropertyValue object are being used)
3. Each property Value represents a value
for a single control with the following rules:<br/>
3. a. the Value of the property is used for as value
of the controls (usually text).<br/>
3. b. the Name of the property is used to map values
to UI controls in the following manner:<br/>
3. b. 1. only the Name of the first X Rows is regarded,
where X is the number of visible rows (in the ainsertRowgenda wizard this would be 5,
since 5 topic rows are visible on the dialog).<br/>
3. b. 2. The Names of the first X (or 5...) rows are the names
of the UI Controls to hold values. When the control scroller scrolls,
it looks at the first 5 rows and uses the names specified there to map the
current values to the specified controls. <br/>
This data model makes the following limitations on the implementation:
When moving rows, only the values should be moved. The Rows objects,
which contain also the Names of the controls should not be switched. <br/>
also when deleting or inserting rows, attention should be paid that no rows
should be removed or inserted. Instead, only the Values should rotate. <br/><br/>
To save the topics in the registry a ConfigSet of objects of type CGTopic is
being used.
This one is not synchronized "live", since it is unnecessary... instead, it is
synchronized on call, before the settings should be saved.
'''

def synchronized(lock):
    ''' Synchronization decorator. '''
    def wrap(f):
        def newFunction(*args, **kw):
            lock.acquire()
            try:
                return f(*args, **kw)
            finally:
                lock.release()
        return newFunction
    return wrap

class TopicsControl(ControlScroller):

    lock = RLock()
    LABEL = "lblTopicCnt_"
    TOPIC = "txtTopicTopic_"
    RESP = "cbTopicResp_"
    TIME = "txtTopicTime_"
    nscrollvalue = 0
    LABEL_PROPS = (PropertyNames.PROPERTY_HEIGHT, PropertyNames.PROPERTY_LABEL,
        PropertyNames.PROPERTY_POSITION_X, PropertyNames.PROPERTY_POSITION_Y,
        PropertyNames.PROPERTY_STEP, PropertyNames.PROPERTY_TABINDEX,
        PropertyNames.PROPERTY_WIDTH)
    TEXT_PROPS = (PropertyNames.PROPERTY_HEIGHT, PropertyNames.PROPERTY_HELPURL,
        PropertyNames.PROPERTY_POSITION_X, PropertyNames.PROPERTY_POSITION_Y,
        PropertyNames.PROPERTY_STEP, PropertyNames.PROPERTY_TABINDEX,
        PropertyNames.PROPERTY_WIDTH)
    lastFocusRow = 0
    lastFocusControl = None

    def __init__(self, dialog, xmsf, agenda):
        try:
            super(TopicsControl, self).__init__(
                dialog, xmsf, 5, 92, 38, 212, 5, 18, LAST_HID)
            self.initializeScrollFields(agenda)
            # set some focus listeners for TAB scroll down and up...
            # prepare scroll down on tab press...
            self.lastTime = \
                self.ControlGroupVector[self.nblockincrement - 1].timebox

            self.lastTime.addKeyListener(KeyListenerProcAdapter(
                self.lastControlKeyPressed))
            #prepare scroll up on tab press...
            self.firstTopic = self.ControlGroupVector[0].textbox
            self.firstTopic.addKeyListener(KeyListenerProcAdapter(
                self.firstControlKeyPressed))
            self.enableButtons()
        except Exception:
            traceback.print_exc()

    '''
    initializes the data of the control.
    @param agenda
    '''

    def initializeScrollFields(self, agenda):
        # create a row for each topic with the given values....
        for i  in xrange(agenda.cp_Topics.getSize()):
            row = self.newRow(i)
            agenda.cp_Topics.getElementAt(i).setDataToRow(row)
            # a parent class method
            self.registerControlGroup(row, i)
            self.updateDocumentRow(i)
        # inserts a blank row at the end...
        self.insertRowAtEnd()

    '''
    Insert a blank (empty) row
    as last row of the control.
    The control has always a blank row at the
    end, which enables the user to enter data...
    '''

    @classmethod
    def insertRowAtEnd(self):
        l = len(ControlScroller.scrollfields)
        self.registerControlGroup(self.newRow(l), l)
        self.setTotalFieldCount(l + 1)
        # if the new row is visible, it must have been disabled
        # so it should be now enabled...
        if l - ControlScroller.nscrollvalue < self.nblockincrement:
            self.ControlGroupVector[l - ControlScroller.nscrollvalue].\
                setEnabled(True)

    '''
    remove the last row
    '''

    @classmethod
    def removeLastRow(self):
        l = len(ControlScroller.scrollfields)
        # if we should scroll up...
        if (l - ControlScroller.nscrollvalue) >= 1 \
                and (l - ControlScroller.nscrollvalue) <= self.nblockincrement \
                and ControlScroller.nscrollvalue > 0:
            while (l - ControlScroller.nscrollvalue >= 1) \
                    and l - ControlScroller.nscrollvalue <= self.nblockincrement \
                    and ControlScroller.nscrollvalue > 0:
                self.setScrollValue(ControlScroller.nscrollvalue - 1)
        # if we should disable a row...
        elif ControlScroller.nscrollvalue == 0 and l - 1 < self.nblockincrement:
            self.ControlGroupVector[l - 1].setEnabled(False)

        self.unregisterControlGroup(l - 1)
        self.setTotalFieldCount(l - 1)

    '''
    in order to use the "move up", "down" "insert" and "remove" buttons,
    we track the last control the gained focus, in order to know which
    row should be handled.
    @param fe
    '''

    @classmethod
    def focusGained(self, fe):
        xc = fe.Source
        self.focusGained2(xc)

    '''
    Sometimes I set the focus programatically to a control
    (for example when moving a row up or down, the focus should move
    with it).
    In such cases, no VCL event is being triggered so it must
    be called programtically.
    This is done by this method.
    @param control
    '''

    @classmethod
    def focusGained2(self, control):
        try:
            #calculate in which row we are...
            name = Helper.getUnoPropertyValue(
                control.Model, PropertyNames.PROPERTY_NAME)
            num = name[name.index("_") + 1:]
            TopicsControl.lastFocusRow = int(num) + ControlScroller.nscrollvalue
            TopicsControl.lastFocusControl = control
            # enable/disable the buttons...
            self.enableButtons()
        except Exception:
            traceback.print_exc()

    '''
    enable or disable the buttons according to the
    current row we are in.
    '''

    @classmethod
    def enableButtons(self):
        UnoDialog.setEnabled(
            ControlScroller.CurUnoDialog.btnInsert,
            TopicsControl.lastFocusRow < len(ControlScroller.scrollfields))
        UnoDialog.setEnabled(
            ControlScroller.CurUnoDialog.btnRemove,
            TopicsControl.lastFocusRow < len(ControlScroller.scrollfields) - 1)
        if TopicsControl.lastFocusControl is not None:
            UnoDialog.setEnabled(
                ControlScroller.CurUnoDialog.btnUp, TopicsControl.lastFocusRow > 0)
            UnoDialog.setEnabled(
                ControlScroller.CurUnoDialog.btnDown,
                TopicsControl.lastFocusRow < len(ControlScroller.scrollfields) - 1)
        else:
            UnoDialog.setEnabled(
                ControlScroller.CurUnoDialog.btnUp, False)
            UnoDialog.setEnabled(
                ControlScroller.CurUnoDialog.btnDown, False)

    '''
    Removes the current row.
    See general class documentation explanation about the
    data model used and the limitations which explain the implementation here.
    '''

    def removeRow(self):
        try:
            for i in xrange(TopicsControl.lastFocusRow,
                    len(ControlScroller.scrollfields) - 1):
                pv1 = ControlScroller.scrollfields[i]
                pv2 = ControlScroller.scrollfields[i + 1]
                pv1[1].Value = pv2[1].Value
                pv1[2].Value = pv2[2].Value
                pv1[3].Value = pv2[3].Value
                self.updateDocumentRow(i)
                if i - ControlScroller.nscrollvalue < self.nblockincrement:
                    self.fillupControl(i - ControlScroller.nscrollvalue)

            self.removeLastRow()
            # update the live preview background document
            self.reduceDocumentToTopics()
            self.enableButtons()
            if TopicsControl.lastFocusControl is not None:
                # the focus should return to the edit control
                self.focus(TopicsControl.lastFocusControl)
        except Exception:
            traceback.print_exc()

    '''
    Inserts a row before the current row.
    See general class documentation explanation about the
    data model used and the limitations which explain the implementation here.
    '''

    def insertRow(self):
        try:
            self.insertRowAtEnd()
            for i in xrange(len(ControlScroller.scrollfields) - 2,
                    TopicsControl.lastFocusRow, -1):
                pv1 = ControlScroller.scrollfields[i]
                pv2 = ControlScroller.scrollfields[i - 1]
                pv1[1].Value = pv2[1].Value
                pv1[2].Value = pv2[2].Value
                pv1[3].Value = pv2[3].Value
                self.updateDocumentRow(i)
                if i - ControlScroller.nscrollvalue < self.nblockincrement:
                    self.fillupControl(i - ControlScroller.nscrollvalue)

            # after rotating all the properties from this row on,
            # we clear the row, so it is practically a new one...
            pv1 = ControlScroller.scrollfields[TopicsControl.lastFocusRow]
            pv1[1].Value = ""
            pv1[2].Value = ""
            pv1[3].Value = ""
            # update the preview document.
            self.updateDocumentRow(TopicsControl.lastFocusRow)
            self.fillupControl(
                TopicsControl.lastFocusRow - ControlScroller.nscrollvalue)
            self.enableButtons()

            if TopicsControl.lastFocusControl is not None:
                self.focus(TopicsControl.lastFocusControl)
        except Exception:
            traceback.print_exc()

    '''
    create a new row with the given index.
    The index is important because it is used in the
    Name member of the PropertyValue objects.
    To know why see general class documentation above (data model explanation)
    @param i the index of the new row
    @return
    '''

    @classmethod
    def newRow(self, i):
        pv = [None] * 4
        pv[0] = Properties.createProperty(
            TopicsControl.LABEL + str(i), "" + str(i + 1) + ".")
        pv[1] = Properties.createProperty(TopicsControl.TOPIC + str(i), "")
        pv[2] = Properties.createProperty(TopicsControl.RESP + str(i), "")
        pv[3] = Properties.createProperty(TopicsControl.TIME + str(i), "")
        return pv

    '''
    Implementation of ControlScroller
    This is a UI method which inserts a new row to the control.
    It uses the child-class ControlRow. (see below).
    @param _index
    @param npos
    @see ControlRow
    '''

    def insertControlGroup(self, _index, npos):
        oControlRow = ControlRow(
            ControlScroller.CurUnoDialog, self.iCompPosX, npos, _index,
            ControlRow.tabIndex)
        self.ControlGroupVector.append(oControlRow)
        ControlRow.tabIndex += 4

    '''
    Checks if a row is empty.
    This is used when the last row is changed.
    If it is empty, the next row (which is always blank) is removed.
    If it is not empty, a next row must exist.
    @param row the index number of the row to check.
    @return true if empty. false if not.
    '''

    @classmethod
    def isRowEmpty(self, row):
        data = self.getTopicData(row)
        # now - is this row empty?
        return data[1].Value  and data[2].Value and data[3].Value

    '''
    update the preview document and
    remove/insert rows if needed.
    @param guiRow
    @param column
    '''
    oldData = []

    @classmethod
    def fieldChanged(self, guiRow, column):
        with TopicsControl.lock:
            try:
                # First, I update the document
                data = self.getTopicData(guiRow + ControlScroller.nscrollvalue)
                if data is None:
                    return

                dataValue = [i.Value for i in data]
                if dataValue == TopicsControl.oldData:
                    return
                else:
                    TopicsControl.oldData = dataValue

                self.updateDocumentCell(
                    guiRow + ControlScroller.nscrollvalue, column, data)
                if self.isRowEmpty(guiRow + ControlScroller.nscrollvalue):
                    '''
                    if this is the row before the last one
                    (the last row is always empty)
                    delete the last row...
                    '''
                    if (guiRow + ControlScroller.nscrollvalue) \
                            == len(ControlScroller.scrollfields) - 2:
                        self.removeLastRow()
                        '''now consequentially check the last two rows,
                        and remove the last one if they are both empty.
                        (actually I check always the "before last" row,
                        because the last one is always empty...
                        '''
                        while len(ControlScroller.scrollfields) > 1 \
                                and self.isRowEmpty(len(ControlScroller.scrollfields) - 2):
                            removeLastRow()
                        cr = self.ControlGroupVector[
                            ControlScroller.scrollfields.size - ControlScroller.nscrollvalue - 1]
                        # if a remove was performed, set focus
                        #to the last row with some data in it...
                        self.focus(getControl(cr, column))
                        # update the preview document.
                        self.reduceDocumentToTopics()

                else:
                    # row contains data
                    # is this the last row?
                    if (guiRow + ControlScroller.nscrollvalue + 1) \
                            == len(ControlScroller.scrollfields):
                        self.insertRowAtEnd()

            except Exception:
                traceback.print_exc()

    '''
    return the corresponding row data for the given index.
    @param topic index of the topic to get.
    @return a PropertyValue array with the data for the given topic.
    '''

    @classmethod
    def getTopicData(self, topic):
        if topic < len(ControlScroller.scrollfields):
            return ControlScroller.scrollfields[topic]
        else:
            return None

    '''
    If the user presses tab on the last control, and
    there *are* more rows in the model, scroll down.
    @param event
    '''

    def lastControlKeyPressed(self, event):
        # if tab without shift was pressed...
        try:
            if event.KeyCode == TAB and event.Modifiers == 0:
                # if there is another row...
                if (self.nblockincrement + ControlScroller.nscrollvalue) \
                        < len(ControlScroller.scrollfields):
                    self.setScrollValue(ControlScroller.nscrollvalue + 1)
                    self.focus(self.getControlByIndex(self.ControlGroupVector[4], 1))
        except Exception:
            traceback.print_exc()

    '''
    If the user presses shift-tab on the first control, and
    there *are* more rows in the model, scroll up.
    @param event
    '''

    def firstControlKeyPressed(self, event):
        # if tab with shift was pressed...
        if (event.KeyCode == TAB) and \
                (event.Modifiers == SHIFT):
            if ControlScroller.nscrollvalue > 0:
                setScrollValue(ControlScroller.nscrollvalue - 1)
                focus(self.lastTime)

    '''
    sets focus to the given control.
    @param textControl
    '''

    @classmethod
    def focus(self, textControl):
        textControl.setFocus()
        text = textControl.Text
        textControl.Selection = uno.createUnoStruct( \
                    'com.sun.star.awt.Selection', 0, len(text))
        self.focusGained2(textControl)

    '''
    moves the given row one row down.
    @param guiRow the gui index of the row to move.
    @param control the control to gain focus after moving.
    '''

    @synchronized(lock)
    def rowDown(self, guiRow=None, control=None):
        try:
            if guiRow is None and control is None:
                guiRow = TopicsControl.lastFocusRow - ControlScroller.nscrollvalue
                control = TopicsControl.lastFocusControl
            # only perform if this is not the last row.
            actuallRow = guiRow + ControlScroller.nscrollvalue
            if actuallRow + 1 < len(ControlScroller.scrollfields):
                # get the current selection
                selection = control.Selection
                # the last row should scroll...
                scroll = (guiRow == self.nblockincrement - 1)
                if scroll:
                    self.setScrollValue(ControlScroller.nscrollvalue + 1)

                scroll1 = ControlScroller.nscrollvalue
                if scroll:
                    aux = -1
                else:
                    aux = 1
                self.switchRows(guiRow, guiRow + aux)
                if ControlScroller.nscrollvalue != scroll1:
                    guiRow += (ControlScroller.nscrollvalue - scroll1)

                self.setSelection(guiRow + (not scroll), control, selection)
        except Exception:
            traceback.print_exc()

    '''
    move the current row up
    '''

    @synchronized(lock)
    def rowUp(self, guiRow=None, control=None):
        try:
            if guiRow is None and control is None:
                guiRow = TopicsControl.lastFocusRow - ControlScroller.nscrollvalue
                control = TopicsControl.lastFocusControl
            # only perform if this is not the first row
            actuallRow = guiRow + ControlScroller.nscrollvalue
            if actuallRow > 0:
                # get the current selection
                selection = control.Selection
                # the last row should scroll...
                scroll = (guiRow == 0)
                if scroll:
                    self.setScrollValue(ControlScroller.nscrollvalue - 1)
                if scroll:
                    aux = 1
                else:
                    aux = -1
                self.switchRows(guiRow, guiRow + aux)
                self.setSelection(guiRow - (not scroll), control, selection)
        except Exception:
            traceback.print_exc()

    '''
    moves the cursor up.
    @param guiRow
    @param control
    '''

    @classmethod
    @synchronized(lock)
    def cursorUp(self, guiRow, control):
        # is this the last full row ?
        actuallRow = guiRow + ControlScroller.nscrollvalue
        #if this is the first row
        if actuallRow == 0:
            return
            # the first row should scroll...

        scroll = (guiRow == 0)
        if scroll:
            self.setScrollValue(ControlScroller.nscrollvalue - 1)
            upperRow = self.ControlGroupVector[guiRow]
        else:
            upperRow = self.ControlGroupVector[guiRow - 1]

        self.focus(self.getControl(upperRow, control))

    '''
    moves the cursor down
    @param guiRow
    @param control
    '''

    @classmethod
    @synchronized(lock)
    def cursorDown(self, guiRow, control):
        # is this the last full row ?
        actuallRow = guiRow + ControlScroller.nscrollvalue
        #if this is the last row, exit
        if actuallRow == len(ControlScroller.scrollfields) - 1:
            return
            # the first row should scroll...

        scroll = (guiRow == self.nblockincrement - 1)
        if scroll:
            self.setScrollValue(ControlScroller.nscrollvalue + 1)
            lowerRow = self.ControlGroupVector[guiRow]
        else:
            # if we scrolled we are done...
            #otherwise..
            lowerRow = self.ControlGroupVector[guiRow + 1]

        self.focus(self.getControl(lowerRow, control))

    '''
    changes the values of the given rows with eachother
    @param row1 one can figure out what this parameter is...
    @param row2 one can figure out what this parameter is...
    '''

    def switchRows(self, row1, row2):
        o1 = ControlScroller.scrollfields[row1 + ControlScroller.nscrollvalue]
        o2 = ControlScroller.scrollfields[row2 + ControlScroller.nscrollvalue]
        temp = None
        for i in xrange(1, len(o1)):
            temp = o1[i].Value
            o1[i].Value = o2[i].Value
            o2[i].Value = temp
        self.fillupControl(row1)
        self.fillupControl(row2)
        self.updateDocumentRow(row1 + ControlScroller.nscrollvalue, o1)
        self.updateDocumentRow(row2 + ControlScroller.nscrollvalue, o2)

        '''
        if we changed the last row, add another one...
        '''
        if (row1 + ControlScroller.nscrollvalue + 1 == \
                    len(ControlScroller.scrollfields)) \
            or (row2 + ControlScroller.nscrollvalue + 1 == \
                    len(ControlScroller.scrollfields)):

            self.insertRowAtEnd()
            '''
            if we did not change the last row but
            we did change the one before - check if we
            have two empty rows at the end.
            If so, delete the last one...
            '''
        elif (row1 + ControlScroller.nscrollvalue) + \
                (row2 + ControlScroller.nscrollvalue) \
                == (len(ControlScroller.scrollfields) * 2 - 5):
            if self.isRowEmpty(len(ControlScroller.scrollfields) - 2) \
                    and self.isRowEmpty(len(ControlScroller.scrollfields) - 1):
                self.removeLastRow()
                self.reduceDocumentToTopics()

    '''
    sets a text selection to a given control.
    This is used when one moves a row up or down.
    After moving row X to X+/-1, the selection (or cursor position) of the
    last focused control should be restored.
    The control's row is the given guiRow.
    The control's column is detecte4d according to the given event.
    This method is called as subsequent to different events,
    thus it is comfortable to use the event here to detect the column,
    rather than in the different event methods.
    @param guiRow the row of the control to set the selection to.
    @param eventSource helps to detect
    the control's column to set the selection to.
    @param s the selection object to set.
    '''

    def setSelection(self, guiRow, eventSource, s):
        cr = self.ControlGroupVector[guiRow]
        control = self.getControl(cr, eventSource)
        control.setFocus()
        control.setSelection(s)

    '''
    returns a control out of the given row, according to a column number.
    @param cr control row object.
    @param column the column number.
    @return the control...
    '''

    @classmethod
    def getControlByIndex(self, cr, column):
        tmp_switch_var1 = column
        if tmp_switch_var1 == 0:
            return cr.label
        elif tmp_switch_var1 == 1:
            return cr.textbox
        elif tmp_switch_var1 == 2:
            return cr.combobox
        elif tmp_switch_var1 == 3:
            return cr.timebox
        else:
            raise IllegalArgumentException ("No such column");

    '''getControl
    returns a control out of the given row, which is
    in the same column as the given control.
    @param cr control row object
    @param control a control indicating a column.
    @return
    '''

    @classmethod
    def getControl(self, cr, control):
        column = self.getColumn(control)
        return self.getControlByIndex(cr, column)

    '''
    returns the column number of the given control.
    @param control
    @return
    '''

    @classmethod
    def getColumn(self, control):
        name = Helper.getUnoPropertyValue(
            control.Model, PropertyNames.PROPERTY_NAME)
        if name.startswith(TopicsControl.TOPIC):
            return 1
        if name.startswith(TopicsControl.RESP):
            return 2
        if name.startswith(TopicsControl.TIME):
            return 3
        if name.startswith(TopicsControl.LABEL):
            return 0
        return -1

    '''
    update the given row in the preview document with the given data.
    @param row
    @param data
    '''

    def updateDocumentRow(self, row, data=None):
        if data is None:
            data = ControlScroller.scrollfields[row]
        try:
            ControlScroller.CurUnoDialog.agendaTemplate.topics.write(
                row, data)
        except Exception, ex:
            traceback.print_exc()

    '''
    updates a single cell in the preview document.
    Is called when a single value is changed, since we really
    don't have to update the whole row for one small changhe...
    @param row the data row to update (topic number).
    @param column the column to update (a gui column, not a document column).
    @param data the data of the entire row.
    '''

    @classmethod
    def updateDocumentCell(self, row, column, data):
        try:
            ControlScroller.CurUnoDialog.agendaTemplate.topics.writeCell(
                row, column, data)
        except Exception, ex:
            traceback.print_exc()

    '''
    when removeing rows, this method updates
    the preview document to show the number of rows
    according to the data model.
    '''

    def reduceDocumentToTopics(self):
        try:
            ControlScroller.CurUnoDialog.agendaTemplate.topics.reduceDocumentTo(
                len(ControlScroller.scrollfields) - 1)
        except Exception, ex:
            traceback.print_exc()

'''
@author rp143992
A class represting a single GUI row.
Note that the instance methods of this class
are being called and handle controls of
a single row.
'''

class ControlRow(object):

    tabIndex = 520
    '''
    constructor. Create the row in the given dialog given cordinates,
    with the given offset (row number) and tabindex.
    Note that since I use this specifically for the agenda wizard,
    the step and all control coordinates inside the
    row are constant (5).
    @param dialog the agenda dialog
    @param x x coordinates
    @param y y coordinates
    @param i the gui row index
    @param tabindex first tab index for this row.
    '''

    def __init__(self, dialog, x, y, i, tabindex):
        self.offset = i
        self.dialog = dialog
        self.label = self.dialog.insertLabel(
            TopicsControl.LABEL + str(i),
            TopicsControl.LABEL_PROPS,
            (8, "" + str(i + 1) + ".",
            x + 4, y + 2, ControlScroller.iStep, tabindex, 10))
        self.textbox = self.dialog.insertTextField(
            TopicsControl.TOPIC + str(i), "topicTextChanged",
            TopicsControl.TEXT_PROPS,
            (12, HelpIds.getHelpIdString(ControlScroller.curHelpIndex + i * 3 + 1),
            x + 15, y, ControlScroller.iStep, tabindex + 1, 84), self)
        self.combobox = self.dialog.insertTextField(
            TopicsControl.RESP + str(i), "responsibleTextChanged",
            TopicsControl.TEXT_PROPS,
            (12, HelpIds.getHelpIdString(ControlScroller.curHelpIndex + i * 3 + 2),
            x + 103, y, ControlScroller.iStep, tabindex + 2, 68), self)
        self.timebox = self.dialog.insertTextField(
            TopicsControl.TIME + str(i), "timeTextChanged",
            TopicsControl.TEXT_PROPS,
            (12, HelpIds.getHelpIdString(ControlScroller.curHelpIndex + i * 3 + 3),
            x + 175, y, ControlScroller.iStep, tabindex + 3, 20), self)
        self.setEnabled(False)
        self.textbox.addKeyListener(KeyListenerProcAdapter(self.keyPressed))
        self.combobox.addKeyListener(KeyListenerProcAdapter(self.keyPressed))
        self.timebox.addKeyListener(KeyListenerProcAdapter(self.keyPressed))
        self.textbox.addFocusListener(FocusListenerProcAdapter(
            TopicsControl.focusGained))
        self.combobox.addFocusListener(FocusListenerProcAdapter(
            TopicsControl.focusGained))
        self.timebox.addFocusListener(FocusListenerProcAdapter(
            TopicsControl.focusGained))

    def topicTextChanged(self):
        try:
            # update the data model
            ControlScroller.fieldInfo(self.offset, 1)
            # update the preview document
            TopicsControl.fieldChanged(self.offset, 1)
        except Exception:
            traceback.print_exc()

    '''
    called through an event listener when the
    responsible text is changed by the user.
    updates the data model and the preview document.
    '''

    def responsibleTextChanged(self):
        try:
            # update the data model
            ControlScroller.fieldInfo(self.offset, 2)
            # update the preview document
            TopicsControl.fieldChanged(self.offset, 2)
        except Exception:
            traceback.print_exc()

    '''
    called through an event listener when the
    time text is changed by the user.
    updates the data model and the preview document.
    '''

    def timeTextChanged(self):
        try:
            # update the data model
            ControlScroller.fieldInfo(self.offset, 3)
            # update the preview document
            TopicsControl.fieldChanged(self.offset, 3)
        except Exception:
            traceback.print_exc()

    '''
    enables/disables the row.
    @param enabled true for enable, false for disable.
    '''

    def setEnabled(self, enabled):
        self.dialog.setEnabled(self.label, enabled)
        self.dialog.setEnabled(self.textbox, enabled)
        self.dialog.setEnabled(self.combobox, enabled)
        self.dialog.setEnabled(self.timebox, enabled)

    '''
    Impelementation of XKeyListener.
    Optionally performs the one of the following:
    cursor up, or down, row up or down
    '''

    def keyPressed(self, event):
        try:
            if self.isMoveDown(event):
                TopicsControl.rowDown(self.offset, event.Source)
            elif self.isMoveUp(event):
                TopicsControl.rowUp(self.offset, event.Source)
            elif self.isDown(event):
                TopicsControl.cursorDown(self.offset, event.Source)
            elif self.isUp(event):
                TopicsControl.cursorUp(self.offset, event.Source)

            TopicsControl.enableButtons()
        except Exception:
            traceback.print_exc()

    def isMoveDown(self, e):
        return (e.KeyCode == DOWN) and (e.Modifiers == MOD1)

    def isMoveUp(self, e):
        return (e.KeyCode == UP) and (e.Modifiers == MOD1)

    def isDown(self, e):
        return (e.KeyCode == DOWN) and (e.Modifiers == 0)

    def isUp(self, e):
        return (e.KeyCode == UP) and (e.Modifiers == 0)
