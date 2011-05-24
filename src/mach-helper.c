/*
 * mach-helper.c: help mach perform tasks needing root privileges
 *
 * Copyright (C) 2003-2007 Thomas Vander Stichele
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#define _GNU_SOURCE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/* pull in configure'd defines */
char *rootsdir = ROOTSDIR;
char *statesdir = STATESDIR;
char *archivesdir = ARCHIVESDIR;
char *cachedir = LOCALSTATEDIR "/cache";

static char const * const ALLOWED_ENV[] =
{
  "APT_CONFIG",
  "dist",
  "ftp_proxy", "http_proxy", "https_proxy", "no_proxy"
};

#define ALLOWED_ENV_SIZE (sizeof (ALLOWED_ENV) / sizeof (ALLOWED_ENV[0]))

/*
 * helper functions
 */

void
usage ()
{
  printf ("Usage: mach-helper [command]\n");
  exit (1);
}

/* print formatted string to stderr, print newline and terminate */
void
error (const char *format, ...)
{
  va_list ap;

  va_start (ap, format);
  fprintf (stderr, "mach-helper: error: ");
  vfprintf (stderr, format, ap);
  va_end (ap);
  fprintf (stderr, "\n");
  exit (1);
}

/*
 * perform checks on the given dir
 * - is the given dir under the allowed hierarchy ?
 * - is it an actual dir ?
 * - are we not being tricked by using . or .. ?
 */
void
check_dir_allowed (const char *allowed, const char *given)
{
  struct stat buf;
  char last;
  int retval;

  /* does given start with allowed ? */
  if (strncmp (given, allowed, strlen (allowed)) != 0)
    error ("%s: not under allowed directory (%s)", given, allowed);

  /* does it try to fool us by using .. ? */
  if (strstr (given, "..") != 0)
    error ("%s: contains '..'", given);

  /* does it try to fool us into following symlinks by having a trailing / ? */
  last = given[strlen (given) - 1];
  if (last == '/')
    error ("%s: ends with '/'", given);

  /* are we chrooting to an actual directory (not a symlink or anything) ? */
  retval = lstat (given, &buf);
  if (retval != 0)
    error ("%s: %s", given, strerror (errno));

  //printf ("DEBUG: mode: %o\n", buf.st_mode);
  if (S_ISLNK (buf.st_mode))
    error ("%s: symbolic link", given);
  if (!(S_ISDIR (buf.st_mode)))
    error ("%s: not a directory", given);
}

/*
 * perform checks on the given file
 * - is the given file under the allowed hierarchy ?
 * - is it an actual file ?
 * - are we not being tricked by using .. ?
 */
void
check_file_allowed (const char *allowed, const char *given)
{
  struct stat buf;
  char last;
  int retval;

  /* does given start with allowed ? */
  if (strncmp (given, allowed, strlen (allowed)) != 0)
    error ("%s: not under allowed directory", given);

  /* does it try to fool us by using .. ? */
  if (strstr (given, "..") != 0)
    error ("%s: contains '..'", given);

  /* does it have a trailing / ? */
  last = given[strlen (given) - 1];
  if (last == '/')
    error ("%s: ends with '/'", given);

  /* are we working with an actual file ? */
  retval = lstat (given, &buf);
  if (retval != 0)
    error ("%s: %s", given, strerror (errno));

  //printf ("DEBUG: mode: %o\n", buf.st_mode);
  if (S_ISLNK (buf.st_mode))
    error ("%s: symbolic link", given);
  if (!(S_ISREG (buf.st_mode)))
    error ("%s: not a regular file", given);
}

/* argv[0] should by convention be the binary name to be executed */
void
do_command (const char *filename, char *const argv[])
{
  /* do not trust user environment;
   * copy over allowed env vars, after setting PATH and HOME ourselves
   */
  char *env[3 + ALLOWED_ENV_SIZE] = {
    [0] = "PATH=/bin:/usr/bin:/usr/sbin",
    [1] = "HOME=/root"
  };
  size_t idx=2;
  size_t i;
  char *envvar;
  char *ld_preload;
  struct stat buf;

  /* elevate privileges */
  setreuid (geteuid (), geteuid ());
  //printf ("DEBUG: First argument: %s\n", *argv);
  //printf ("DEBUG: Executing %s\n", filename);
  /* FIXME: for a debug option */
  /*
  printf ("Executing %s ", filename);
  char **arg;
  for (arg = (char **) &(argv[1]); *arg; ++arg)
    printf ("%s ", *arg);
  printf ("\n");
  */

  /* add LD_PRELOAD for our selinux lib if MACH_LD_PRELOAD is set, or if
   * /selinux exists */
  envvar = getenv ("MACH_LD_PRELOAD");
  if (envvar != 0 || stat ("/selinux", &buf) == 0)
  {
    ld_preload = strdup("LD_PRELOAD=" LIBDIR "/libselinux-mach.so");
    env[idx++] = ld_preload;
  }

  for (i = 0; i < ALLOWED_ENV_SIZE; ++i)
  {
    char *ptr = getenv (ALLOWED_ENV[i]);
    if (ptr==0) continue;
    ptr -= strlen (ALLOWED_ENV[i]) + 1;
    env[idx++] = ptr;
  }

  execve (filename, argv, env);
  error ("executing %s: %s", filename, strerror (errno));
}

