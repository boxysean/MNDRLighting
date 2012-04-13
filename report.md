MNDR
====
In February 2012, the Rainbroz met MNDR, shortly before her 2012 tour with the Ting Tings.

MNDR is an electro-pop act consisting of lead singer/co-producer Amanda Warner and co-producer Peter Wade. For live MNDR gigs, Amanda performs solo, using synths, computers, and music-reactive visuals. The visuals are graphically-complex programs built from simple shapes made by Jamie Carreiro. Jamie overlays his visuals over the entire stage using a projector and uses MIDI from the audio to synchronize his patterns with the music. He also controls effects live using an iPad.

Jamie could not join MNDR for her tour, so the Rainbroz teamed up with MNDR to create a music-reactive light show for MNDR to take on the road. What we created was a self-contained unit that powers 72 RGB LEDs from within MNDR's transparent Activity Center.

[[video of the box]]

Hardware
========
The Activity Center is a 30" x 20" x 7" box with one open end made of bulletproof glass. It houses most of the gear that MDNR needs for her tour. We lined 7 edges of the Activity Center with LPD8806 RGB LEDs from Adafruit.

![Activity Center](http://dump.boxysean.com/mndr/images/P1000601.JPG)

We created a lighting controller unit we call the Rainbow Brain housed within a small bamboo box that lives in the Activity Center. The Rainbow Brain has a MIDI input and power input, and outputs signals to the RGB LED strip and a couple of AC relays for additional effects.

![Rainbow Brain](http://dump.boxysean.com/mndr/images/P1000606.JPG)

Inside the Rainbow Brain is an Arduino Uno and a Sparkfun MIDI breakout board. (That's it!) We put the entire music-reactive (MIDI-reactive) logic onto the Arduino. No computer was required to directly control the Arduino.

![Rainbow Brain](http://dump.boxysean.com/mndr/images/P1000609.JPG)

Lighting design
===============
MNDR picked the set list for her tour to start off punchy, followed by a few ballads, and finishing off with her best pop anthems. We designed the follow the arc of the set. As such, the lighting started plain, stayed subtle during the ballads, and we let loose during the final couple of songs.

Each song was given a simple color palette of one or two colors. We broke the songs down into its components (chorus, verse, bridge, etc.) and programmed lighting patterns that suited each song component.

The lighting patterns for the song components were made from three simple effects -- pulsing, chasing, and solid. (Chasing is commonly referred to as "the Knight Rider effect".)

These effects may make sense conceptually on a single LED or on a line of LEDs. For example, imagine a pulsing LED that fades on and off. What was interesting when designing the patterns, however, was sequencing the 7 LED lines of the Activity Center with these effects. For instance, pulsing each line sequentially to highlight each box edge individually, or alternating pulses between all of the horizontal lines and all of the vertical lines. These kinds of sequences were chosen as the patterns for each song component.

Different pulse speeds and trigger rate were also considerations in a pattern. For instance, ballads more likely were given slow infrequent fades, but upbeat songs were given more short staccato flashes on every downbeat.

Likewise, chaser effects were combined to produce patterns. The busiest pattern we created started a chaser from each corner of the Activity Center on each downbeat. This effect turned out to work very nicely with the electronic effects of MNDR's song "Sparrow" [[GIF???]].

We gave each song component the combination of a pulse pattern, chaser pattern, and solid base color. Pulse and chaser patterns activated at the same time leads to a busy visual aesthetic, so we reserved that for the climax of final songs.

MIDI interface
==============
MNDR uses Ableton Live as her master sequencer for melodies, drum tracks, and bass synth. Ableton Live also acts as sequencer for our light show by sending custom MIDI messages to the Rainbow Brain.

To control the LEDs in the lighting design outlined above, the Rainbroz and MNDR's team agreed that Ableton Live would send us the following messages: MIDI program change messages to indicate the start of a new song, MIDI timecode messages to indicate the BPM of the current song, and MIDI notes at the beginning of each song component (chorus, verse, bridge, etc.).

We used MIDI timecode as the "clock" of the effects. When enabled, Ableton Live sends 24 evenly spaced timecode messages in a bar. Each message the Rainbow Brain receives causes the LEDs to advance to the next frame. Depending on what programmed patterns were activated, every 24th message also triggered new pulses or chasers.

Each MNDR song was assigned a unique MIDI program change value. Ableton Live sends this value at the beginning of each track, and we used this to change the state of our code. The MIDI note at the beginning of each song is C1 and indicates the first song component. When the next component The next component is labeled C#1, then D1, and so on.

The Rainbow Brain works without any direct control, but we programmed one of MNDR's MIDI pads to trigger a flash of bright white LEDs when hit. We similarly programmed two other pads to toggle their relays on and off.

Software
========
The software is there! Read it to learn it.

How it turned out
=================
The most effective use of our lighting system was when it was subtle. For instance, I am most proud of how the box accompanied MNDR on her ballad "Feed Me Diamonds". When MNDR performed this song, she stepped in front of the box and appealed to her audience on a personal level. Behind her, the box glowed a static purple. During a bridge in the song, it remained purple but gently pulsed brighter on each downbeat, giving a visual drive to the track. As she built to the closing climax, brighter chasers appeared overtop the solid purple to help her conclude her song.

During her set, MNDR transitioned from "Caligula" to "C.L.U.B." as a DJ would beat match two records at a party. The transition consisted of a few loop stages that required MNDR to manually advance the Ableton Live sequencer to the next loop stage until C.L.U.B began in earnest. Within the loop stage, MNDR switched settings and tweaked knobs, creating a nice electro improvisational breakdown that the crowd went crazy for. With each new song element that was introduced via new loop stages, the box knowingly added extra chasers in concert with MNDR's large hand gestures, signifying a key change in the sound made by the electronic performer.

Thanks
======
Thanks to MNDR and her team, especially Emmett Harley, for the opportunity to work on such an interesting project!
