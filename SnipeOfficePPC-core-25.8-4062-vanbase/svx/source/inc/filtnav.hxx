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
#ifndef _SVX_FILTNAV_HXX
#define _SVX_FILTNAV_HXX

#include <com/sun/star/form/XForm.hpp>
#include <com/sun/star/form/runtime/XFormController.hpp>
#include <com/sun/star/form/runtime/XFilterController.hpp>
#include <svl/lstner.hxx>
#include <svl/brdcst.hxx>

#include <vcl/window.hxx>
#include <sfx2/childwin.hxx>
#include <svl/poolitem.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/dockwin.hxx>
#include <sfx2/ctrlitem.hxx>
#include <vcl/image.hxx>
#include <svtools/treelistbox.hxx>

#include <vcl/dialog.hxx>
#include <vcl/group.hxx>
#include <vcl/dockwin.hxx>
#include "svx/fmtools.hxx"
#include "fmexch.hxx"
#include "sqlparserclient.hxx"

class FmFormShell;

//........................................................................
namespace svxform
{
//........................................................................

class FmFilterItem;
class FmFilterItems;
class FmParentData;
class FmFilterAdapter;

//========================================================================
// data structure for the filter model
//========================================================================
class FmFilterData
{
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > m_xORB;
    FmParentData*           m_pParent;
    ::rtl::OUString         m_aText;

public:
    TYPEINFO();
    FmFilterData(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxFactory,FmParentData* pParent = NULL, const ::rtl::OUString& rText = ::rtl::OUString())
        :m_xORB( _rxFactory )
        ,m_pParent( pParent )
        ,m_aText( rText )
    {}
    virtual ~FmFilterData(){}

    void    SetText( const ::rtl::OUString& rText ){ m_aText = rText; }
    ::rtl::OUString  GetText() const { return m_aText; }
    FmParentData* GetParent() const {return m_pParent;}

    virtual Image GetImage() const;
};

//========================================================================
class FmParentData : public FmFilterData
{
protected:
    ::std::vector< FmFilterData* >  m_aChildren;

public:
    TYPEINFO();
    FmParentData(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxFactory,FmParentData* pParent, const ::rtl::OUString& rText)
        : FmFilterData(_rxFactory,pParent, rText)
    {}
    virtual ~FmParentData();

    ::std::vector< FmFilterData* >& GetChildren() { return m_aChildren; }
};

//========================================================================
// Item representing the forms and subforms
class FmFormItem : public FmParentData
{
    ::com::sun::star::uno::Reference< ::com::sun::star::form::runtime::XFormController >    m_xController;
    ::com::sun::star::uno::Reference< ::com::sun::star::form::runtime::XFilterController >  m_xFilterController;

public:
    TYPEINFO();

    FmFormItem(  const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxFactory,FmParentData* _pParent,
                 const ::com::sun::star::uno::Reference< ::com::sun::star::form::runtime::XFormController > & _xController,
                 const ::rtl::OUString& _rText)
        :FmParentData( _rxFactory, _pParent, _rText )
        ,m_xController( _xController )
        ,m_xFilterController( _xController, ::com::sun::star::uno::UNO_QUERY_THROW )
    {
    }

    inline const ::com::sun::star::uno::Reference< ::com::sun::star::form::runtime::XFormController >&
        GetController() { return m_xController; }

    inline const ::com::sun::star::uno::Reference< ::com::sun::star::form::runtime::XFilterController >&
        GetFilterController() { return m_xFilterController; }

    virtual Image GetImage() const;
};

//========================================================================
class FmFilterItems : public FmParentData
{
public:
    TYPEINFO();
    FmFilterItems(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxFactory):FmParentData(_rxFactory,NULL, ::rtl::OUString()){}
    FmFilterItems(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxFactory,FmFormItem* pParent, const ::rtl::OUString& rText ):FmParentData(_rxFactory,pParent, rText){}

    FmFilterItem* Find( const ::sal_Int32 _nFilterComponentIndex ) const;
    virtual Image GetImage() const;
};

//========================================================================
class FmFilterItem : public FmFilterData
{
    ::rtl::OUString     m_aFieldName;
    const sal_Int32     m_nComponentIndex;

public:
    TYPEINFO();
    FmFilterItem(
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxFactory,
        FmFilterItems* pParent,
        const ::rtl::OUString& aFieldName,
        const ::rtl::OUString& aCondition,
        const sal_Int32 _nComponentIndex
    );

    const ::rtl::OUString& GetFieldName() const {return m_aFieldName;}
    sal_Int32 GetComponentIndex() const { return m_nComponentIndex; }

