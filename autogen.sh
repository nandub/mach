#!/bin/sh
set -x
aclocal -I m4
# libtoolize --force
# autoheader
autoconf
automake -a
echo "./autogen.sh $@" > autoregen.sh
chmod +x autoregen.sh
./configure $@
