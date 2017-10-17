dnl $Id$
dnl config.m4 for extension mmseg

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(mmseg, for mmseg support,
Make sure that the comment is aligned:
[  --with-mmseg             Include mmseg support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(mmseg, whether to enable mmseg support,
dnl Make sure that the comment is aligned:
dnl [  --enable-mmseg           Enable mmseg support])

if test "$PHP_MMSEG" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-mmseg -> check with-path
  SEARCH_PATH="/usr/local /usr /opt /opt/mmseg"     # you might want to change this
  SEARCH_FOR="/include/mmseg/SegmenterManager.h"  # you most likely want to change this
  if test -r $PHP_MMSEG/$SEARCH_FOR; then # path given as parameter
    MMSEG_DIR=$PHP_MMSEG
  else # search default path list
    AC_MSG_CHECKING([for mmseg files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        MMSEG_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi
  
  if test -z "$MMSEG_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the mmseg distribution])
  fi

  dnl # --with-mmseg -> add include path
  PHP_ADD_INCLUDE($MMSEG_DIR/include/mmseg)


  dnl # --with-mmseg -> check for lib and symbol presence
  LIBNAME=mmseg # you may want to change this
  LIBSYMBOL=mmseg # you most likely want to change this 

  PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $MMSEG_DIR/lib, MMSEG_SHARED_LIBADD)
  PHP_SUBST(MMSEG_SHARED_LIBADD)
  PHP_REQUIRE_CXX() 
  PHP_ADD_LIBRARY(stdc++, 1, EXTRA_LDFLAGS)  
  PHP_NEW_EXTENSION(mmseg, mmseg.cpp, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
