# TANKBALL

~~~
CREDITS & COPYING:
   - joystix monospace.ttf
     REF: https://www.fontspring.com/fonts/typodermic/joystix

   - Box2D
     REF: https://github.com/erincatto/box2d/blob/master/LICENSE

   - SDL2
     REF: https://www.libsdl.org/license.php

   - SDL2_gfx
     REF: https://www.ferzkopp.net/Software/SDL2_gfx/Docs/html/index.html

   - SDL2_ttf
     REF: https://hg.libsdl.org/SDL_ttf/file/b4347abd4fbc/COPYING.txt

   - bluez
     REF: http://www.bluez.org/download/

   - Arduino-Makefile
     REF: https://github.com/sudar/Arduino-Makefile
~~~

### GAME DEPS COMPILER
~~~
   - gcc / c++
   - Box2D
   - SDL2
   - SDL2_gfx
   - SDL2_ttf
~~~

### DRIVER COMPILER
[source](/hardware/linux/src/sppinput/)
~~~
   - gcc
   - golang
   - bluez-dev
~~~

### FIRMWARE GAMEPAD
[source](/hardware/arduino/joystick/)
~~~
   - Only Native Arduino
   - Makefile
   - Arduino-Makefile
~~~

### HARDWARE
~~~
   - ARDUINO       / one | leonardo | ...
   - HC-05         / Bluetooth
   - HC-SR04       / Ultra sonic
   - MPU6050       / IMU (Inertial Measurment Units)
   - LED RGB
   - PUSH BUTTONS
~~~
  
~~~
$ source ./gamecontrollerdb.sh
~~~

### HARDWARE JOYSTICK

![hardware](hardware.jpeg)

### DRIVER JOYSTICK

![driver](driver.png)
