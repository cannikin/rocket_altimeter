# Rocket Altimeter

This Arduino sketch uses a LightBlue Bean, Bosch BMP085 barometer and a couple of switches
to record and display maximum measured altitude. It was originally designed to be strapped
to a rocket to record how it reached.

Button A sets the unit to altitude recording mode. Button B exits recording mode and 
displays the maximum altitude reached with a series of blinks: first the hundreds position,
then tens, the ones. For example:

    [blink] ... [blink] [blink] ... [blink][blink][blink]

Reads 123 feet. There is a slight delay (750ms) between each digit position.
