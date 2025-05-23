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

#ifndef SW_WRITERHELPER
#define SW_WRITERHELPER

#include <typeinfo>
#include <vector>
#include <map>
#include <com/sun/star/embed/XEmbeddedObject.hpp>

#include <sfx2/objsh.hxx>
#include "types.hxx"
#include <svl/itempool.hxx>     //SfxItemPool
#include <svl/itemset.hxx>      //SfxItemSet
#include <format.hxx>               //SwFmt
#include <node.hxx>                 //SwCntntNode
#include <pam.hxx>                  //SwPaM
#include <tools/poly.hxx>           //Polygon, PolyPolygon
#include <doc.hxx>                  //SwDoc

class SwTxtFmtColl;
class SwCharFmt;
class SdrObject;
class SdrOle2Obj;
class OutlinerParaObject;
class SwNumFmt;
class SwTxtNode;
class SwNoTxtNode;
class SwFmtCharFmt;
class Graphic;
class SwDoc;
class SwNumRule;

namespace sw
{
    namespace util
    {
        class ItemSort
            : public std::binary_function<sal_uInt16, sal_uInt16, bool>
        {
        public:
            bool operator()(sal_uInt16 nA, sal_uInt16 nB) const;
        };
    }
}

namespace sw
{
    /// STL container of Paragraph Styles (SwTxtFmtColl)
    typedef std::vector<SwTxtFmtColl *> ParaStyles;
    /// STL iterator for ParaStyles
    typedef ParaStyles::iterator ParaStyleIter;
    /// STL container of SfxPoolItems (Attributes)
    typedef std::map<sal_uInt16, const SfxPoolItem *, sw::util::ItemSort> PoolItems;
    /// STL const iterator for ParaStyles
    typedef PoolItems::const_iterator cPoolItemIter;


    /** Make exporting a Writer Frame easy

        In word all frames are effectively anchored to character or as
        character. This is nice and simple, writer is massively complex in this
        area, so this sw::Frame simplies matters by providing a single unified
        view of the multitute of elements in writer and their differing quirks.

        A sw::Frame wraps a writer frame and is guaranted to have a suitable
        anchor position available from it. It hides much of the needless
        complexity of the multitude of floating/inline elements in writer, it...

        Guarantees an anchor position for a frame.
        Provides a readable way to see if we are anchored inline. (as character)
        Provides a simple way to flag what type of entity this frame describes.
        Provides the size of the element as drawn by writer.

        @author
        <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
    */
    class Frame
    {
    public:
        enum WriterSource {eTxtBox, eGraphic, eOle, eDrawing, eFormControl,eBulletGrf};//For i120928,add Grf Bul Type
    private:
        const SwFrmFmt* mpFlyFrm;
        SwPosition maPos;
        Size maSize;
        // #i43447# - Size of the frame in the layout.
        // Especially needed for graphics, whose layout size can differ from its
        // size, because it is scaled into its environment.
        Size maLayoutSize;

        WriterSource meWriterType;
        const SwNode *mpStartFrameContent;
        bool mbIsInline;
        bool mbForBullet:1;
        Graphic maGrf;
    public:
        Frame(const SwFrmFmt &rFlyFrm, const SwPosition &rPos);
    Frame(const Graphic&, const SwPosition &);
        /** Get the writer SwFrmFmt that this object describes

            @return
            The wrapped SwFrmFmt
        */
        const SwFrmFmt &GetFrmFmt() const { return *mpFlyFrm; }

        /** Get the position this frame is anchored at

            @return
            The anchor position of this frame
        */
        const SwPosition &GetPosition() const { return maPos; }

        /** Get the node this frame is anchored into

            @return
            The SwTxtNode this frame is anchored inside
        */
        const SwCntntNode *GetCntntNode() const
            { return maPos.nNode.GetNode().GetCntntNode(); }

        /** Get the type of frame that this wraps

            @return
            a WriterSource which describes the source type of this wrapper
        */
        WriterSource GetWriterType() const { return meWriterType; }

        /** Is this frame inline (as character)

            @return
            whether this is inline or not
        */
        bool IsInline() const;


