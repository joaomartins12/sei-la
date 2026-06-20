!page(example_studio_adv_sidechain, Advanced Side Chain)

_SDK Location: examples/studio/adv_sidechain_

This demonstrates using the signal strength of a bus to drive
a parameter on another bus, in this case the side chain input
for a compressor to automatically duck.

To recreate:

$* Create a project with an event to play dialog and one to play
    music to duck.
$* Create a music bus and assign the music event to it, and a 
    dialog bus and assign the dialog event to it.
$* Right click on the dialog bus and select "Create Bus Controller".
    This creates a controller whose value is the signal strength
    on the given bus.
$* Add a "Compress" filter to the Music bus. Set the "Release" to
    350 ms, the "Threshold" to -40db, the "Ratio" to 80, the "Hold"
    to 375 ms, the "Max Gain Reduction" to -20 db, the "Make-up Gain" to
    2 dB, the "Side Chain Enable" to 1, and bind the "Side Chain 
    Input" to the bus controller created earlier.
$* Audition the music event, and use ctrl-space to audition the
    dialog event. 

