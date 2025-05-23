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

#ifndef _HWPINFO_H_
#define _HWPINFO_H_

#include "hwplib.h"

#define CHAIN_MAX_PATH  40
#define ANNOTATION_LEN  24

class HWPFile;
class CTextOut;
class CHTMLOut;

/**
 * Information of page (phisical)
 */
typedef struct
{
    unsigned char paper_kind;
    unsigned char paper_direction;
    hunit     paper_height;
    hunit     paper_width;
    hunit     top_margin;
    hunit     bottom_margin;
    hunit     left_margin;
    hunit     right_margin;
    hunit     header_length;
    hunit     footer_length;
    hunit     gutter_length;
} PaperInfo;

/* ?????? ??????, ???????? ???? */
typedef struct
{
     char type;  // 0- background color, 1 - external image, 2- embeded image
     char reserved1[8];
     int luminance; /* ???? ( -100 ~ 100 ) */
     int contrast; /* ???? ( -100 ~ 100 ) */
     char effect; /* 0-????????, 1-????????????, 2-???? */
     char reserved2[8];
     char filename[260 + 1]; // filename
     unsigned char color[3]; //0 - red, 1 - green, 2 - blue
     unsigned short flag; /* 0 - ????????, 1 - ????????, 2 - ??????, 3 - ???????? */
     int range; /* 0-????, 1-????????, 3-??????, 4-?????? */
     char reserved3[27];
     int size;
     char *data;        // image data
     bool isset;
} PaperBackInfo;

/* ???????? ???? */
/**
 * Information of printing for chained page
 */
typedef struct
{
    unsigned char chain_page_no;
    unsigned char chain_footnote_no;
    unsigned char chain_filename[CHAIN_MAX_PATH];
} DocChainInfo;

/* ???? ???? */
/**
 * Summary of document
 */
struct HWPSummary
{
    unsigned short    title[56];
    unsigned short    subject[56];
    unsigned short    author[56];
    unsigned short    date[56];
    unsigned short    keyword[2][56];
    unsigned short    etc[3][56];

    bool Read(HWPFile &);
};

/**
  HWPInfo class?? HWPFile?? ???? ?????? ???? ???? ??????????.
  ?????? ???? ???? ????????(layout) ?????? ????????????
  ????????, ???? ???? ?????? ???????? ????. \\

  HWPInfo ???????? HWPFile?? #GetHWPInfo()# ?????? ???? ???? ?? ????.

  @memo Hwp ???? ????
*/
/**
 * Contains the basic information of the hwp document:
 * margin, layout, paper and internal data.
 */
class DLLEXPORT HWPInfo
{
    public:
/**
 * The column index of a document that is saved at last
 */
        short     cur_col;
/**
 * The row index of a document that is saved at last
 */
        short     cur_row;
/**
 * Paper Information
 */
        PaperInfo paper;

          PaperBackInfo back_info;
/**
 * Sets the attribute of read-only or read/write.
 */
        short     readonly;
        unsigned char reserved1[4];
/**
 * Information about document chain
 */
        DocChainInfo  chain_info;
        unsigned char annotation[ANNOTATION_LEN];
        short     encrypted;
// unsigned char    reserved2[6];
        short     beginpagenum;                   /* ?????????? ???? */
/**
 * Information about footnote
 */
        short     beginfnnum;                     /* ???????????? */
        short     countfn;                        /* ???? ???? */
        hunit     splinetext, splinefn;
        hunit     spfnfn;
        unsigned char fnchar;
        unsigned char fnlinetype;
// layout info
/**
 * Information about page layout
 */
        hunit     bordermargin[4];
        short     borderline;

        unsigned char empty_line_hide;
        unsigned char table_move;
        unsigned char compressed;
        unsigned char reserved3;
        short     info_block_len;
/* ?????????? 128 ?????????? */
/**
 * Summary of document
 */
        HWPSummary    summary;
        unsigned char *info_block;

        HWPInfo(void);
        ~HWPInfo(void);

        bool Read(HWPFile &hwpf);
        bool Write(CTextOut &txtf);
        bool Write(CHTMLOut &html);

};

/* ???? ???? ?????? */
/**
 * @short Style of character
 */
struct CharShape
{
/**
 * Index of character style
 */
    int       index;                              /* ???????? ???????? ????. */
/**
 * Font size
 */
    hunit     size;
    unsigned char font[NLanguage];
    unsigned char ratio[NLanguage];
    signed char   space[NLanguage];               /* ???? */
    unsigned char color[2];
    unsigned char shade;
    unsigned char attr;
    unsigned char reserved[4];

    bool Read(HWPFile &);
};

/* ?? ?????? ???? ?????? */

#define MAXTABS 40
/**
 * @short Tab properties
 */
typedef struct
{
    unsigned char type;
    unsigned char dot_continue;
    hunit     position;
} TabSet;

/**
 * @short Column properties
 */
typedef struct
{
    unsigned char ncols;
    unsigned char separator;
    hunit     spacing;
    hunit     columnlen, columnlen0;
} ColumnDef;

/**
 * @short Style of paragraph
 */
struct ParaShape
{
/**
 * Index of paragraph style
 */
    int       index;                              /* ???????? ???????? ???? */
    hunit     left_margin;
    hunit     right_margin;
    hunit     indent;
    hunit     lspacing;
    hunit     pspacing_prev;
    hunit     pspacing_next;
    unsigned char condense;
    unsigned char arrange_type;
    TabSet    tabs[MAXTABS];
    ColumnDef coldef;
    unsigned char shade;
    unsigned char outline;
    unsigned char outline_continue;
    unsigned char reserved[2];
    CharShape *cshape;
     unsigned char pagebreak;

    bool  Read(HWPFile &);
//  virtual ~ParaShape();
};
#endif                                            /* _HWPINFO_H_ */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
