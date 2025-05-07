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

package installer::windows::language;

use installer::exiter;

####################################################
# Determining the Windows language (LCID)
# English: 1033
####################################################

sub get_windows_language
{
    my ($language) = @_;

    my $windowslanguage = "";

    if ( $installer::globals::msilanguage->{$language} ) { $windowslanguage = $installer::globals::msilanguage->{$language}; }

    if ( $windowslanguage eq "" ) { installer::exiter::exit_program("ERROR: Unknown language $language in function get_windows_language", "get_windows_language"); }

    return $windowslanguage;
}

####################################################
# Determining the Windows language ANSI-Codepage
# English: 1252
####################################################

sub get_windows_encoding
{
    my ($language) = @_;

    my $windowsencoding = "";

    if ( $installer::globals::msiencoding->{$language} ) { $windowsencoding = $installer::globals::msiencoding->{$language}; }

    if ( $windowsencoding eq "" ) { $windowsencoding = "0"; }   # setting value, if the language is not listed in the encodinglist

    if ( $windowsencoding eq "0" ) { $windowsencoding = "65001"; }  # languages with "0" have to be available in UTF-8 (65001)

    # Asian multilingual installation sets need a code neutral Windows Installer database -> $windowsencoding = 0
    if (( $language eq "en-US" ) && (( $installer::globals::product =~ /suitemulti/i ) || ( $installer::globals::product =~ /officemulti/i ) || ( $installer::globals::product =~ /c05office/i ) || ( $installer::globals::added_english ))) { $windowsencoding = "0"; }

    return $windowsencoding;
}

1;
