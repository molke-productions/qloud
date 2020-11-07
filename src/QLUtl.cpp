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

#include <cmath>
#include <iostream>
#include "QLUtl.h"

double QLUtl::toDb(double d) {
	if(fabs(d) < 1.0e-10)
		return -200.0;
	double r = 20.0 * log10(fabs(d));
	return round(r * 10) / 10;
}

void QLUtl::d(std::string in) {
	if(QL_DEBUG)
		std::cout << in << std::endl;
}

void QLUtl::d(QString in) {
	if(QL_DEBUG)
		std::cout << in.toStdString() << std::endl;
}

void QLUtl::d(int in) {
	if(QL_DEBUG)
		std::cout << in << std::endl;
}

void QLUtl::d(double in) {
	if(QL_DEBUG)
		std::cout << in << std::endl;
}

void QLUtl::d(char* in) {
	if(QL_DEBUG)
		std::cout << in << std::endl;
}

void QLUtl::d() {
	if(QL_DEBUG)
		std::cout << "*************************************" << std::endl;
}

void QLUtl::checkFileError(const QFile& file) {
	QFile::FileError e = file.error();
	if(e == QFile::NoError)
		return;
	QString f = file.fileName() + ": ";
	switch(e) {
		case QFile::ReadError:
			throw QLE(f + "An error occurred when reading from the file!");
		case QFile::WriteError:
			throw QLE(f + "An error occurred when writing to the file!");
		case QFile::FatalError:
			throw QLE(f + "A fatal error occurred!");
		case QFile::ResourceError:
			throw QLE(f + "QFile::ResourceError!");
		case QFile::OpenError:
			throw QLE(f + "The file could not be opened!");
		case QFile::AbortError:
			throw QLE(f + "The operation was aborted!");
		case QFile::TimeOutError:
			throw QLE(f + "A timeout occurred!");
		case QFile::UnspecifiedError:
			throw QLE(f + "An unspecified error occurred!");
		case QFile::RemoveError:
			throw QLE(f + "The file could not be removed!");
		case QFile::RenameError:
			throw QLE(f + "The file could not be renamed!");
		case QFile::PositionError:
			throw QLE(f + "The position in the file could not be changed!");
		case QFile::ResizeError:
			throw QLE(f + "The file could not be resized!");
		case QFile::PermissionsError:
			throw QLE(f + "The file could not be accessed!");
		case QFile::CopyError:
			throw QLE(f + "The file could not be copied!");
		default:
			return;
	}
}

void QLUtl::showCritical(QWidget* parent, const QString& msg) {
	QMessageBox::critical(parent, "Error", msg);
}

void QLUtl::showInfo(QWidget* parent, const QString& msg) {
	QMessageBox::information(parent, "Information", msg);
}

bool QLUtl::setComboToData(QComboBox* combo, int data) {
	for(int i=0; i < combo->count(); i++) {
		int tmp = QVariant(combo->itemText(i)).toInt();
		if(tmp == data) {
			combo->setCurrentIndex(i);
			return true;
		}
	}
	return false;
}


void QLUtl::toDbInPlace(double* in, int length, bool normToZero) {
	if( normToZero ) {
		double max = 0.0;
		double shift = 0.0;
		for(int i=0; i < length; i++) {
			double tmp = fabs(in[i]);
			if(max < tmp)
				max = tmp;
		}
		shift = QLUtl::toDb(max);
		for(int i = 0; i < length; i++)
			in[i] = QLUtl::toDb(in[i]) - shift;
		return;
	}
	for(int i = 0; i < length; i++)
		in[i] = QLUtl::toDb(in[i]);
}


double* QLUtl::fastSmooth(double* in, double smoothFactor, int length) {
	double* smoothed = new double[length];
	double rightFactor = pow(2.0, smoothFactor / 2.0);
	double leftFactor = 1.0 / rightFactor;

	int left;
	int right;
	double tmp;
	for(int i = 0; i < length; i++) {
		left = int(leftFactor * i + 0.5);
		if(left < 0)
			left = 0;

		right = int(rightFactor * i + 0.5);
		if(right > length-1 )
			right = length -1;

		tmp = 0.0;
		int increment = 1;
		if(right - left > 2048)
			increment = (right - left) / 2048;
		int count = 0;
		for(int j = left; j <= right; j += increment) {
			tmp += in[j];
			count++;
		}

		smoothed[i]  = tmp / count;
	}

	return smoothed;
}

