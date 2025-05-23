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

/**
 *
 * @file ttcr.h
 * @brief TrueType font creator
 * @author Alexander Gelfenbain
 */

#ifndef __TTCR_H
#define __TTCR_H

#include "sft.hxx"

namespace vcl
{
    typedef struct _TrueTypeCreator TrueTypeCreator;

/* TrueType data types */
    typedef struct {
        sal_uInt16 aw;
        sal_Int16  lsb;
    } longHorMetrics;

/* A generic base class for all TrueType tables */
    struct TrueTypeTable {
        sal_uInt32  tag;                         /* table tag                                                */
        sal_uInt8   *rawdata;                    /* raw data allocated by GetRawData_*()                     */
        void        *data;                       /* table specific data                                      */
    };

/** Error codes for most functions */
    enum TTCRErrCodes {
        TTCR_OK = 0,                        /**< no error                                               */
        TTCR_ZEROGLYPHS = 1,                /**< At least one glyph should be defined                   */
        TTCR_UNKNOWN = 2,                   /**< Unknown TrueType table                                 */
        TTCR_GLYPHSEQ = 3,                  /**< Glyph IDs are not sequential in the glyf table         */
        TTCR_NONAMES = 4,                   /**< 'name' table does not contain any names                */
        TTCR_NAMETOOLONG = 5,               /**< 'name' table is too long (string data > 64K)           */
        TTCR_POSTFORMAT = 6                 /**< unsupported format of a 'post' table                   */
    };

/* ============================================================================
 *
 * TrueTypeCreator methods
 *
 * ============================================================================ */

/**
 * TrueTypeCreator constructor.
 * Allocates all internal structures.
 */
    void TrueTypeCreatorNewEmpty(sal_uInt32 tag, TrueTypeCreator **_this);

/**
 * Adds a TrueType table to the TrueType creator.
 * SF_TABLEFORMAT value.
 * @return value of SFErrCodes type
 */
    int AddTable(TrueTypeCreator *_this, TrueTypeTable *table);

/**
 * Removes a TrueType table from the TrueType creator if it is stored there.
 * It also calls a TrueTypeTable destructor.
 * Note: all generic tables (with tag 0) will be removed if this function is
 * called with the second argument of 0.
 * @return value of SFErrCodes type
 */
    void RemoveTable(TrueTypeCreator *_this, sal_uInt32 tag);



/**
 * Writes a TrueType font generated by the TrueTypeCreator to a segment of
 * memory that this method allocates. When it is not needed anymore the caller
 * is supposed to call free() on it.
 * @return value of SFErrCodes type
 */
    int StreamToMemory(TrueTypeCreator *_this, sal_uInt8 **ptr, sal_uInt32 *length);

/**
 * Writes a TrueType font  generated by the TrueTypeCreator to a file
 * @return value of SFErrCodes type
 */
    int StreamToFile(TrueTypeCreator *_this, const char* fname);


/* ============================================================================
 *
 * TrueTypeTable methods
 *
 * ============================================================================ */


/**
 * This function converts the data of a TrueType table to a raw array of bytes.
 * It may allocates the memory for it and returns the size of the raw data in bytes.
 * If memory is allocated it does not need to be freed by the caller of this function,
 * since the pointer to it is stored in the TrueTypeTable and it is freed by the destructor
 * @return TTCRErrCode
 *
 */

