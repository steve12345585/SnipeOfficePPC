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

#ifndef _HBOX_H_
#define _HBOX_H_

#include <list>

#include "hwplib.h"
#include "hwpfile.h"
#include "hinfo.h"
#include "hpara.h"

/**
 * The HBox class is the base class for all date classes in hwp document.
 * For example, there are special character, table, image, etc.
 * It has one character. The ascii code value of special characters are smaller than 32. General character is greater than 32.
 *
 * @short Base class for characters
 */
struct HBox
{
    public:
        hchar hh;

/**
 * Construct a HBox object with parameter hch.
 * @param hch 16bit character being able to have Korean character.
 */
        HBox( hchar hch );
        virtual ~HBox();
/**
 * @returns The Size of HBox object
 */
        virtual int   WSize();
/**
 * @returns The Height of HBox object as hunit value.
 */
        virtual hunit Height(CharShape *csty);
/**
 * Read properties from HIODevice object like stream, file, memory.
 *
 * @param hwpf HWPFile Object having all information for a hwp file.
 * @returns True if reading from stream is successful.
 */
        virtual int   Read(HWPFile &hwpf);

        virtual hchar_string GetString();
    private:
        static int boxCount;
};

/**
 * @short Class for saving data to be skipped.
 */
struct SkipData: public HBox
{
    ulong data_block_len;
    hchar dummy;
    char  *data_block;

    SkipData(hchar);
    virtual ~SkipData();
    virtual int Read(HWPFile &hwpf);
};
struct DateCode;
struct FieldCode : public HBox
{
    uchar type[2];                    /* 2/0 - ����, 3/0-�������, 3/1-��������, 3/2-���糯¥, 4/0-����Ʋ */
    char *reserved1;
    unsigned short location_info;     /* 0 - ���ڵ�, 1 - �����ڵ� */
    char *reserved2;
    hchar *str1;
    hchar *str2;
    hchar *str3;
    char *bin;

     DateCode *m_pDate;

    FieldCode();
    virtual ~FieldCode();
    virtual int Read(HWPFile &hwpf);
};
/**
 * Kind of BOOKMARK
 */
enum
{
    BM_MARK,
    BM_BEGIN,
    BM_END
};

#define BMK_COMMENT_LEN 15

/**
 * @short Class for BOOKMARK
 */
struct Bookmark: public HBox
{
    hchar         dummy;
    hchar         id[BMK_COMMENT_LEN + 1];
    unsigned short    type;

    Bookmark();
    virtual ~Bookmark();
    virtual int Read(HWPFile &hwpf);
};

// date format(7)
const int DATE_SIZE = 40;

/**
 * @short Class for saving date format made by user
 */
struct DateFormat: public HBox
{
    hchar format[DATE_SIZE];
    hchar dummy;

    DateFormat();
    virtual int Read(HWPFile &hwpf);
};

/**
 * @short Class for current date and time with specified format.
 */
struct DateCode: public HBox
{
    enum
    {
        YEAR, MONTH, WEEK, DAY, HOUR, MIN
    };

    hchar format[DATE_SIZE];
/**
 * year/month/week/day/hour/minute
 */
    short date[6];
    hchar dummy;
    unsigned char key;

    DateCode();
    virtual int Read(HWPFile &hwpf);

    virtual hchar_string GetString();
};

/**
 * @short Tab object
 */
struct Tab: public HBox
{
    hunit width;
    unsigned short leader;
    hchar dummy;

    Tab();
    virtual int Read(HWPFile &hwpf);
};

// tbox(10) TABLE BOX MATH BUTTON HYPERTEXT

