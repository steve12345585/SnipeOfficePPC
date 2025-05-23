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

#ifndef SC_POSTIT_HXX
#define SC_POSTIT_HXX

#include <boost/shared_ptr.hpp>
#include <rtl/ustring.hxx>
#include <tools/gen.hxx>
#include "address.hxx"
#include "scdllapi.h"

#include <map>

class EditTextObject;
class OutlinerParaObject;
class SdrCaptionObj;
class SdrPage;
class SfxItemSet;
class ScDocument;
struct ScCaptionInitData;

// ============================================================================

/** Internal data for a cell annotation. */
struct SC_DLLPUBLIC ScNoteData
{
    typedef ::boost::shared_ptr< ScCaptionInitData > ScCaptionInitDataRef;

    ::rtl::OUString     maDate;             /// Creation date of the note.
    ::rtl::OUString     maAuthor;           /// Author of the note.
    ScCaptionInitDataRef mxInitData;        /// Initial data for invisible notes without SdrObject.
    SdrCaptionObj*      mpCaption;          /// Drawing object representing the cell note.
    bool                mbShown;            /// True = note is visible.

    explicit            ScNoteData( bool bShown = false );
                        ~ScNoteData();
};

// ============================================================================

/**
 * Additional class containing cell annotation data.
 */
class SC_DLLPUBLIC ScPostIt
{
public:
    /** Creates an empty note and its caption object and places it according to
        the passed cell position. */
    explicit            ScPostIt( ScDocument& rDoc, const ScAddress& rPos, bool bShown );

    /** Copy constructor. Clones the note and its caption to a new document. */
    explicit            ScPostIt( ScDocument& rDoc, const ScAddress& rPos, const ScPostIt& rNote );

    /** Creates a note from the passed note data with existing caption object.

        @param bAlwaysCreateCaption  Instead of a pointer to an existing
            caption object, the passed note data structure may contain a
            reference to an ScCaptionInitData structure containing information
            about how to construct a missing caption object. If sal_True is passed,
            the caption drawing object will be created immediately from that
            data. If sal_False is passed and the note is not visible, it will
            continue to cache that data until the caption object is requested.
     */
    explicit            ScPostIt(
                            ScDocument& rDoc, const ScAddress& rPos,
                            const ScNoteData& rNoteData, bool bAlwaysCreateCaption );

    /** Removes the caption object from drawing layer, if this note is its owner. */
                        ~ScPostIt();

    /** Clones this note and its caption object, if specified.

        @param bCloneCaption  If sal_True is passed, clones the caption object and
            inserts it into the drawing layer of the destination document. If
            sal_False is passed, the cloned note will refer to the old caption
            object (used e.g. in Undo documents to restore the pointer to the
            existing caption object).
     */
    ScPostIt*           Clone(
                            const ScAddress& rOwnPos,
                            ScDocument& rDestDoc, const ScAddress& rDestPos,
                            bool bCloneCaption ) const;

    /** Returns the data struct containing all note settings. */
    inline const ScNoteData& GetNoteData() const { return maNoteData; }

    /** Returns the creation date of this note. */
    inline const ::rtl::OUString& GetDate() const { return maNoteData.maDate; }
    /** Sets a new creation date for this note. */
    inline void         SetDate( const ::rtl::OUString& rDate ) { maNoteData.maDate = rDate; }

    /** Returns the author date of this note. */
    inline const ::rtl::OUString& GetAuthor() const { return maNoteData.maAuthor; }
    /** Sets a new author date for this note. */
    inline void         SetAuthor( const ::rtl::OUString& rAuthor ) { maNoteData.maAuthor = rAuthor; }

    /** Sets date and author from system settings. */
    void                AutoStamp();

    /** Returns the pointer to the current outliner object, or null. */
    const OutlinerParaObject* GetOutlinerObject() const;
    /** Returns the pointer to the current edit text object, or null. */
    const EditTextObject* GetEditTextObject() const;

