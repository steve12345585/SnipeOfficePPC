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

#ifndef _IOS_CLIPBOARD_HXX_
#define _IOS_CLIPBOARD_HXX_

#include <rtl/ustring.hxx>
#include <sal/types.h>
#include <cppuhelper/compbase4.hxx>
#include <com/sun/star/datatransfer/XTransferable.hpp>
#include <com/sun/star/datatransfer/clipboard/XClipboardEx.hpp>
#include <com/sun/star/datatransfer/clipboard/XClipboardOwner.hpp>
#include <com/sun/star/datatransfer/clipboard/XClipboardListener.hpp>
#include <com/sun/star/datatransfer/clipboard/XClipboardNotifier.hpp>
#include <com/sun/star/datatransfer/XMimeContentTypeFactory.hpp>
#include <com/sun/star/datatransfer/clipboard/XFlushableClipboard.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <cppuhelper/basemutex.hxx>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>

#include <boost/utility.hpp>
#include <list>

#include <premac.h>
#import <UIKit/UIKit.h>
#include <postmac.h>

class IosClipboard;

@interface EventListener : NSObject
{
     IosClipboard* pIosClipboard;
}

// Init the pasteboard change listener with a reference to the OfficeClipboard
// instance
- (EventListener*)initWithIosClipboard: (IosClipboard*) pcb;

// Promiss resolver function
- (void)pasteboard:(UIPasteboard*)sender provideDataForType:(NSString *)type;

-(void)applicationDidBecomeActive:(NSNotification*)aNotification;

-(void)disposing;
@end


class IosClipboard : public ::cppu::BaseMutex,
                      public ::cppu::WeakComponentImplHelper4< com::sun::star::datatransfer::clipboard::XClipboardEx,
                                                               com::sun::star::datatransfer::clipboard::XClipboardNotifier,
                                                               com::sun::star::datatransfer::clipboard::XFlushableClipboard,
                                                               com::sun::star::lang::XServiceInfo >,
                      private ::boost::noncopyable
{
public:
  /* Create a clipboard instance.

     @param pasteboard
     If not equal NULL the instance will be instantiated with the provided
     pasteboard reference and 'bUseSystemClipboard' will be ignored

     @param bUseSystemClipboard
     If 'pasteboard' is NULL 'bUseSystemClipboard' determines whether the
     system clipboard will be created (bUseSystemClipboard == true) or if
     the DragPasteboard if bUseSystemClipboard == false
   */
  IosClipboard(UIPasteboard* pasteboard = NULL,
                bool bUseSystemClipboard = true);

  ~IosClipboard();

  //------------------------------------------------
  // XClipboard
  //------------------------------------------------

  virtual ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable > SAL_CALL getContents()
    throw( ::com::sun::star::uno::RuntimeException );

  virtual void SAL_CALL setContents( const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable >& xTransferable,
                                     const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::clipboard::XClipboardOwner >& xClipboardOwner )
    throw( ::com::sun::star::uno::RuntimeException );

  virtual ::rtl::OUString SAL_CALL getName()
    throw( ::com::sun::star::uno::RuntimeException );

  //------------------------------------------------
  // XClipboardEx
  //------------------------------------------------

  virtual sal_Int8 SAL_CALL getRenderingCapabilities()
    throw( ::com::sun::star::uno::RuntimeException );

  //------------------------------------------------
  // XClipboardNotifier
  //------------------------------------------------

  virtual void SAL_CALL addClipboardListener( const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::clipboard::XClipboardListener >& listener )
    throw( ::com::sun::star::uno::RuntimeException );

  virtual void SAL_CALL removeClipboardListener( const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::clipboard::XClipboardListener >& listener )
    throw( ::com::sun::star::uno::RuntimeException );

  //------------------------------------------------
  // XFlushableClipboard
  //------------------------------------------------

  virtual void SAL_CALL flushClipboard( ) throw( com::sun::star::uno::RuntimeException );

  //------------------------------------------------
  // XServiceInfo
  //------------------------------------------------

  virtual ::rtl::OUString SAL_CALL getImplementationName()
    throw(::com::sun::star::uno::RuntimeException);

  virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName )
    throw(::com::sun::star::uno::RuntimeException);

  virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames()
    throw(::com::sun::star::uno::RuntimeException);

  /* Get a reference to the used pasteboard.
   */
  UIPasteboard* getPasteboard() const;

  /* Notify the current clipboard owner that he is no longer the clipboard owner.
   */
  void fireLostClipboardOwnershipEvent(::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::clipboard::XClipboardOwner> oldOwner,
                                       ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable > oldContent);

  void pasteboardChangedOwner();

  void provideDataForType(UIPasteboard* sender, NSString* type);

  void applicationDidBecomeActive(NSNotification* aNotification);

private:

  /* Notify all registered XClipboardListener that the clipboard content
     has changed.
  */
  void fireClipboardChangedEvent();

private:
  ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XMimeContentTypeFactory > mrXMimeCntFactory;
  ::std::list< ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::clipboard::XClipboardListener > > mClipboardListeners;
  ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable > mXClipboardContent;
  com::sun::star::uno::Reference< com::sun::star::datatransfer::clipboard::XClipboardOwner > mXClipboardOwner;
  bool mIsSystemPasteboard;
  UIPasteboard* mPasteboard;
  int mPasteboardChangeCount;
  EventListener* mEventListener;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
