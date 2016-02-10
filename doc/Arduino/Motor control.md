##Controlling motors with an arduino board
Motor control is possible with an external [Dual DC motor driver](http://www.robotshop.com/en/pololu-dual-dc-motor-driver-1a-4-5v-3-5v-tb6612fng.html).
A simple source code and tutorial can be found [here](http://bildr.org/2012/04/tb6612fng-arduino/).

Be careful to wire your controller correctly, respect the nominal voltage of the motors and declare your correct.
A memory-optimisation possibility in the example source code is to declare the pins as constants, not as variables.
The stop() function must be handled with care, since it only puts the motors on standby; that means that when you use
the move() function to set a new speed, the previous speed of the second motor will be reactivated after shutting down
the standby mode...
This problem has been solved in motor.ino in the src folder.