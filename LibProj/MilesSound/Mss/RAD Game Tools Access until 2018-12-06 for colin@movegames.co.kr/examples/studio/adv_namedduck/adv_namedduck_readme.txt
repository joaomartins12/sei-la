!page(example_studio_adv_namedduck, Advanced Named Ducks)

_SDK Location: examples/studio/adv_namedduck_

This demonstrates automatic ducking for events.

To recreate:

$* Create a project with an event to play dialog and one to play
    music to duck.
$* Select the *Project* tab.
$* Create a bus and assign the music event to it.
$* Right click on the mixer snapshots pane and create a Snapshot
    Pack and name it "MyDuck"
$* Right click on the new snapshot group and create a Snapshot,
    and name it "Duck" - this must be the name in order to work.
$* Select the "Duck" snapshot in the list. The back of the bus
    design space will say "Editing Snapshot". 
$* Move the "Input Gain" fader in the music bus to -10 dB.
$* Select the "Baseline Mixer State" in the snapshots listbox.
$* Return to the "Bank" tab and select the dialog event. In the
    properties, set the Named Duck to "MyDuck"
$* Audition the music event, and use ctrl-space to audition the
    dialog event. 


~~~
    Selecting a snapshot in the Project tab will cause Miles
    Studio to use that snapshot when auditioning. If "Duck" were
    still selected, the duck would have no apparent effect as
    it would already be applied!
~~~

~~~
    In order for a named duck to work, the snapshot in the snapshot
    pack MUST be named "Duck".
~~~

~~~
    Ducks can affect anything a snapshot can, not just volumes!
~~~
