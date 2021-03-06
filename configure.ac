AC_INIT(README)

AS_VERSION(mach, MACH, 1, 0, 4, 1)
AC_SUBST(RELEASE, $MACH_RELEASE)

AC_CANONICAL_SYSTEM

AM_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_CONFIG_SRCDIR([src/mach-helper.c])
AM_CONFIG_HEADER(config.h)

AC_SUBST_FILE(AUTHORS)
AUTHORS=$srcdir/AUTHORS

AM_INIT_AUTOMAKE($PACKAGE, $VERSION)
AC_SUBST(ACLOCAL_AMFLAGS, "-I m4")

AS_AC_EXPAND(SYSCONFDIR, $sysconfdir)
AC_DEFINE_UNQUOTED(SYSCONFDIR, "$SYSCONFDIR",
    [Location of configuration files])
AC_MSG_NOTICE(Storing configuration files in $SYSCONFDIR)

AS_AC_EXPAND(LOCALSTATEDIR, $localstatedir)
AC_DEFINE_UNQUOTED(LOCALSTATEDIR, "$LOCALSTATEDIR",
    [Location of local state files])
AC_MSG_NOTICE(Using localstatedir $LOCALSTATEDIR)

AS_AC_EXPAND(LIBDIR, $libdir)
AC_DEFINE_UNQUOTED(LIBDIR, "$LIBDIR",
    [Location of libraries])
AC_MSG_NOTICE(Using libdir $LIBDIR)

dnl rootsdir
AC_ARG_ENABLE(rootsdir,
  AC_HELP_STRING([--enable-rootsdir=PATH], [create roots in PATH]),
  [ rootsdir=$enableval ],
  [ rootsdir=$localstatedir/lib/mach/roots]
)
AS_AC_EXPAND(ROOTSDIR, $rootsdir)
AC_DEFINE_UNQUOTED(ROOTSDIR, "$ROOTSDIR",
    [Location of mach roots])
AC_MSG_NOTICE(Using rootsdir $ROOTSDIR)

dnl archivesdir
AC_ARG_ENABLE(archivesdir,
  AC_HELP_STRING([--enable-archivesdir=PATH], [store archives in PATH]),
  [ archivesdir=$enableval ],
  [ archivesdir=$localstatedir/cache/mach/archives]
)
AS_AC_EXPAND(ARCHIVESDIR, $archivesdir)
AC_DEFINE_UNQUOTED(ARCHIVESDIR, "$ARCHIVESDIR",
    [Location of cached packages])
AC_MSG_NOTICE(Using archivesdir $ARCHIVESDIR)

dnl statesdir
AC_ARG_ENABLE(statesdir,
  AC_HELP_STRING([--enable-statesdir=PATH], [use states in PATH]),
  [ statesdir=$enableval ],
  [ statesdir=$localstatedir/lib/mach/states]
)
AS_AC_EXPAND(STATESDIR, $statesdir)
AC_DEFINE_UNQUOTED(STATESDIR, "$STATESDIR",
    [Location of root states])
AC_MSG_NOTICE(Using statesdir $STATESDIR)

dnl builduser
AC_ARG_ENABLE(builduser,
  AC_HELP_STRING([--enable-builduser=USERNAME], [use USERNAME for builds]),
  [ BUILDUSER=$enableval ],
  [ BUILDUSER=machbuild ]
)
AC_SUBST(BUILDUSER)
AC_MSG_NOTICE(Using build username $BUILDUSER)

dnl buildgroup
AC_ARG_ENABLE(buildgroup,
  AC_HELP_STRING([--enable-buildgroup=GROUPNAME], [use GROUPNAME for builds]),
  [ BUILDGROUP=$enableval ],
  [ BUILDGROUP=machbuild ]
)
AC_SUBST(BUILDGROUP)
AC_MSG_NOTICE(Using build groupname $BUILDGROUP)

