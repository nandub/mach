/*
 * mach-helper.c: help mach perform tasks needing root privileges
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>


/* pull in configure'd defines */
char *rootsdir = ROOTSDIR;
char *statesdir = STATESDIR;
char *archivesdir = ARCHIVESDIR;

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
    error ("%s: not under allowed directory", given);

  /* does it try to fool us by using . or .. ? */
  if (strstr (given, ".") != NULL)
    error ("%s: contains '.'", given);

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
 * - are we not being tricked by using . or .. ?
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

  /* does it try to fool us by using . or .. ? */
  /* FIXME: this also excludes dot files, but that's not really a problem */
  if (strstr (given, "/.") != NULL)
    error ("%s: contains '/.'", given);

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
  /* do not trust user environment */
  char *env[] = { "PATH=/bin:/usr/bin:/usr/sbin", NULL };
  int retval;
  char **arg;

  /* elevate privileges */
  setreuid (geteuid (), geteuid ());
  //printf ("DEBUG: First argument: %s\n", *argv);
  //printf ("DEBUG: Executing %s\n", filename);
  /* FIXME: for a debug option */
  /*
  printf ("Executing %s ", filename);
  for (arg = (char **) &(argv[1]); *arg; ++arg)
    printf ("%s ", *arg);
  printf ("\n");
  */
  retval = execve (filename, argv, env);
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
 * allow bind mounts of the archives dir:
 * mount -o bind (archivesdir) (root)/var/cache/apt/archives
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
    /* see if it's the archivesdir */
    if (strncmp (archivesdir, argv[4], strlen (archivesdir)) != 0)
      error ("%s: mount not allowed", argv[4]);
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

/* clean out a chroot dir */
void
do_rm (int argc, char *argv[])
{
  /* enough arguments ? mach-helper rm -rfv (rootdir), 4 */
  if (argc < 4)
    error ("not enough arguments");

  /* see if we're doing rm -rfv */
  if (strncmp ("-rfv", argv[2], 4) != 0)
    error ("%s: options not allowed", argv[2]);

  /* see if we're doing -rfv on a dir under rootsdir */
  check_dir_allowed (rootsdir, argv[3]);

  /* all checks passed, execute */
  do_command ("/bin/rm", &(argv[2]));
}

/* perform rpm commands on root */
void
do_rpm (int argc, char *argv[])
{
  /* enough arguments ? mach-helper rpm --root (rootdir) ... , 4 */
  if (argc < 4)
    error ("not enough arguments");

  /* --root */
  if (strncmp ("--root", argv[2], 6) != 0)
    error ("%s: options not allowed", argv[2]);

  /* check given dir */
  check_dir_allowed (rootsdir, argv[3]);

  /* all checks passed, execute */
  do_command ("/bin/rpm", &(argv[1]));
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
  else
  {
    error ("Command %s not recognized !\n", argv[1]);
    exit (1);
  }
  exit (0);
}
