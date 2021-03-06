#!/bin/sh
set -x
aclocal -I m4 || exit 1
libtoolize --force || exit 1
autoheader || exit 1
autoconf || exit 1
automake -a || exit 1
echo "./autogen.sh $@" > autoregen.sh
chmod +x autoregen.sh
./configure $@
