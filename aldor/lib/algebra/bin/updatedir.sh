#!/bin/sh
##########################################################
#                                                        #
# Updates a directory of the src tree                    #
#                                                        #
# The Aldor Development Group (2001-2002)                #
#                                                        #
##########################################################

# Initial checks
if [ $# != 2 ] 
then 
          echo "Usage: $0 \$UPDATING \$UPDATED" 
          exit 1 
fi

# Variables
UPDATING=$1
UPDATED=$2

# Initial checks
if [ ! -d $UPDATING ]
then
	echo "Sorry: cannot find $UPDATING"
	exit 1
fi

if [ ! -d $UPDATED ]
then
	echo "Sorry: cannot find $UPDATED"
	exit 1
fi

# computing
cp -fu $UPDATING/Makefile $UPDATED/Makefile
touch $UPDATING/alg_12345678901234567890.as
cp -fu $UPDATING/alg_*.as $UPDATED/
rm -f $UPDATED/alg_12345678901234567890.as
touch $UPDATING/sm_12345678901234567890.as
cp -fu $UPDATING/sm_*.as $UPDATED/
rm -f $UPDATED/sm_12345678901234567890.as
touch $UPDATING/sit_12345678901234567890.as
cp -fu $UPDATING/sit_*.as $UPDATED/
rm -f $UPDATED/sit_12345678901234567890.as