enum ttype { TBL_TYPE, TXT_TYPE, EQU_TYPE, BUTTON_TYPE, HYPERTEXT_TYPE };
enum                                              /* TxtBox->margin[n][?] */
{
    OUT_M, IN_M, CELL_M
};
enum                                              /* TxtBox->margin[?][n] */
{
    L_M, R_M, T_M, B_M
};
enum anchor { CHAR_ANCHOR, PARA_ANCHOR, PAGE_ANCHOR, PAPER_ANCHOR };
enum { TF_OCCUPY_SPACE, TF_TRANSPARENT,TF_ARROUND_TEXT };
enum
{
    CAP_OUT_BOT,  CAP_OUT_TOP,  CAP_IN_BOT,    CAP_IN_TOP,
    CAP_LEFT_BOT, CAP_LEFT_TOP, CAP_RIGHT_BOT, CAP_RIGHT_TOP,
    EQU_CAP_POS
};

/**
 * Definitions for frame's common 'option' member.
 */
#define DRAW_FRAME      1                         /* <-> no frame, bit 0 */
#define NORMAL_PIC      2                         /* <-> reverse,  bit 1 */
#define FIT_TO_COLUMN   4                         /* fig_xs is columnlen */
#define UNKNOWN_FILE    8                         /* need check reverse for pcx mono */
#define HYPERTEXT_FLAG  16
#define UNKNOWN_SIZE    32
#define FOPT_TEMP_BIT   0x00010000                /* temporary starts from 16th bits */

struct CellLine
{
    unsigned char key;
    unsigned char top;                            // 0 - ���ξ���, 1-single, 2-thick, 3-double
    unsigned char bottom;
    unsigned char left;
    unsigned char right;
    short color;                                  // cell color
    unsigned char shade;                          // <100%
};
/**
 * A cell has four properties to specify the position for itself in hwp.
 * @li xpos -  distance between left border of cell and left border of table
 * @li ypos -  distance between top border of cell and top border of table
 * @li width -  distance between left border of cell and right border of cell
 * @li height -  distance between top border of cell and bottom border of table
 * This is differ from the format of other office in fact cell has two properties
 * - rowindex and column index.
 *
 * @short Cell object
 */
struct Cell                                       // Cell
{
    unsigned char key;                            // Index value of border style
    short     p;
    short     color;                              // cell color
    short     x, y;                               // [x,y] cell pos
    short     w, h;                               // [x,y] cell size
    short     txthigh, cellhigh;                  // used maximum
    unsigned char flag, changed, used;            // unused(file)
    unsigned char ver_align;                      // vertical align  {1=center}
    unsigned char linetype[4];                    // [left,right,top,bottom]
    unsigned char shade;                          // <100%
    unsigned char diagonal;                       // { 0=none,\=1,/=2,X=3}
    unsigned char protect;

    void  Read( HWPFile &hwpf );
};

/**
 * @short Style for floating object
 */
struct FBoxStyle
{
/**
 * Anchor type : paragraph , page, char
 */
    unsigned char anchor_type;
/**
 * Kind of wrap
 */
    unsigned char txtflow;                        /* �׸�����. 0-2(�ڸ�����,����,��︲) */
/**
 * Horizontal alignment
 */
    short     xpos;                               // left, right, center, xx
/**
 * Vertical alignment
 */
    short     ypos;                               // page top, bottom, yy
/**
 * Every margin of border
 * [0-2][] : out/in/cell margin
 * [][0-3] : left/right/top/bottom
 */
    short     margin[3][4];                       // out : left, right, top, bottom
/**
 * Index of floating object
 */
    short     boxnum;                             /* ��Ÿ���ǽ����� ��Ÿ�� �̸����� ���� ���� */
/**
 * Type of floating object : line, txtbox, image, table, equalizer and button
 */
    unsigned char boxtype;                        // (L)ine, t(X)tbox, Picture - (G)
    short     cap_len; /* ĸ���� ���� */

    void *cell;

};

/**
 * This object is for floating object like table, image, line and so on.
 *
 * @short floating object
 */
struct FBox: public HBox
{
    int zorder;
    short     option;                             // draw frame
    hchar     ctrl_ch;
    FBoxStyle style;

