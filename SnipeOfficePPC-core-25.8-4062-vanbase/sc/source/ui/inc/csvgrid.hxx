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

// ============================================================================

#ifndef _SC_CSVGRID_HXX
#define _SC_CSVGRID_HXX

#include <vcl/virdev.hxx>
#include <vcl/menu.hxx>
#include <unotools/options.hxx>

#include <vector>
#include <memory>
#include "scdllapi.h"
#include "csvcontrol.hxx"
#include "csvsplits.hxx"


// ----------------------------------------------------------------------------

namespace svtools { class ColorConfig; }
class EditEngine;
class ScEditEngineDefaulter;
class ScAsciiOptions;
class ScAccessibleCsvControl;


// ============================================================================

const sal_uInt8 CSV_COLFLAG_NONE    = 0x00;         /// Nothing set.
const sal_uInt8 CSV_COLFLAG_SELECT  = 0x01;         /// Column is selected.

const sal_uInt32 CSV_COLUMN_INVALID = CSV_VEC_NOTFOUND;


// ----------------------------------------------------------------------------

/** This struct contains the state of one table column. */
struct ScCsvColState
{
    sal_Int32                   mnType;             /// Data type.
    sal_uInt8                   mnFlags;            /// Flags (i.e. selection state).

    inline explicit             ScCsvColState(
                                        sal_Int32 nType = CSV_TYPE_DEFAULT,
                                        sal_uInt8 nFlags = CSV_COLFLAG_NONE ) :
                                    mnType( nType ), mnFlags( nFlags ) {}

    inline bool                 IsSelected() const;
    inline void                 Select( bool bSel );
};

inline bool ScCsvColState::IsSelected() const
{
    return (mnFlags & CSV_COLFLAG_SELECT) != 0;
}

inline void ScCsvColState::Select( bool bSel )
{
    if( bSel ) mnFlags |= CSV_COLFLAG_SELECT; else mnFlags &= ~CSV_COLFLAG_SELECT;
}


// ----------------------------------------------------------------------------

typedef ::std::vector< ScCsvColState > ScCsvColStateVec;


// ============================================================================

/** A data grid control for the CSV import dialog. The design of this control
    simulates a Calc spreadsheet with row and column headers. */
class SC_DLLPUBLIC ScCsvGrid : public ScCsvControl, public utl::ConfigurationListener
{
private:
    typedef ::std::auto_ptr< ScEditEngineDefaulter > ScEditEnginePtr;

    VirtualDevice               maBackgrDev;        /// Grid background, headers, cell texts.
    VirtualDevice               maGridDev;          /// Data grid with selection and cursor.
    PopupMenu                   maPopup;            /// Popup menu for column types.

    ::svtools::ColorConfig*     mpColorConfig;      /// Application color configuration.
    Color                       maBackColor;        /// Cell background color.
    Color                       maGridColor;        /// Table grid color.
    Color                       maGridPBColor;      /// Grid color for "first imported line" delimiter.
    Color                       maAppBackColor;     /// Background color for unused area.
    Color                       maTextColor;        /// Text color for data area.
    Color                       maHeaderBackColor;  /// Background color for headers.
    Color                       maHeaderGridColor;  /// Grid color for headers.
    Color                       maHeaderTextColor;  /// Text color for headers.
    Color                       maSelectColor;      /// Header color of selected columns.

    ScEditEnginePtr             mpEditEngine;       /// For drawing cell texts.
    Font                        maHeaderFont;       /// Font for column and row headers.
    Font                        maMonoFont;         /// Monospace font for data cells.
    Size                        maWinSize;          /// Size of the control.
    Size                        maEdEngSize;        /// Paper size for edit engine.

    ScCsvSplits                 maSplits;           /// Vector with split positions.
    ScCsvColStateVec            maColStates;        /// State of each column.
    StringVec                   maTypeNames;        /// UI names of data types.
    StringVecVec                maTexts;            /// 2D-vector for cell texts.

    sal_Int32                   mnFirstImpLine;     /// First imported line (0-based).
    sal_uInt32                  mnRecentSelCol;     /// Index of most recently selected column.
    sal_uInt32                  mnMTCurrCol;        /// Current column of mouse tracking.
    bool                        mbMTSelecting;      /// Mouse tracking: true = select, false = deselect.

    // ------------------------------------------------------------------------
public:
    explicit                    ScCsvGrid( ScCsvControl& rParent );
    virtual                     ~ScCsvGrid();

    /** Finishes initialization. Must be called after constructing a new object. */
    void Init();

    // common grid handling ---------------------------------------------------
public:
    /** Updates layout data dependent from the control's state. */
    void                        UpdateLayoutData();
    /** Updates X coordinate of first visible position dependent from line numbers. */
    void                        UpdateOffsetX();
    /** Apply current layout data to the grid control. */
    void                        ApplyLayout( const ScCsvLayoutData& rOldData );
    /** Sets the number of the first imported line (for visual feedback). nLine is 0-based! */
    void                        SetFirstImportedLine( sal_Int32 nLine );

