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

#include "precompile.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "hwplib.h"
#include "hwpfile.h"
#include "hiodev.h"
#include "hfont.h"
#include "hstyle.h"
#include "hbox.h"
#include "hpara.h"
#include "htags.h"
#include "hcode.h"
#include "hstream.h"

#include <osl/diagnose.h>

#define HWPHeadLen  128
#define HWPSummaryLen   1008

#define FILESTG_SIGNATURE       0xF8995567
#define FILESTG_SIGNATURE_NORMAL    0xF8995568

HWPFile *HWPFile::cur_doc = 0;
static int ccount = 0;
static int pcount = 0;
static int datecodecount = 0;

HWPFile::HWPFile()
    : version(HWP_V30)
    , compressed(false)
    , encrypted(false)
    , linenumber(0)
    , info_block_len(0)
    , error_code(HWP_NoError)
    , oledata(0)
    , m_nCurrentPage(1)
    , m_nMaxSettedPage(0)
    , hiodev(0)
    , currenthyper(0)
{
    SetCurrentDoc(this);
}

HWPFile::~HWPFile()
{
    delete oledata;
    delete hiodev;

    std::list < ColumnInfo* >::iterator it_column = columnlist.begin();
    for (; it_column != columnlist.end(); ++it_column)
        delete *it_column;

    std::list < HWPPara* >::iterator it = plist.begin();
    for (; it != plist.end(); ++it)
        delete *it;

    std::list < Table* >::iterator tbl = tables.begin();
    for (; tbl != tables.end(); ++tbl)
        delete *tbl;

    std::list < HyperText* >::iterator hyp = hyperlist.begin();
    for (; hyp != hyperlist.end(); ++hyp)
    {
        delete *hyp;
    }
}

int HWPFile::ReadHwpFile(HStream & stream)
{
    if (Open(stream) != HWP_NoError)
        return State();
    InfoRead();
    FontRead();
    StyleRead();
    AddColumnInfo();
    ParaListRead();
    TagsRead();

    return State();
}

int detect_hwp_version(const char *str)
{
    if (memcmp(V20SIGNATURE, str, HWPIDLen) == 0)
        return HWP_V20;
    else if (memcmp(V21SIGNATURE, str, HWPIDLen) == 0)
        return HWP_V21;
    else if (memcmp(V30SIGNATURE, str, HWPIDLen) == 0)
        return HWP_V30;
    return 0;
}

// HIODev wrapper

int HWPFile::Open(HStream & stream)
{
    HStreamIODev *hstreamio = new HStreamIODev(stream);

    if (!hstreamio->open())
    {
        delete hstreamio;

        return SetState(HWP_EMPTY_FILE);
    }

    HIODev *pPrev = SetIODevice(hstreamio);
    delete pPrev;

    char idstr[HWPIDLen];

    if (ReadBlock(idstr, HWPIDLen) <= 0
        || HWP_V30 != (version = detect_hwp_version(idstr)))
    {
        return SetState(HWP_UNSUPPORTED_VERSION);
    }
    return HWP_NoError;
}


int HWPFile::State(void) const
{
    return error_code;
}


int HWPFile::SetState(int errcode)
{
    error_code = errcode;
    return error_code;
}


int HWPFile::Read1b(void)
{
    return hiodev ? hiodev->read1b() : -1;
}


int HWPFile::Read2b(void)
{
    return hiodev ? hiodev->read2b() : -1;
}


long HWPFile::Read4b(void)
{
    return hiodev ? hiodev->read4b() : -1;
}


int HWPFile::Read1b(void *ptr, size_t nmemb)
{
    return hiodev ? hiodev->read1b(ptr, nmemb) : 0;
}


int HWPFile::Read2b(void *ptr, size_t nmemb)
{
    return hiodev ? hiodev->read2b(ptr, nmemb) : 0;
}


int HWPFile::Read4b(void *ptr, size_t nmemb)
{
    return hiodev ? hiodev->read4b(ptr, nmemb) : 0;
}


size_t HWPFile::ReadBlock(void *ptr, size_t size)
{
    return hiodev ? hiodev->readBlock(ptr, size) : 0;
}


size_t HWPFile::SkipBlock(size_t size)
{
    return hiodev ? hiodev->skipBlock(size) : 0;
}


bool HWPFile::SetCompressed(bool flag)
{
    return hiodev ? hiodev->setCompressed(flag) : false;
}