    int GetRawData(TrueTypeTable *, sal_uInt8 **ptr, sal_uInt32 *len, sal_uInt32 *tag);

/**
 *
 * Creates a new raw TrueType table. The difference between this constructor and
 * TrueTypeTableNew_tag constructors is that the latter create structured tables
 * while this constructor just copies memory pointed to by ptr to its buffer
 * and stores its length. This constructor is suitable for data that is not
 * supposed to be processed in any way, just written to the resulting TTF file.
 */
    TrueTypeTable *TrueTypeTableNew(sal_uInt32 tag,
                                    sal_uInt32 nbytes,
                                    const sal_uInt8* ptr);

/**
 * Creates a new 'head' table for a TrueType font.
 * Allocates memory for it. Since a lot of values in the 'head' table depend on the
 * rest of the tables in the TrueType font this table should be the last one added
 * to the font.
 */
    TrueTypeTable *TrueTypeTableNew_head(sal_uInt32 fontRevision,
                                         sal_uInt16 flags,
                                         sal_uInt16 unitsPerEm,
                                         const sal_uInt8  *created,
                                         sal_uInt16 macStyle,
                                         sal_uInt16 lowestRecPPEM,
                                         sal_Int16  fontDirectionHint);

/**
 * Creates a new 'hhea' table for a TrueType font.
 * Allocates memory for it and stores it in the hhea pointer.
 */
    TrueTypeTable *TrueTypeTableNew_hhea(sal_Int16  ascender,
                                         sal_Int16  descender,
                                         sal_Int16  linegap,
                                         sal_Int16  caretSlopeRise,
                                         sal_Int16  caretSlopeRun);

/**
 * Creates a new empty 'loca' table for a TrueType font.
 *
 * INTERNAL: gets called only from ProcessTables();
 */
    TrueTypeTable *TrueTypeTableNew_loca(void);

/**
 * Creates a new 'maxp' table based on an existing maxp table.
 * If maxp is 0, a new empty maxp table is created
 * size specifies the size of existing maxp table for
 * error-checking purposes
 */
    TrueTypeTable *TrueTypeTableNew_maxp( const sal_uInt8* maxp, int size);

/**
 * Creates a new empty 'glyf' table.
 */
    TrueTypeTable *TrueTypeTableNew_glyf(void);

/**
 * Creates a new empty 'cmap' table.
 */
    TrueTypeTable *TrueTypeTableNew_cmap(void);

/**
 * Creates a new 'name' table. If n != 0 the table gets populated by
 * the Name Records stored in the nr array. This function allocates
 * memory for its own copy of NameRecords, so nr array has to
 * be explicitly deallocated when it is not needed.
 */
    TrueTypeTable *TrueTypeTableNew_name(int n, NameRecord *nr);

/**
 * Creates a new 'post' table of one of the supported formats
 */
    TrueTypeTable *TrueTypeTableNew_post(sal_uInt32 format,
                                         sal_uInt32 italicAngle,
                                         sal_Int16 underlinePosition,
                                         sal_Int16 underlineThickness,
                                         sal_uInt32 isFixedPitch);


/*------------------------------------------------------------------------------
 *
 *  Table manipulation functions
 *
 *------------------------------------------------------------------------------*/


/**
 * Add a character/glyph pair to a cmap table
 */
    void cmapAdd(TrueTypeTable *, sal_uInt32 id, sal_uInt32 c, sal_uInt32 g);

/**
 * Add a glyph to a glyf table.
 *
 * @return glyphID of the glyph in the new font
 *
 * NOTE: This function does not duplicate GlyphData, so memory will be
 * deallocated in the table destructor
 */
    sal_uInt32 glyfAdd(TrueTypeTable *, GlyphData *glyphdata, TrueTypeFont *fnt);

/**
 * Query the number of glyphs currently stored in the 'glyf' table
 *
 */
    sal_uInt32 glyfCount(const TrueTypeTable *);

/**
 * Add a Name Record to a name table.
 * NOTE: This function duplicates NameRecord, so the argument
 * has to be deallocated by the caller (unlike glyfAdd)
 */
    void nameAdd(TrueTypeTable *, NameRecord *nr);

} // namespace


extern "C"
{
/**
 * Destructor for the TrueTypeTable object.
 */
 void TrueTypeTableDispose(vcl::TrueTypeTable *);

/**
 * TrueTypeCreator destructor. It calls destructors for all TrueTypeTables added to it.
 */
 void TrueTypeCreatorDispose(vcl::TrueTypeCreator *_this);
}

#endif /* __TTCR_H */

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
