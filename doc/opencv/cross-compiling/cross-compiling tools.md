## Raspberry Pi cross-compiling tools

To install a cross-compiled version of opencv, you first need to cross-compile it !

[This link](http://stackoverflow.com/questions/19162072/installing-raspberry-pi-cross-compiler) describes how to Cross-Compile for the Pi
However, we don't want to use the outdated cross-compiler tools from the Raspberry git, instead we use the gnueabihf for linux tool.
Install it using :

```  sudo apt-get install gcc-4.9-arm-linux-gnueabihf g++-4.9-arm-linux-gnueabihf ```

Here we install the 4.9 version, because on the Pi the gcc version installed (in Feb 16) is 4.9.2. If you install a newer version, you may not be able to run compiled files on the Pi.

You can then check wether the install process succeeded using this command :

```   arm-linux-gnueabihf-gcc-4.9 -v ```

You do not only need to have the right cross compilation tools, you also need to download the libraries from the Pi
I suggest you to make a "raspberry" folder in your home folder, as well as a "rootfs" folder in it:

```  mkdir -p ~/raspberry/rootfs ```

Then using rsync you download the {usr,lib} folders from the RPi on in the rootfs folder:

```  rsync -rl --delete-after --safe-links pi@$PI:/{lib,usr} $HOME/raspberry/rootfs ```

When this is done we need to create a cmake config file. Let's call it ```pi.cmake``` and let's put it in the ~/raspberry/ folder:

```  nano ~/raspberry/pi.cmake ```

The content of this config file is the following (don't forget to modifiy CC tools version numbers accordingly)

```
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 1)
SET(CMAKE_C_COMPILER /usr/bin/arm-linux-gnueabihf-gcc-4.9)
SET(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabihf-g++-4.9)
SET(CMAKE_FIND_ROOT_PATH $ENV{HOME}/raspberry/rootfs)
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
```

Now the cross-compiler is set up, you can try it by (cross-)compiling a HelloWorld sample :

```
  git clone https://github.com/jameskbride/cmake-hello-world.git
  cd cmake-hello-world
  mkdir build
  cd build
  cmake -D CMAKE_TOOLCHAIN_FILE=$HOME/raspberrypi/pi.cmake ../
  make
  scp CMakeHelloWorld pi@192.168.1.PI:/home/pi/
  ssh pi@192.168.1.PI ./CMakeHelloWorld
```