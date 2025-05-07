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

# Add certain symbol patterns to the first global section.
#
# The below code fails with 'perverted' mapfiles (using a strange line layout,
# or containing version UDK_3_0_0 without a global section, ...).

BEGIN { state = 0 }
END {
    if (state == 0) {
        print "# Weak RTTI symbols for C++ exceptions:"
        print "UDK_3_0_0 {"
        print "global:"
        print "_ZTI*; _ZTS*;"
        print "};"
    }
    print "# Unique libstdc++ symbols:"
    print "GLIBCXX_3.4 {"
    print "global:"
    print "_ZGVNSt7num_put*; _ZNSt7num_put*;"
    print "_ZNSs4_Rep20_S_empty_rep_storageE*;"
    print "};"
}
state == 2 {
    print "_ZTI*; _ZTS*; # weak RTTI symbols for C++ exceptions"
    state = 3
}
# #i66636# - ???
/^[\t ]*UDK_3_0_0[\t ]*\{/ { state = 1 }
/^[\t ]*global[\t ]*:/ && state == 1 { state = 2 }
{ print }
