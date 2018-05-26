# About

QLoud is a tool to measure a loudspeaker frequency response and distortions.
This is just an attempt of porting it to QT5.
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

* loudspeakers DIY-ing (xover tuning).


# Measurement chain

* the app, generating excitation signal,
* sound card line out,
* power amplifier,
* loudspeaker under testing,
* measurement microphone (with mic preamp),
* sound card line in,
* the app, capturing loudspeaker response.


# Result

After audio system response capturing is done, IR (Impulse Response)
is calculated. Currently these IR-aware analysis outcomes are available:

* plotting of SPL (Sound Pressure Level) as function of frequency (dB/Hz scale),
* plotting of IR-power as function of time (dB/s),
* plotting of Step Response as function of time (amp/s),
* plotting of harmonics as function of frequency (dB/Hz scale):
	* 2nd – green
	* 3rd – red
	* 4th – light green
	* 5th – light red


IR-power plot is useful to see a room reflections behaviour.
Step Response plot is useful for speakers time alignment in multiway loudspeakers.
All plots support zooming with history and panning.


# A few hints

* move mouse above “?” sign at plot window and wait,
* to change a measurement description double-click it,
* to delete a measurement, use context menu on measurements table,
* to see what the app do, just connect app’s JACK ports directly and try,
* to see what your sound card do, use loopback for line in/out,
* take into account your measurement mic slope at high frequencies when
  observing harmonics plot.
