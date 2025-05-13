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

#
# check - a perl script to check some files and directories if they exists
# A first simple check if the SDK was built completely.
#

$return = 0;

$StartDir = "$ARGV[0]";
$OperatingSystem = "$ARGV[1]";
$ExePrefix = "$ARGV[2]";

print "Check for $OperatingSystem\n";

if (-d "$StartDir") {
    # check binaries
    print "check binaries: ";
    if (-d "$StartDir/bin") {
	my @binarylist = ( "idlc","cppumaker","javamaker",
               "regcompare","autodoc",
               "unoapploader", "uno-skeletonmaker" );
    if ($ENV{SYSTEM_UCPP} eq "") {
	push @binarylist,"ucpp";
    }

    foreach $i (@binarylist)
    {
        if (! -e "$StartDir/bin/$i$ExePrefix") {
        $return++;
        print "\nERROR: \"$StartDir/bin/$i$ExePrefix\" is missing\n";
        } else {
        print "+";
        }
    }

    if ($OperatingSystem eq "windows" || $OperatingSystem eq "mingw") {
        if ($OperatingSystem eq "windows" && ! -e "$StartDir/bin/climaker.exe") {
        $return++;
        print "\nERROR: \"$StartDir/bin/climaker.exe\" is missing\n";
        } else {
        print "+";
        }
    }
    if ($OperatingSystem eq "macosx") {
        if (! -e "$StartDir/bin/addsym-macosx.sh") {
        $return++;
        print "\nERROR: \"$StartDir/bin/addsym-macosx.sh\" is missing\n";
        } else {
        print "+";
        }
    }
    } else {
    $return++;
    }
    print "\n";

    # packaging files
    print "check packaging files: ";
    if (-d "$StartDir/docs") {
    my @filelist = ( "install.html",
             "notsupported.html","sdk_styles.css","tools.html",
             "images/arrow-1.gif", "images/arrow-3.gif",
             "images/odk-footer-logo.gif",
             "images/bg_table.png","images/bg_table2.png",
             "images/bg_table3.png", "images/nav_down.png",
             "images/nav_home.png","images/nav_left.png",
             "images/nav_right.png","images/nav_up.png",
             "images/sdk_head-1.png", "images/sdk_head-2.png",
             "images/sdk_line-1.gif", "images/sdk_line-2.gif",
             "common/ref/idl.css", "images/nada.gif",
             "images/arrow-2.gif", "images/bluball.gif",
             "images/ooo-main-app_32.png");

    foreach $i (@filelist)
    {
        if (! -e "$StartDir/docs/$i") {
        $return++;
        print "\nERROR: \"$StartDir/docs/$i\" is missing\n";
        } else {
        print "+";
        }
    }
    } else {
    $return++;
    }
    print "\n";

    #check configure files
    print "check config files: ";
    if ($OperatingSystem eq "windows" || $OperatingSystem eq "mingw") {
    if (! -e "$StartDir/setsdkenv_windows.bat") {
        print "\nERROR: \"$StartDir/setsdkenv_windows.bat\" is missing\n";
        $return++;
    }
    if (! -e "$StartDir/setsdkenv_windows.template") {
        print "\nERROR: \"$StartDir/setsdkenv_windows.template\" is missing\n";
        $return++;
    }
    if (! -e "$StartDir/cfgWin.js") {
        print "\nERROR: \"$StartDir/cfgWin.js\" is missing\n";
        $return++;
    }
    } else {
    if (! -e "$StartDir/configure.pl") {
        print "\nERROR: \"$StartDir/configure.pl\" is missing\n";
        $return++;
    }
    if (! -e "$StartDir/config.guess") {
        print "\nERROR: \"$StartDir/config.guess\" is missing\n";
        $return++;
    }
    if (! -e "$StartDir/config.sub") {
        print "\nERROR: \"$StartDir/config.sub\" is missing\n";
        $return++;
       }
    if (! -e "$StartDir/setsdkenv_unix") {
        print "\nERROR: \"$StartDir/setsdkenv_unix\" is missing\n";
        $return++;
    }
    if (! -e "$StartDir/setsdkenv_unix.sh.in") {
        print "\nERROR: \"$StartDir/setsdkenv_unix.sh.in\" is missing\n";
        $return++;
    }
    if (! -e "$StartDir/setsdkenv_unix.csh.in") {
        print "\nERROR: \"$StartDir/setsdkenv_unix.csh.in\" is missing\n";
        $return++;
    }
    }
    print "\n";

    #check setting files
    print "check setting files: ";
    if (-d "$StartDir/settings") {
    if (! -e "$StartDir/settings/settings.mk") {
        print "\nERROR: \"$StartDir/settings/settings.mk\" is missing\n";
        $return++;
    }
    if (! -e "$StartDir/settings/std.mk") {
        print "\nERROR: \"$StartDir/settings/std.mk\" is missing\n";
        $return++;
    }
    if (! -e "$StartDir/settings/stdtarget.mk") {
        print "\nERROR: \"$StartDir/settings/stdtarget.mk\" is missing\n";
        $return++;
    }
    } else {
    $return++;
    }
    print "\n";

    #check cpp docu, it is only a first and simple check
    # improvement required
    if ($ENV{'DOXYGEN'} ne '') {
        print "check cpp docu: ";
        if (-d "$StartDir/docs/cpp/ref") {
            if (! -e "$StartDir/docs/cpp/ref/index.html") {
                print "\nERROR: \"$StartDir/docs/cpp/ref/index.html\" is missing\n";
                $return++;
            }
        } else {
            $return++;
        }
        print "\n";
    }

    #check java docu, it is only a first and simple check
    # improvement required
    my $solar_java = $ENV{"SOLAR_JAVA"};
    my $JDK = $ENV{"JDK"};
    if (defined($solar_java) && $solar_java ne "" && (!defined($JDK) || $JDK ne "gcj")) {
    print "check java docu: ";
    if (-d "$StartDir/docs/java/ref") {
        if (! -e "$StartDir/docs/java/ref/index.html") {
        print "\nERROR: \"$StartDir/docs/java/ref/index.html\" is missing\n";
        $return++;
        }

        my @dir_list = ( "lib","lib/uno","lib/uno/helper","lib/uno/helper/class-use",
                 "uno","uno/class-use","comp","comp/helper",
                 "comp/helper/class-use");

        foreach $i (@dir_list)
        {
        if (! -d "$StartDir/docs/java/ref/com/sun/star/$i") {
            $return++;
            print "\nERROR: \"$StartDir/docs/java/ref/com/sun/star/$i\" is missing\n";
        } else {
            print "+";
        }
        }
    } else {
        $return++;
    }
    print "\n";
    }

    #check idl docu, it is only a first and simple check
    # improvement required
    print "check idl docu: ";
    if (-d "$StartDir/docs/common/ref") {
    if (! -e "$StartDir/docs/common/ref/module-ix.html") {
        print "\nERROR: \"$StartDir/docs/common/ref/module-ix.html\" is missing\n";
        $return++;
    }
    if (! -d "$StartDir/docs/common/ref/index-files") {
        print "\nERROR: \"$StartDir/docs/common/ref/index-files\" is missing\n";
        $return++;
    }
    if (! -e "$StartDir/docs/common/ref/index-files/index-10.html") {
        print "\nERROR: \"$StartDir/docs/common/ref/index-files/index-10.html\" is missing\n";
        $return++;
    }

    my @idl_dirlist = ( "accessibility",
                "animations",
                "auth",
                "awt",
                "awt/tab",
                "awt/tree",
                "awt/grid",
                "beans",
                "bridge",
                "bridge/oleautomation",
                "chart",
                "chart2",
                "chart2/data",
                "configuration",
                "configuration/backend",
                "configuration/backend/xml",
                "configuration/bootstrap",
                "connection",
                "container",
                "datatransfer",
                "datatransfer/clipboard",
                "datatransfer/dnd",
                "deployment",
                "deployment/ui",
                "document",
                "drawing",
                "drawing/framework",
                "embed",
                "form",
                "form/binding",
                "form/component",
                "form/control",
                "form/inspection",
                "form/runtime",
                "form/submission",
                "form/validation",
                "formula",
                "frame",
                "frame/status",
                "gallery",
                "geometry",
                "graphic",
                "i18n",
                "image",
                "inspection",
                "io",
                "java",
                "lang",
                "ldap",
                "linguistic2",
                "loader",
                "logging",
                "mail",
                "media",
                "mozilla",
                "packages",
                "packages/manifest",
                "packages/zip",
                "plugin",
                "presentation",
                "reflection",
                "registry",
                "rendering",
                "report",
                "report/inspection",
                "resource",
                "scanner",
                "script",
                "script/browse",
                "script/provider",
                "sdb",
                "sdb/application",
                "sdb/tools",
                "sdbc",
                "sdbcx",
                "security",
                "sheet",
                "smarttags",
                "style",
                "svg",
                "system",
                "table",
                "task",
                "text",
                "text/fieldmaster",
                "text/textfield",
                "text/textfield/docinfo",
                "ucb",
                "ui",
                "ui/dialogs",
                "uno",
                "uri",
                "util",
                "view",
                "xforms",
                "xml",
                "xml/crypto",
                "xml/crypto/sax",
                "xml/csax",
                "xml/dom",
                "xml/dom/events",
                "xml/dom/views",
                "xml/input",
                "xml/sax",
                "xml/wrapper",
                "xml/xpath",
                "xsd" );

    foreach $i (@idl_dirlist)
    {
        if (! -d "$StartDir/docs/common/ref/com/sun/star/$i") {
        $return++;
        print "\nERROR: \"$StartDir/docs/common/ref/com/sun/star/$i\" is missing\n";
        } else {
        print "+";
        }
    }
    } else {
    $return++;
    }
    print "\n";

} else {
    $return++;
}

if( $return != 0 )
{
    print "ERROR\n";
    unlink "$ARGV[3]";
} else {
    print "OK\n";
}
exit $return;
