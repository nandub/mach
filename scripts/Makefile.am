bin_SCRIPTS = mach
sbin_SCRIPTS = mach-helper

EXTRA_DIST = $(bin_SCRIPTS) $(sbin_SCRIPTS)

all: all-am chmod

chmod: $(bin_SCRIPTS) $(sbin_SCRIPTS)
	chmod +x $^