dnl arch
arch=i386
case "$target" in
    i?86-*|athlon-*)   arch=i386 ;;
    x86_64-*)          arch=x86_64 ;;
    powerpc*)          arch=ppc ;;
esac

dnl default root
dnl first try to guess the platform
CHECK_RELEASE(fedora,    "(Rawhide)",    defaultroot="fedora-development-$arch")

CHECK_RELEASE(fedora,    "(Twenty Two)",        defaultroot="fedora-22-$arch")
CHECK_RELEASE(fedora,    "(Twenty One)",        defaultroot="fedora-21-$arch")
CHECK_RELEASE(fedora,    "(Heisenbug)",         defaultroot="fedora-20-$arch")
CHECK_RELEASE(fedora,    "(Schrödinger’s Cat)", defaultroot="fedora-19-$arch")
CHECK_RELEASE(fedora,    "(Spherical Cow)",     defaultroot="fedora-18-$arch")
CHECK_RELEASE(fedora,    "(Beefy Miracle)",     defaultroot="fedora-17-$arch")
CHECK_RELEASE(fedora,    "(Verne)",             defaultroot="fedora-16-$arch")
CHECK_RELEASE(fedora,    "(Lovelock)",          defaultroot="fedora-15-$arch")
CHECK_RELEASE(fedora,    "(Laughlin)",          defaultroot="fedora-14-$arch")
CHECK_RELEASE(fedora,    "(Goddard)",           defaultroot="fedora-13-$arch")
CHECK_RELEASE(fedora,    "(Constantine)",       defaultroot="fedora-12-$arch")
CHECK_RELEASE(fedora,    "(Leonidas)",          defaultroot="fedora-11-$arch")
CHECK_RELEASE(fedora,    "(Cambridge)",         defaultroot="fedora-10-$arch")
CHECK_RELEASE(fedora,    "(Sulphur)",           defaultroot="fedora-9-$arch")
CHECK_RELEASE(fedora,    "(Werewolf)",          defaultroot="fedora-8-$arch")
CHECK_RELEASE(fedora,    "(Moonshine)",         defaultroot="fedora-7-$arch")
CHECK_RELEASE(fedora,    "(Zod)",               defaultroot="fedora-6-$arch")
CHECK_RELEASE(fedora,    "(Bordeaux)",          defaultroot="fedora-5-$arch")
CHECK_RELEASE(fedora,    "(Stentz)",            defaultroot="fedora-4-$arch")
CHECK_RELEASE(fedora,    "(Heidelberg)",        defaultroot="fedora-3-$arch")
CHECK_RELEASE(fedora,    "(Tettnang)",          defaultroot="fedora-2-$arch")
CHECK_RELEASE(fedora,    "(Yarrow)",            defaultroot="fedora-1-$arch")

CHECK_RELEASE(redhat,    "(Guinness)",          defaultroot="redhat-70-$arch")
CHECK_RELEASE(redhat,    "(Seawolf)",           defaultroot="redhat-71-$arch")
CHECK_RELEASE(redhat,    "(Enigma)",            defaultroot="redhat-72-$arch")
CHECK_RELEASE(redhat,    "(Valhalla)",          defaultroot="redhat-73-$arch")
CHECK_RELEASE(redhat,    "(Psyche)",            defaultroot="redhat-80-$arch")
CHECK_RELEASE(redhat,    "(Shrike)",            defaultroot="redhat-9-$arch")

CHECK_RELEASE(redhat, "CentOS release 3",       defaultroot="centos-3-$arch")
CHECK_RELEASE(redhat,    "(Taroon)",            defaultroot="centos-3-$arch")
CHECK_RELEASE(redhat, "CentOS release 4",       defaultroot="centos-4-$arch")
CHECK_RELEASE(redhat,    "(Nahant)",            defaultroot="centos-4-$arch")
CHECK_RELEASE(redhat, "CentOS release 5",       defaultroot="centos-5-$arch")
CHECK_RELEASE(redhat,    "(Tikanga)",           defaultroot="centos-5-$arch")