    short     box_xs, box_ys;
    short     cap_xs, cap_ys ;
    short     xs, ys;                             // ys = fig_ys + cap_ys + margin
// xs = fig_xs + cap_xs + margin
    short     cap_margin;
    char      xpos_type, ypos_type;
    unsigned char smart_linesp;

/*  �� �ڷ�� tbox�� pic������ ���Ͽ� ������� �ʰ� ����ø� ������,
    line������ ���Ͽ� ����Ѵ�.
 */
    short     boundsy, boundey;
    unsigned char boundx, draw;

/**
 * Physical x,y position.
 */
    short     pgx, pgy;                           // physical xpos, ypos
    short     pgno, showpg;                       // pageno where code is

    FBox      *prev, *next;

    FBox( hchar hch );
    virtual ~FBox();
};

struct Table;
/**
 * The TxtBox class saves object properties about table, textbox, equalizer or button
 */
struct TxtBox: public FBox
{
    hchar     reserved[2];
    hchar     dummy;

    short     dummy1;                             // to not change structure size */
    short     cap_len;
    short     next;
    short     dummy2;                             // to not change structure size */
    unsigned char reserved1;
/**
 * caption position
 */
    short     cap_pos;                            // caption pos
    short     num;                                // numbering

    short     dummy3;
    short     baseline;                           //(for equ)

/**
 * The value of type indicates as the below: zero is table, one is
 * textbox, two is equalizer and three is button.
 */
    short     type;                               // 0-table, 1-textbox, 2-����, 3-button
/**
 * nCell is greater than one only for table, otherwise it is 1.
 */
    short     nCell;                              //:=1    offset 80
/**
 * If value of protect is 1, size of cell cann't change.
 */
    short     protect;                            //1=size lock

    Cell      *cell;
     Table *m_pTable;
/**
 * Paragraph list
 */
    std::list<HWPPara*> *plists;
/**
 * Caption
 */
    std::list<HWPPara*> caption;

    TxtBox();
    virtual ~TxtBox();

/**
 * @returns Count of cell.
 */
    virtual int NCell()   { return nCell; }
/**
 * This is one of table, text-box, equalizer and button
 * @returns Type of this object.
 */
    virtual int Type()    { return type;  }

    virtual int Read(HWPFile &hwpf);

    virtual hunit  Height(CharShape *csty);
};

#define ALLOWED_GAP 5
#define INIT_SIZE 20
#define ADD_AMOUNT 10

struct Columns
{
     int *data;
     size_t nCount;
     size_t nTotal;
     Columns(){
          nCount = 0;
          nTotal = INIT_SIZE;
          data = new int[nTotal];
     }
     ~Columns(){ delete[] data; }

     void AddColumnsSize(){
          int *tmp = data;
          if (nTotal + ADD_AMOUNT < nTotal) // overflow
          {
              throw ::std::bad_alloc();
          }
          data = new int[nTotal + ADD_AMOUNT];
          for (size_t i = 0 ; i < nTotal ; i++)
                data[i] = tmp[i];
          nTotal += ADD_AMOUNT;
          delete[] tmp;
     }

     void insert(int pos){
          if( nCount == 0 ){
                data[nCount++] = pos;
                return;
          }
          for (size_t i = 0 ; i < nCount; i++ ) {
                if( pos < data[i] + ALLOWED_GAP && pos > data[i] - ALLOWED_GAP )
                     return;  // Already exist;
                if( pos < data[i] ){
                     if( nCount == nTotal )
                          AddColumnsSize();
                     for (size_t j = nCount ; j > i ; j--)
                          data[j] = data[j-1];
                     data[i] = pos;
                     nCount++;
                     return;
                }
          }
          // last postion.
          if( nCount == nTotal )
                AddColumnsSize();
          data[nCount++] = pos;
     }

     int getIndex(int pos)
     {
          if( pos == 0 )
                return 0;
          for (size_t i = 0 ; i < nCount; i++) {
                if( pos < data[i] + ALLOWED_GAP && pos > data[i] - ALLOWED_GAP )
                     return i;
          }
          return -1;
     }
};

struct Rows
{
     int *data;
     size_t nCount;
     size_t nTotal;
     Rows(){
          nCount = 0;
          nTotal = INIT_SIZE;
          data = new int[nTotal];
     }
     ~Rows(){ delete[] data; }

