sbin_PROGRAMS = mach-helper

mach_helper_SOURCES = mach-helper.c
AM_CFLAGS = -Wall -Werror

if SELINUX_ENABLED
lib_LTLIBRARIES = libselinux-mach.la

libselinux_mach_la_SOURCES = selinux-mach.c
else
endif
