#------------------------------------------------------------------------------
# Orginial Macros Copyright 2003 Eric Sunshine <sunshine@sunshineco.com>
# Determine host platform.  Recognized families: Unix, Windows, MacOS/X.
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Determine host CPU.
#
# CS_CHECK_HOST_CPU
#       Set the shell variable cs_host_cpu to a normalized form of the CPU name
#       returned by config.guess/config.sub.  Typically, Crystal Space's
#       conception of CPU name is the same as that returned by
#       config.guess/config.sub, but there may be exceptions as seen in the
#       `case' statement.  Also takes the normalized name, uppercases it to
#       form a name suitable for the C preprocessor.  Additionally sets the
#       TARGET.PROCESSOR Jamconfig property.
#------------------------------------------------------------------------------
AC_DEFUN([CS_CHECK_HOST_CPU],
    [AC_REQUIRE([AC_CANONICAL_HOST])
    case $host_cpu in
        [[Ii][3-9]86*|[Xx]86*]) cs_host_cpu=x86 ;;
        *) cs_host_cpu=$host_cpu ;;
    esac
    cs_host_cpu_normalized="AS_TR_CPP([$cs_host_cpu])"
    CS_JAMCONFIG_PROPERTY([TARGET.PROCESSOR], [$cs_host_cpu_normalized])
    ])


#------------------------------------------------------------------------------
# CS_CHECK_HOST
#       Sets the shell variables cs_host_target cs_host_family,
#       cs_host_os_normalized, and cs_host_os_normalized_uc.  Emits appropriate
#       OS_UNIX, OS_WIN32, OS_MACOSX via AC_DEFINE(), and TARGET.OS and
#       TARGET.OS.NORMALIZED to Jamconfig.
#------------------------------------------------------------------------------
AC_DEFUN([CS_CHECK_HOST],
    [AC_REQUIRE([AC_CANONICAL_HOST])
    CS_CHECK_HOST_CPU
    cs_host_os_normalized=''
    case $host_os in
        mingw*|cygwin*)
            cs_host_target=win32gcc
            cs_host_family=windows
            ;;
        darwin*)
            _CS_CHECK_HOST_DARWIN
            ;;
        *)
            # Everything else is assumed to be Unix or Unix-like.
            cs_host_target=unix
            cs_host_family=unix
	    ;;
    esac

    case $cs_host_family in
	windows)
            AC_DEFINE([OS_WIN32], [], [define when compiling for Win32])
	    AS_IF([test -z "$cs_host_os_normalized"],
		[cs_host_os_normalized='Win32'])
	    ;;
	unix)
            AC_DEFINE([OS_UNIX], [],
		[define when compiling for Unix and Unix-like (i.e. MacOS/X)])
	    AS_IF([test -z "$cs_host_os_normalized"],
		[cs_host_os_normalized='Unix'])
	    ;;
    esac

    cs_host_os_normalized_uc="AS_TR_CPP([$cs_host_os_normalized])"
    CS_JAMCONFIG_PROPERTY([TARGET.OS], [$cs_host_os_normalized_uc])
    CS_JAMCONFIG_PROPERTY([TARGET.OS.NORMALIZED], [$cs_host_os_normalized])
])

AC_DEFUN([_CS_CHECK_HOST_DARWIN],
    [AC_REQUIRE([AC_PROG_CC])
    AC_REQUIRE([AC_PROG_CXX])

    # Both MacOS/X and Darwin are identified via $host_os as "darwin".  We need
    # a way to distinguish between the two.  If Carbon.h is present, then
    # assume MacOX/S; if not, assume Darwin.  If --with-x=yes was invoked, and
    # Carbon.h is present, then assume that user wants to cross-build for
    # Darwin even though build host is MacOS/X.  Implementation note: At least
    # one MacOS/X user switches between gcc 2.95 and gcc 3.3 with a script
    # which toggles the values of CC, CXX, and CPP.  Unfortunately, CPP was
    # being set to run the preprocessor directly ("cpp", for instance) rather
    # than running it via the compiler ("gcc -E", for instance).  The problem
    # with running the preprocessor directly is that __APPLE__ and __GNUC__ are
    # not defined, which causes the Carbon.h check to fail.  We avoid this
    # problem by supplying a non-empty fourth argument to AC_CHECK_HEADER(),
    # which causes it to test compile the header only (which is a more robust
    # test), rather than also testing it via the preprocessor.

    AC_CHECK_HEADER([Carbon/Carbon.h],
	[cs_host_macosx=yes], [cs_host_macosx=no], [/* force compile */])

    AS_IF([test $cs_host_macosx = yes],
	[AC_MSG_CHECKING([for --with-x])
	AS_IF([test "${with_x+set}" = set && test "$with_x" = "yes"],
	    [AC_MSG_RESULT([yes (assume Darwin)])
	    cs_host_macosx=no],
	    [AC_MSG_RESULT([no])])])

    AS_IF([test $cs_host_macosx = yes],
	[cs_host_target=macosx
	cs_host_family=unix
	cs_host_os_normalized='MacOS/X'
        AC_DEFINE([OS_MACOSX], [], [define when compiling for MacOS/X])

	AC_CACHE_CHECK([for Objective-C compiler], [cs_cv_prog_objc],
	    [cs_cv_prog_objc="$CC"])
	CS_JAMCONFIG_PROPERTY([CMD.OBJC], [$cs_cv_prog_objc])
	AC_CACHE_CHECK([for Objective-C++ compiler], [cs_cv_prog_objcxx],
	    [cs_cv_prog_objcxx="$CXX"])
	CS_JAMCONFIG_PROPERTY([CMD.OBJC++], [$cs_cv_prog_objcxx])],

	[cs_host_target=unix
	cs_host_family=unix])])