/*
 * actual command implementations
 */

/* allow apt-get -c (statesdir/.../apt.conf) ... */
void
do_apt_get (int argc, char *argv[])
{
  /* enough arguments ? mach-helper apt-get -c (statesdir/.../apt.conf) .., 5 */
  if (argc < 5)
    error ("not enough arguments");

  /* -c */
  if (strncmp ("-c", argv[2], 2) != 0)
    error ("%s: options not allowed", argv[2]);

  /* check given file */
  check_file_allowed (statesdir, argv[3]);

  do_command ("/usr/bin/apt-get", &(argv[1]));
}

/* allow yum -c (statesdir/.../yum.conf) ... */
void
do_yum (int argc, char *argv[])
{
  char *installroot = NULL;

  /* enough arguments ?
   * mach-helper yum -c (statesdir/.../yum.conf) --installroot=..., 6 */
  if (argc < 6)
    error ("not enough arguments");

  /* -c */
  if (strncmp ("-c", argv[2], 2) != 0)
    error ("%s: options not allowed", argv[2]);

  /* check given file */
  check_file_allowed (statesdir, argv[3]);

  /* installroot */
  if (strncmp ("--installroot=", argv[4], 14) != 0)
    error ("%s: option not allowed", argv[4]);

  installroot = argv[4] + 14;
  check_dir_allowed (rootsdir, installroot);

  do_command ("/usr/bin/yum", &(argv[1]));
}

void
do_chroot (int argc, char *argv[])
{
  if (argc < 3)
    error ("No directory given for chroot !");
  //printf ("DEBUG: rootsdir: %s\n", rootsdir);

  /* do we allow this dir ? */
  check_dir_allowed (rootsdir, argv[2]);

  do_command ("/usr/sbin/chroot", &(argv[1]));
}

/*
 * allow bind mounts of the archives or selinux dir:
 * mount -o bind (archivesdir) (root)/var/cache/apt/archives
 * mount -o bind /selinux (root)/selinux
 * allow proc mounts:
 * mount -t proc proc (root)/proc
 */
void
do_mount (int argc, char *argv[])
{
  /* see if we have enough arguments for it to be what we want, ie. 5 */
  if (argc < 5)
    error ("not enough arguments");

  /* see if it's -o bind or -t proc */
  if ((strncmp ("-o", argv[2], 2) == 0) && (strncmp ("bind", argv[3], 4) == 0))
  {
    /* see if it's the archivesdir or selinux */
    if ((strncmp (archivesdir, argv[4], strlen (archivesdir)) != 0) &&
        (strncmp ("/selinux", argv[4], strlen ("/selinux")) != 0))
    {
      error ("%s: mount not allowed", argv[4]);
    }
  }
  else if ((strncmp ("-t", argv[2], 2) == 0) &&
           (strncmp ("proc", argv[3], 4) == 0))
  {
    /* see if we're mounting proc to somewhere in rootsdir */
    if (strncmp (rootsdir, argv[5], strlen (rootsdir)) != 0)
      error ("proc: mount not allowed on %s", argv[5]);
  }
  else
    error ("unallowed mount type");

  /* all checks passed, execute */
  do_command ("/bin/mount", &(argv[1]));
}

void
do_rm_cache_file (int argc, char *argv[])
{
  /* see if we're doing rm on a file under cachedir */
  check_file_allowed (cachedir, argv[2]);

  /* all checks passed, execute */
  do_command ("/bin/rm", &(argv[1]));
}

void
do_rm_cache_dir (int argc, char *argv[])
{
  /* see if we're doing rm -rfv */
  if (strncmp ("-rfv", argv[2], 4) != 0)
    error ("%s: options not allowed", argv[2]);

  /* see if we're doing -rfv on a dir under rootsdir */
  check_dir_allowed (cachedir, argv[3]);

  /* all checks passed, execute */
  do_command ("/bin/rm", &(argv[1]));
}

void
do_rm_root_dir (int argc, char *argv[])
{
  /* see if we're doing rm -rfv */
  if (strncmp ("-rfv", argv[2], 4) != 0)
    error ("%s: options not allowed", argv[2]);

  /* see if we're doing -rfv on a dir under rootsdir */
  check_dir_allowed (rootsdir, argv[3]);

  /* all checks passed, execute */
  do_command ("/bin/rm", &(argv[1]));
}

/* clean out a chroot dir, or remove a mach cache file
 * rm cachedir/mach/somefile
 * rm -rfv (cachedir)
 * rm -rfv (rootdir)
 */