    /** Returns the caption text of this note. */
    ::rtl::OUString     GetText() const;
    /** Changes the caption text of this note. All text formatting will be lost. */
    void                SetText( const ScAddress& rPos, const ::rtl::OUString& rText );

    /** Returns an existing note caption object. returns null, if the note
        contains initial caption data needed to construct a caption object. */
    inline SdrCaptionObj* GetCaption() const { return maNoteData.mpCaption; }
    /** Returns the caption object of this note. Creates the caption object, if
        the note contains initial caption data instead of the caption. */
    SdrCaptionObj*      GetOrCreateCaption( const ScAddress& rPos ) const;
    /** Forgets the pointer to the note caption object. */
    void                ForgetCaption();

    /** Shows or hides the note caption object. */
    void                ShowCaption( const ScAddress& rPos, bool bShow = true );
    /** Returns true, if the caption object is visible. */
    inline bool         IsCaptionShown() const { return maNoteData.mbShown; }

    /** Shows or hides the caption temporarily (does not change internal visibility state). */
    void                ShowCaptionTemp( const ScAddress& rPos, bool bShow = true );

    /** Updates caption position according to position of the passed cell. */
    void                UpdateCaptionPos( const ScAddress& rPos );

private:
                        ScPostIt( const ScPostIt& );
    ScPostIt&           operator=( const ScPostIt& );

    /** Creates the caption object from initial caption data if existing. */
    void                CreateCaptionFromInitData( const ScAddress& rPos ) const;
    /** Creates a new caption object at the passed cell position, clones passed existing caption. */
    void                CreateCaption( const ScAddress& rPos, const SdrCaptionObj* pCaption = 0 );
    /** Removes the caption object from the drawing layer, if this note is its owner. */
    void                RemoveCaption();

private:
    ScDocument&         mrDoc;              /// Parent document containing the note.
    mutable ScNoteData  maNoteData;         /// Note data with pointer to caption object.
};

// ============================================================================

class SC_DLLPUBLIC ScNoteUtil
{
public:
    /** Tries to update the position of note caption objects in the specified range. */
    static void         UpdateCaptionPositions( ScDocument& rDoc, const ScRange& rRange );

    /** Creates and returns a caption object for a temporary caption. */
    static SdrCaptionObj* CreateTempCaption( ScDocument& rDoc, const ScAddress& rPos,
                            SdrPage& rDrawPage, const ::rtl::OUString& rUserText,
                            const Rectangle& rVisRect, bool bTailFront );

    /** Creates a cell note using the passed caption drawing object.

        This function is used in import filters to reuse the imported drawing
        object as note caption object.

        @param rCaption  The drawing object for the cell note. This object MUST
            be inserted into the document at the correct drawing page already.

        @return  Pointer to the new cell note object if insertion was
            successful (i.e. the passed cell position was valid), null
            otherwise. The Calc document is the owner of the note object. The
            passed item set and outliner object are deleted automatically if
            creation of the note was not successful.
     */
    static ScPostIt*    CreateNoteFromCaption(
                            ScDocument& rDoc, const ScAddress& rPos,
                            SdrCaptionObj& rCaption, bool bShown );

    /** Creates a cell note based on the passed caption object data.

        This function is used in import filters to use an existing imported
        item set and outliner object to create a note caption object. For
        performance reasons, it is possible to specify that the caption drawing
        object for the cell note is not created yet but the note caches the
        passed data needed to create the caption object on demand (see
        parameter bAlwaysCreateCaption).

        @param pItemSet  Pointer to an item set on heap memory containing all
            formatting attributes of the caption object. This function takes
            ownership of the passed item set.

        @param pOutlinerObj  Pointer to an outliner object on heap memory
            containing (formatted) text for the caption object. This function
            takes ownership of the passed outliner object.

        @param rCaptionRect  The absolute position and size of the caption
            object. The rectangle may be empty, in this case the default
            position and size is used.

        @param bAlwaysCreateCaption  If sal_True is passed, the caption drawing
            object will be created immediately. If sal_False is passed, the caption
            drawing object will not be created if the note is not visible
            (bShown = sal_False), but the cell note will cache the passed data.
            MUST be set to sal_False outside of import filter implementations!

        @return  Pointer to the new cell note object if insertion was
            successful (i.e. the passed cell position was valid), null
            otherwise. The Calc document is the owner of the note object.
     */
    static ScPostIt*    CreateNoteFromObjectData(
                            ScDocument& rDoc, const ScAddress& rPos,
                            SfxItemSet* pItemSet, OutlinerParaObject* pOutlinerObj,
                            const Rectangle& rCaptionRect, bool bShown,
                            bool bAlwaysCreateCaption );

