#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
#
# Copyright 2000, 2010 Oracle and/or its affiliates.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# This file is part of SnipeOffice.
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

package installer::worker;

use Cwd;
use File::Copy;
use File::stat;
use File::Temp qw(tmpnam);
use File::Path;
use installer::control;
use installer::converter;
use installer::exiter;
use installer::files;
use installer::globals;
use installer::logger;
use installer::pathanalyzer;
use installer::scpzipfiles;
use installer::scriptitems;
use installer::systemactions;
use installer::windows::language;

#################################################
# Writing some global information into
# the list of files without flag PATCH
#################################################

sub write_nopatchlist_header
{
    my ( $content ) = @_;

    my @header = ();
    my $infoline = "This is a list of files, that are defined in scp-projects without\n";
    push(@header, $infoline);
    $infoline = "flag \"PATCH\". Important: This does not mean in any case, that \n";
    push(@header, $infoline);
    $infoline = "this files are included into or excluded from a patch. \n\n";
    push(@header, $infoline);
    $infoline = "Exception Linux: A patch rpm is a complete rpm. This means that all \n";
    push(@header, $infoline);
    $infoline = "files are included into a patch rpm, if only one file of the rpm has the \n";
    push(@header, $infoline);
    $infoline = "style \"PATCH\". \n\n";
    push(@header, $infoline);

    for ( my $i = 0; $i <= $#header; $i++ ) { push(@{$content},$header[$i]); }
}

#################################################
# Creating the content of the list of files
# without flag PATCH.
# All files are saved in
# @{$installer::globals::nopatchfilecollector}
#################################################

sub create_nopatchlist
{
    my @content =();

    write_nopatchlist_header(\@content);

    for ( my $i = 0; $i <= $#{$installer::globals::nopatchfilecollector}; $i++ )
    {
        my $onefile = ${$installer::globals::nopatchfilecollector}[$i];
        my $oneline = $onefile->{'destination'};
        if ( $onefile->{'zipfilename'} ) { $oneline = $oneline . " (" . $onefile->{'zipfilename'} . ")"; }
        $oneline = $oneline . "\n";
        push(@content, $oneline);
    }

    return \@content;
}

#########################################
# Saving the patchlist file
#########################################

sub _save_patchlist_file
{
    my ($installlogdir, $patchlistfilename) = @_;

    my $installpatchlistdir = installer::systemactions::create_directory_next_to_directory($installlogdir, "patchlist");
    $patchlistfilename =~ s/log\_/patchfiles\_/;
    $patchlistfilename =~ s/\.log/\.txt/;
    installer::files::save_file($installpatchlistdir . $installer::globals::separator . $patchlistfilename, \@installer::globals::patchfilecollector);
    installer::logger::print_message( "... creating patchlist file $patchlistfilename \n" );

    if (( $installer::globals::patch ) && ( ! $installer::globals::creating_windows_installer_patch ))  # only for non-Windows patches
    {
        $patchlistfilename =~ s/patchfiles\_/nopatchfiles\_/;
        my $nopatchlist = create_nopatchlist();
        installer::files::save_file($installpatchlistdir . $installer::globals::separator . $patchlistfilename, $nopatchlist);
        installer::logger::print_message( "... creating patch exclusion file $patchlistfilename \n" );
    }

}

###############################################################
# Removing all directories of a special language
# in the directory $basedir
###############################################################

sub remove_old_installation_sets
{
    my ($basedir) = @_;

    installer::logger::print_message( "... removing old installation directories ...\n" );

    my $removedir = $basedir;

    if ( -d $removedir ) { installer::systemactions::remove_complete_directory($removedir, 1); }

    # looking for non successful old installation sets

    $removedir = $basedir . "_witherror";
    if ( -d $removedir ) { installer::systemactions::remove_complete_directory($removedir, 1); }

    $removedir = $basedir . "_inprogress";
    if ( -d $removedir ) { installer::systemactions::remove_complete_directory($removedir, 1); }

    # finally the $basedir can be created empty

    if ( $installer::globals::localinstalldirset ) { installer::systemactions::create_directory_structure($basedir); }

    installer::systemactions::create_directory($basedir);
}

###############################################################
# Creating the installation directory structure
###############################################################

sub create_installation_directory
{
    my ($shipinstalldir, $languagestringref, $current_install_number_ref) = @_;

    my $installdir = "";

    my $languageref = $languagestringref;

    $installdir = installer::systemactions::create_directories("install", $languageref);
    installer::logger::print_message( "... creating installation set in $installdir ...\n" );
    remove_old_installation_sets($installdir);
    my $inprogressinstalldir = $installdir . "_inprogress";
    installer::systemactions::rename_directory($installdir, $inprogressinstalldir);
    $installdir = $inprogressinstalldir;

    $installer::globals::saveinstalldir = $installdir;  # saving directory globally, in case of exiting

    return $installdir;
}

###############################################################
# Analyzing and creating the log file
###############################################################

sub analyze_and_save_logfile
{
    my ($loggingdir, $installdir, $installlogdir, $allsettingsarrayref, $languagestringref, $current_install_number) = @_;

    my $is_success = 1;
    my $finalinstalldir = "";

    installer::logger::print_message( "... checking log file " . $loggingdir . $installer::globals::logfilename . "\n" );

    my $contains_error = installer::control::check_logfile(\@installer::globals::logfileinfo);

    # Dependent from the success, the installation directory can be renamed.

    if ( $contains_error )
    {
        my $errordir = installer::systemactions::rename_string_in_directory($installdir, "_inprogress", "_witherror");
        # Error output to STDERR
        for ( my $j = 0; $j <= $#installer::globals::errorlogfileinfo; $j++ )
        {
            my $line = $installer::globals::errorlogfileinfo[$j];
            $line =~ s/\s*$//g;
            installer::logger::print_error( $line );
        }
        $is_success = 0;

        $finalinstalldir = $errordir;
    }
    else
    {
        my $destdir = "";

	$destdir = installer::systemactions::rename_string_in_directory($installdir, "_inprogress", "");

        $finalinstalldir = $destdir;
    }

    # Saving the logfile in the log file directory and additionally in a log directory in the install directory

    my $numberedlogfilename = $installer::globals::logfilename;
    installer::logger::print_message( "... creating log file $numberedlogfilename \n" );
    installer::files::save_file($loggingdir . $numberedlogfilename, \@installer::globals::logfileinfo);
    installer::files::save_file($installlogdir . $installer::globals::separator . $numberedlogfilename, \@installer::globals::logfileinfo);

    # Saving the list of patchfiles in a patchlist directory in the install directory
    if (( $installer::globals::patch ) || ( $installer::globals::creating_windows_installer_patch )) { _save_patchlist_file($installlogdir, $numberedlogfilename); }

    if ( $installer::globals::creating_windows_installer_patch ) { $installer::globals::creating_windows_installer_patch = 0; }

    # Exiting the packaging process, if an error occurred.
    # This is important, to get an error code "-1", if an error was found in the log file,
    # that did not break the packaging process

    if ( ! $is_success) { installer::exiter::exit_program("ERROR: Found an error in the logfile. Packaging failed.", "analyze_and_save_logfile"); }

    return ($is_success, $finalinstalldir);
}

###############################################################
# Removing all directories that are saved in the
# global directory @installer::globals::removedirs
###############################################################

sub clean_output_tree
{
    installer::logger::print_message( "... cleaning the output tree ...\n" );

    for ( my $i = 0; $i <= $#installer::globals::removedirs; $i++ )
    {
        if ( -d $installer::globals::removedirs[$i] )
        {
            installer::logger::print_message( "... removing directory $installer::globals::removedirs[$i] ...\n" );
            installer::systemactions::remove_complete_directory($installer::globals::removedirs[$i], 1);
        }
    }

    # Last try to remove the ship test directory

    if ( $installer::globals::shiptestdirectory )
    {
        if ( -d $installer::globals::shiptestdirectory )
        {
            my $infoline = "Last try to remove $installer::globals::shiptestdirectory . \n";
            push(@installer::globals::logfileinfo, $infoline);
            my $systemcall = "rmdir $installer::globals::shiptestdirectory";
            my $returnvalue = system($systemcall);
        }
    }
}

###########################################################
# Setting one language in the language independent
# array of include paths with $(LANG)
###########################################################

sub get_language_specific_include_paths
{
    my ( $patharrayref, $onelanguage ) = @_;

    my @patharray = ();

    for ( my $i = 0; $i <= $#{$patharrayref}; $i++ )
    {
        my $line = ${$patharrayref}[$i];
        $line =~ s/\$\(LANG\)/$onelanguage/g;
        push(@patharray ,$line);
    }

    return \@patharray;
}

##############################################################
# Collecting all items with a defined flag
##############################################################

sub collect_all_items_with_special_flag
{
    my ($itemsref, $flag) = @_;

    my @allitems = ();

    for ( my $i = 0; $i <= $#{$itemsref}; $i++ )
    {
        my $oneitem = ${$itemsref}[$i];
        my $styles = "";
        if ( $oneitem->{'Styles'} ) { $styles = $oneitem->{'Styles'} };

        if ( $styles =~ /\b$flag\b/ )
        {
            push( @allitems, $oneitem );
        }
    }

    return \@allitems;
}

##############################################################
# Collecting all files without patch flag in
# $installer::globals::nopatchfilecollector
##############################################################

sub collect_all_files_without_patch_flag
{
    my ($filesref) = @_;

    my $newfiles = collect_all_items_without_special_flag($filesref, "PATCH");

    for ( my $i = 0; $i <= $#{$newfiles}; $i++ ) { push(@{$installer::globals::nopatchfilecollector}, ${$newfiles}[$i]); }
}

##############################################################
# Collecting all items without a defined flag
##############################################################

sub collect_all_items_without_special_flag
{
    my ($itemsref, $flag) = @_;

    my @allitems = ();

    for ( my $i = 0; $i <= $#{$itemsref}; $i++ )
    {
        my $oneitem = ${$itemsref}[$i];
        my $styles = "";
        if ( $oneitem->{'Styles'} ) { $styles = $oneitem->{'Styles'} };

        if ( !( $styles =~ /\b$flag\b/ ))
        {
            push( @allitems, $oneitem );
        }
    }

    return \@allitems;
}

##############################################################
# Removing all items with a defined flag from collector
##############################################################

sub remove_all_items_with_special_flag
{
    my ($itemsref, $flag) = @_;

    my @allitems = ();

    for ( my $i = 0; $i <= $#{$itemsref}; $i++ )
    {
        my $oneitem = ${$itemsref}[$i];
        my $styles = "";
        if ( $oneitem->{'Styles'} ) { $styles = $oneitem->{'Styles'} };
        if ( $styles =~ /\b$flag\b/ )
        {
            my $infoline = "Attention: Removing from collector: $oneitem->{'Name'} !\n";
            push( @installer::globals::logfileinfo, $infoline);
            if ( $flag eq "BINARYTABLE_ONLY" ) { push(@installer::globals::binarytableonlyfiles, $oneitem); }
            next;
        }
        push( @allitems, $oneitem );
    }

    return \@allitems;
}

###########################################################
# Mechanism for simple installation without packing
###########################################################

sub install_simple ($$$$$$)
{
    my ($packagename, $languagestring, $directoriesarray, $filesarray, $linksarray, $unixlinksarray) = @_;

    installer::logger::print_message( "... installing module $packagename ...\n" );

    my $destdir = $installer::globals::destdir;
    my @lines = ();

    installer::logger::print_message( "DestDir: $destdir \n" );
    installer::logger::print_message( "Rootpath: $installer::globals::rootpath \n" );

    `mkdir -p $destdir` if $destdir ne "";
    `mkdir -p $destdir$installer::globals::rootpath`;

    # Create Directories
    for ( my $i = 0; $i <= $#{$directoriesarray}; $i++ )
    {
        my $onedir = ${$directoriesarray}[$i];
        my $dir = "";

        if ( $onedir->{'Dir'} ) { $dir = $onedir->{'Dir'}; }

        if ((!($dir =~ /\bPREDEFINED_/ )) || ( $dir =~ /\bPREDEFINED_PROGDIR\b/ ))
        {
            my $hostname = $onedir->{'HostName'};

            # ignore '.' subdirectories
            next if ( $hostname =~ m/\.$/ );
            # remove './' from the path
            $hostname =~ s/\.\///g;

            # printf "mkdir $destdir$hostname\n";
            mkdir $destdir . $hostname;
            push @lines, "%dir " . $hostname . "\n";
        }
    }

    for ( my $i = 0; $i <= $#{$filesarray}; $i++ )
    {
        my $onefile = ${$filesarray}[$i];
        my $unixrights = $onefile->{'UnixRights'};
        my $destination = $onefile->{'destination'};
        my $sourcepath = $onefile->{'sourcepath'};

        # This is necessary to install SDK that includes files with $ in its name
        # Otherwise, the following shell commands does not work and the file list
        # is not correct
        $destination =~ s/\$\$/\$/;
        $sourcepath =~ s/\$\$/\$/;

        # remove './' from the path
        $sourcepath =~ s/\.\///g;
        $destination =~ s/\.\///g;

        push @lines, "$destination\n";
        if(-d  "$destdir$destination"){
            rmtree("$destdir$destination");
        }
        if(-e "$destdir$destination") {
            unlink "$destdir$destination";
        }

        copy ("$sourcepath", "$destdir$destination") || die "Can't copy file: $sourcepath -> $destdir$destination $!";
        my $sourcestat = stat($sourcepath);
        utime ($sourcestat->atime, $sourcestat->mtime, "$destdir$destination");
        chmod (oct($unixrights), "$destdir$destination") || die "Can't change permissions: $!";
        push @lines, "$destination\n";
    }

    for ( my $i = 0; $i <= $#{$linksarray}; $i++ )
    {
        my $onelink = ${$linksarray}[$i];
        my $destination = $onelink->{'destination'};
        my $destinationfile = $onelink->{'destinationfile'};

        if(-e "$destdir$destination") {
            unlink "$destdir$destination";
        }
        symlink ("$destinationfile", "$destdir$destination") || die "Can't create symlink: $!";
        push @lines, "$destination\n";
    }

    for ( my $i = 0; $i <= $#{$unixlinksarray}; $i++ )
    {
        my $onelink = ${$unixlinksarray}[$i];
        my $target = $onelink->{'Target'};
        my $destination = $onelink->{'destination'};
        my $cmd = "ln -sf '$target' '$destdir$destination'";

        system($cmd) && die "Failed \"$cmd\"";
        push @lines, "$destination\n";
    }

    if ( $destdir ne "" )
    {
        my $filelist;
        my $fname = $installer::globals::destdir . "/$packagename";
        open ($filelist, ">$fname") || die "Can't open $fname: $!";
        print $filelist @lines;
        close ($filelist);
    }

}

###########################################################
# Selecting patch items
###########################################################

sub select_patch_items
{
    my ( $itemsref, $itemname ) = @_;

    installer::logger::include_header_into_logfile("Selecting items for patches. Item: $itemname");

    my @itemsarray = ();

    for ( my $i = 0; $i <= $#{$itemsref}; $i++ )
    {
        my $oneitem = ${$itemsref}[$i];

        my $name = $oneitem->{'Name'};
        if (( $name =~ /\bLICENSE/ ) || ( $name =~ /\bREADME/ ))
        {
            push(@itemsarray, $oneitem);
            next;
        }

        # Items with style "PATCH" have to be included into the patch
        my $styles = "";
        if ( $oneitem->{'Styles'} ) { $styles = $oneitem->{'Styles'}; }
        if ( $styles =~ /\bPATCH\b/ ) { push(@itemsarray, $oneitem); }
    }

    return \@itemsarray;
}

###########################################################
# Selecting patch items
###########################################################

sub select_patch_items_without_name
{
    my ( $itemsref, $itemname ) = @_;

    installer::logger::include_header_into_logfile("Selecting RegistryItems for patches");

    my @itemsarray = ();

    for ( my $i = 0; $i <= $#{$itemsref}; $i++ )
    {
        my $oneitem = ${$itemsref}[$i];

        # Items with style "PATCH" have to be included into the patch
        my $styles = "";
        if ( $oneitem->{'Styles'} ) { $styles = $oneitem->{'Styles'}; }
        if ( $styles =~ /\bPATCH\b/ ) { push(@itemsarray, $oneitem); }
    }

    return \@itemsarray;
}

###########################################################
# Selecting langpack items
###########################################################

sub select_langpack_items
{
    my ( $itemsref, $itemname ) = @_;

    installer::logger::include_header_into_logfile("Selecting RegistryItems for Language Packs");

    my @itemsarray = ();

    for ( my $i = 0; $i <= $#{$itemsref}; $i++ )
    {
        my $oneitem = ${$itemsref}[$i];

        # Items with style "LANGUAGEPACK" have to be included into the patch
        my $styles = "";
        if ( $oneitem->{'Styles'} ) { $styles = $oneitem->{'Styles'}; }
        if (( $styles =~ /\bLANGUAGEPACK\b/ ) || ( $styles =~ /\bFORCELANGUAGEPACK\b/ )) { push(@itemsarray, $oneitem); }
    }

    return \@itemsarray;
}

###########################################################
# Selecting helppack items
###########################################################

sub select_helppack_items
{
    my ( $itemsref, $itemname ) = @_;

    installer::logger::include_header_into_logfile("Selecting RegistryItems for Help Packs");

    my @itemsarray = ();

    for ( my $i = 0; $i <= $#{$itemsref}; $i++ )
    {
        my $oneitem = ${$itemsref}[$i];

        # Items with style "HELPPACK" have to be included into the patch
        my $styles = "";
        if ( $oneitem->{'Styles'} ) { $styles = $oneitem->{'Styles'}; }
        if (( $styles =~ /\bHELPPACK\b/ ) || ( $styles =~ /\bFORCEHELPPACK\b/ )) { push(@itemsarray, $oneitem); }
    }

    return \@itemsarray;
}

###########################################################
# Searching if LICENSE and README, which are not removed
# in select_patch_items are really needed for the patch.
# If not, they are removed now.
###########################################################

sub analyze_patch_files
{
    my ( $filesref ) = @_;

    installer::logger::include_header_into_logfile("Analyzing patch files");

    my @filesarray = ();

    for ( my $i = 0; $i <= $#{$filesref}; $i++ )
    {
        my $onefile = ${$filesref}[$i];
        my $styles = "";
        if ( $onefile->{'Styles'} ) { $styles = $onefile->{'Styles'}; }
        if ( !( $styles =~ /\bPATCH\b/) ) { next; } # removing all files without flag PATCH (LICENSE, README, ...)

        if ( $installer::globals::iswindowsbuild )
        {
            # all files of the Windows patch belong to the root module
            $onefile->{'modules'} = $installer::globals::rootmodulegid;
        }

        push(@filesarray, $onefile);
    }

    return \@filesarray;
}

###########################################################
# reorganizing the patchfile content,
# sorting for directory to decrease the file size
###########################################################

sub reorg_patchfile
{
    my ($patchfiles, $patchfiledirectories) = @_;

    my @patchfilesarray = ();
    my $line = "";
    my $directory = "";

    # iterating over all directories, writing content into new patchfiles list

    for ( my $i = 0; $i <= $#{$patchfiledirectories}; $i++ )
    {
        $directory = ${$patchfiledirectories}[$i];
        $line = "[" . $directory . "]" . "\n";
        push(@patchfilesarray, $line);

        for ( my $j = 0; $j <= $#{$patchfiles}; $j++ )
        {
            if ( ${$patchfiles}[$j] =~ /^\s*(.*?)\s*\tXXXXX\t\Q$directory\E\s*$/ )
            {
                $line = $1 . "\n";
                push(@patchfilesarray, $line);
            }
        }
    }

    return \@patchfilesarray;
}

###########################################################
# One special file has to be the last in patchfile.txt.
# Controlling this file, guarantees, that all files were
# patch correctly. Using version.ini makes it easy to
# control this by looking into the about box
# -> shifting one section to the end
###########################################################

sub shift_section_to_end
{
    my ($patchfilelist) = @_;

    my @patchfile = ();
    my @lastsection = ();
    my $lastsection = "program";
    my $notlastsection = "Basis\\program";
    my $record = 0;

    for ( my $i = 0; $i <= $#{$patchfilelist}; $i++ )
    {
        my $line = ${$patchfilelist}[$i];

        if (( $record ) && ( $line =~ /^\s*\[/ )) { $record = 0; }

        if (( $line =~ /^\s*\[\Q$lastsection\E\\\]\s*$/ ) && ( ! ( $line =~ /\Q$notlastsection\E\\\]\s*$/ ))) { $record = 1; }

        if ( $record ) { push(@lastsection, $line); }
        else { push(@patchfile, $line); }
    }

    if ( $#lastsection > -1 )
    {
        for ( my $i = 0; $i <= $#lastsection; $i++ )
        {
            push(@patchfile, $lastsection[$i]);
        }
    }

    return \@patchfile;
}

###########################################################
# One special file has to be the last in patchfile.txt.
# Controlling this file, guarantees, that all files were
# patch correctly. Using version.ini makes it easy to
# control this by looking into the about box
# -> shifting one file of the last section to the end
###########################################################

sub shift_file_to_end
{
    my ($patchfilelist) = @_;

    my @patchfile = ();
    my $lastfilename = "version.ini";
    my $lastfileline = "";
    my $foundfile = 0;

    # Only searching this file in the last section
    my $lastsectionname = "";

    for ( my $i = 0; $i <= $#{$patchfilelist}; $i++ )
    {
        my $line = ${$patchfilelist}[$i];
        if ( $line =~ /^\s*\[(.*?)\]\s*$/ ) { $lastsectionname = $1; }
    }

    my $record = 0;
    for ( my $i = 0; $i <= $#{$patchfilelist}; $i++ )
    {
        my $line = ${$patchfilelist}[$i];

        if ( $line =~ /^\s*\[\Q$lastsectionname\E\]\s*$/ ) { $record = 1; }

        if (( $line =~ /^\s*\"\Q$lastfilename\E\"\=/ ) && ( $record ))
        {
            $lastfileline = $line;
            $foundfile = 1;
            $record = 0;
            next;
        }

        push(@patchfile, $line);
    }

    if ( $foundfile ) { push(@patchfile, $lastfileline); }

    return  \@patchfile;
}

###########################################################
# Renaming Windows files in Patch and creating file
# patchfiles.txt
###########################################################

sub prepare_windows_patchfiles
{
    my ( $filesref, $languagestringref, $allvariableshashref ) = @_;

    my @patchfiles = ();
    my %patchfiledirectories = ();
    my $patchfilename = "patchlist.txt";
    my $patchfilename2 = "patchmsi.dll";

    if ( ! $allvariableshashref->{'WINDOWSPATCHLEVEL'} ) { installer::exiter::exit_program("ERROR: No Windows patch level defined in list file (WINDOWSPATCHLEVEL) !", "prepare_windows_patchfiles"); }
    my $windowspatchlevel = $installer::globals::buildid;

    for ( my $i = 0; $i <= $#{$filesref}; $i++ )
    {
        my $onefile = ${$filesref}[$i];

        my $filename = $onefile->{'Name'};
        if (( $filename eq $patchfilename ) || ( $filename eq $patchfilename2 )) { next; }

        my $styles = "";
        if ( $onefile->{'Styles'} ) { $styles = $onefile->{'Styles'}; }
        if ( $styles =~ /\bDONTRENAMEINPATCH\b/ ) { next; }

        # special handling for files with flag DONTSHOW. This files get the extension ".dontshow" to be filtered by dialogs.
        my $localwindowspatchlevel = $windowspatchlevel;
        if ( $styles =~ /\bDONTSHOW\b/ ) { $localwindowspatchlevel = $localwindowspatchlevel . "\.dontshow"; }

        my $olddestination = $onefile->{'destination'};
        my $newdestination = $olddestination . "." . $localwindowspatchlevel;
        my $localfilename = $olddestination;
        installer::pathanalyzer::make_absolute_filename_to_relative_filename(\$localfilename);  # file name part
        my $line = "\"" . $localfilename . "\"" . "=" . "\"" . "\." . $localwindowspatchlevel . "\"";
        $onefile->{'destination'} = $newdestination;

        my $newfilename = $onefile->{'Name'} . "." . $localwindowspatchlevel;
        $onefile->{'Name'} = $newfilename;

        # adding section information (section is the directory)
        my $origolddestination = $olddestination;
        installer::pathanalyzer::get_path_from_fullqualifiedname(\$olddestination); # directory part
        if ( ! $olddestination ) { $olddestination = "_root";  }
        if ( ! exists($patchfiledirectories{$olddestination}) ) { $patchfiledirectories{$olddestination} = 1; }
        $line = $line . "\tXXXXX\t" . $olddestination . "\n";

        push(@patchfiles, $line);

        # also collecting all files from patch in @installer::globals::patchfilecollector
        my $patchfileline = $origolddestination . "\n";
        push(@installer::globals::patchfilecollector, $patchfileline);
    }

    my $winpatchdirname = "winpatch";
    my $winpatchdir = installer::systemactions::create_directories($winpatchdirname, $languagestringref);

    my ($patchlistfile) = grep {$_->{Name} eq $patchfilename} @{$filesref};
    if (! defined $patchlistfile) {
        installer::exiter::exit_program("ERROR: Could not find file $patchfilename in list of files!", "prepare_windows_patchfiles");
    }

    # reorganizing the patchfile content, sorting for directory to decrease the file size
    my $sorteddirectorylist = [ sort keys %patchfiledirectories ];
    my $patchfilelist = reorg_patchfile(\@patchfiles, $sorteddirectorylist);

    # shifting version.ini to the end of the list, to guarantee, that all files are patched
    # if the correct version is shown in the about box
    $patchfilelist = shift_section_to_end($patchfilelist);
    $patchfilelist = shift_file_to_end($patchfilelist);

    # saving the file
    $patchfilename = $winpatchdir . $installer::globals::separator . $patchfilename;
    installer::files::save_file($patchfilename, $patchfilelist);

    my $infoline = "\nCreated list of patch files: $patchfilename\n";
    push( @installer::globals::logfileinfo, $infoline);

    # and assigning the new source
    $patchlistfile->{'sourcepath'} = $patchfilename;

    # and finally checking the file size
    if ( -f $patchfilename )    # test of existence
    {
        my $filesize = ( -s $patchfilename );
        $infoline = "Size of patch file list: $filesize\n\n";
        push( @installer::globals::logfileinfo, $infoline);
        installer::logger::print_message( "... size of patch list file: $filesize Byte ... \n" );
    }

}

###########################################################
# Replacing %-variables with the content
# of $allvariableshashref
###########################################################

sub replace_variables_in_string
{
    my ( $string, $variableshashref ) = @_;

    if ( $string =~ /^.*\%\w+.*$/ )
    {
        my $key;

        # we want to substitute FOO_BR before FOO to avoid floating _BR suffixes
        foreach $key (sort { length ($b) <=> length ($a) } keys %{$variableshashref})
        {
            my $value = $variableshashref->{$key};
            $key = "\%" . $key;
            $string =~ s/\Q$key\E/$value/g;
        }
    }

    return $string;
}

#################################################################
# Copying the files defined as ScpActions into the
# installation set.
#################################################################

sub put_scpactions_into_installset
{
    my ($installdir) = @_;

    installer::logger::include_header_into_logfile("Start: Copying scp action files into installation set");

    for ( my $i = 0; $i <= $#installer::globals::allscpactions; $i++ )
    {
        my $onescpaction = $installer::globals::allscpactions[$i];

        my $subdir = "";
        if ( $onescpaction->{'Subdir'} ) { $subdir = $onescpaction->{'Subdir'}; }

        if ( $onescpaction->{'Name'} eq "loader.exe" ) { next; }    # do not copy this ScpAction loader

        my $destdir = $installdir;
        $destdir =~ s/\Q$installer::globals::separator\E\s*$//;
        if ( $subdir ) { $destdir = $destdir . $installer::globals::separator . $subdir; }

        my $sourcefile = $onescpaction->{'sourcepath'};
        my $destfile = $destdir . $installer::globals::separator . $onescpaction->{'DestinationName'};

        my $styles = "";
        if ( $onescpaction->{'Styles'} ) { $styles = $onescpaction->{'Styles'}; }
        if (( $styles =~ /\bFILE_CAN_MISS\b/ ) && ( $sourcefile eq "" )) { next; }

        if (( $subdir =~ /\// ) || ( $subdir =~ /\\/ ))
        {
            installer::systemactions::create_directory_structure($destdir);
        }
        else
        {
            installer::systemactions::create_directory($destdir);
        }

        installer::systemactions::copy_one_file($sourcefile, $destfile);

        if ( $onescpaction->{'UnixRights'} )
        {
            chmod oct($onescpaction->{'UnixRights'}), $destfile;
        }

    }

    installer::logger::include_header_into_logfile("End: Copying scp action files into installation set");

}

#################################################################
# Collecting scp actions for all languages
#################################################################

sub collect_scpactions
{
    my ($allscpactions) = @_;

    for ( my $i = 0; $i <= $#{$allscpactions}; $i++ )
    {
        push(@installer::globals::allscpactions, ${$allscpactions}[$i]);
    }
}

###########################################################
# Adding additional variables into the variableshashref,
# that are defined in include files in the solver. The
# names of the include files are stored in
# ADD_INCLUDE_FILES (comma separated list).
###########################################################

sub add_variables_from_inc_to_hashref
{
    my ($allvariables, $includepatharrayref) = @_;

    my $infoline = "";
    my $includefilelist = $allvariables->{'ADD_INCLUDE_FILES'} || "";

    for my $includefilename (split /,\s*/, $includefilelist)
    {
        $includefilename =~ s/^\s*//;
        $includefilename =~ s/\s*$//;
        $includefilenameref = installer::scriptitems::get_sourcepath_from_filename_and_includepath(\$includefilename, $includepatharrayref, 1);
        if ( $$includefilenameref eq "" ) { installer::exiter::exit_program("Include file $includefilename not found!\nADD_INCLUDE_FILES = $allvariables->{'ADD_INCLUDE_FILES'}", "add_variables_from_inc_to_hashref"); }

        $infoline = "Including inc file: $$includefilenameref \n";
        push( @installer::globals::globallogfileinfo, $infoline);

        my $includefile = installer::files::read_file($$includefilenameref);

        for ( my $j = 0; $j <= $#{$includefile}; $j++ )
        {
            # Analyzing all "key=value" lines
            my $oneline = ${$includefile}[$j];

            if ( $oneline =~ /^\s*(\S+)\s*\=\s*(.*?)\s*$/ ) # no white space allowed in key
            {
                my $key = $1;
                my $value = $2;
                $allvariables->{$key} = $value;
                $infoline = "Setting of variable: $key = $value\n";
                push( @installer::globals::globallogfileinfo, $infoline);
            }
        }
    }
}

##############################################
# Collecting all files from include paths
##############################################

sub collect_all_files_from_includepaths
{
    my ($patharrayref) = @_;

    installer::logger::globallog("Reading all directories: Start");
    installer::logger::print_message( "... reading include paths ...\n" );
    # empty the global

    @installer::globals::allincludepaths =();
    my $infoline;

    for ( my $i = 0; $i <= $#{$patharrayref}; $i++ )
    {
        $includepath = ${$patharrayref}[$i];
        installer::remover::remove_leading_and_ending_whitespaces(\$includepath);

        if ( ! -d $includepath )
        {
            $infoline = "$includepath does not exist. (Can be removed from include path list?)\n";
            push( @installer::globals::globallogfileinfo, $infoline);
            next;
        }

        my @sourcefiles = ();
        my $pathstring = "";
        installer::systemactions::read_full_directory($includepath, $pathstring, \@sourcefiles);

        if ( ! ( $#sourcefiles > -1 ))
        {
            $infoline = "$includepath is empty. (Can be removed from include path list?)\n";
            push( @installer::globals::globallogfileinfo, $infoline);
        }
        else
        {
            my $number = $#sourcefiles + 1;
            $infoline = "Directory $includepath contains $number files (including subdirs)\n";
            push( @installer::globals::globallogfileinfo, $infoline);

            my %allfileshash = ();
            $allfileshash{'includepath'} = $includepath;

            for ( my $j = 0; $j <= $#sourcefiles; $j++ )
            {
                $allfileshash{$sourcefiles[$j]} = 1;
            }

            push(@installer::globals::allincludepaths, \%allfileshash);
        }
    }

    $installer::globals::include_paths_read = 1;

    installer::logger::globallog("Reading all directories: End");
    push( @installer::globals::globallogfileinfo, "\n");
}

##############################################
# Searching for a file with the gid
##############################################

sub find_file_by_id
{
    my ( $filesref, $gid ) = @_;

    my $foundfile = 0;
    my $onefile;

    for ( my $i = 0; $i <= $#{$filesref}; $i++ )
    {
        $onefile = ${$filesref}[$i];
        my $filegid = $onefile->{'gid'};

        if ( $filegid eq $gid )
        {
            $foundfile = 1;
            last;
        }
    }

    if (! $foundfile ) { $onefile  = ""; }

    return $onefile;
}

#################################################
# Generating paths for cygwin (second version)
# This function generates smaller files for
#################################################

sub generate_cygwin_paths
{
    my ($filesref) = @_;

    installer::logger::include_timestamp_into_logfile("Starting generating cygwin paths");

    my $infoline = "Generating cygwin paths (generate_cygwin_paths)\n";
    push( @installer::globals::logfileinfo, $infoline);

    my $max = 5000;  # number of paths in one file

    my @pathcollector = ();
    my $startnumber = 0;
    my $counter = 0;

    for ( my $i = 0; $i <= $#{$filesref}; $i++ )
    {
        my $line = ${$filesref}[$i]->{'sourcepath'} . "\n";
        push(@pathcollector, $line);
        $counter++;

        if (( $i == $#{$filesref} ) || ((( $counter % $max ) == 0 ) && ( $i > 0 )))
        {
            my $tmpfilename = "cygwinhelper_" . $i . ".txt";
            my $temppath = $installer::globals::temppath;
            $temppath =~ s/\Q$installer::globals::separator\E\s*$//;
            $tmpfilename = $temppath . $installer::globals::separator . $tmpfilename;
            $infoline = "Creating temporary file for cygwin conversion: $tmpfilename (contains $counter paths)\n";
            push( @installer::globals::logfileinfo, $infoline);
            if ( -f $tmpfilename ) { unlink $tmpfilename; }

            installer::files::save_file($tmpfilename, \@pathcollector);

            my $success = 0;
            my @cyg_sourcepathlist = qx{cygpath -w -f "$tmpfilename"};
            chomp @cyg_sourcepathlist;

            # Validating the array, it has to contain the correct number of values
            my $new_paths = $#cyg_sourcepathlist + 1;
            if ( $new_paths == $counter ) { $success = 1; }

            if ($success)
            {
                $infoline = "Success: Successfully converted to cygwin paths!\n";
                push( @installer::globals::logfileinfo, $infoline);
            }
            else
            {
                $infoline = "ERROR: Failed to convert to cygwin paths!\n";
                push( @installer::globals::logfileinfo, $infoline);
                installer::exiter::exit_program("ERROR: Failed to convert to cygwin paths!", "generate_cygwin_paths");
            }

            for ( my $j = 0; $j <= $#cyg_sourcepathlist; $j++ )
            {
                my $number = $startnumber + $j;
                ${$filesref}[$number]->{'cyg_sourcepath'} = $cyg_sourcepathlist[$j];
            }

            if ( -f $tmpfilename ) { unlink $tmpfilename; }

            @pathcollector = ();
            $startnumber = $startnumber + $max;
            $counter = 0;
        }
    }

    # Checking existence fo cyg_sourcepath for every file
    for ( my $i = 0; $i <= $#{$filesref}; $i++ )
    {
        if (( ! exists(${$filesref}[$i]->{'cyg_sourcepath'}) ) || ( ${$filesref}[$i]->{'cyg_sourcepath'} eq "" ))
        {
            $infoline = "ERROR: No cygwin sourcepath defined for file ${$filesref}[$i]->{'sourcepath'}\n";
            push( @installer::globals::logfileinfo, $infoline);
            installer::exiter::exit_program("ERROR: No cygwin sourcepath defined for file ${$filesref}[$i]->{'sourcepath'}!", "generate_cygwin_paths");
        }
    }

    installer::logger::include_timestamp_into_logfile("Ending generating cygwin paths");
}

################################################
# Files with flag HIDDEN get a dot at the
# beginning of the file name. This cannot be
# defined in scp2 project, because tooling
# cannot handle files with beginning dot
# correctly.
################################################

sub resolving_hidden_flag
{
    my ($filesarrayref, $variableshashref, $item, $languagestringref) = @_;

    my $diritem = lc($item);
    my $infoline = "";

    my $hiddendirbase = installer::systemactions::create_directories("hidden_$diritem", $languagestringref);

    installer::logger::include_header_into_logfile("$item with flag HIDDEN:");

    for ( my $i = 0; $i <= $#{$filesarrayref}; $i++ )
    {
        my $onefile = ${$filesarrayref}[$i];
        my $styles = "";

        if ( $onefile->{'Styles'} ) { $styles = $onefile->{'Styles'}; }

        if ( $styles =~ /\bHIDDEN\b/ )
        {
            # Language specific subdirectory

            my $onelanguage = $onefile->{'specificlanguage'};

            if ($onelanguage eq "")
            {
                $onelanguage = "00";    # files without language into directory "00"
            }

            my $hiddendir = $hiddendirbase . $installer::globals::separator . $onelanguage . $installer::globals::separator;
            installer::systemactions::create_directory($hiddendir); # creating language specific directories

            # copy files and edit them with the variables defined in the zip.lst

            my $onefilename = $onefile->{'Name'};
            my $newfilename = "\." . $onefilename;
            my $sourcefile = $onefile->{'sourcepath'};
            my $destfile = $hiddendir . $newfilename;

            my $copysuccess = installer::systemactions::copy_one_file($sourcefile, $destfile);

            if ( $copysuccess )
            {
                $onefile->{'sourcepath'} = $destfile;
                $destination = $onefile->{'destination'};
                installer::pathanalyzer::get_path_from_fullqualifiedname(\$destination);
                if ( $destination eq "" ) { $onefile->{'destination'} = $newfilename; }
                else { $onefile->{'destination'} = $destination . $installer::globals::separator . $newfilename; }

                $infoline = "Success: Using file with flag HIDDEN from \"$onefile->{'sourcepath'}\"!\n";
                push( @installer::globals::logfileinfo, $infoline);
            }
            else
            {
                $infoline = "Error: Failed to copy HIDDEN file from \"$sourcefile\" to \"$destfile\"!\n";
                push( @installer::globals::logfileinfo, $infoline);
            }
        }
    }

    $infoline = "\n";
    push( @installer::globals::logfileinfo, $infoline);
}

######################################################
# Getting the first entry from a list of languages
######################################################

sub get_first_from_list
{
    my ( $list ) = @_;

    my $first = $list;

    if ( $list =~ /^\s*(.+?),(.+)\s*$/) # "?" for minimal matching
    {
        $first = $1;
    }

    return $first;
}

################################################
# Setting all spellchecker languages
################################################

sub set_spellcheckerlanguages
{
    my ( $productlanguagesarrayref, $allvariables ) = @_;

    my %productlanguages = ();
    for ( my $i = 0; $i <= $#{$productlanguagesarrayref}; $i++ ) { $productlanguages{${$productlanguagesarrayref}[$i]} = 1;  }

    my $spellcheckfilename = $allvariables->{'SPELLCHECKERFILE'};

    my $spellcheckfileref = installer::scriptitems::get_sourcepath_from_filename_and_includepath(\$spellcheckfilename, "", 1);

    if ($$spellcheckfileref eq "") { installer::exiter::exit_program("ERROR: Could not find $spellcheckfilename!", "set_spellcheckerlanguages"); }

    my $infoline = "Using spellchecker file: $$spellcheckfileref \n";
    push( @installer::globals::globallogfileinfo, $infoline);

    my $spellcheckfile = installer::files::read_file($$spellcheckfileref);
    my %spellcheckhash = ();

    for ( my $j = 0; $j <= $#{$spellcheckfile}; $j++ )
    {
        # Analyzing all "key=value" lines
        my $oneline = ${$spellcheckfile}[$j];

        if ( $oneline =~ /^\s*(\S+)\s*\=\s*\"(.*?)\"\s*$/ ) # no white space allowed in key
        {
            my $onelang = $1;
            my $languagelist = $2;

            # Special handling for language packs. Only include the first language of the language list.
            # If no spellchecker shall be included, the keyword "EMPTY" can be used.

            if ( $installer::globals::languagepack )
            {
                my $first = get_first_from_list($languagelist);

                if ( $first eq "EMPTY" )     # no spellchecker into language pack
                {
                    $languagelist = "";
                }
                else
                {
                    $languagelist = $first;
                }
            }
            else  # no language pack, so EMPTY is not required
            {
                $languagelist =~ s/^\s*EMPTY\s*,//; # removing the entry EMPTY
            }

            $spellcheckhash{$onelang} = $languagelist;
        }
    }

    # Collecting all required languages in %installer::globals::spellcheckerlanguagehash

    foreach my $lang (keys %productlanguages)
    {
        my $languagelist = "";
        if ( exists($spellcheckhash{$lang}) ) { $languagelist = $spellcheckhash{$lang}; }
        else { $languagelist = ""; }  # no dictionary unless defined in SPELLCHECKERFILE

        my $langlisthash = installer::converter::convert_stringlist_into_hash(\$languagelist, ",");
        foreach my $onelang ( keys %{$langlisthash} ) { $installer::globals::spellcheckerlanguagehash{$onelang} = 1; }
    }

    $installer::globals::analyze_spellcheckerlanguage = 1;

    # Logging

    my $langstring = "";
    foreach my $lang (sort keys %installer::globals::spellcheckerlanguagehash) { $langstring = $langstring . "," . $lang }
    $langstring =~ s/^\s*,//;

    $infoline = "Collected spellchecker languages for spellchecker: $langstring \n";
    push( @installer::globals::globallogfileinfo, $infoline);
}

################################################
# Including a license text into setup script
################################################

sub put_license_into_setup
{
    my ($installdir, $includepatharrayref) = @_;

    # find and read english license file
    my $licenselanguage = "en-US";                  # always english !
    # my $licensefilename = "LICENSE"; # _" . $licenselanguage;
    my $licensefilename = "license"; # . $licenselanguage . ".txt";
    my $licenseincludepatharrayref = get_language_specific_include_paths($includepatharrayref, $licenselanguage);

    my $licenseref = installer::scriptitems::get_sourcepath_from_filename_and_includepath(\$licensefilename, $licenseincludepatharrayref, 0);
    if ($$licenseref eq "") { installer::exiter::exit_program("ERROR: Could not find License file $licensefilename!", "put_license_into_setup"); }
    my $licensefile = installer::files::read_file($$licenseref);

    # Read setup
    my $setupfilename = $installdir . $installer::globals::separator . "setup";
    my $setupfile = installer::files::read_file($setupfilename);

    # Replacement
    my $infoline = "Adding licensefile into setup script\n";
    push( @installer::globals::logfileinfo, $infoline);

    my $includestring = "";
    for ( my $i = 0; $i <= $#{$licensefile}; $i++ ) { $includestring = $includestring . ${$licensefile}[$i]; }
    for ( my $i = 0; $i <= $#{$setupfile}; $i++ ) { ${$setupfile}[$i] =~ s/LICENSEFILEPLACEHOLDER/$includestring/; }

    # Write setup
    installer::files::save_file($setupfilename, $setupfile);
}

#########################################################
# Collecting all pkgmap files from an installation set
#########################################################

sub collectpackagemaps
{
    my ( $installdir, $languagestringref, $allvariables ) = @_;

    installer::logger::include_header_into_logfile("Collecing all packagemaps (pkgmap):");

    my $pkgmapdir = installer::systemactions::create_directories("pkgmap", $languagestringref);
    my $subdirname = $allvariables->{'UNIXPRODUCTNAME'} . "_pkgmaps";
    my $pkgmapsubdir = $pkgmapdir . $installer::globals::separator . $subdirname;
    if ( -d $pkgmapsubdir ) { installer::systemactions::remove_complete_directory($pkgmapsubdir); }
    if ( ! -d $pkgmapsubdir ) { installer::systemactions::create_directory($pkgmapsubdir); }

    $installdir =~ s/\/\s*$//;
    # Collecting all packages in $installdir and its sub package ("packages")
    my $searchdir = $installdir . $installer::globals::separator . $installer::globals::epmoutpath;

    my $allpackages = installer::systemactions::get_all_directories_without_path($searchdir);

    for ( my $i = 0; $i <= $#{$allpackages}; $i++ )
    {
        my $pkgmapfile = $searchdir . $installer::globals::separator . ${$allpackages}[$i] . $installer::globals::separator . "pkgmap";
        my $destfilename = $pkgmapsubdir . $installer::globals::separator . ${$allpackages}[$i] . "_pkgmap";
        installer::systemactions::copy_one_file($pkgmapfile, $destfilename);
    }

    # Create a tar gz file with all package maps
    my $tarfilename = $subdirname . ".tar";
    my $targzname = $tarfilename . ".gz";
    $systemcall = "cd $pkgmapdir; tar -cf - $subdirname | gzip > $targzname";
    installer::systemactions::make_systemcall($systemcall);
    installer::systemactions::remove_complete_directory($pkgmapsubdir, 1);
}

1;
