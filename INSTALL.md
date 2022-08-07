# Requirements

* QT5 ( http://trolltech.com/ ), tested with QT 5.15.1
* JACK ( http://jackaudio.org/ )
* sndfile ( http://www.mega-nerd.com/libsndfile/ )
* fftw ( http://www.fftw.org/ )


# Installation

* look in `src/src.pro` to modify include dirs if you want
* set `PREFIX` (default is `C:\Qloud` on Windows or `/usr/local`) with `qmake`
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

* install dependencies
```
sudo apt install qttools5-dev libjack-jackd2-dev libsndfile1-dev libfftw3-dev libqt5charts5-dev
```
`libjack-jackd2-dev` can be exchanged with `libjack-dev` if using jackd1
