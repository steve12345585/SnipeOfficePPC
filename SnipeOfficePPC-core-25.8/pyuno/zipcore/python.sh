#!/bin/sh
#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
#
# Copyright 2000, 2010 Oracle and/or its affiliates.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# This file is part of OpenOffice.org.
#
# OpenOffice.org is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# OpenOffice.org is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.  If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************

# resolve installation directory
sd_cwd="`pwd`"
if [ -h "$0" ] ; then
    sd_basename=`basename "$0"`
    sd_script=`ls -l "$0" | sed "s/.*${sd_basename} -> //g"`
    cd "`dirname "$0"`"
    cd "`dirname "$sd_script"`"
else
    cd "`dirname "$0"`"
fi
sd_prog=`pwd`
cd "$sd_cwd"

# Set PATH so that crash_report is found:
PATH=$sd_prog${PATH+:$PATH}
export PATH

# Set UNO_PATH so that "officehelper.bootstrap()" can find soffice executable:
: ${UNO_PATH=$sd_prog}
export UNO_PATH

# Set URE_BOOTSTRAP so that "uno.getComponentContext()" bootstraps a complete
# OOo UNO environment:
: ${URE_BOOTSTRAP=vnd.sun.star.pathname:$sd_prog/fundamentalrc}
export URE_BOOTSTRAP

NONMACSECTION
# Set %%OOO_LIBRARY_PATH_VAR%% so that python.bin finds libpython2.6.so (this
# can go once python.bin contains a proper RPATH):
%%OOO_LIBRARY_PATH_VAR%%=$sd_prog${%%OOO_LIBRARY_PATH_VAR%%:+:$%%OOO_LIBRARY_PATH_VAR%%}
export %%OOO_LIBRARY_PATH_VAR%%

PYTHONPATH=$sd_prog:$sd_prog/python-core-%%PYVERSION%%/lib:$sd_prog/python-core-%%PYVERSION%%/lib/lib-dynload:$sd_prog/python-core-%%PYVERSION%%/lib/lib-tk:$sd_prog/python-core-%%PYVERSION%%/lib/site-packages${PYTHONPATH+:$PYTHONPATH}
export PYTHONPATH
PYTHONHOME=$sd_prog/python-core-%%PYVERSION%%
export PYTHONHOME

# execute binary
exec "$sd_prog/python.bin" "$@"
MACSECTION
PYTHONHOME=$sd_prog/OOoPython.framework
export PYTHONHOME

pybasislibdir=$PYTHONHOME/Versions/%%PYVERSION%%/lib/python%%PYVERSION%%
PYTHONPATH=$sd_prog:$pybasislibdir:$pybasislibdir/lib-dynload:$pybasislibdir/lib-tk:$pybasislibdir/site-packages${PYTHONPATH+:$PYTHONPATH}
export PYTHONPATH

# execute binary
exec "$PYTHONHOME/Versions/%%PYVERSION%%/Resources/Python.app/Contents/MacOS/OOoPython" "$@"
