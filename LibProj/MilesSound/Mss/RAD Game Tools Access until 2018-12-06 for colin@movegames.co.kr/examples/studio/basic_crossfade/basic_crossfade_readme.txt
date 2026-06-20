!page(example_studio_basic_crossfade, Basic Crossfade)

_SDK Location: examples/studio/basic_crossfade_

This demonstrates a very basic project that contains two sources 
and two events that are set up to crossfade.

To recreate with your own sound:

$* Create a new project in Miles Studio
$* Select the *Bank* tab.
$* Drag two .wav files from your computer to the *Sources* list.
$* Drag in the Sources list to select both of the new Sources
$* Right click on the sources and select *Create Event For Each*, 
    and accept the names.
$* Select one of the events in the *Events* list.
$* Below the *Event Details* pane, right click on the *Fade Out* selector
    and chose *Set Value*. Set the value to 300 ms.
$* Select the other event and similarly set the *Fade In* value to 300 ms.
$* Right click on the same event in the *Event Details* pane and select
    *Add Action Track Above*, then *Stop*.

Upon completion, there are two events. The first will play a sound, 
and the second will stop all sounds using the fade out specified for
the sound, and then fade in another sound.

It's important to note that the action tracks are run from top to
bottom. If the *Stop* action were below the *Play* track, then 
the effect would be a fade out, as the second sound would get
stopped as soon as it was started.