    /** Finds a column position nearest to nPos which does not cause scrolling the visible area. */
    sal_Int32                   GetNoScrollCol( sal_Int32 nPos ) const;

private:
    /** Reads colors from system settings. */
    SC_DLLPRIVATE void                        InitColors();
    /** Initializes all font settings. */
    SC_DLLPRIVATE void                        InitFonts();
    /** Initializes all data dependent from the control's size. */
    SC_DLLPRIVATE void                        InitSizeData();

    // split handling ---------------------------------------------------------
public:
    /** Inserts a split. */
    void                        InsertSplit( sal_Int32 nPos );
    /** Removes a split. */
    void                        RemoveSplit( sal_Int32 nPos );
    /** Inserts a new or removes an existing split. */
    void                        MoveSplit( sal_Int32 nPos, sal_Int32 nNewPos );
    /** Removes all splits. */
    void                        RemoveAllSplits();
    /** Removes all splits and inserts the splits from rSplits. */
    void                        SetSplits( const ScCsvSplits& rSplits );

private:
    /** Inserts a split and adjusts column data. */
    SC_DLLPRIVATE bool                        ImplInsertSplit( sal_Int32 nPos );
    /** Removes a split and adjusts column data. */
    SC_DLLPRIVATE bool                        ImplRemoveSplit( sal_Int32 nPos );
    /** Clears the split array and re-inserts boundary splits. */
    SC_DLLPRIVATE void                        ImplClearSplits();

    // columns/column types ---------------------------------------------------
public:
    /** Returns the number of columns. */
    inline sal_uInt32           GetColumnCount() const { return maColStates.size(); }
    /** Returns the index of the first visible column. */
    sal_uInt32                  GetFirstVisColumn() const;
    /** Returns the index of the last visible column. */
    sal_uInt32                  GetLastVisColumn() const;

    /** Returns true, if nColIndex points to an existing column. */
    bool                        IsValidColumn( sal_uInt32 nColIndex ) const;
    /** Returns true, if column with index nColIndex is (at least partly) visible. */
    bool                        IsVisibleColumn( sal_uInt32 nColIndex ) const;

    /** Returns X coordinate of the specified column. */
    sal_Int32                   GetColumnX( sal_uInt32 nColIndex ) const;
    /** Returns column index from output coordinate. */
    sal_uInt32                  GetColumnFromX( sal_Int32 nX ) const;

    /** Returns start position of the column with the specified index. */
    inline sal_Int32            GetColumnPos( sal_uInt32 nColIndex ) const { return maSplits[ nColIndex ]; }
    /** Returns column index from position. A split counts to its following column. */
    sal_uInt32                  GetColumnFromPos( sal_Int32 nPos ) const;
    /** Returns the character width of the column with the specified index. */
    sal_Int32                   GetColumnWidth( sal_uInt32 nColIndex ) const;

    /** Returns the vector with the states of all columns. */
    inline const ScCsvColStateVec& GetColumnStates() const { return maColStates; }
    /** Sets all column states to the values in the passed vector. */
    void                        SetColumnStates( const ScCsvColStateVec& rColStates );
    /** Returns the data type of the selected columns. */
    sal_Int32                   GetSelColumnType() const;
    /** Changes the data type of all selected columns. */
    void                        SetSelColumnType( sal_Int32 nType );
    /** Sets new UI data type names. */
    void                        SetTypeNames( const StringVec& rTypeNames );
    /** Returns the UI type name of the specified column. */
    const String&               GetColumnTypeName( sal_uInt32 nColIndex ) const;

    /** Fills the options object with column data for separators mode. */
    void                        FillColumnDataSep( ScAsciiOptions& rOptions ) const;
    /** Fills the options object with column data for fixed width mode. */
    void                        FillColumnDataFix( ScAsciiOptions& rOptions ) const;

private:
    /** Returns the data type of the specified column. */
    SC_DLLPRIVATE sal_Int32                   GetColumnType( sal_uInt32 nColIndex ) const;
    /** Returns the data type of the specified column. */
    SC_DLLPRIVATE void                        SetColumnType( sal_uInt32 nColIndex, sal_Int32 nColType );

    /** Scrolls data grid vertically. */
    SC_DLLPRIVATE void                        ScrollVertRel( ScMoveMode eDir );
    /** Executes the data type popup menu. */
    SC_DLLPRIVATE void                        ExecutePopup( const Point& rPos );

    // selection handling -----------------------------------------------------
public:
    /** Returns true, if the specified column is selected. */
    bool                        IsSelected( sal_uInt32 nColIndex ) const;
    /** Returns index of the first selected column. */
    sal_uInt32                  GetFirstSelected() const;
    /** Returns index of the first selected column really after nFromIndex. */
    sal_uInt32                  GetNextSelected( sal_uInt32 nFromIndex ) const;
    /** Returns true, if at least one column is selected. */
    inline bool                 HasSelection() const { return GetFirstSelected() != CSV_COLUMN_INVALID; }

    /** Selects or deselects the specified column. */
    void                        Select( sal_uInt32 nColIndex, bool bSelect = true );
    /** Toggles selection of the specified column. */
    void                        ToggleSelect( sal_uInt32 nColIndex );
    /** Selects or deselects the specified column range. */
    void                        SelectRange( sal_uInt32 nColIndex1, sal_uInt32 nColIndex2, bool bSelect = true );
    /** Selects or deselects all columns. */
    void                        SelectAll( bool bSelect = true );