        /** Even if the frame isn't an inline frame, force it to behave as one

            There are a variety of circumstances where word cannot have
            anything except inline elements, e.g. inside frames. So its easier
            to force this sw::Frame into behaving as one, instead of special
            casing export code all over the place.

        */
        void ForceTreatAsInline();

        /** Get the first node of content in the frame

         @return
         the first node of content in the frame, might not be any at all.
        */
        const SwNode *GetContent() const { return mpStartFrameContent; }
        const Graphic &GetGraphic() const { return maGrf; }
        bool HasGraphic() const { return mbForBullet; }


        /** Does this sw::Frame refer to the same writer content as another

         @return
         if the two sw::Frames are handling the same writer frame
        */
        bool RefersToSameFrameAs(const Frame &rOther) const
        {
            if (mbForBullet && rOther.mbForBullet)
                return (maGrf == rOther.maGrf);
            else if ((!mbForBullet) && (!rOther.mbForBullet))
                return (mpFlyFrm == rOther.mpFlyFrm);
            else
                return false;
        }

        /** The Size of the contained element

         @return
         the best size to use to export to word
        */
        const Size GetSize() const { return maSize; }

        /** The layout size of the contained element

            #i43447# - Needed for graphics, which are scaled into its environment

            @return layout size
        */
        const Size GetLayoutSize() const
        {
            return maLayoutSize;
        }
    };

    /// STL container of Frames
    typedef std::vector<Frame> Frames;
    /// STL iterator for Frames
    typedef std::vector<Frame>::iterator FrameIter;
}

