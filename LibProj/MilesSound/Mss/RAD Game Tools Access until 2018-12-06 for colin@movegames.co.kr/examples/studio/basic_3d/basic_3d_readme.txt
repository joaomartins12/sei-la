!page(example_studio_basic_3d, Basic 3D)

_SDK Location: examples/studio/basic_3d_

This demostrates a very basic project that plays a Source spatialized to
sound as though it exists in a 3d space.

To recreate with your own sound:

$* Create a new project in Miles Studio
$* Select the *Bank* tab.
$* Drag a .wav file from your computer to the *Sources* list.
    A new source will be created under the bank *NewBank*.    
$* Right click on the new source, and select *Create Event To Play*.
$* Select the event in the *Events* list.
$* In the properties list below the *Event Details* pane, select
    *Infinite* for the *Loop* property.
$* Select the *Project* tab.
$* Right click on the grey *Design* pane and select *Create Bus*.
$* Click on the new bus to select it, and in the properties list below 
    the *Design* pane, mark the *3D* checkbox.
$* Drag your Event from the *Events To Preview* list on the left
    to the new bus in the *Design* pane.
$* Hit spacebar to test the event.

$~By default, Miles Studio places the voice behind and to the left
of the listener. To move the voice around while it plays, go to the
*View->Preview Controls* menu item and drag the red dot around. To
have Miles automatically move the voice during playback, select the
"Move Over Time" checkbox.

~~~
    Positioning in the Preview Controls only take place in Miles Studio for testing purposes,
    and are not reflected in game.
~~~

$~Voices are 3d spatialized based on the bus they are *assigned to*.
If the voice's audio signal passes through a 3d bus on the way to
the Master output, it will *not* be spatialized. To illustrate:

MyEvent[Play Action] -> My3DBus -> Master -- The voice started by the
    play action will be spatialized.

MyEvent[Play Action] -> AnotherBus -> My3DBus -> Master -- The voice
    started by the play action will NOT be spatialized, even though
    the voice's audio data will pass through My3DBus and be affected
    by its faders and filters.


$~To adjust how the volume, low pass, and spread is affected over the distance
from the listener, scroll down in the properties list below the *Bank* tab
with the desired 3D bus selected.