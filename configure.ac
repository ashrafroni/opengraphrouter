AC_INIT([opengraphrouter],[0.1.0],[routergeocoder@lists.osgeo.org])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CXX
AC_SEARCH_LIBS([OGRRegisterAll], [gdal gdal1.5.0], [],
  AC_MSG_ERROR([Error: gdal library can not be found.]))
AC_SEARCH_LIBS([FCGI_Accept], [fcgi], [],
  AC_MSG_ERROR([Error: fcgi library can not be found.]))
AC_CONFIG_SRCDIR([src/opengraphrouter.cpp])
AC_SUBST([LIBS])
AC_OUTPUT([Makefile src/Makefile])
