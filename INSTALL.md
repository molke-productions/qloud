# Requirements

* Qt6 or Qt5 ( http://trolltech.com/ )
* JACK ( http://jackaudio.org/ )
* sndfile ( http://www.mega-nerd.com/libsndfile/ )
* fftw ( http://www.fftw.org/ )


# Installation

* modify include directories in `src/src.pro` if needed
* run either `qmake6` or `qmake`, setting `PREFIX` (default is `C:\Qloud` on Windows or `/usr/local`)
```
qmake PREFIX=/usr
```
* run
```
make
make install INSTALL_ROOT=/my/package/root
```
The `qloud` executable will be in the `bin` directory.

## On Debian/Ubuntu

* install dependencies for Qt5 (Qt6 should be similar)
```
sudo apt install qttools5-dev libqt5charts5-dev libjack-jackd2-dev libsndfile1-dev libfftw3-dev
```
`libjack-jackd2-dev` can be exchanged with `libjack-dev` if using jackd1
