!page(example_studio_adv_buschain, Advanced Buschain)

_SDK Location: examples/studio/adv_buschain_

This demonstrates routing a sound through several buses chained
together, each affecting the pitch of the source event.

To recreate with your own sound:

$* Create a project with an event to play a sound.
$* Select the *Design* tab.
$* Create several buses.
$* Drag from the output nib on a bus to connect it to another bus.
$* Create a controller and attach it to the pitch for each bus.
$* Right click on the bound pitch fader to add a controller transform.
$* Adjust the transform to the desired effect.
$* Assign the event to the first bus in the chain.
$* Audition the event and drag the controller preview value to 
    affect the resulting pitch.


~~~
    Pitch on a bus does not affect events that reach the bus via Send.
    That is to say, only pitch accumulated from the Primary Output
    will affect the event.
~~~

~~~
    To select a controller for previewing its value, right click on 
    any fader it is bound to and click "Select Controller"
~~~