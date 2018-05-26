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

#include <QtXml/QtXml>
#include "QLCfg.h"
#include "QLUtl.h"

QLCfg::QLCfg(QString aPath) {
	this->path = aPath;
}

QLCfg::~QLCfg() {}

void QLCfg::appendIr(IRInfo info) {
	this->initFile();
	QDomDocument doc = this->read();

	QDomNodeList list = doc.elementsByTagName("names");
	if(list.size() != 1)
		throw QLE("Failed getting <names/>!");
	QDomElement names = list.at(0).toElement();

	// check if such key exists
	QDomNodeList nameList = names.childNodes();
	for(int i=0; i < nameList.size(); i++) {
		QDomElement e = nameList.at(i).toElement();
		if(info.key == e.attribute("key", "-1"))
			throw QLE(info.key + ": such key already exists!");
	}

	QDomElement name = doc.createElement("name");
	name.setAttribute("key", info.key);
	name.setAttribute("info", info.info);
	name.setAttribute("length", info.length);
	name.setAttribute("rate", info.rate);
	name.setAttribute("depth", info.depth);
	name.setAttribute("fMin", info.fMin);
	name.setAttribute("fMax", info.fMax);
	name.setAttribute("maxLevel", info.maxLevel);
	names.appendChild(name);

	this->write(doc);
}

void QLCfg::removeIr(QString key) {
	QDomDocument doc = this->read();

	QDomNodeList list = doc.elementsByTagName("names");
	if(list.size() != 1)
		throw QLE("Failed getting <names/>!");
	QDomElement names = list.at(0).toElement();

	QDomNodeList nameList = names.childNodes();
	bool found = false;
	for(int i=0; i < nameList.size(); i++) {
		QDomElement e = nameList.at(i).toElement();
		if(key == e.attribute("key", "-1")) {
			names.removeChild(e);
			found = true;
			break;
		}
	}
	if( ! found)
		throw QLE(key + ": such key is not found!");

	this->write(doc);
}

// it is unsufficient to remove/append: we want to preserve items oder
// Moreover, only 'info' attribute may be replaced!
void QLCfg::replaceIr(IRInfo info) {
	QDomDocument doc = this->read();

	QDomNodeList list = doc.elementsByTagName("names");
	if(list.size() != 1)
		throw QLE("Failed getting <names/>!");
	QDomElement names = list.at(0).toElement();

	QDomNodeList nameList = names.childNodes();
	bool found = false;
	for(int i=0; i < nameList.size(); i++) {
		QDomElement e = nameList.at(i).toElement();
		if(info.key == e.attribute("key", "-1")) {
			e.setAttribute("info", info.info);
			found = true;
			break;
		}
	}
	if( ! found)
		throw QLE(info.key + ": such key is not found!");

	this->write(doc);
}

QString QLCfg::nextIrKey() {
	this->initFile();
	QDomDocument doc = this->read();
	QDomNodeList list = doc.elementsByTagName("nextKey");
	if(list.size() != 1)
		throw QLE("Failed getting <nextKey/>!");

	QDomElement e = list.at(0).toElement();
	int key = e.attribute("next", "-1").toInt();
	if(key < 1)
		throw QLE("Invalid \"next\" attribute!");
	e.setAttribute("next", key+1);
	this->write(doc);

	return this->irKeyToString(key);
}

QList<IRInfo> QLCfg::getIrs() {
	if( ! this->fileExists()) {
		QList<IRInfo> emptyList;
		return emptyList;
	}

	QDomDocument doc = this->read();

	QDomNodeList list = doc.elementsByTagName("names");
	if(list.size() != 1)
		throw QLE("Failed getting <names/>!");
	QDomElement names = list.at(0).toElement();

	QList<IRInfo> infos;
	QDomNodeList nameList = names.childNodes();
	for(int i=0; i < nameList.size(); i++) {
		QDomElement e = nameList.at(i).toElement();
		IRInfo ii;
		ii.key = e.attribute("key", "-1");
		ii.info = e.attribute("info", "ERROR!");
		ii.length = e.attribute("length", "-1").toInt();
		ii.rate = e.attribute("rate", "-1").toInt();
		ii.depth = e.attribute("depth", "-1").toInt();
		ii.fMin = e.attribute("fMin", "-1").toInt();
		ii.fMax = e.attribute("fMax", "-1").toInt();
		ii.maxLevel = e.attribute("maxLevel", "1.0").toDouble();
		infos.append(ii);
	}

	return infos;
}

