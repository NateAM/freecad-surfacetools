
lib_LTLIBRARIES=libSurfaceTools.la SurfaceTools.la

libSurfaceTools_la_SOURCES=\
		AppSurfaceToolsPy.cpp \
		PreCompiled.cpp \
		PreCompiled.h

includedir = @includedir@/Mod/SurfaceTools/App

# the library search path.
libSurfaceTools_la_LDFLAGS = -L../../../Base -L../../../App $(all_libraries) \
		-version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libSurfaceTools_la_CPPFLAGS = -DSurfaceToolsAppExport=

libSurfaceTools_la_LIBADD   = \
		@BOOST_SYSTEM_LIB@ \
		-l@PYTHON_LIB@ \
		-lxerces-c \
		-lFreeCADBase \
		-lFreeCADApp

#--------------------------------------------------------------------------------------
# Loader of libSurfaceTools

SurfaceTools_la_SOURCES=\
		AppSurfaceTools.cpp

# the library search path.
SurfaceTools_la_LDFLAGS = $(libSurfaceTools_la_LDFLAGS) -module -avoid-version
SurfaceTools_la_CPPFLAGS = $(libSurfaceTools_la_CPPFLAGS)

SurfaceTools_la_LIBADD   = \
		$(libSurfaceTools_la_LIBADD) \
		-lSurfaceTools

SurfaceTools_la_DEPENDENCIES = libSurfaceTools.la

#--------------------------------------------------------------------------------------

# set the include path found by configure
AM_CXXFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/src $(all_includes)

libdir = $(prefix)/Mod/SurfaceTools

EXTRA_DIST = \
		CMakeLists.txt
