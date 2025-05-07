/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/




#include "op.h"
#include "optab.h"


// Bearbeitungsfunktion sal_Char *X( sal_Char * )
OPCODE_FKT pOpFkt[ FKT_LIMIT ] =
{                       //  Code
    OP_BOF,             //    0
    OP_EOF,             //    1
    NI,                 //    2
    NI,                 //    3
    NI,                 //    4
    NI,                 //    5
    NI,                 //    6
    OP_Window1,         //    7
    OP_ColumnWidth,     //    8
    NI,                 //    9
    NI,                 //   10
    OP_NamedRange,      //   11
    OP_Blank,           //   12
    OP_Integer,         //   13
    OP_Number,          //   14
    OP_Label,           //   15
    OP_Formula,         //   16
    NI,                 //   17
    NI,                 //   18
    NI,                 //   19
    NI,                 //   20
    NI,                 //   21
    NI,                 //   22
    NI,                 //   23
    NI,                 //   24
    NI,                 //   25
    NI,                 //   26
    NI,                 //   27
    NI,                 //   28
    NI,                 //   29
    NI,                 //   30
    NI,                 //   31
    NI,                 //   32
    NI,                 //   33
    NI,                 //   34
    NI,                 //   35
    NI,                 //   36
    OP_Footer,          //   37
    OP_Header,          //   38
    NI,                 //   39
    OP_Margins,         //   40
    NI,                 //   41
    NI,                 //   42
    NI,                 //   43
    NI,                 //   44
    NI,                 //   45
    NI,                 //   46
    NI,                 //   47
    NI,                 //   48
    NI,                 //   49
    NI,                 //   50
    NI,                 //   51
    NI,                 //   52
    NI,                 //   53
    NI,                 //   54
    NI,                 //   55
    NI,                 //   56
    NI,                 //   57
    NI,                 //   58
    NI,                 //   59
    NI,                 //   60
    NI,                 //   61
    NI,                 //   62
    NI,                 //   63
    NI,                 //   64
    NI,                 //   65
    NI,                 //   66
    NI,                 //   67
    NI,                 //   68
    NI,                 //   69
    NI,                 //   70
    OP_SymphNamedRange, //   71
    NI,                 //   72
    NI,                 //   73
    NI,                 //   74
    NI,                 //   75
    NI,                 //   76
    NI,                 //   77
    NI,                 //   78
    NI,                 //   79
    NI,                 //   80
    NI,                 //   81
    NI,                 //   82
    NI,                 //   83
    NI,                 //   84
    NI,                 //   85
    NI,                 //   86
    NI,                 //   87
    NI,                 //   88
    NI,                 //   89
    NI,                 //   90
    NI,                 //   91
    NI,                 //   92
    NI,                 //   93
    NI,                 //   94
    NI,                 //   95
    NI,                 //   96
    NI,                 //   97
    NI,                 //   98
    NI,                 //   99
    OP_HiddenCols,      //  100
};


OPCODE_FKT pOpFkt123[ FKT_LIMIT123 ] =
{                       //  Code
    OP_BOF123,              //    0
    OP_EOF123,              //    1
    NI,                 //    2
    NI,                 //    3
    NI,                 //    4
    NI,                 //    5
    NI,                 //    6
    NI,         //    7
    NI,     //    8
    NI,                 //    9
    NI,                 //   10
    NI,     //   11
    NI,         //   12
    NI,         //   13
    NI,         //   14
    NI,         //   15
    NI,         //   16
    NI,                 //   17
    NI,                 //   18
    NI,                 //   19
    NI,                 //   20
    NI,                 //   21
    OP_Label123,                //   22
    NI,                 //   23
    NI,                 //   24
    NI,                 //   25
    NI,                 //   26
    OP_CreatePattern123,            //   27
    NI,                 //   28
    NI,                 //   29
    NI,                 //   30
    NI,                 //   31
    NI,                 //   32
    NI,                 //   33
    NI,                 //   34
    OP_SheetName123,    //   35
    NI,                 //   36
    OP_Number123,                   //   37
    OP_Note123,         //   38
    OP_IEEENumber123,                   //   39
    OP_Formula123,          //   40
    NI,                 //   41
    NI,                 //   42
    NI,                 //   43
    NI,                 //   44
    NI,                 //   45
    NI,                 //   46
    NI,                 //   47
    NI,                 //   48
    NI,                 //   49
    NI,                 //   50
    NI,                 //   51
    NI,                 //   52
    NI,                 //   53
    NI,                 //   54
    NI,                 //   55
    NI,                 //   56
    NI,                 //   57
    NI,                 //   58
    NI,                 //   59
    NI,                 //   60
    NI,                 //   61
    NI,                 //   62
    NI,                 //   63
    NI,                 //   64
    NI,                 //   65
    NI,                 //   66
    NI,                 //   67
    NI,                 //   68
    NI,                 //   69
    NI,                 //   70
    NI, //   71
    NI,                 //   72
    NI,                 //   73
    NI,                 //   74
    NI,                 //   75
    NI,                 //   76
    NI,                 //   77
    NI,                 //   78
    NI,                 //   79
    NI,                 //   80
    NI,                 //   81
    NI,                 //   82
    NI,                 //   83
    NI,                 //   84
    NI,                 //   85
    NI,                 //   86
    NI,                 //   87
    NI,                 //   88
    NI,                 //   89
    NI,                 //   90
    NI,                 //   91
    NI,                 //   92
    NI,                 //   93
    NI,                 //   94
    NI,                 //   95
    NI,                 //   96
    NI,                 //   97
    NI,                 //   98
    NI,                 //   99
    NI      //  100
};


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
