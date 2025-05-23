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

#ifndef IDOCUMENTMARKACCESS_HXX_INCLUDED
#define IDOCUMENTMARKACCESS_HXX_INCLUDED

#include <sal/types.h>
#include <IMark.hxx>
#include <boost/shared_ptr.hpp>

class SwPaM;
class KeyCode;
class String;
struct SwPosition;
class SwTxtNode;

namespace sw { namespace mark {
    class SaveBookmark; // FIXME: Ugly: SaveBookmark is a core-internal class, and should not be used in the interface
}}

/** Provides access to the marks of a document.
*/
class IDocumentMarkAccess
{
    public:
        enum MarkType
        {
            UNO_BOOKMARK,
            DDE_BOOKMARK,
            BOOKMARK,
            CROSSREF_HEADING_BOOKMARK,
            CROSSREF_NUMITEM_BOOKMARK,
            TEXT_FIELDMARK,
            CHECKBOX_FIELDMARK,
            NAVIGATOR_REMINDER
        };

        typedef ::boost::shared_ptr< ::sw::mark::IMark> pMark_t;
        typedef ::std::vector< pMark_t > container_t;
        typedef container_t::iterator iterator_t;
        typedef container_t::const_iterator const_iterator_t;
        typedef container_t::const_reverse_iterator const_reverse_iterator_t;

        /// To avoid recursive calls of deleteMark, the removal of dummy
        /// characters of fieldmarks has to be delayed; this is the baseclass
        /// that can be subclassed for that purpose.
        struct ILazyDeleter { virtual ~ILazyDeleter() { } };

        /** Generates a new mark in the document for a certain selection.

           @param rPaM
           [in] the selection being marked.

           @param rProposedName
           [in] the proposed name of the new mark.

           @param eMark
           [in] the type of the new mark.

           @returns
           a pointer to the new mark (name might have changed).
        */
        virtual ::sw::mark::IMark* makeMark(const SwPaM& rPaM,
            const ::rtl::OUString& rProposedName,
            MarkType eMark) =0;

        virtual sw::mark::IFieldmark* makeFieldBookmark( const SwPaM& rPaM,
            const rtl::OUString& rName,
            const rtl::OUString& rType) = 0;
        virtual sw::mark::IFieldmark* makeNoTextFieldBookmark( const SwPaM& rPaM,
            const rtl::OUString& rName,
            const rtl::OUString& rType) = 0;

        /** Returns a mark in the document for a paragraph.
            If there is none, a mark will be created.

           @param rTxtNode
           [in] the paragraph being marked (a selection over the paragraph is marked)

           @param eMark
           [in] the type of the new mark.

           @returns
           a pointer to the new mark (name might have changed).
        */
        virtual ::sw::mark::IMark* getMarkForTxtNode(const SwTxtNode& rTxtNode,
            MarkType eMark) =0;

        /** Moves an existing mark to a new selection and performs needed updates.
            @param io_pMark
            [in/out] the mark to be moved

            @param rPaM
            [in] new selection to be marked
        */

        virtual void repositionMark(::sw::mark::IMark* io_pMark,
            const SwPaM& rPaM) =0;

        /** Renames an existing Mark, if possible.
            @param io_pMark
            [in/out] the mark to be renamed

            @param rNewName
            [in] new name for the mark

            @returns false, if renaming failed (because the name is already in use)
        */
            virtual bool renameMark(::sw::mark::IMark* io_pMark,
                const ::rtl::OUString& rNewName) =0;

        /** Corrects marks (absolute)
            This method ignores the previous position of the mark in the paragraph

            @param rOldNode
            [in] the node from which nodes should be moved

            @param rNewPos
            [in] new position to which marks will be moved, if nOffset == 0

            @param nOffset
            [in] the offset by which the mark gets positioned of rNewPos
        */
        virtual void correctMarksAbsolute(const SwNodeIndex& rOldNode,
            const SwPosition& rNewPos,
            const xub_StrLen nOffset) =0;

        /** Corrects marks (relative)
            This method uses the previous position of the mark in the paragraph as offset

            @param rOldNode
            [in] the node from which nodes should be moved

            @param rNewPos
            [in] new position to which marks from the start of the paragraph will be
                 moved, if nOffset == 0

            @param nOffset
            [in] the offset by which the mark gets positioned of rNewPos in addition to
                 its old position in the paragraph
        */
        virtual void correctMarksRelative(const SwNodeIndex& rOldNode,
            const SwPosition& rNewPos,
            const xub_StrLen nOffset) =0;

        /** Deletes marks in a range
        */
        virtual void deleteMarks(
            const SwNodeIndex& rStt,
            const SwNodeIndex& rEnd,
            ::std::vector< ::sw::mark::SaveBookmark>* pSaveBkmk, // Ugly: SaveBookmark is core-internal
            const SwIndex* pSttIdx,
            const SwIndex* pEndIdx) =0;

        /** Deletes a mark.

            @param ppMark
            [in] an iterator pointing to the Mark to be deleted.
        */
        virtual ::boost::shared_ptr<ILazyDeleter>
            deleteMark(const IDocumentMarkAccess::const_iterator_t ppMark) =0;

        /** Deletes a mark.

            @param ppMark
            [in] the name of the mark to be deleted.
        */
        virtual void deleteMark(const ::sw::mark::IMark* const pMark) =0;

        /** Clear (deletes) all marks.
        */
        virtual void clearAllMarks() =0;

        /** returns a STL-like random access iterator to the begin of the sequence of marks.
        */
        virtual const_iterator_t getMarksBegin() const =0;

        /** returns a STL-like random access iterator to the end of the sequence of marks.
        */
        virtual const_iterator_t getMarksEnd() const =0;

        /** returns the number of marks.
        */
        virtual sal_Int32 getMarksCount() const =0;

        /** Finds a mark by name.

            @param rName
            [in] the name of the mark to find.

            @returns
            an iterator pointing to the mark, or pointing to getMarksEnd() if nothing was found.
        */
        virtual const_iterator_t findMark(const ::rtl::OUString& rMark) const =0;


        // interface IBookmarks (BOOKMARK, CROSSREF_NUMITEM_BOOKMARK, CROSSREF_HEADING_BOOKMARK)

        /** returns a STL-like random access iterator to the begin of the sequence the IBookmarks.
        */
        virtual const_iterator_t getBookmarksBegin() const =0;

        /** returns a STL-like random access iterator to the end of the sequence of IBookmarks.
        */
        virtual const_iterator_t getBookmarksEnd() const =0;

        /** returns the number of IBookmarks.
        */
        virtual sal_Int32 getBookmarksCount() const =0;

        /** Finds a bookmark by name.

            @param rName
            [in] the name of the bookmark to find.

            @returns
            an iterator pointing to the bookmark, or getBookmarksEnd() if nothing was found.
        */
        virtual const_iterator_t findBookmark(const ::rtl::OUString& rMark) const =0;


        // Fieldmarks
        virtual ::sw::mark::IFieldmark* getFieldmarkFor(const SwPosition& pos) const =0;
        virtual ::sw::mark::IFieldmark* getFieldmarkBefore(const SwPosition& pos) const =0;
        virtual ::sw::mark::IFieldmark* getFieldmarkAfter(const SwPosition& pos) const =0;

        /** Returns the MarkType used to create the mark
        */
        static MarkType SAL_DLLPUBLIC_EXPORT GetType(const ::sw::mark::IMark& rMark);
    protected:
        virtual ~IDocumentMarkAccess() {};
};

#endif // IDOCUMENTBOOKMARKACCESS_HXX_INCLUDED

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