CHECK_RELEASE(yellowdog, "(Sirius)",            defaultroot="yellowdog-30-ppc")

dnl now append a modifier if asked for

AC_ARG_WITH(flavor,
  AC_HELP_STRING([--with-flavor=FLAVOR], [set flavor of root to FLAVOR]),
  [ flavor="${withval}" ],
  [ flavor=]
)
if test "x$flavor" != "x"
then
  defaultroot="$defaultroot-$flavor"
fi

AC_ARG_WITH(defaultroot,
  AC_HELP_STRING([--with-defaultroot=ROOT], [set default root to ROOT]),
  [ defaultroot=${withval} ]
)
AS_AC_EXPAND(DEFAULTROOT, $defaultroot)
AC_MSG_NOTICE(Using default root $DEFAULTROOT)

AC_MSG_CHECKING([if the system is running selinux])
if test -d /selinux
then
  detect_selinux=yes
else
  if test -d /sys/fs/selinux
  then
    detect_selinux=yes
  else
    detect_selinux=no
  fi
fi
AC_MSG_RESULT($detect_selinux)


AC_ARG_ENABLE(selinux,
  AC_HELP_STRING([--enable-selinux], [enable support for selinux hosts]),
    enable_selinux=$enableval,enable_selinux=$detect_selinux
)
AC_MSG_NOTICE(Compiling with SELinux support: $enable_selinux)

AM_CONDITIONAL(SELINUX_ENABLED, test "x$enable_selinux" = "xyes")
AC_CHECK_HEADERS([selinux/selinux.h], HAVE_SELINUX_H=yes)

if test "x${enable_selinux}" = "xyes" -a "x${HAVE_SELINUX_H}" != "xyes";
then
  AC_MSG_ERROR([To compile SELinux support, you need selinux/selinux.h])
fi

if test "x${enable_selinux}" = "xyes";
then
  # detect prototype of lsetfilecon
  AC_MSG_CHECKING([for lsetfilecon prototype with security_context_t])
  AC_COMPILE_IFELSE(
    [
      AC_LANG_PROGRAM(
        [[#include <selinux/selinux.h>]],
        [[extern int lsetfilecon (const char * path, security_context_t con);]]
    )],
    [
      AC_MSG_RESULT([yes])
      AC_DEFINE_UNQUOTED([HAVE_LSETFILECON_TYPED], 1,
        [Define to 1 if lsetfilecon takes security_context_t as second arg.])
    ],
    [
      AC_MSG_RESULT([no])
      AC_DEFINE_UNQUOTED([HAVE_LSETFILECON_TYPED], 0,
        [Define to 1 if lsetfilecon takes security_context_t as second arg.])
    ]
  )
fi

AS_AC_EXPAND(DEFAULTROOT, $defaultroot)
AC_MSG_NOTICE(Using default root $DEFAULTROOT)

AC_ARG_WITH(pkgdir,
  AC_HELP_STRING([--with-pkgdir=PKGDIR], [set usrsrc distro dir to PKGDIR]),
  [ pkgdir="${withval}" ],
  [ pkgdir=]
)
if test "x$pkgdir" != "x"
then
  pkgdir="$pkgdir"
else
  pkgdir="rpm"
fi

AS_AC_EXPAND(PKGDIR, $pkgdir)
AC_MSG_NOTICE(Using packagedir $pkgdir)

AS_AC_EXPAND(SBINDIR, $sbindir)
AC_MSG_NOTICE(Placing mach-helper in $SBINDIR)

dnl check for C compiler
AC_PROG_CC

dnl check for python
AS_PATH_PYTHON(2.2)
if test "x$PYTHON" == "x"; then
  AC_MSG_ERROR(No suitable python version found)
fi

dnl output stuff
AC_OUTPUT(
Makefile
etc/Makefile
etc/conf
etc/dist.d/Makefile
doc/Makefile
m4/Makefile
mach.spec
scripts/Makefile
scripts/mach
src/Makefile
)