     void AddRowsSize(){
          int *tmp = data;
          if (nTotal + ADD_AMOUNT < nTotal) // overflow
          {
              throw ::std::bad_alloc();
          }
          data = new int[nTotal + ADD_AMOUNT];
          for (size_t i = 0 ; i < nTotal ; i++)
                data[i] = tmp[i];
          nTotal += ADD_AMOUNT;
          delete[] tmp;
     }

     void insert(int pos){
          if( nCount == 0 ){
                data[nCount++] = pos;
                return;
          }
          for (size_t i = 0 ; i < nCount; i++) {
                if( pos < data[i] + ALLOWED_GAP && pos > data[i] - ALLOWED_GAP )
                     return;  // Already exist;
                if( pos < data[i] ){
                     if( nCount == nTotal )
                          AddRowsSize();
                     for (size_t j = nCount ; j > i ; j--)
                          data[j] = data[j-1];
                     data[i] = pos;
                     nCount++;
                     return;
                }
          }
          // last postion.
          if( nCount == nTotal )
                AddRowsSize();
          data[nCount++] = pos;
     }

     int getIndex(int pos)
     {
          if( pos == 0 )
                return 0;
          for (size_t i = 0 ; i < nCount; i++) {
                if( pos < data[i] + ALLOWED_GAP && pos > data[i] - ALLOWED_GAP )
                     return i;
          }
          return -1;
     }
};

struct TCell
{
     int nColumnIndex;
     int nRowIndex;
     int nColumnSpan;
     int nRowSpan;
     Cell *pCell;
};

struct Table
{
     Table(){};
     ~Table(){
          std::list<TCell*>::iterator it = cells.begin();
          for( ; it != cells.end(); ++it)
                delete *it;
     };
     Columns columns;
     Rows rows;
    std::list<TCell*> cells;
     TxtBox *box;
};

/* picture (11) �׸�, OLE�׸�, ���Ա׸�, �׸��� */
enum pictype
{
    PICTYPE_FILE, PICTYPE_OLE, PICTYPE_EMBED,
    PICTYPE_DRAW, PICTYPE_UNKNOWN
};
/**
 * @short External image file
 */
struct PicDefFile
{
    char path[256];
    void *img;
    bool skipfind;
};

/**
 * @short Embeded image file
 */
struct PicDefEmbed
{
    char  embname[16];
};

/**
 * @short Win32 ole object
 */
struct PicDefOle
{
    char  embname[16];
    void  *hwpole;
};

/**
 * @short Drawing object of hwp
 */
struct PicDefDraw
{
    void      *hdo;
    uint      zorder;
    ZZRect    vrect;
    int       mbrcnt;
};

/**
 * @short For using common case
 */
struct PicDefUnknown
{
    char  path[256];
};

typedef union
{
    PicDefFile        picfile;
    PicDefEmbed       picembed;
    PicDefOle     picole;
    PicDefDraw        picdraw;
    PicDefUnknown     picun;
} PicDef;

#define PIC_INFO_LEN    348
/**
 * There are four kinds of image.
 * @li External image
 * @li Embeded image
 * @li Win32 ole object
 * @li Drawing object of hwp
 *
 * @short Image object
 */
struct Picture: public FBox
{
    hchar     reserved[2];
    hchar     dummy;
/**
 * follow_block_size is the size information of the Drawing object of hwp.
 * It's value is greater than 0 if the pictype is PICTYPE_DRAW.
 */
    ulong     follow_block_size;                  /* �߰����� ����. */
    short     dummy1;                             // to not change structure size */
    short     dummy2;                             // to not change structure size */
    uchar     reserved1;
/**
 * Position of caption
 */
    short     cap_pos;                            // caption pos
/**
 * Index of current Picture object
 */
    short     num;                                // numbering

/**
 * Type of this object
 * It is one of external/ole/embeded/drawing picture
 */
    uchar     pictype;
    hunit     skip[2];
/**
 * Ratio of magnification or reduction.
 */
    hunit     scale[2];
    PicDef    picinfo;
    char      reserved3[9];