namespace sw
{
    namespace util
    {
        /** Provide a dynamic_cast style cast for SfxPoolItems

            A SfxPoolItem generally need to be cast back to its original type
            to be useful, which is both tedious and errorprone. So item_cast is
            a helper template to aid the process and test if the cast is
            correct.

            @param rItem
            The SfxPoolItem which is to be casted

            @tplparam T
            A SfxPoolItem derived class to cast rItem to

            @return A rItem upcasted back to a T

            @exception std::bad_cast Thrown if the rItem was not a T

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        template<class T> const T & item_cast(const SfxPoolItem &rItem)
            throw(std::bad_cast)
        {
            if (!rItem.IsA(STATICTYPE(T)))
                throw std::bad_cast();
            return static_cast<const T &>(rItem);
        }

        /** Provide a dynamic_cast style cast for SfxPoolItems

            A SfxPoolItem generally need to be cast back to its original type
            to be useful, which is both tedious and errorprone. So item_cast is
            a helper template to aid the process and test if the cast is
            correct.

            @param pItem
            The SfxPoolItem which is to be casted

            @tplparam T
            A SfxPoolItem derived class to cast pItem to

            @return A pItem upcasted back to a T or 0 if pItem was not a T

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        template<class T> const T * item_cast(const SfxPoolItem *pItem)
        {
            if (pItem && !pItem->IsA(STATICTYPE(T)))
                pItem = 0;
            return static_cast<const T *>(pItem);
        }

        /** Extract a SfxPoolItem derived property from a SwCntntNode

            Writer's attributes are retrieved by passing a numeric identifier
            and receiving a SfxPoolItem reference which must then typically be
            cast back to its original type which is both tedious and verbose.

            ItemGet uses item_cast () on the retrived reference to test that the
            retrived property is of the type that the developer thinks it is.

            @param rNode
            The SwCntntNode to retrieve the property from

            @param eType
            The numeric identifier of the property to be retrieved

            @tplparam T
            A SfxPoolItem derived class of the retrieved property

            @exception std::bad_cast Thrown if the property was not a T

            @return The T requested

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        template<class T> const T & ItemGet(const SwCntntNode &rNode,
            sal_uInt16 eType) throw(std::bad_cast)
        {
            return item_cast<T>(rNode.GetAttr(eType));
        }

        /** Extract a SfxPoolItem derived property from a SwFmt

            Writer's attributes are retrieved by passing a numeric identifier
            and receiving a SfxPoolItem reference which must then typically be
            cast back to its original type which is both tedious and verbose.

            ItemGet uses item_cast () on the retrived reference to test that the
            retrived property is of the type that the developer thinks it is.

            @param rFmt
            The SwFmt to retrieve the property from

            @param eType
            The numeric identifier of the property to be retrieved

            @tplparam T
            A SfxPoolItem derived class of the retrieved property

            @exception std::bad_cast Thrown if the property was not a T

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        template<class T> const T & ItemGet(const SwFmt &rFmt,
            sal_uInt16 eType) throw(std::bad_cast)
        {
            return item_cast<T>(rFmt.GetFmtAttr(eType));
        }

        /** Extract a SfxPoolItem derived property from a SfxItemSet

            Writer's attributes are retrieved by passing a numeric identifier
            and receiving a SfxPoolItem reference which must then typically be
            cast back to its original type which is both tedious and verbose.

            ItemGet uses item_cast () on the retrived reference to test that the
            retrived property is of the type that the developer thinks it is.

            @param rSet
            The SfxItemSet to retrieve the property from

            @param eType
            The numeric identifier of the property to be retrieved

            @tplparam T
            A SfxPoolItem derived class of the retrieved property

            @exception std::bad_cast Thrown if the property was not a T

            @return The T requested

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        template<class T> const T & ItemGet(const SfxItemSet &rSet,
            sal_uInt16 eType) throw(std::bad_cast)
        {
            return item_cast<T>(rSet.Get(eType));
        }

        /** Extract a default SfxPoolItem derived property from a SfxItemPool

            Writer's attributes are retrieved by passing a numeric identifier
            and receiving a SfxPoolItem reference which must then typically be
            cast back to its original type which is both tedious and verbose.

            DefaultItemGet returns a reference to the default property of a
            given SfxItemPool for a given property id, e.g. default fontsize

            DefaultItemGet uses item_cast () on the retrived reference to test
            that the retrived property is of the type that the developer thinks
            it is.

            @param rPool
            The SfxItemPool whose default property we want

            @param eType
            The numeric identifier of the default property to be retrieved

            @tplparam T
            A SfxPoolItem derived class of the retrieved property

            @exception std::bad_cast Thrown if the property was not a T

            @return The T requested

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        template<class T> const T & DefaultItemGet(const SfxItemPool &rPool,
            sal_uInt16 eType) throw(std::bad_cast)
        {
            return item_cast<T>(rPool.GetDefaultItem(eType));
        }

        /** Extract a default SfxPoolItem derived property from a SwDoc

            Writer's attributes are retrieved by passing a numeric identifier
            and receiving a SfxPoolItem reference which must then typically be
            cast back to its original type which is both tedious and verbose.

            DefaultItemGet returns a reference to the default property of a
            given SwDoc (Writer Document) for a given property id, e.g default
            fontsize

            DefaultItemGet uses item_cast () on the retrived reference to test
            that the retrived property is of the type that the developer thinks
            it is.

            @param rPool
            The SfxItemPool whose default property we want

            @param eType
            The numeric identifier of the default property to be retrieved

            @tplparam T
            A SfxPoolItem derived class of the retrieved property

            @exception std::bad_cast Thrown if the property was not a T

            @return The T requested

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        template<class T> const T & DefaultItemGet(const SwDoc &rDoc,
            sal_uInt16 eType) throw(std::bad_cast)
        {
            return DefaultItemGet<T>(rDoc.GetAttrPool(), eType);
        }

        /** Return a pointer to a SfxPoolItem derived class if it exists in an
            SfxItemSet

            Writer's attributes are retrieved by passing a numeric identifier
            and receiving a SfxPoolItem reference which must then typically be
            cast back to its original type which is both tedious and verbose.

            HasItem returns a pointer to the requested SfxPoolItem for a given
            property id if it exists in the SfxItemSet or its chain of parents,
            e.g. fontsize

            HasItem uses item_cast () on the retrived pointer to test that the
            retrived property is of the type that the developer thinks it is.

            @param rSet
            The SfxItemSet whose property we want

            @param eType
            The numeric identifier of the default property to be retrieved

            @tplparam T
            A SfxPoolItem derived class of the retrieved property

            @return The T requested or 0 if no T found with id eType

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        template<class T> const T* HasItem(const SfxItemSet &rSet,
            sal_uInt16 eType)
        {
            return item_cast<T>(rSet.GetItem(eType));
        }

        /** Return a pointer to a SfxPoolItem derived class if it exists in an
            SwFmt

            Writer's attributes are retrieved by passing a numeric identifier
            and receiving a SfxPoolItem reference which must then typically be
            cast back to its original type which is both tedious and verbose.

            HasItem returns a pointer to the requested SfxPoolItem for a given
            property id if it exists in the SwFmt e.g. fontsize

            HasItem uses item_cast () on the retrived pointer to test that the
            retrived property is of the type that the developer thinks it is.

            @param rSet
            The SwFmt whose property we want

            @param eType
            The numeric identifier of the default property to be retrieved

            @tplparam T
            A SfxPoolItem derived class of the retrieved property

            @return The T requested or 0 if no T found with id eType

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        template<class T> const T* HasItem(const SwFmt &rFmt,
            sal_uInt16 eType)
        {
            return HasItem<T>(rFmt.GetAttrSet(), eType);
        }

        /** Get the Paragraph Styles of a SwDoc

            Writer's styles are in one of those dreaded macro based pre-STL
            containers. Give me an STL container of the paragraph styles
            instead.

            @param rDoc
            The SwDoc document to get the styles from

            @return A ParaStyles containing the SwDoc's Paragraph Styles

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        ParaStyles GetParaStyles(const SwDoc &rDoc);


        /** Get a Paragraph Style which fits a given name

            Its surprisingly tricky to get a style when all you have is a name,
            but that's what this does

            @param rDoc
            The SwDoc document to search in

            @param rName
            The name of the style to search for

            @return A Paragraph Style if one exists which matches the name

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        SwTxtFmtColl* GetParaStyle(SwDoc &rDoc, const rtl::OUString& rName);

        /** Get a Character Style which fits a given name

            Its surprisingly tricky to get a style when all you have is a name,
            but that's what this does

            @param rDoc
            The SwDoc document to search in

            @param rName
            The name of the style to search for

            @return A Character Style if one exists which matches the name

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        SwCharFmt* GetCharStyle(SwDoc &rDoc, const rtl::OUString& rName);

        /** Sort sequence of Paragraph Styles by assigned outline style list level

            Sort ParaStyles in ascending order of assigned outline style list level,
            e.g.  given Normal/Heading1/Heading2/.../Heading10 at their default
            assigned outline style list levels of body level/level 1/level 2/.../level 10

            #i98791#
            adjust the sorting algorithm due to introduced outline level attribute

            @param rStyles
            The ParaStyles to sort

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        void SortByAssignedOutlineStyleListLevel(ParaStyles &rStyles);

        /** Get the SfxPoolItems of a SfxItemSet

            Writer's SfxPoolItems (attributes) are in one of those dreaded
            macro based pre-STL containers. Give me an STL container of the
            items instead.

            @param rSet
            The SfxItemSet to get the items from

            @param rItems
            The sw::PoolItems to put the items into

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        void GetPoolItems(const SfxItemSet &rSet, PoolItems &rItems, bool bExportParentItemSet );

        const SfxPoolItem *SearchPoolItems(const PoolItems &rItems,
            sal_uInt16 eType);

        template<class T> const T* HasItem(const sw::PoolItems &rItems,
            sal_uInt16 eType)
        {
            return item_cast<T>(SearchPoolItems(rItems, eType));
        }


        /** Remove properties from an SfxItemSet which a SwFmtCharFmt overrides

            Given an SfxItemSet and a SwFmtCharFmt remove from the rSet all the
            properties which the SwFmtCharFmt would override. An SfxItemSet
            contains attributes, and a SwFmtCharFmt is a "Character Style",
            so if the SfxItemSet contains bold and so does the character style
            then delete bold from the SfxItemSet

            @param
            rFmt the SwFmtCharFmt which describes the Character Style

            @param
            rSet the SfxItemSet from which we want to remove any properties
            which the rFmt would override

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>

            @see #i24291# for examples
        */
        void ClearOverridesFromSet(const SwFmtCharFmt &rFmt, SfxItemSet &rSet);