    virtual Image GetImage() const;
};

//========================================================================
class FmFilterModel : public FmParentData
                     ,public SfxBroadcaster
                     ,public ::svxform::OSQLParserClient
{
    friend class FmFilterAdapter;

    ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess >           m_xControllers;
    ::com::sun::star::uno::Reference< ::com::sun::star::form::runtime::XFormController >    m_xController;
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >        m_xORB;
    FmFilterAdapter*        m_pAdapter;
    FmFilterItems*          m_pCurrentItems;

public:
    TYPEINFO();
    FmFilterModel(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxFactory);
    virtual ~FmFilterModel();

    void Update(const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > & xControllers, const ::com::sun::star::uno::Reference< ::com::sun::star::form::runtime::XFormController > & xCurrent);
    void Clear();
    sal_Bool ValidateText(FmFilterItem* pItem, UniString& rText, UniString& rErrorMsg) const;
    void Append(FmFilterItems* pItems, FmFilterItem* pFilterItem);
    void SetTextForItem(FmFilterItem* pItem, const ::rtl::OUString& rText);

    FmFormItem* GetCurrentForm() const {return m_pCurrentItems ? (FmFormItem*)m_pCurrentItems->GetParent() : NULL;}
    FmFilterItems* GetCurrentItems() const {return m_pCurrentItems;}
    void SetCurrentItems(FmFilterItems* pCurrent);
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > getORB() const { return m_xORB; }

    const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > & GetControllers() const {return m_xControllers;}
    const ::com::sun::star::uno::Reference< ::com::sun::star::form::runtime::XFormController > & GetCurrentController() const {return m_xController;}
    void SetCurrentController(const ::com::sun::star::uno::Reference< ::com::sun::star::form::runtime::XFormController > & xController);

    void Remove(FmFilterData* pFilterItem);
    void AppendFilterItems( FmFormItem& _rItem );
    void EnsureEmptyFilterRows( FmParentData& _rItem );

protected:
    void Insert(const ::std::vector<FmFilterData*>::iterator& rPos, FmFilterData* pFilterItem);
    void Remove( const ::std::vector<FmFilterData*>::iterator& rPos );
    FmFormItem* Find(const ::std::vector<FmFilterData*>& rItems, const ::com::sun::star::uno::Reference< ::com::sun::star::form::runtime::XFormController > & xController) const;
    FmFormItem* Find(const ::std::vector<FmFilterData*>& rItems, const ::com::sun::star::uno::Reference< ::com::sun::star::form::XForm >& xForm) const;
    void Update(const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > & xControllers, FmParentData* pParent);
};

//========================================================================
//========================================================================
class OFilterItemExchange : public OLocalExchange
{
    ::std::vector<FmFilterItem*>    m_aDraggedEntries;
    FmFormItem*             m_pFormItem;        // ensure that we drop on the same form

public:
    OFilterItemExchange();

    static sal_uInt32       getFormatId( );
    inline static sal_Bool  hasFormat( const DataFlavorExVector& _rFormats );

    const ::std::vector<FmFilterItem*>& getDraggedEntries() const { return m_aDraggedEntries; }
    void setDraggedEntries(const ::std::vector<FmFilterItem*>& _rList) { m_aDraggedEntries = _rList; }
    FmFormItem* getFormItem() const { return m_pFormItem; }

    void setFormItem( FmFormItem* _pItem ) { m_pFormItem = _pItem; }
    void addSelectedItem( FmFilterItem* _pItem) { m_aDraggedEntries.push_back(_pItem); }

protected:
    virtual void AddSupportedFormats();
};

inline sal_Bool OFilterItemExchange::hasFormat( const DataFlavorExVector& _rFormats )
{
    return OLocalExchange::hasFormat( _rFormats, getFormatId() );
}

//====================================================================
//= OFilterExchangeHelper
//====================================================================
class OFilterExchangeHelper : public OLocalExchangeHelper
{
public:
    OFilterExchangeHelper(Window* _pDragSource) : OLocalExchangeHelper(_pDragSource) { }

    OFilterItemExchange* operator->() const { return static_cast<OFilterItemExchange*>(m_pTransferable); }

protected:
    virtual OLocalExchange* createExchange() const;
};

//========================================================================
class FmFilterNavigator : public SvTreeListBox, public SfxListener
{
    enum DROP_ACTION{ DA_SCROLLUP, DA_SCROLLDOWN, DA_EXPANDNODE };

    FmFilterModel*          m_pModel;
    SvTreeListEntry*            m_pEditingCurrently;
    OFilterExchangeHelper   m_aControlExchange;