    std::list<HWPPara*> caption;
/**
 * It's for the Drawing object
 */
    unsigned char *follow;                        /* �׸������� drawing�϶�, �߰�����. */

    bool ishyper;

    Picture();
    virtual ~Picture();

    virtual int   Type    ();
    virtual int   Read    (HWPFile &hwpf);

    virtual hunit  Height (CharShape *sty);
};

// line (14)
/**
 * @short Line
 */
struct Line: public FBox
{
    hchar     reserved[2];
    hchar     dummy;

    char      reserved2[8];

    short     sx, sy, ex, ey;
    short     width, shade, color;

    Line();

    virtual int Read(HWPFile &hwpf);
};

// hidden(15)
/**
 * @short Hidden section
 */
struct Hidden: public HBox
{
    hchar     reserved[2];
    hchar     dummy;

    unsigned char info[8];                        // h, next, dummy
    std::list<HWPPara*> plist;

    Hidden();
    virtual ~Hidden();

    virtual int Read(HWPFile &hwpf);
};

/**
 * @short Header or footer
 */
struct HeaderFooter: public HBox
{
    hchar     reserved[2];
    hchar     dummy;

    unsigned char info[8];
/**
 * Header or footer
 */
    unsigned char type;
    unsigned char where;
    unsigned char linenumber;

     unsigned int m_nPageNumber;

/**
 * Paragraph list of header or footer
 */
    std::list<HWPPara*> plist;

    HeaderFooter();
    virtual ~HeaderFooter();

    virtual int Read(HWPFile &hwpf);
};

/**
 * Both footnote and endnote are comment. Footnote is located at the end of paragraph; endnote is located at the end of page. The Footnote class represents footnote and endnote.
 * @short Footnote or endnote
 */
struct Footnote: public HBox
{
    hchar     reserved[2];
    hchar     dummy;

    unsigned char  info[8];
/**
 * The number of current footnote/endnote
 */
    unsigned short number;
/**
 * Set the type of Footnote either footnote or endnote.
 */
    unsigned short type;
/**
 * The width of the Footnote object.
 */
    hunit     width;
/**
 * Paragraph list of Footnote objects
 */
    std::list<HWPPara*> plist;

    Footnote();
    virtual ~Footnote();

    virtual int Read(HWPFile &hwpf);
};

// auto number(18)
/**
 * Kind of auto input number
 */
enum
{
    PGNUM_AUTO,
    FNNUM_AUTO,
    ENNUM_AUTO,
    PICNUM_AUTO,
    TBLNUM_AUTO,
    EQUNUM_AUTO
};

/**
 * @short Input current index of page,comment,table and picture.
 */
struct AutoNum: public HBox
{
    unsigned short type;
    unsigned short number;
    hchar      dummy;

    AutoNum();

    virtual int Read(HWPFile &hwpf);
};

/**
 * @short Input new number as current index of page,comment,table and picture.
 */
struct NewNum: public HBox
{
    unsigned short type;
    unsigned short number;
    hchar      dummy;

    NewNum();

    virtual int Read(HWPFile &hwpf);
};

// page numger(20)
/**
 * @short Input page index in footer or header
 */
struct ShowPageNum: public HBox
{
/**
 * Location of page number to be inserted.
 */
    unsigned short where;
     unsigned int m_nPageNumber;
/**
 * Shape of page number to be inserted.
 */
    unsigned short shape;
    hchar      dummy;

    ShowPageNum();

    virtual int Read(HWPFile &hwpf);
};

/* Ȧ���ʽ��� (21) */
#define HIDE_HD         1                         /* bit 0 */
#define HIDE_FT         2                         /* bit 1 */
#define HIDE_PGNUM      4                         /* bit 2 */
#define HIDE_FRAME      8                         /* bit 3 */
/**
 * Controls the display of page number, header, footer and border.
 */
struct PageNumCtrl: public HBox
{
/**
 * object type
 */
    unsigned short kind;
/**
 * control command.
 */
    unsigned short what;
    hchar      dummy;

    PageNumCtrl();

    virtual int Read(HWPFile &hwpf);
};

