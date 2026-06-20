!page(example_studio_basic_routing, Basic Routing)

_SDK Location: examples/studio/basic_routing_

This demonstrates how to set up custom routing for a source in an
event.

To recreate with your own sound:

$* Create a new project in Miles Studio
$* Select the *Bank* tab.
$* Drag a .wav file from your computer to the *Sources* list.
    A new source will be created under the bank *NewBank*.    
$* Right click on the new source, and select *Create Event To Play*.
$* Select the new event, and change the "Primary Output Type"
    to "Routed".
$* Set the output and input channel count to the desired routing
    matrix to set, and add any channel mappings that are desired.

Routing is reasonably complex because the matrix that is used
is inherently specific to the source's channel counts, and the
output's channel count. If both are known with certainty - for
instance if the only source in the event is mono, and it routes
to a bus that has been set to 5.1 - then only one matrix is 
needed. If the event outputs to a bus that uses the "Default"
channel count, then the channel count is determined at game
load, and more than one matrix may be needed.

If a matrix that matches the output channel count isn't found,
then another matrix that matches the source's channel count is used,
with the corresponding speakers mapped as best as possible. If
there is no matrix that matches the source channel count, then
the routing fails and an error is logged at run time.
