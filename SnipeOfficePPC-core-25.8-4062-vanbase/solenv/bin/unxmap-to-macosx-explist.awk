#
# This file is part of the LibreOffice project.
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

# Generate an exported symbols list out of a map file (as use on Linux/Solaris) in order to
# build shared libraries on Mac OS X
#
# The below code fails may fail with 'perverted' mapfiles (using a strange line layout etc.)

# Skip 'SECTION_NAME {' lines
/^[\t ]*.*[\t ]*\{/ { next }

# Skip 'global:' or 'local:' lines
/global:/ || /local:/ { next }

# Skip '*;' lines
/^[\t ]*\*;[\t ]*/ { next }

# Skip section end '}?;' lines
/^[\t ]*\}[\t ]*.*[;]*/ { next }

# Skip comment or empty lines
/^[\t ]*#.*/ || /^[\t ]*$/ || /^$/ { next }

# Echo all lines containing symbol names and prefix them with '_'
# because symbols on Mac OS X start always with '__'
{
    # There may appear multiple symbols in one line
    # e.g. "sym1; sym2; # and finally a comment"
    # take this into account
    for (i = 1; i <= NF ; i++) {
	if ($i !~ /^[\t ]*#.*/) { # as long as the current field doesn't start with '#'
	    gsub(/[\t ;]/, "", $i) # Remove leading spaces and trailing ';'
	    printf("_%s\n",$i)
	}
	else { # ignore everything after a '#' (comment) sign
	    break
	}
    }
}
