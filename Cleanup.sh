#!/bin/bash
#
# GCC SH4 Linux Cleanup Script
#
#
# Derived from KNNSpeed's "Simple Kernel":
# https://github.com/KNNSpeed/Simple-Kernel
# V0.8, January 3, 2020
#

set +v

#
# set +v disables displaying all of the code you see here in the command line
#

echo
echo "Running cleanup procedures: press CTRL+C to exit now. Otherwise..."
read -n1 -r -p "Press any key to continue..."

#
# Get current path
#

CurDir=$PWD

#
# Delete generated files
#

rm program.elf
rm program.bin
rm output.map
rm objects.list

#
# Move into startup folder
#

cd $CurDir/startup

#
# Delete compiled object files
#

rm *.o
rm *.d
rm *.out

#
# Move into user source directory
#

cd $CurDir/src

#
# Delete compiled object files
#

rm *.o
rm *.d
rm *.out

#
# Return to folder started from
#

cd $CurDir

#
# Display completion message and exit
#

echo
echo "Done! All clean."
echo
