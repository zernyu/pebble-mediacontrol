pebble-mediacontrol
===================

Control your computer's music through your Pebble watch!

This is a very rough work in progress to give Pebble wearers the lazy superpower of controlling the media playing on their computers right from their wrists!

Right now, there is only Windows support and I'm not even sure if the bundled .exe even works on computers without a Ruby dev environment installed. But if it works for you, simply build and install the watch app and then enter in your computer's IP address and the port the server is listening on (hardcoded as 8000 as of right now :P) and you should be good to go!

The ruby script uses the ffi gem to send virtual-key codes to Windows.

I plan on adding OS X support in the future
