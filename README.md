# About

QLoud is a tool to measure frequency response and distortion.

This was just an attempt to port it to Qt5. It now uses Qt Charts instead of Qwt for plotting.
The original app page is now [archived](https://web.archive.org/web/20250123210056/http://gaydenko.com/qloud/).

Writing this app was inspired by [excellent applications written by Fons Adriaensen](https://kokkinizita.linuxaudio.org/linuxaudio/index.html).

Theoretical background belongs to the late [Angelo Farina](https://angelofarina.it/).
In particular, [this method](https://angelofarina.it/Public/Papers/134-AES00.PDF) was used.


# Target use

* loudspeaker DIY (crossover tuning)


# Measurement chain

* the app, generating excitation signal
* sound card line out
* power amplifier
* loudspeaker under test
* measurement microphone (with microphone preamp)
* sound card line in
* the app, capturing loudspeaker response


# Result

After audio system response capturing is done, an IR (impulse response) is calculated. 
Currently, the following IR-aware analysis outcomes are available:

* plotting of SPL (sound pressure level) as a function of frequency (dB/Hz)
* plotting of IR power as a function of time (dB/s)
* plotting of step response as a function of time (amp/s)
* plotting of harmonics as a function of frequency (dB/Hz):
	* 2nd – green
	* 3rd – red
	* 4th – light green
	* 5th – light red

The IR power plot is useful to see room reflection behavior.
The step response plot is useful for speaker time alignment in multiway loudspeakers.


# A few hints

* move mouse above “?” sign at plot window and wait
* to change a measurement description, double-click it
* to delete a measurement, use the context menu on the measurements table
* to see what the app does, just connect the app’s JACK ports directly and try
* to see what your sound card does, use loopback for line in/out
* take into account your measurement mic slope at high frequencies when
  observing harmonics plot
