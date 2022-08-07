#ifndef SPLPLOT_H
#define SPLPLOT_H

#include "Plotter.h"

class SplPlot : public Plotter {
	Q_OBJECT

public:
	SplPlot(const QString& aDir, IRInfo anIi, QWidget *parent = 0);
	~SplPlot();

	double getMaxTrimLength(); // secs
	bool exportSeries(const QString &filename) override;

public slots:
	void setSmooth(double smoothFactor);
	void setWinLength(double secs);
	void enablePhase(int);

private:
	IR* ir;
	const QString& dir;
	IRInfo ii;

	double* freqs;
	double* amps;
	double* phase;

	double winLength;
	double smoothFactor;

	QLineSeries *ampCurve;
	QLineSeries *phaseCurve;

	QAbstractAxis *XAxis;
	QAbstractAxis *YAxis;
	QAbstractAxis *YPAxis;

	void recalculate();
};

#endif // SPLPLOT_H
