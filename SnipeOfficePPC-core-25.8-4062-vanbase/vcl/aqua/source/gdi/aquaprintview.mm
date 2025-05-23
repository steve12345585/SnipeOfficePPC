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


#include "vcl/print.hxx"

#include "aqua/aquaprintview.h"
#include "aqua/salprn.h"

@implementation AquaPrintView
-(id)initWithController: (vcl::PrinterController*)pController withInfoPrinter: (AquaSalInfoPrinter*)pInfoPrinter
{
    NSRect aRect = { { 0, 0 }, [pInfoPrinter->getPrintInfo() paperSize] };
    if( (self = [super initWithFrame: aRect]) != nil )
    {
        mpController = pController;
        mpInfoPrinter = pInfoPrinter;
    }
    return self;
}

-(BOOL)knowsPageRange: (NSRangePointer)range
{
    range->location = 1;
    range->length = mpInfoPrinter->getCurPageRangeCount();
    return YES;
}

-(NSRect)rectForPage: (int)page
{
    NSSize aPaperSize =  [mpInfoPrinter->getPrintInfo() paperSize];
    int nWidth = (int)aPaperSize.width;
    // #i101108# sanity check
    if( nWidth < 1 )
        nWidth = 1;
    NSRect aRect = { { page % nWidth, page / nWidth }, aPaperSize };
    return aRect;
}

-(NSPoint)locationOfPrintRect: (NSRect)aRect
{
    (void)aRect;
    NSPoint aPoint = { 0, 0 };
    return aPoint;
}

-(void)drawRect: (NSRect)rect
{
    mpInfoPrinter->setStartPageOffset( static_cast<int>(rect.origin.x), static_cast<int>(rect.origin.y) );
    NSSize aPaperSize =  [mpInfoPrinter->getPrintInfo() paperSize];
    int nPage = (int)(aPaperSize.width * rect.origin.y + rect.origin.x);
    
    // page count is 1 based
    if( nPage - 1 < (mpInfoPrinter->getCurPageRangeStart() + mpInfoPrinter->getCurPageRangeCount() ) )
        mpController->printFilteredPage( nPage-1 );
}
@end

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
