dnl $Id$
dnl config.m4 for extension mmseg

PHP_ARG_WITH(mmseg, for mmseg support,
Make sure that the comment is aligned:
[  --with-mmseg             Include mmseg support])

if test "$PHP_MMSEG" != "no"; then

  dnl # --with-mmseg -> check with-path
  SEARCH_PATH="/usr/local /usr /opt /opt/mmseg"   
  SEARCH_FOR="/include/mmseg/SegmenterManager.h" 
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

  # --with-mmseg -> check for lib and symbol presence
  LIBNAME=mmseg 
  LIBSYMBOL=mmseg 

  PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $MMSEG_DIR/lib, MMSEG_SHARED_LIBADD)
  PHP_SUBST(MMSEG_SHARED_LIBADD)
  PHP_REQUIRE_CXX() 
  PHP_ADD_LIBRARY(stdc++, 1, EXTRA_LDFLAGS)  
  PHP_NEW_EXTENSION(mmseg, mmseg.cpp, $ext_shared,, -Wno-deprecated)
fi
