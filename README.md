# Couvabot

#### What it is:

Couvabot is a robot that was built by 3 students (Clément Nussbaumer, Ludovic Giezendanner and Romain Emery) to compete in a  [small contest](http://robopoly.epfl.ch/files/content/sites/robopoly/files/evenements/GC_2015-2016/2015-2016%20-%20Grand%20Concours%20Robopoly.pdf) in our University [EPFL](http://epfl.ch), in Switzerland.

It was sadly never able to participate, as we lost our two motors to an overheating problem the day before the contest, which prevented us to complete the necessarily tests and to correctly test the code.

However it was a great opportunity to learn the basics of robot building, power controlling, project managing, team working, robot controlling, micro controller programming, not-much-sleeping, and more :)

#### What the robot does :

It is designed to load cylinders (r = 30mm, h = 40mm) in two storage containers at its back, using semicircular loading ramps.
This is best explained with pictures:

![alt text][modele_global-view]
![alt text][modele_rear-view]

[modele_global-view]: https://drive.google.com/uc?id=0Bysw2qjh_V3-TS1nTVk5cjFPTzg "Global view of the 3d model"
[modele_rear-view]: https://drive.google.com/uc?id=0Bysw2qjh_V3-djhHZVhfMWFYYUk "Rear view of the 3d model"

It then brings the stacked cylinders (2 maximum per container) back to its "home".

### How the robot works :

There are two microcontrollers on the robot : a Raspberry Pi 2 and an Arduino Mega.

#### The Raspberry

It runs two threads simultaneously :
1. The first thread keeps track of the remaining time, of the cylinder(s) that the robot already carries, and it basically takes the decisions for all of the robots movements (displacement of the robot, catching of the cylinders when they are close enough, choice of the first or second loading lane, opening of the containers when "at home", ...) given information from the second thread and from the Arduino: it performs a Braitenberg obstacle avoidance based on IR sensors connected to the Arduino
2. The second thread does a visual scanning of its environment using a USB webcam: it converts the colors to the HSV color space and detects the possible cylinder candidates.

#### The Arduino

It is responsible of all electronic components onboard.
It gets its commands from the Raspberry, and gives some feedback using a SPI communication channel.
The Arduino controls :
* the wheels motors (through a PWM brushless DC motor driver)
* the 5 servo-motors
* the conveyor belt motor (through an other driver)
* also gets distance data from 8 IR sensors around the robot and sends them to the RPi upon request.

### The SPI communication

In order to exchange data with an interesting throughput between the two micro controllers, we chose the [SPI protocol](https://en.wikipedia.org/wiki/Serial_Peripheral_Interface_Bus). Using this we were able to achieve a ~2 Mhz rate between the two controllers. We have used this [extremely useful article](http://robotics.hobbizine.com/raspiduino.html) about SPI communication between an Arduino and a Raspberry to get started.
We then had to design a (simple but effective) communication protocol between the two micro controllers.

### How is the code organized ?

There are a lot of folders under `src`, of which only two are actually used for the robot : `Arduino_couvabot` and `Raspberry_couvabot`. The other folders contain libraries or tools used to compile the Arduino code using cmake:
* `RPi/bcm2835-1.49` contains the code of an excellent C library ([bcm2835](http://www.airspayce.com/mikem/bcm2835/)) that we use to command the GPIO ports of the Raspberry and to sort the technical aspects of the SPI communication out.
* `cmake` contains the files used to compile and upload bootcodes to the Arduino without using the Arduino IDE. These files are from this (really useful!) project : [Arduino Cmake](https://github.com/queezythegreat/arduino-cmake)
* the other folders are test folders that should be removed.

If I were to do this again, I would use the "submodule" functionality a lot more, and I would avoid so many subfolders. Sorry for the lack of clarity of these hierarchy.

### Who did what ?

#### Clément
* wrote the Raspberry Pi program
* builded the microcontrollers support
* developed the SPI protocol

#### Ludovic
* wrote the Arduino program
* did the wiring of the robot
* developed the SPI protocol

#### Romain
* builded the robot base, as well as the metallic structure.
* wrote the Braitenberg obstacle avoidance code

This is a coarse repartition of the tasks, as we in fact worked together all along the project, but it globally reflects the tasks assignment.

### What now ?

The project would need a cleanup (mainly the `main.c` code located in `Raspberry_couvabot/src`, as it was written "in a hurry"), and we should document the various parts of the project

##### Where was  it built ?

As we hadn't a better place for it, we built it in a (big) kitchen that was converted (for a month) to an electronic workshop!

This overall picture tells it all :)

![alt text][robot_global-view]

[robot_global-view]: https://docs.google.com/uc?id=0Bysw2qjh_V3-V2wyQi03X0ljS0k "Global view of the robot (over the hob!)"
