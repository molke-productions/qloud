unix {
    isEmpty(PREFIX): PREFIX = /usr/local
    isEmpty(BINDIR): BINDIR = $$PREFIX/bin
    isEmpty(DATADIR): DATADIR = $$PREFIX/share
}

win32-g++ {
    isEmpty(PREFIX): PREFIX = "C:/Program Files/QLoud"
    isEmpty(BINDIR): BINDIR = $$PREFIX
    isEmpty(DATADIR): DATADIR = $$PREFIX
}


isEmpty(QMAKE_LRELEASE):QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease

VERSION = 1.3
REVISION = $$system(git describe --long --tags 2>/dev/null || echo "stable")

