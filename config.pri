isEmpty(PREFIX) {
    win32:PREFIX = C:/Qloud
    else:PREFIX = /usr/local
}

isEmpty(QMAKE_LRELEASE):QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
