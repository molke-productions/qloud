include(config.pri)

SUBDIRS += src
CONFIG += warn_on \
 qt \
 thread

TEMPLATE = subdirs

desktop.files = qloud.desktop
desktop.path = $$PREFIX/share/applications

icon.files = qloud.xpm
icon.path = $$PREFIX/share/pixmaps

translations.path = $$DATADIR/$${TARGET}
translations.files = locale

INSTALLS += desktop icon translations
