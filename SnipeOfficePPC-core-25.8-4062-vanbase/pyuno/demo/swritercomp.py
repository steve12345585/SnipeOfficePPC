# -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file incorporates work covered by the following license notice:
#
#   Licensed to the Apache Software Foundation (ASF) under one or more
#   contributor license agreements. See the NOTICE file distributed
#   with this work for additional information regarding copyright
#   ownership. The ASF licenses this file to you under the Apache
#   License, Version 2.0 (the "License"); you may not use this file
#   except in compliance with the License. You may obtain a copy of
#   the License at http://www.apache.org/licenses/LICENSE-2.0 .
#

# just a simple copy of the swriter.py demo, but implemented as a component. The advantage is,
# that the component may run within the office process which may give a performance improvement.

import unohelper
import uno

# a UNO struct later needed to create a document
from com.sun.star.text.ControlCharacter import PARAGRAPH_BREAK
from com.sun.star.text.TextContentAnchorType import AS_CHARACTER
from com.sun.star.awt import Size

from com.sun.star.lang import XMain

def insertTextIntoCell( table, cellName, text, color ):
    tableText = table.getCellByName( cellName )
    cursor = tableText.createTextCursor()
    cursor.setPropertyValue( "CharColor", color )
    tableText.setString( text )

# the UNO component
# implementing the interface com.sun.star.lang.XMain
# unohelper.Base implements the XTypeProvider interface
class SWriterComp(XMain,unohelper.Base):
    def __init__( self, ctx ):
        self.ctx = ctx

    # implementation for XMain.run( [in] sequence< any > )
    def run( self,args ):
        ctx = self.ctx
        smgr = ctx.ServiceManager
        desktop = smgr.createInstanceWithContext( "com.sun.star.frame.Desktop",ctx)

        # open a writer document
        doc = desktop.loadComponentFromURL( "private:factory/swriter","_blank", 0, () )

        text = doc.Text
        cursor = text.createTextCursor()
        text.insertString( cursor, "The first line in the newly created text document.\n", 0 )
        text.insertString( cursor, "Now we are in the second line\n" , 0 )

        # create a text table
        table = doc.createInstance( "com.sun.star.text.TextTable" )

        # with 4 rows and 4 columns
        table.initialize( 4,4)

        text.insertTextContent( cursor, table, 0 )
        rows = table.Rows

        table.setPropertyValue( "BackTransparent", uno.Bool(0) )
        table.setPropertyValue( "BackColor", 13421823 )
        row = rows.getByIndex(0)
        row.setPropertyValue( "BackTransparent", uno.Bool(0) )
        row.setPropertyValue( "BackColor", 6710932 )

        textColor = 16777215

        insertTextIntoCell( table, "A1", "FirstColumn", textColor )
        insertTextIntoCell( table, "B1", "SecondColumn", textColor )
        insertTextIntoCell( table, "C1", "ThirdColumn", textColor )
        insertTextIntoCell( table, "D1", "SUM", textColor )

        table.getCellByName("A2").setValue(22.5)
        table.getCellByName("B2").setValue(5615.3)
        table.getCellByName("C2").setValue(-2315.7)
        table.getCellByName("D2").setFormula("sum <A2:C2>")

        table.getCellByName("A3").setValue(21.5)
        table.getCellByName("B3").setValue(615.3)
        table.getCellByName("C3").setValue(-315.7)
        table.getCellByName("D3").setFormula("sum <A3:C3>")

        table.getCellByName("A4").setValue(121.5)
        table.getCellByName("B4").setValue(-615.3)
        table.getCellByName("C4").setValue(415.7)
        table.getCellByName("D4").setFormula("sum <A4:C4>")


        cursor.setPropertyValue( "CharColor", 255 )
        cursor.setPropertyValue( "CharShadowed", uno.Bool(1) )

        text.insertControlCharacter( cursor, PARAGRAPH_BREAK, 0 )
        text.insertString( cursor, " This is a colored Text - blue with shadow\n" , 0 )
        text.insertControlCharacter( cursor, PARAGRAPH_BREAK, 0 )

        textFrame = doc.createInstance( "com.sun.star.text.TextFrame" )
        textFrame.setSize( Size(15000,400))
        textFrame.setPropertyValue( "AnchorType" , AS_CHARACTER )

        text.insertTextContent( cursor, textFrame, 0 )

        textInTextFrame = textFrame.getText()
        cursorInTextFrame = textInTextFrame.createTextCursor()
        textInTextFrame.insertString( cursorInTextFrame, "The first line in the newly created text frame.", 0 )
        textInTextFrame.insertString( cursorInTextFrame, "\nWith this second line the height of the rame raises.",0)
        text.insertControlCharacter( cursor, PARAGRAPH_BREAK, 0 )

        cursor.setPropertyValue( "CharColor", 65536 )
        cursor.setPropertyValue( "CharShadowed", uno.Bool(0) )

        text.insertString( cursor, " That's all for now!" , 0 )
        return 0

# pythonloader looks for a static g_ImplementationHelper variable
g_ImplementationHelper = unohelper.ImplementationHelper()
g_ImplementationHelper.addImplementation( \
    SWriterComp,"org.openoffice.comp.pyuno.swriter",("org.openoffice.demo.SWriter",),)

# vim:set shiftwidth=4 softtabstop=4 expandtab:
