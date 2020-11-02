/*
    Copyright (C) 2006 Andrew Gaydenko <a@gaydenko.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <QApplication>
#include "QLWin.h"
#include "QLE.h"
#include "QLUtl.h"
#include "config.h"


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    //app.setOrganizationName("Herewe");
    //app.setOrganizationDomain("herewe");
    app.setApplicationName("QLoud");
    app.setApplicationVersion(VERSION " (" REVISION ")");

    QString locale = QLocale::system().name();
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + locale,
            QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QTranslator qloudTranslator;
    if (!qloudTranslator.load(TARGET "_" + locale, "locale"))
#ifdef __mswin
        qloudTranslator.load(TARGET "_" + locale, QCoreApplication::applicationDirPath() + QDir::separator() + "locale");
#else
        qloudTranslator.load(TARGET "_" + locale, DATADIR "/" TARGET "/locale");
#endif
    app.installTranslator(&qloudTranslator);

    QCommandLineParser parser;
    parser.setApplicationDescription(QObject::tr("Loudspeaker response measurement program."));
    parser.addHelpOption();
    parser.addVersionOption();

    parser.process(app);

    try {
        QLWin * w = new QLWin(0);
        w->show();
        app.exec();
        delete w;
    } catch(QLE e) {
        QLUtl::d(e.msg);
    }
    return 0;
}

