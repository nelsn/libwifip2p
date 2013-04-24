#!/bin/bash
#
# This script generate all missing files.
#
#

# create m4 directory
mkdir -p m4

# run autotools
autoreconf -i

