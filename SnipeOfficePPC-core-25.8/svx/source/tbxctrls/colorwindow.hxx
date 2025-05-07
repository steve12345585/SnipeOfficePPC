/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
//========================================================================
// class SvxColorWindow_Impl --------------------------------------------------
//========================================================================
#ifndef __SVX_COLORWINDOW_HXX_
#define __SVX_COLORWINDOW_HXX_

#include <sfx2/tbxctrl.hxx>
#include <svtools/valueset.hxx>
#include <svl/lstner.hxx>
#include <rtl/ustring.hxx>
#include <com/sun/star/frame/XFrame.hpp>

//========================================================================
// class SvxColorWindow_Impl --------------------------------------------------
//========================================================================

class SvxColorWindow_Impl : public SfxPopupWindow
{
    using FloatingWindow::StateChanged;

private:
    const sal_uInt16 theSlotId;
    ValueSet       aColorSet;
    rtl::OUString  maCommand;
    const Color    mLastColor;

#if _SOLAR__PRIVATE
    DECL_LINK( SelectHdl, void * );
#endif

protected:
    virtual void    Resize();
    virtual sal_Bool    Close();

public:
    SvxColorWindow_Impl( const rtl::OUString& rCommand,
                         sal_uInt16 nSlotId,
                         const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame >& rFrame,
                         const String& rWndTitle,
                         Window* pParentWindow,
                         const Color rLastColor = COL_AUTO);
    ~SvxColorWindow_Impl();
    void                StartSelection();

    virtual void        KeyInput( const KeyEvent& rKEvt );
    virtual void        StateChanged( sal_uInt16 nSID, SfxItemState eState, const SfxPoolItem* pState );

    virtual SfxPopupWindow* Clone() const;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
