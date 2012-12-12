#!/bin/bash
#
# This script generate all missing files.
#
#

# create m4 directory
mkdir -p m4

echo "## run libtoolize ##"
libtoolize --force

echo "## run aclocal ##"
aclocal

echo "## run automake ##"
automake --add-missing

autoreconf -i