    /** Creates a cell note based on the passed string and inserts it into the
        document.

        @param rNoteText  The text used to create the note caption object. Must
            not be empty.

        @param bAlwaysCreateCaption  If sal_True is passed, the caption drawing
            object will be created immediately. If sal_False is passed, the caption
            drawing object will not be created if the note is not visible
            (bShown = sal_False), but the cell note will cache the passed data.
            MUST be set to sal_False outside of import filter implementations!

        @return  Pointer to the new cell note object if insertion was
            successful (i.e. the passed cell position was valid), null
            otherwise. The Calc document is the owner of the note object.
     */
    static ScPostIt*    CreateNoteFromString(
                            ScDocument& rDoc, const ScAddress& rPos,
                            const ::rtl::OUString& rNoteText, bool bShown,
                            bool bAlwaysCreateCaption );
};

class SC_DLLPUBLIC ScNotes
{
private:
    typedef std::pair<SCCOL, SCROW> ScAddress2D;
    typedef std::map<ScAddress2D, ScPostIt*> ScNoteMap;
    ScNoteMap maNoteMap;

    ScNotes(const ScNotes& rNotes);
    ScNotes operator=(const ScNotes& rNotes);
    ScDocument* mpDoc;
public:
    ScNotes(ScDocument* pDoc);
    ~ScNotes();

    typedef ScNoteMap::iterator iterator;
    typedef ScNoteMap::const_iterator const_iterator;

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    size_t size() const;
    bool empty() const;

    ScPostIt* findByAddress(SCCOL nCol, SCROW nRow);
    const ScPostIt* findByAddress(SCCOL nCol, SCROW nRow) const;

    ScPostIt* findByAddress(const ScAddress& rAddress);
    const ScPostIt* findByAddress(const ScAddress& rAddress) const;
    /**
     * takes ownership of the
     */
    bool insert( SCCOL nCol, SCROW nRow, ScPostIt* );
    bool insert( const ScAddress& rPos, ScPostIt* );

    void erase(SCCOL, SCROW, bool bForgetCaption = false);
    void erase(const ScAddress& rPos);

    /** Returns and forgets the cell note object at the passed cell address. */
    ScPostIt*       ReleaseNote( const ScAddress& rPos );
    ScPostIt*       ReleaseNote( SCCOL nCol, SCROW nRow );
    /** Returns the pointer to an existing or created cell note object at the passed cell address. */
    ScPostIt* GetOrCreateNote( const ScAddress& rPos );

    void clear();

    void clone(ScDocument* pDoc, SCCOL nCol1, SCROW nRow1, SCCOL nCol2, SCROW nRow2, bool bCloneNoteCaption, SCTAB nTab, ScNotes& rTarget);
    void CopyFromClip(const ScNotes& maNotes, ScDocument* pDoc, SCCOL nCol1, SCROW nRow1, SCCOL nCol2, SCROW nRow2, SCsCOL nDx, SCsROW nDy, SCTAB nTab, bool bCloneCaption);

    void erase(SCCOL nCol1, SCROW nRow1, SCCOL nCol2, SCROW nRow2, bool bForgetCaption = false);

    void CreateAllNoteCaptions(SCTAB nTab);
};

// ============================================================================

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
