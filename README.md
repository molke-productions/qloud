# About

QLoud is a tool to measure a loudspeaker frequency response and distortions.
This is just an attempt of porting it to QT5.
On the branch __qtcharts__, this tool _does not need_ __libQwt__, but __QtCharts__.
The original app page is here:

http://gaydenko.com/qloud/

Writing this app was inspired by excellent applications written
by Fons Adriaensen:

http://users.skynet.be/solaris/linuxaudio/

Theoretical background belongs to Angelo Farina:

http://pcfarina.eng.unipr.it/

In particular, this method was used:

http://pcfarina.eng.unipr.it/Public/Papers/134-AES00.PDF


# Target use

* loudspeakers DIY-ing (crossover tuning).


# Measurement chain

* the app, generating excitation signal
* sound card line out
* power amplifier
* loudspeaker under testing
* measurement microphone (with mic preamp)
* sound card line in
* the app, capturing loudspeaker response


# Result

After audio system response capturing is done, an IR (impulse response)
is calculated. Currently, these IR-aware analysis outcomes are available:

* plotting of SPL (sound pressure level) as a function of frequency (dB/Hz)
* plotting of IR power as a function of time (dB/s)
* plotting of step response as a function of time (amp/s)
* plotting of harmonics as a function of frequency (dB/Hz):
	* 2nd – green
	* 3rd – red
	* 4th – light green
	* 5th – light red


The IR power plot is useful to see room reflections behavior.
The step response plot is useful for speakers time alignment in multiway loudspeakers.
All plots support zooming with history and panning.


# A few hints

* move mouse above “?” sign at plot window and wait
* to change a measurement description double-click it
* to delete a measurement, use the context menu on the measurements table
* to see what the app does, just connect the app’s JACK ports directly and try
* to see what your sound card does, use loopback for line in/out
* take into account your measurement mic slope at high frequencies when
  observing harmonics plot
