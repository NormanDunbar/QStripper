#############################################################################
# Makefile for building: QStripper
# Generated by qmake (2.00a) (Qt 4.1.2) on: Mon 8. May 12:09:59 2006
# Project:  QStripper.pro
# Template: app
# Command: qmake -win32 -o Makefile QStripper.pro
#############################################################################

first: release
install: release-install
uninstall: release-uninstall
MAKEFILE      = Makefile
QMAKE         = qmake
DEL_FILE      = del
CHK_DIR_EXISTS= if not exist
MKDIR         = mkdir
COPY          = copy /y
COPY_FILE     = $(COPY)
COPY_DIR      = xcopy /s /q /y /i
INSTALL_FILE  = $(COPY_FILE)
INSTALL_DIR   = $(COPY_DIR)
DEL_FILE      = del
SYMLINK       = 
DEL_DIR       = rmdir
MOVE          = move
CHK_DIR_EXISTS= if not exist
MKDIR         = mkdir
SUBTARGETS    =  \
		release \
		debug

release: $(MAKEFILE).Release FORCE
	$(MAKE) -f $(MAKEFILE).Release
release-make_default: $(MAKEFILE).Release FORCE
	$(MAKE) -f $(MAKEFILE).Release 
release-make_first: $(MAKEFILE).Release FORCE
	$(MAKE) -f $(MAKEFILE).Release first
release-all: $(MAKEFILE).Release FORCE
	$(MAKE) -f $(MAKEFILE).Release all
release-clean: $(MAKEFILE).Release FORCE
	$(MAKE) -f $(MAKEFILE).Release clean
release-distclean: $(MAKEFILE).Release FORCE
	$(MAKE) -f $(MAKEFILE).Release distclean
release-install: $(MAKEFILE).Release FORCE
	$(MAKE) -f $(MAKEFILE).Release install
release-uninstall: $(MAKEFILE).Release FORCE
	$(MAKE) -f $(MAKEFILE).Release uninstall
debug: $(MAKEFILE).Debug FORCE
	$(MAKE) -f $(MAKEFILE).Debug
debug-make_default: $(MAKEFILE).Debug FORCE
	$(MAKE) -f $(MAKEFILE).Debug 
debug-make_first: $(MAKEFILE).Debug FORCE
	$(MAKE) -f $(MAKEFILE).Debug first
debug-all: $(MAKEFILE).Debug FORCE
	$(MAKE) -f $(MAKEFILE).Debug all
debug-clean: $(MAKEFILE).Debug FORCE
	$(MAKE) -f $(MAKEFILE).Debug clean
debug-distclean: $(MAKEFILE).Debug FORCE
	$(MAKE) -f $(MAKEFILE).Debug distclean
debug-install: $(MAKEFILE).Debug FORCE
	$(MAKE) -f $(MAKEFILE).Debug install
debug-uninstall: $(MAKEFILE).Debug FORCE
	$(MAKE) -f $(MAKEFILE).Debug uninstall

Makefile: QStripper.pro  C:/Qt/4.1.2/mkspecs/win32-g++\qmake.conf C:/Qt/4.1.2/mkspecs/qconfig.pri \
		..\..\..\..\Qt\4.1.2\mkspecs\features\qt_config.prf \
		..\..\..\..\Qt\4.1.2\mkspecs\features\exclusive_builds.prf \
		..\..\..\..\Qt\4.1.2\mkspecs\features\default_pre.prf \
		..\..\..\..\Qt\4.1.2\mkspecs\features\win32\default_pre.prf \
		..\..\..\..\Qt\4.1.2\mkspecs\features\release.prf \
		..\..\..\..\Qt\4.1.2\mkspecs\features\debug_and_release.prf \
		..\..\..\..\Qt\4.1.2\mkspecs\features\default_post.prf \
		..\..\..\..\Qt\4.1.2\mkspecs\features\win32\rtti.prf \
		..\..\..\..\Qt\4.1.2\mkspecs\features\win32\exceptions.prf \
		..\..\..\..\Qt\4.1.2\mkspecs\features\win32\stl.prf \
		..\..\..\..\Qt\4.1.2\mkspecs\features\shared.prf \
		..\..\..\..\Qt\4.1.2\mkspecs\features\warn_on.prf \
		..\..\..\..\Qt\4.1.2\mkspecs\features\qt.prf \
		..\..\..\..\Qt\4.1.2\mkspecs\features\win32\thread.prf \
		..\..\..\..\Qt\4.1.2\mkspecs\features\moc.prf \
		..\..\..\..\Qt\4.1.2\mkspecs\features\win32\windows.prf \
		..\..\..\..\Qt\4.1.2\mkspecs\features\resources.prf \
		..\..\..\..\Qt\4.1.2\mkspecs\features\uic.prf \
		C:\Qt\4.1.2\lib\qtmain.prl
	$(QMAKE) -win32 -o Makefile QStripper.pro
C:/Qt/4.1.2/mkspecs/qconfig.pri:
..\..\..\..\Qt\4.1.2\mkspecs\features\qt_config.prf:
..\..\..\..\Qt\4.1.2\mkspecs\features\exclusive_builds.prf:
..\..\..\..\Qt\4.1.2\mkspecs\features\default_pre.prf:
..\..\..\..\Qt\4.1.2\mkspecs\features\win32\default_pre.prf:
..\..\..\..\Qt\4.1.2\mkspecs\features\release.prf:
..\..\..\..\Qt\4.1.2\mkspecs\features\debug_and_release.prf:
..\..\..\..\Qt\4.1.2\mkspecs\features\default_post.prf:
..\..\..\..\Qt\4.1.2\mkspecs\features\win32\rtti.prf:
..\..\..\..\Qt\4.1.2\mkspecs\features\win32\exceptions.prf:
..\..\..\..\Qt\4.1.2\mkspecs\features\win32\stl.prf:
..\..\..\..\Qt\4.1.2\mkspecs\features\shared.prf:
..\..\..\..\Qt\4.1.2\mkspecs\features\warn_on.prf:
..\..\..\..\Qt\4.1.2\mkspecs\features\qt.prf:
..\..\..\..\Qt\4.1.2\mkspecs\features\win32\thread.prf:
..\..\..\..\Qt\4.1.2\mkspecs\features\moc.prf:
..\..\..\..\Qt\4.1.2\mkspecs\features\win32\windows.prf:
..\..\..\..\Qt\4.1.2\mkspecs\features\resources.prf:
..\..\..\..\Qt\4.1.2\mkspecs\features\uic.prf:
C:\Qt\4.1.2\lib\qtmain.prl:
qmake: qmake_all FORCE
	@$(QMAKE) -win32 -o Makefile QStripper.pro

qmake_all: FORCE

make_default: release-make_default debug-make_default FORCE
make_first: release-make_first debug-make_first FORCE
all: release-all debug-all FORCE
clean: release-clean debug-clean FORCE
distclean: release-distclean debug-distclean FORCE
	-$(DEL_FILE) Makefile

release-mocclean: $(MAKEFILE).Release
	$(MAKE) -f $(MAKEFILE).Release mocclean
debug-mocclean: $(MAKEFILE).Debug
	$(MAKE) -f $(MAKEFILE).Debug mocclean
mocclean: release-mocclean debug-mocclean

release-mocables: $(MAKEFILE).Release
	$(MAKE) -f $(MAKEFILE).Release mocables
debug-mocables: $(MAKEFILE).Debug
	$(MAKE) -f $(MAKEFILE).Debug mocables
mocables: release-mocables debug-mocables
FORCE:

$(MAKEFILE).Release: Makefile
$(MAKEFILE).Debug: Makefile
