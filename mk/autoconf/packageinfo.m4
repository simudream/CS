#==============================================================================
# packageinfo.m4
#    Macros for setting general info on the package, such as name and version
#    numbers and propagate them to the generated make and Jam property files.
#
# Copyright (C)2003 by Matthias Braun <matze@braunis.de>
# Copyright (C)2003 by Eric Sunshine <sunshine@sunshineco.com>
#
#    This library is free software; you can redistribute it and/or modify it
#    under the terms of the GNU Library General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or (at your
#    option) any later version.
#
#    This library is distributed in the hope that it will be useful, but
#    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
#    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
#    License for more details.
#
#    You should have received a copy of the GNU Library General Public License
#    along with this library; if not, write to the Free Software Foundation,
#    Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
#==============================================================================

#------------------------------------------------------------------------------
# CS_PACKAGEINFO([LONGNAME], [COPYRIGHT, [HOMEPAGE])
#	Set additional information for the package.  Note that the version
#	number of your application should only contain numbers, because on
#	Windows you can only set numerical values in some of the file
#	properties (such as versioninfo .rc files).
#------------------------------------------------------------------------------
AC_DEFUN([CS_PACKAGEINFO],
    [PACKAGE_LONGNAME="[$1]"
    PACKAGE_COPYRIGHT="[$2]"
    PACKAGE_HOMEPAGE="[$3]"
])


#------------------------------------------------------------------------------
# CS_EMIT_PACKAGEINFO(TARGET)
#	Emit extended package information to a designated target.  If TARGET is
#	"jam", then information is emitted via CS_JAMCONFIG_PROPERTY macros.
#	If TARGET is "make", then information is emitted via
#	CS_MAKEFILE_PROPERTY macros.
#------------------------------------------------------------------------------
AC_DEFUN([CS_EMIT_PACKAGEINFO],
    [_CS_EMIT_PACKAGEINFO([$1], [PACKAGE_NAME], [$PACKAGE_NAME])
    _CS_EMIT_PACKAGEINFO([$1], [PACKAGE_VERSION], [$PACKAGE_VERSION])
    _CS_EMIT_PACKAGEINFO([$1], [PACKAGE_STRING], [$PACKAGE_STRING])
    _CS_EMIT_PACKAGEINFO([$1], [PACKAGE_BUGREPORT], [$PACKAGE_BUGREPORT])
    _CS_EMIT_PACKAGEINFO([$1], [PACKAGE_LONGNAME], [$PACKAGE_LONGNAME])
    _CS_EMIT_PACKAGEINFO([$1], [PACKAGE_HOMEPAGE], [$PACKAGE_HOMEPAGE])
    _CS_EMIT_PACKAGEINFO([$1], [PACKAGE_COPYRIGHT], [$PACKAGE_COPYRIGHT])
    m4_define([cs_ver_components], m4_translit(AC_PACKAGE_VERSION, [.], [ ]))
    m4_case([$1],
    [make], [CS_MAKEFILE_PROPERTY([PACKAGE_VERSION_LIST], cs_ver_components)],
    [jam], [CS_JAMCONFIG_APPEND([PACKAGE_VERSION_LIST ?= cs_ver_components ;
])],
    [_CS_EMIT_PACKAGEINFO_FATAL([$1])])])

AC_DEFUN([_CS_EMIT_PACKAGEINFO],
    [m4_case([$1],
	[make], [CS_MAKEFILE_PROPERTY([$2], [$3], [$4])],
	[jam], [CS_JAMCONFIG_PROPERTY([$2], [$3], [$4])],
	[_CS_EMIT_PACKAGEINFO_FATAL([$1])])])

AC_DEFUN([_CS_EMIT_PACKAGEINFO_FATAL],
    [AC_FATAL([CS_EMIT_PACKAGEINFO: unrecognized emitter ([$1])])])
