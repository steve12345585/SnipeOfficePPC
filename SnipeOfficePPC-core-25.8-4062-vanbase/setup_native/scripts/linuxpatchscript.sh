#!/bin/sh
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

MYUID=`id | sed "s/(.*//g" | sed "s/.*=//"`

if [ $MYUID -ne 0 ]
then
    echo You need to have super-user permissions to run this patch script
    exit 1
fi

echo
echo "Searching for the PRODUCTNAMEPLACEHOLDER installation ..."

RPMNAME=`rpm -qa | grep SEARCHPACKAGENAMEPLACEHOLDER`

if [ "x$RPMNAME" != "x" ]
then
  PRODUCTINSTALLLOCATION="`rpm --query --queryformat "%{INSTALLPREFIX}" $RPMNAME`"
  FULLPRODUCTINSTALLLOCATION="${PRODUCTINSTALLLOCATION}/PRODUCTDIRECTORYNAME"
else
  echo "PRODUCTNAMEPLACEHOLDER is not installed"
  exit 1
fi

# Last chance to exit ..
echo
read -p "Patching the installation in ${FULLPRODUCTINSTALLLOCATION}. Continue (y/n) ? " -n 1 reply leftover
echo
[ "$reply" = "y" ] || exit 1

echo
echo "About to update the following packages ..."

BASEDIR=`dirname $0`

RPMLIST=""
for i in `ls $BASEDIR/RPMS/*.rpm`
do
  rpm --query `rpm --query --queryformat "%{NAME}\n" --package $i` && RPMLIST="$RPMLIST $i"
done

# Save UserInstallation value
BOOTSTRAPRC="${FULLPRODUCTINSTALLLOCATION}/program/bootstraprc"
USERINST=`grep UserInstallation ${BOOTSTRAPRC}`

# Check, if kde-integration rpm is available
KDERPM=`ls $BASEDIR/RPMS/*.rpm | grep kde-integration`

if [ "x$KDERPM" != "x" ]; then
  # Check, that $RPMLIST does not contain kde integration rpm (then it is already installed)
  KDERPMINSTALLED=`grep kde-integration ${RPMLIST}`

  if [ "x$KDERPMINSTALLED" = "x" ]; then
    # Install the kde integration rpm
    RPMLIST="$RPMLIST $KDERPM"
  fi
fi

echo
rpm --upgrade -v --hash --prefix $PRODUCTINSTALLLOCATION --notriggers $RPMLIST
echo

# Some RPM versions have problems with -U and --prefix
if [ ! -f ${BOOTSTRAPRC} ]; then
  echo Update failed due to a bug in RPM, uninstalling ..
  rpm --erase -v --nodeps --notriggers `rpm --query --queryformat "%{NAME} " --package $RPMLIST`
  echo
  echo Now re-installing new packages ..
  echo
  rpm --install -v --hash --prefix $PRODUCTINSTALLLOCATION --notriggers $RPMLIST
  echo
fi

# Restore the UserInstallation key if necessary
DEFUSERINST=`grep UserInstallation ${BOOTSTRAPRC}`
if [ "${USERINST}" != "${DEFUSERINST}" ]; then
  mv -f ${BOOTSTRAPRC} ${BOOTSTRAPRC}.$$
  sed "s|UserInstallation.*|${USERINST}|" ${BOOTSTRAPRC}.$$ > ${BOOTSTRAPRC}
  rm -f ${BOOTSTRAPRC}.$$
fi

echo "Done."

exit 0
