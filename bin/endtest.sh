#!/bin/sh
#
# This script file is used to autodetect whether or not the C++ compiler
# has a built-in 'bool' type.
#
# Arguments: $1 is the name of the C++ compiler (gcc, c++, etc.)
#
# The output of this script (a makefile fragment) is configuration
# information needed for building Crystal Space.  It is pipied to stdout,
# and errors are piped to stderr.

CXX=$1

echo "int main() { int x = 0x1234; return *(unsigned char *)&x == 0x12; }" > endtest.cpp

${CXX} endtest.cpp 2>/dev/null || echo "endtest.sh: cannot compile testcase" >&2
if ./endtest; then
	echo "CS_LITTLE_ENDIAN = 1"
else
	echo "CS_BIG_ENDIAN = 1"
fi

rm -f endtest.cpp endtest

exit 0
