#!/bin/bash
##########################################################
#                                                        #
# Updates a src tree from another                        #
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

echo "Updating $UPDATING from $UPDATED... "

# Computing

cd $ALGEBRAROOT 
ln -s $UPDATING updating
ln -s $UPDATED updated
cd $ALGEBRAROOT/bin
make update
cd $ALGEBRAROOT 
rm -f updating
rm -f updated

echo "... done."
exit 0

