!page(example_studio_basic_controller, Basic Controller)

_SDK Location: examples/studio/basic_controller_

This demonstrates a very basic project that uses a game driven
value to ramp the volume of a sound up over 2 seconds.

To recreate with your own sound:

$* Create a new project in Miles Studio
$* Select the *Bank* tab.
$* Drag a .wav file from your computer to the *Sources* list.
    A new source will be created under the bank *NewBank*.    
$* Right click on the new source, and select *Create Event To Play*.
$* Select the new event in the *Events* pane on the left.
$* Right click on the *Input Gain* fader and select *Attach Controller*.
$* Press *New* and accept the default name.

Upon completion, the Input Gain will be set to the decibel value
supplied by the game continuously. The supplied example code
ramps the controller *NewController* from -96 to 0 over 2 seconds.