HIODev *HWPFile::SetIODevice(HIODev * new_hiodev)
{
    HIODev *old_hiodev = hiodev;

    hiodev = new_hiodev;

    return old_hiodev;
}


// end of HIODev wrapper

bool HWPFile::InfoRead(void)
{
    return _hwpInfo.Read(*this);
}


bool HWPFile::FontRead(void)
{
    return _hwpFont.Read(*this);
}


bool HWPFile::StyleRead(void)
{
    return _hwpStyle.Read(*this);
}


bool HWPFile::ParaListRead(void)
{
    return ReadParaList(plist);
}

bool HWPFile::ReadParaList(std::list < HWPPara* > &aplist, unsigned char flag)
{
    HWPPara *spNode = new HWPPara;
     unsigned char tmp_etcflag;
     unsigned char prev_etcflag = 0;
    while (spNode->Read(*this, flag))
    {
         if( !(spNode->etcflag & 0x04) ){
          tmp_etcflag = spNode->etcflag;
          spNode->etcflag = prev_etcflag;
          prev_etcflag = tmp_etcflag;
         }
        if (spNode->nch && spNode->reuse_shape)
        {
            if (!aplist.empty()){
                     spNode->pshape = aplist.back()->pshape;
                }
                else{
                     spNode->nch = 0;
                     spNode->reuse_shape = 0;
                }
        }
          spNode->pshape.pagebreak = spNode->etcflag;
          if( spNode->nch )
                AddParaShape( &spNode->pshape );

        if (!aplist.empty())
            aplist.back()->SetNext(spNode);
        aplist.push_back(spNode);
        spNode = new HWPPara;
    }
    delete spNode;

    return true;
}


bool HWPFile::TagsRead(void)
{
    ulong tag;
    long size;

    while (1)
    {
        tag = Read4b();
        size = Read4b();
        if (size <= 0 && tag > 0){
            continue;
          }

        if (tag == FILETAG_END_OF_COMPRESSED ||
            tag == FILETAG_END_OF_UNCOMPRESSED)
            return true;
        switch (tag)
        {
            case FILETAG_EMBEDDED_PICTURE:
            {
                EmPicture *emb = new EmPicture(size);

                if (true == emb->Read(*this))
                    emblist.push_back(emb);
                else
                    delete emb;
            }
            break;
            case FILETAG_OLE_OBJECT:
                if (oledata)
                    delete oledata;
                oledata = new OlePicture(size);
                oledata->Read(*this);
                break;
            case FILETAG_HYPERTEXT:
            {
                if( (size % 617) != 0 )
                    SkipBlock( size );
                else
                    for( int i = 0 ; i < size/617 ; i++)
                {
                    HyperText *hypert = new HyperText;
                    hypert->Read(*this);
                    hyperlist.push_back(hypert);
                }
                break;
            }
                case 6:
                {
                     ReadBlock(_hwpInfo.back_info.reserved1, 8);
                     _hwpInfo.back_info.luminance = Read4b();
                     _hwpInfo.back_info.contrast = Read4b();
                     _hwpInfo.back_info.effect = sal::static_int_cast<char>(Read1b());
                     ReadBlock(_hwpInfo.back_info.reserved2, 7);
                     ReadBlock(_hwpInfo.back_info.filename, 260);
                     ReadBlock(_hwpInfo.back_info.color, 3);
                     unsigned short nFlag = sal::static_int_cast<unsigned short>(Read2b());
                     _hwpInfo.back_info.flag = nFlag >> 8 ;
                     int nRange = Read4b();
                     _hwpInfo.back_info.range = nRange >> 24;
                     ReadBlock(_hwpInfo.back_info.reserved3, 27);
                     _hwpInfo.back_info.size = Read4b();

                     _hwpInfo.back_info.data = new char[(unsigned int)_hwpInfo.back_info.size];
                     ReadBlock(_hwpInfo.back_info.data, _hwpInfo.back_info.size);

                     if( _hwpInfo.back_info.size > 0 )
                          _hwpInfo.back_info.type = 2;
                     else if( _hwpInfo.back_info.filename[0] )
                          _hwpInfo.back_info.type = 1;
                     else
                          _hwpInfo.back_info.type = 0;


                     _hwpInfo.back_info.isset = true;

                     break;
                }
            case FILETAG_PRESENTATION:
            case FILETAG_PREVIEW_IMAGE:
            case FILETAG_PREVIEW_TEXT:
            default:
                SkipBlock(size);
        }
    }
}


