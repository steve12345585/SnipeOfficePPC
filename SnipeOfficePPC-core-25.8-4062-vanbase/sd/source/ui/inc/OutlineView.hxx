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

#ifndef SD_OUTLINE_VIEW_HXX
#define SD_OUTLINE_VIEW_HXX

#include <vcl/image.hxx>
#include <editeng/lrspitem.hxx>
#include "View.hxx"

class SdPage;
class SdrPage;
class Paragraph;
class SdrTextObj;
class Outliner;
class SfxProgress;
struct PaintFirstLineInfo;
struct PasteOrDropInfos;

namespace sd { namespace tools {
class EventMultiplexerEvent;
} }

namespace sd {

class DrawDocShell;
class OutlineViewShell;
class OutlineViewModelChangeGuard;
class DrawDocShell;

static const int MAX_OUTLINERVIEWS = 4;

/*************************************************************************
|*
|* Ableitung von ::sd::View fuer den Outline-Modus
|*
\************************************************************************/

class OutlineView
    : public ::sd::View
{
    friend class OutlineViewModelChangeGuard;
public:
    OutlineView (DrawDocShell& rDocSh,
        ::Window* pWindow,
        OutlineViewShell& rOutlineViewSh);
    ~OutlineView (void);

    /** This method is called by the view shell that owns the view to tell
        the view that it can safely connect to the application.
        This method must not be called before the view shell is on the shell
        stack.
    */
    void ConnectToApplication (void);
    void DisconnectFromApplication (void);

    TYPEINFO();

    SdrTextObj*     GetTitleTextObject(SdrPage* pPage);
    SdrTextObj*     GetOutlineTextObject(SdrPage* pPage);

    SdrTextObj*     CreateTitleTextObject(SdPage* pPage);
    SdrTextObj*     CreateOutlineTextObject(SdPage* pPage);

    virtual void AddWindowToPaintView(OutputDevice* pWin);
    virtual void DeleteWindowFromPaintView(OutputDevice* pWin);

    OutlinerView*   GetViewByWindow (::Window* pWin) const;
    SdrOutliner*    GetOutliner() { return(&mrOutliner) ; }

    Paragraph*      GetPrevTitle(const Paragraph* pPara);
    Paragraph*      GetNextTitle(const Paragraph* pPara);
    SdPage*         GetActualPage();
    SdPage*         GetPageForParagraph( Paragraph* pPara );
    Paragraph*      GetParagraphForPage( ::Outliner& rOutl, SdPage* pPage );

    /** selects the paragraph for the given page at the outliner view*/
    void            SetActualPage( SdPage* pActual );

    virtual void Paint (const Rectangle& rRect, ::sd::Window* pWin);
    virtual void AdjustPosSizePixel(
        const Point &rPos,
        const Size &rSize,
        ::sd::Window* pWindow);

                    // Callbacks fuer LINKs
    DECL_LINK( ParagraphInsertedHdl, Outliner * );
    DECL_LINK( ParagraphRemovingHdl, Outliner * );
    DECL_LINK( DepthChangedHdl, Outliner * );
    DECL_LINK( StatusEventHdl, void * );
    DECL_LINK( BeginMovingHdl, Outliner * );
    DECL_LINK( EndMovingHdl, Outliner * );
    DECL_LINK(RemovingPagesHdl, void *);
    DECL_LINK( IndentingPagesHdl, OutlinerView * );
    DECL_LINK( BeginDropHdl, void * );
    DECL_LINK( EndDropHdl, void * );
    DECL_LINK( PaintingFirstLineHdl, PaintFirstLineInfo* );

    sal_uLong         GetPaperWidth() const { return 2*21000; }  // DIN A4 Breite

    sal_Bool          PrepareClose(sal_Bool bUI = sal_True);

    virtual sal_Bool    GetAttributes( SfxItemSet& rTargetSet, sal_Bool bOnlyHardAttr=sal_False ) const;
    virtual sal_Bool    SetAttributes(const SfxItemSet& rSet, sal_Bool bReplaceAll = sal_False);

    void               FillOutliner();
    void               SetLinks();
    void               ResetLinks() const;

    SfxStyleSheet*     GetStyleSheet() const;

    void               SetSelectedPages();

    virtual sal_Int8 AcceptDrop (
        const AcceptDropEvent& rEvt,
        DropTargetHelper& rTargetHelper,
        ::sd::Window* pTargetWindow = NULL,
        sal_uInt16 nPage = SDRPAGE_NOTFOUND,
        sal_uInt16 nLayer = SDRPAGE_NOTFOUND);
    virtual sal_Int8 ExecuteDrop (
        const ExecuteDropEvent& rEvt,
        DropTargetHelper& rTargetHelper,
        ::sd::Window* pTargetWindow = NULL,
        sal_uInt16 nPage = SDRPAGE_NOTFOUND,
        sal_uInt16 nLayer = SDRPAGE_NOTFOUND);


    // Re-implement GetScriptType for this view to get correct results
    virtual sal_uInt16 GetScriptType() const;

    /** After this method has been called with <TRUE/> following changes of
        the current page are ignored in that the corresponding text is not
        selected.
        This is used to supress unwanted side effects between selection and
        cursor position.
    */
    void IgnoreCurrentPageChanges (bool bIgnore);
    void InvalidateSlideNumberArea();

    /** creates and inserts an empty slide for the given paragraph. */
    SdPage* InsertSlideForParagraph( Paragraph* pPara );

    void UpdateParagraph( sal_Int32 nPara );

protected:
    virtual void OnBeginPasteOrDrop( PasteOrDropInfos* pInfos );
    virtual void OnEndPasteOrDrop( PasteOrDropInfos* pInfos );

private:
    /** call this method before you do anything that can modify the outliner
        and or the drawing document model. It will create needed undo actions */
    void BeginModelChange();

    /** call this method after BeginModelChange(), when all possible model
        changes are done. */
    void EndModelChange();

    /** merge edit engine undo actions if possible */
    void TryToMergeUndoActions();

    /** updates all changes in the outliner model to the draw model */
    void UpdateDocument();

    OutlineViewShell&   mrOutlineViewShell;
    SdrOutliner&        mrOutliner;
    OutlinerView*       mpOutlinerView[MAX_OUTLINERVIEWS];

    std::vector<Paragraph*> maOldParaOrder;
    std::vector<Paragraph*> maSelectedParas;

    sal_Int32               mnPagesToProcess;    // for the progress bar
    sal_Int32               mnPagesProcessed;

    sal_Bool                mbFirstPaint;

    SfxProgress*        mpProgress;

    /** stores the last used document color.
        this is changed in onUpdateStyleSettings()
    */
    Color maDocColor;

    /** updates the high contrast settings and document color if they changed.
        @param bForceUpdate forces the method to set all style settings
    */
    void onUpdateStyleSettings( bool bForceUpdate = false );

    /** this link is called from the vcl applicaten when the stylesettings
        change. Its only purpose is to call onUpdateStyleSettings() then.
    */
    DECL_LINK( AppEventListenerHdl, void * );

    DECL_LINK(EventMultiplexerListener, sd::tools::EventMultiplexerEvent*);

    /** holds a model guard during drag and drop between BeginMovingHdl and EndMovingHdl */
    std::auto_ptr< OutlineViewModelChangeGuard > maDragAndDropModelGuard;

    Font maPageNumberFont;
    Font maBulletFont;

    SvxLRSpaceItem maLRSpaceItem;
    Image maSlideImage;
};

// calls IgnoreCurrentPageChangesLevel with true in ctor and with false in dtor
class OutlineViewPageChangesGuard
{
public:
    OutlineViewPageChangesGuard( OutlineView* pView );
    ~OutlineViewPageChangesGuard();
private:
    OutlineView* mpView;
};

// calls BeginModelChange() on c'tor and EndModelChange() on d'tor
class OutlineViewModelChangeGuard
{
public:
    OutlineViewModelChangeGuard( OutlineView& rView );
    ~OutlineViewModelChangeGuard();
private:
    OutlineView& mrView;
};

} // end of namespace sd

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