double* QLUtl::smooth(double* in, double smoothFactor, int length) {
	double* smoothed = new double[length];
	double rightFactor = pow(2.0, smoothFactor / 2.0);
	double leftFactor = 1.0 / rightFactor;

	int left;
	int right;
	double tmp;
	for(int i = 0; i < length; i++) {
		left = int(leftFactor * i + 0.5);
		if(left < 0)
			left = 0;

		right = int(rightFactor * i + 0.5);
		if(right > length-1 )
			right = length -1;

		tmp = 0.0;
		for(int j = left; j <= right; j++)
			tmp += in[j];

		smoothed[i]  = tmp / (right - left + 1);
	}

	return smoothed;
}


double* QLUtl::smoothForLog(
	double* in,
	double* freqs,
	double smoothFactor,
	int length
) {
	double* smoothed = new double[length];
	double freqFactor = pow(2.0, smoothFactor / 2.0);
	double stepRatio = freqs[1] / freqs[0];
	int delta = 0;
	double ratio = 1.0;
	while(ratio < freqFactor) {
		ratio *= stepRatio;
		delta++;
	}

	int left;
	int right;
	double tmp;
	int deltaInUse;
	for(int i = 0; i < length; i++) {
		deltaInUse = delta;

		left = i - deltaInUse;
		if(left < 0) {
			deltaInUse = i;
			left = 0;
		}

		right = i + deltaInUse;
		if(right > length-1 ) {
			deltaInUse = length - 1 - i;
			right = length - 1;
		}

		tmp = 0.0;
		for(int j = left; j <= right; j++)
			tmp += in[j];

		smoothed[i]  = tmp / ((deltaInUse << 1) + 1);
	}

	return smoothed;
}


double* QLUtl::logFreqs(int pointsAmount, double fMin, double fMax) {
	double* freqs = new double[pointsAmount];
	double ratio = log(double(fMax) / fMin);
	ratio /= pointsAmount;
	for(int i = 0; i < pointsAmount; i++)
		freqs[i] = exp(ratio * i) * fMin;
	return freqs;
}


double* QLUtl::spaceAmpsToFreqs(
	int pointsAmount,
	double* freqs,
	int ampsAmount,
	double* amps
) {
	double* out = new double[pointsAmount];
	for(int i = 0; i < pointsAmount; i++) {
		double freq = freqs[i];
		int d1 = int(freq);
		int d2 = d1 + 1;
		if(d2 >= ampsAmount)
			out[i] = amps[d1];
		else
			out[i] = amps[d1] + (amps[d2] - amps[d1]) * (freq - d1);
	}
	return out;
}


fftw_complex* QLUtl::doFFT(
	double* inBuf,
	int inLength,
	int rate
) {
	// zero padding to get whole number of seconds
	int fftLength = inLength;
	if(fftLength % rate)
		fftLength += rate - (fftLength % rate);

	double* buf = new double[fftLength];
	for(int i = 0; i < fftLength; i++)
		buf[i] = 0.0;

	for(int i = 0; i < inLength; i++)
		buf[i] = inBuf[i];

	int secs = fftLength / rate;
	int fftResultLength = rate / 2;

	// here we shell accumulate one-second-length transformations
	fftw_complex* fftResult = (fftw_complex*)fftw_malloc(
		sizeof(fftw_complex) * fftResultLength
	);
	for(int i = 0; i < fftResultLength; i++) {
		fftResult[i][0] = 0.0;
		fftResult[i][1] = 0.0;
	}

	fftw_complex* in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * rate);
	fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * rate);

	for(int i = 0; i < secs; i++) {
		// take this second from inBuf ...
		for(int j = 0; j < rate; j++) {
			in[j][0] = buf[i * rate + j];
			in[j][1] = 0.0;
		}
		// ... transform it ...
		fftw_plan plan = fftw_plan_dft_1d(
			rate, in, out, FFTW_FORWARD, FFTW_ESTIMATE
		);
		fftw_execute(plan);
		fftw_destroy_plan(plan);
		// ... and sum with other seconds
		for(int j = 0; j < fftResultLength; j++) {
			fftResult[j][0] += out[j][0];
			fftResult[j][1] += out[j][1];
		}
	}

	fftw_free(in);
	fftw_free(out);
	delete[] buf;

	return fftResult;
}
