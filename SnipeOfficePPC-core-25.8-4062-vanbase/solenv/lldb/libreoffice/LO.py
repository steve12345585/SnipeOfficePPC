# -*- tab-width: 4; indent-tabs-mode: nil; py-indent-offset: 4 -*-
# Version: MPL 1.1 / GPLv3+ / LGPLv3+
#
# The contents of this file are subject to the Mozilla Public License Version
# 1.1 (the "License"); you may not use this file except in compliance with
# the License or as specified alternatively below. You may obtain a copy of
# the License at http://www.mozilla.org/MPL/
#
# Software distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
# for the specific language governing rights and limitations under the
# License.
#
# Major Contributor(s):
# Copyright (C) 2012 Tor Lillqvist <tml@iki.fi> (initial developer)
# Copyright (C) 2012 SUSE Linux http://suse.com (initial developer's employer)
#
# All Rights Reserved.
#
# For minor contributions see the git repository.
#
# Alternatively, the contents of this file may be used under the terms of
# either the GNU General Public License Version 3 or later (the "GPLv3+"), or
# the GNU Lesser General Public License Version 3 or later (the "LGPLv3+"),
# in which case the provisions of the GPLv3+ or the LGPLv3+ are applicable
# instead of those above.

# To use, add something like this to your ~/.lldbinit:

# script sys.path[:0] = [os.path.expanduser('~/lo/ios/solenv/lldb/libreoffice')]
# script import LO
# type summary add --python-function LO.rtluString_summary rtl_uString
# type summary add --python-function LO.rtl_OUString_summary rtl::OUString

import lldb

def rtluString_summary(rtlustring, dict):
    e = lldb.SBError()

    length = rtlustring.GetChildMemberWithName('length').GetValueAsUnsigned(0)
    buffer = rtlustring.GetChildMemberWithName('buffer')

    s = '"'
    i = 0
    while i < length:
        c = buffer.GetPointeeData(i, 1).GetUnsignedInt16(e, 0)
        if c == ord('"'):
            s = s + '\\"'
        elif c == ord('\\'):
            s = s + '\\\\'
        elif c == ord('\n'):
            s = s + '\\n'
        elif c == ord('\r'):
            s = s + '\\r'
        elif c == ord('\t'):
            s = s + '\\t'
        elif c < ord(' '):
            s = s + '\\{:03o}'.format(c)
        elif c < 127:
            s = s + chr(c)
        else:
            s = s + '\\u{:04x}'.format(c)
        i = i + 1
    s = s + '"'

    return s

def rtl_OUString_summary(rtloustring, dict):
    return rtluString_summary(rtloustring.GetChildMemberWithName('pData'), dict)

# vim:set shiftwidth=4 softtabstop=4 expandtab:
