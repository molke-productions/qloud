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

#include "Plotter.h"
#include "QLCfg.h"



Plotter::Plotter(QWidget *parent)
    : QChartView(parent)
{
    chart = new QChart();
    chart->legend()->hide();
    this->setChart(chart);
    this->setRenderHint(QPainter::Antialiasing);
    this->setRubberBand(QChartView::HorizontalRubberBand);

}

Plotter::~Plotter()
{
    delete chart;
}

void Plotter::setTitle(const QString &title)
{
    chart->setTitle(title);
}

QString Plotter::getTitle()
{
    return chart->title();
}

void Plotter::appendSeries(QLineSeries *series, QAbstractAxis* xaxis, Qt::Alignment xalign, QAbstractAxis* yaxis, Qt::Alignment yalign)
{
    if (xaxis)
        chart->addAxis(xaxis, xalign);

    if (yaxis)
        chart->addAxis(yaxis, yalign);
    chart->addSeries(series);

    if (xaxis)
        series->attachAxis(xaxis);
    else
        series->attachAxis(chart->axes(Qt::Orientation::Horizontal)[0]);

    if (yaxis)
        series->attachAxis(yaxis);
    else
        series->attachAxis(chart->axes(Qt::Orientation::Vertical)[0]);

    list.append(series);
}

void Plotter::removeSeries(QLineSeries *series, QAbstractAxis* yattached)
{
    list.removeAll(series);
    chart->removeAxis(yattached);
    chart->removeSeries(series);
}

bool Plotter::gnuplotSeries(const QString &filename)
{
    QFile file(filename);

    QString f("# QLoud plot: ");
    f += getTitle() + "\n";

    int len = list.at(0)->points().size();

    QString line;
    for (int i = 0; i < len; i++) {
        line.clear();
        for (int s = 0; s < list.size(); s++ ) {
            QVector<QPointF> points = list.at(s)->pointsVector();
            QPointF p = points.at(i);
            if (s == 0) {
                line += QString("%1 %2 ").arg(p.x()).arg(p.y());
            } else {
                line += QString("%1 ").arg(p.y());
            }
        }

        line.chop(1);
        line.append('\n');
        f.append(line);
    }

    file.open(QIODevice::WriteOnly);
    file.write(f.toUtf8());
    file.close();
    return true;
}