        /** Get the Floating elements in a SwDoc

            Writer's FrmFmts may or may not be anchored to some text content,
            e.g. Page Anchored elements will not be. For the winword export we
            need them to have something to be anchored to. So this method
            returns all the floating elements in a document as a STL container
            of sw::Frames which are guaranteed to have an appropriate anchor.

            @param rDoc
            The SwDoc document to get the styles from

            @param pPaM
            The SwPam to describe the selection in the document to get the
            elements from. 0 means the entire document.

            @return A Frames containing the selections Floating elements

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        Frames GetFrames(const SwDoc &rDoc, SwPaM *pPaM = 0);

        /** Get the Frames anchored to a given node

            Given a container of frames, find the ones anchored to a given node

            @param rFrames
            The container of frames to search in

            @param rNode
            The SwNode to check for anchors to

            @return the Frames in rFrames anchored to rNode

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        Frames GetFramesInNode(const Frames &rFrames, const SwNode &rNode);

        /** Get the Numbering Format used on a paragraph

            There are two differing types of numbering formats that may be on a
            paragraph, normal and outline. The outline is that numbering you
            see in tools->outline numbering. Theres no difference in the
            numbering itself, just how you get it from the SwTxtNode. Needless
            to say the filter generally couldn't care less what type of
            numbering is in use.

            @param rTxtNode
            The SwTxtNode that is the paragraph

            @return A SwNumFmt pointer that describes the numbering level
            on this paragraph, or 0 if there is none.

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        const SwNumFmt* GetNumFmtFromTxtNode(const SwTxtNode &rTxtNode);

        const SwNumRule* GetNumRuleFromTxtNode(const SwTxtNode &rTxtNd);
        const SwNumRule* GetNormalNumRuleFromTxtNode(const SwTxtNode &rTxtNd);


        /** Get the SwNoTxtNode associated with a SwFrmFmt if here is one

            There are two differing types of numbering formats that may be on a
            paragraph, normal and outline. The outline is that numbering you
            see in tools->outline numbering. Theres no difference in the
            numbering itself, just how you get it from the SwTxtNode. Needless
            to say the filter generally couldn't care less what type of
            numbering is in use.

            @param rFmt
            The SwFrmFmt that may describe a graphic

            @return A SwNoTxtNode pointer that describes the graphic of this
            frame if there is one, or 0 if there is none.

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        SwNoTxtNode *GetNoTxtNodeFromSwFrmFmt(const SwFrmFmt &rFmt);

        /** Does a node have a "page break before" applied

            Both text nodes and tables in writer can have "page break before"
            This function gives a unified view to both entities

            @param rNode
            The SwNode to query the page break of

            @return true if there is a page break, false otherwise

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        bool HasPageBreak(const SwNode &rNode);


        /** Make a best fit Polygon from a PolyPolygon

            For custom contours in writer we use a PolyPolygon, while word uses
            a simple polygon, so we need to try and make the best polygon from
            a PolyPolygon

            @param rPolyPoly
            The PolyPolygon to try and turn into a Polygon

            @return best fit Polygon from rPolyPoly

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        Polygon PolygonFromPolyPolygon(const PolyPolygon &rPolyPoly);

        /** Determine if the font is the special Star|Open Symbol font

            @param rFontName
            The FontName to test for being Star|Open Symbol

            @return true if this is Star|Open Symbol

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        bool IsStarSymbol(const rtl::OUString &rFontName);

        /** Make setting a drawing object's layer in a Writer document easy


            Word has the simple concept of a drawing object either in the
            foreground and in the background. We have an additional complexity
            that form components live in a seperate layer, which seems
            unnecessarily complicated. So in the winword filter we set the
            object's layer through this class with either SendObjectToHell for
            the bottom layer and SendObjectToHeaven for the top and we don't
            worry about the odd form layer design wrinkle.

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        class SetLayer
        {
        private:
            sal_uInt8 mnHeavenLayer, mnHellLayer, mnFormLayer;
            enum Layer {eHeaven, eHell};
            void SetObjectLayer(SdrObject &rObject, Layer eLayer) const;
            void Swap(SetLayer &rOther) throw();
        public:

            /** Make Object live in the bottom drawing layer

                @param rObject
                The object to be set to the bottom layer
            */
            void SendObjectToHell(SdrObject &rObject) const;

