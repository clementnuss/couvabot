#Multi-tasking with Arduino
A rather simple way to do multi-tasking on a one-core processor like
 arduino is very well described on [this](https://learn.adafruit.com/multi-tasking-the-arduino-part-1/overview) site.
 Part two of the tutorial is located [here](https://learn.adafruit.com/multi-tasking-the-arduino-part-2/overview).
 
 Basically the most important thing to know is that the delay() function is to avoid, since it takes the total occupancy
  of the processor (in the second tutorial we see that there is a way to use it...).
  
 The usage of classes (in C++) is recommended, it allows us to create specific objects for every different modules 
 used with the arduino. It simplifies a lot the code, and once set, is easy to use.
 
 The concept of multi-tasking is that instruction won't be executed one after another anymore, but at a certain time, using
 the millis() function. Like if you checked your watch to do something.
 The advantage is that until that time hasn't come, you can do anything else.
 
 Another interesting possibility is the usage of Interrupts (described in tutorial two). 
 It allows us to execute an instruction at any time in the code, with complete priority, in response to an interupt 
 call. The interrupt function has to be declared at the beginning. 
 Some tips to know for the correct usage are given on the arduino [site](https://www.arduino.cc/en/Reference/AttachInterrupt).