// mail merge(22)
/**
 * Generates the mailing list automatically using address book and mail body format.
 * @short Generates mailing list
 */
struct MailMerge: public HBox
{
    unsigned char field_name[20];
    hchar     dummy;

    MailMerge();

    virtual int Read(HWPFile &hwpf);
    virtual hchar_string GetString();
};

// char compositon(23)
/**
 * The compose struct displays characters at position. The maximum character count for composition is three.
 * @short Composition several characters
 */
struct Compose: public HBox
{
    hchar     compose[3];
    hchar     dummy;

    Compose();

    virtual int Read(HWPFile &hwpf);
};

// hyphen(24)
/**
 * @short Hyphen
 */
struct Hyphen: public HBox
{
/**
 * Width of hyphen
 */
    hchar     width;
    hchar     dummy;

    Hyphen();

    virtual int Read(HWPFile &hwpf);
};

// toc mark(25)
/**
 * The TocMark class is for making the content of a table.
 * When you set TocMark on current position, hwp makes it as toc automatically.
 * @short Table of contents
 */
struct TocMark: public HBox
{
    hchar     kind;
    hchar     dummy;

    TocMark();

    virtual int Read(HWPFile &hwpf);
};

// index mark(26)
/**
 * IndexMark marks the table of search.
 * If you set IndexMark at current position, hwp make it as search index.
 * @short Table of search
 */
struct IndexMark: public HBox
{
    hchar     keyword1[60];
    hchar     keyword2[60];
    unsigned short pgno;
    hchar     dummy;

    IndexMark();

    virtual int Read(HWPFile &hwpf);
};

// outline(28)
#define MAX_OUTLINE_LEVEL   7

enum
{
    OLSTY_USER = 0,
    OLSTY_NUMS1 = 1,
    OLSTY_NUMS2 = 2,
    OLSTY_NUMSIG1 = 3,
    OLSTY_NUMSIG2 = 4,
    OLSTY_NUMSIG3 = 5,
    OLSTY_BULUSER = 128,
    OLSTY_BULLET1 = 129,
    OLSTY_BULLET2 = 130,
    OLSTY_BULLET3 = 131,
    OLSTY_BULLET4 = 132,
    OLSTY_BULLET5 = 133
};

// value is in style->userchar[level];
enum
{
    UDO_NUM,
    UDO_UROM,
    UDO_LROM,
    UDO_UENG,
    UDO_LENG,
    UDO_SYLL,
    UDO_JAMO,
    UDO_HANJA,
    UDO_SP_CNUM,
    UDO_SP_CLENG,
    UDO_SP_CSYLL,
    UDO_SP_CJAMO,
    N_UDO
};
/**
 * Number and format of title.
 * @short Number and format of title
 */
class Outline: public HBox
{
    public:
/**
 * kind of numbering format
 */
        unsigned short kind;
        unsigned char  shape;
/**
 * level of number, Ex) The level of 1.3.2.4 is four
 */
        unsigned char  level;
/**
 * value of level
 */
        unsigned short number[MAX_OUTLINE_LEVEL];
/**
 * shape of level
 */
        hchar     user_shape[MAX_OUTLINE_LEVEL];
/**
 * decoration charactor for the level type
 */
        hchar     deco[MAX_OUTLINE_LEVEL][2];     /* ����� ���ǽ� �յ� ���� */
        hchar     dummy;

        Outline();

        virtual int   Read(HWPFile &hwpf);
        hchar_string GetUnicode() const;
};

/* ���� ��ĭ(30) */
/**
 * The Special space to be treated non-space when a string is
 * cut at the end of line
 * @short Special space
 */
struct KeepSpace: public HBox
{
    hchar dummy;

    KeepSpace();

    virtual int Read(HWPFile &hwpf);
};

/* ������ ��ĭ(31) */
/**
 * @short Space with always same width not relation with fonts.
 */
struct FixedSpace: public HBox
{
    hchar     dummy;

    FixedSpace();

    virtual int Read(HWPFile &hwpf);
};
#endif                                            /* _HBOX_H_ */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
