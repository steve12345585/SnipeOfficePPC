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
#ifndef _FILERROR_HXX_

namespace fileaccess {

// Error codes used to deliver the resulting exceptions
#define LOWEST_FREE_ERROR                               72

#define TASKHANDLER_NO_ERROR                            0
#define TASKHANDLER_UNSUPPORTED_COMMAND                 1
#define TASKHANDLING_WRONG_SETPROPERTYVALUES_ARGUMENT   2
#define TASKHANDLING_WRONG_GETPROPERTYVALUES_ARGUMENT   3
#define TASKHANDLING_WRONG_OPEN_ARGUMENT                4
#define TASKHANDLING_WRONG_DELETE_ARGUMENT              5
#define TASKHANDLING_WRONG_TRANSFER_ARGUMENT            6
#define TASKHANDLING_WRONG_INSERT_ARGUMENT              7
#define TASKHANDLING_WRONG_CREATENEWCONTENT_ARGUMENT    8
#define TASKHANDLING_UNSUPPORTED_OPEN_MODE              9

#define TASKHANDLING_DELETED_STATE_IN_OPEN_COMMAND     10
#define TASKHANDLING_INSERTED_STATE_IN_OPEN_COMMAND    11

#define TASKHANDLING_OPEN_FILE_FOR_PAGING              12
#define TASKHANDLING_NOTCONNECTED_FOR_PAGING           13
#define TASKHANDLING_BUFFERSIZEEXCEEDED_FOR_PAGING     14
#define TASKHANDLING_IOEXCEPTION_FOR_PAGING            15
#define TASKHANDLING_READING_FILE_FOR_PAGING           16

#define TASKHANDLING_OPEN_FOR_INPUTSTREAM              17
#define TASKHANDLING_OPEN_FOR_STREAM                   18
#define TASKHANDLING_OPEN_FOR_DIRECTORYLISTING         19

#define TASKHANDLING_NOFRESHINSERT_IN_INSERT_COMMAND   22
#define TASKHANDLING_NONAMESET_INSERT_COMMAND          23
#define TASKHANDLING_NOCONTENTTYPE_INSERT_COMMAND      24

#define TASKHANDLING_ANYOTHER_WRITE                    25   // not used
#define TASKHANDLING_NO_OPEN_FILE_FOR_OVERWRITE        26
#define TASKHANDLING_NO_OPEN_FILE_FOR_WRITE            27
#define TASKHANDLING_NOTCONNECTED_FOR_WRITE            28
#define TASKHANDLING_BUFFERSIZEEXCEEDED_FOR_WRITE      29
#define TASKHANDLING_IOEXCEPTION_FOR_WRITE             30
#define TASKHANDLING_FILEIOERROR_FOR_WRITE             31
#define TASKHANDLING_FILEIOERROR_FOR_NO_SPACE          71
#define TASKHANDLING_FILESIZE_FOR_WRITE                32
#define TASKHANDLING_INPUTSTREAM_FOR_WRITE             33
#define TASKHANDLING_NOREPLACE_FOR_WRITE               34
#define TASKHANDLING_ENSUREDIR_FOR_WRITE               35

#define TASKHANDLING_FOLDER_EXISTS_MKDIR               69
#define TASKHANDLING_INVALID_NAME_MKDIR                70
#define TASKHANDLING_CREATEDIRECTORY_MKDIR             36

#define TASKHANDLING_NOSUCHFILEORDIR_FOR_REMOVE        38
#define TASKHANDLING_VALIDFILESTATUS_FOR_REMOVE        39
#define TASKHANDLING_OPENDIRECTORY_FOR_REMOVE          40
#define TASKHANDLING_DELETEFILE_FOR_REMOVE             41
#define TASKHANDLING_DELETEDIRECTORY_FOR_REMOVE        42
#define TASKHANDLING_FILETYPE_FOR_REMOVE               43
#define TASKHANDLING_VALIDFILESTATUSWHILE_FOR_REMOVE   44
#define TASKHANDLING_DIRECTORYEXHAUSTED_FOR_REMOVE     45

#define TASKHANDLING_TRANSFER_ACCESSINGROOT            46
#define TASKHANDLING_TRANSFER_INVALIDSCHEME            47
#define TASKHANDLING_TRANSFER_INVALIDURL               48
#define TASKHANDLING_TRANSFER_DESTFILETYPE             50
#define TASKHANDLING_TRANSFER_BY_MOVE_SOURCE           51
#define TASKHANDLING_TRANSFER_BY_MOVE_SOURCESTAT       52
#define TASKHANDLING_KEEPERROR_FOR_MOVE                53
#define TASKHANDLING_NAMECLASH_FOR_MOVE                54
#define TASKHANDLING_NAMECLASHMOVE_FOR_MOVE            55
#define TASKHANDLING_NAMECLASHSUPPORT_FOR_MOVE         56
#define TASKHANDLING_OVERWRITE_FOR_MOVE                57
#define TASKHANDLING_RENAME_FOR_MOVE                   58
#define TASKHANDLING_RENAMEMOVE_FOR_MOVE               59

#define TASKHANDLING_TRANSFER_BY_COPY_SOURCE           60
#define TASKHANDLING_TRANSFER_BY_COPY_SOURCESTAT       61
#define TASKHANDLING_KEEPERROR_FOR_COPY                62
#define TASKHANDLING_OVERWRITE_FOR_COPY                63
#define TASKHANDLING_RENAME_FOR_COPY                   64
#define TASKHANDLING_RENAMEMOVE_FOR_COPY               65
#define TASKHANDLING_NAMECLASH_FOR_COPY                66
#define TASKHANDLING_NAMECLASHMOVE_FOR_COPY            67
#define TASKHANDLING_NAMECLASHSUPPORT_FOR_COPY         68

}

#endif




/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