            /** Make Object lives in the top top layer

                @param rObject
                The object to be set to the bottom layer
            */
            void SendObjectToHeaven(SdrObject &rObject) const;

            /** Normal constructor

                @param rDoc
                The Writer document whose drawing layers we will be inserting
                objects into
            */
            SetLayer(const SwDoc &rDoc);

            SetLayer(const SetLayer &rOther) throw();
            SetLayer& operator=(const SetLayer &rOther) throw();
        };
    }

    namespace hack
    {
            /** Map an ID valid in one SfxItemPool to its equivalent in another

            Given a WhichId (the id that identifies a property e.g. bold) which
            is correct in a given SfxItemPool, get the equivalent whichId in
            another SfxItemPool

            This arises because the drawing layer uses the same properties as
            writer e.g. SvxWeight, but for some reason uses different ids
            for the same properties as writer.

            @param rDestPool
            The SfxItemPool in whose terms the Id is returned

            @param rSrcPool
            The SfxItemPool in whose terms the Id is passed in

            @param nWhich
            The Id to transform from source to dest

            @return 0 on failure, the correct property Id on success

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        sal_uInt16 TransformWhichBetweenPools(const SfxItemPool &rDestPool,
            const SfxItemPool &rSrcPool, sal_uInt16 nWhich);

        /** Map a SwDoc WhichId to the equivalent Id for a given SfxItemSet

            Given a WhichId (the id that identifies a property e.g. bold) which
            is correct for a Writer document, get the equivalent whichId which
            for a given SfxItemSet.

            This arises because the drawing layer uses the same properties as
            writer e.g. SvxWeight, but for some reason uses different ids
            for the same properties as writer.

            This is effectively the same as TransformWhichBetweenPools except
            at a slightly different layer.

            @param rSet
            The SfxItemSet in whose terms the Id is returned

            @param rDoc
            The SwDoc in whose terms the Id is passed in

            @param nWhich
            The Id to transform from writer to the SfxItemSet's domain

            @return 0 on failure, the correct SfxItemSet Id on success

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        sal_uInt16 GetSetWhichFromSwDocWhich(const SfxItemSet &rSet,
            const SwDoc &rDoc, sal_uInt16 nWhich);


        /** Make inserting an OLE object into a Writer document easy

            The rest of Office uses SdrOle2Obj for their OLE objects, Writer
            doesn't, which makes things a bit difficult as this is the type of
            object that the escher import code shared by the MSOffice filters
            produces when it imports an OLE object.

            This utility class takes ownership of the OLE object away from a
            SdrOle2Obj and can massage it into the condition best suited to
            insertion into Writer.

            If the object was not transferred into Writer then it is deleted
            during destruction.

            @author
            <a href="mailto:cmc@openoffice.org">Caol&aacute;n McNamara</a>
        */
        class DrawingOLEAdaptor
        {
        private:
            rtl::OUString msOrigPersistName;
            com::sun::star::uno::Reference < com::sun::star::embed::XEmbeddedObject > mxIPRef;
            SfxObjectShell& mrPers;
            Graphic* mpGraphic;
        public:
            /** Take ownership of a SdrOle2Objs OLE object

                @param rObj
                The SdrOle2Obj whose OLE object we want to take control of

                @param rPers
                The SvPersist of a SwDoc (SwDoc::GetPersist()) into which we
                may want to move the object, or remove it from if unwanted.
            */
            DrawingOLEAdaptor(SdrOle2Obj &rObj, SfxObjectShell &rPers);

            /// Destructor will destroy the owned OLE object if not transferred
            ~DrawingOLEAdaptor();

            /** Transfer ownership of the OLE object to a document's SvPersist

                TransferToDoc moves the object into the persist under the name
                passed in. This name is then suitable to be used as an argument
                to SwDoc::InsertOLE.

                The object is no longer owned by the adaptor after this call,
                subsequent calls are an error and return false.

                @param rName
                The name to store the object under in the document.

                @return On success true is returned, otherwise false. On
                success rName is then suitable for user with SwDoc::InsertOLE
            */
            bool TransferToDoc(rtl::OUString &rName);
        private:
            /// No assigning allowed
            DrawingOLEAdaptor& operator=(const DrawingOLEAdaptor&);
            /// No copying allowed
            DrawingOLEAdaptor(const DrawingOLEAdaptor &rDoc);
        };
    }
}

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
