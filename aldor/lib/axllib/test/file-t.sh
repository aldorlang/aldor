#!/bin/sh
#
# This script tests file manipulations.

AXIOMXL="${ALDORROOT}/bin/aldor"
PATH=/tmp:/usr/local/bin:/usr/bin:/bin
export PATH

$AXIOMXL -Wtest+file