    /** Returns index of the focused column. */
    inline sal_uInt32           GetFocusColumn() const { return GetColumnFromPos( GetGridCursorPos() ); }

private:
    /** Moves column cursor to a new position. */
    SC_DLLPRIVATE void                        MoveCursor( sal_uInt32 nColIndex );
    /** Moves column cursor to the given direction. */
    SC_DLLPRIVATE void                        MoveCursorRel( ScMoveMode eDir );

    /** Clears the entire selection without notify. */
    SC_DLLPRIVATE void                        ImplClearSelection();

    /** Executes selection action for a specific column. */
    SC_DLLPRIVATE void                        DoSelectAction( sal_uInt32 nColIndex, sal_uInt16 nModifier );

    // cell contents ----------------------------------------------------------
public:
    /** Fills all cells of a line with the passed text (separators mode). */
    void                        ImplSetTextLineSep(
                                    sal_Int32 nLine, const rtl::OUString& rTextLine,
                                    const String& rSepChars, sal_Unicode cTextSep, bool bMergeSep );
    /** Fills all cells of a line with the passed text (fixed width mode). */
    void                        ImplSetTextLineFix( sal_Int32 nLine, const rtl::OUString& rTextLine );

    /** Returns the text of the specified cell. */
    const String&               GetCellText( sal_uInt32 nColIndex, sal_Int32 nLine ) const;

    // event handling ---------------------------------------------------------
protected:
    virtual void                Resize();
    virtual void                GetFocus();
    virtual void                LoseFocus();

    virtual void                MouseButtonDown( const MouseEvent& rMEvt );
    virtual void                Tracking( const TrackingEvent& rTEvt );
    virtual void                KeyInput( const KeyEvent& rKEvt );
    virtual void                Command( const CommandEvent& rCEvt );

    virtual void                DataChanged( const DataChangedEvent& rDCEvt );

    virtual void                ConfigurationChanged( ::utl::ConfigurationBroadcaster*, sal_uInt32 );

    // painting ---------------------------------------------------------------
protected:
    virtual void                Paint( const Rectangle& );

public:
    /** Redraws the entire data grid. */
    void                        ImplRedraw();
    /** Returns a pointer to the used edit engine. */
    EditEngine*                 GetEditEngine();

private:
    /** Returns the width of the control. */
    inline sal_Int32            GetWidth() const { return maWinSize.Width(); }
    /** Returns the height of the control. */
    inline sal_Int32            GetHeight() const { return maWinSize.Height(); }

    /** Sets a clip region in the specified output device for the specified column. */
    SC_DLLPRIVATE void                        ImplSetColumnClipRegion( OutputDevice& rOutDev, sal_uInt32 nColIndex );
    /** Draws the header of the specified column to the specified output device. */
    SC_DLLPRIVATE void                        ImplDrawColumnHeader( OutputDevice& rOutDev, sal_uInt32 nColIndex, Color aFillColor );

    /** Draws the text at the specified position to maBackgrDev. */
    SC_DLLPRIVATE void                        ImplDrawCellText( const Point& rPos, const String& rText );
    /** Draws the "first imported line" separator to maBackgrDev (or erases, if bSet is false). */
    SC_DLLPRIVATE void                        ImplDrawFirstLineSep( bool bSet );
    /** Draws the column with index nColIndex to maBackgrDev. */
    SC_DLLPRIVATE void                        ImplDrawColumnBackgr( sal_uInt32 nColIndex );
    /** Draws the row headers column to maBackgrDev. */
    SC_DLLPRIVATE void                        ImplDrawRowHeaders();
    /** Draws all columns and the row headers column to maBackgrDev. */
    SC_DLLPRIVATE void                        ImplDrawBackgrDev();

    /** Draws the column with index nColIndex with its selection state to maGridDev. */
    SC_DLLPRIVATE void                        ImplDrawColumnSelection( sal_uInt32 nColIndex );
    /** Draws all columns with selection and cursor to maGridDev. */
    SC_DLLPRIVATE void                        ImplDrawGridDev();

    /** Redraws the entire column (background and selection). */
    SC_DLLPRIVATE void                        ImplDrawColumn( sal_uInt32 nColIndex );

    /** Optimized drawing: Scrolls horizontally and redraws only missing parts. */
    SC_DLLPRIVATE void                        ImplDrawHorzScrolled( sal_Int32 nOldPos );

    /** Inverts the cursor bar at the specified position in maGridDev. */
    SC_DLLPRIVATE void                        ImplInvertCursor( sal_Int32 nPos );

    /** Draws directly tracking rectangle to the column with the specified index. */
    SC_DLLPRIVATE void                        ImplDrawTrackingRect( sal_uInt32 nColIndex );

    // accessibility ----------------------------------------------------------
protected:
    /** Creates a new accessible object. */
    virtual ScAccessibleCsvControl* ImplCreateAccessible();
};


// ============================================================================

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