    AutoTimer           m_aDropActionTimer;
    unsigned short      m_aTimerCounter;
    Point               m_aTimerTriggered;      // die Position, an der der DropTimer angeschaltet wurde
    DROP_ACTION         m_aDropActionType;

public:
    FmFilterNavigator( Window* pParent );
    virtual ~FmFilterNavigator();

    void UpdateContent(
            const ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > & xControllers,
            const ::com::sun::star::uno::Reference< ::com::sun::star::form::runtime::XFormController > & xCurrent
        );
    const FmFilterModel* GetFilterModel() const {return m_pModel;}

protected:
    using Control::Notify;

    virtual void KeyInput( const KeyEvent& rKEvt );
    virtual void Command( const CommandEvent& rEvt );
    virtual void Notify( SfxBroadcaster& rBC, const SfxHint& rHint );
    virtual void InitEntry(SvTreeListEntry* pEntry, const OUString& rStr, const Image& rImg1, const Image& rImg2, SvLBoxButtonKind eButtonKind);
    virtual sal_Bool Select( SvTreeListEntry* pEntry, sal_Bool bSelect=sal_True );
    virtual sal_Bool EditingEntry( SvTreeListEntry* pEntry, Selection& rSelection );
    virtual sal_Bool EditedEntry( SvTreeListEntry* pEntry, const rtl::OUString& rNewText );

    virtual sal_Int8    AcceptDrop( const AcceptDropEvent& rEvt );
    virtual sal_Int8    ExecuteDrop( const ExecuteDropEvent& rEvt );
    virtual void        StartDrag( sal_Int8 nAction, const Point& rPosPixel );

    void DeleteSelection();
    SvTreeListEntry* FindEntry(const FmFilterData* pItem) const;
    void Insert(FmFilterData* pItem, sal_uLong nPos);
    void Remove(FmFilterData* pItem);

    DECL_LINK(OnRemove, SvTreeListEntry*);
    DECL_LINK(OnEdited, SvTreeListEntry*);
    DECL_LINK(OnDropActionTimer, void*);

private:
    /** returns the first form item and the selected FilterItems in the vector
        @param  _rItemList
            Is filled inside. <OUT/>
        @return
            The first form item.
    */
    FmFormItem* getSelectedFilterItems(::std::vector<FmFilterItem*>& _rItemList);
    /* inserts the filter items into the tree model and creates new FilterItems if needed.
        @param  _rFilterList
            The items which should be inserted.
        @param  _pTargetItems
            The target where to insert the items.
        @param  _bCopy
            If <TRUE/> the items will not be removed from the model, otherwise they will.
    */
    void insertFilterItem(const ::std::vector<FmFilterItem*>& _rFilterList,FmFilterItems* _pTargetItems,sal_Bool _bCopy = sal_False);
    SvTreeListEntry* getPrevEntry(SvTreeListEntry* _pStartWith = NULL);
    SvTreeListEntry* getNextEntry(SvTreeListEntry* _pStartWith = NULL);

    using SvTreeListBox::Select;
    using SvTreeListBox::ExecuteDrop;
    using SvTreeListBox::Insert;
};

//========================================================================
class FmFilterNavigatorWin : public SfxDockingWindow, public SfxControllerItem
{
private:
    FmFilterNavigator* m_pNavigator;

protected:
    virtual void Resize();
    virtual sal_Bool Close();
    virtual Size CalcDockingSize( SfxChildAlignment );
    virtual SfxChildAlignment CheckAlignment( SfxChildAlignment, SfxChildAlignment );

public:
    FmFilterNavigatorWin( SfxBindings *pBindings, SfxChildWindow *pMgr,
                   Window* pParent );
    virtual ~FmFilterNavigatorWin();

    void UpdateContent( FmFormShell* pFormShell );
    void StateChanged( sal_uInt16 nSID, SfxItemState eState, const SfxPoolItem* pState );
    void FillInfo( SfxChildWinInfo& rInfo ) const;

    using SfxDockingWindow::StateChanged;

    virtual void GetFocus();
};

//========================================================================
class FmFilterNavigatorWinMgr : public SfxChildWindow
{
public:
    FmFilterNavigatorWinMgr( Window *pParent, sal_uInt16 nId, SfxBindings *pBindings,
                      SfxChildWinInfo *pInfo );
    SFX_DECL_CHILDWINDOW( FmFilterNavigatorWinMgr );
};

//........................................................................
}   // namespace svxform
//........................................................................

#endif // _SVX_FILTNAV_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
