
lib_LTLIBRARIES=libSurfaceToolsGui.la SurfaceToolsGui.la

BUILT_SOURCES=

libSurfaceToolsGui_la_SOURCES=\
		AppSurfaceToolsGuiPy.cpp \
		Command.cpp \
		PreCompiled.cpp \
		PreCompiled.h \
		Workbench.cpp \
		Workbench.h

includedir = @includedir@/Mod/SurfaceTools/Gui

# the library search path.
libSurfaceToolsGui_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Gui -L../App \
		$(QT_LIBS) $(all_libraries) \
		$(sim_ac_coin_ldflags) $(sim_ac_coin_libs) \
		$(sim_ac_soqt_ldflags) $(sim_ac_soqt_libs) \
		-version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@

libSurfaceToolsGui_la_CPPFLAGS = -DSurfaceToolsAppExport= -DSurfaceToolsGuiExport=

libSurfaceToolsGui_la_LIBADD   = \
		@BOOST_SYSTEM_LIB@ \
		-l@PYTHON_LIB@ \
		-lxerces-c \
		-lFreeCADBase \
		-lFreeCADApp \
		-lFreeCADGui \
		-lSurfaceTools

#--------------------------------------------------------------------------------------
# Loader of libSurfaceToolsGui

SurfaceToolsGui_la_SOURCES=\
		AppSurfaceToolsGui.cpp

# the library search path.
SurfaceToolsGui_la_LDFLAGS = $(libSurfaceToolsGui_la_LDFLAGS) -module -avoid-version
SurfaceToolsGui_la_CPPFLAGS = $(libSurfaceToolsGui_la_CPPFLAGS)

SurfaceToolsGui_la_LIBADD   = \
		$(libSurfaceToolsGui_la_LIBADD) \
		-lSurfaceToolsGui

SurfaceToolsGui_la_DEPENDENCIES = libSurfaceToolsGui.la

#--------------------------------------------------------------------------------------

# rule for Qt MetaObject Compiler:
moc_%.cpp: %.h
	$(QT_MOC) $< -o $(@F)

# rule for Qt MetaObject Compiler:
%.moc: %.h
	$(QT_MOC) $< -o $(@F)

# rules for Qt User Interface Compiler:
ui_%.h: %.ui
	$(QT_UIC) $< -o $(@F)

# rules for Qt Resource Compiler:
qrc_%.cpp: Resources/%.qrc
	$(QT_RCC) -name $(*F) $< -o $(@F)

# set the include path found by configure
AM_CXXFLAGS = -I$(top_srcdir)/src -I$(top_builddir)/src $(all_includes) $(QT_CXXFLAGS) \
		-I$(sim_ac_coin_includedir) -I$(sim_ac_soqt_includedir)

libdir = $(prefix)/Mod/SurfaceTools

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = \
		CMakeLists.txt \
		Resources/SurfaceTools.qrc