void
do_rm (int argc, char *argv[])
{
  if (argc == 3) {
    do_rm_cache_file (argc, argv);
    return;
  }

  if (argc != 4)
      error ("wrong number of parameters");

  if (strncmp (argv[3], rootsdir, strlen (rootsdir)) == 0)
    do_rm_root_dir (argc, argv);
  else if (strncmp (argv[3], cachedir, strlen (cachedir)) == 0)
    do_rm_cache_dir (argc, argv);
  else
    error ("%s: not under allowed directories", argv[3]);
}

/* perform rpm commands on root */
void
do_rpm (int argc, char *argv[])
{
  const char *rpm_bin = NULL;
  struct stat buf;

  /* enough arguments ? mach-helper rpm --root (rootdir) ... , 4 */
  if (argc < 4)
    error ("not enough arguments");

  /* --root */
  if (strncmp ("--root", argv[2], 6) != 0)
    error ("%s: options not allowed", argv[2]);

  /* Find our rpm binary */
  if (stat ("/bin/rpm", &buf) == 0)
    rpm_bin = "/bin/rpm";
  else if (stat ("/usr/bin/rpm", &buf) == 0)
    rpm_bin = "/usr/bin/rpm";
  else
    error ("Could not locate rpm binary in /bin or /usr/bin");

  /* check given dir */
  check_dir_allowed (rootsdir, argv[3]);

  /* all checks passed, execute */
  do_command (rpm_bin, &(argv[1]));
}


/* unmount archivesdir and proc */
void
do_umount (int argc, char *argv[])
{
  /* enough arguments ? mach-helper umount (dir), 3 */
  if (argc < 3)
    error ("not enough arguments");

  /* see if we're unmounting from somewhere in rootsdir */
  check_dir_allowed (rootsdir, argv[2]);

  /* all checks passed, execute */
  do_command ("/bin/umount", &(argv[1]));
}

/* make /dev/null or /dev/zero device node */
void
do_mknod (int argc, char *argv[])
{
  /* enough arguments ? mach-helper mknod (name) -m (mode) (type) (major) (minor), 8 */
  if (argc < 8)
    error ("not enough arguments");

  /* check given file */
  if (strncmp (argv[2], rootsdir, strlen (rootsdir)) != 0)
    error ("%s: not under allowed directory (%s)", argv[2], rootsdir);

  /* does it try to fool us by using .. ? */
  if (strstr (argv[2], "..") != 0)
    error ("%s: contains '..'", argv[2]);

  /* does it have a trailing / ? */
  int last = argv[2][strlen (argv[2]) - 1];
  if (last == '/')
    error ("%s: ends with '/'", argv[2]);

  /* -m */
  if (strncmp ("-m", argv[3], 2) != 0)
    error ("%s: options not allowed", argv[3]);

  /* type */
  if (strncmp ("666", argv[4], 3) != 0)
    error ("%s: options not allowed", argv[4]);

  /* type */
  if (strncmp ("c", argv[5], 1) != 0)
    error ("%s: options not allowed", argv[5]);

  /* major */
  if (strncmp ("1", argv[6], 1) != 0)
    error ("%s: options not allowed", argv[6]);

  /* minor */
  if (strlen(argv[7]) != 1)
    error ("%s: options not allowed", argv[7]);

  if (*argv[7] != '3' && *argv[7] != '5' && *argv[7] != '8' && *argv[7] != '9')
    error ("%s: options not allowed", argv[7]);

  /* all checks passed, execute */
  do_command ("/bin/mknod", &(argv[1]));
}

/* allow showing the environment, but nothing else */
void
do_env (int argc, char *argv[])
{
  /* enough arguments ? mach-helper mknod (name) -m (mode) (type) (major) (minor), 8 */
  if (argc > 2)
    error ("too many arguments");

  do_command ("/bin/env", &(argv[1]));
}

int
main (int argc, char *argv[])
{

  /* verify input */
  if (argc < 2) usage ();

  /* see which command we are trying to run */
  if (strncmp ("chroot", argv[1], 6) == 0)
    do_chroot (argc, argv);
  else if (strncmp ("mount", argv[1], 5) == 0)
    do_mount (argc, argv);
  else if (strncmp ("rm", argv[1], 2) == 0)
    do_rm (argc, argv);
  else if (strncmp ("umount", argv[1], 6) == 0)
    do_umount (argc, argv);
  else if (strncmp ("rpm", argv[1], 3) == 0)
    do_rpm (argc, argv);
  else if (strncmp ("apt-get", argv[1], 7) == 0)
    do_apt_get (argc, argv);
  else if (strncmp ("yum", argv[1], 3) == 0)
    do_yum (argc, argv);
  else if (strncmp ("mknod", argv[1], 5) == 0)
    do_mknod (argc, argv);
  else if (strncmp ("env", argv[1], 3) == 0)
    do_env (argc, argv);
  else
  {
    error ("Command %s not recognized !\n", argv[1]);
    exit (1);
  }
  exit (0);
}
