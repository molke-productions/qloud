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
	app.setOrganizationName("Andrew_Gaydenko");
	app.setApplicationName("QLoud");
	app.setApplicationVersion(VERSION " (" REVISION ")");

	QString locale = QLocale::system().name();
	QTranslator qtTranslator;
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
	if (qtTranslator.load(QLocale::system(), u"qtbase"_qs, u"_"_qs,
				QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
		app.installTranslator(&qtTranslator);
#else
	qtTranslator.load("qt_" + locale,
			QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	app.installTranslator(&qtTranslator);
#endif

	QTranslator qloudTranslator;
	if (qloudTranslator.load(TARGET "_" + locale, "../locale"))
		app.installTranslator(&qloudTranslator);
#ifdef __mswin
	else if (qloudTranslator.load(TARGET "_" + locale, QCoreApplication::applicationDirPath() + QDir::separator() + "locale"))
#else
	else if (qloudTranslator.load(TARGET "_" + locale, DATADIR "/" TARGET "/locale"))
#endif
		app.installTranslator(&qloudTranslator);

	QCommandLineParser parser;
	parser.setApplicationDescription(
		QObject::tr("Loudspeaker response measurement program.")
	);
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument(
		"workdir",
		QCoreApplication::translate("main", "Project directory to work on.")
	);

	parser.process(app);

	const QString *wrkDir = nullptr;
	const QStringList args = parser.positionalArguments();
	if (args.length() > 0)
		wrkDir = &args.at(0);

	try {
		QLWin * w = new QLWin(wrkDir, 0);
		w->show();
		app.exec();
		delete w;
	} catch(QLE e) {
		QLUtl::d(e.msg);
	}
	return 0;
}

