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


package par2script::files;

use par2script::exiter;

############################################
# File Operations
############################################

sub check_file
{
    my ($arg) = @_;

    if(!( -f $arg ))
    {
        par2script::exiter::exit_program("ERROR: Cannot find file $arg", "check_file");
    }
}

sub read_file
{
    my ($localfile) = @_;

    my @localfile = ();

    open( IN, "<$localfile" ) || par2script::exiter::exit_program("ERROR: Cannot open file: $localfile", "read_file");
    while ( <IN> ) { push(@localfile, $_); }
    close( IN );

    return \@localfile;
}

###########################################
# Saving files
###########################################

sub save_file
{
    my ($savefile, $savecontent) = @_;
    open( OUT, ">$savefile" );
    print OUT @{$savecontent};
    close( OUT);
    if (! -f $savefile) { pre2par::exiter::exit_program("ERROR: Cannot write file: $savefile", "save_file"); }
}

1;
