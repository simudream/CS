#! /bin/sh
#==============================================================================
# Determine if various tools are available.
#
# IMPORTS
#    checktool()
#	Shell function which checks if the program mentioned as its sole
#	argument can be found in the PATH.
#    msg_*()
#	Functions for reporting progress to users.
#
# EXPORTS
#    CMD.MKDIR
#	Makefile variable emitted to the standard output stream.  Value is the
#	command to create a directory.
#    CMD.MKDIRS
#	Makefile variable emitted to the standard output stream.  Value is the
#	command to create a directory plus any missing parent directories.
#    NASM.AVAILABLE
#	Makefile variable emitted to the standard output stream.  Value is
#	"yes" if nasm is available, otherwise the variable is not set.
#    DEPEND_TOOL
#	Makefile varaible emitted to the standard output stream.  Value is
#	"mkdep" if a special dependency generation tool was located.
#    MAKEDEP.AVAILABLE
#	Makefile variable emitted to the standard ouput stream.  Value is "yes"
#	if the customized Crystal Space "makedep" program is available.
#==============================================================================

#------------------------------------------------------------------------------
# Determine how to create a directory.
#------------------------------------------------------------------------------
msg_checking "how to create a directory"
echo "CMD.MKDIR = mkdir"
msg_result "mkdir"


#------------------------------------------------------------------------------
# Determine how to create a directory including missing parents.  First try
# "mkdir -p".  If that fails, try the old "mkdirs" command.  When cleaning up,
# also delete a directory named "-p" since it might have been created
# inadvertently if mkdir failed to recognize the -p option.
#------------------------------------------------------------------------------
msg_checking "how to create a directory tree"
MKDIRS="no"
rm -rf mkdtest
mkdir -p mkdtest/mkdtest/mkdtest >/dev/null 2>&1
if [ $? -eq 0 ]; then
  MKDIRS="mkdir -p"
elif [ -n `checkprog "mkdirs"` ]; then
  mkdirs mkdtest/mkdtest/mkdtest >/dev/null 2>&1
  if [ $? -eq 0 ]; then
    MKDIRS="mkdirs"
  fi
fi
rm -rf - -p mkdtest

if [ "${MKDIRS}" != "no" ]; then
  echo "CMD.MKDIRS = ${MKDIRS}"
fi
msg_result "${MKDIRS}"


#------------------------------------------------------------------------------
# Check if NASM is installed and has the correct version.
#------------------------------------------------------------------------------
[ -z "${NASMBIN}" ] && NASMBIN=`checktool nasm`
if [ -n "${NASMBIN}" ]; then
  msg_checking "for nasm extensions"
  echo "%xdefine TEST" >conftest.asm
  # Well, we really should check here for obj format,
  # but we'll use ELF as it really doesn't matter.
  ${NASMBIN} -f elf conftest.asm -o conftest.o 2>/dev/null
  if [ $? -eq 0 ]; then
    echo "NASM.AVAILABLE = yes"
    msg_result "yes"
  else
    msg_result "no"
  fi
  rm -f conftest.asm conftest.o
fi


#------------------------------------------------------------------------------
# Check if makedep is installed and has the correct version.
#------------------------------------------------------------------------------
[ -z "${MAKEDEP}" ] && MAKEDEP=`checktool makedep`
if [ -n "${MAKEDEP}" ]; then
  msg_checking "for makedep extensions"
  echo "DEPEND_TOOL = mkdep"
  MAKEDEP_VERSION=`makedep -V | sed -e "s/.*Version *//"`
  if [ `expr "${MAKEDEP_VERSION}" ">" 0.0.0` = "1" ]; then
    echo "MAKEDEP.AVAILABLE = yes"
    msg_result "yes"
  else
    msg_result "no"
  fi
fi


#------------------------------------------------------------------------------
# Check for Bison.
#------------------------------------------------------------------------------
BISONBIN=`checktool bison`
if [ -n "${BISONBIN}" ]; then
    echo "BISONBIN = ${BISONBIN}"
fi


#------------------------------------------------------------------------------
# Check for Flex.
#------------------------------------------------------------------------------
FLEXBIN=`checktool flex`
if [ -n "${FLEXBIN}" ]; then
    echo "FLEXBIN = ${FLEXBIN}"
fi


#------------------------------------------------------------------------------
# Check for Swig and Swig extensions.
#------------------------------------------------------------------------------
SWIGBIN=`checktool swig`
if [ -n "${SWIGBIN}" ]; then
    echo "SWIGBIN = ${SWIGBIN}"
fi


#------------------------------------------------------------------------------
# Check for Luaswig.
#------------------------------------------------------------------------------
LUASWIGBIN=`checktool luaswig`
if [ -n "${SWIGBIN}" ]; then
    echo "LUASWIGBIN = ${LUASWIGBIN}"
fi