ColumnDef *HWPFile::GetColumnDef(int num)
{
    std::list<ColumnInfo*>::iterator it = columnlist.begin();

    for(int i = 0; it != columnlist.end() ; ++it, i++){
        if( i == num )
	  break;
    }

    if( it != columnlist.end() )
        return (*it)->coldef;
    else
        return 0;
}
/* @return �ε����� 1���� �����Ѵ�. */
int HWPFile::GetPageMasterNum(int page)
{
    std::list<ColumnInfo*>::iterator it = columnlist.begin();
    ColumnInfo *now = 0;
    int i;

    for( i = 1 ; it != columnlist.end() ; ++it, i++){
        now = *it;
        if( page < now->start_page )
            return i-1;
    }
    return i-1;
}

HyperText *HWPFile::GetHyperText()
{
    std::list<HyperText*>::iterator it = hyperlist.begin();

    for( int i = 0; it != hyperlist.end(); ++it, i++ ){
	if( i == currenthyper )
	  break;
    }

    currenthyper++;
    return *it;
}

EmPicture *HWPFile::GetEmPicture(Picture * pic)
{
    char *name = pic->picinfo.picembed.embname;

    name[0] = 'H';
    name[1] = 'W';
    name[2] = 'P';

    std::list < EmPicture* >::iterator it = emblist.begin();
    for (; it != emblist.end(); ++it)
        if (strcmp(name, (*it)->name) == 0)
            return *it;
    return 0;
}

EmPicture *HWPFile::GetEmPictureByName(char * name)
{
    name[0] = 'H';
    name[1] = 'W';
    name[2] = 'P';

    std::list < EmPicture* >::iterator it = emblist.begin();
    for (; it != emblist.end(); ++it)
        if (strcmp(name, (*it)->name) == 0)
            return *it;
    return 0;
}


void HWPFile::AddBox(FBox * box)
{
// LATER if we don't use box->next(),
// AddBox() and GetBoxHead() are useless;
    if (!blist.empty())
    {
        box->prev = blist.back();
        box->prev->next = box;
    }
    else
        box->prev = 0;
    blist.push_back(box);
}


ParaShape *HWPFile::getParaShape(int index)
{
    std::list<ParaShape*>::iterator it = pslist.begin();

    for( int i = 0; it != pslist.end(); ++it, i++ ){
	if( i == index )
	  break;
    }

    return *it;
}


CharShape *HWPFile::getCharShape(int index)
{
    std::list<CharShape*>::iterator it = cslist.begin();

    for( int i = 0; it != cslist.end(); ++it, i++ ){
	if( i == index )
	  break;
    }

    return *it;
}


FBoxStyle *HWPFile::getFBoxStyle(int index)
{
    std::list<FBoxStyle*>::iterator it = fbslist.begin();

    for( int i = 0; it != fbslist.end(); ++it, i++ ){
	if( i == index )
	  break;
    }

    return *it;
}

DateCode *HWPFile::getDateCode(int index)
{
    std::list<DateCode*>::iterator it = datecodes.begin();

    for( int i = 0; it != datecodes.end(); ++it, i++ ){
	if( i == index )
	  break;
    }

    return *it;
}

HeaderFooter *HWPFile::getHeaderFooter(int index)
{
    std::list<HeaderFooter*>::iterator it = headerfooters.begin();

    for( int i = 0; it != headerfooters.end(); ++it, i++ ){
	if( i == index )
	  break;
    }

    return *it;
}

ShowPageNum *HWPFile::getPageNumber(int index)
{
    std::list<ShowPageNum*>::iterator it = pagenumbers.begin();

    for( int i = 0; it != pagenumbers.end(); ++it, i++ ){
	if( i == index )
	  break;
    }

    return *it;

}

Table *HWPFile::getTable(int index)
{
    std::list<Table*>::iterator it = tables.begin();

    for( int i = 0; it != tables.end(); ++it, i++ ){
	if( i == index )
	  break;
    }

    return *it;
}

