#
# This file is part of the SnipeOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file incorporates work covered by the following license notice:
#
#   Licensed to the Apache Software Foundation (ASF) under one or more
#   contributor license agreements. See the NOTICE file distributed
#   with this work for additional information regarding copyright
#   ownership. The ASF licenses this file to you under the Apache
#   License, Version 2.0 (the "License"); you may not use this file
#   except in compliance with the License. You may obtain a copy of
#   the License at http://www.apache.org/licenses/LICENSE-2.0 .
#

PRJ=..$/..

PRJNAME=crashrep
TARGET=crashrep
TARGETTYPE=CUI

ENABLE_EXCEPTIONS=TRUE
LIBTARGET=NO
LIBSALCPPRT=$(0)

# --- Settings -----------------------------------------------------

.INCLUDE :  settings.mk

# ------------------------------------------------------------------

# Only build crash reporter if either a product build with debug info
# or a non-pro build is done.

.IF "$(ENABLE_CRASHDUMP)" != "" || "$(PRODUCT)" == ""

SOLARLIB!:=$(SOLARLIB:s/jre/jnore/)

OBJFILES=\
    $(OBJ)$/main.obj

APP1NOSAL=TRUE
APP1TARGET=$(TARGET)
APP1OBJS=$(OBJFILES)
APP1RPATH=OOO

.IF "$(OS)" != "FREEBSD" && "$(OS)" != "MACOSX" && "$(OS)"!="NETBSD" && \
    "$(OS)" != "DRAGONFLY"
APP1STDLIBS+=-ldl -lnsl
.ENDIF
.IF "$(OS)" == "SOLARIS"
APP1STDLIBS+=-lsocket
.ENDIF

.ENDIF #  "$(ENABLE_CRASHDUMP)" != "" || "$(PRODUCT)" == ""

# --- Targets ------------------------------------------------------

.INCLUDE :	target.mk

$(OBJ)$/main.obj: $(INCCOM)$/_version.h

# Building crash_report
