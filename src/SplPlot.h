#ifndef SPLPLOT_H
#define SPLPLOT_H

#include "Plotter.h"

class SplPlot : public Plotter {
	Q_OBJECT

public:
	SplPlot(const QString& aDir, QWidget *parent = 0);
	~SplPlot();

	void setIrInfo(const IRInfo& ii) override;

	double getMaxTrimLength(); // secs
	bool exportSeries(const QString &filename) override;

public slots:
	void setSmooth(int smoothFactor);
	void setWinLength(double secs);
	void enablePhase(int);

private:
	IR* ir;
	QString dir;
	IRInfo ii;

	double* freqs = nullptr;
	double* amps = nullptr;
	double* phase = nullptr;

	double winLength;
	int smoothFactor;

	QLineSeries *ampCurve = nullptr;
	QLineSeries *phaseCurve = nullptr;

	QLogValueAxis *XAxis;
	QAbstractAxis *YAxis;
	QAbstractAxis *YPAxis;

	void recalculate();
};

#endif // SPLPLOT_H