void HWPFile::AddParaShape(ParaShape * pshape)
{
    int nscount = 0;
    for(int j = 0 ; j < MAXTABS-1 ; j++)
    {
          if( j > 0 && pshape->tabs[j].position == 0 )
                break;
          if( pshape->tabs[0].position == 0 ){
                if( pshape->tabs[j].type || pshape->tabs[j].dot_continue ||
                     (pshape->tabs[j].position != 1000 *j) )
                          nscount = j;
          }
          else{
                if( pshape->tabs[j].type || pshape->tabs[j].dot_continue ||
                     (pshape->tabs[j].position != 1000 * (j + 1)) )
                          nscount = j;
          }
    }
    if( nscount )
        pshape->tabs[MAXTABS-1].type = sal::static_int_cast<char>(nscount);
     int value = compareParaShape(pshape);
    if( value == 0 || nscount )
    {
        pshape->index = ++pcount;
        pslist.push_back(pshape);
    }
    else
        pshape->index = value;
}


void HWPFile::AddCharShape(CharShape * cshape)
{
    int value = compareCharShape(cshape);
    if( value == 0 )
    {
        cshape->index = ++ccount;
        cslist.push_back(cshape);
    }
    else
        cshape->index = value;
}

void HWPFile::AddColumnInfo()
{
    ColumnInfo *cinfo = new ColumnInfo(m_nCurrentPage);
    columnlist.push_back(cinfo);
    setMaxSettedPage();
}

void HWPFile::SetColumnDef(ColumnDef *coldef)
{
    ColumnInfo *cinfo = columnlist.back();
    if( cinfo->bIsSet )
        return;
    cinfo->coldef = coldef;
    cinfo->bIsSet = true;
}

void HWPFile::AddDateFormat(DateCode * hbox)
{
    hbox->key = sal::static_int_cast<char>(++datecodecount);
    datecodes.push_back(hbox);
}

void HWPFile::AddPageNumber(ShowPageNum * hbox)
{
    pagenumbers.push_back(hbox);
}

void HWPFile::AddHeaderFooter(HeaderFooter * hbox)
{
    headerfooters.push_back(hbox);
}

void HWPFile::AddTable(Table * hbox)
{
    tables.push_back(hbox);
}

void HWPFile::AddFBoxStyle(FBoxStyle * fbstyle)
{
    fbslist.push_back(fbstyle);
}

int HWPFile::compareCharShape(CharShape *shape)
{
    int count = cslist.size();
    if( count > 0 )
    {
        CharShape *cshape=0;
        for(int i = 0; i< count; i++)
        {
            cshape = getCharShape(i);

            if( shape->size == cshape->size &&
                shape->font[0] == cshape->font[0] &&
                shape->ratio[0] == cshape->ratio[0] &&
                shape->space[0] == cshape->space[0] &&
                shape->color[1] == cshape->color[1] &&
                shape->color[0] == cshape->color[0] &&
                shape->shade == cshape->shade &&
                shape->attr == cshape->attr )
            {
                return cshape->index;
            }
        }
    }
    return 0;
}


int HWPFile::compareParaShape(ParaShape *shape)
{
    int count = pslist.size();
    if( count > 0 )
    {
        ParaShape *pshape=0;
        for(int i = 0; i< count; i++)
        {
            pshape = getParaShape(i);
            if( shape->left_margin == pshape->left_margin &&
                shape->right_margin == pshape->right_margin &&
                shape->pspacing_prev == pshape->pspacing_prev &&
                shape->pspacing_next == pshape->pspacing_next &&
                shape->indent == pshape->indent &&
                shape->lspacing == pshape->lspacing &&
                shape->arrange_type == pshape->arrange_type &&
                shape->outline == pshape->outline  &&
                     shape->pagebreak == pshape->pagebreak)
            {
                    if( shape->cshape->size == pshape->cshape->size &&
                         shape->cshape->font[0] == pshape->cshape->font[0] &&
                         shape->cshape->ratio[0] == pshape->cshape->ratio[0] &&
                         shape->cshape->space[0] == pshape->cshape->space[0] &&
                         shape->cshape->color[1] == pshape->cshape->color[1] &&
                         shape->cshape->color[0] == pshape->cshape->color[0] &&
                         shape->cshape->shade == pshape->cshape->shade &&
                         shape->cshape->attr == pshape->cshape->attr )
                    {
                         return pshape->index;
                    }
            }
        }
    }
    return 0;
}


HWPFile *GetCurrentDoc(void)
{
    return HWPFile::cur_doc;
}


HWPFile *SetCurrentDoc(HWPFile * hwpfp)
{
    HWPFile *org = HWPFile::cur_doc;

    HWPFile::cur_doc = hwpfp;
    return org;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
