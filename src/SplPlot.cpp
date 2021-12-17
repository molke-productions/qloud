#include "SplPlot.h"
#include "IR.h"
#include "FileFft.h"
#include "Plotter.h"
#include "QLUtl.h"
#include "QLCfg.h"

static const QColor AMP_CURVE_COLOR(0, 0, 172);
static const QColor PHASE_CURVE_COLOR(0, 150, 0);

SplPlot::SplPlot(
	const QString& aDir,
	IRInfo anIi,
	QWidget *parent
) : Plotter(parent) {
	this->setAttribute(Qt::WA_DeleteOnClose);

	this->dir = aDir;
	this->ii = anIi;

	this->freqs = 0;
	this->amps = 0;
	this->phase = 0;
	this->phaseCurve = 0;

	this->ir = new IR(dir, ii.key);

	setTitle(tr("Frequency Response"));

	XAxis = new QLogValueAxis(this->chart);
	((QLogValueAxis*) XAxis)->setBase(10.0);
	((QLogValueAxis*) XAxis)->setLabelFormat("%d");
	XAxis->setTitleText(tr("Frequency in Hz"));
	XAxis->setRange(10, 100000);
	((QLogValueAxis *) XAxis)->setMinorTickCount(8);

	YAxis = new QValueAxis(this->chart);
	YAxis->setTitleText(tr("Amplitude in dB"));
	((QValueAxis *) YAxis)->setLabelFormat("%d");
	YAxis->setMax(20);
	YAxis->setMin(-100);
	((QValueAxis *) YAxis)->setTickCount(7);
	((QValueAxis *) YAxis)->setMinorTickCount(10);

	this->ampCurve = new QLineSeries(this->chart);
	this->ampCurve->setPen(QPen(AMP_CURVE_COLOR));
	appendSeries(ampCurve, XAxis, Qt::AlignBottom, "Hz", YAxis, Qt::AlignLeft, "dB");

	if(QLCfg::USE_PHASE) {
		YPAxis = new QValueAxis(this->chart);
		YPAxis->setTitleText(tr("Phase in degrees"));
		((QValueAxis *) YPAxis)->setLabelFormat("%d");
		YPAxis->setMax(180);
		YPAxis->setMin(-180);

		this->phaseCurve = new QLineSeries(this->chart);
		this->phaseCurve->setPen(QPen(PHASE_CURVE_COLOR));
	}

	this->smoothFactor = Plotter::DEFAULT_SMOOTH; // 1/6 octave
	this->winLength = 0.5; // 500 ms for right window
	this->recalculate();
}

SplPlot::~SplPlot() {
	if(this->freqs)
		delete this->freqs;
	if(this->amps)
		delete this->amps;
	if(this->phase)
		delete this->phase;
	if(this->ir)
		delete ir;
	if(this->phaseCurve)
		delete this->phaseCurve; // may be detached
}

bool SplPlot::exportSeries(const QString &filename) {
	QFile file(filename);

	QString f("# QLoud plot: ");
	f += getTitle() + "\n";

	if(QLCfg::USE_PHASE && chart->series().length() == 2) {
		f += "# freq, amp, phase\n";
	} else {
		f += "# freq, amp\n";
	}

	QList<QPointF> Apoints = this->ampCurve->points();
	QList<QPointF> Ppoints = this->phaseCurve->points();

	for (int i = 0; i < Apoints.size(); i++ ) {
		QString line;
		if(QLCfg::USE_PHASE && chart->series().length() == 2) {
			line = "%1 %2 %4\n";
			line = line.arg(Apoints.at(i).x()).arg(Apoints.at(i).y()).arg(Ppoints.at(i).y());
		} else {
			line = "%1 %2\n";
			line = line.arg(Apoints.at(i).x()).arg(Apoints.at(i).y());
		}
		f.append(line);
	}

	file.open(QIODevice::WriteOnly);
	file.write(f.toUtf8());
	file.close();
	return true;
}

double SplPlot::getMaxTrimLength() {
	return this->ir->getMaxTrimLength();
}

void SplPlot::setSmooth(double aSmoothFactor) {
	this->smoothFactor = aSmoothFactor;
	this->recalculate();
}

void SplPlot::setWinLength(double msecs) {
	this->winLength = msecs / 1000.0;
	this->recalculate();
}

void SplPlot::enablePhase(int state) {
	if( ! QLCfg::USE_PHASE)
		return;

	if(state) {
		appendSeries(
			phaseCurve,
			nullptr, Qt::AlignBottom, "Hz",
			YPAxis, Qt::AlignRight, "°"
		);
	} else {
		removeSeries(phaseCurve, YPAxis);
	}
}

// private
void SplPlot::recalculate() {
	this->ir->trim(this->winLength);

	FileFft* ff = new FileFft(
		this->dir + "/" + this->ii.key + IR::trimmedIrFileName(), this->ii
	);

	if(this->freqs)
		delete this->freqs;
	this->freqs = ff->getFreqs();

	if(this->amps)
		delete this->amps;
	this->amps = ff->getAmps(1.0 / this->smoothFactor);

	if(QLCfg::USE_PHASE) {
		if(this->phase)
			delete this->phase;
		this->phase = ff->getPhase(1.0 / this->smoothFactor);
	}

	delete ff;

	QList<QPointF> points;
	for (int i = 0; i < FileFft::POINTS_AMOUNT; ++i) {
		points.append(QPointF(this->freqs[i], this->amps[i]));
	}
	this->ampCurve->replace(points);

	if(QLCfg::USE_PHASE) {
		QList<QPointF> ppoints;
		for (int i = 0; i < FileFft::POINTS_AMOUNT; ++i) {
			ppoints.append(QPointF(this->freqs[i], this->phase[i]));
		}
		this->phaseCurve->replace(ppoints);
	}
}
