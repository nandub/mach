dnl check-release.m4
dnl check for a Fedora/Red Hat release file
dnl check for file /etc/$1-release containing string $2.
dnl execute $3 if found and $4 if not.

AC_DEFUN([CHECK_RELEASE],
[
  DIST=[$1]
  NAME=[$2]

  if test -f /etc/$DIST-release
  then
    AC_MSG_CHECKING(/etc/$DIST-release for $NAME)
    if grep $NAME /etc/$DIST-release > /dev/null; then
      AC_MSG_RESULT(found)
      ifelse([$3], , :, [$3])
    else
      AC_MSG_RESULT(not found)
      ifelse([$4], , :, [$4])
    fi
  fi
])
