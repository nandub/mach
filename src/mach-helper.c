/*
 * mach-helper.c: help mach perform tasks needing root privileges
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

char *rootsdir = ROOTSDIR;

void
usage ()
{
  printf ("Usage: mach-helper [command]\n");
  exit (1);
}

/* argv[0] should by convention be the binary name to be executed */
void
do_command (const char *filename, char *const argv[])
{
  /* do not trust user environment */
  char *env[] = { "PATH=/bin:/usr/bin:/usr/sbin", NULL };
  int retval;

  /* elevate privileges */
  setreuid (geteuid (), geteuid ());
  printf ("First argument: %s\n", *argv);
  printf ("Executing %s\n", filename);
  retval = execve (filename, argv, env);
  printf ("Error executing %s: %s\n", filename, strerror (errno));
}

void
do_chroot (int argc, char *argv[])
{
  int retval;
  struct stat buf;
  char last;

  if (argc < 3)
  {
    fprintf (stderr, "No directory given for chroot !\n");
    exit (1);
  }
  //printf ("DEBUG: rootsdir: %s\n", rootsdir);
  /* does the first argument start with the compiled-in rootsdir ? */
  if (strncmp (rootsdir, argv[2], strlen (rootsdir)) != 0)
  {
    fprintf (stderr, "Not chrooting to the right directory !\n");
    exit (1);
  }
  /* does it try to fool us by using . or .. ? */
  if (strstr (argv[2], ".") != NULL)
  {
    printf ("Error on %s: contains '.'\n", argv[2]);
    exit (1);
  }
  /* does it try to fool us into following symlinks by having a trailing / ? */
  last = argv[2][strlen (argv[2]) - 1];
  if (last == '/')
  {
    printf ("Error on %s: ends with '/'\n", argv[2]);
    exit (1);
  }
  /* are we chrooting to an actual directory (not a symlink or anything) ? */
  retval = lstat (argv[2], &buf);
  if (retval != 0)
  {
    printf ("Error on %s: %s\n", argv[2], strerror (errno));
    exit (1);
  }
  //printf ("DEBUG: mode: %o\n", buf.st_mode);
  if (S_ISLNK (buf.st_mode))
  {
    printf ("Error on %s: symbolic link\n", argv[2]);
    exit (1);
  }
  if (!(S_ISDIR (buf.st_mode)))
  {
    printf ("Error on %s: not a directory\n", argv[2]);
    exit (1);
  }
  
  do_command ("/usr/sbin/chroot", &(argv[1]));
}

int
main (int argc, char *argv[])
{

  /* verify input */
  if (argc < 2) usage ();
  
  /* see which command we are trying to run */
  if (strncmp ("chroot", argv[1], 6) == 0)
    do_chroot (argc, argv);
  else
  {
    fprintf (stderr, "Command %s not recognized !\n", argv[1]);
    exit (1);
  }
  exit (0);
}