void QLCfg::setExcit(const ExcitCfg& cfg) {
	cfg.check();
	this->initFile();
	QDomDocument doc = this->read();
	QDomNodeList list = doc.elementsByTagName("excitation");
	if(list.size() != 1)
		throw QLE("Failed getting <excitation/>!");

	QDomElement e = list.at(0).toElement();
	e.setAttribute("length", cfg.length);
	e.setAttribute("rate", cfg.rate);
	e.setAttribute("depth", cfg.depth);
	e.setAttribute("fMin", cfg.fMin);
	e.setAttribute("fMax", cfg.fMax);

	this->write(doc);
}

ExcitCfg QLCfg::getExcit() {
	if( ! this->fileExists()) {
		ExcitCfg eCfg;
		return eCfg;
	}

	QDomDocument doc = this->read();
	QDomNodeList list = doc.elementsByTagName("excitation");
	if(list.size() != 1)
		throw QLE("Failed getting <excitation/>!");
	QDomElement e = list.at(0).toElement();

	ExcitCfg cfg;

	cfg.length = e.attribute("length").toInt();
	cfg.rate = e.attribute("rate").toInt();
	cfg.depth = e.attribute("depth").toInt();
	cfg.fMin = e.attribute("fMin").toInt();
	cfg.fMax = e.attribute("fMax").toInt();

	return cfg;
}

IRInfo QLCfg::getIr(const QString& key) {
	QList<IRInfo> list = this->getIrs();
	foreach(IRInfo ii, list)
	if(ii.key == key)
		return ii;
	throw QLE("IR info not found for key " + key);
}

// private
QString QLCfg::fileName() {
	return this->path + "/qloud.xml";
}

bool QLCfg::fileExists() {
	QFile file(this->fileName());
	bool exists = file.exists();
	QLUtl::checkFileError(file);
	return exists;
}

void QLCfg::initFile() {
	if(this->fileExists())
		return;

	QDomDocument doc;

	// header and root
	QDomNode header = doc.createProcessingInstruction(
		"xml",
		"version=\"1.0\" encoding=\"UTF-8\""
	);
	doc.appendChild(header);

	QDomElement root = doc.createElement("qloud");

	// ExcitCfg
	ExcitCfg eCfg; // with default parameters
	QDomElement excit = doc.createElement("excitation");
	excit.setAttribute("length", QVariant(eCfg.length).toString());
	excit.setAttribute("rate", QVariant(eCfg.rate).toString());
	excit.setAttribute("depth", QVariant(eCfg.depth).toString());
	excit.setAttribute("fMin", QVariant(eCfg.fMin).toString());
	excit.setAttribute("fMax", QVariant(eCfg.fMax).toString());
	root.appendChild(excit);

	// next IR key
	QDomElement irNames = doc.createElement("irNames");

	QDomElement nextKey = doc.createElement("nextKey");
	nextKey.setAttribute("next", 1);
	irNames.appendChild(nextKey);

	// IRInfo-s
	QDomElement names = doc.createElement("names");
	irNames.appendChild(names);
	root.appendChild(irNames);

	//
	doc.appendChild(root);

	// save
	QFile file(this->fileName());
	file.open(QIODevice::WriteOnly);
	QTextStream stream(&file);
	doc.save(stream, 4);
	file.close();
	QLUtl::checkFileError(file);
}


QDomDocument QLCfg::read() {
	this->initFile();
	QFile file(this->fileName());
	file.open(QIODevice::ReadOnly);
	QLUtl::checkFileError(file);
	QString error;
	int row, column;
	QDomDocument doc;
	doc.setContent(&file, &error, &row, &column);
	file.close();
	QLUtl::checkFileError(file);
	if(error.length()) {
		error += " at row ";
		error += QVariant(row).toString();
		error += " and column ";
		error += QVariant(column).toString();
		throw QLE(error);
	}
	return doc;
}

void QLCfg::write(const QDomDocument& doc) {
	QFile file(this->fileName());
	file.open(QIODevice::WriteOnly);
	QLUtl::checkFileError(file);
	QTextStream stream(&file);
	doc.save(stream, QLCfg::INDENT);
	file.close();
	QLUtl::checkFileError(file);
}

QString QLCfg::irKeyToString(int key) {
	QVariant v(key);
	QString s(v.toString());
	if(key < 10)
		return "000" + s;
	if(key < 100)
		return "00" + s;
	if(key < 1000)
		return "0" + s;
	if(key < 10000)
		return s;
	throw QLE("Too many measurements. Are you crazy?");
}
